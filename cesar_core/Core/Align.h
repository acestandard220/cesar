#pragma once
#include "Types.h"

namespace cesar
{
	template<typename T>
	constexpr T AlignUp(T value, Uint64 alignment)
	{
		return (value + alignment - 1) & ~(alignment - 1);
	}

	template<typename T>
	constexpr T AlignDown(T value, Uint64 alignment)
	{
		return (value) & ~(alignment - 1);
	}
}