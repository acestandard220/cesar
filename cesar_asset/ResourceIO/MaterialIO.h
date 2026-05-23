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
		LoadFromMtl      = BIT(2),
		LoadFromGlb_Gltf = BIT(3),
		LoadFromFBX      = BIT(4)
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
		virtual void SaveToDisk(const std::filesystem::path& file_path) override;

	private:
		MaterialData* LoadMaterialFromMtl(const std::string& mtlPath, const std::string& materialName);
		MaterialData* LoadMaterialFromGlbGltf(MaterialLoadDesc* load_desc);
	};

}
