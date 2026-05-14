#pragma once

#include "BufferLayout.h"

#include <cstdint>

namespace GLib {

    class VertexBuffer{
    public:
        VertexBuffer(size_t size);
        VertexBuffer(float* vertices, size_t size);
        ~VertexBuffer();

        void Bind();
        void Unbind();

        void SetData(const void* data, size_t size);

        const BufferLayout& GetLayout() const { return m_Layout; }
        void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
    private:
        uint32_t m_RendererID;
        BufferLayout m_Layout;
    };



    class IndexBuffer{
    public:
        IndexBuffer(uint32_t* indices, uint32_t count);
        ~IndexBuffer();

        void Bind();
        void Unbind();

        uint32_t GetCount() const { return m_Count; }
    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };
}