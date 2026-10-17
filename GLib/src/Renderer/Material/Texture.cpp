#include "Texture.h"

#include "Window/Window.h"

#include <iostream>

#include <glad/glad.h>
#include <stb_image.h>

namespace GLib {

    Texture::Texture(const TextureSpecification &specification)
        : m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height),
        m_DataFormat(m_Specification.DataFormat), m_InternalFormat(m_Specification.InternalFormat)
    {
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            m_InternalFormat,
            m_Width,
            m_Height,
            0,
            m_DataFormat,
            GL_UNSIGNED_BYTE,
            nullptr
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        CheckGLError("Texture");
    }

    Texture::Texture(const std::string &path)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(0);
        stbi_uc* data = nullptr;
        data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (data) {
            m_Width = width;
            m_Height = height;

            GLenum internalFormat = 0, dataFormat = 0;
            if (channels == 4) {
                internalFormat = GL_RGBA8;
                dataFormat = GL_RGBA;
            }
            else if (channels == 3) {
                internalFormat = GL_RGB8;
                dataFormat = GL_RGB;
            }

            m_InternalFormat = internalFormat;
            m_DataFormat = dataFormat;

            if (internalFormat == 0 || dataFormat == 0) {
                std::cout << "ERROR [Texture]: Image format not supported\n";
                exit(1);
            }

            glGenTextures(1, &m_RendererID);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

            CheckGLError("Texture");

            stbi_image_free(data);
        }
        else {
            std::cout << "ERROR [Texture]: Failed to load texture: " << path << std::endl;
        }
    }

    Texture::Texture(const PixelData& pixels) {
        m_Width = pixels.Width;
        m_Height = pixels.Height;
        m_InternalFormat = pixels.Channels == 4 ? GL_RGBA8 : GL_RGB8;
        m_DataFormat = pixels.Channels == 4 ? GL_RGBA : GL_RGB;

        if (m_InternalFormat == 0 || m_DataFormat == 0) {
            std::cout << "ERROR [Texture]: Image format not supported\n";
        }
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, pixels.Data.get());
    }

    Texture::~Texture()
    {
        if (m_RendererID != 0)
            glDeleteTextures(1, &m_RendererID);
    }

    Texture::Texture(Texture&& other) noexcept
        : m_Specification(other.m_Specification),
          m_Width(other.m_Width), m_Height(other.m_Height),
          m_RendererID(other.m_RendererID),
          m_InternalFormat(other.m_InternalFormat), m_DataFormat(other.m_DataFormat)
    {
        other.m_RendererID = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other) {
            if (m_RendererID != 0)
                glDeleteTextures(1, &m_RendererID);

            m_Specification = other.m_Specification;
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            m_RendererID = other.m_RendererID;
            m_InternalFormat = other.m_InternalFormat;
            m_DataFormat = other.m_DataFormat;

            other.m_RendererID = 0;
        }
        return *this;
    }

    void Texture::SetData(void *data, uint32_t size)
    {
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        if (size != m_Width * m_Height * bpp) {
            std::cout << "ERROR [Texture::SetData]: Image data must be entire texture!";
            exit(1);
        }
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }

    void Texture::Bind(uint32_t slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void Texture::LoadPixelData(const std::filesystem::path& path, TextureType type, PixelData& out) {
        stbi_set_flip_vertically_on_load(0); // dont flip because of thread safety
        int width, height, channels;
        stbi_uc* data = nullptr;
        data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

        if (!data) {
            std::cout << "ERROR [Texture::LoadPixelData]: Failed to load texture: " << path << std::endl;
            return;
        }

        out.Width = width;
        out.Height = height;
        out.Channels = channels;
        out.Data.reset(data);

        if (channels == 4 && (type == TextureType::Diffuse || type == TextureType::Albedo)) {
            for (int i = 0; i < width * height; i++) {
                if (out.Data[i * 4 + 3] < 250) {  // small threshold avoids false positives from lossy compression artifacts
                    out.IsTransparent = true;
                    break;
                }
            }
        }

        out.IsLoaded = true;
    }

    std::shared_ptr<Texture> Texture::WhiteTexture() {
        static std::shared_ptr<Texture> texture = [] {
            auto tex = std::make_shared<Texture>(TextureSpecification());

            uint8_t white[] = { 255, 255, 255, 255 };
            tex->SetData(white, sizeof(white));
            return tex;
        }();
        return texture;
    }

    std::shared_ptr<Texture> Texture::BlackTexture() {
        static std::shared_ptr<Texture> texture = [] {
            auto tex = std::make_shared<Texture>(TextureSpecification());

            uint8_t black[] = { 0, 0, 0, 255 };
            tex->SetData(black, sizeof(black));
            return tex;
        }();
        return texture;
    }

    std::shared_ptr<Texture> Texture::DefaultNormalTexture() {
        static std::shared_ptr<Texture> texture = [] {
            auto tex = std::make_shared<Texture>(TextureSpecification());

            uint8_t normal[] = { 128, 128, 255, 255 };
            tex->SetData(normal, sizeof(normal));
            return tex;
        }();
        return texture;
    }
}