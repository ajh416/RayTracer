#include "Window.h"

#include <RayTracer.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Window::Window(int width, int height, const char *title) {
	this->width = width;
	this->height = height;
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create window" << std::endl;
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}
}

Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::Update() {
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Window::RenderImGui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Ray Tracer");
	ImGui::Text("Hello, world!");
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
