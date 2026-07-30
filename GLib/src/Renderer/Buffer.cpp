#include "Buffer.h"

#include "glad/glad.h"

namespace GLib {
    //////////////////////////////////////////////////////////////////////////////
    ////// V VertexBuffer V //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    VertexBuffer::VertexBuffer(size_t size)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    }

    VertexBuffer::VertexBuffer(float *vertices, size_t size)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
        : m_RendererID(other.m_RendererID), m_Layout(std::move(other.m_Layout))
    {
        other.m_RendererID = 0;
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept
    {
        if (this != &other) {
            if (m_RendererID != 0)
                glDeleteBuffers(1, &m_RendererID);

            m_RendererID = other.m_RendererID;
            m_Layout = std::move(other.m_Layout);
            other.m_RendererID = 0;
        }
        return *this;
    }

    void VertexBuffer::Bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void VertexBuffer::Unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::SetData(const void *data, size_t size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }



    //////////////////////////////////////////////////////////////////////////////
    ////// V IndexBuffer V ///////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////

    IndexBuffer::IndexBuffer(uint32_t *indices, uint32_t count)
        : m_Count(count)
    {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept
        : m_RendererID(other.m_RendererID), m_Count(other.m_Count)
    {
        other.m_RendererID = 0;
        other.m_Count = 0;
    }

    IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept
    {
        if (this != &other) {
            if (m_RendererID != 0)
                glDeleteBuffers(1, &m_RendererID);

            m_RendererID = other.m_RendererID;
            m_Count = other.m_Count;
            other.m_RendererID = 0;
            other.m_Count = 0;
        }
        return *this;
    }

    void IndexBuffer::Bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    void IndexBuffer::Unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

}