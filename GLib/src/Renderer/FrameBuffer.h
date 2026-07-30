#pragma once

#include <utility>

namespace GLib {

    // enum class FrambufferTextureFormat 
    // {
    //     None = 0,

    //     RGBA8,

    //     DEPTH24STELCIL8,

    //     Depth = DEPTH24STELCIL8,
    // };

    class FrameBuffer {
    public:
        FrameBuffer(uint32_t width, uint32_t height, uint32_t samples = 1, bool swapChainTarget = false);
        ~FrameBuffer();

        void Bind();
        void Unbind();

        void Invalidate();
        void Resize(uint32_t width, uint32_t height);

        uint32_t GetColorAttachmentID() const { return m_ColorAttachmentID; }
        uint32_t GetDepthAttachmentID() const { return m_DepthAttachmentID; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        uint32_t GetSamples() const { return m_Samples; }
        bool GetSwapChainTarget() const { return m_SwapChainTarget; }
    private:
        uint32_t m_RendererID = 0;
        uint32_t m_ColorAttachmentID = 0, m_DepthAttachmentID = 0;

        uint32_t m_Width, m_Height;
        uint32_t m_Samples = 1;

        bool m_SwapChainTarget = false;
    };

}