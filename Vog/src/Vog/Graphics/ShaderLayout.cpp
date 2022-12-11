#include "vogpch.h"

#include "Vog/Core/Log.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLUtils.h"

#include "Vog/Graphics/ShaderLayout.h"

namespace vog{

//#define debug_ShaderLayout


#pragma region ShaderLayout_impl
	ShaderLayout::ShaderLayout(const std::unordered_map<std::string, OpenGLShaderData>& shaderDataMap_)
	{
		reload(shaderDataMap_);
	}

	void ShaderLayout::reload(const std::unordered_map<std::string, OpenGLShaderData>& shaderDataMap_)
	{
		clear();

		m_indexMap.reserve(shaderDataMap_.size());
		m_elements.reserve(shaderDataMap_.size());

		uint32_t byteSize = 0;
		uint32_t index = 0;

		for (const auto& [name, opengShaderData] : shaderDataMap_)
		{
			ShaderDataType type = Utils::getShaderDataType(opengShaderData.type);
			if (Utils::isIgnoreShaderDataType(type))
				continue;

			m_indexMap[name] = index;

			ShaderLayoutElement element;
			element.type = type;
			element.name = name;
			element.offset = byteSize;
			element.byteSize = Utils::getShaderDataTypeSize(element.type) * opengShaderData.count;
			m_elements.push_back(element);

			byteSize += element.byteSize;
			index++;
		}

		ShaderLayoutElement::LessThan lessThan;
		std::sort(m_elements.begin(), m_elements.end(), lessThan);

		m_byteSize = byteSize;
	}

	void ShaderLayout::clear()
	{
		m_elements.clear();
		m_indexMap.clear();
		m_byteSize = 0;
	}
#pragma endregion

#pragma region ShaderDataList_impl

	ShaderDataList::ShaderDataList(const RefPtr<ShaderLayout>& m_pShaderLayout_)
	{
		init(m_pShaderLayout_);
	}

	ShaderDataList::~ShaderDataList()
	{
	}

	ShaderDataList::ShaderDataList(const ShaderDataList& rhs_)
	{
		_copy(rhs_);
	}

	void ShaderDataList::operator=(const ShaderDataList& rhs_)
	{
		if (this == &rhs_)
			return;
		// the data type will free if previous is not null
		_copy(rhs_);
	}

	void ShaderDataList::inviladte(const RefPtr<ShaderLayout>& m_pShaderLayout_)
	{
		VOG_CORE_ASSERT(m_pShaderLayout, "");

		m_pShaderLayout = m_pShaderLayout_;
		m_buffer.allocate(m_pShaderLayout->getByteSize());

#ifdef debug_ShaderLayout
		dumpDataMap();
#endif // debug_ShaderLayout
	}

	void ShaderDataList::init(const RefPtr<ShaderLayout>& m_pShaderLayout_)
	{
		VOG_CORE_ASSERT(!m_pShaderLayout, "");

		m_pShaderLayout = m_pShaderLayout_;

		if (m_pShaderLayout->getByteSize() > 0)
			m_buffer.allocate(m_pShaderLayout->getByteSize());

#ifdef debug_ShaderLayout
		dumpDataMap();
#endif // debug_ShaderLayout
	}

	void ShaderDataList::reload()
	{
		VOG_CORE_ASSERT(m_pShaderLayout, "");
		// same ShaderLayout, but the acutal ShaderLayout object is reloaded
		// if the byteSize is less than previous, it will not allocate a new buffer
		m_buffer.allocate(m_pShaderLayout->getByteSize());
	}

	void ShaderDataList::reset(ShaderDataList&& list)
	{
		this->m_buffer.reset(std::move(list.m_buffer));
		this->m_pShaderLayout = std::move(list.m_pShaderLayout);
	}

	void ShaderDataList::dumpDataMap() const
	{
		VOG_CORE_LOG_TRACE("Shader Data:");
		auto pLayout = getLayout();
		for (const auto& element : *pLayout)
		{
#ifdef debug_ShaderLayout

			VOG_CORE_LOG_TRACE("\t type: {0} name: {1}, offset: {2}, count: {3}, byteSize: {4}",
				element.type, element.name, element.offset, element.count(), element.byteSize);

#else
			VOG_CORE_LOG_TRACE("\t type: {0}, offset: {2}, count: {3}, byteSize: {4}",
				element.type, element.offset, element.count(), element.byteSize);
#endif // debug_ShaderLayout
		}
	}

