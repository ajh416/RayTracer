#pragma once

#include "RayTracer.h"
#include "Vector.h"

#include <vector>

class Camera
{
public:
	Camera() = default;

	Camera(int image_width, float aspect_ratio, glm::vec3 origin = { 0, 0, 0 })
		: m_Width(image_width), m_Height(static_cast<int>(image_width / aspect_ratio)), m_AspectRatio(aspect_ratio)
	{
		m_ViewportWidth = m_AspectRatio * m_ViewportHeight;

		// make this negative to look at the positive Z rather than negative Z
		glm::vec3 focal_length = { 0.0, 0.0, 1.0 };

		m_Origin = origin;
		m_Horizontal = glm::vec3(float(m_ViewportWidth), 0.0, 0.0);
		m_Vertical = glm::vec3(0.0, float(m_ViewportHeight), 0.0);
		m_LowerLeftCorner = m_Origin - m_Horizontal.operator/=(2) - m_Vertical.operator/=(2) - focal_length;

		RecalculateRayDirections();
	}

	bool Update();

	void RecalculateRayDirections();

	glm::vec3 GetRayDirection(glm::vec2&& coord);

	glm::vec3 CalculateRayDirection(glm::vec2&& coord);

	glm::vec3 GetOrigin() const { return m_Origin; }
	glm::vec3 GetHorizontal() const { return m_Horizontal; }
	glm::vec3 GetVertical() const { return m_Vertical; }
	glm::vec3 GetLowerLeftCorner() const { return m_LowerLeftCorner; }
private:
	int m_Width;
	int m_Height;
	float m_AspectRatio = 16.0f / 9.0f;

	std::vector<glm::vec3> m_RayDirections;
	glm::vec2 m_MousePosition;

	float m_ViewportHeight = 2.0f;
	float m_ViewportWidth;
	glm::vec3 m_Origin = { 0.0, 0.0, 0.0 };
	glm::vec3 m_Horizontal;
	glm::vec3 m_Vertical;
	glm::vec3 m_LowerLeftCorner;
};
