#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

namespace vog {

	static constexpr int s_cubemap_faces_count = 6;

	enum class TextureParameter
	{
		None = 0,
		Nearest, Linear, Linear_Mipmap_Linear,
		Repeat, Clamp_To_Edge, Clamp_To_Border,
	};

	struct VOG_API TextureSpecification
	{
	public:
		//TextureSpecification()
		//	:
		//	minFilter(TextureParameter::Linear), magFilter(TextureParameter::Nearest),
		//	wrap_s(TextureParameter::Clamp_To_Edge), wrap_t(TextureParameter::Clamp_To_Edge), wrap_r(TextureParameter::Clamp_To_Edge)
		//{}
	public:
		TextureParameter minFilter = TextureParameter::Linear_Mipmap_Linear;
		TextureParameter magFilter = TextureParameter::Linear;					// cannot use Linear_Mipmap_Linear
		TextureParameter wrap_s = TextureParameter::Repeat;
		TextureParameter wrap_t = TextureParameter::Repeat;
		TextureParameter wrap_r = TextureParameter::Repeat;
	};

	class VOG_API Texture : public NonCopyable
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;

		virtual void bind(uint32_t slot_ = 0) = 0;
		virtual void setData(void* pData_, size_t size_) = 0;

		virtual void unbind() = 0;

		virtual inline uint32_t getWidth() const = 0;
		virtual inline uint32_t getHeight() const = 0;
		virtual inline uint32_t getRendererID() const = 0;

	private:
	};

	class VOG_API Texture2D : public Texture
	{
	public:
		Texture2D() = default;
		virtual ~Texture2D() = default;

		static RefPtr<Texture2D> create(uint32_t width_, uint32_t height_, const TextureSpecification& specification_ = {});
		static RefPtr<Texture2D> create(const std::string& path_, const TextureSpecification& specification_ = {});

		static void bind(uint32_t rendererID_, uint32_t slot_ = 0);
	private:
	};

	class VOG_API Cubemap : public Texture
	{
	public:
		Cubemap() = default;
		virtual ~Cubemap() = default;

		static RefPtr<Cubemap> create(uint32_t size_, const TextureSpecification & specification_ = {});
		static RefPtr<Cubemap> create(const std::string & fileDirectory_, const TextureSpecification & specification_ = {});
	private:
	};
}