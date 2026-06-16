#include <iostream>
#include <filesystem>
#include "GLib.h"

int main() {
    std::cout << "Starting...\n";
    GLib::Window::Init(700, 700, "Hello Window");
    std::shared_ptr<GLib::Scene> scene = GLib::CreateScene();

    GLib::Entity CameraEntity = scene->CreateEntity("Camera Entity");
    CameraEntity.Add<GLib::CameraComponent>().m_Camera.SetPerspective(90, 0.01f, 1000.0f);
    CameraEntity.Get<GLib::TransformComponent>().m_Translation = glm::vec3(0.0f, 0.0f, 2.0f);


    bool cursorEnabled = true;
    // GLib::Texture brickTexture = GLib::Texture("assets/textures/wall.jpg", GLib::TextureType::Diffuse);
    // GLib::Texture boxTexture = GLib::Texture("assets/textures/container2.png", GLib::TextureType::Diffuse);
    // GLib::Texture boxSpecTexture = GLib::Texture("assets/textures/container2_specular.png", GLib::TextureType::Specular);

    auto entMonkey = scene->CreateEntity("Monkey");
    entMonkey.Add<GLib::ModelRenderingComponent>("assets/models/Monkey/Monkey.obj");
    entMonkey.Get<GLib::TransformComponent>().m_Translation = glm::vec3(3.0f, 0.0f, 3.0f);

    auto entBackpack = scene->CreateEntity("Backpack");
    entBackpack.Add<GLib::ModelRenderingComponent>("assets/models/backpack/backpack.obj");
    entBackpack.Get<GLib::TransformComponent>().m_Translation = glm::vec3(-3.0f, 0.0f, -3.0f);

    auto entBunny = scene->CreateEntity("Bunny");
    entBunny.Add<GLib::ModelRenderingComponent>("assets/models/bunny/bunny.obj");
    entBunny.Get<GLib::TransformComponent>().m_Scale = glm::vec3(5.0f, 5.0f, 5.0f);
    
    auto entBeast = scene->CreateEntity("Beast");
    entBeast.Add<GLib::ModelRenderingComponent>("assets/models/beast/beast.obj");
    entBeast.Get<GLib::TransformComponent>().m_Scale = glm::vec3(0.03f, 0.03f, 0.03f);
    
    scene->AddSystem<GLib::RenderSystem>();

    while (true) {
        double dt = scene->CalculateDeltaTime();

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

        float camSpeed = 2.5f * dt;

        glm::vec3 direction = CameraEntity.Get<GLib::TransformComponent>().GetRotation();
        glm::vec3 right = CameraEntity.Get<GLib::TransformComponent>().GetRightVector();
        glm::vec3 up = CameraEntity.Get<GLib::TransformComponent>().GetUpVector();

        if (GLib::Input::IsKeyDown(Key::W)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation += camSpeed * direction;
        }
        if (GLib::Input::IsKeyDown(Key::S)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation -= camSpeed * direction;
        }
        if (GLib::Input::IsKeyDown(Key::A)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation -= camSpeed * right;
        }
        if (GLib::Input::IsKeyDown(Key::D)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation += camSpeed * right;
        }
        if (GLib::Input::IsKeyDown(Key::Space)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation += camSpeed * up;
        }
        if (GLib::Input::IsKeyDown(Key::LeftShift)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation -= camSpeed * up;
        }

        if (!cursorEnabled){
            float deltaX = GLib::Input::MouseDeltaX;
            float deltaY = GLib::Input::MouseDeltaY;
            const float sensitivity = 0.2;
            CameraEntity.Get<GLib::TransformComponent>().m_Yaw -= deltaX * sensitivity;
            CameraEntity.Get<GLib::TransformComponent>().m_Pitch -= deltaY * sensitivity;
        }

        scene->UpdateSystems();
    }

    GLib::Window::Close();
    return 0;
}