#include "vogpch.h"
#include "MemoryBuffer.h"

namespace vog {

//#define debug_memory_buffer

#ifdef debug_memory_buffer
	static uint32_t totalMemoryAllocated = 0;
#endif // debug_memory_buffer

	MemoryBuffer::MemoryBuffer(uint32_t byteSize_)
		:
		m_byteSize(byteSize_)
	{
		allocate(byteSize_);
	}

	MemoryBuffer::~MemoryBuffer()
	{
		deallocate();
	}

	MemoryBuffer::MemoryBuffer(const MemoryBuffer& rhs_)
	{
		reserve(rhs_.m_byteSize);
		Utils::my_memcpy<char>(m_pBuffer, rhs_.m_pBuffer, rhs_.m_byteSize);
		m_byteSize = rhs_.m_byteSize;
		m_byteCap = rhs_.m_byteSize; // not copy error
	}

	void MemoryBuffer::operator=(const MemoryBuffer& rhs_)
	{
		if (this == &rhs_ || rhs_.getByteSize() <= 0)
			return;
		allocate(rhs_.m_byteSize);
		Utils::my_memcpy<char>(m_pBuffer, rhs_.m_pBuffer, rhs_.m_byteSize);
		m_byteSize = rhs_.m_byteSize;
		m_byteCap = rhs_.m_byteSize; // not copy error
	}

	MemoryBuffer::MemoryBuffer(MemoryBuffer&& rhs_)
	{
		reset(std::move(rhs_));
	}

	void MemoryBuffer::operator=(MemoryBuffer&& rhs_)
	{
		if (this == &rhs_)
			return;
		deallocate();
		reset(std::move(rhs_));
	}

	void MemoryBuffer::allocate(uint32_t byteSize_)
	{
		/*if (m_pBuffer)
		{
			deallocate();
		}*/
		//bool isShouldResetZero = m_byteCap == 0;
		reserve(byteSize_);
		//VOG_CORE_ASSERT(m_pBuffer, "");

		if (m_pBuffer)
		{
			memset(m_pBuffer, 0, byteSize_);
		}
	}

	void MemoryBuffer::deallocate()
	{
		if (m_pBuffer)
		{
#ifdef debug_memory_buffer
			VOG_CORE_LOG_TRACE("MemoryBuffer deallocate, byteSize = {0}", m_byteCap);
			totalMemoryAllocated -= m_byteCap;
			VOG_CORE_LOG_TRACE("Total Memory Allocated = {0}", totalMemoryAllocated);
#endif // debug_memory_buffer
			free(m_pBuffer);
			m_pBuffer = nullptr;
			m_byteSize = 0;
			m_byteCap = 0;
		}
	}

	void MemoryBuffer::reserve(uint32_t new_cap_)
	{
		if (new_cap_ <= m_byteCap)
			return;

		auto newCap = m_byteCap + m_byteCap / 4;
		if (newCap < new_cap_)
			newCap = new_cap_;

		char* pNewData = static_cast<char*>(malloc(static_cast<uint32_t>(newCap)));
		char* dst = pNewData;
		char* src = m_pBuffer;
		char* end = src + m_byteSize;

		uint32_t newByteSize = newCap * sizeof(char);

		// it will not destruct previous stuff, as assumed only pod is use!
		if (m_pBuffer)
		{
			Utils::my_memcpy<char>(pNewData, m_pBuffer, m_byteSize);
			free(m_pBuffer);
#ifdef debug_memory_buffer
			VOG_CORE_LOG_TRACE("MemoryBuffer deallocate, byteSize = {0}", m_byteCap);
			totalMemoryAllocated -= m_byteCap;
#endif // debug_memory_buffer
		}
		m_pBuffer = pNewData;
		m_byteSize = newByteSize;
		m_byteCap = newCap;

#ifdef debug_memory_buffer
		VOG_CORE_LOG_TRACE("MemoryBuffer allocate, byteSize = {0}", newCap);
		totalMemoryAllocated += newCap;
		VOG_CORE_LOG_TRACE("Total Memory Allocated = {0}", totalMemoryAllocated);
#endif // debug_memory_buffer
	}

	void MemoryBuffer::clear()
	{
		memset(m_pBuffer, 0, m_byteSize);
		//m_byteSize = 0;
	}

	void MemoryBuffer::reset(MemoryBuffer&& buffer)
	{
		m_pBuffer = std::move(buffer.m_pBuffer);
		m_byteSize = buffer.m_byteSize;

		buffer.m_pBuffer = nullptr;
		buffer.m_byteSize = 0;
	}
}