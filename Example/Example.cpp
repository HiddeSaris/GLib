#include <iostream>
#include "GLib.h"

int main() {
    GLib::Window window(600, 800, "Hello Window");
    while (true) {
        if (GLib::Input::IsMouseDown(MouseButton::Left)) {
            std::cout << "Pressed!" << std::endl;
        }
        std::cout << GLib::Input::MouseDeltaX << ", " << GLib::Input::MouseDeltaY << " - " << GLib::Input::ScrollX << ", " << GLib::Input::ScrollY << std::endl;

        window.Update();
    }
    return 0;
}