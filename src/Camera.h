#pragma once

#include "RayTracer.h"
#include "Vector.h"

#include <vector>

class Camera
{
public:
	Camera() = default;

	Camera(int image_width, float aspect_ratio, Vec3f origin = { 0, 0, 0})
		: m_Width(image_width), m_Height(static_cast<int>(image_width / aspect_ratio)), m_AspectRatio(aspect_ratio)
	{
		m_ViewportWidth = m_AspectRatio * m_ViewportHeight;

		// make this negative to look at the positive Z rather than negative Z
		Vec3f focal_length = { 0.0, 0.0, 1.0 };

		m_Origin = origin;
		m_Horizontal = Vec3f(float(m_ViewportWidth), 0.0, 0.0);
		m_Vertical = Vec3f(0.0, float(m_ViewportHeight), 0.0);
		m_LowerLeftCorner = m_Origin - m_Horizontal / 2 - m_Vertical / 2 - focal_length;

		RecalculateRayDirections();
	}

	void Update();

	void RecalculateRayDirections();

	Vec3f GetRayDirection(Vector2<float>&& coord);

	Vector3<float> CalculateRayDirection(Vector2<float>&& coord);

	Vector3<float> GetOrigin() const { return m_Origin; }
	Vector3<float> GetHorizontal() const { return m_Horizontal; }
	Vector3<float> GetVertical() const { return m_Vertical; }
	Vector3<float> GetLowerLeftCorner() const { return m_LowerLeftCorner; }
private:
	int m_Width;
	int m_Height;
	float m_AspectRatio = 16.0f / 9.0f;

	std::vector<Vec3f> m_RayDirections;

	float m_ViewportHeight = 2.0f;
	float m_ViewportWidth;
	Vec3f m_Origin = { 0.0, 0.0, 0.0 };
	Vec3f m_Horizontal;
	Vec3f m_Vertical;
	Vec3f m_LowerLeftCorner;
};
