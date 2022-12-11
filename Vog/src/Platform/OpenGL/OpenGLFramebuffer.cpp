#include "vogpch.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/OpenGL/OpenGLDefines.h"
#include "Platform/OpenGL/OpenGLUtils.h"

namespace vog {
    // should query the gpu to get this value
    static const uint32_t s_maxFramebufferSize = 8192;
    OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& specification_)
        :
        m_specification(specification_)
    {
        for (auto& attachment : m_specification.attachment.attachments)
        {
            if (Utils::isDepthTextureFormat(attachment.textureFormat))
                m_depthSpecification = attachment.textureFormat;
            else
                m_colorAttachmentSpecifications.push_back(attachment.textureFormat);
        }

        _invalidate();
    }

    OpenGLFramebuffer::~OpenGLFramebuffer()
    {
        if (m_rendererID)
        {
            glDebugCall(glDeleteFramebuffers(1, &m_rendererID));
            glDebugCall(glDeleteTextures(1, &m_depthAttachmentID));
            glDebugCall(glDeleteTextures((GLsizei)m_colorAttachmentIDs.size(), m_colorAttachmentIDs.data()));
        }
    }

    void OpenGLFramebuffer::bind()
    {
        glDebugCall(glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID));
        glDebugCall(glViewport(0, 0, m_specification.width, m_specification.height));
    }

    void OpenGLFramebuffer::unbind()
    {
        glDebugCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void OpenGLFramebuffer::resize(uint32_t width_, uint32_t height_)
    {
        if (width_ <= 0 || height_ <= 0 || width_ > s_maxFramebufferSize || height_ > s_maxFramebufferSize)
        {
            VOG_CORE_LOG_WARN("Attempted to resize framebuffer to {0}, {1}", width_, height_);
            return;
        }
        m_specification.width = width_;
        m_specification.height = height_;

        _invalidate();
    }

    void OpenGLFramebuffer::blit(uint32_t dst_rendererID_, uint32_t dst_Width_, uint32_t dst_height_, BufferBitType type_)
    {
        VOG_CORE_ASSERT(type_ != BufferBitType::Color, "not yet done others param!");

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_rendererID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst_rendererID_);

        GLenum bufferBitType = Utils::toOpenGLBufferBitType(type_);
        GLenum filter;
        if (bufferBitType == GL_DEPTH_BUFFER_BIT)
            filter = GL_NEAREST;
        else
            filter = GL_LINEAR;

        glBlitFramebuffer(0, 0, m_specification.width, m_specification.height, 0, 0, dst_Width_, dst_height_, Utils::toOpenGLBufferBitType(type_), filter);
    }

    void OpenGLFramebuffer::_invalidate()
    {
        if (m_rendererID)
        {
            glDebugCall(glDeleteFramebuffers(1, &m_rendererID));
            glDebugCall(glDeleteTextures(1, &m_depthAttachmentID));
            glDebugCall(glDeleteTextures((GLsizei)m_colorAttachmentIDs.size(), m_colorAttachmentIDs.data()));
        }

        glDebugCall(glCreateFramebuffers(1, &m_rendererID));
        glDebugCall(glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID));

        int samples = m_specification.samples;
        bool isMultiSampled = m_specification.samples > 1;
        size_t colorAttachmentSize = m_colorAttachmentSpecifications.size();

        uint32_t width = m_specification.width;
        uint32_t height = m_specification.height;

        if (colorAttachmentSize)
        {
            m_colorAttachmentIDs.resize(colorAttachmentSize);
            Utils::createTextures(isMultiSampled, m_colorAttachmentIDs.data(), colorAttachmentSize);

            // create color attachments
            for (uint32_t i = 0; i < colorAttachmentSize; i++)
            {
                VOG_CORE_ASSERT(!Utils::isDepthTextureFormat(m_colorAttachmentSpecifications[i].textureFormat), "");

                uint32_t rendererID = m_colorAttachmentIDs[i];
                Utils::bindTexture(isMultiSampled, rendererID);
                switch (m_colorAttachmentSpecifications[i].textureFormat)
                {
                    case FramebufferTextureFormat::RGBA8:           
                        Utils::attachColorTexture(m_colorAttachmentIDs[i], samples, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, width, height, i);             break;
                    case FramebufferTextureFormat::RGBA16F:
                        Utils::attachColorTexture(m_colorAttachmentIDs[i], samples, GL_RGBA16F, GL_RGBA, GL_FLOAT, width, height, i);                   break;
                    case FramebufferTextureFormat::RED_INTEGER:     
                        Utils::attachColorTexture(m_colorAttachmentIDs[i], samples, GL_R32I, GL_RED_INTEGER, GL_UNSIGNED_BYTE, width, height, i);       break;

                    assert(0);
                }
            }
        }

        if (m_depthSpecification.textureFormat != FramebufferTextureFormat::None)
        {
            VOG_CORE_ASSERT(Utils::isDepthTextureFormat(m_depthSpecification.textureFormat), "");

            Utils::createTextures(isMultiSampled, &m_depthAttachmentID, 1);
            VOG_CORE_ASSERT(m_depthAttachmentID != 0, "depth attachment is invalid");

            uint32_t rendererID = m_depthAttachmentID;
            Utils::bindTexture(isMultiSampled, rendererID);
            switch (m_depthSpecification.textureFormat)
            {
                case FramebufferTextureFormat::DEPTH24STENCIL8:    
                    Utils::attachDepthTexture(rendererID, samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL_ATTACHMENT, width, height);    
                    break;
                case FramebufferTextureFormat::DEPTH32F:
                    Utils::attachDepthTexture(rendererID, samples, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT, width, height);
                    break;
                assert(0);
            }
        }

        if (colorAttachmentSize > 1)
        {
            VOG_CORE_ASSERT(colorAttachmentSize <= 4, "max 4 color texture now");
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDebugCall(glDrawBuffers((GLsizei)colorAttachmentSize, buffers));
        }
        else if (colorAttachmentSize == 0)
        {
            glDrawBuffer(GL_NONE);
        }

        VOG_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");


        glDebugCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}