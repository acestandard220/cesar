#pragma once
#include "Types.h"

namespace cesar
{
    template<typename Enum>
    struct EnumOpt {
        constexpr static Bool enable = false;
        constexpr static Bool enable_equal = false;
        constexpr static Bool enable_or = false;
    };

    template<typename T>
    constexpr Bool EnumOptEnable = EnumOpt<T>::enable;

    template<typename T>
    constexpr Bool EnumOptEqual = EnumOpt<T>::enable_equal;

    template<typename T>
    constexpr Bool EnumOptOr = EnumOpt<T>::enable_or;

    template<typename Enum>
        requires std::is_enum_v<Enum>&& EnumOptEnable<Enum>
    Enum& operator|=(Enum& A, const Enum& B) {         
        using Type = std::underlying_type_t<Enum>;
        A = static_cast<Enum>(static_cast<Type>(A) | static_cast<Type>(B));
        return A;
    }

    template<typename Enum>
        requires std::is_enum_v<Enum> && (EnumOptEnable<Enum> || EnumOptEqual<Enum>)
    Bool operator==(const Enum& A, const Enum& B) {
        using Type = std::underlying_type_t<Enum>;
        return static_cast<Type>(A) == static_cast<Type>(B);
    }

    template<typename Enum>
        requires std::is_enum_v<Enum> && (EnumOptEnable<Enum> || EnumOptOr<Enum>)
    Enum operator|(const Enum& A, const Enum& B) {
        using Type = std::underlying_type_t<Enum>;
        return static_cast<Enum>(static_cast<Type>(A) | static_cast<Type>(B));
    }


#define CESAR_ENABLE_ENUM_OPS(Enum) \
    template<> struct cesar::EnumOpt<Enum> { \
        constexpr static Bool enable       = true; \
        constexpr static Bool enable_equal = false; \
    }

#define CESAR_ENABLE_ENUM_EQUAL_COMP_OP(Enum) \
    template<> struct cesar::EnumOpt<Enum> { \
        constexpr static Bool enable       = false; \
        constexpr static Bool enable_equal = true; \
    }
}