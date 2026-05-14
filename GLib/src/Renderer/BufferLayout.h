#pragma once

#include <initializer_list>
#include <vector>
#include <string>
#include <iostream>

typedef unsigned int GLuint;

namespace GLib{

    struct BufferElement{
        std::string Name;
        GLuint BaseType;  // GL_INT, GL_FLOAT or GL_BOOL
        uint32_t Size;
        uint32_t Amount;    // 1, 2, 3 or 4   for example 3 for mat3, 4 for vec4, 1 for float
        size_t Offset;
        bool IsMatrix;
        bool Normalized;

        BufferElement() = default;
        BufferElement(const std::string& name, GLuint base_type, uint32_t amount, bool is_matrix = false, bool normalized = false);
    };

    class BufferLayout {
    public:
        BufferLayout() {}

        BufferLayout(std::initializer_list<BufferElement> elements)
            : m_Elements(elements)
        {
            CalculateOffsetAndStride();
        }

        uint32_t GetStride() const { return m_Stride; }
        const std::vector<BufferElement>& GetElements() const { return m_Elements; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

    private:
        void CalculateOffsetAndStride(){
            size_t offset = 0;
            m_Stride = 0;
            for(auto& element : m_Elements){
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }
    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };

}