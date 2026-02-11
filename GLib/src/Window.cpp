#include "Window.h"
#include "Input.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace GLib {

    static int s_GLFWWindowCount = 0;

    Window::Window(int width, int height, std::string title) 
        : m_Width(width), m_Height(height), m_Title(title)
    {
        InitGLFW();
        InitOpenGL();
    }
    
    Window::~Window() {
        glfwDestroyWindow(m_Window);
        s_GLFWWindowCount--;

        if (s_GLFWWindowCount == 0){
            glfwTerminate();
        }
    }

    void Window::Update() {
        Input::Update();
        if (glfwWindowShouldClose(m_Window)) {
            exit(0);
        }
        glfwSwapBuffers(m_Window);
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
        
        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), NULL, NULL);
        if (m_Window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(1);
        }
        glfwMakeContextCurrent(m_Window);

        Input::Init(m_Window);

        SetVSync(true);

        glfwSetWindowUserPointer(m_Window, this);
        glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
        glfwSetScrollCallback(m_Window, ScrollCallback);
    }

    void Window::InitOpenGL() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            exit(1);
        }  

        glViewport(0, 0, m_Width, m_Height);
    }

    void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        win->m_Width = width;
        win->m_Height = height;
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
            VSyncEnabled = enabled;
    }

    void Window::HideCursor() {
        if (glfwRawMouseMotionSupported()) // turn off raw mouse motion
            glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    void Window::DisableCursor() {
        if (glfwRawMouseMotionSupported()) // turn on raw mouse motion
            glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Window::ShowCursor() {
        if (glfwRawMouseMotionSupported()) // turn off raw mouse motion
            glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Window::ConfineCursor() {
        if (glfwRawMouseMotionSupported()) // turn off raw mouse motion
            glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);

        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
    }

}
