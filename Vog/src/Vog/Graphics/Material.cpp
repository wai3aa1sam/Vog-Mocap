#include "vogpch.h"
#include "Vog/Graphics/Material.h"

#include "Vog/Resources/AssetManager.h"

#include "Vog/ImGui/ImGuiLibrary.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace vog {

	Material::Material(const RefPtr<Shader>& pShader_)
		:
		m_pShader(pShader_), m_list(pShader_->getShaderLayout())
	{
		uint32_t texturesSize = 0;
		uint32_t binding = 0;

		// load binding as texture index
		const auto& pShaderLayout = m_list.getLayout();
		for (const auto& element : *pShaderLayout)
		{
			switch (element.type)
			{
				case ShaderDataType::Float:				m_list.setFloat(element.name, 1.0f);				break;
				case ShaderDataType::Float2:			m_list.setFloat2(element.name, Vector2f(1.0f));		break;
				case ShaderDataType::Float3:			m_list.setFloat3(element.name, Vector3f(1.0f));		break;
				case ShaderDataType::Float4:			m_list.setFloat4(element.name, Vector4f(1.0f));		break;
				case ShaderDataType::Int:				m_list.setInt(element.name, 0);						break;
				case ShaderDataType::Bool:				m_list.setBool(element.name, 0);					break;
			}

			if (element.type == ShaderDataType::Texture2D)		// not yet handle texture array
			{
				binding = m_pShader->getBindingLocation(element.name);
				m_list.setInt(element.name, binding);
				texturesSize++;
			}
			else if (element.type == ShaderDataType::Cubemap)	// not yet handle texture array
			{
				binding = m_pShader->getBindingLocation(element.name);
				m_list.setInt(element.name, binding);
				texturesSize++;
			}
		}
		m_texturePtrs.reserve(texturesSize);
		auto& pDefaultTexture = AssetManager::getDefaultTexture();
		for (uint32_t i = 0; i < texturesSize; i++)
		{
			m_texturePtrs.push_back(pDefaultTexture);
		}
	}

	Material::Material(const Material& rhs_)
	{
		_copy(rhs_);
	}

	void Material::operator=(const Material& rhs_)
	{
		if (this == &rhs_)
			return;
		_copy(rhs_);
	}

	Material::~Material()
	{
	}

	void Material::uploadMaterial()
	{
		if (!m_pShader)
			return;

		if (m_pShader->getIsReloaded())
			reload();

		//m_pShader->bind();

		uint32_t shaderID = m_pShader->getID();

		const auto pShaderLayout = m_list.getLayout();
		for (const auto& element : *pShaderLayout)
		{
			const std::string& name = element.name;
			switch (element.type)
			{
				case ShaderDataType::Float:			m_pShader->setFloatArray(name,	m_list.getShaderDataValues<float>(name), element.count());		break;
				case ShaderDataType::Float2:		m_pShader->setFloat2Array(name, m_list.getShaderDataValues<Vector2f>(name), element.count());	break;
				case ShaderDataType::Float3:		m_pShader->setFloat3Array(name, m_list.getShaderDataValues<Vector3f>(name), element.count());	break;
				case ShaderDataType::Float4:		m_pShader->setFloat4Array(name, m_list.getShaderDataValues<Vector4f>(name), element.count());	break;
				case ShaderDataType::Int:			m_pShader->setIntArray(name,	m_list.getShaderDataValues<int>(name), element.count());		break;
				case ShaderDataType::Bool:			m_pShader->setBoolArray(name,	m_list.getShaderDataValues<bool>(name), element.count());		break;
				case ShaderDataType::Mat3:			m_pShader->setMat3Array(name,	m_list.getShaderDataValues<Matrix3f>(name), element.count());	break;
				case ShaderDataType::Mat4:			m_pShader->setMat4Array(name,	m_list.getShaderDataValues<Matrix4f>(name), element.count());	break;
			}

			int* pIndex = nullptr;
			if (element.type == ShaderDataType::Texture2D)
			{
				pIndex = m_list.getShaderDataValues<int>(name);
				if (pIndex == nullptr || (*pIndex < 0 || *pIndex >= m_texturePtrs.size()))		// cannot get
				{
					VOG_CORE_LOG_ERROR("cannot find texture binding, name: {0}", name);
					continue;
				}

				if (!m_texturePtrs.empty())
				{
					for (size_t i = 0; i < element.count(); i++, pIndex++)
					{
						const auto& pTexture = m_texturePtrs[*pIndex];
						if (pTexture)
							pTexture->bind(*pIndex);
					}
				}
			}
		}
	}

	void Material::resetShader(const RefPtr<Shader>& pShader_)
	{
		m_pShader = pShader_;
		//m_list = pShader_->getShaderLayout();			// it will create a temp ShaderDataList and copy to the m_list..........., use explicit to solve
		m_list.inviladte(pShader_->getShaderLayout());
		//m_texturePtrs.clear();

		uint32_t texturesSize = 0;
		uint32_t binding = 0;

		// load binding as texture index
		const auto& pShaderLayout = m_list.getLayout();
		for (const auto& element : *pShaderLayout)
		{
			if (element.type == ShaderDataType::Texture2D)		// not yet handle texture array
			{
				binding = m_pShader->getBindingLocation(element.name);
				m_list.setInt(element.name, binding);
				texturesSize++;
			}
			else if (element.type == ShaderDataType::Cubemap)	// not yet handle texture array
			{
				binding = m_pShader->getBindingLocation(element.name);
				m_list.setInt(element.name, binding);
				texturesSize++;
			}
		}

		uint32_t prevSize = static_cast<uint32_t>(m_texturePtrs.size());
		auto& pDefaultTexture = AssetManager::getDefaultTexture();
		if (texturesSize > prevSize)		// check if prev size has texture
		{
			m_texturePtrs.reserve(texturesSize);
			auto& pDefaultTexture = AssetManager::getDefaultTexture();
			for (uint32_t i = prevSize; i < texturesSize; i++)
			{
				m_texturePtrs.push_back(pDefaultTexture);
			}
		}
		else
		{
			m_texturePtrs.erase(m_texturePtrs.begin() + texturesSize, m_texturePtrs.begin() + prevSize);
		}
	}

	void Material::reload()
	{
		const auto& pShaderLayout = m_list.getLayout();

		ShaderDataList copy_list(m_list);

		if (pShaderLayout->getByteSize() > 0)
			m_list.reload();

		uint32_t texturesSize = 0;
		uint32_t binding = 0;

		// load binding as texture index
		for (const auto& element : *pShaderLayout)
		{
			// copy back the original value if exit
			if (copy_list.getLayout()->getElement_safe(element.name))
			{
				switch (element.type)
				{
					case ShaderDataType::Float:				m_list.setFloatArray(element.name, copy_list.getShaderDataValues<float>(element.name));					break;
					case ShaderDataType::Float2:			m_list.setFloat2Array(element.name, copy_list.getShaderDataValues<Vector2f>(element.name));				break;
					case ShaderDataType::Float3:			m_list.setFloat3Array(element.name, copy_list.getShaderDataValues<Vector3f>(element.name));				break;
					case ShaderDataType::Float4:			m_list.setFloat4Array(element.name, copy_list.getShaderDataValues<Vector4f>(element.name));				break;
					case ShaderDataType::Int:				m_list.setIntArray(element.name, copy_list.getShaderDataValues<int>(element.name));						break;
					case ShaderDataType::Bool:				m_list.setBoolArray(element.name, copy_list.getShaderDataValues<bool>(element.name));					break;
				}
			}

			if (element.type == ShaderDataType::Texture2D)		// not yet handle texture array
			{
				binding = m_pShader->getBindingLocation(element.name);
				m_list.setInt(element.name, binding);
				texturesSize++;
			}
			else if (element.type == ShaderDataType::Cubemap)	// not yet handle texture array
			{
				binding = m_pShader->getBindingLocation(element.name);
				m_list.setInt(element.name, binding);
				texturesSize++;
			}
		}

		uint32_t prevSize = static_cast<uint32_t>(m_texturePtrs.size());

		if (texturesSize > prevSize)		// check if prev size has texture
		{
			m_texturePtrs.reserve(texturesSize);
			auto& pDefaultTexture = AssetManager::getDefaultTexture();
			for (uint32_t i = prevSize; i < texturesSize; i++)
			{
				m_texturePtrs.push_back(pDefaultTexture);
			}
		}
		else
		{
			m_texturePtrs.erase(m_texturePtrs.begin() + texturesSize, m_texturePtrs.begin() + prevSize);
		}
	}

	bool Material::hasTexture(const std::string& name_)
	{
		int binding = m_pShader->getBindingLocation(name_);
		if (binding < 0)
			return false;
		return m_texturePtrs[binding] != AssetManager::getDefaultTexture();
	}
	
