#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"
#include "Vog/Math/MyMath.h"
#include "Vog/Memory/MemoryBuffer.h"

#include "Vog/Graphics/Texture.h"

namespace vog {

	struct OpenGLShaderData;

	enum class VOG_API ShaderDataType
	{
		None = 0,
		Int, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Bool,
		Texture2D, Cubemap,		// store the texture index(binding) in the vector
	};

	namespace Utils
	{
		static uint32_t getShaderDataTypeSize(ShaderDataType type_)
		{
			switch (type_)
			{
				case ShaderDataType::Int:			return 4;
				case ShaderDataType::Int2:			return 4 * 2;
				case ShaderDataType::Int3:			return 4 * 3;
				case ShaderDataType::Int4:			return 4 * 4;
				case ShaderDataType::Float:			return 4;
				case ShaderDataType::Float2:		return 4 * 2;
				case ShaderDataType::Float3:		return 4 * 3;
				case ShaderDataType::Float4:		return 4 * 4;
				case ShaderDataType::Mat3:			return 4 * 3 * 3;
				case ShaderDataType::Mat4:			return 4 * 4 * 4;
				case ShaderDataType::Bool:			return 4;		// could use 1? althought opengl bool is 4 byte, but it shoulf be fine to store 1 byte in cpu
				case ShaderDataType::Texture2D:		return 4;		// set as int
				case ShaderDataType::Cubemap:		return 4;		// set as int
			}
			VOG_CORE_ASSERT(0, "Invalid shader data type!");
			return 0;
		}

		static constexpr bool isIgnoreShaderDataType(ShaderDataType type_)
		{
			if (type_ == ShaderDataType::Mat3 || type_ == ShaderDataType::Mat4)
				return true;
			return false;
		}

		static const char* getShaderDataTypeName(ShaderDataType type_)
		{
			switch (type_)
			{
				case ShaderDataType::Int:			return "Int";
				case ShaderDataType::Int2:			return "Int2";
				case ShaderDataType::Int3:			return "Int3";
				case ShaderDataType::Int4:			return "Int4";
				case ShaderDataType::Float:			return "Float";
				case ShaderDataType::Float2:		return "Float2";
				case ShaderDataType::Float3:		return "Float3";
				case ShaderDataType::Float4:		return "Float4";
				case ShaderDataType::Mat3:			return "Mat3";
				case ShaderDataType::Mat4:			return "Mat4";
				case ShaderDataType::Bool:			return "Bool";
				case ShaderDataType::Texture2D:		return "Texture2D";
				case ShaderDataType::Cubemap:		return "Cubemap";
			}
			VOG_CORE_ASSERT(0, "Invalid shader data type!");
			return nullptr;
		}
	}

	struct VOG_API ShaderLayoutElement
	{
		ShaderDataType type;
		std::string name;
		uint32_t offset = 0;
		uint32_t byteSize = 0;
		//char* pData = nullptr;

		struct LessThan
		{
			bool operator()(const ShaderLayoutElement& lhs_, const ShaderLayoutElement& rhs_) const
			{
				return lhs_.offset < rhs_.offset;
			}
		};

		ShaderLayoutElement() = default;
		uint32_t count() const { return byteSize / Utils::getShaderDataTypeSize(type); }
	};

	class ShaderLayout
	{
		friend class ShaderDataList;
	public:
		ShaderLayout() = default;
		~ShaderLayout() = default;
		ShaderLayout(const std::unordered_map<std::string, OpenGLShaderData>& shaderDataMap_);

		void clear();
		void reload(const std::unordered_map<std::string, OpenGLShaderData>& shaderDataMap_);

		inline ShaderLayoutElement* getElement_safe(const std::string& name_)
		{
			//VOG_CORE_ASSERT(m_indexMap.find(name_) != m_indexMap.end(), "cannot find element, name: {0}", name_);
			if (auto it = m_indexMap.find(name_); it != m_indexMap.end())
				return &m_elements[it->second];
			return nullptr;
		}

		inline const ShaderLayoutElement* getElement_safe(const std::string& name_) const
		{
			//VOG_CORE_ASSERT(m_indexMap.find(name_) != m_indexMap.end(), "cannot find element, name: {0}", name_);
			if (auto it = m_indexMap.find(name_); it != m_indexMap.end())
				return &m_elements[it->second];
			return nullptr;
		}

		inline ShaderLayoutElement& getElement(const std::string& name_) 
		{ 
			VOG_CORE_ASSERT(m_indexMap.find(name_) != m_indexMap.end(), "cannot find element, name: {0}", name_);
			return m_elements[m_indexMap.at(name_)];
		}

		inline const ShaderLayoutElement& getElement(const std::string& name_) const
		{
			return m_elements[m_indexMap.at(name_)];
		}

