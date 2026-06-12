#include "Model.h"


namespace GLib {

    // void Model::Bind(Shader&  shader)
    // {
    //     for (unsigned int i = 0; i < m_Meshes.size(); i++){
    //         m_Meshes[i].Bind(shader);
    //     }
    // }

    // void Model::Draw(Shader & shader, const glm::vec3& position)
    // {
    //     for (unsigned int i = 0; i < m_Meshes.size(); i++){
    //         m_Meshes[i].Draw(shader, position);
    //     }
    // }

    void Model::loadModel(std::string path)
    {
        std::cout << "Loading Model '" << path << "'...\n";
        Assimp::Importer import;
        import.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
            std::cout << "Error (Assimp) [Model::loadModel]: " << import.GetErrorString() << "\n";
            return;
        }
        m_Directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++){
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++){
            processNode(node->mChildren[i], scene);
        }
    }

    std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<std::shared_ptr<Texture>> textures;
        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++){
            // "a_Position", GL_FLOAT, 3
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);

            // "a_Normal", GL_FLOAT, 3
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);

            // "a_TexCoord", GL_FLOAT, 2
            if (mesh->mTextureCoords[0]){
                vertices.push_back(mesh->mTextureCoords[0][i].x);
                vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
            else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }

            // "a_Color", GL_FLOAT, 4
            if (mesh->mColors[0]){
                vertices.push_back(mesh->mColors[0][i].r);
                vertices.push_back(mesh->mColors[0][i].g);
                vertices.push_back(mesh->mColors[0][i].b);
                vertices.push_back(mesh->mColors[0][i].a);
            }
            else {
                vertices.push_back(1.0f);
                vertices.push_back(1.0f);
                vertices.push_back(1.0f);
                vertices.push_back(1.0f);
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++){
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++){
                indices.push_back(face.mIndices[j]);
            }
        }

        if (mesh->mMaterialIndex >= 0){
            aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<std::shared_ptr<Texture>> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::Specular);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }
        
        return std::make_shared<Mesh>(textures, vertices.data(), vertices.size() * sizeof(float), indices.data(), (uint32_t)indices.size());
    }

    std::vector<std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType aiType, TextureType textureType)
    {
        std::vector<std::shared_ptr<Texture>> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(aiType); i++){
            aiString str;
            mat->GetTexture(aiType, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < m_TexturesLoaded.size(); j++){
                if (std::strcmp(m_TexturesLoaded[j]->GetPath().data(), (m_Directory + "/" + str.C_Str()).c_str()) == 0){
                    textures.push_back(m_TexturesLoaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip){
                std::shared_ptr<Texture> texture = std::make_shared<Texture>(m_Directory + '/' + str.C_Str(), textureType);
                textures.push_back(texture);
                m_TexturesLoaded.push_back(texture);
            }
        }

        return textures;
    }

}