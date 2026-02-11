#include "Input.h"

#include <GLFW/glfw3.h>

namespace GLib {

    GLFWwindow* Input::s_Window = nullptr;

    double Input::MouseX = 0;
    double Input::MouseY = 0;
    std::pair<double, double> Input::MousePosition = {0, 0};
    double Input::MouseDeltaX = 0;
    double Input::MouseDeltaY = 0;

    double Input::ScrollX = 0;
    double Input::ScrollY = 0;

    std::array<bool, GLFW_KEY_LAST + 1> Input::s_CurrentKeys{};
    std::array<bool, GLFW_KEY_LAST + 1> Input::s_PreviousKeys{};
    
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> Input::s_CurrentMouseButtons{};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> Input::s_PreviousMouseButtons{};

    void Input::Init(GLFWwindow* window) {
        s_Window = window;
    }

    void Input::Update() {
        ScrollX = 0;
        ScrollY = 0;
        
        glfwPollEvents();
        
        double oldX = MouseX;
        double oldY = MouseY;
        glfwGetCursorPos(s_Window, &MouseX, &MouseY);
        MousePosition = {MouseX, MouseY};
        MouseDeltaX = MouseX - oldX;
        MouseDeltaY = MouseY - oldY;

        s_PreviousKeys         = s_CurrentKeys;
        s_PreviousMouseButtons = s_CurrentMouseButtons;

        for (int i = 0; i < GLFW_KEY_LAST + 1; i++) {
            s_CurrentKeys[i] = glfwGetKey(s_Window, i) == GLFW_PRESS;
        }
        for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST + 1; i++) {
            s_CurrentMouseButtons[i] = glfwGetMouseButton(s_Window, i) == GLFW_PRESS;
        }
    }

    // keyboard input
    bool Input::IsKeyDown(Key key){
        if (static_cast<int>(key) < 0 || key > Key::Last) {
            return false;
        }
        return s_CurrentKeys[static_cast<int>(key)];
    }

    bool Input::IsKeyPressed(Key key) {
        if (static_cast<int>(key) < 0 || key > Key::Last) {
            return false;
        }
        return s_CurrentKeys[static_cast<int>(key)] && !s_PreviousKeys[static_cast<int>(key)];
    }

    bool Input::IsKeyReleased(Key key) {
        if (static_cast<int>(key) < 0 || key > Key::Last) {
            return false;
        }
        return !s_CurrentKeys[static_cast<int>(key)] && s_PreviousKeys[static_cast<int>(key)];
    }

    // mouse input
    bool Input::IsMouseDown(MouseButton button){
        if (static_cast<int>(button) < 0 || button > MouseButton::Last) {
            return false;
        }
        return s_CurrentMouseButtons[static_cast<int>(button)];
    }

    bool Input::IsMousePressed(MouseButton button) {
        if (static_cast<int>(button) < 0 || button > MouseButton::Last) {
            return false;
        }
        return s_CurrentMouseButtons[static_cast<int>(button)] && !s_PreviousMouseButtons[static_cast<int>(button)];
    }

    bool Input::IsMouseReleased(MouseButton button) {
        if (static_cast<int>(button) < 0 || button > MouseButton::Last) {
            return false;
        }
        return !s_CurrentMouseButtons[static_cast<int>(button)] && s_PreviousMouseButtons[static_cast<int>(button)];
    }


}