#pragma region set_func
	void Material::setFloat(const std::string& name_, float value_)
	{
		m_list.setFloat(name_, value_);
	}

	void Material::setFloat4(const std::string& name_, const Vector4f& value_)
	{
		m_list.setFloat4(name_, value_);
	}

	void Material::setTexture2D(const std::string& name_, const RefPtr<Texture2D>& value_)
	{
		const auto& layout = m_list.getLayoutElement(name_);
		auto textureIndex = m_list.getShaderDataValue<int>(name_);

		VOG_CORE_ASSERT(m_texturePtrs.size() > 0 && textureIndex < m_texturePtrs.size(), "");
		if (layout.type == ShaderDataType::Texture2D)
		{
			m_texturePtrs[textureIndex] = value_;
		}
		else
		{
			VOG_CORE_LOG_ERROR("Wrong shader data type! attempt to put {0} to {1}", Utils::getShaderDataTypeName(layout.type), Utils::getShaderDataTypeName(ShaderDataType::Texture2D));
		}
	}

	void Material::setCubemap(const std::string& name_, const RefPtr<Cubemap>& value_)
	{
		VOG_CORE_ASSERT(0, "");
	}

	void Material::setFloatArray(const std::string& name_, const float* values_)
	{
		m_list.setFloatArray(name_, values_);
	}

	void Material::setFloat4Array(const std::string& name_, const Vector4f* values_)
	{
		m_list.setFloat4Array(name_, values_);
	}

	void Material::setTexture2DArray(const std::string& name_, const RefPtr<Texture2D>* values_)
	{
		VOG_CORE_ASSERT(0, "");
	}

	void Material::setCubemapArray(const std::string& name_, const RefPtr<Cubemap>* values_)
	{
		VOG_CORE_ASSERT(0, "");
	}
	
