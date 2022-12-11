#include "vogpch.h"
#include "Platform/OpenGL/OpenGLUtils.h"
#include "Platform/OpenGL/OpenGLDefines.h"

namespace vog {
namespace Utils
{
	GLenum toOpenGLBufferBitType(BufferBitType bufferBitType_)
	{
		switch (bufferBitType_)
		{
			case BufferBitType::Color:		return GL_COLOR_BUFFER_BIT;
			case BufferBitType::Depth:		return GL_DEPTH_BUFFER_BIT;
			case BufferBitType::Stencil:	return GL_STENCIL_BUFFER_BIT;
		default: break;
		}
		VOG_CORE_ASSERT(0, "Invalid Buffer Bit Type!");
		return 0;
	}

	GLenum bitConcatOpenGLBufferBitType(BufferBitType bufferBitType_)
	{
		GLenum result = 0;
		uint32_t bit = 0x1;
		if ((bufferBitType_ & bit) == bit) { result |= GL_COLOR_BUFFER_BIT; }	bit = bit << 1;
		if ((bufferBitType_ & bit) == bit) { result |= GL_DEPTH_BUFFER_BIT; }	bit = bit << 1;
		if ((bufferBitType_ & bit) == bit) { result |= GL_STENCIL_BUFFER_BIT; } bit = bit << 1;
		return result;
	}

	GLenum toOpenGLPrimitive(Primitive primitive_)
	{
		switch (primitive_)
		{
			case Primitive::Triangle:	return GL_TRIANGLES;
			case Primitive::Line:		return GL_LINES;
			case Primitive::Point:		return GL_POINTS;
		default: break;
		}
		VOG_CORE_ASSERT(0, "Invalid Primitive!");
		return 0;
	}

	GLenum toOpenGLBaseDataType(VertexDataType type_)
	{
		switch (type_)
		{
			case VertexDataType::Int:		return GL_INT;
			case VertexDataType::Int2:		return GL_INT;
			case VertexDataType::Int3:		return GL_INT;
			case VertexDataType::Int4:		return GL_INT;
			case VertexDataType::Float:		return GL_FLOAT;
			case VertexDataType::Float2:	return GL_FLOAT;
			case VertexDataType::Float3:	return GL_FLOAT;
			case VertexDataType::Float4:	return GL_FLOAT;
			case VertexDataType::Mat3:		return GL_FLOAT;
			case VertexDataType::Mat4:		return GL_FLOAT;
			case VertexDataType::Bool:		return GL_BOOL;
		default: break;
		}
		VOG_CORE_ASSERT(0, "Invalid shader data type!");
		return 0;
	}

	bool isDepthTextureFormat(FramebufferTextureFormat textureFormat_)
	{
		switch (textureFormat_)
		{
			case FramebufferTextureFormat::DEPTH24STENCIL8:     return true;
			case FramebufferTextureFormat::DEPTH32F:			return true;
		}
		return false;
	}

	GLenum toOpenGLFramebufferTextureFormat(FramebufferTextureFormat textureFormat_)
	{
		switch (textureFormat_)
		{
			case FramebufferTextureFormat::RGBA8:               return GL_RGBA8;
			case FramebufferTextureFormat::RGBA16F:             return GL_RGBA16F;
			case FramebufferTextureFormat::RED_INTEGER:         return GL_RED_INTEGER;
			case FramebufferTextureFormat::DEPTH24STENCIL8:     return GL_DEPTH24_STENCIL8;
			case FramebufferTextureFormat::DEPTH32F:			return GL_DEPTH_COMPONENT32F;
		}
		VOG_CORE_ASSERT(false, "");
		return 0;
	}