	void ShaderDataList::_copy(const ShaderDataList& rhs_)
	{
		m_buffer = rhs_.m_buffer;
		m_pShaderLayout = rhs_.m_pShaderLayout;
	}

#pragma region setData
	void ShaderDataList::setFloat(const std::string& name_, float value_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<float>(value_, layoutElement->offset);
	}

	void ShaderDataList::setFloat2(const std::string& name_, const Vector2f& value_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<Vector2f>(value_, layoutElement->offset);
	}

	void ShaderDataList::setFloat3(const std::string& name_, const Vector3f& value_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<Vector3f>(value_, layoutElement->offset);
	}

	void ShaderDataList::setFloat4(const std::string& name_, const Vector4f& value_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<Vector4f>(value_, layoutElement->offset);
	}

	void ShaderDataList::setInt(const std::string& name_, int value_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<int>(value_, layoutElement->offset);
	}

	void ShaderDataList::setBool(const std::string& name_, bool value_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<bool>(value_, layoutElement->offset);
	}

	void ShaderDataList::setMat3(const std::string& name_, const Matrix3f& value_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<Matrix3f>(value_, layoutElement->offset);
	}

	void ShaderDataList::setMat4(const std::string& name_, const Matrix4f& value_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<Matrix4f>(value_, layoutElement->offset);
	}

	//void ShaderDataList::setTexture2D(const std::string& name_, const RefPtr<Texture2D>& value_)
	//{
	//	const auto* layoutElement = getLayoutElement_safe(name_);
	//	if (layoutElement)
	//		m_buffer.write<RefPtr<Texture2D>>(value_, shaderData.offset);
	//}

	//void ShaderDataList::setCubemap(const std::string& name_, const RefPtr<Cubemap>& value_)
	//{
	//	const auto* layoutElement = getLayoutElement_safe(name_);
	//	if (layoutElement)
	//		m_buffer.write<RefPtr<Cubemap>>(value_, shaderData.offset);
	//}

	void ShaderDataList::setFloatArray(const std::string& name_, const float* values_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<float>(values_, layoutElement->offset, layoutElement->byteSize);
	}

	void ShaderDataList::setFloat2Array(const std::string& name_, const Vector2f* values_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<Vector2f>(values_, layoutElement->offset, layoutElement->byteSize);
	}

	void ShaderDataList::setFloat3Array(const std::string& name_, const Vector3f* values_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<Vector3f>(values_, layoutElement->offset, layoutElement->byteSize);
	}

	void ShaderDataList::setFloat4Array(const std::string& name_, const Vector4f* values_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<Vector4f>(values_, layoutElement->offset, layoutElement->byteSize);
	}

	void ShaderDataList::setIntArray(const std::string& name_, const int* values_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<int>(values_, layoutElement->offset, layoutElement->byteSize);
	}

	void ShaderDataList::setBoolArray(const std::string& name_, const bool* values_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<bool>(values_, layoutElement->offset, layoutElement->byteSize);
	}

	void ShaderDataList::setMat3(const std::string& name_, const Matrix3f* values_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<Matrix3f>(values_, layoutElement->offset, layoutElement->byteSize);
	}

	void ShaderDataList::setMat4(const std::string& name_, const Matrix4f* values_)
	{
		const auto* layoutElement = getLayoutElement_safe(name_);
		if (layoutElement)
			m_buffer.write<Matrix4f>(values_, layoutElement->offset, layoutElement->byteSize);
	}

	//void ShaderDataList::setTexture2DArray(const std::string& name_, const RefPtr<Texture2D>* values_)
	//{
	//	const auto* layoutElement = getLayoutElement_safe(name_);
	//	if (layoutElement)
	//		m_buffer.write<RefPtr<Texture2D>>(values_, layoutElement->offset, layoutElement->byteSize);
	//}

	//void ShaderDataList::setCubemapArray(const std::string& name_, const RefPtr<Cubemap>* values_)
	//{
	//	const auto* layoutElement = getLayoutElement_safe(name_);
	//	if (layoutElement)
	//		m_buffer.write<RefPtr<Cubemap>>(values_, layoutElement->offset, layoutElement->byteSize);
	//}
#pragma endregion
#pragma endregion



}


