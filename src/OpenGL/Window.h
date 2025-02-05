struct GLFWwindow;

class Window {
public:
        Window(int width, int height, const char *title);
        ~Window();
        void Update();
        bool ShouldClose();
	    void BeginImGui();
	    void EndImGui();
        GLFWwindow *GetWindow();
        int GetWidth() const;
        int GetHeight() const;

private:
	    GLFWwindow *window;
	    int width;
	    int height;
};
