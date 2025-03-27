#pragma once

#include "RayTracer.h"

#include <vector>

class Camera
{
public:
	Camera() = default;

	Camera(int image_width, float aspect_ratio, glm::vec3 origin = { 0, 0, 0 });

	bool Update();

	void RecalculateRayDirections();

	glm::vec3 GetRayDirection(glm::vec2&& coord);

	void CalculateViewMatrix();
	void CalculateProjectionMatrix();

	glm::vec3 CalculateRayDirection(glm::vec2&& coord);

	glm::mat4 GetInverseView() const { return m_InverseView; }
	glm::mat4 GetInverseProjection() const { return m_InverseProjection; }
	glm::mat4 GetViewProjection() const { return glm::inverse(m_InverseProjection) * glm::inverse(m_InverseView); }
	glm::vec3 GetPosition() const { return m_Position; }
private:
	int m_Width;
	int m_Height;
	float m_AspectRatio = 16.0f / 9.0f;

	std::vector<glm::vec3> m_RayDirections;
	glm::vec2 m_LastMousePosition = { 0, 0 };

	glm::vec3 m_ForwardDirection = { 0, 0, 0 };
	glm::mat4 m_InverseView = { 1.0f };
	glm::mat4 m_InverseProjection = { 1.0f };

	glm::vec3 m_Position = { 0.0, 0.0, 0.0 };
};
