#include "VertexArray.h"

#include "glad/glad.h"

namespace GLib {

    VertexArray::VertexArray()
    {
        glGenVertexArrays(1, &m_RenderID);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_RenderID);
    }

    VertexArray::VertexArray(VertexArray&& other) noexcept
        : m_RenderID(other.m_RenderID),
          m_VertexBufferIndex(other.m_VertexBufferIndex),
          m_VertexBuffers(std::move(other.m_VertexBuffers)),
          m_IndexBuffer(std::move(other.m_IndexBuffer))
    {
        other.m_RenderID = 0;
        other.m_VertexBufferIndex = 0;
    }

    VertexArray& VertexArray::operator=(VertexArray&& other) noexcept
    {
        if (this != &other) {
            if (m_RenderID != 0)
                glDeleteVertexArrays(1, &m_RenderID);

            m_RenderID = other.m_RenderID;
            m_VertexBufferIndex = other.m_VertexBufferIndex;
            m_VertexBuffers = std::move(other.m_VertexBuffers);
            m_IndexBuffer = std::move(other.m_IndexBuffer);

            other.m_RenderID = 0;
            other.m_VertexBufferIndex = 0;
        }
        return *this;
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(m_RenderID);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer)
    {
        glBindVertexArray(m_RenderID);
        vertexBuffer->Bind();

        const BufferLayout& layout = vertexBuffer->GetLayout();
        for(const BufferElement& element : layout){

            if (element.IsMatrix){
                for (uint32_t i = 0; i < element.Amount; i++){
                    glEnableVertexAttribArray(m_VertexBufferIndex);
                    glVertexAttribPointer(m_VertexBufferIndex, 
                        element.Amount,
                        element.BaseType,
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        layout.GetStride(),
                        (const void*) (element.Offset + sizeof(float) * element.Amount * i)
                    );
                    glVertexAttribDivisor(m_VertexBufferIndex, 1);
                    m_VertexBufferIndex++;
                }
            }
            else if (element.BaseType == GL_FLOAT){
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribPointer(m_VertexBufferIndex, 
                    element.Amount,
                    element.BaseType,
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*) (element.Offset)
                );
                m_VertexBufferIndex++;
            }
            else { // GL_INT or GL_BOOL
                glEnableVertexAttribArray(m_VertexBufferIndex);
                glVertexAttribIPointer(m_VertexBufferIndex, 
                    element.Amount,
                    element.BaseType,
                    layout.GetStride(),
                    (const void*) (element.Offset)
                );
                m_VertexBufferIndex++;
            }
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer)
    {
        glBindVertexArray(m_RenderID);
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }

    void VertexArray::DeleteVertexArrays()
    {
        glDeleteVertexArrays(1, &m_RenderID);
    }

}