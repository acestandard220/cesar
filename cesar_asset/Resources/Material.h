#pragma once
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"
#include "ImageTexture.h"

#include <SimpleMath/SimpleMath.h>
using namespace DirectX::SimpleMath;

namespace cesar {

	static constexpr Uint32 START_MATERIAL_COUNT = 256;

	enum class ImageTextureChannel
	{
		R = 0,
		G = 1,
		B = 2,
		A = 3
	};

	struct AlbedoProperty
	{
		Vector4 color;
		Uint32  map_index;
	};

	struct NormalProperty {
		Float  normal_strength;
		Uint32 map_index;
	};

	struct AmbientOcclusionProperty {
		Float  ao_strength;
		Uint32 map_index;
		ImageTextureChannel channel;
	};

	struct MetallicProperty {
		Float  metallic_strength;
		Uint32 map_index;
		ImageTextureChannel channel;
	};

	struct RoughnessProperty {
		Float  roughness_strength;
		Uint32 map_index;
		ImageTextureChannel channel;
	};

	struct GPU_STRUCTURE MaterialData {
		AlbedoProperty           albedo;
		NormalProperty           normal;
		AmbientOcclusionProperty ao;
		MetallicProperty         metallic;
		RoughnessProperty        roughness;

		Vector2 map_scale;
	};

	struct Material : public Resource
	{
		MaterialData* material_data = nullptr;
	};

	template<>struct ResourceTypeTrait<Material> { static constexpr ResourceType type = ResourceType::Material; };
}