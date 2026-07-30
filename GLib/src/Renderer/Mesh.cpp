#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace GLib {

    // Mesh::Mesh(std::vector<Texture> textures, float *vertices, uint32_t *indices)
    //     : m_Textures(textures),
    //     m_VertexBuffer(std::make_shared<VertexBuffer>(vertices, sizeof(vertices))),
    //     m_IndexBuffer(std::make_shared<IndexBuffer>(indices, (uint32_t)(sizeof(indices) / sizeof(uint32_t))))
    // {
    //     m_VertexArray.Bind();
    //     m_VertexBuffer->SetLayout(m_Layout);
    //     m_VertexArray.AddVertexBuffer(m_VertexBuffer);
    //     m_VertexArray.SetIndexBuffer(m_IndexBuffer);
    // }

    Mesh::Mesh(std::vector<std::shared_ptr<Texture>> textures, float *vertices, uint32_t vertex_count, uint32_t *indices, uint32_t index_count)
        : m_Textures(textures)
    {
        m_VertexArray.Bind();

        m_VertexBuffer = std::make_shared<VertexBuffer>(vertices, vertex_count); 
        m_VertexBuffer->SetLayout(m_Layout);
        m_VertexArray.AddVertexBuffer(m_VertexBuffer);

        m_IndexBuffer = std::make_shared<IndexBuffer>(indices, index_count);
        m_VertexArray.SetIndexBuffer(m_IndexBuffer);
        m_VertexArray.Unbind();

        for (auto& tex : m_Textures){
            if (tex->IsTransparent()) {
                m_IsTransparent = true;
                break;
            }
        }
        
    }

    void Mesh::Bind(Shader &shader) const
    {
        shader.Bind();
        for (unsigned int i = 0; i < m_Textures.size(); i++) {
            if (!m_Textures[i]->IsLoaded())
                continue;
            
            m_Textures[i]->Bind(i);

            TextureType type = m_Textures[i]->GetType();
            if (type == TextureType::Diffuse){
                shader.UploadUniformInt("texture_diffuse", i);
            }
            else if (type == TextureType::Specular){
                shader.UploadUniformInt("texture_specular", i);
            }
            else {
                std::cout << "Error [Mesh::Bind]: Unknown Texture type!\n";
            }
        }

        m_VertexArray.Bind();
    }

}