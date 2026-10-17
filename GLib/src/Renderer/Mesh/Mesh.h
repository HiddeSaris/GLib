#pragma once

#include <vector>

#include "Renderer/Material/Texture.h"
#include "Renderer/Material/Shader.h"
#include "VertexArray.h"
#include "Buffer.h"

namespace GLib {
    
    #define GL_FLOAT 0x1406

    enum class MeshTopology {
        Points,
        Lines,
        LineLoop,
        LineStrip,
        Triangles,
        TriangleStrip,
        TriangleFan,
    };

    unsigned int MeshTopologyToGL(MeshTopology topology);

    class Mesh {
    public:
        //Mesh(std::vector<Texture> textures, float* vertices, uint32_t* indices);
        Mesh(float* vertices, uint32_t vertex_bytes, uint32_t* indices, uint32_t index_count, MeshTopology topology);

        void Bind() const;

        const VertexArray& GetVertexArray() const { return m_VertexArray; }
        const BufferLayout& GetLayout() const { return m_Layout; }
        MeshTopology GetMeshTopology() const { return m_Topology; }

    private:
        BufferLayout m_Layout = BufferLayout({
            { "a_Position", GL_FLOAT, 3 },
            { "a_Normal", GL_FLOAT, 3 },
            { "a_TexCoord", GL_FLOAT, 2 },
            { "a_Color", GL_FLOAT, 4 },
        });
        VertexArray m_VertexArray; 
        std::shared_ptr<VertexBuffer> m_VertexBuffer; 
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
        MeshTopology m_Topology;
    };

}