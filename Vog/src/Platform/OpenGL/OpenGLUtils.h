#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/GraphicsDefines.h"

#include "Vog/Graphics/Framebuffer.h"
#include "Vog/Graphics/Shader.h"

#include <glad/glad.h>

namespace vog {

namespace Utils
{
	GLenum toOpenGLBufferBitType(BufferBitType bufferBitType_);
	GLenum bitConcatOpenGLBufferBitType(BufferBitType bufferBitType_);
	GLenum toOpenGLPrimitive(Primitive primitive_);

	GLenum toOpenGLBaseDataType(VertexDataType type_);

    bool isDepthTextureFormat(FramebufferTextureFormat textureFormat_);

    //GLenum toOpenGLFramebufferTextureFormat(FramebufferTextureFormat textureFormat_);

    GLenum toTextureTarget(bool isMultiSampled_);
    void createTextures(bool isMultiSampled_, uint32_t* rendererIDs_, size_t count_);
    void bindTexture(bool isMultiSampled_, uint32_t rendererID_);
    void attachColorTexture(uint32_t rendererID_, int samples_, GLenum internalFormat_, GLenum format_, GLenum dataType_, uint32_t width_, uint32_t height_, int attachmentIndex_);
    void attachDepthTexture(uint32_t rendererID_, int samples_, GLenum internalFormat_, GLenum format_, GLenum dataType_, GLenum attachmentType_, uint32_t width_, uint32_t height_);

	GLenum getShaderType(const std::string& token_);
    ShaderDataType getShaderDataType(GLenum type_);

	GLenum toOpenGLTextureParameter(TextureParameter parameter_);
	GLenum toOpenGLInternalFormat(int channels_);
	GLenum toOpenGLDataFormat(int channels_);
	GLenum toOpenGLCubeMapFace(const std::string& filename_);

	GLenum toOpenGLCullFaceType(CullFaceType type_);
	GLenum toOpenGLDepthFuncConfig(DepthFuncConfig config_);
}
}