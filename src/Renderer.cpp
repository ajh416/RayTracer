#include "Renderer.h"

#include "Ray.h"
#include "Vector.h"

#include <string.h>
#include <array>
#include <future>
#include <algorithm>
#include <execution>

void Renderer::Render(const Scene &scene, Camera &cam) {
	if (m_Camera == nullptr || m_Scene == nullptr) {
		m_Camera = &cam;
		m_Scene = &scene;
	}

	if (m_FrameIndex == 1) {
		memset(m_AccumulationData, 0, m_Image->Width * m_Image->Height * sizeof(glm::vec3));
	}
	
#define MT 1

#if MT
#if RT_WINDOWS
#include <execution>

        // A good bit faster than using my 8 thread version below
        // This doesn't work on linux apparently
        // something something libtbb
        std::for_each(std::execution::par_unseq, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(), [this](uint32_t y) {
                std::for_each(std::execution::par_unseq, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
                              [this, y](uint32_t x) {
                                auto color = PerPixel({ (float)x, (float)y });
								m_AccumulationData[x + y * this->m_Image->Width] += color;
								auto accumulated_color = m_AccumulationData[x + y * this->m_Image->Width];
								accumulated_color /= (float)m_FrameIndex;
                                accumulated_color = glm::clamp(accumulated_color, glm::vec3(0.0f), glm::vec3(1.0f));
                                m_Image->Data[x + y * this->m_Image->Width] = Utils::VectorToUInt32(accumulated_color);
                              });
        });

#else // RT_WINDOWS
#define NUM_THREADS 8

        struct ImgBlock {
                uint32_t start_w, start_h, end_w, end_h;
        };
        std::vector<ImgBlock> blocks;

        /*
         * Following section from
         * https://superkogito.github.io/blog/2020/10/01/divide_image_using_opencv.html
         * Adapted by ajh416 for use without opencv
         */

        // init image dimensions
        uint32_t imgWidth = this->m_Image->Width;
        uint32_t imgHeight = this->m_Image->Height;

        // std::cout << "IMAGE SIZE: "
        //           << "(" << imgWidth << "," << imgHeight << ")" << std::endl;

        // init block dimensions
        uint32_t bwSize;
        uint32_t bhSize;

        uint32_t blockWidth = imgWidth / (NUM_THREADS / 2);
        uint32_t blockHeight = imgHeight / (2);

        uint32_t y0 = 0;
        while (y0 < imgHeight) {
                // compute the block height
                bhSize = ((y0 + blockHeight) > imgHeight) * (blockHeight - (y0 + blockHeight - imgHeight)) +
                         ((y0 + blockHeight) <= imgHeight) * blockHeight;
                int x0 = 0;
                while (x0 < imgWidth) {
                        // compute the block height
                        bwSize = ((x0 + blockWidth) > imgWidth) * (blockWidth - (x0 + blockWidth - imgWidth)) +
                                 ((x0 + blockWidth) <= imgWidth) * blockWidth;

                        // crop block
                        blocks.push_back(ImgBlock(x0, y0, x0 + bwSize, y0 + bhSize));

                        // update x-coordinate
                        x0 = x0 + blockWidth;
                }

                // update y-coordinate
                y0 = y0 + blockHeight;
        }

        std::array<std::future<void>, NUM_THREADS> threads;
        for (int i = 0; i < NUM_THREADS; i++) {
                threads[i] = std::async(
                    std::launch::async,
                    [&](int i) {
                            uint32_t start_w = blocks[i].start_w, start_h = blocks[i].start_h, end_w = blocks[i].end_w,
                                     end_h = blocks[i].end_h;
                            // printf("thread %i start: (%u, %u), end: "
                            //        "(%u, %u)\n",
                            //        i, start_w, start_h, end_w, end_h);

                            for (int y = start_h; y < end_h; y++) {
                                    for (int x = start_w; x < end_w; x++) {
                                            auto color = PerPixel({ (float)x, (float)y });
                                            m_AccumulationData[x + y * this->m_Image->Width] += color;
                                            auto accumulated_color = m_AccumulationData[x + y * this->m_Image->Width];
                                            accumulated_color /= (float)m_FrameIndex;
                                            accumulated_color = glm::clamp(accumulated_color, glm::vec3(0.0f), glm::vec3(1.0f));
                                            m_Image->Data[x + y * this->m_Image->Width] = Utils::VectorToUInt32(accumulated_color);
                                    }
                            }
                            // printf("thread %i finished\n", i);
                    },
                    i);
        }

        // Wait while the threads complete. TODO: IS THIS NECESSARY?
        for (int i = 0; i < NUM_THREADS; i++)
                while (!threads[i].valid()) {
                }

#endif // RT_WINDOWS

#else // MT

        for (int y = 0; y < (int)m_Image->Height; y++) {
                for (int x = 0; x < (int)m_Image->Width; x++) {
                        for (int i = 0; i < m_Settings.AccumulateMax; i++) {
                                auto color = PerPixel({(float)x, (float)y});
                                m_AccumulationData[x + y * m_Image->Width] += color;
                        }
                        auto accumulated_color = m_AccumulationData[x + y * m_Image->Width];
                        accumulated_color /= (float)m_Settings.AccumulateMax;

                        m_Image->Data[x + y * m_Image->Width] = Utils::VectorToUInt32(accumulated_color);
                }
        }

#endif // MT
	if (m_Settings.Accumulate)
		m_FrameIndex++;
    else
		m_FrameIndex = 1;
}

