#include <iostream>
#include <filesystem>
#include "GLib.h"

int main() {
    std::cout << "Starting...\n";
    GLib::Window::Init(700, 700, "Hello Window");
    GLib::PerspectiveCamera camera = GLib::PerspectiveCamera(90, 0.01, 1000);
    camera.SetPosition(glm::vec3(0.0f, 0.0f, 2.0f));

    bool cursorEnabled = true;
    GLib::Texture brickTexture = GLib::Texture("assets/textures/wall.jpg", GLib::TextureType::Diffuse);
    GLib::Texture boxTexture = GLib::Texture("assets/textures/container2.png", GLib::TextureType::Diffuse);
    GLib::Texture boxSpecTexture = GLib::Texture("assets/textures/container2_specular.png", GLib::TextureType::Specular);

    GLib::Model monkey("assets/models/Monkey/Monkey.obj");
    GLib::Model backpack("assets/models/backpack/backpack.obj");
    //GLib::Model house("assets/models/RevitHouse/RevitHouse.obj");
    GLib::Model bunny("assets/models/bunny/bunny.obj");
    GLib::Model beast("assets/models/beast/beast.obj");

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

        float camSpeed = 0.05f;

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

        boxSpecTexture.Bind(1);
        GLib::Render::RenderQuad(glm::vec3(0.0f, 0.0f, 0.0f), boxTexture);

        if (GLib::Input::IsKeyDown(Key::B)){
            GLib::Render::RenderCube(glm::vec3(0.0f, 0.0f, 1.0f));
        }

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        GLib::Render::Submit(monkey, transform);

        transform = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(3.0f)), glm::vec3(-1.0f, -1.0f, -1.0f));
        GLib::Render::Submit(backpack, transform);
        GLib::Render::Submit(bunny, glm::scale(glm::mat4(1.0f), glm::vec3(5.0f)));
        GLib::Render::Submit(beast, glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));

        GLib::Render::EndFrame();
        GLib::Window::Update();
    }

    GLib::Window::Close();
    return 0;
}