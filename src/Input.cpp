#include <Input.h>

#include <GLFW/glfw3.h>

GLFWwindow* Input::m_Window = nullptr;

void Input::Init(GLFWwindow* window) {
		m_Window = window;
}

bool Input::IsKeyPressed(int key)
{
		return glfwGetKey(m_Window, key) == GLFW_PRESS;
}

bool Input::IsMouseButtonPressed(int button)
{
		return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
}

glm::vec2 Input::GetMousePosition()
{
		double x, y;
		glfwGetCursorPos(m_Window, &x, &y);
		return glm::vec2((float)x, (float)y);
}

float Input::GetMouseX()
{
		double x, y;
		glfwGetCursorPos(m_Window, &x, &y);
		return (float)x;
}

float Input::GetMouseY()
{
		double x, y;
		glfwGetCursorPos(m_Window, &x, &y);
		return (float)y;
}
