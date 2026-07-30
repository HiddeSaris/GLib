#pragma once

#include "Buffer.h"
#include <memory>

namespace GLib {

    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        VertexArray(VertexArray&& other) noexcept;
        VertexArray& operator=(VertexArray&& other) noexcept;

        void Bind() const;
        void Unbind() const;

        void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
        void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

        const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
        const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

    private:
        void DeleteVertexArrays();
    private:
        uint32_t m_RenderID;
        uint32_t m_VertexBufferIndex = 0;
        std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
    };

}