#pragma once

#include "Asset.h"
#include "AssetLoader.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <future>
#include <filesystem>

namespace GLib {

    template <typename A>
    struct PendingLoad {
        std::shared_future<std::any> data;
        Handle<A> handle;
        AssetLoader* loader;
    };

    struct AssetManagerData {
        std::unordered_map<std::string, uint32_t> pathToInd;

        std::vector<std::unique_ptr<AssetLoader>> loaders;
        std::unordered_map<std::string, uint32_t> extToLoaderInd;

        std::vector<std::function<void()>> updateCallbacks;
    };

    class AssetManager {
    public:
        static void Init();
        static void Update();

        template<typename A>
        static Handle<A> Load(const std::filesystem::path& path);

        template<typename A>
        static Handle<A> Add(std::shared_ptr<A> asset);

        template<typename A>
        static bool IsLoaded(Handle<A> handle);
        
        template<typename A>
        static std::shared_ptr<A> Get(Handle<A> handle);

        template<typename L>
        static void InsertLoader();
        
        template<typename A>
        static AssetStorage<A>& GetAssetStorage();
    private:
        static int GetLoaderIdx(const std::filesystem::path& path);

        template<typename A>
        static std::vector<PendingLoad<A>>& GetPending();

        template<typename A>
        static void UpdatePending();

        template<typename A>
        static void EnsureRegistered();
    private:
        static AssetManagerData m_Data;
    };

    
    template<typename A>
    inline Handle<A> AssetManager::Load(const std::filesystem::path &path) {
        if (m_Data.pathToInd.find(path.string()) != m_Data.pathToInd.end()) {
            return Handle<A>{ m_Data.pathToInd.at(path.string()) };
        }

        int index = GetLoaderIdx(path);
        AssetLoader* loader = m_Data.loaders[index].get();
        Handle<A> handle = GetAssetStorage<A>().Reserve();
        m_Data.pathToInd[path.string()] = handle.index;

        EnsureRegistered<A>();

        std::shared_future<std::any> future = std::async(std::launch::async, 
            [loader, path]() {return loader->LoadData(path); }).share();

        GetPending<A>().push_back({ future, handle, loader });
        return handle;
    }

    template <typename A>
    inline Handle<A> AssetManager::Add(std::shared_ptr<A> asset) {
        Handle<A> handle = GetAssetStorage<A>().Reserve();
        GetAssetStorage<A>().Insert(handle, asset);
        return handle;
    }

    template <typename A>
    inline bool AssetManager::IsLoaded(Handle<A> handle) {
        return GetAssetStorage<A>().IsLoaded(handle);
    }

    template <typename A>
    inline std::shared_ptr<A> AssetManager::Get(Handle<A> handle) {
        if (!GetAssetStorage<A>().IsLoaded(handle)) {
            std::cout << "Error [AssetManager::Get]: asset was not yet loaded\n";
        }
        return GetAssetStorage<A>().Get(handle);
    }

    template <typename L>
    inline void AssetManager::InsertLoader() {
        m_Data.loaders.emplace_back(std::make_unique<L>());

        uint32_t index = m_Data.loaders.size() - 1;
        std::vector<std::string> extentions = m_Data.loaders[index]->GetFileExtentions();
        for (std::string ext : extentions){
            m_Data.extToLoaderInd.insert({ext, index});
        }
    }

    template <typename A>
    inline AssetStorage<A>& AssetManager::GetAssetStorage() {
        static AssetStorage<A> s_AssetStorage;
        return s_AssetStorage;
    }

    template <typename A>
    inline std::vector<PendingLoad<A>>& AssetManager::GetPending() {
        static std::vector<PendingLoad<A>> s_Pending;
        return s_Pending;
    }
    
    template <typename A>
    inline void AssetManager::UpdatePending() {
        auto& pending = GetPending<A>();
        for (auto it = pending.begin(); it != pending.end(); ) {
            if (it->data.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                std::any result = it->loader->InitAsset(it->data.get());
                std::shared_ptr<A> asset;
                if (result.type() != typeid(asset)) {
                    std::cout << "Error [AssetManager::Load]: loader did not produce a shared pointer of the requested asset type.\n";
                    exit(1);
                }
                asset = std::any_cast<std::shared_ptr<A>>(result);
                GetAssetStorage<A>().Insert(it->handle, asset);
                it = pending.erase(it);
            }
            else {
                it++;
            }
        }
    }
    
    template <typename A>
    inline void AssetManager::EnsureRegistered() {
        static bool registered = []() {
            m_Data.updateCallbacks.push_back(UpdatePending<A>);
            return true;
        }();
        (void)registered;
    }
}