#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
        Window(int width, int height, const char *title);
        ~Window();
        void Update();
        bool ShouldClose();
        void SetKeyCallback(GLFWkeyfun callback);
        void SetMouseButtonCallback(GLFWmousebuttonfun callback);
        void SetCursorPosCallback(GLFWcursorposfun callback);
        void SetScrollCallback(GLFWscrollfun callback);
        GLFWwindow *GetWindow();
        int GetWidth();
        int GetHeight();

private:
	void RenderImGui();

private:
	GLFWwindow *window;
	int width;
	int height;
};
