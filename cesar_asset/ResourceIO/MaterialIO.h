#pragma once 
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"
#include "ResourceIO.h"
#include "../Resources/Material.h"

#include <filesystem>

namespace cesar
{
	enum class MaterialLoadFlags : Uint32
	{
		None             = 0,
		LoadFromMeshIO   = BIT(1),
		LoadFromGlb_Gltf = BIT(2),
		LoadFromFBX      = BIT(3)
	};
	CESAR_ENABLE_ENUM_OPS(MaterialLoadFlags);

	struct MaterialLoadDesc : public ResourceLoadDesc
	{
		MaterialLoadFlags flags = MaterialLoadFlags::None;

		MaterialLoadDesc()
		{
			type = ResourceType::Material;
		}
	};

	// CesarAssetHeader | MaterialData | Unique_Map_Count | [ImagePath.Size] | ImagePaths
	struct MaterialAssetHeader : public CesarAssetHeader
	{
		MaterialData material_data;
		Uint32 unique_map_count;
		Uint32 pad;
	};

	class MaterialIO :public IResourceIO
	{
	public:
		MaterialIO() = default;
		MaterialIO(ResourceCache* resource_cache)
			:IResourceIO(resource_cache)
		{

		}
		virtual ~MaterialIO() = default;

		virtual std::unique_ptr<Resource> LoadFromFile(ResourceLoadDesc& load_desc) override;
		virtual void SaveToDisk(const ResourceSaveDesc& save_desc, void* resource) override;

	private:
		std::unique_ptr<Material> LoadNative(MaterialLoadDesc* load_desc);
		std::unique_ptr<Material> LoadGlbGltfMaterial(MaterialLoadDesc* load_desc);
		std::unique_ptr<Material> LoadAssimpMaterial(MaterialLoadDesc* load_desc);
	};

}
