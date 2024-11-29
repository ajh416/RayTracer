#include "Window.h"

#include <RayTracer.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Window::Window(int width, int height, const char *title) {
	this->width = width;
	this->height = height;
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(EXIT_FAILURE);
	}
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create window" << std::endl;
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (!status) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}

Window::~Window() {
	printf("Destroying window\n");
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::Update() {
	glfwPollEvents();
	glfwSwapBuffers(window);
}

bool Window::ShouldClose() {
	return glfwWindowShouldClose(window);
}

void Window::BeginImGui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::EndImGui() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

GLFWwindow* Window::GetWindow() {
	return window;
}

int Window::GetWidth() {
	return width;
}

int Window::GetHeight() {
	return height;
}

