#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

namespace GLib {

    unsigned int MeshTopologyToGL(MeshTopology topology) {
        switch (topology) {
        case MeshTopology::Points:
            return GL_POINTS;
        case MeshTopology::Lines:
            return GL_LINES;
        case MeshTopology::LineLoop:
            return GL_LINE_LOOP;
        case MeshTopology::LineStrip:
            return GL_LINE_STRIP;
        case MeshTopology::Triangles:
            return GL_TRIANGLES;
        case MeshTopology::TriangleStrip:
            return GL_TRIANGLE_STRIP;
        case MeshTopology::TriangleFan:
            return GL_TRIANGLE_FAN;
        default:
            return GL_TRIANGLES;
        }
    }

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

    Mesh::Mesh(float *vertices, uint32_t vertex_bytes, uint32_t *indices, uint32_t index_count, MeshTopology topology)
        : m_Topology(topology)
    {
        m_VertexArray.Bind();

        m_VertexBuffer = std::make_shared<VertexBuffer>(vertices, vertex_bytes); 
        m_VertexBuffer->SetLayout(m_Layout);
        m_VertexArray.AddVertexBuffer(m_VertexBuffer);

        m_IndexBuffer = std::make_shared<IndexBuffer>(indices, index_count);
        m_VertexArray.SetIndexBuffer(m_IndexBuffer);
        m_VertexArray.Unbind();
        
    }

    void Mesh::Bind() const
    {
        // for (unsigned int i = 0; i < m_Textures.size(); i++) {
        //     if (!m_Textures[i]->IsLoaded())
        //         continue;
            
        //     m_Textures[i]->Bind(i);

        //     TextureType type = m_Textures[i]->GetType();
        //     if (type == TextureType::Diffuse){
        //         shader.UploadUniformInt("texture_diffuse", i);
        //     }
        //     else if (type == TextureType::Specular){
        //         shader.UploadUniformInt("texture_specular", i);
        //     }
        //     else if (type == TextureType::Albedo){
        //         shader.UploadUniformInt("u_AlbedoMap", i);
        //     }
        //     else if (type == TextureType::Normal){
        //         shader.UploadUniformInt("u_NormalMap", i);
        //     }
        //     else if (type == TextureType::Metallic){
        //         shader.UploadUniformInt("u_MetallicMap", i);
        //     }
        //     else if (type == TextureType::Roughness){
        //         shader.UploadUniformInt("u_RoughnessMap", i);
        //     }
        //     else if (type == TextureType::AmbientOcclusion){
        //         shader.UploadUniformInt("u_AOMap", i);
        //     }
        //     else {
        //         std::cout << "Error [Mesh::Bind]: Unknown Texture type!\n";
        //     }
        // }

        m_VertexArray.Bind();
    }

}