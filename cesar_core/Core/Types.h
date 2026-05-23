#pragma once

#include <cstdint>
#include <list>
#include <array>
#include <memory>
#include <span>
#include <type_traits>

namespace cesar
{
	using Uint8 = std::uint8_t;
	using Uint16 = std::uint16_t;
	using Uint32 = std::uint32_t;
	using Uint64 = std::uint64_t;

	using Int8 = std::int8_t;
	using Int16 = std::int16_t;
	using Int32 = std::int32_t;
	using Int64 = std::int64_t;

	using Float = float;
	using Double = double;

	using Bool = bool;
	using Char = char;
	using Wchar = wchar_t;

	template<typename T>
	struct GpuStructure {
		static_assert(std::is_trivially_copyable_v<T>, "GPU structs must be trivially copyable");
		static_assert(std::is_standard_layout_v<T>, "GPU structs must be standard layout");
	};

	template<typename T>
	using MemoryBlock = std::span<T>;
}