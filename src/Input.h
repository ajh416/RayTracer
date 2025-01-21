#pragma once

struct GLFWwindow;

class Input {
public:
		static void Init(GLFWwindow* window);

		static bool IsKeyPressed(int key);
		static bool IsMouseButtonPressed(int button);
		static float GetMouseX();
		static float GetMouseY();
private:
		static GLFWwindow* m_Window;
};