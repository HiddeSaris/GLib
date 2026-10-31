#pragma once

#include <iostream>
#include <string>

struct GLFWwindow;

namespace GLib {
    class Window {
    public:
        static void Init();
        static void Close();
        
        static void Update();
        static void ToggleFullscreen();
        static void HideCursor();
        static void DisableCursor();
        static void ShowCursor();
        static void ConfineCursor();
        
        static int GetWidth() { return s_Width; }
        static int GetHeight() { return s_Height; }
        static int GetViewportWidth() { return s_ViewportWidth; }
        static int GetViewportHeight() { return s_ViewportHeight; }
        static std::string GetTitle() { return s_Title; }
        static GLFWwindow* GetWindowID() { return s_Window; }
        static double GetTime();
        static bool IsFullscreen();
        static bool IsClosed() { return !s_Initialized; }
        static bool ShouldClose();

        static void SetSize(int width, int height);
        static void SetPos(int x, int y);
        static void SetTitle(const std::string& title);
        static void SetVSync(bool enabled);
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
        static bool s_Initialized;
        static bool s_VSyncEnabled;
    private:
        static void CreateWindow();
        static void InitGLFW();
        static void InitOpenGL();
        // static void InitImGui();
        static void DestructImGui();
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height); 
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset); 
    };

    void CheckGLError(const char* label);
    
}   