void Renderer::SetImage(Image &image) {
        m_Image = &image;
        delete[] m_AccumulationData;
        m_AccumulationData = new glm::vec3[m_Image->Width * m_Image->Height];

        m_ImageVerticalIter.resize(m_Image->Height);
        m_ImageHorizontalIter.resize(m_Image->Width);

        for (uint32_t x = 0; x < m_Image->Height; x++)
                m_ImageVerticalIter[x] = x;

        for (uint32_t y = 0; y < m_Image->Width; y++)
                m_ImageHorizontalIter[y] = y;
}

glm::vec3 Renderer::PerPixel(const glm::vec2 &&coord) {
        glm::vec3 light_res = glm::vec3(0.0f);
        glm::vec3 res = glm::vec3(0.0f);

        for (int i = 0; i < m_Settings.NumberOfSamples; i++) {
                glm::vec3 bounce_res = glm::vec3(0.0f);
                glm::vec3 ray_color = glm::vec3(1.0f);

                Ray r = Ray(m_Camera->GetPosition(), m_Camera->GetRayDirection({coord.x, coord.y}));

                for (int i = 0; i < m_Settings.NumberOfBounces + 1; i++) {
                        auto payload = TraceRay(r);
                        if (payload.HitDistance < 0) // did not hit object
                                break;

                        const Object *Object = m_Scene->Objects[payload.ObjectIndex];
                        const Material &material = m_Scene->Materials[Object->MaterialIndex];

                        glm::vec3 emitted_light = material.EmissionColor * material.EmissionStrength;
                        bounce_res += emitted_light * ray_color;
                        ray_color = ray_color * material.Albedo;

                        r.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
                        r.Direction =
                            glm::reflect(r.Direction, payload.WorldNormal + material.Roughness * Utils::RandomVector(-0.1f, 0.1f));
                }

                res += glm::clamp(bounce_res, glm::vec3(0.0f), glm::vec3(1.0f));
        }

        return res / (float)(m_Settings.NumberOfSamples);
}

HitPayload Renderer::TraceRay(const Ray &ray) {
        int objectIndex = -1;
        float hitDistance = std::numeric_limits<float>::max();

        for (int i = 0; i < m_Scene->Objects.size(); i++) {
                const Object *Object = m_Scene->Objects[i];
                float newDistance = 0;

                if (Object->Hit(ray, 0, hitDistance, newDistance)) {
                        if (newDistance > 0.0 && newDistance < hitDistance) {
                                hitDistance = newDistance;
                                objectIndex = i;
                        }
                }
        }

        if (objectIndex < 0)
                return Miss(ray);

        return ClosestHit(ray, hitDistance, objectIndex);
}

HitPayload Renderer::ClosestHit(const Ray &ray, float hitDistance, int objectIndex) {
        HitPayload payload;
        payload.HitDistance = hitDistance;
        payload.ObjectIndex = objectIndex;

        const Object *closestObject = m_Scene->Objects[objectIndex];

        glm::vec3 origin = ray.Origin - closestObject->Origin;
        payload.WorldPosition = origin + hitDistance * ray.Direction;
        payload.WorldNormal = glm::normalize(payload.WorldPosition);
        payload.WorldPosition += closestObject->Origin;

        return payload;
}

constexpr HitPayload Renderer::Miss(const Ray &ray) {
        constexpr HitPayload payload = {.HitDistance = -1};
        return payload;
}
