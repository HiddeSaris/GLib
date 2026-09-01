#pragma once

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

#include <string>
#include <vector>
#include <future>

namespace GLib
{

    class Model {
    public:
        struct PendingTexture {
            std::string path;
            TextureType type;
            std::shared_future<Texture::PixelData> future;
        };

        struct MeshBuildData {
            std::vector<float> Vertices;
            std::vector<uint32_t> Indices;
            std::vector<std::shared_ptr<Texture>> ReadyTextures;
            std::vector<PendingTexture> PendingTextures;
        };

        Model(std::string path) {
            loadModel(path);
        }

        //void Bind(Shader& shader);
        //void Draw(Shader& shader, const glm::vec3& position);

        const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_Meshes; }

        bool IsTransparent() const { return m_IsTransparent; }
    private:
        std::vector<std::shared_ptr<Mesh>> m_Meshes;
        std::unordered_map<std::string, std::shared_ptr<Texture>> m_TexturesLoaded;
        std::unordered_map<std::string, std::shared_future<Texture::PixelData>> m_PendingTextures;
        std::string m_Directory;
        bool m_IsTransparent = false;


        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform, std::vector<MeshBuildData>& out);
        MeshBuildData processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4& tranform);
        void loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureType textureType, MeshBuildData& data);
        static glm::mat4 AiToGlm(const aiMatrix4x4& mat);
    };

}
