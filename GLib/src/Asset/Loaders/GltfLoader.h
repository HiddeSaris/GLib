#pragma once

#include <filesystem>
#include <optional>
#include <vector>
#include <stack>

#include <fastgltf/core.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "Asset/AssetManager.h"
#include "Asset/AssetLoader.h"
#include "Renderer/Mesh/Mesh.h"
#include "Renderer/Material/Material.h"
#include "Scene/Scene.h"

namespace GLib {
    
    /*struct GltfMaterial {
        glm::vec4 baseColor;
        std::optional<Handle<Texture>> baseColorTexture;
        
        float metallic;
        float rougness;
        std::optional<Handle<Texture>> metallicRoughnessTexture;
        
        std::optional<Handle<Texture>> normalTexture;
        
        std::optional<Handle<Texture>> ambientOcclusionTexture;
    };*/

    struct GltfPrimitive {
        Handle<Mesh> mesh;
        std::optional<Handle<Material>> material;
        std::string name;
    };
    
    struct GltfMesh {
        std::vector<GltfPrimitive> primitives;
    };

    struct GltfNode {
        std::string name;
        std::vector<Handle<GltfNode>> children;
        std::optional<Handle<GltfMesh>> mesh;
        
        glm::vec3 translation;
        glm::quat rotation;
        glm::vec3 scale;
    };
    
    struct GltfScene {
        std::string name;
        std::vector<Handle<GltfNode>> rootNodes;
    };

    struct Gltf {
        std::vector<Handle<GltfScene>> scenes;
        Handle<GltfScene> defaultScene;
        std::vector<Handle<GltfNode>> nodes;

        std::vector<Handle<GltfMesh>> meshes;
        std::vector<Handle<Material>> materials;
    };

    class GltfLoader : public AssetLoader {
        struct MaterialData {
            glm::vec4 baseColor = glm::vec4(1.0f);
            float metallic = 1.0f;
            float roughness = 1.0f;
            int albedoTexIndex = -1;
            int normalTexIndex = -1;
            int metallicRoughnessTexIndex = -1;
            int aoTexIndex = -1;
            bool doubleSided = false;
        };

        struct TextureData {
            std::string uri;
            
            unsigned char* pixels;
            int width, height, channels;
            bool isEmbedded = false;

            Handle<Texture> loadedTexture;
        };

        struct MeshData {
            std::string name;
            std::vector<float> vertices;
            std::vector<uint32_t> indices;
            std::optional<size_t> materialIndex;
            MeshTopology topology = MeshTopology::Triangles;
        };

        struct NodeData {
            std::string name;
            glm::vec3 translation{0.0f};
            glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
            glm::vec3 scale{1.0f};
            int meshIndex = -1;
            std::vector<uint32_t> children;
        };

        struct SceneData {
            std::string name;
            std::vector<uint32_t> rootNodes;
        };

        struct ImportedModel {
            std::vector<MaterialData> materials;
            std::vector<TextureData> textures;
            std::vector<std::vector<MeshData>> primitives;
            std::vector<NodeData> nodes;

            std::vector<SceneData> scenes;
            std::optional<size_t> defaultScene;
        };

    public:
        std::any LoadData(const std::filesystem::path& path) const override;
        std::any InitAsset(std::any data) const override;

        std::vector<std::string> GetFileExtentions() const override {
            return {"gltf", "glb"};
        }
    private:
        std::shared_ptr<fastgltf::Asset> LoadAsset(const std::filesystem::path& path) const;
        void LoadMaterials(std::shared_ptr<fastgltf::Asset> asset, ImportedModel& model) const;
        void LoadTextures(std::shared_ptr<fastgltf::Asset> asset, ImportedModel& model, const std::filesystem::path& baseDir) const;
        void LoadMeshes(std::shared_ptr<fastgltf::Asset> asset, ImportedModel& model) const;
        void GetNodeTransform(const fastgltf::Node& node, glm::vec3& scale, glm::quat& rotation, glm::vec3& translation) const;
        void LoadNodes(std::shared_ptr<fastgltf::Asset> asset, ImportedModel& model) const;
        void LoadScenes(std::shared_ptr<fastgltf::Asset> asset, ImportedModel& model) const;
    };

    class Scene;
    void GltfInitScene(Scene& scene, Entity root);

}