	GLenum toTextureTarget(bool isMultiSampled_)
	{
		return isMultiSampled_ ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	void createTextures(bool isMultiSampled_, uint32_t* rendererIDs_, size_t count_)
	{
		glDebugCall(glCreateTextures(toTextureTarget(isMultiSampled_), (GLsizei)count_, rendererIDs_));
	}

	void bindTexture(bool isMultiSampled_, uint32_t rendererID_)
	{
		glDebugCall(glBindTexture(toTextureTarget(isMultiSampled_), rendererID_));
	}

	void attachColorTexture(uint32_t rendererID_, int samples_, GLenum internalFormat_, GLenum format_, GLenum dataType_, uint32_t width_, uint32_t height_, int attachmentIndex_)
	{
		bool isMultiSampled = samples_ > 1;
		GLenum target = toTextureTarget(isMultiSampled);
		if (isMultiSampled)
		{
			glDebugCall(glTexImage2DMultisample(target, samples_, internalFormat_, width_, height_, GL_FALSE));
		}
		else
		{
			glDebugCall(glTexImage2D(target, 0 /* use for mipmap */, internalFormat_, width_, height_, 0, format_, dataType_, nullptr));

			glGenerateMipmap(target);  //Generate num_mipmaps number of mipmaps here.

			glDebugCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
			glDebugCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			glDebugCall(glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
			glDebugCall(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			glDebugCall(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		}

		glDebugCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex_, target, rendererID_, 0));
	}

	void attachDepthTexture(uint32_t rendererID_, int samples_, GLenum internalFormat_, GLenum format_, GLenum dataType_, GLenum attachmentType_, uint32_t width_, uint32_t height_)
	{
		bool isMultiSampled = samples_ > 1;
		GLenum target = toTextureTarget(isMultiSampled);
		if (isMultiSampled)
		{
			glDebugCall(glTexImage2DMultisample(target, samples_, internalFormat_, width_, height_, GL_FALSE));
		}
		else
		{
			//glDebugCall(glTexStorage2D(target, 1 /* use for mipmap */, internalFormat_, width_, height_));      // cannot read
			glDebugCall(glTexImage2D(target, 0 /* use for mipmap */, internalFormat_, width_, height_, 0, format_, dataType_, nullptr));

			//glDebugCall(glTexImage2D(target, 0 /* use for mipmap */, GL_DEPTH24_STENCIL8, width_, height_, 0,  GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr));

			glDebugCall(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			glDebugCall(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			glDebugCall(glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER));
			glDebugCall(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
			glDebugCall(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));

			if (true)
			{
				glDebugCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
				float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glDebugCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
			}

		}

		glDebugCall(glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType_, target, rendererID_, 0));
	}

	GLenum getShaderType(const std::string& token_)
	{
		if (token_ == "vertex")								return GL_VERTEX_SHADER;
		if (token_ == "fragment" || token_ == "pixel")		return GL_FRAGMENT_SHADER;
		if (token_ == "geometry")							return GL_GEOMETRY_SHADER;

		VOG_CORE_ASSERT(0, "Unkown shader type!");
		return 0;
	}

	ShaderDataType getShaderDataType(GLenum type_)
	{
		switch (type_)
		{
			case GL_INT:			return ShaderDataType::Int;
			case GL_INT_VEC2:		return ShaderDataType::Int2;
			case GL_INT_VEC3:		return ShaderDataType::Int3;
			case GL_INT_VEC4:		return ShaderDataType::Int4;
			case GL_FLOAT:			return ShaderDataType::Float;
			case GL_FLOAT_VEC2:		return ShaderDataType::Float2;
			case GL_FLOAT_VEC3:		return ShaderDataType::Float3;
			case GL_FLOAT_VEC4:		return ShaderDataType::Float4;
			case GL_FLOAT_MAT3:		return ShaderDataType::Mat3;
			case GL_FLOAT_MAT4:		return ShaderDataType::Mat4;
			case GL_BOOL:			return ShaderDataType::Bool;
			case GL_SAMPLER_2D:		return ShaderDataType::Texture2D;
			case GL_SAMPLER_CUBE:	return ShaderDataType::Cubemap;
		}
		VOG_CORE_ASSERT(0, "Invalid shader data type!");
		return ShaderDataType::None;
	}

	GLenum toOpenGLTextureParameter(TextureParameter parameter_)
	{
		switch (parameter_)
		{
			case TextureParameter::Nearest:					return GL_NEAREST;
			case TextureParameter::Linear:					return GL_LINEAR;
			case TextureParameter::Linear_Mipmap_Linear:	return GL_LINEAR_MIPMAP_LINEAR;
			case TextureParameter::Repeat:					return GL_REPEAT;
			case TextureParameter::Clamp_To_Edge:			return GL_CLAMP_TO_EDGE;
			case TextureParameter::Clamp_To_Border:			return GL_CLAMP_TO_BORDER;

		}
		VOG_CORE_ASSERT(0, "Invalid Texture Parameter!");
		return 0;
	}

	GLenum toOpenGLInternalFormat(int channels_)
	{
		switch (channels_)
		{
			case 1: return GL_R8;
			case 2: return GL_RG8;
			case 3: return GL_RGB8;
			case 4: return GL_RGBA8;
		}
		VOG_CORE_ASSERT(0, "");
		return 0;
	}

	GLenum toOpenGLDataFormat(int channels_)
	{
		switch (channels_)
		{
			case 1: return GL_RED;
			case 2: return GL_RG;
			case 3: return GL_RGB;
			case 4: return GL_RGBA;
		}
		VOG_CORE_ASSERT(0, "");
		return 0;
	}

	GLenum toOpenGLCubeMapFace(const std::string& filename_)
	{
		//static const std::unordered_map<std::string, GLenum>
		if (filename_ == "right")	return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		if (filename_ == "left")	return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		if (filename_ == "top")		return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		if (filename_ == "bottom")	return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		if (filename_ == "back")	return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		if (filename_ == "front")	return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		VOG_CORE_ASSERT(0, "");
		return 0;
	}

	GLenum toOpenGLCullFaceType(CullFaceType type_)
	{
		switch (type_)
		{
			case CullFaceType::Front:			return GL_FRONT;
			case CullFaceType::Back:			return GL_BACK;
			case CullFaceType::Front_And_Back:	return GL_FRONT_AND_BACK;
		}
		VOG_CORE_ASSERT(0, "");
		return 0;
	}

	GLenum toOpenGLDepthFuncConfig(DepthFuncConfig config_)
	{
		switch (config_)
		{
			case DepthFuncConfig::Always:			return GL_ALWAYS;
			case DepthFuncConfig::Never:			return GL_NEVER;
			case DepthFuncConfig::Less:				return GL_LESS;
			case DepthFuncConfig::Less_Equal:		return GL_LEQUAL;
			case DepthFuncConfig::Greater:			return GL_GREATER;
			case DepthFuncConfig::Greater_Equal:	return GL_GEQUAL;
			case DepthFuncConfig::Equal:			return GL_EQUAL;
			case DepthFuncConfig::Not_Equal:		return GL_NOTEQUAL;
		}
		VOG_CORE_ASSERT(0, "");
		return 0;
	}
}
}