#pragma endregion

	RefPtr<Material> Material::create(const RefPtr<Shader>& pShader_)
	{
		return createRefPtr<Material>(pShader_);
	}

	RefPtr<Material> Material::create(const RefPtr<Material>& pMaterial_)
	{
		return createRefPtr<Material>(*pMaterial_.get());
	}

	void Material::onImGuiRender()
	{
		// have not handle texture array

		//ImGui::Text("Shader: ");
		//ImGui::SameLine();
		//ImGui::Text(label_.c_str());

		ImGui::PushID(m_pShader->getID());
		const auto pShaderLayout = getLayout();
		for (const auto& element : *pShaderLayout)
		{
			const std::string& name = element.name;
			switch (element.type)
			{
				case ShaderDataType::Float:			ImGuiLibrary::drawDragFloatArray(name, m_list.getShaderDataValues<float>(name), element.count());			break;
				case ShaderDataType::Float2:		ImGuiLibrary::drawDragFloat2Array(name, m_list.getShaderDataValues<Vector2f>(name), element.count());		break;
				case ShaderDataType::Float3:		ImGuiLibrary::drawDragFloat3Array(name, m_list.getShaderDataValues<Vector3f>(name), element.count());		break;
				case ShaderDataType::Float4:		ImGuiLibrary::drawDragFloat4Array(name, m_list.getShaderDataValues<Vector4f>(name), element.count());		break;
				case ShaderDataType::Int:			ImGuiLibrary::drawInputIntArray(name, m_list.getShaderDataValues<int>(name), element.count());				break;
				case ShaderDataType::Bool:			ImGuiLibrary::drawCheckboxArray(name, m_list.getShaderDataValues<bool>(name), element.count());				break;
			}

			if (element.type == ShaderDataType::Texture2D)
			{
				int* pIndex = m_list.getShaderDataValues<int>(name);
				VOG_CORE_ASSERT(pIndex, "");
				ImGuiLibrary::drawTextureIcon(name, m_texturePtrs[*pIndex]->getRendererID());
			}
			else if (element.type == ShaderDataType::Cubemap)
			{
				VOG_CORE_ASSERT(0, "");
			}
		}

		ImGui::PopID();
	}

	void Material::_copy(const Material& material_)
	{
		m_pShader = material_.m_pShader;
		m_list = material_.m_list;

		size_t newTexturesSize = material_.m_texturePtrs.size();
		size_t prevTexturesSize = m_texturePtrs.size();
		if (newTexturesSize > prevTexturesSize)		// check if prev size has texture
		{
			m_texturePtrs.reserve(newTexturesSize);
			auto& pDefaultTexture = AssetManager::getDefaultTexture();
			for (size_t i = prevTexturesSize; i < newTexturesSize; i++)
			{
				m_texturePtrs.push_back(pDefaultTexture);
			}
		}
		else
		{
			m_texturePtrs.erase(m_texturePtrs.begin() + newTexturesSize, m_texturePtrs.begin() + prevTexturesSize);
		}
	}
}