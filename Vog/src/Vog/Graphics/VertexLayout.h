#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include <vector>

namespace vog {

	enum VOG_API class VertexDataType
	{
		None = 0,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Bool
	};

	namespace Utils
	{
		inline static uint32_t VOG_API getVertexDataTypeSize(VertexDataType type_)
		{
			switch (type_)
			{
			case VertexDataType::Int:		return 4;
			case VertexDataType::Int2:		return 4 * 2;
			case VertexDataType::Int3:		return 4 * 3;
			case VertexDataType::Int4:		return 4 * 4;
			case VertexDataType::Float:		return 4;
			case VertexDataType::Float2:	return 4 * 2;
			case VertexDataType::Float3:	return 4 * 3;
			case VertexDataType::Float4:	return 4 * 4;
			case VertexDataType::Mat3:		return 4 * 3 * 3;
			case VertexDataType::Mat4:		return 4 * 4 * 4;
			case VertexDataType::Bool:		return 4;
			}
			VOG_CORE_ASSERT(0, "Invalid shader data type!");
			return 0;
		}

		inline static uint32_t VOG_API getVertexDataTypeCount(VertexDataType type_)
		{
			switch (type_)
			{
			case VertexDataType::Int:		return 1;
			case VertexDataType::Int2:		return 2;
			case VertexDataType::Int3:		return 3;
			case VertexDataType::Int4:		return 4;
			case VertexDataType::Float:		return 1;
			case VertexDataType::Float2:	return 2;
			case VertexDataType::Float3:	return 3;
			case VertexDataType::Float4:	return 4;
			case VertexDataType::Mat3:		return 3;
			case VertexDataType::Mat4:		return 4;
			case VertexDataType::Bool:		return 1;
			default: break;
			}
			VOG_CORE_ASSERT(0, "Invalid shader data type!");
			return 0;
		}
	}

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertexLayout::position), (void*)0);

	struct VOG_API VertexLayoutElemets
	{
	public:
		VertexDataType type;
		std::string name;
		bool isNormalized = false;
		uint32_t count;

		VertexLayoutElemets(VertexDataType type_, const std::string& name_, bool isNormalized_ = false)
			:
			type(type_), name(name_), count(Utils::getVertexDataTypeCount(type_)), isNormalized(isNormalized_)
		{}
	};

	class VOG_API VertexLayout
	{
	public:
		VertexLayout() = default;
		VertexLayout(const std::initializer_list<VertexLayoutElemets>& elements_)
			:
			m_elements(elements_)
		{
			_processTotalElementBytes();
		}

		inline uint32_t getTotalElementsBytes() const { return m_totalElementsBytes; }
		inline const std::vector<VertexLayoutElemets>& getVertexLayout() const { return m_elements; }

		std::vector<VertexLayoutElemets>::iterator begin() { return m_elements.begin(); }
		std::vector<VertexLayoutElemets>::iterator end() { return m_elements.end(); }

		std::vector<VertexLayoutElemets>::const_iterator begin() const { return m_elements.cbegin(); }
		std::vector<VertexLayoutElemets>::const_iterator end() const { return m_elements.cend(); }

	private:
		void _processTotalElementBytes()
		{
			for (auto& element : m_elements)
			{
				m_totalElementsBytes += Utils::getVertexDataTypeSize(element.type);
			}
		}

	private:
		std::vector<VertexLayoutElemets> m_elements;
		uint32_t m_totalElementsBytes = 0;
	};
}