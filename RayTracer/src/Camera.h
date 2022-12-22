#pragma once

#include "RayTracer.h"
#include "Vector.h"

class Camera
{
public:
	Camera() = default;

	Camera(int image_width, float aspect_ratio)
		: m_Width(image_width), m_Height(static_cast<int>(image_width / aspect_ratio)), m_AspectRatio(aspect_ratio)
	{
		m_ViewportWidth = m_AspectRatio * m_ViewportHeight;

		// make this negative to look at the positive Z rather than negative Z
		Vec3f focal_length = { 0.0, 0.0, 1.0 };

		m_Origin = Vec3f(0, 0, 0);
		m_Horizontal = Vec3f(Float(m_ViewportWidth), 0.0, 0.0);
		m_Vertical = Vec3f(0.0, Float(m_ViewportHeight), 0.0);
		m_LowerLeftCorner = m_Origin - m_Horizontal / 2 - m_Vertical / 2 - focal_length;
	}

	Vector3<Float> CalculateRayDirection(Vector2<Float>&& coord) const {
		return m_LowerLeftCorner + coord.x * m_Horizontal + coord.y * m_Vertical - m_Origin;
	}

	Vector3<Float> GetOrigin() const { return m_Origin; }
	Vector3<Float> GetHorizontal() const { return m_Horizontal; }
	Vector3<Float> GetVertical() const { return m_Vertical; }
	Vector3<Float> GetLowerLeftCorner() const { return m_LowerLeftCorner; }
private:
	int m_Width;
	int m_Height;
	float m_AspectRatio = 16.0f / 9.0f;

	float m_ViewportHeight = 2.0f;
	float m_ViewportWidth;
	Vec3f m_Origin = { 0.0, 0.0, 0.0 };
	Vec3f m_Horizontal;
	Vec3f m_Vertical;
	Vec3f m_LowerLeftCorner;
};