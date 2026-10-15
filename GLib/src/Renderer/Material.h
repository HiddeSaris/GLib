#pragma once

#include <optional>

#include "Asset/AssetManager.h"
#include "Renderer/Texture.h"

#include <glm/glm.hpp>

namespace GLib {

    struct Material {
        glm::vec4 baseColor = glm::vec4(1.0f);
        std::optional<Handle<Texture>> baseColorTexture;
        
        float metallic = 0.0f;
        float rougness = 1.0f;
        std::optional<Handle<Texture>> metallicRoughnessTexture;
        
        std::optional<Handle<Texture>> normalTexture;
        
        std::optional<Handle<Texture>> ambientOcclusionTexture;

        bool doubleSided = false;
    };

}