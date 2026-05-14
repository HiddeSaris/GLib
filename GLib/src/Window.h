#pragma once

#include <iostream>
#include <string>

struct GLFWwindow;

namespace GLib {
    class Window {
    public:
        static void Init(int width, int height, std::string title);
        static void Close();

        static void Update();
        static void SetVSync(bool enabled);
        static void HideCursor();
        static void DisableCursor();
        static void ShowCursor();
        static void ConfineCursor();

        static double GetTime();
        
        static int GetWidth() { return s_Width; }
        static int GetHeight() { return s_Height; }
        static std::string GetTitle() { return s_Title; }
        static GLFWwindow* GetWindowID() { return s_Window; }
    private:
        static int s_Width;
        static int s_Height;
        static std::string s_Title;
        static GLFWwindow* s_Window;
        static bool s_VSyncEnabled;
    private:
        static void InitGLFW();
        static void InitOpenGL();
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height); 
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset); 
    };

}   