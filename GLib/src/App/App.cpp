#include "App.h"

#include "Renderer/Render.h"
#include "Window/Window.h"
#include "Scene/Systems.h"
#include "Asset/AssetManager.h"
#include "Asset/Loaders/TextureLoader.h"
#include "Asset/Loaders/GltfLoader.h"

namespace GLib {
    
    App::App() {
        AddSystems(Stage::Update, AssetManagerSystem, RenderUpdateSystem, GltfUpdateSystem);
        
        AssetManager::InsertLoader<TextureLoader>();
        AssetManager::InsertLoader<GltfLoader>();
    }

    void App::Update() {
        Render::BeginFrame();

        for (auto system : m_Systems[Stage::PreUpdate]) {
            system(m_Scene);
        }
        for (auto system : m_Systems[Stage::Update]) {
            system(m_Scene);
        }
        for (auto system : m_Systems[Stage::PostUpdate]) {
            system(m_Scene);
        }

        Render::EndFrame();
        Window::Update();
        Render::Clear();
    }

    void App::Run() {

        Window::Init();
        Render::Init(this);

        for (auto system : m_Systems[Stage::Setup]) {
            system(m_Scene);
        }

        while (!ShouldExit()) {
            Update();
        }

        Window::Close();
    }

}