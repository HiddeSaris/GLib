#include <iostream>
#include <filesystem>
#include "GLib.h"

#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"
#include "btBulletDynamicsCommon.h"

int main() {
    std::cout << "Starting...\n";
    GLib::Window::Init(700, 700, "Hello Window", false);
    GLib::Render::SetClearColor(0.5f, 0.7f, 0.9f, 1.0f);
    std::shared_ptr<GLib::Scene> scene = GLib::CreateScene();

    GLib::Entity CameraEntity = scene->CreateEntity("Camera Entity");
    CameraEntity.Add<GLib::CameraComponent>().m_Camera.SetPerspective(90, 0.01f, 1000.0f);
    //CameraEntity.Add<GLib::SpotLightComponent>(glm::vec3(1.0f), glm::vec3(0.6f, 0.8f, 1.0f), 10.0f);
    CameraEntity.Get<GLib::TransformComponent>().Translation = glm::vec3(0.0f, 1.0f, 2.0f);
    const float default_cam_speed = 2.5f;

    
    scene->AddSystem<GLib::PhysicsSystem>();
    scene->AddSystem<GLib::RenderSystem>();

    bool cursorEnabled = true;
    auto transparentTex = std::make_shared<GLib::Texture>("assets/textures/blending_transparent_window.png", GLib::TextureType::Diffuse);
    
    // auto entCube = scene->CreateEntity("Cube");
    // entCube.Add<GLib::ModelComponent>("assets/models/cube/cube.obj");
    // entCube.Get<GLib::TransformComponent>().Translation = glm::vec3(0.0f, -0.2f, 0.0f);
    // entCube.Get<GLib::TransformComponent>().Scale = glm::vec3(9.0f, 0.1f, 15.0f);
    // entCube.Add<GLib::RigidBodyComponent>(new btBoxShape(btVector3(1, 1, 1)), 0);
    
    // auto entSphere = scene->CreateEntity("Sphere");
    // entSphere.Add<GLib::ModelComponent>("assets/models/sphere/sphere.obj");
    // entSphere.Add<GLib::PointLightComponent>(10.0f);
    // entSphere.Add<GLib::RigidBodyComponent>(new btSphereShape(1), 1);
    // entSphere.Get<GLib::TransformComponent>().Translation = glm::vec3(0.0f, 10.0f, 0.0f);

    auto entDir = scene->CreateEntity("DirLight");
    entDir.Add<GLib::DirectionalLightComponent>(glm::vec3(0.6f, -1.0f, 0.4f), glm::vec3(0.9f, 0.7f, 0.3f), 10.0f);

    // auto entQuad = scene->CreateEntity("Quad");
    // entQuad.Add<GLib::VertexArrayComponent>(GLib::Render::GetQuadVertexArray(), transparentTex);
    // entQuad.Get<GLib::TransformComponent>().Translation = glm::vec3(1.0f);

    auto entSponza = scene->CreateEntity("Sponza");
    entSponza.Add<GLib::ModelComponent>("assets/models/main1_sponza/NewSponza_Main_glTF_003.gltf");
    entSponza.Get<GLib::TransformComponent>().Translation = glm::vec3(0.0f, 0.0f, 0.0f);
    entSponza.Get<GLib::TransformComponent>().SetYawDeg(90.0f);

    GLib::FrameBuffer framebuffer = GLib::FrameBuffer(800, 600);
    //framebuffer.Bind();
        

    bool running = true;
    float camSpeed = default_cam_speed;
    bool moving = false;
    int entities = 3;
    while (running) {
        GLib::Render::BeginFrame();
        //ImGui::DockSpaceOverViewport();

        float dt = GLib::Render::GetDeltaTime();

        if (GLib::Input::IsKeyDown(Key::Escape)){
            running = false;
        }
        if (GLib::Input::IsKeyDown(Key::F)){
            GLib::Window::ToggleFullscreen();
        }
        if (GLib::Input::IsKeyDown(Key::B)){
            entities++;
            auto ent = scene->CreateEntity("Sphere");
            ent.Add<GLib::ModelComponent>("assets/models/sphere/sphere.obj");
            ent.Add<GLib::RigidBodyComponent>(new btSphereShape(1), 1);
            ent.Get<GLib::TransformComponent>().Translation = glm::vec3(0.0f, 15.0f, 0.0f);
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
            CameraEntity.Get<GLib::TransformComponent>().Translation += camSpeed * dt * direction;
            moving = true;
        }
        if (GLib::Input::IsKeyDown(Key::S)){
            CameraEntity.Get<GLib::TransformComponent>().Translation -= camSpeed * dt * direction;
            moving = true;
        }
        if (GLib::Input::IsKeyDown(Key::A)){
            CameraEntity.Get<GLib::TransformComponent>().Translation -= camSpeed * dt * right;
            moving = true;
        }
        if (GLib::Input::IsKeyDown(Key::D)){
            CameraEntity.Get<GLib::TransformComponent>().Translation += camSpeed * dt * right;
            moving = true;
        }
        if (GLib::Input::IsKeyDown(Key::Space)){
            CameraEntity.Get<GLib::TransformComponent>().Translation += camSpeed * dt * up;
            moving = true;
        }
        if (GLib::Input::IsKeyDown(Key::LeftShift)){
            CameraEntity.Get<GLib::TransformComponent>().Translation -= camSpeed * dt * up;
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
            //auto& light = CameraEntity.Get<GLib::SpotLightComponent>();
            //light.Direction = trans.GetRotation();
        }

        ImGui::Begin("game window", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
            ImGui::BeginChild("Game Render");

            ImVec2 wsize = ImGui::GetWindowSize();
            ImGui::Image((ImTextureID)framebuffer.GetColorAttachmentID(), wsize, ImVec2(0, 1), ImVec2(1, 0));

            ImGui::EndChild();
        ImGui::End();

        ImGui::Begin("Physics", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);

            // if (ImGui::DragFloat3("sphere pos", glm::value_ptr(entSphere.Get<GLib::TransformComponent>().m_Translation)))
            //     entSphere.Get<GLib::RigidBodyComponent>().Activate();
            // btVector3 vel = entSphere.Get<GLib::RigidBodyComponent>().m_Body->getVelocityInLocalPoint(btVector3(0.0f, 0.0f, 0.0f));
            // ImGui::Text("Sphere vel: %f, %f, %f", vel.getX(), vel.getY(), vel.getZ());
            ImGui::Text("num entities: %i", scene->GetNumEntities());
            ImGui::Text("FPS: %f", 1/dt);
        
        ImGui::End();


        scene->UpdateSystems();

        //framebuffer.Unbind();
        GLib::Render::EndFrame();
        GLib::Render::Clear();
        //framebuffer.Bind();
    }

    GLib::Window::Close();
    return 0;
}