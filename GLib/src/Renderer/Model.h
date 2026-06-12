#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

#include <string>
#include <vector>

namespace GLib
{
    
    class Model {
    public:
        Model(std::string path) {
            loadModel(path);
        }

        //void Bind(Shader& shader);
        //void Draw(Shader& shader, const glm::vec3& position);

        const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return m_Meshes; }
    private:
        std::vector<std::shared_ptr<Mesh>> m_Meshes;
        std::vector<std::shared_ptr<Texture>> m_TexturesLoaded;
        std::string m_Directory;

        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene);
        std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureType textureType);
    };

}
