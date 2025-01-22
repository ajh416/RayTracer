#include "Camera.h"

#include "Input.h"
#include <GLFW/glfw3.h>
#include <OpenGL/Window.h>

bool Camera::Update() {
		glm::vec2 mousePos = Input::GetMousePosition();
		glm::vec2 delta = (mousePos - m_MousePosition) * 0.002f;
		m_MousePosition = mousePos;

		bool moved = false;
		if (!Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
				return false;

		if (Input::IsKeyPressed(GLFW_KEY_W)) {
				m_Origin += glm::vec3(0.0f, 0.0f, 0.1f);
				moved = true;
				
		}
		if (Input::IsKeyPressed(GLFW_KEY_S)) {
				m_Origin += glm::vec3(0.0f, 0.0f, -0.1f);
				moved = true;
		}
		if (Input::IsKeyPressed(GLFW_KEY_A)) {
				m_Origin += glm::vec3(-0.1f, 0.0f, 0.0f);
				moved = true;
		}
		if (Input::IsKeyPressed(GLFW_KEY_D)) {
				m_Origin += glm::vec3(0.1f, 0.0f, 0.0f);
				moved = true;
		}

		return moved;
}

void Camera::RecalculateRayDirections() {
	m_RayDirections.resize(m_Width * m_Height);

	for (int y = 0; y < m_Height; y++) {
		for (int x = 0; x < m_Width; x++) {
			float u = float(x + Utils::Randomfloat()) / (m_Width - 1);
			float v = float(y + Utils::Randomfloat()) / (m_Height - 1);
			m_RayDirections[x + y * m_Width] = CalculateRayDirection({ u, v });
		}
	}
}

glm::vec3 Camera::GetRayDirection(glm::vec2&& coord) {
	return m_RayDirections[(int)coord.x + (int)coord.y * m_Width];
}

glm::vec3 Camera::CalculateRayDirection(glm::vec2&& coord) {
	return m_LowerLeftCorner + coord.x * m_Horizontal + coord.y * m_Vertical - m_Origin;
}
