#pragma once
#define NOMINMAX
#include <wrl.h>
#include <wrl/client.h>

#include <memory>
#include <span>
#include <ranges>
#include <algorithm>
#include <cassert>
#include <print>
#include <concepts>
#include "Types.h"


using namespace Microsoft::WRL;

#ifdef _DEBUG
#define CESAR_NAME_D3D12_OBJECT(object, name) wchar_t lName_##object[512];            \
                                   swprintf_s(lName_##object, 512, L"%hs", name); \
                                   object->SetName(lName_##object);
#else
    #define CESAR_NAME_D3D12_OBJECT(object, name)
#endif //_DEBUG


#define CESAR_ASSERT(expr)            assert(expr)
#define SOL_ASSERT_MSG(expr, msg)   assert((expr) && (msg))

#define CESAR_DEBUGBREAK() __debugbreak()
#define CESAR_D3D12_CHECK(hresult) if(FAILED(hresult)) __debugbreak()
#define CESAR_FEATURE_NO_IMPL(msg) CESAR_DEBUGBREAK(); \
    CESAR_ASSERT(false && msg)

#define CESAR_SIZEOF(x) sizeof(x)
#define CESAR_SIZEOF_BUFFER(x,c) (sizeof(x) * c)

#define BIT(x) (1<<x)

template<typename Enum> requires std::is_enum_v<Enum>
inline constexpr bool HasFlag(Enum value, Enum flag)
{
    CESAR_ASSERT(std::has_single_bit(static_cast<std::make_unsigned_t<std::underlying_type_t<Enum>>>(std::to_underlying(flag))));
    return (std::to_underlying(value) & std::to_underlying(flag)) != 0;
}

#define GPU_STRUCTURE

#define CESAR_NO_DEFAULT_CONSTRUCTOR(T) T() = delete 
#define CESAR_NONCOPYABLE(T) T(const T&) = delete; T& operator=(const T&) = delete

#define CESAR_OFFSET_OFF(type, member) offsetof(type,member)