#include "ImageTextureIO.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include "../cesar_render_core/OfflineContext.h"
#include "../../cesar_core/Core/ThreadPool.h"

using namespace cesar;
namespace cesar {

    static void UploadTextureData(OfflineContext*  context, TextureJobManager* loader, ImageLoadDesc* load_desc, Buffer* upload_buffer, ImageTexture* im_tex, Bool gen_mips)
    {
        Texture* texture = im_tex->gpu_texture.get();
        Uint32 srv_index = im_tex->srv_index;
        if (HasFlag(load_desc->flags, ImageLoadFlags::LoadFromMaterial))
        {
            loader->Submit([context, upload_buffer, texture, srv_index, gen_mips]() {
                context->UploadTextureData(upload_buffer, texture);
                if(gen_mips)
                    context->GenerateMips(texture, srv_index);
            });
        }
        else {
            loader->ExecuteImmediate([context, upload_buffer, texture, srv_index, gen_mips]() {
                context->UploadTextureData(upload_buffer, texture);
                if (gen_mips)
                    context->GenerateMips(texture, srv_index);
            });
            delete upload_buffer;
            im_tex->data = nullptr;
        }
    }

	std::unique_ptr<Resource> ImageTextureIO::LoadFromFile(ResourceLoadDesc& load_desc)
	{
		ImageLoadDesc* image_load_desc = static_cast<ImageLoadDesc*>(&load_desc);
        if (load_desc.is_cooked) {
            return LoadNative(image_load_desc);
        }

        if (!HasFlag(image_load_desc->flags, ImageLoadFlags::LoadFromMemory))
            return LoadFromFileNonNative(image_load_desc);
        else
            return LoadFromMemory(image_load_desc); 
	}

    void ImageTextureIO::SaveToDisk(const ResourceLoadDesc& load_desc, void* resource)
    {
        ImageTexture* image_texture = static_cast<ImageTexture*>(resource);
        Texture* gpu_texture = image_texture->gpu_texture.get();

        OfflineContext* offline_context = resource_cache->offline_context;
        ResourceCache* cache = resource_cache;

        const filespace::filepath cooked_path = cache->GetCookedAssetPath(load_desc.file_path);
        const TextureDesc desc = gpu_texture->GetDesc();

        ImageAssetHeader header{};
        header.version = 1;
        header.uuid = image_texture->GetUUID();
        header.type = ResourceType::ImageTexture;
        header.ref_count = image_texture->GetRefCount();
        header.width = image_texture->width;
        header.height = image_texture->height;
        header.copyable_size = gpu_texture->GetTextureCopyableSize();
        header.mip_count = desc.mips;
        header.array_size = desc.array_size;
        header.format = desc.format;
        header.tex_type = desc.type;
        header.misc_flag = desc.misc_flag;

        const Uint32 subresource_count = gpu_texture->GetSubresourceCount();
        std::vector<Subresource> asset_subresources(subresource_count);
        gpu_texture->GetTextureCopyableSubresources(asset_subresources);

        gThreadPool.SubmitJob([cache, offline_context, gpu_texture, header, cooked_path,
            asset_subresources = std::move(asset_subresources)]() mutable
            {
                Buffer* data_buffer = nullptr;

                cache->texture_loader->ExecuteImmediate([&]() {
                    data_buffer = offline_context->GetTexturePixels(gpu_texture);
                    });

                if (!data_buffer) {
                    LOG_ERROR("Failed to get texture pixels.");
                    return;
                }

                void* data = data_buffer->GetPersistentPointer();

                std::ofstream output(cooked_path, std::ios::binary | std::ios::out);
                if (!output) {
                    LOG_ERROR("Failed to save asset to disk.");
                    delete data_buffer;
                    return;
                }

                constexpr std::size_t base_size = CESAR_SIZEOF(CesarAssetHeader);
                constexpr std::size_t total_size = CESAR_SIZEOF(ImageAssetHeader);

                const char* header_bytes = reinterpret_cast<const char*>(&header);
                output.write(header_bytes, static_cast<std::streamsize>(base_size));
                output.write(header_bytes + base_size, static_cast<std::streamsize>(total_size - base_size));
                output.write(reinterpret_cast<const char*>(asset_subresources.data()),
                    static_cast<std::streamsize>(CESAR_SIZEOF_BUFFER(Subresource, asset_subresources.size())));
                output.write(reinterpret_cast<const char*>(data),
                    static_cast<std::streamsize>(data_buffer->GetSize()));

                delete data_buffer;
            });
    }

