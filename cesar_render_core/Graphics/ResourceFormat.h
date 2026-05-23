// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../../cesar_core/cesar_core.h"
#include <dxgiformat.h>

namespace cesar
{
    enum class ResourceFormat
    {
        R16_TYPELESS,
        R32_TYPELESS,
        R24G8_TYPELESS,
        R32_G24B8_TYPELESS,

        //4 Channels (RGBA)
        RGBA32_FLOAT,
        RGBA32_UINT,
        RGBA32_SINT,

        RGBA16_FLOAT,
        RGBA16_UNORM,
        RGBA16_UINT,
        RGBA16_SNORM,
        RGBA16_SINT,

        RGBA8_UNORM,
        RGBA8_UNORM_SRGB,
        RGBA8_UINT,
        RGBA8_SNORM,
        RGBA8_SINT,

        BGRA8_UNORM,
        BGRA8_UNORM_SRGB,

        R10G10B10A2_UNORM,
        R10G10B10A2_UINT,

        R11G11B10_FLOAT,

        //3 Channels (RGB)
        RGB32_FLOAT,
        RGB32_UINT,
        RGB32_SINT,

        //Shared exponent HDR
        RGB9E5_SHAREDEXP,

        //2 Channels (RG)
        RG32_FLOAT,
        RG32_UINT,
        RG32_SINT,

        RG16_FLOAT,
        RG16_UNORM,
        RG16_UINT,
        RG16_SNORM,
        RG16_SINT,

        RG8_UNORM,
        RG8_UINT,
        RG8_SNORM,
        RG8_SINT,

        //1 Channel (R)
        R32_FLOAT,
        R32_UINT,
        R32_SINT,

        R16_FLOAT,
        R16_UNORM,
        R16_UINT,
        R16_SNORM,
        R16_SINT,

        R8_UNORM,
        R8_UINT,
        R8_SNORM,
        R8_SINT,

        //Depth / Stencil
        D32_FLOAT,
        D24_UNORM_S8_UINT,
        D16_UNORM,

        //Compressed formats (Block Compression)
        BC1_UNORM,
        BC1_UNORM_SRGB,

        BC2_UNORM,
        BC2_UNORM_SRGB,

        BC3_UNORM,
        BC3_UNORM_SRGB,

        BC4_UNORM,
        BC4_SNORM,

        BC5_UNORM,
        BC5_SNORM,

        BC6H_UF16,
        BC6H_SF16,

        BC7_UNORM,
        BC7_UNORM_SRGB,

        UNKNOWN
    };

