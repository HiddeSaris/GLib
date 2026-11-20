#include <iostream>
#include "GLib.h"

#include "glm/gtc/type_ptr.hpp"

#include "imgui.h"

using namespace GLib;

bool cursorEnabled = true;

void setup(Scene& scene) {
    std::cout << "Starting...\n";
    Render::SetClearColor(0.5f, 0.7f, 0.9f, 1.0f);
    
    scene.CreateEntity(
        TagComponent("Camera Entity"),
        TransformComponent(glm::vec3(0.0f, 1.0f, 2.0f)),
        CameraComponent()
    );
    
    scene.CreateEntity(
        TagComponent("PointLight"),
        TransformComponent().SetTranslation(glm::vec3(-1.0f, 0.0f, 1.0f)),
        PointLightComponent(glm::vec3(0.9f, 0.7f, 0.3f), 10.0f)
    );

    scene.CreateEntity(
        TagComponent("DirLight"),
        TransformComponent(),
        DirectionalLightComponent(glm::vec3(0.6f, -1.0f, 0.4f), glm::vec3(0.9f, 0.7f, 0.3f), 10.0f)
    );

    Handle<Gltf> gltf = AssetManager::Load<Gltf>("assets/models/flighthelmet/source/FlightHelmet/FlightHelmet.gltf");
    scene.CreateEntity(
        TagComponent("helmet"), 
        TransformComponent(),
        GltfSceneComponent(gltf)
    );
}

void update(Scene& scene) {
    scene.ShowImGuiDebugMenu();
    if (Input::IsKeyDown(Key::Escape)){
        Window::Close();
    }

    if (Input::IsKeyDown(Key::F)){
        Window::ToggleFullscreen();
    }

    if (Input::IsKeyPressed(Key::LeftAlt)){
        if (cursorEnabled)
            Window::DisableCursor();
        else {
            Window::ShowCursor();
        }
        cursorEnabled = !cursorEnabled;
    }
}

void camera_move(Scene& scene) {
    for (auto [entity, cam, trans] : scene.View<CameraComponent, TransformComponent>().Each()) {
        glm::vec3 direction = trans.GetRotation();
        glm::vec3 right = trans.GetRightVector();
        glm::vec3 up = trans.GetUpVector();
        
        float dt = Render::GetDeltaTime();
        const float default_cam_speed = 0.5f;
        bool moving = false;
        static float camSpeed = default_cam_speed;
        if (Input::IsKeyDown(Key::W)){
            trans.Translation += camSpeed * dt * direction;
            moving = true;
        }
        if (Input::IsKeyDown(Key::S)){
            trans.Translation -= camSpeed * dt * direction;
            moving = true;
        }
        if (Input::IsKeyDown(Key::A)){
            trans.Translation -= camSpeed * dt * right;
            moving = true;
        }
        if (Input::IsKeyDown(Key::D)){
            trans.Translation += camSpeed * dt * right;
            moving = true;
        }
        if (Input::IsKeyDown(Key::Space)){
            trans.Translation += camSpeed * dt * up;
            moving = true;
        }
        if (Input::IsKeyDown(Key::LeftShift)){
            trans.Translation -= camSpeed * dt * up;
            moving = true;
        }

        if (moving){
            camSpeed += 0.5f * dt;
        }
        else {
            camSpeed = default_cam_speed;
        }

        if (!cursorEnabled){
            float deltaX = Input::MouseDeltaX;
            float deltaY = Input::MouseDeltaY;
            const float sensitivity = 0.2;
            trans.RotateDeg(glm::vec3(0.0f, 1.0f, 0.0f), -deltaX * sensitivity);
            trans.RotateDeg(trans.GetRightVector(), -deltaY * sensitivity);
            //auto& light = CameraEntity.Get<SpotLightComponent>();
            //light.Direction = trans.GetRotation();
        }
    }
}

int main() {
    App() // Ply / Poly / Vortex / Breeze
        .AddSystems(Stage::Setup, setup)
        .AddSystems(Stage::Update, update, camera_move)
        .Run();
}
