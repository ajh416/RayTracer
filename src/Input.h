#pragma once

#include <RayTracer.h>

struct GLFWwindow;

class Input {
public:
		static void Init(GLFWwindow* window);

		static GLFWwindow* GetWindow() { return m_Window; }

		static bool IsKeyPressed(int key);
		static bool IsMouseButtonPressed(int button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
private:
		static GLFWwindow* m_Window;
};