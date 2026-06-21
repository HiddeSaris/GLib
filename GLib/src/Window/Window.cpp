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
    int Window::s_Width = 0;
    int Window::s_Height = 0;
    int Window::s_PosX = 0;
    int Window::s_PosY = 0;
    int Window::s_ViewportWidth = 0;
    int Window::s_ViewportHeight = 0;
    int Window::s_WindowedWidth = 0;
    int Window::s_WindowedHeight = 0;
    int Window::s_WindowedPosX = 0;
    int Window::s_WindowedPosY = 0;
    std::string Window::s_Title = "";
    GLFWwindow* Window::s_Window = nullptr;

    void Window::Init(int width, int height, std::string title, bool start_fullscreen)
    {
        s_Width = width;
        s_Height = height;
        s_Title = title;

        InitGLFW(start_fullscreen);
        InitOpenGL();

        InitImGui();
    }

    void Window::Close()
    {
        glfwDestroyWindow(s_Window);
        s_GLFWWindowCount--;

        if (s_GLFWWindowCount == 0){
            glfwTerminate();

            ImGuiEndFrame();
            DestructImGui();
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

    void Window::CreateWindow(bool fullscreen)
    {
        GLFWmonitor* monitor = NULL;
        int width = s_Width;
        int height = s_Height;

        if (fullscreen) {
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

    void Window::InitGLFW(bool start_fullscreen)
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
        
        CreateWindow(start_fullscreen);
        
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
        
        Render::Init();
    }

    void Window::InitImGui()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        // ImGuiViewport* viewport = ImGui::GetWindowViewport();
        // viewport->Flags |= ImGuiViewportFlags_TopMost;
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(s_Window, true);
        ImGui_ImplOpenGL3_Init();
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
        Render::OnViewportResize(width, height);
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

    void Window::ImGuiNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Window::ImGuiEndFrame()
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)s_Width, (float)s_Height);

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    bool Window::IsFullscreen()
    {
        return glfwGetWindowMonitor(s_Window) != nullptr;
    }
}
