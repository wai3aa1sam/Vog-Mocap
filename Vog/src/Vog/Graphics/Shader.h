#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"
#include "Vog/Math/MyMath.h"

#include "Vog/Graphics/ShaderLayout.h"

namespace vog {

	class VOG_API Shader : public NonCopyable
	{
	public:
		using MaterialSameShaderReloadCallBackFn = std::function<void(Shader*)>;

		Shader() = default;
		virtual ~Shader() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void reload(const std::string& filepath_ = std::string{}) = 0;
		//virtual ShaderDataList reflect() = 0;

		virtual void setIsReloaded(bool isReloaded_) = 0;
		virtual bool getIsReloaded() = 0;
		//virtual void setMaterialSameShaderReload(const MaterialSameShaderReloadCallBackFn& fp_) = 0;

		virtual void setInt(const std::string& name_, int value_) = 0;
		virtual void setBool(const std::string& name_, bool value_) = 0;

		virtual void setFloat(const std::string& name_, float value_) = 0;
		virtual void setFloat2(const std::string& name_, const Vector2f& value_) = 0;
		virtual void setFloat3(const std::string& name_, const Vector3f& value_) = 0;
		virtual void setFloat4(const std::string& name_, const Vector4f& value_) = 0;

		virtual void setMat3(const std::string& name_, const Matrix3f& value_) = 0;
		virtual void setMat4(const std::string& name_, const Matrix4f& value_) = 0;

		virtual void setIntArray(const std::string& name_, const int* values_, uint32_t count_) = 0;
		virtual void setBoolArray(const std::string& name_, const bool* values_, uint32_t count_) = 0;
		virtual void setFloatArray(const std::string& name_, const float* values_, uint32_t count_) = 0;
		virtual void setFloat2Array(const std::string& name_, const Vector2f* values_, uint32_t count_) = 0;
		virtual void setFloat3Array(const std::string& name_, const Vector3f* values_, uint32_t count_) = 0;
		virtual void setFloat4Array(const std::string& name_, const Vector4f* values_, uint32_t count_) = 0;
		virtual void setMat3Array(const std::string& name_, const Matrix3f* values_, uint32_t count_) = 0;
		virtual void setMat4Array(const std::string& name_, const Matrix4f* values_, uint32_t count_) = 0;

		inline virtual int getUniformLocation(const std::string& name_) const = 0;
		virtual int getBindingLocation(const std::string& name_) const = 0;

		inline virtual RefPtr<ShaderLayout> getShaderLayout() = 0;

		inline virtual const std::string& getName() const = 0;
		inline virtual uint32_t getID() const = 0;

		static RefPtr<Shader> create(const std::string& name_, const std::string& vertexSrc_, const std::string& fragSrc_);
		static RefPtr<Shader> create(const std::string& filepath_);

	private:
	};

	class VOG_API ShaderLibrary : public NonCopyable
	{
		friend class AssetManager;
	public:
		ShaderLibrary() = default;
		~ShaderLibrary() = default;

		void destroy();

		void reload();

		void add(const std::string& name_, const RefPtr<Shader>& pShader_);
		void add(const RefPtr<Shader>& pShader_);

		RefPtr<Shader>& load(const std::string& filepath_);
		RefPtr<Shader>& load(const std::string& name_, const std::string& filepath_);

		void endScene();	// currently reset all shader isReloaded to false;
		
		inline RefPtr<Shader>& getShader(const std::string& name_)
		{
			if (isExist(name_))
				return m_shaderMap.at(name_);
			VOG_CORE_LOG_CRITICAL("Shader: {0}, do not exit!", name_);
			return m_shaderMap.at("Default");
		}
		inline RefPtr<Shader> operator[](const std::string& name_) { return getShader(name_); }
		inline bool isExist(const std::string& name_) const { return m_shaderMap.find(name_) != m_shaderMap.end(); }

	private:
		std::unordered_map<std::string, RefPtr<Shader>> m_shaderMap;
	};
}