#include "vogpch.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL/OpenGLDefines.h"
#include "Platform/OpenGL/OpenGLUtils.h"

#include <stb/stb_image.h>

// References
// mipmap: http://www.xphere.me/2020/06/mipmapping-effects-of-not-having-it-and-how-to-setup-in-opengl-4-5/

namespace vog {

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width_, uint32_t height_, const TextureSpecification& specification_)
		:
		m_width(width_), m_height(height_), m_internalFormat(GL_RGBA8), m_dataFormat(GL_RGBA)
	{
		glDebugCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererId));
		glDebugCall(glTextureStorage2D(m_rendererId, 1, m_internalFormat, m_width, m_height));

		glDebugCall(glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, Utils::toOpenGLTextureParameter(specification_.minFilter)));
		glDebugCall(glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, Utils::toOpenGLTextureParameter(specification_.magFilter)));

		glDebugCall(glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, Utils::toOpenGLTextureParameter(specification_.wrap_s)));
		glDebugCall(glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, Utils::toOpenGLTextureParameter(specification_.wrap_t)));

		//glDebugCall(glTextureSubImage2D(m_rendererId, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, nullptr));		// navdia don't like this
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath_, const TextureSpecification& specification_)
	{
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(filepath_.c_str(), &width, &height, &channels, 0);
        VOG_CORE_ASSERT(data, "Failed to load image!");		// should load default image if no data presented
        m_width = width;
        m_height = height;

		GLenum internalFormat = Utils::toOpenGLInternalFormat(channels);
		GLenum dataFormat = Utils::toOpenGLDataFormat(channels);

		VOG_CORE_ASSERT(internalFormat != 0 && dataFormat != 0, " invalid channels!");

		m_internalFormat = internalFormat;
		m_dataFormat = dataFormat;

		GLsizei mipmapLevels = static_cast<GLsizei>(floor(log2(std::max(width, height))));
		if (mipmapLevels == 0)
		{
			mipmapLevels = 1;
		}

        glDebugCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererId));

		glBindTexture(GL_TEXTURE_2D, m_rendererId);

        glDebugCall(glTextureStorage2D(m_rendererId, mipmapLevels, internalFormat, m_width, m_height));

        glDebugCall(glTextureParameteri(m_rendererId, GL_TEXTURE_MIN_FILTER, Utils::toOpenGLTextureParameter(specification_.minFilter)));
        glDebugCall(glTextureParameteri(m_rendererId, GL_TEXTURE_MAG_FILTER, Utils::toOpenGLTextureParameter(specification_.magFilter)));

		GLenum warp_s = Utils::toOpenGLTextureParameter(specification_.wrap_s);
		GLenum warp_t = Utils::toOpenGLTextureParameter(specification_.wrap_t);

		if (warp_s == GL_CLAMP_TO_BORDER || warp_t == GL_CLAMP_TO_BORDER)
		{
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		}

        glDebugCall(glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_S, warp_s));
        glDebugCall(glTextureParameteri(m_rendererId, GL_TEXTURE_WRAP_T, warp_t));

		glDebugCall(glTextureSubImage2D(m_rendererId, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data));

		if (mipmapLevels > 1)
		{
			glDebugCall(glGenerateTextureMipmap(m_rendererId));  //Generate num_mipmaps number of mipmaps here.
		}

		//if (mipmapLevels == 1)
		//	mem = (float)(m_width * m_height * channels) / 1048576.0f;					// Calculate the texture mem (in mb)
		//else
		//	mem = (float)(m_width * m_height * channels * 1.33f) / 1048576.0f;			// Calculate the texture mem (in mb) for mipmaps

        stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDebugCall(glDeleteTextures(1, &m_rendererId));
	}

	void OpenGLTexture2D::bind(uint32_t slot_)
	{
		glDebugCall(glBindTextureUnit(slot_, m_rendererId));
		
		// eqv to
		//glActiveTexture(GL_TEXTURE0 + slot_);
		//glBindTexture(GL_TEXTURE_2D, m_rendererId);
	}

	void OpenGLTexture2D::setData(void* pData_, size_t size_)
	{
		uint32_t bytePerPixel = (m_dataFormat == GL_RGBA) ? 4 : 3;
		VOG_CORE_ASSERT(size_ == m_height * m_width * bytePerPixel, "Wrong texture data buffer size!");
		glDebugCall(glTextureSubImage2D(m_rendererId, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, pData_));
	}

	void OpenGLTexture2D::unbind()
	{
		glDebugCall(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void OpenGLTexture2D::bind(uint32_t rendererID_, uint32_t slot_)
	{
		glDebugCall(glBindTextureUnit(slot_, rendererID_));
	}

#pragma region OpenGLCubemap_impl

	OpenGLCubemap::OpenGLCubemap(uint32_t size_, const TextureSpecification& specification_)
	{
		VOG_CORE_ASSERT(0, "not yet done internal format stuff!");

		m_size = size_;

		glDebugCall(glGenTextures(1, &m_rendererId));
		glDebugCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererId));

		for (int i = 0; i < s_cubemap_faces_count; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, m_size, m_size, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		}

		glDebugCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, Utils::toOpenGLTextureParameter(specification_.minFilter)));
		glDebugCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, Utils::toOpenGLTextureParameter(specification_.magFilter)));
		glDebugCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, Utils::toOpenGLTextureParameter(specification_.wrap_s)));
		glDebugCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, Utils::toOpenGLTextureParameter(specification_.wrap_t)));
		glDebugCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, Utils::toOpenGLTextureParameter(specification_.wrap_r)));

		glDebugCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

		glDebugCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

		m_size = size_;
		//m_internalFormat = internalFormat;
		//m_dataFormat = dataFormat;
	}

	OpenGLCubemap::OpenGLCubemap(const std::filesystem::path& fileDirectory_, const TextureSpecification& specification_)
	{
		VOG_CORE_ASSERT(std::filesystem::exists(fileDirectory_), "");

		int width, height, channels;
		GLenum internalFormat = 0, dataFormat = 0;
		
		// get all files
		std::filesystem::path filepaths[s_cubemap_faces_count];
		int index = 0;
		for (auto& directoryEntry : std::filesystem::directory_iterator(fileDirectory_))
		{
			VOG_CORE_ASSERT(index < 6, "there should not be more than 6 photos for cubemap!");
			const auto& path = directoryEntry.path();
			//VOG_CORE_LOG_INFO("{0}", path.string());
			filepaths[index] = path;
			index++;
		}

		stbi_set_flip_vertically_on_load(0);

		glDebugCall(glGenTextures(1, &m_rendererId));
		glDebugCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererId));

		int lastChannels = 0;		// sanity check

		for (int i = 0; i < s_cubemap_faces_count; i++)
		{
			stbi_uc* data = stbi_load(filepaths[i].string().c_str(), &width, &height, &channels, 0);

			if (i == 0)
				lastChannels = channels;

			VOG_CORE_ASSERT(width == height, "");
			VOG_CORE_ASSERT(lastChannels == channels, "");

			internalFormat = Utils::toOpenGLInternalFormat(channels);
			dataFormat = Utils::toOpenGLDataFormat(channels);
			if (data)
			{
				GLenum cubmapFace = Utils::toOpenGLCubeMapFace(filepaths[i].stem().string());
				glTexImage2D(cubmapFace,
					0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
				lastChannels = channels;
			}
			else
			{
				VOG_CORE_LOG_ERROR("failed to load cube map, filename: {0}", filepaths[i]);
			}
			stbi_image_free(data);
		}

		glDebugCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, Utils::toOpenGLTextureParameter(specification_.minFilter)));
		glDebugCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, Utils::toOpenGLTextureParameter(specification_.magFilter)));
		glDebugCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, Utils::toOpenGLTextureParameter(specification_.wrap_s)));
		glDebugCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, Utils::toOpenGLTextureParameter(specification_.wrap_t)));
		glDebugCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, Utils::toOpenGLTextureParameter(specification_.wrap_r)));

		glDebugCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));

		glDebugCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

		m_size = width;
		m_internalFormat = internalFormat;
		m_dataFormat = dataFormat;
	}

	OpenGLCubemap::~OpenGLCubemap()
	{
		if (m_rendererId)
		{
			glDebugCall(glDeleteTextures(1, &m_rendererId));
		}
	}

	void OpenGLCubemap::bind(uint32_t slot_)
	{
		glDebugCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererId));
	}

	void OpenGLCubemap::setData(void* pData_, size_t size_)
	{
		VOG_CORE_ASSERT(0, "not yet done!");
	}

	void OpenGLCubemap::unbind()
	{
		glDebugCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
	}
#pragma endregion
}