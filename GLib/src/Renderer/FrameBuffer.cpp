#include "FrameBuffer.h"

#include "glad/glad.h"

#include "iostream"

namespace GLib {

    FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, uint32_t samples, bool swapChainTarget)
        : m_Width(width), m_Height(height), m_Samples(samples), m_SwapChainTarget(swapChainTarget)
    {
        Invalidate();
    }

    FrameBuffer::~FrameBuffer() {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(1, &m_ColorAttachmentID);
        glDeleteTextures(1, &m_DepthAttachmentID);
    }

    void FrameBuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Width, m_Height);
    }

    void FrameBuffer::Unbind(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Invalidate() {
        if (m_RendererID) {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(1, &m_ColorAttachmentID);
            glDeleteTextures(1, &m_DepthAttachmentID);
        }

        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        glGenTextures(1, &m_ColorAttachmentID);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentID, 0);
        
        glGenTextures(1, &m_DepthAttachmentID);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentID, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            std::cout << "Error [FrameBuffer::Invalidate]: Framebuffer is incomplete.\n";
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Resize(uint32_t width, uint32_t height) {
        m_Width = width;
        m_Height = height;

        Invalidate();
    }

}