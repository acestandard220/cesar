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
		FlipUV = 1 << 0,
		GenerateMips = 1 << 1,

		LoadFromMemory = 1 << 2
	};
	CESAR_ENABLE_ENUM_OPS(ImageLoadFlags);

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
		virtual void SaveToDisk(const std::filesystem::path& file_path) override;
	private:
		std::unique_ptr<Resource> LoadFromFileNonNative(ImageLoadDesc* load_desc);
		std::unique_ptr<Resource> LoadFromMemory(ImageLoadDesc* load_desc);
	};

}