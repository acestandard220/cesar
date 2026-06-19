#pragma once
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"

#include "../../cesar_render_core/OfflineContext.h"

namespace cesar {
	enum class MapType
	{
		Albedo, Normal, AO, Roughness, Metallic
	};

	struct ImageTexture : public Resource
	{
		Uint32 width;
		Uint32 height;

		void* data; //Can be raw pixel data/Can be upload buffer/Can be readbackbuffer// This is usually a hack to free the pointer later
		std::unique_ptr<Texture> gpu_texture;
		Uint32 srv_index = 0;
	};

	template<>struct ResourceTypeTrait<ImageTexture> { static constexpr ResourceType type = ResourceType::ImageTexture; };
}