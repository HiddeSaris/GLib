#pragma once

#include <utility>
#include <string>
#include <vector>
#include <memory>

// froward declaration without including stbi_image
extern "C" void stbi_image_free(void* retval_from_stbi_load);

namespace GLib{

    enum class TextureType {
        None,
        Diffuse,
        Specular,
        Albedo,
        Normal,
        Metallic,
        Roughness,
        AmbientOcclusion,
    };

    struct TextureSpecification {
        TextureSpecification() = default;
        TextureSpecification(uint32_t width, uint32_t height, uint32_t dataFormat, uint32_t internalFormat, TextureType type, bool generateMips)
            : Width(width), Height(height), DataFormat(dataFormat), InternalFormat(internalFormat), Type(type), GenerateMips(generateMips)
        {}

        uint32_t Width = 1;
        uint32_t Height = 1;
        uint32_t DataFormat = 0x1908; //GL_RGBA;
        uint32_t InternalFormat = 0x8058; //GL_RGBA8;
        TextureType Type = TextureType::Diffuse;
        bool GenerateMips = true;
    };

    class Texture {
    public:
        struct PixelData {
            std::unique_ptr<uint8_t[], void(*)(void*)> Data{ nullptr, stbi_image_free };
            int Width = 0, Height = 0, Channels = 0;
            bool IsTransparent = false;
            bool IsLoaded = false;
        };

        Texture(const TextureSpecification& specification);
        Texture(const std::string& path, TextureType type);
        Texture(std::string path, TextureType type, const PixelData& pixels);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        const TextureSpecification& GetSpecification() const { return m_Specification; }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetRendererID() const { return m_RendererID; }
        TextureType GetType() const { return m_Type; }

        const std::string& GetPath() const { return m_Path; }

        void SetData(void* data, uint32_t size);
        //void AttachToFramebuffer();

        void Bind(uint32_t slot = 0) const;

        static void LoadPixelData(const std::string& path, TextureType type, PixelData& out);

        bool IsLoaded() const { return m_IsLoaded; }
        bool IsTransparent() const { return m_IsTransparent; }

        bool operator==(const Texture& other) const {
            return m_RendererID == other.GetRendererID();
        }
    private:
        TextureSpecification m_Specification;
        TextureType m_Type = TextureType::Albedo;

        std::string m_Path;
        bool m_IsLoaded = false;
        bool m_IsTransparent = false;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        uint32_t m_InternalFormat, m_DataFormat;
    };

}