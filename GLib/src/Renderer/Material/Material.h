#pragma once

#include <optional>

#include "Asset/AssetManager.h"
#include "Texture.h"

#include <glm/glm.hpp>

namespace GLib {

    enum class AlphaMode {
        Opaque,
        Mask,
        Blend,
    };

    struct Material {
        glm::vec4 baseColor = glm::vec4(1.0f);
        std::optional<Handle<Texture>> baseColorTexture;
        
        float metallic = 1.0f;
        float roughness = 1.0f;
        std::optional<Handle<Texture>> metallicRoughnessTexture;
        
        std::optional<Handle<Texture>> normalTexture;
        
        std::optional<Handle<Texture>> ambientOcclusionTexture;

        AlphaMode alphaMode = AlphaMode::Opaque;
        float alphaCutOff = 0.5f;
        bool doubleSided = false;

        void Bind(Shader& shader) {
            shader.Bind();

            constexpr int albedoSlot = 0;
            if (baseColorTexture.has_value()) {
                AssetManager::Get(baseColorTexture.value())->Bind(albedoSlot);
            }
            else {
                Texture::WhiteTexture()->Bind(albedoSlot);
            }
            shader.UploadUniformFloat4("u_Albedo", baseColor);
            shader.UploadUniformInt("u_AlbedoMap", albedoSlot);
            
            
            constexpr int metallicRoughnessSlot = 1;
            if (metallicRoughnessTexture.has_value()) {
                AssetManager::Get(metallicRoughnessTexture.value())->Bind(metallicRoughnessSlot);
            }
            else {
                Texture::WhiteTexture()->Bind(metallicRoughnessSlot);
            }
            shader.UploadUniformFloat("u_Metallic", metallic);
            shader.UploadUniformFloat("u_Roughness", roughness);
            shader.UploadUniformInt("u_MetallicRoughnessMap", metallicRoughnessSlot);

            constexpr int normalSlot = 2;
            if (normalTexture.has_value()) {
                AssetManager::Get(normalTexture.value())->Bind(normalSlot);
            }
            else {
                Texture::DefaultNormalTexture()->Bind(normalSlot);
            }
            shader.UploadUniformInt("u_NormalMap", normalSlot);
            
            constexpr int aoSlot = 3;
            if (ambientOcclusionTexture.has_value()) {
                AssetManager::Get(ambientOcclusionTexture.value())->Bind(aoSlot);
            }
            else {
                Texture::WhiteTexture()->Bind(aoSlot);
            }
            shader.UploadUniformInt("u_AOMap", aoSlot);

            shader.UploadUniformFloat("u_AlphaCutOff", (alphaMode == AlphaMode::Mask) ? alphaCutOff : 0.0f);
        }
    };

}