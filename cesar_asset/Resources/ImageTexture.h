#pragma once
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"

#include "../../cesar_render_core/OfflineContext.h"

namespace cesar {

	enum class ImageTextureFormat
	{
		R8  = 0,
		R16 = 1,
		R32 = 2,

		RG8  = 3,
		RG16 = 4,
		RG32 = 5,

		RGB8  = 6,
		RGB16 = 7,
		RGB32 = 8,

		RGBA8  = 9,
		RGBA16 = 10,
		RGBA32 = 11,

		Unknown 
	};
	inline constexpr cesar::Uint32 GetChannelCount(ImageTextureFormat format) {
		switch (format) {
		    case ImageTextureFormat::R8:
		    case ImageTextureFormat::R16:
		    case ImageTextureFormat::R32:
		    	return 1;
		    
		    case ImageTextureFormat::RG8:
		    case ImageTextureFormat::RG16:
		    case ImageTextureFormat::RG32:
		    	return 2;
		    
		    case ImageTextureFormat::RGB8:
		    case ImageTextureFormat::RGB16:
		    case ImageTextureFormat::RGB32:
		    	return 3;
		    
		    case ImageTextureFormat::RGBA8:
		    case ImageTextureFormat::RGBA16:
		    case ImageTextureFormat::RGBA32:
		    	return 4;
		    
		    default:
		    	return 0;
		}
	}
	inline constexpr cesar::Uint32 GetBitPerChannel(ImageTextureFormat format) {
		switch (format) {
		    case ImageTextureFormat::R8:
		    case ImageTextureFormat::RG8:
		    case ImageTextureFormat::RGB8:
		    case ImageTextureFormat::RGBA8:
		    	return 8;
		    
		    case ImageTextureFormat::R16:
		    case ImageTextureFormat::RG16:
		    case ImageTextureFormat::RGB16:
		    case ImageTextureFormat::RGBA16:
		    	return 16;
		    
		    case ImageTextureFormat::R32:
		    case ImageTextureFormat::RG32:
		    case ImageTextureFormat::RGB32:
		    case ImageTextureFormat::RGBA32:
		    	return 32;
		    
		    default:
		    	return 0;
		}
	}

	inline constexpr ImageTextureFormat GetImageTextureFormat(cesar::Uint32 channel_count, cesar::Uint32 bit_per_channel) {
		switch (channel_count) {
			case 1:
				switch (bit_per_channel) {
					case 8:  return ImageTextureFormat::R8;
					case 16: return ImageTextureFormat::R16;
					case 32: return ImageTextureFormat::R32;
					default: return ImageTextureFormat::Unknown;
				}
			case 2:
				switch (bit_per_channel) {
					case 8:  return ImageTextureFormat::RG8;
					case 16: return ImageTextureFormat::RG16;
					case 32: return ImageTextureFormat::RG32;
					default: return ImageTextureFormat::Unknown;
				}
			case 3:
				switch (bit_per_channel) {
					case 8:  return ImageTextureFormat::RGB8;
					case 16: return ImageTextureFormat::RGB16;
					case 32: return ImageTextureFormat::RGB32;
					default: return ImageTextureFormat::Unknown;
				}
			case 4:
				switch (bit_per_channel) {
					case 8:  return ImageTextureFormat::RGBA8;
					case 16: return ImageTextureFormat::RGBA16;
					case 32: return ImageTextureFormat::RGBA32;
					default: return ImageTextureFormat::Unknown;
				}
			default:
				return ImageTextureFormat::Unknown;
		}
	}

	inline Uint32 GetMipCount(Uint32 width, Uint32 height, Uint32 depth = 1)
	{
		Uint32 maxDim = std::max<Uint32>(width, height);
		return static_cast<Uint32>(std::floor(std::log2(maxDim))) + 1;
		return 1;
	}

	struct ImageTexture : public Resource
	{
		Uint32 width;
		Uint32 height;
		ImageTextureFormat format;

		std::unique_ptr<Texture> gpu_texture;
		Uint32 srv_index = 0;
	};

	template<>struct ResourceTypeTrait<ImageTexture> { static constexpr ResourceType type = ResourceType::ImageTexture; };
}