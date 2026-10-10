#pragma once

#include <string>
#include <vector>
#include <any>
#include <filesystem>

namespace GLib {

    class AssetLoader {
    public:
        virtual ~AssetLoader() = default;
        virtual std::any LoadData(const std::filesystem::path& path) const = 0;
        virtual std::any InitAsset(std::any data) const = 0;
        
        virtual std::vector<std::string> GetFileExtentions() const = 0;
    };

}
