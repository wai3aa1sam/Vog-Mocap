#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Shader.h"
#include "Vog/Graphics/ShaderLayout.h"

#include "Vog/Graphics/Texture.h"

#include <unordered_map>

namespace vog {

	enum class TextureType
	{
		None = 0,
		Albedo, Normal, Specular, Height, Bump
	};

	namespace Utils
	{
		static const char* textureTypeToShaderUniformName(TextureType txtureType_)
		{
			switch (txtureType_)
			{
				case vog::TextureType::Albedo:		return "u_albedo_map";
				case vog::TextureType::Normal:		return "u_normal_map";
				case vog::TextureType::Specular:	return "u_specular_map";
				case vog::TextureType::Height:		return "u_height_map";
				case vog::TextureType::Bump:		return "u_bump_map";
			}
			//VOG_CORE_LOG_ERROR("invaild texture type: {0}", txtureType_);
			return nullptr;
		}
	}

	class VOG_API Material
	{
	public:
		Material() = default;
		explicit Material(const RefPtr<Shader>& pShader_);
		Material(const Material& rhs_);
		void operator=(const Material& rhs_);

		~Material();
		void uploadMaterial();

		void resetShader(const RefPtr<Shader>& pShader_);
		void reload();

		void setFloat(const std::string& name_, float value_);
		//void setFloat2(const std::string& name_, const Vector2f& value_);
		//void setFloat3(const std::string& name_, const Vector3f& value_);
		void setFloat4(const std::string& name_, const Vector4f& value_);
		//void setInt(const std::string& name_, int value_);
		//void setBool(const std::string& name_, bool value_);
		//void setMat3(const std::string& name_, const Matrix3f& value_);
		//void setMat4(const std::string& name_, const Matrix4f& value_);
		void setTexture2D(const std::string& name_, const RefPtr<Texture2D>& value_);
		void setCubemap(const std::string& name_, const RefPtr<Cubemap>& value_);

		void setFloatArray(const std::string& name_, const float* values_);
		//void setFloat2Array(const std::string& name_, const Vector2f* values_);
		//void setFloat3Array(const std::string& name_, const Vector3f* values_);
		void setFloat4Array(const std::string& name_, const Vector4f* values_);
		//void setIntArray(const std::string& name_, const int* values_);
		//void setBoolArray(const std::string& name_, const bool* values_);
		//void setMat3Array(const std::string& name_, const Matrix3f* values_);
		//void setMat4Array(const std::string& name_, const Matrix4f* values_);
		void setTexture2DArray(const std::string& name_, const RefPtr<Texture2D>* values_);
		void setCubemapArray(const std::string& name_, const RefPtr<Cubemap>* values_);

		bool hasTexture(const std::string& name_);

		template<typename T>
		const T& getValue(const std::string& name_) const
		{
			return m_list.getShaderDataValue<T>(name_);
		}

		template<typename T>
		T& getValue(const std::string& name_)
		{
			return m_list.getShaderDataValue<T>(name_);
		}

		template<typename T>
		const T* getValues(const std::string& name_) const
		{
			return m_list.getShaderDataValues<T*>(name_);
		}

		template<typename T>
		T* getValues(const std::string& name_)
		{
			return m_list.getShaderDataValues<T*>(name_);
		}

		//std::unordered_map<TextureType, uint32_t> textureIndexMap;		// TODO: remove, only use for the mesh now

		const RefPtr<Shader>& getShader() const { return m_pShader; }

		RefPtr<ShaderLayout> getLayout() { return m_list.getLayout(); }
		const RefPtr<ShaderLayout>& getLayout() const { return m_list.getLayout(); }

		size_t getTexturesCount() { return m_texturePtrs.size(); }

		static RefPtr<Material> create(const RefPtr<Shader>& pShader_);
		static RefPtr<Material> create(const RefPtr<Material>& pMaterial_);

		void onImGuiRender();

	private:
		void _copy(const Material& material_);
		//void _onSameShaderMaterialReload(Shader* pShader_);

	private:
		RefPtr<Shader> m_pShader;
		ShaderDataList m_list;

		std::vector<RefPtr<Texture>> m_texturePtrs;
	};
}