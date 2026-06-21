#pragma once
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"
#include "ResourceIO.h"
#include "../Resources/ImageTexture.h"
#include "../ResourceCache.h"

#include <filesystem>

namespace cesar {

	enum class ImageLoadFlags : Int32
	{
		None = 0,
		FlipUV = BIT(1),
		GenerateMips = BIT(2),

		LoadFromMemory = BIT(3),
		LoadFromMaterial = BIT(4)


		//NormalMap = 1<<3,
		//AlbedoMap =1<<4,
		//AOMap = 1<<5,
		//RoughnessMap = 1<<6,
		//MetallicMap = 1<<7
	};
	CESAR_ENABLE_ENUM_OPS(ImageLoadFlags);

	struct ImageAssetHeader : public CesarAssetHeader
	{
		Uint32 width;
		Uint32 height;

		Uint32 mip_count;
		Uint32 array_size;

		TextureType tex_type;
		TextureMiscFlag misc_flag;
		ResourceFormat format;

		Uint32 pad;
		Uint64 copyable_size;
	};

	struct ImageLoadDesc : public ResourceLoadDesc
	{
		ImageLoadFlags flags = ImageLoadFlags::None;

		ImageLoadDesc()
		{
			type = ResourceType::ImageTexture;
		}
	};

	class ImageTextureIO : public IResourceIO
	{
	public:
		ImageTextureIO() = default;
		ImageTextureIO(ResourceCache* resource_cache)
			:IResourceIO(resource_cache) {

		}
		virtual ~ImageTextureIO() = default;

		virtual std::unique_ptr<Resource> LoadFromFile(ResourceLoadDesc& load_desc) override;
		virtual void SaveToDisk(const ResourceSaveDesc& save_desc, void* resource) override;
	private:
		std::unique_ptr<Resource> LoadNative(ImageLoadDesc* load_desc);
		std::unique_ptr<Resource> LoadFromFileNonNative(ImageLoadDesc* load_desc);
		std::unique_ptr<Resource> LoadFromMemory(ImageLoadDesc* load_desc);
	};

}