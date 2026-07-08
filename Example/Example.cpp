#include <iostream>
#include <filesystem>
#include "GLib.h"

#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"
#include "btBulletDynamicsCommon.h"

int main() {
    std::cout << "Starting...\n";
    GLib::Window::Init(700, 700, "Hello Window", false);
    std::shared_ptr<GLib::Scene> scene = GLib::CreateScene();

    GLib::Entity CameraEntity = scene->CreateEntity("Camera Entity");
    CameraEntity.Add<GLib::CameraComponent>().m_Camera.SetPerspective(90, 0.01f, 1000.0f);
    CameraEntity.Get<GLib::TransformComponent>().m_Translation = glm::vec3(0.0f, 1.0f, 2.0f);
    const float default_cam_speed = 2.5f;

    
    scene->AddSystem<GLib::PhysicsSystem>();
    scene->AddSystem<GLib::RenderSystem>();

    bool cursorEnabled = true;
    // GLib::Texture brickTexture = GLib::Texture("assets/textures/wall.jpg", GLib::TextureType::Diffuse);
    // GLib::Texture boxTexture = GLib::Texture("assets/textures/container2.png", GLib::TextureType::Diffuse);
    // GLib::Texture boxSpecTexture = GLib::Texture("assets/textures/container2_specular.png", GLib::TextureType::Specular);

    // auto entMonkey = scene->CreateEntity("Monkey");
    // entMonkey.Add<GLib::ModelRenderingComponent>("assets/models/Monkey/Monkey.obj");
    // entMonkey.Get<GLib::TransformComponent>().m_Translation = glm::vec3(3.0f, 0.0f, 3.0f);

    // auto entBackpack = scene->CreateEntity("Backpack");
    // entBackpack.Add<GLib::ModelRenderingComponent>("assets/models/backpack/backpack.obj");
    // entBackpack.Get<GLib::TransformComponent>().m_Translation = glm::vec3(-3.0f, 0.0f, -3.0f);

    // auto entBunny = scene->CreateEntity("Bunny");
    // entBunny.Add<GLib::ModelRenderingComponent>("assets/models/bunny/bunny.obj");
    // entBunny.Get<GLib::TransformComponent>().m_Scale = glm::vec3(5.0f, 5.0f, 5.0f);
    
    // auto entBeast = scene->CreateEntity("Beast");
    // entBeast.Add<GLib::ModelRenderingComponent>("assets/models/beast/beast.obj");
    // entBeast.Get<GLib::TransformComponent>().m_Scale = glm::vec3(0.03f, 0.03f, 0.03f);
    
    auto entSponza = scene->CreateEntity("Sponza");
    entSponza.Add<GLib::ModelRenderingComponent>("assets/models/sponza/sponza.obj");
    entSponza.Get<GLib::TransformComponent>().m_Translation = glm::vec3(0.0f, 0.0f, 0.0f);
    entSponza.Get<GLib::TransformComponent>().SetYawDeg(90.0f);
    
    auto entCube = scene->CreateEntity("Cube");
    entCube.Add<GLib::ModelRenderingComponent>("assets/models/cube/cube.obj");
    entCube.Get<GLib::TransformComponent>().m_Translation = glm::vec3(0.0f, -0.1f, 0.0f);
    entCube.Get<GLib::TransformComponent>().m_Scale = glm::vec3(9.0f, 0.1f, 15.0f);
    //entCube.Get<GLib::TransformComponent>().m_Pitch = 0.0f;
    entCube.Add<GLib::RigidBodyComponent>(new btBoxShape(btVector3(1, 1, 1)), 0);
    
    auto entSphere = scene->CreateEntity("Sphere");
    entSphere.Add<GLib::ModelRenderingComponent>("assets/models/sphere/sphere.obj");
    entSphere.Add<GLib::RigidBodyComponent>(new btSphereShape(1), 1);
    entSphere.Get<GLib::TransformComponent>().m_Translation = glm::vec3(0.0f, 10.0f, 0.0f);

    bool running = true;
    float camSpeed = default_cam_speed;
    bool moving = false;
    int entities = 3;
    while (running) {
        float dt = scene->GetDeltaTime();

        ImGui::Begin("Physics");

        if (ImGui::DragFloat3("sphere pos", glm::value_ptr(entSphere.Get<GLib::TransformComponent>().m_Translation)))
            entSphere.Get<GLib::RigidBodyComponent>().Activate();
        btVector3 vel = entSphere.Get<GLib::RigidBodyComponent>().m_Body->getVelocityInLocalPoint(btVector3(0.0f, 0.0f, 0.0f));
        ImGui::Text("Sphere vel: %f, %f, %f", vel.getX(), vel.getY(), vel.getZ());
        ImGui::Text("num entities: %i", scene->GetNumEntities());
        ImGui::Text("FPS: %f", 1/dt);
        
        ImGui::End();

        if (GLib::Input::IsKeyDown(Key::Escape)){
            running = false;
        }
        if (GLib::Input::IsKeyDown(Key::F)){
            GLib::Window::ToggleFullscreen();
        }
        if (GLib::Input::IsKeyDown(Key::B)){
            entities++;
            auto ent = scene->CreateEntity("Sphere");
            ent.Add<GLib::ModelRenderingComponent>("assets/models/sphere/sphere.obj");
            ent.Add<GLib::RigidBodyComponent>(new btSphereShape(1), 1);
            ent.Get<GLib::TransformComponent>().m_Translation = glm::vec3(0.0f, 15.0f, 0.0f);
        }

        if (GLib::Input::IsKeyPressed(Key::LeftAlt)){
            if (cursorEnabled)
                GLib::Window::DisableCursor();
            else {
                GLib::Window::ShowCursor();
            }
            cursorEnabled = !cursorEnabled;
        }

        glm::vec3 direction = CameraEntity.Get<GLib::TransformComponent>().GetRotation();
        glm::vec3 right = CameraEntity.Get<GLib::TransformComponent>().GetRightVector();
        glm::vec3 up = CameraEntity.Get<GLib::TransformComponent>().GetUpVector();

        moving = false;
        if (GLib::Input::IsKeyDown(Key::W)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation += camSpeed * dt * direction;
            moving = true;
        }
        if (GLib::Input::IsKeyDown(Key::S)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation -= camSpeed * dt * direction;
            moving = true;
        }
        if (GLib::Input::IsKeyDown(Key::A)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation -= camSpeed * dt * right;
            moving = true;
        }
        if (GLib::Input::IsKeyDown(Key::D)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation += camSpeed * dt * right;
            moving = true;
        }
        if (GLib::Input::IsKeyDown(Key::Space)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation += camSpeed * dt * up;
            moving = true;
        }
        if (GLib::Input::IsKeyDown(Key::LeftShift)){
            CameraEntity.Get<GLib::TransformComponent>().m_Translation -= camSpeed * dt * up;
            moving = true;
        }

        if (moving){
            camSpeed += 0.5f * dt;
        }
        else {
            camSpeed = default_cam_speed;
        }

        if (!cursorEnabled){
            float deltaX = GLib::Input::MouseDeltaX;
            float deltaY = GLib::Input::MouseDeltaY;
            const float sensitivity = 0.2;
            auto& trans = CameraEntity.Get<GLib::TransformComponent>();
            trans.RotateDeg(glm::vec3(0.0f, 1.0f, 0.0f), -deltaX * sensitivity);
            trans.RotateDeg(trans.GetRightVector(), -deltaY * sensitivity);
        }

        scene->UpdateSystems();

    }

    GLib::Window::Close();
    return 0;
}