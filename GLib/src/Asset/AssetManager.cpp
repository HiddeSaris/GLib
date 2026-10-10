#include "AssetManager.h"

namespace GLib {

    AssetManagerData AssetManager::m_Data;

    void AssetManager::Init() {
        
    }

    void AssetManager::Update() {
        for (auto& func : m_Data.updateCallbacks) {
            func();
        }
    }

    int AssetManager::GetLoaderIdx(const std::filesystem::path& path) {
        size_t hasExtention = path.has_extension();
        if (!hasExtention) {
            std::cout << "Error [AssetManger::GetLoaderInd]: path has no file extention: '" << path.string() << "'\n";
            exit(1);
        }

        std::string extention = path.extension().string().substr(1);
        if (m_Data.extToLoaderInd.find(extention) == m_Data.extToLoaderInd.end()){
            std::cout << "Error [AssetManger::GetLoaderInd]: no loader found for extention: '" << extention << "'\n";
            exit(1);
        }

        return m_Data.extToLoaderInd[extention];
    }

}