		uint32_t getCount() { return static_cast<uint32_t>(m_elements.size()); }
		uint32_t getByteSize() { return m_byteSize; }

		bool contains(const std::string& name_)			{ return m_indexMap.find(name_) != m_indexMap.end(); }
		bool contains(const std::string& name_) const	{ return m_indexMap.find(name_) != m_indexMap.end(); }

		std::vector<ShaderLayoutElement>::iterator begin() { return m_elements.begin(); }
		std::vector<ShaderLayoutElement>::iterator end() { return m_elements.end(); }

		std::vector<ShaderLayoutElement>::const_iterator begin() const { return m_elements.cbegin(); }
		std::vector<ShaderLayoutElement>::const_iterator end() const { return m_elements.cend(); }

	private:
		std::unordered_map<std::string, uint32_t> m_indexMap;
		std::vector<ShaderLayoutElement> m_elements;
		uint32_t m_byteSize = 0;
	};

	class VOG_API ShaderDataList
	{
	public:
		ShaderDataList() = default;
		explicit ShaderDataList(const RefPtr<ShaderLayout>& m_pShaderLayout_);

		~ShaderDataList();

		ShaderDataList(const ShaderDataList& rhs_);
		void operator=(const ShaderDataList& rhs_);

		void inviladte(const RefPtr<ShaderLayout>& m_pShaderLayout_);
		void init(const RefPtr<ShaderLayout>& m_pShaderLayout_);
		void reload();

		ShaderLayoutElement& operator[](uint32_t index_)
		{
			VOG_CORE_ASSERT(index_ < getSize(), "out of bound");
			return m_pShaderLayout->m_elements[index_];
		}

		inline ShaderLayoutElement& getLayoutElement(const std::string& name_) { return m_pShaderLayout->getElement(name_); }
		inline ShaderLayoutElement* getLayoutElement_safe(const std::string& name_) { return m_pShaderLayout->getElement_safe(name_); }

		template<typename T>
		const T& getShaderDataValue(const std::string& name_) const
		{
			const auto& layoutElement = getLayoutElement(name_);
			return m_buffer.read<T>(layoutElement.offset);
		}

		template<typename T>
		T& getShaderDataValue(const std::string& name_)
		{
			const auto& layoutElement = getLayoutElement(name_);
			return m_buffer.read<T>(layoutElement.offset);
		}

		template<typename T>
		const T* getShaderDataValues(const std::string& name_) const
		{
			const auto& layoutElement = getLayoutElement(name_);
			return reinterpret_cast<T*>(m_buffer.readAddress(layoutElement.offset));
		}

		template<typename T>
		T* getShaderDataValues(const std::string& name_)
		{
			const auto& layoutElement = getLayoutElement(name_);
			return reinterpret_cast<T*>(m_buffer.readAddress(layoutElement.offset));
		}

		void setFloat(const std::string& name_, float value_);
		void setFloat2(const std::string& name_, const Vector2f& value_);
		void setFloat3(const std::string& name_, const Vector3f& value_);
		void setFloat4(const std::string& name_, const Vector4f& value_);
		void setInt(const std::string& name_, int value_);
		void setBool(const std::string& name_, bool value_);
		void setMat3(const std::string& name_, const Matrix3f& value_);
		void setMat4(const std::string& name_, const Matrix4f& value_);
		//void setTexture2D(const std::string& name_, const RefPtr<Texture2D>& value_);
		//void setCubemap(const std::string& name_, const RefPtr<Cubemap>& value_);

		void setFloatArray(const std::string& name_, const float* values_);
		void setFloat2Array(const std::string& name_, const Vector2f* values_);
		void setFloat3Array(const std::string& name_, const Vector3f* values_);
		void setFloat4Array(const std::string& name_, const Vector4f* values_);
		void setIntArray(const std::string& name_, const int* values_);
		void setBoolArray(const std::string& name_, const bool* values_);
		void setMat3(const std::string& name_, const Matrix3f* values_);
		void setMat4(const std::string& name_, const Matrix4f* values_);
		//void setTexture2DArray(const std::string& name_, const RefPtr<Texture2D>* values_);
		//void setCubemapArray(const std::string& name_, const RefPtr<Cubemap>* values_);

		void reset(ShaderDataList&& list);

		void dumpDataMap() const;

		uint32_t getSize() const { return static_cast<uint32_t>(m_pShaderLayout->getCount()); }
		uint32_t getByteSize() const { return m_buffer.getByteSize(); }

		MemoryBuffer& getBuffer() { return m_buffer; }

		RefPtr<ShaderLayout>& getLayout() { return m_pShaderLayout; }
		const RefPtr<ShaderLayout>& getLayout() const { return m_pShaderLayout; }

	private:
		void _copy(const ShaderDataList& rhs_);

	private:
		MemoryBuffer m_buffer;
		RefPtr<ShaderLayout> m_pShaderLayout;
	};
}