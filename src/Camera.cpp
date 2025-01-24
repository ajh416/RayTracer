#include "Camera.h"

#include "Input.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>
#include <OpenGL/Window.h>

Camera::Camera(int image_width, float aspect_ratio, glm::vec3 origin)
		: m_Width(image_width), m_Height(static_cast<int>(image_width / aspect_ratio)), m_AspectRatio(aspect_ratio)
{
		m_Width = image_width;
		m_Height = static_cast<int>(image_width / aspect_ratio);
		m_AspectRatio = aspect_ratio;
		
		m_ForwardDirection = { 0.0f, 0.0f, -1.0f };
		m_Position = origin;
		m_LastMousePosition = Input::GetMousePosition();

		CalculateProjectionMatrix();
		CalculateViewMatrix();
		RecalculateRayDirections();
}

bool Camera::Update() {
		glm::vec2 mousePos = Input::GetMousePosition();
		glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
		m_LastMousePosition = mousePos;

		constexpr glm::vec3 up = { 0.0f, 1.0f, 0.0f };
		glm::vec3 right = glm::cross(m_ForwardDirection, up);

		bool moved = false;
		if (!Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
				return false;

		if (Input::IsKeyPressed(GLFW_KEY_W)) {
				m_Position += m_ForwardDirection;
				moved = true;
				
		}
		if (Input::IsKeyPressed(GLFW_KEY_S)) {
				m_Position -= m_ForwardDirection;
				moved = true;
		}
		if (Input::IsKeyPressed(GLFW_KEY_A)) {
				m_Position -= right;
				moved = true;
		}
		if (Input::IsKeyPressed(GLFW_KEY_D)) {
				m_Position += right;
				moved = true;
		}

		if (delta.x != 0.0f || delta.y != 0.0f)
		{
				float pitchDelta = delta.y * 0.3f;
				float yawDelta = delta.x * 0.3f;

				glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, right),
						glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));

				// requires glm experimental def set
				m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

				moved = true;
		}

		if (moved) {
				CalculateViewMatrix();
				CalculateProjectionMatrix();
				RecalculateRayDirections();
		}

		return moved;
}

void Camera::RecalculateRayDirections() {
	m_RayDirections.resize(m_Width * m_Height);

	for (int y = 0; y < m_Height; y++) {
		for (int x = 0; x < m_Width; x++) {
			//float u = float(x + Utils::Randomfloat()) / (m_Width - 1);
			//float v = float(y + Utils::Randomfloat()) / (m_Height - 1);
			m_RayDirections[x + y * m_Width] = CalculateRayDirection({ x, y });
		}
	}
}

glm::vec3 Camera::GetRayDirection(glm::vec2&& coord) {
	return m_RayDirections[(int)coord.x + (int)coord.y * m_Width];
}

void Camera::CalculateViewMatrix()
{
		const auto view = glm::perspectiveFov(glm::radians(45.0f), (float)m_Width, (float)m_Height, 0.1f, 100.0f);
		m_InverseProjection = glm::inverse(view);
}

void Camera::CalculateProjectionMatrix()
{
		const auto proj = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0, 1, 0));
		m_InverseView = glm::inverse(proj);
}

glm::vec3 Camera::CalculateRayDirection(glm::vec2&& coord) {
		coord = { coord.x / (float)m_Width, coord.y / (float)m_Height };
		coord = coord * 2.0f - 1.0f; // -1 -> 1

		glm::vec4 target = m_InverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
		glm::vec3 rayDirection = glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
		return rayDirection;
}
