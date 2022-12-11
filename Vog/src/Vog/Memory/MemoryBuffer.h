#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

namespace vog
{
	// ***************************** Notes
	// this buffer should be only used for varying pods / pod wrapper class, eg. current memoryBuffer layout: float, int, glm::vec4, int[]....
	// the responsibility for freeing memory should be the user if not using those types
	// *****************************

	namespace Utils
	{
		static constexpr int s_size_of_theshold_to_memcpy = 128;

		template<typename T>
		inline static void my_memcpy(T* pDst_, const T* pSrc_, size_t byteSize_)
		{
			VOG_CORE_ASSERT(pDst_, "");
			if (byteSize_ <= 0)
				return;

			size_t count = byteSize_ / sizeof(T);

			if (byteSize_ > s_size_of_theshold_to_memcpy && std::is_pod<T>())
			{
				memcpy(pDst_, pSrc_, byteSize_);
			}
			else
			{
				const T* pEnd = pSrc_ + count;
				for (;pSrc_ < pEnd; ++pSrc_, ++pDst_)
				{
					*pDst_ = *pSrc_;
				}
			}
		}
	}

	class VOG_API MemoryBuffer
	{
	public:
		MemoryBuffer() = default;
		MemoryBuffer(uint32_t byteSize_);
		~MemoryBuffer();

		MemoryBuffer(const MemoryBuffer& rhs_);
		void operator=(const MemoryBuffer& rhs_);

		MemoryBuffer(MemoryBuffer&& rhs_);
		void operator=(MemoryBuffer&& rhs_);

		void allocate(uint32_t byteSize_);
		void deallocate();

		void reserve(uint32_t new_cap_);
		void clear();

		void reset(MemoryBuffer&& buffer);

		const void* readAddress(uint32_t byteOffset_) const
		{
			VOG_CORE_ASSERT(m_pBuffer + byteOffset_ <= m_pBuffer + m_byteSize, "");	// check_bound
			return m_pBuffer + byteOffset_;
		}

		void* readAddress(uint32_t byteOffset_)
		{
			VOG_CORE_ASSERT(m_pBuffer + byteOffset_ <= m_pBuffer + m_byteSize, "");	// check_bound
			return m_pBuffer + byteOffset_;
		}

		template<typename T>
		const T& read(uint32_t byteOffset_) const
		{
			VOG_CORE_ASSERT(m_pBuffer + byteOffset_ <= m_pBuffer + m_byteSize, "");	// check_bound

			char* cursor = m_pBuffer + byteOffset_;
			return *reinterpret_cast<T*>(cursor);
		}

		template<typename T>
		T& read(uint32_t byteOffset_)
		{
			VOG_CORE_ASSERT(m_pBuffer + byteOffset_ <= m_pBuffer + m_byteSize, "");	// check_bound

			char* cursor = m_pBuffer + byteOffset_;
			return *reinterpret_cast<T*>(cursor);
		}

		template<typename T>
		void write(const T& value_, uint32_t byteOffset_)
		{
			write<T>(&value_, byteOffset_, sizeof(T));
		}

		template<typename T>
		void write(const T* values_, uint32_t byteOffset_, uint32_t byteSize_)
		{
			char* cursor = m_pBuffer + byteOffset_;
			VOG_CORE_ASSERT(m_pBuffer + byteOffset_ <= m_pBuffer + m_byteSize && m_pBuffer + byteOffset_ + byteSize_ <= m_pBuffer + m_byteSize, "");	// check_bound

			bool isPod = std::is_pod<T>::value;
			if (isPod)
			{
				Utils::my_memcpy<T>(reinterpret_cast<T*>(cursor), values_, byteSize_);
			}
			else
			{
				uint32_t count = byteSize_ / sizeof(T);
				T* dst = reinterpret_cast<T*>(cursor);
				T* end = dst + count;
				for (; dst < end; ++dst, ++values_)
				{
					dst->~T();
					new(dst) T(*values_);		// just copy, no move
				}
			}
		}

		void* get() { return m_pBuffer; }
		const void* get() const { return m_pBuffer; }

		uint32_t getByteSize() { return m_byteSize; }
		uint32_t getByteSize() const { return m_byteSize; }

		operator char* () { return m_pBuffer; }

	private:

	private:
		char* m_pBuffer = nullptr;
		uint32_t m_byteSize = 0;
		uint32_t m_byteCap = 0;
	};
}