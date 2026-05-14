#include "Window.h"
#include "Input.h"
#include "Renderer/Render.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace GLib {

    static int s_GLFWWindowCount = 0;

    bool Window::s_VSyncEnabled = false;
    int Window::s_Width = 0;
    int Window::s_Height = 0;
    std::string Window::s_Title = "";
    GLFWwindow* Window::s_Window = nullptr;

    void Window::Init(int width, int height, std::string title)
    {
        s_Width = width;
        s_Height = height;
        s_Title = title;
        InitGLFW();
        InitOpenGL();
    }

    void Window::Close()
    {
        glfwDestroyWindow(s_Window);
        s_GLFWWindowCount--;

        if (s_GLFWWindowCount == 0){
            glfwTerminate();
        }
    }

    void Window::Update()
    {
        Input::Update();
        if (glfwWindowShouldClose(s_Window)) {
            exit(0);
        }
        glfwSwapBuffers(s_Window);
    }

    void Window::InitGLFW() {
        if (s_GLFWWindowCount == 0) {
            s_GLFWWindowCount++;
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
        else {
            std::cout << "Multiple windows is not supported" << std::endl;
            exit(1);
        }
        
        s_Window = glfwCreateWindow(s_Width, s_Height, s_Title.c_str(), NULL, NULL);
        if (s_Window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(1);
        }
        glfwMakeContextCurrent(s_Window);
        
        Input::Init(s_Window);

        SetVSync(true);

        glfwSetFramebufferSizeCallback(s_Window, FramebufferSizeCallback);
        glfwSetScrollCallback(s_Window, ScrollCallback);
    }

    void Window::InitOpenGL() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            exit(1);
        }  

        glViewport(0, 0, s_Width, s_Height);
        
        Render::Init();
    }

    void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        s_Width = width;
        s_Height = height;
        glViewport(0, 0, width, height);
    } 

    void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        Input::ScrollX = xoffset;
        Input::ScrollY = yoffset;
    }

    void Window::SetVSync(bool enabled) {
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
            s_VSyncEnabled = enabled;
    }

    void Window::HideCursor() {
        if (glfwRawMouseMotionSupported()) // turn off raw mouse motion
            glfwSetInputMode(s_Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

        glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void Window::DisableCursor() {
        if (glfwRawMouseMotionSupported()) // turn on raw mouse motion
            glfwSetInputMode(s_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Window::ShowCursor() {
        if (glfwRawMouseMotionSupported()) // turn off raw mouse motion
            glfwSetInputMode(s_Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

        glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Window::ConfineCursor() {
        if (glfwRawMouseMotionSupported()) // turn off raw mouse motion
            glfwSetInputMode(s_Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

        glfwSetInputMode(s_Window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
    }

    double Window::GetTime()
    {
        return glfwGetTime();
    }

}
