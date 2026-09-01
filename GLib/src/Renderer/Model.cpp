#include "Model.h"

#include "glm/gtc/type_ptr.hpp"

#include <filesystem>

namespace GLib {

    void Model::loadModel(std::string path)
    {
        std::cout << "Loading Model '" << path << "'... ";

        std::string binPath = path + ".assbin";
        Assimp::Importer import;
        const aiScene* scene = nullptr;

        if (std::filesystem::exists(binPath)) 
            scene = import.ReadFile(binPath, 0);

        if (!scene) {
            scene = import.ReadFile(path, 
                aiProcess_Triangulate | 
                aiProcess_FlipUVs | 
                aiProcess_GenNormals |
                aiProcess_JoinIdenticalVertices |
                aiProcess_OptimizeMeshes
            );

            if (scene) {
                Assimp::Exporter exporter;
                exporter.Export(scene, "assbin", binPath, 0);
            }
        }

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            std::cout << "Error (Assimp) [Model::loadModel]: " << import.GetErrorString() << "\n";
            return;
        }
        m_Directory = path.substr(0, path.find_last_of('/'));

        std::vector<MeshBuildData> meshData;
        processNode(scene->mRootNode, scene, glm::mat4(1.0f), meshData);

        m_Meshes.reserve(meshData.size());
        for (auto& data : meshData) {
            std::vector<std::shared_ptr<Texture>> textures = std::move(data.ReadyTextures);

            for (auto& pending : data.PendingTextures) {
                auto it = m_TexturesLoaded.find(pending.path);
                if (it != m_TexturesLoaded.end()){
                    textures.push_back(it->second);
                    continue;
                }

                const Texture::PixelData& pixels = pending.future.get();
                if (!pixels.IsLoaded) continue;

                auto texture = std::make_shared<Texture>(pending.path, pending.type, pixels);
                m_TexturesLoaded[pending.path] = texture;
                textures.push_back(texture);
            }

            auto mesh = std::make_shared<Mesh>(textures, data.Vertices.data(), (uint32_t)(data.Vertices.size() * sizeof(float)), 
                                               data.Indices.data(), (uint32_t)data.Indices.size());

            if (mesh->IsTransparent())
                m_IsTransparent = true;
            
            m_Meshes.push_back(mesh);
        }

        m_PendingTextures.clear();
        std::cout << "Done!\n";
    }

    void Model::processNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform, std::vector<MeshBuildData>& out) {
        glm::mat4 transform = parentTransform * AiToGlm(node->mTransformation);
        for (unsigned int i = 0; i < node->mNumMeshes; i++){
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            out.push_back(processMesh(mesh, scene, transform));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++){
            processNode(node->mChildren[i], scene, transform, out);
        }
    }

    Model::MeshBuildData Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4& transform)
    {
        MeshBuildData data;
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));

        for (unsigned int i = 0; i < mesh->mNumVertices; i++){
            glm::vec4 pos = transform * glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
            glm::vec3 normal = glm::normalize(normalMatrix * glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));

            data.Vertices.insert(data.Vertices.end(), {
                pos.x, pos.y, pos.z,         // "a_Position", GL_FLOAT, 3
                normal.x, normal.y, normal.z // "a_Normal", GL_FLOAT, 3
            });

            // "a_TexCoord", GL_FLOAT, 2
            if (mesh->mTextureCoords[0]){
                data.Vertices.insert(data.Vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
            }
            else {
                data.Vertices.insert(data.Vertices.end(), { 0.0f, 0.0f });
            }

            // "a_Color", GL_FLOAT, 4
            if (mesh->mColors[0]){
                data.Vertices.insert(data.Vertices.end(), { mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a });
            }
            else {
                data.Vertices.insert(data.Vertices.end(), { 1.0f, 1.0f, 1.0f, 1.0f });
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++){
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++){
                data.Indices.push_back(face.mIndices[j]);
            }
        }

        if (mesh->mMaterialIndex >= 0){
            aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];
            loadMaterialTextures(material, aiTextureType_BASE_COLOR,        TextureType::Albedo,            data);
            loadMaterialTextures(material, aiTextureType_NORMALS,           TextureType::Normal,            data);
            loadMaterialTextures(material, aiTextureType_METALNESS,         TextureType::Metallic,          data);
            loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, TextureType::Roughness,         data);
            loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, TextureType::AmbientOcclusion,  data);
            loadMaterialTextures(material, aiTextureType_DIFFUSE,           TextureType::Diffuse,           data);
            loadMaterialTextures(material, aiTextureType_SPECULAR,          TextureType::Specular,          data);
        }
        
        return data;
    }

    void Model::loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureType textureType, MeshBuildData& data)
    {
        for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++){
            aiString str;
            mat->GetTexture(aiType, i, &str);
            std::string path = m_Directory + '/' + str.C_Str();

            if (auto it = m_TexturesLoaded.find(path); it != m_TexturesLoaded.end()) {
                data.ReadyTextures.push_back(it->second);
                continue;
            }
            if (auto it = m_PendingTextures.find(path); it != m_PendingTextures.end()) {
                data.PendingTextures.push_back({ path, textureType, it->second});
                continue;
            }

            std::shared_future<Texture::PixelData> future = std::async(std::launch::async, [path, textureType]() {
                Texture::PixelData data;
                Texture::LoadPixelData(path, textureType, data);
                return data;
            }).share();

            m_PendingTextures[path] = future;
            data.PendingTextures.push_back({ path, textureType, future });
        }
    }

    glm::mat4 Model::AiToGlm(const aiMatrix4x4 &mat) {
        return glm::transpose(glm::make_mat4(&mat.a1));
    }
}