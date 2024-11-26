#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
public:
        Window(int width, int height, const char *title);
        ~Window();
        void Update();
        bool ShouldClose();
	void BeginImGui();
	void EndImGui();
        GLFWwindow *GetWindow();
        int GetWidth();
        int GetHeight();

private:
	GLFWwindow *window;
	int width;
	int height;
};
