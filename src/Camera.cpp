#include "Camera.h"

void Camera::Update() {
	while (m_Origin.x < 1.0f)
		m_Origin.x += 0.01f;
}
