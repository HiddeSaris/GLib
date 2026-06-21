#pragma once

#include <iostream>
#include <string>

struct GLFWwindow;

namespace GLib {
    class Window {
    public:
        static void Init(int width, int height, std::string title, bool start_fullscreen = false);
        static void Close();

        static void Update();
        static void ToggleFullscreen();
        static void SetVSync(bool enabled);
        static void HideCursor();
        static void DisableCursor();
        static void ShowCursor();
        static void ConfineCursor();

        static double GetTime();

        static void ImGuiNewFrame();
        static void ImGuiEndFrame();
        
        static int GetWidth() { return s_Width; }
        static int GetHeight() { return s_Height; }
        static std::string GetTitle() { return s_Title; }
        static GLFWwindow* GetWindowID() { return s_Window; }
        static bool IsFullscreen();
    private:
        static int s_Width;
        static int s_Height;
        static int s_PosX;
        static int s_PosY;
        static int s_ViewportWidth;
        static int s_ViewportHeight;
        static int s_WindowedWidth;
        static int s_WindowedHeight;
        static int s_WindowedPosX;
        static int s_WindowedPosY;
        static std::string s_Title;
        static GLFWwindow* s_Window;
        static bool s_VSyncEnabled;
    private:
        static void CreateWindow(bool fullscreen);
        static void InitGLFW(bool start_fullscreen);
        static void InitOpenGL();
        static void InitImGui();
        static void DestructImGui();
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height); 
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset); 
    };

}   