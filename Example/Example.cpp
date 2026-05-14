#include <iostream>
#include "GLib.h"

int main() {
    std::cout << "Starting...\n";
    GLib::Window::Init(700, 700, "Hello Window");
    GLib::PerspectiveCamera camera = GLib::PerspectiveCamera(90, 0.01, 100);

    bool cursorEnabled = true;
    std::shared_ptr<GLib::Texture> brickTexture = std::make_shared<GLib::Texture>("assets/textures/wall.jpg");

    while (true) {
        GLib::Render::BeginFrame(camera);

        if (GLib::Input::IsKeyDown(Key::Escape)){
            break;
        }

        if (GLib::Input::IsKeyPressed(Key::LeftAlt)){
            if (cursorEnabled)
                GLib::Window::DisableCursor();
            else {
                GLib::Window::ShowCursor();
            }
            cursorEnabled = !cursorEnabled;
        }

        float camSpeed = 0.1f;

        if (GLib::Input::IsKeyDown(Key::W)){
            camera.SetPosition(camera.GetPosition() + camSpeed * camera.GetDirection());
        }
        if (GLib::Input::IsKeyDown(Key::S)){
            camera.SetPosition(camera.GetPosition() - camSpeed * camera.GetDirection());
        }
        if (GLib::Input::IsKeyDown(Key::A)){
            camera.SetPosition(camera.GetPosition() - camSpeed * camera.GetRightVector());
        }
        if (GLib::Input::IsKeyDown(Key::D)){
            camera.SetPosition(camera.GetPosition() + camSpeed * camera.GetRightVector());
        }
        if (GLib::Input::IsKeyDown(Key::Space)){
            camera.SetPosition(camera.GetPosition() + camSpeed * camera.GetUpVector());
        }
        if (GLib::Input::IsKeyDown(Key::LeftShift)){
            camera.SetPosition(camera.GetPosition() - camSpeed * camera.GetUpVector());
        }

        if (!cursorEnabled){
            float deltaX = GLib::Input::MouseDeltaX;
            float deltaY = GLib::Input::MouseDeltaY;
            const float sensitivity = 0.2;
            camera.SetYaw(camera.GetYaw() - deltaX * sensitivity);
            camera.SetPitch(camera.GetPitch() - deltaY * sensitivity);
        }

        
        GLib::Render::RenderQuad(glm::vec3(0.0f, 0.0f, 0.0f), brickTexture);

        if (GLib::Input::IsKeyDown(Key::B)){
            GLib::Render::RenderCube(glm::vec3(0.0f, 0.0f, 1.0f));
        }

        GLib::Render::EndFrame();
        GLib::Window::Update();
    }

    GLib::Window::Close();
    return 0;
}