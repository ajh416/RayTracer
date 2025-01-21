#include "Camera.h"

#include "Input.h"
#include <GLFW/glfw3.h>

#include <OpenGL/Window.h>

void Camera::Update() {
		if (Input::IsKeyPressed(GLFW_KEY_W)) {
				m_Origin += Vec3f(0.0f, 0.0f, 0.1f);
				RecalculateRayDirections();
				printf("W\n");
				
		}
		if (Input::IsKeyPressed(GLFW_KEY_S)) {
				m_Origin += Vec3f(0.0f, 0.0f, -0.1f);
				RecalculateRayDirections();
				printf("S\n");
		}
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

Vec3f Camera::GetRayDirection(Vector2<float>&& coord) {
	return m_RayDirections[(int)coord.x + (int)coord.y * m_Width];
}

Vector3<float> Camera::CalculateRayDirection(Vector2<float>&& coord) {
	return m_LowerLeftCorner + coord.x * m_Horizontal + coord.y * m_Vertical - m_Origin;
}
