#pragma once

#include <iostream>
#include <string>

struct GLFWwindow;

namespace GLib {
    class Window {
    public:
        Window(int width, int height, std::string title);
        ~Window();
        void Update();
        void SetVSync(bool enabled);
        void HideCursor();
        void DisableCursor();
        void ShowCursor();
        void ConfineCursor();
    private:
        int m_Width;
        int m_Height;
        std::string m_Title;
        GLFWwindow* m_Window;
        bool VSyncEnabled = false;
    private:
        void InitGLFW();
        void InitOpenGL();
        static void FramebufferSizeCallback(GLFWwindow* window, int width, int height); 
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset); 
    };

}   