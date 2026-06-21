#pragma once

#include <array>
#include "KeyCodes.h"

struct GLFWwindow;

namespace GLib {

    class Input {
    public:
        static double MouseX, MouseY;
        static std::pair<double, double> MousePosition;
        static double MouseDeltaX, MouseDeltaY;
        static double ScrollX, ScrollY;
    public:
        static void Init(GLFWwindow* window);

        static void Update();

        static bool IsKeyDown(Key key);
        static bool IsKeyPressed(Key key);
        static bool IsKeyReleased(Key key);
        
        static bool IsMouseDown(MouseButton button);
        static bool IsMousePressed(MouseButton button);
        static bool IsMouseReleased(MouseButton button);
    private:
        static GLFWwindow* s_Window;

        static std::array<bool, static_cast<int>(Key::Last) + 1> s_CurrentKeys;
        static std::array<bool, static_cast<int>(Key::Last) + 1> s_PreviousKeys;

        static std::array<bool, static_cast<int>(MouseButton::Last) + 1> s_CurrentMouseButtons;
        static std::array<bool, static_cast<int>(MouseButton::Last) + 1> s_PreviousMouseButtons;
    };

}