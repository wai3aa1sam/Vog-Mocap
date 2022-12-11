#pragma once
#include "Vog/Core/Core.h"

namespace vog {

	enum class BufferBitType
	{
		None	= 0,
		Color	= BIT(0),
		Depth	= BIT(1),
		Stencil = BIT(2),
	};

	enum class Primitive
	{
		None = 0,
		Point,
		Line,
		Triangle,
	};

	enum class CullFaceType
	{
		None = 0,
		Front,
		Back,
		Front_And_Back,
	};

	enum class DepthFuncConfig
	{
		None = 0,
		Always,
		Never,
		Equal,
		Not_Equal,
		Less,
		Less_Equal,
		Greater,
		Greater_Equal,
	};


#pragma region BufferBitType_operator_overload
	inline BufferBitType operator | (BufferBitType lhs_, BufferBitType rhs_)
	{
		using T = std::underlying_type_t<BufferBitType>;
		return static_cast<BufferBitType>(static_cast<T>(lhs_) | static_cast<T>(rhs_));
	}

	inline BufferBitType& operator |= (BufferBitType& lhs_, BufferBitType rhs_)
	{
		lhs_ = lhs_ | rhs_;
		return lhs_;
	}

	inline uint32_t operator & (BufferBitType lhs_, uint32_t rhs_)
	{
		return static_cast<uint32_t>(lhs_) & rhs_;
	}
#pragma endregion
}