    inline constexpr DXGI_FORMAT ToDXGIFormat(ResourceFormat format)
    {
        switch (format)
        {
        case ResourceFormat::R16_TYPELESS:return DXGI_FORMAT_R16_TYPELESS;
        case ResourceFormat::R32_TYPELESS:return DXGI_FORMAT_R32_TYPELESS;
            // --- RGBA32 ---
        case ResourceFormat::RGBA32_FLOAT: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case ResourceFormat::RGBA32_UINT:  return DXGI_FORMAT_R32G32B32A32_UINT;
        case ResourceFormat::RGBA32_SINT:  return DXGI_FORMAT_R32G32B32A32_SINT;

            // --- RGB32 ---
        case ResourceFormat::RGB32_FLOAT: return DXGI_FORMAT_R32G32B32_FLOAT;
        case ResourceFormat::RGB32_UINT:  return DXGI_FORMAT_R32G32B32_UINT;
        case ResourceFormat::RGB32_SINT:  return DXGI_FORMAT_R32G32B32_SINT;

            // --- RGBA16 ---
        case ResourceFormat::RGBA16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case ResourceFormat::RGBA16_UNORM: return DXGI_FORMAT_R16G16B16A16_UNORM;
        case ResourceFormat::RGBA16_UINT:  return DXGI_FORMAT_R16G16B16A16_UINT;
        case ResourceFormat::RGBA16_SNORM: return DXGI_FORMAT_R16G16B16A16_SNORM;
        case ResourceFormat::RGBA16_SINT:  return DXGI_FORMAT_R16G16B16A16_SINT;

            // --- RG32 ---
        case ResourceFormat::RG32_FLOAT: return DXGI_FORMAT_R32G32_FLOAT;
        case ResourceFormat::RG32_UINT:  return DXGI_FORMAT_R32G32_UINT;
        case ResourceFormat::RG32_SINT:  return DXGI_FORMAT_R32G32_SINT;

            // --- RGBA8 ---
        case ResourceFormat::RGBA8_UNORM:        return DXGI_FORMAT_R8G8B8A8_UNORM;
        case ResourceFormat::RGBA8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case ResourceFormat::RGBA8_UINT:         return DXGI_FORMAT_R8G8B8A8_UINT;
        case ResourceFormat::RGBA8_SNORM:        return DXGI_FORMAT_R8G8B8A8_SNORM;
        case ResourceFormat::RGBA8_SINT:         return DXGI_FORMAT_R8G8B8A8_SINT;

            // --- BGRA8 ---
        case ResourceFormat::BGRA8_UNORM:        return DXGI_FORMAT_B8G8R8A8_UNORM;
        case ResourceFormat::BGRA8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

            // --- Packed ---
        case ResourceFormat::R10G10B10A2_UNORM: return DXGI_FORMAT_R10G10B10A2_UNORM;
        case ResourceFormat::R10G10B10A2_UINT:  return DXGI_FORMAT_R10G10B10A2_UINT;
        case ResourceFormat::R11G11B10_FLOAT:   return DXGI_FORMAT_R11G11B10_FLOAT;

        case ResourceFormat::RGB9E5_SHAREDEXP: return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;

            // --- RG16 ---
        case ResourceFormat::RG16_FLOAT: return DXGI_FORMAT_R16G16_FLOAT;
        case ResourceFormat::RG16_UNORM: return DXGI_FORMAT_R16G16_UNORM;
        case ResourceFormat::RG16_UINT:  return DXGI_FORMAT_R16G16_UINT;
        case ResourceFormat::RG16_SNORM: return DXGI_FORMAT_R16G16_SNORM;
        case ResourceFormat::RG16_SINT:  return DXGI_FORMAT_R16G16_SINT;

            // --- RG8 ---
        case ResourceFormat::RG8_UNORM: return DXGI_FORMAT_R8G8_UNORM;
        case ResourceFormat::RG8_UINT:  return DXGI_FORMAT_R8G8_UINT;
        case ResourceFormat::RG8_SNORM: return DXGI_FORMAT_R8G8_SNORM;
        case ResourceFormat::RG8_SINT:  return DXGI_FORMAT_R8G8_SINT;

            // --- R32 ---
        case ResourceFormat::R32_FLOAT: return DXGI_FORMAT_R32_FLOAT;
        case ResourceFormat::R32_UINT:  return DXGI_FORMAT_R32_UINT;
        case ResourceFormat::R32_SINT:  return DXGI_FORMAT_R32_SINT;

            // --- R16 ---
        case ResourceFormat::R16_FLOAT: return DXGI_FORMAT_R16_FLOAT;
        case ResourceFormat::R16_UNORM: return DXGI_FORMAT_R16_UNORM;
        case ResourceFormat::R16_UINT:  return DXGI_FORMAT_R16_UINT;
        case ResourceFormat::R16_SNORM: return DXGI_FORMAT_R16_SNORM;
        case ResourceFormat::R16_SINT:  return DXGI_FORMAT_R16_SINT;

            // --- R8 ---
        case ResourceFormat::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
        case ResourceFormat::R8_UINT:  return DXGI_FORMAT_R8_UINT;
        case ResourceFormat::R8_SNORM: return DXGI_FORMAT_R8_SNORM;
        case ResourceFormat::R8_SINT:  return DXGI_FORMAT_R8_SINT;

            // --- Depth ---
        case ResourceFormat::D32_FLOAT:         return DXGI_FORMAT_D32_FLOAT;
        case ResourceFormat::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case ResourceFormat::D16_UNORM:         return DXGI_FORMAT_D16_UNORM;

            // --- BC ---
        case ResourceFormat::BC1_UNORM:        return DXGI_FORMAT_BC1_UNORM;
        case ResourceFormat::BC1_UNORM_SRGB:   return DXGI_FORMAT_BC1_UNORM_SRGB;
        case ResourceFormat::BC2_UNORM:        return DXGI_FORMAT_BC2_UNORM;
        case ResourceFormat::BC2_UNORM_SRGB:   return DXGI_FORMAT_BC2_UNORM_SRGB;
        case ResourceFormat::BC3_UNORM:        return DXGI_FORMAT_BC3_UNORM;
        case ResourceFormat::BC3_UNORM_SRGB:   return DXGI_FORMAT_BC3_UNORM_SRGB;
        case ResourceFormat::BC4_UNORM:        return DXGI_FORMAT_BC4_UNORM;
        case ResourceFormat::BC4_SNORM:        return DXGI_FORMAT_BC4_SNORM;
        case ResourceFormat::BC5_UNORM:        return DXGI_FORMAT_BC5_UNORM;
        case ResourceFormat::BC5_SNORM:        return DXGI_FORMAT_BC5_SNORM;
        case ResourceFormat::BC6H_UF16:        return DXGI_FORMAT_BC6H_UF16;
        case ResourceFormat::BC6H_SF16:        return DXGI_FORMAT_BC6H_SF16;
        case ResourceFormat::BC7_UNORM:        return DXGI_FORMAT_BC7_UNORM;
        case ResourceFormat::BC7_UNORM_SRGB:   return DXGI_FORMAT_BC7_UNORM_SRGB;

        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    inline constexpr ResourceFormat FromDXGIFormat(DXGI_FORMAT format)
    {
        switch (format)
        {
        case DXGI_FORMAT_R16_TYPELESS:        return ResourceFormat::R16_TYPELESS;
        case DXGI_FORMAT_R32_TYPELESS:        return ResourceFormat::R32_TYPELESS;

            // --- RGBA32 ---
        case DXGI_FORMAT_R32G32B32A32_FLOAT:  return ResourceFormat::RGBA32_FLOAT;
        case DXGI_FORMAT_R32G32B32A32_UINT:   return ResourceFormat::RGBA32_UINT;
        case DXGI_FORMAT_R32G32B32A32_SINT:   return ResourceFormat::RGBA32_SINT;

            // --- RGB32 ---
        case DXGI_FORMAT_R32G32B32_FLOAT:     return ResourceFormat::RGB32_FLOAT;
        case DXGI_FORMAT_R32G32B32_UINT:      return ResourceFormat::RGB32_UINT;
        case DXGI_FORMAT_R32G32B32_SINT:      return ResourceFormat::RGB32_SINT;

            // --- RGBA16 ---
        case DXGI_FORMAT_R16G16B16A16_FLOAT:  return ResourceFormat::RGBA16_FLOAT;
        case DXGI_FORMAT_R16G16B16A16_UNORM:  return ResourceFormat::RGBA16_UNORM;
        case DXGI_FORMAT_R16G16B16A16_UINT:   return ResourceFormat::RGBA16_UINT;
        case DXGI_FORMAT_R16G16B16A16_SNORM:  return ResourceFormat::RGBA16_SNORM;
        case DXGI_FORMAT_R16G16B16A16_SINT:   return ResourceFormat::RGBA16_SINT;

            // --- RG32 ---
        case DXGI_FORMAT_R32G32_FLOAT:        return ResourceFormat::RG32_FLOAT;
        case DXGI_FORMAT_R32G32_UINT:         return ResourceFormat::RG32_UINT;
        case DXGI_FORMAT_R32G32_SINT:         return ResourceFormat::RG32_SINT;

            // --- RGBA8 ---
        case DXGI_FORMAT_R8G8B8A8_UNORM:      return ResourceFormat::RGBA8_UNORM;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return ResourceFormat::RGBA8_UNORM_SRGB;
        case DXGI_FORMAT_R8G8B8A8_UINT:       return ResourceFormat::RGBA8_UINT;
        case DXGI_FORMAT_R8G8B8A8_SNORM:      return ResourceFormat::RGBA8_SNORM;
        case DXGI_FORMAT_R8G8B8A8_SINT:       return ResourceFormat::RGBA8_SINT;

            // --- BGRA8 ---
        case DXGI_FORMAT_B8G8R8A8_UNORM:      return ResourceFormat::BGRA8_UNORM;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return ResourceFormat::BGRA8_UNORM_SRGB;

            // --- Packed ---
        case DXGI_FORMAT_R10G10B10A2_UNORM:   return ResourceFormat::R10G10B10A2_UNORM;
        case DXGI_FORMAT_R10G10B10A2_UINT:    return ResourceFormat::R10G10B10A2_UINT;
        case DXGI_FORMAT_R11G11B10_FLOAT:     return ResourceFormat::R11G11B10_FLOAT;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:  return ResourceFormat::RGB9E5_SHAREDEXP;

            // --- RG16 ---
        case DXGI_FORMAT_R16G16_FLOAT:        return ResourceFormat::RG16_FLOAT;
        case DXGI_FORMAT_R16G16_UNORM:        return ResourceFormat::RG16_UNORM;
        case DXGI_FORMAT_R16G16_UINT:         return ResourceFormat::RG16_UINT;
        case DXGI_FORMAT_R16G16_SNORM:        return ResourceFormat::RG16_SNORM;
        case DXGI_FORMAT_R16G16_SINT:         return ResourceFormat::RG16_SINT;

            // --- RG8 ---
        case DXGI_FORMAT_R8G8_UNORM:          return ResourceFormat::RG8_UNORM;
        case DXGI_FORMAT_R8G8_UINT:           return ResourceFormat::RG8_UINT;
        case DXGI_FORMAT_R8G8_SNORM:          return ResourceFormat::RG8_SNORM;
        case DXGI_FORMAT_R8G8_SINT:           return ResourceFormat::RG8_SINT;

            // --- R32 ---
        case DXGI_FORMAT_R32_FLOAT:           return ResourceFormat::R32_FLOAT;
        case DXGI_FORMAT_R32_UINT:            return ResourceFormat::R32_UINT;
        case DXGI_FORMAT_R32_SINT:            return ResourceFormat::R32_SINT;

            // --- R16 ---
        case DXGI_FORMAT_R16_FLOAT:           return ResourceFormat::R16_FLOAT;
        case DXGI_FORMAT_R16_UNORM:           return ResourceFormat::R16_UNORM;
        case DXGI_FORMAT_R16_UINT:            return ResourceFormat::R16_UINT;
        case DXGI_FORMAT_R16_SNORM:           return ResourceFormat::R16_SNORM;
        case DXGI_FORMAT_R16_SINT:            return ResourceFormat::R16_SINT;

            // --- R8 ---
        case DXGI_FORMAT_R8_UNORM:            return ResourceFormat::R8_UNORM;
        case DXGI_FORMAT_R8_UINT:             return ResourceFormat::R8_UINT;
        case DXGI_FORMAT_R8_SNORM:            return ResourceFormat::R8_SNORM;
        case DXGI_FORMAT_R8_SINT:             return ResourceFormat::R8_SINT;

            // --- Depth ---
        case DXGI_FORMAT_D32_FLOAT:           return ResourceFormat::D32_FLOAT;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:   return ResourceFormat::D24_UNORM_S8_UINT;
        case DXGI_FORMAT_D16_UNORM:           return ResourceFormat::D16_UNORM;

            // --- BC ---
        case DXGI_FORMAT_BC1_UNORM:           return ResourceFormat::BC1_UNORM;
        case DXGI_FORMAT_BC1_UNORM_SRGB:      return ResourceFormat::BC1_UNORM_SRGB;
        case DXGI_FORMAT_BC2_UNORM:           return ResourceFormat::BC2_UNORM;
        case DXGI_FORMAT_BC2_UNORM_SRGB:      return ResourceFormat::BC2_UNORM_SRGB;
        case DXGI_FORMAT_BC3_UNORM:           return ResourceFormat::BC3_UNORM;
        case DXGI_FORMAT_BC3_UNORM_SRGB:      return ResourceFormat::BC3_UNORM_SRGB;
        case DXGI_FORMAT_BC4_UNORM:           return ResourceFormat::BC4_UNORM;
        case DXGI_FORMAT_BC4_SNORM:           return ResourceFormat::BC4_SNORM;
        case DXGI_FORMAT_BC5_UNORM:           return ResourceFormat::BC5_UNORM;
        case DXGI_FORMAT_BC5_SNORM:           return ResourceFormat::BC5_SNORM;
        case DXGI_FORMAT_BC6H_UF16:           return ResourceFormat::BC6H_UF16;
        case DXGI_FORMAT_BC6H_SF16:           return ResourceFormat::BC6H_SF16;
        case DXGI_FORMAT_BC7_UNORM:           return ResourceFormat::BC7_UNORM;
        case DXGI_FORMAT_BC7_UNORM_SRGB:      return ResourceFormat::BC7_UNORM_SRGB;

        default:
            return ResourceFormat::UNKNOWN;
        }
    }

    inline constexpr cesar::Uint32 GetFormatStride(ResourceFormat format)
    {
        switch (format)
        {
            // --- BC formats (bytes per 4x4 block) ---
        case ResourceFormat::BC1_UNORM:
        case ResourceFormat::BC1_UNORM_SRGB:
        case ResourceFormat::BC4_UNORM:
        case ResourceFormat::BC4_SNORM:
            return 8u;

        case ResourceFormat::BC2_UNORM:
        case ResourceFormat::BC2_UNORM_SRGB:
        case ResourceFormat::BC3_UNORM:
        case ResourceFormat::BC3_UNORM_SRGB:
        case ResourceFormat::BC5_UNORM:
        case ResourceFormat::BC5_SNORM:
        case ResourceFormat::BC6H_UF16:
        case ResourceFormat::BC6H_SF16:
        case ResourceFormat::BC7_UNORM:
        case ResourceFormat::BC7_UNORM_SRGB:
            return 16u;

            // --- 128-bit (16 bytes) ---
        case ResourceFormat::RGBA32_FLOAT:
        case ResourceFormat::RGBA32_UINT:
        case ResourceFormat::RGBA32_SINT:
            return 16u;

            // --- 96-bit (12 bytes) ---
        case ResourceFormat::RGB32_FLOAT:
        case ResourceFormat::RGB32_UINT:
        case ResourceFormat::RGB32_SINT:
            return 12u;

            // --- 64-bit (8 bytes) ---
        case ResourceFormat::RGBA16_FLOAT:
        case ResourceFormat::RGBA16_UNORM:
        case ResourceFormat::RGBA16_UINT:
        case ResourceFormat::RGBA16_SNORM:
        case ResourceFormat::RGBA16_SINT:

        case ResourceFormat::RG32_FLOAT:
        case ResourceFormat::RG32_UINT:
        case ResourceFormat::RG32_SINT:
            return 8u;

            // --- 32-bit (4 bytes) ---
        case ResourceFormat::RGBA8_UNORM:
        case ResourceFormat::RGBA8_UNORM_SRGB:
        case ResourceFormat::RGBA8_UINT:
        case ResourceFormat::RGBA8_SNORM:
        case ResourceFormat::RGBA8_SINT:

        case ResourceFormat::BGRA8_UNORM:
        case ResourceFormat::BGRA8_UNORM_SRGB:

        case ResourceFormat::R10G10B10A2_UNORM:
        case ResourceFormat::R10G10B10A2_UINT:
        case ResourceFormat::R11G11B10_FLOAT:

        case ResourceFormat::RG16_FLOAT:
        case ResourceFormat::RG16_UNORM:
        case ResourceFormat::RG16_UINT:
        case ResourceFormat::RG16_SNORM:
        case ResourceFormat::RG16_SINT:

        case ResourceFormat::R32_FLOAT:
        case ResourceFormat::R32_UINT:
        case ResourceFormat::R32_SINT:

        case ResourceFormat::D32_FLOAT:
        case ResourceFormat::D24_UNORM_S8_UINT:

        case ResourceFormat::RGB9E5_SHAREDEXP:
            return 4u;

            // --- 16-bit (2 bytes) ---
        case ResourceFormat::RG8_UNORM:
        case ResourceFormat::RG8_UINT:
        case ResourceFormat::RG8_SNORM:
        case ResourceFormat::RG8_SINT:

        case ResourceFormat::R16_FLOAT:
        case ResourceFormat::R16_UNORM:
        case ResourceFormat::R16_UINT:
        case ResourceFormat::R16_SNORM:
        case ResourceFormat::R16_SINT:

        case ResourceFormat::D16_UNORM:
            return 2u;

            // --- 8-bit (1 byte) ---
        case ResourceFormat::R8_UNORM:
        case ResourceFormat::R8_UINT:
        case ResourceFormat::R8_SNORM:
        case ResourceFormat::R8_SINT:
            return 1u;

        default:
            break;
        }

        return 0u; // safer than 16 (invalid format detection)
    }
}

