#include "Window.h"
#include "Input.h"
#include "Renderer/Render.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace GLib {

    static int s_GLFWWindowCount = 0;

    bool Window::s_VSyncEnabled = false;
    int Window::s_Width = 1600;
    int Window::s_Height = 900;
    int Window::s_PosX = 0;
    int Window::s_PosY = 0;
    int Window::s_ViewportWidth = 0;
    int Window::s_ViewportHeight = 0;
    int Window::s_WindowedWidth = 0;
    int Window::s_WindowedHeight = 0;
    int Window::s_WindowedPosX = 0;
    int Window::s_WindowedPosY = 0;
    std::string Window::s_Title = "GLib Game Engine";
    GLFWwindow* Window::s_Window = nullptr;
    bool Window::s_Initialized = false;

    void Window::Init() {
        InitGLFW();
        InitOpenGL();

        s_Initialized = true;
    }

    void Window::Close()
    {
        glfwDestroyWindow(s_Window);
        s_GLFWWindowCount--;
        s_Initialized = false;
        
        if (s_GLFWWindowCount == 0){
            glfwTerminate();

            DestructImGui();
        }
    }

    void Window::Update()
    {
        if (!IsClosed())
            Input::Update();
        
        glfwSwapBuffers(s_Window);
    }

    void Window::ToggleFullscreen()
    {
        bool wasFullscreen = IsFullscreen();

        if (!wasFullscreen) {
            glfwGetWindowPos(s_Window, &s_WindowedPosX, &s_WindowedPosY);
            glfwGetWindowSize(s_Window, &s_WindowedWidth, &s_WindowedHeight);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            glfwSetWindowMonitor(s_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else {
            glfwSetWindowMonitor(s_Window, nullptr, s_WindowedPosX, s_WindowedPosY, s_WindowedWidth, s_WindowedHeight, 0);
        }

        glfwGetWindowPos(s_Window, &s_PosX, &s_PosY);
        glfwGetWindowSize(s_Window, &s_Width, &s_Height);
        glfwGetFramebufferSize(s_Window, &s_ViewportWidth, &s_ViewportHeight);
        glViewport(0, 0, s_ViewportWidth, s_ViewportHeight);
    }

    void Window::CreateWindow()
    {
        GLFWmonitor* monitor = NULL;
        int width = s_Width;
        int height = s_Height;

        if (false) {
            int mon_count = -1;
            GLFWmonitor** mon_list = glfwGetMonitors(&mon_count);

            if (mon_count <= 0){
                std::cout << "Error occured during window creation!" << std::endl;
                exit(1);
            }

            if (mon_count > 1){
                monitor = mon_list[1];
            }
            else {
                monitor = mon_list[0];
            }

            width = glfwGetVideoMode(monitor)->width;
            height = glfwGetVideoMode(monitor)->height;
        }

        s_Window = glfwCreateWindow(width, height, s_Title.c_str(), monitor, NULL);
        if (s_Window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(1);
        }
        glfwMakeContextCurrent(s_Window);

        s_WindowedWidth = s_Width;
        s_WindowedHeight = s_Height;
        s_WindowedPosX = s_PosX;
        s_WindowedPosY = s_PosY;

        glfwGetWindowSize(s_Window, &s_Width, &s_Height);
        glfwGetWindowPos(s_Window, &s_PosX, &s_PosY);
        glfwGetFramebufferSize(s_Window, &s_ViewportWidth, &s_ViewportHeight);
    }

    void Window::InitGLFW()
    {
        if (s_GLFWWindowCount == 0) {
            s_GLFWWindowCount++;
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_DEPTH_BITS, 24);
        }
        else {
            std::cout << "Multiple windows is not supported" << std::endl;
            exit(1);
        }
        
        CreateWindow();
        
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

        glViewport(0, 0, s_ViewportWidth, s_ViewportHeight);
    }

    void Window::DestructImGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        s_Width = width;
        s_Height = height;
        glViewport(0, 0, width, height);
        if (width != 0 && height != 0)
            Render::OnViewportResize(width, height);
    } 

    void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        Input::ScrollX = xoffset;
        Input::ScrollY = yoffset;
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

    bool Window::IsFullscreen()
    {
        return glfwGetWindowMonitor(s_Window) != nullptr;
    }

    bool Window::ShouldClose() {
        return glfwWindowShouldClose(s_Window) || IsClosed();
    }

    void Window::SetSize(int width, int height) {
        if (s_Initialized) {
            glfwSetWindowSize(s_Window, width, height);
        }
        s_Width = width;
        s_Height = height;
    }

    void Window::SetPos(int x, int y) {
        if (s_Initialized) {
            glfwSetWindowPos(s_Window, x, y);
        }
        s_PosX = x;
        s_PosY = y;
    }

    void Window::SetTitle(const std::string &title) {
        if (s_Initialized) {
            glfwSetWindowTitle(s_Window, title.c_str());
        }
        s_Title = title;
    }

    void Window::SetVSync(bool enabled) {
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
        
        s_VSyncEnabled = enabled;
    }

    void CheckGLError(const char* label) {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cout << "GL Error [" << label << "]: 0x" << std::hex << err << std::dec << "\n";
        }
    }
}
