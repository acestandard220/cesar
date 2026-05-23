#include "ImageTextureIO.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include "../cesar_render_core/OfflineContext.h"

using namespace cesar;
namespace cesar {

	std::unique_ptr<Resource> ImageTextureIO::LoadFromFile(ResourceLoadDesc& load_desc)
	{
		ImageLoadDesc* image_load_desc = static_cast<ImageLoadDesc*>(&load_desc);
        if (!HasFlag(image_load_desc->flags, ImageLoadFlags::LoadFromMemory))
            return LoadFromFileNonNative(image_load_desc);
        else
            return LoadFromMemory(image_load_desc); // TODO: Implement LoadFromMemory functionality.
	}

	void ImageTextureIO::SaveToDisk(const std::filesystem::path& file_path)
	{
	
	}

    std::unique_ptr<Resource> ImageTextureIO::LoadFromFileNonNative(ImageLoadDesc* load_desc)
    {
        ImageLoadDesc image_load_desc = static_cast<ImageLoadDesc>(*load_desc);

        std::unique_ptr<ImageTexture> image_texture = std::make_unique<ImageTexture>();
        void* data;
        Int32 width, height, nChannel, bpc;

        stbi_set_flip_vertically_on_load(HasFlag(image_load_desc.flags, ImageLoadFlags::FlipUV));

        Bool is16Bit = false;
        if (stbi_is_16_bit(load_desc->file_path.string().c_str())) {
            bpc = 16;
            data = stbi_load_16(image_load_desc.file_path.string().c_str(), &width, &height, &nChannel, 4);
            is16Bit = true;
        }
        else {
            bpc = 8;
            data = stbi_load(image_load_desc.file_path.string().c_str(), &width, &height, &nChannel, 4);
            is16Bit = false;
        }

        if (!data) {
            LOG_ERROR("Failed to load Image Texture File - {}", image_load_desc.file_path.string().c_str());
            return nullptr;
        }

        nChannel = 4;
        image_texture->width  = width;
        image_texture->height = height;
        image_texture->format = GetImageTextureFormat(nChannel, bpc);
        load_desc->uuid = CESAR_INVALID_UUID;

        OfflineContext* context = resource_cache->offline_context;

        TextureDesc texture_desc{};
        texture_desc.type = TextureType::Texture2D;
        texture_desc.usage = ResourceUsage::Default;
        
        switch (bpc) {
            case 8:
                texture_desc.format = ResourceFormat::RGBA8_UNORM;
                break;
            case 16:
                texture_desc.format = ResourceFormat::RGBA16_FLOAT;
                break;
            case 32:
                texture_desc.format = ResourceFormat::RGBA32_FLOAT;
                break;
        }

		const Uint32 max_mip_count = GetMipCount(width, height);

        texture_desc.height = height;
        texture_desc.width  = width;
        texture_desc.array_size = 1;
        texture_desc.mips = HasFlag(image_load_desc.flags, ImageLoadFlags::GenerateMips) ? max_mip_count : 1;
        texture_desc.bind_flag = ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess;
        texture_desc.misc_flag = TextureMiscFlag::SRGB;
        texture_desc.intial_state = ResourceState::CopyDst;

        image_texture->gpu_texture = context->CreateTexture(data, texture_desc);
        image_texture->srv_index   = context->AllocateBindlessTextureSRV(image_texture->gpu_texture.get());
		context->GenerateMips(image_texture->gpu_texture.get(), image_texture->srv_index);

        stbi_image_free(data);
        return image_texture;
    }

    std::unique_ptr<Resource> ImageTextureIO::LoadFromMemory(ImageLoadDesc* load_desc)
    {
		struct _data_
		{
			void* data;
			size_t size;
		};

        std::unique_ptr<ImageTexture> image_texture = std::make_unique<ImageTexture>();

		_data_* data = reinterpret_cast<_data_*>(load_desc->payload);

        Int32 width, height, nChannel, bpc;
        stbi_set_flip_vertically_on_load(HasFlag(load_desc->flags, ImageLoadFlags::FlipUV));

		Bool is16Bit = false;
        if (stbi_is_16_bit_from_memory(static_cast<const stbi_uc*>(data->data), data->size))
        {
            data->data = stbi_load_16_from_memory(static_cast<const stbi_uc*>(data->data), data->size, &width, &height, &nChannel, 4);
			bpc = 16;
            is16Bit = true;
        }
        else
        {
            data->data = stbi_load_from_memory(static_cast<const stbi_uc*>(data->data), data->size, &width, &height, &nChannel, 4);
			bpc = 8;
			is16Bit = false;
        }

        nChannel = 4;
        image_texture->width = width;
        image_texture->height = height;
        image_texture->format = GetImageTextureFormat(nChannel, bpc);
        load_desc->uuid = CESAR_INVALID_UUID;

        OfflineContext* context = resource_cache->offline_context;

        TextureDesc texture_desc{};
        texture_desc.type = TextureType::Texture2D;
        texture_desc.usage = ResourceUsage::Default;

        switch (bpc) {
        case 8:
            texture_desc.format = ResourceFormat::RGBA8_UNORM;
            break;
        case 16:
            texture_desc.format = ResourceFormat::RGBA16_FLOAT;
            break;
        case 32:
            texture_desc.format = ResourceFormat::RGBA32_FLOAT;
            break;
        }

        const Uint32 max_mip_count = GetMipCount(width, height);

        texture_desc.height = height;
        texture_desc.width = width;
        texture_desc.array_size = 1;
        texture_desc.mips = HasFlag(load_desc->flags, ImageLoadFlags::GenerateMips) ? max_mip_count : 1;
        texture_desc.bind_flag = ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess;
        texture_desc.misc_flag = TextureMiscFlag::SRGB;
        texture_desc.intial_state = ResourceState::CopyDst;

        image_texture->gpu_texture = context->CreateTexture(data->data, texture_desc);
        image_texture->srv_index = context->AllocateBindlessTextureSRV(image_texture->gpu_texture.get());
        context->GenerateMips(image_texture->gpu_texture.get(), image_texture->srv_index);

        stbi_image_free(data->data);
        return image_texture;
    }

}
