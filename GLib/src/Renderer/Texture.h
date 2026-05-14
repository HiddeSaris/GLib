#pragma once

#include <utility>
#include <string>


namespace GLib{

    struct TextureSpecification {
        uint32_t Width = 1;
        uint32_t Height = 1;
        uint32_t DataFormat = 0x1908; //GL_RGBA;
        uint32_t InternalFormat = 0x8058; //GL_RGBA8;
        bool GenerateMips = true;
    };

    class Texture {
    public:
        Texture(const TextureSpecification& specification);
        Texture(const std::string& path);
        ~Texture();

        const TextureSpecification& GetSpecification() const { return m_Specification; }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetRendererID() const { return m_RendererID; }

        const std::string& GetPath() const { return m_Path; }

        void SetData(void* data, uint32_t size);

        void Bind(uint32_t slot = 0) const;

        bool IsLoaded() const { return m_IsLoaded; }

        bool operator==(const Texture& other) const {
            return m_RendererID == other.GetRendererID();
        }
    private:
        TextureSpecification m_Specification;

        std::string m_Path;
        bool m_IsLoaded = false;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        uint32_t m_InternalFormat, m_DataFormat;
    };

}