    //TODO: This Load path assumes non cube textures
    std::unique_ptr<Resource> ImageTextureIO::LoadNative(ImageLoadDesc* load_desc)
    {
        ZoneScopedN("ImageTextureIO::LoadFromFileNative")

        ImageLoadDesc image_load_desc = static_cast<ImageLoadDesc>(*load_desc);

        std::unique_ptr<ImageTexture> image_texture = std::make_unique<ImageTexture>();

        std::ifstream input(image_load_desc.file_path, std::ios::binary);
        if (!input.is_open()) {
            LOG_ERROR("Failed to open cached mesh file.");
            return nullptr;
        }

        ImageAssetHeader header{};

        constexpr std::size_t base_size  = CESAR_SIZEOF(CesarAssetHeader);
        constexpr std::size_t total_size = CESAR_SIZEOF(ImageAssetHeader);

        char* header_bytes = reinterpret_cast<char*>(&header);

        input.read(header_bytes, static_cast<std::streamsize>(base_size));
        input.read(header_bytes + base_size, static_cast<std::streamsize>(total_size - base_size));
        
        std::vector<Subresource> asset_subresources(header.mip_count * header.array_size);
        input.read(reinterpret_cast<char*>(asset_subresources.data()), static_cast<std::streamsize>(CESAR_SIZEOF_BUFFER(Subresource, asset_subresources.size())));

        OfflineContext* context = resource_cache->offline_context;

        image_texture->width = header.width;
        image_texture->height = header.height;
        image_texture->srv_index = 0;

        TextureDesc texture_desc{};
        texture_desc.type  = header.tex_type;
        texture_desc.usage = ResourceUsage::Default;
        texture_desc.format = header.format;

        const Uint32 max_mip_count = header.mip_count;

        texture_desc.height       = header.height;
        texture_desc.width        = header.width;
        texture_desc.array_size   = header.array_size;
        texture_desc.mips         = header.mip_count;
        texture_desc.bind_flag    = ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess;
        texture_desc.misc_flag    = header.misc_flag;
        texture_desc.intial_state = ResourceState::CopyDst;

        Char* data = (Char*)malloc(header.copyable_size);
        input.read(data, header.copyable_size);

        const auto name = load_desc->file_path.stem().string();
        image_texture->gpu_texture = context->CreatePersistentTexture(texture_desc, name.c_str());
        image_texture->srv_index   = context->AllocateBindlessTextureSRV(image_texture->gpu_texture.get());

        Buffer* upload_buffer = context->CreateUploadbuffer(header.copyable_size, name.c_str());
        upload_buffer->Upload(std::span<Uint8>((Uint8*)data, header.copyable_size));

        auto im_tex = image_texture.get();
        im_tex->data = upload_buffer;

        auto loader = resource_cache->texture_loader.get();
        UploadTextureData(context, loader, load_desc, upload_buffer, image_texture.get(), false);

        //loader->Submit([context, upload_buffer, im_tex](){
        //    context->UploadTextureData(upload_buffer, im_tex->gpu_texture.get());
        //});
        //loader->AddPostJob([upload_buffer]() {
        //    delete upload_buffer;
        //});

        free(data);

        return image_texture;
    }

    std::unique_ptr<Resource> ImageTextureIO::LoadFromFileNonNative(ImageLoadDesc* load_desc)
    {
        ZoneScopedN("ImageTextureIO::LoadFromFileNonNative")

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
        image_texture->srv_index = 0;
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

        const auto name = load_desc->file_path.stem().string();
        image_texture->gpu_texture = context->CreatePersistentTexture(texture_desc, name.c_str());
        image_texture->srv_index   = context->AllocateBindlessTextureSRV(image_texture->gpu_texture.get());

        const Uint64 byte_size = width * height * GetFormatStride(texture_desc.format);
        Buffer* upload_buffer = context->CreateUploadbuffer(byte_size, name.c_str());
        upload_buffer->Upload(std::span<Uint8>((Uint8*)data, byte_size));

        auto im_tex = image_texture.get();
        im_tex->data = upload_buffer;

        auto loader = resource_cache->texture_loader.get();

        UploadTextureData(context, loader, load_desc, upload_buffer, image_texture.get(), true);

        //loader->Submit([context, upload_buffer, im_tex]() {
        //    context->UploadTextureData(upload_buffer, im_tex->gpu_texture.get());
        //    context->GenerateMips(im_tex->gpu_texture.get(), im_tex->srv_index);
        //});

        //loader->AddPostJob([upload_buffer]() {
        //    delete upload_buffer;
        //});

        free(data);

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
        void* pixel_data = nullptr;

        Int32 width, height, nChannel, bpc;
        stbi_set_flip_vertically_on_load(HasFlag(load_desc->flags, ImageLoadFlags::FlipUV));

		Bool is16Bit = false;
        if (stbi_is_16_bit_from_memory(static_cast<const stbi_uc*>(data->data), data->size))
        {
            pixel_data = stbi_load_16_from_memory(static_cast<const stbi_uc*>(data->data), data->size, &width, &height, &nChannel, 4);
			bpc = 16;
            is16Bit = true;
        }
        else
        {
            pixel_data = stbi_load_from_memory(static_cast<const stbi_uc*>(data->data), data->size, &width, &height, &nChannel, 4);
			bpc = 8;
			is16Bit = false;
        }

        if (pixel_data == nullptr) {
            return nullptr;
        }

        nChannel = 4;
        image_texture->width  = width;
        image_texture->height = height;
        image_texture->srv_index = 0;
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

        const auto name = load_desc->file_path.stem().string();
        image_texture->gpu_texture = context->CreatePersistentTexture(texture_desc, name.c_str());
        image_texture->srv_index = context->AllocateBindlessTextureSRV(image_texture->gpu_texture.get());

        const Uint64 byte_size = width * height * GetFormatStride(texture_desc.format);
        Buffer* upload_buffer = context->CreateUploadbuffer(byte_size, name.c_str());
        upload_buffer->Upload(std::span<Uint8>((Uint8*)pixel_data, byte_size));

        auto im_tex = image_texture.get();
        im_tex->data = upload_buffer;

        auto loader = resource_cache->texture_loader.get();
        UploadTextureData(context, loader, load_desc, upload_buffer, image_texture.get(), true);

        //loader->Submit([context, upload_buffer, im_tex]() {
        //    context->UploadTextureData(upload_buffer, im_tex->gpu_texture.get());
        //    context->GenerateMips(im_tex->gpu_texture.get(), im_tex->srv_index);
        //});
        //loader->AddPostJob([upload_buffer]() {
        //    delete upload_buffer;
        //});

        free(pixel_data);
        return image_texture;
    }

}
