#pragma once

#include "Renderer/Texture.h"
#include "Asset/AssetLoader.h"

namespace GLib {

    class TextureLoader : public AssetLoader {
    public:
        std::any LoadData(const std::filesystem::path& path) const override {
            auto data = std::make_shared<Texture::PixelData>();
            Texture::LoadPixelData(path, TextureType::Albedo, *data);
            return data;
        }

        std::any InitAsset(std::any data) const override {
            auto pixels = std::any_cast<std::shared_ptr<Texture::PixelData>>(std::move(data));
            return std::make_shared<Texture>(TextureType::Albedo, *pixels);
        }

        std::vector<std::string> GetFileExtentions() const override {
            return {"png", "jpg", "jpeg"};
        }
    };

}