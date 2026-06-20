#include "MaterialIO.h"
#include "../ResourceCache.h"
#include "../../cesar_core/Core/ThreadPool.h"

#include <tinyobj/tiny_obj_loader.h>
#include <fastgltf/types.hpp>
#include <fastgltf/core.hpp>

#include <fstream>
#include <sstream>

namespace cesar
{
    static void LoadImageTexturesAsync(TextureJobManager* loader, Material* material, MaterialLoadDesc* load_desc)
    {
        gThreadPool.SubmitJob([loader, material]() {
            ZoneScopedN("LoadImageTexturesAsync")

            loader->ExecuteAll();
            loader->Wait();

            delete material->albedo_map->data;
            material->albedo_map->data = nullptr;
            delete material->normal_map->data;
            material->normal_map->data = nullptr;
            delete material->ao_map->data;
            material->ao_map->data        = nullptr;
            delete material->roughness_map->data;
            material->roughness_map->data = nullptr;
            delete material->metallic_map->data;
            material->metallic_map->data  = nullptr;
        });
    }

	std::unique_ptr<Resource> MaterialIO::LoadFromFile(ResourceLoadDesc& load_desc)
	{
        MaterialLoadDesc* mtl_load_desc = (MaterialLoadDesc*)(&load_desc);
        if (load_desc.is_cooked){
            return LoadNative(mtl_load_desc);
        }else if (HasFlag(mtl_load_desc->flags, MaterialLoadFlags::LoadFromGlb_Gltf)) {
            return LoadGlbGltfMaterial(mtl_load_desc);
        }
	}

    void MaterialIO::SaveToDisk(const ResourceLoadDesc& load_desc, void* resource)
    {
        ZoneScopedN("MaterialIO::SaveToDisk")

        Material* material_resource = static_cast<Material*>(resource);
        MaterialLoadDesc* mtl_load_desc = (MaterialLoadDesc*)(&load_desc);

        MaterialAssetHeader header{};
        header.uuid = material_resource->GetUUID();
        header.version = 1;
        header.ref_count = material_resource->GetRefCount();
        header.type = ResourceType::Material;

        header.material_data = *material_resource->material_data;

        const auto cooked_path = resource_cache->GetCookedAssetPath(load_desc.file_path);
        std::ofstream output(cooked_path, std::ios::binary | std::ios::out);

        if (!output) {
            LOG_ERROR("Failed to save asset to disk.");
            return;
        }

        std::vector<Uint32> map_map;
        std::vector<Uint32> unqiue_map;

        std::vector<filespace::filepath> cooked_image_paths;
        cooked_image_paths.push_back(material_resource->albedo_map->GetCookedPath());
        cooked_image_paths.push_back(material_resource->normal_map->GetCookedPath());
        cooked_image_paths.push_back(material_resource->ao_map->GetCookedPath());
        cooked_image_paths.push_back(material_resource->roughness_map->GetCookedPath());
        cooked_image_paths.push_back(material_resource->metallic_map->GetCookedPath());


        std::unordered_map<std::filesystem::path, uint32_t> path_to_unique;

        map_map.reserve(cooked_image_paths.size());

        for (uint32_t i = 0; i < cooked_image_paths.size(); ++i)
        {
            const auto& path = cooked_image_paths[i];

            auto it = path_to_unique.find(path);

            if (it == path_to_unique.end())
            {
                uint32_t unique_index =
                    static_cast<uint32_t>(unqiue_map.size());

                path_to_unique.emplace(path, unique_index);

                unqiue_map.push_back(i);
                map_map.push_back(unique_index);
            }
            else
            {
                map_map.push_back(it->second);
            }
        }

        header.material_data.albedo.map_index = map_map[0];
        header.material_data.normal.map_index = map_map[1];
        header.material_data.ao.map_index = map_map[2];
        header.material_data.roughness.map_index = map_map[3];
        header.material_data.metallic.map_index = map_map[4];

        header.unique_map_count = static_cast<Uint32>(unqiue_map.size());

        constexpr std::size_t base_size  = CESAR_SIZEOF(CesarAssetHeader);
        constexpr std::size_t total_size = CESAR_SIZEOF(MaterialAssetHeader);

        const char* header_bytes = reinterpret_cast<const char*>(&header);
        output.write(header_bytes, static_cast<std::streamsize>(base_size));
        output.write(header_bytes + base_size, static_cast<std::streamsize>(total_size - base_size));

        for (Uint32 i = 0; i < unqiue_map.size(); i++)
        {
            auto length = cooked_image_paths[unqiue_map[i]].string().size();
            output.write(reinterpret_cast<const char*>(&length), CESAR_SIZEOF(Uint32));
        }

        for (Uint32 i = 0; i < unqiue_map.size(); i++)
        {
            const auto& path = cooked_image_paths[unqiue_map[i]].string();
            output.write(reinterpret_cast<const char*>(path.c_str()), path.size());
        }

        output.close();
    }

    std::unique_ptr<Material> MaterialIO::LoadNative(MaterialLoadDesc* load_desc)
    {
        ZoneScopedN("MaterialIO::LoadNative")

        LOG_DEBUG("LOADING CACHED MATERIAL_RESOURCE");

        std::unique_ptr<Material> material_resource = std::make_unique<Material>();

        MaterialAssetHeader header;

        std::ifstream input(load_desc->file_path, std::ios::binary);
        if (!input.is_open()) {
            LOG_ERROR("Failed to open cached mesh file.");
            return nullptr;
        }

        constexpr std::size_t base_size = CESAR_SIZEOF(CesarAssetHeader);
        constexpr std::size_t total_size = CESAR_SIZEOF(MaterialAssetHeader);

        char* header_bytes = reinterpret_cast<char*>(&header);

        input.read(header_bytes, static_cast<std::streamsize>(base_size));
        input.read(header_bytes + base_size, static_cast<std::streamsize>(total_size - base_size));
        
        MaterialData& material_data = header.material_data;

        LinearAllocator<MaterialData>* material_data_allocator = resource_cache->GetMaterialAllocator();
        MemoryBlock<MaterialData> material_data_block = material_data_allocator->Allocate(1);
        CopyToMemoryBlock(material_data_block, &header.material_data);

        material_resource->material_data = material_data_block.data();

        //Load Material Image Data
        {
            std::vector<Uint32> image_path_lengths(header.unique_map_count);
            input.read(reinterpret_cast<char*>(image_path_lengths.data()), CESAR_SIZEOF_BUFFER(Uint32, header.unique_map_count));

            Uint32 character_length = 0;
            for (const auto& i : image_path_lengths)
                character_length += i;

            std::vector<Char> image_path_characters(character_length);
            input.read(reinterpret_cast<char*>(image_path_characters.data()), character_length);

            auto GetMapPath = [&](Uint32 map_index)->MemoryBlock<Char>
                {
                    const Uint32 map_path_length = image_path_lengths[map_index];
                    Uint32 map_offset = 0;
                    for (Uint32 i = 0; i < map_index; i++)
                        map_offset += image_path_lengths[i];

                    return MemoryBlock<Char>(image_path_characters.data() + map_offset, map_path_length);
                };

            const auto albedo_path = GetMapPath(material_data.albedo.map_index);
            const auto normal_path = GetMapPath(material_data.normal.map_index);
            const auto ao_path = GetMapPath(material_data.ao.map_index);
            const auto roughness_path = GetMapPath(material_data.roughness.map_index);
            const auto metallic_path = GetMapPath(material_data.metallic.map_index);

            //Load And Set Material Images

            ImageLoadFlags flags = ImageLoadFlags::LoadFromMemory | ImageLoadFlags::FlipUV | ImageLoadFlags::GenerateMips | ImageLoadFlags::LoadFromMaterial;
            {
                {
                    ImageLoadDesc image_load_desc{};
                    image_load_desc.flags = flags;
                    image_load_desc.file_path = filespace::filepath(albedo_path.begin(), albedo_path.end());
                    ImageTexture* _texture = resource_cache->LoadResource<ImageTexture>(image_load_desc);

                    if (_texture == nullptr)
                    {
                        _texture = resource_cache->GetDefaultInvalidTexture();
                    }
                    material_data.albedo.map_index = _texture->srv_index;
                    material_resource->albedo_map = _texture;
                }

                {
                    ImageLoadDesc image_load_desc{};
                    image_load_desc.flags = flags;
                    image_load_desc.file_path = filespace::filepath(normal_path.begin(), normal_path.end());
                    ImageTexture* _texture = resource_cache->LoadResource<ImageTexture>(image_load_desc);
                    if (_texture == nullptr)
                    {
                        _texture = resource_cache->GetDefaultNormalTexture();
                    }
                    material_resource->normal_map = _texture;
                    material_data.normal.map_index = _texture->srv_index;
                }

                {
                    ImageLoadDesc image_load_desc{};
                    image_load_desc.flags = flags;
                    image_load_desc.file_path = filespace::filepath(ao_path.begin(), ao_path.end());
                    ImageTexture* _texture = resource_cache->LoadResource<ImageTexture>(image_load_desc);
                    if (_texture == nullptr)
                    {
                        _texture = resource_cache->GetDefaultWhiteTexture();
                    }
                    material_resource->ao_map = _texture;
                    material_data.ao.map_index = _texture->srv_index;
                }

                {
                    ImageLoadDesc image_load_desc{};
                    image_load_desc.flags = flags;
                    image_load_desc.file_path = filespace::filepath(roughness_path.begin(), roughness_path.end());
                    ImageTexture* _texture = resource_cache->LoadResource<ImageTexture>(image_load_desc);
                    if (_texture == nullptr)
                    {
                        _texture = resource_cache->GetDefaultWhiteTexture();
                    }
                    material_resource->roughness_map = _texture;
                    material_data.roughness.map_index = _texture->srv_index;
                }

                {
                    ImageLoadDesc image_load_desc{};
                    image_load_desc.flags = flags;
                    image_load_desc.file_path = filespace::filepath(metallic_path.begin(), metallic_path.end());
                    ImageTexture* _texture = resource_cache->LoadResource<ImageTexture>(image_load_desc);
                    if (_texture == nullptr)
                    {
                        _texture = resource_cache->GetDefaultBlackTexture();
                    }
                    material_resource->metallic_map = _texture;
                    material_data.metallic.map_index = _texture->srv_index;
                }
            }
        }

        LoadImageTexturesAsync(resource_cache->texture_loader.get(), material_resource.get(), load_desc);

        return material_resource;
    }

    std::unique_ptr<Material> MaterialIO::LoadGlbGltfMaterial(MaterialLoadDesc* material_load_desc)
    {
        ZoneScopedN("MaterialIO::LoadGlbGltfMaterial")
        LOG_DEBUG("LOADING NON-NATIVE MATERIAL RESOURCE");

        std::unique_ptr<Material> material_resource = std::make_unique<Material>();

        MaterialData material_data{};

        struct _data_
        {
            const void* data;
            Uint32 index;
        };

        _data_* data = (_data_*)material_load_desc->payload;

        fastgltf::Expected<fastgltf::Asset>* _asset = (fastgltf::Expected<fastgltf::Asset>*)data->data;
        fastgltf::Asset* asset = &_asset->get();
        fastgltf::Material* material = &asset->materials[data->index];

        auto load_texture = [&](size_t tex_index, ImageLoadFlags flags) -> ImageTexture*
            {
                auto& texture = asset->textures[tex_index];
                if (!texture.imageIndex.has_value()) return nullptr;

                auto& image = asset->images[texture.imageIndex.value()];
                if (auto* bv = std::get_if<fastgltf::sources::BufferView>(&image.data))
                {
                    auto& buffer_view = asset->bufferViews[bv->bufferViewIndex];
                    auto& buffer = asset->buffers[buffer_view.bufferIndex];

                    if (auto* arr = std::get_if<fastgltf::sources::Array>(&buffer.data))
                    {
                        struct _data_ {
                            void* data;
                            Uint64 size;
                        } img_data = {
                            .data = arr->bytes.data() + buffer_view.byteOffset,
                            .size = buffer_view.byteLength
                        };

                        ImageLoadDesc desc{};
                        desc.payload = &img_data;
                        desc.flags = flags;
                        desc.no_path = true;
                        desc.file_path = std::format("{}_image_{}", material_load_desc->file_path.stem().string(), texture.imageIndex.value());
                        return resource_cache->LoadResource<ImageTexture>(desc);
                    }
                }
            };

        auto& pbr_info = material->pbrData;
        material_data.albedo.color = {
                material->pbrData.baseColorFactor[0],
                material->pbrData.baseColorFactor[1],
                material->pbrData.baseColorFactor[2],
                material->pbrData.baseColorFactor[3]
        };

        auto image_load_flags = ImageLoadFlags::FlipUV | ImageLoadFlags::GenerateMips |
                                ImageLoadFlags::LoadFromMemory | ImageLoadFlags::LoadFromMaterial;
        // Base color
        if (material->pbrData.baseColorTexture.has_value())
        {
            auto& info = pbr_info.baseColorTexture.value();
            if (auto* tex = load_texture(info.textureIndex, image_load_flags))
            {
                material_data.albedo.map_index = tex->srv_index;
                material_resource->albedo_map = tex;
            }
            else {
                auto invalid_texture_d = resource_cache->GetDefaultInvalidTexture();
                material_data.albedo.map_index = invalid_texture_d->srv_index;
                material_resource->albedo_map = invalid_texture_d;
            }
        }
        else {
            auto invalid_texture_d = resource_cache->GetDefaultInvalidTexture();
            material_data.albedo.map_index = invalid_texture_d->srv_index;
            material_resource->albedo_map = invalid_texture_d;
        }

        if (material->normalTexture.has_value())
        {
            auto& info = material->normalTexture.value();
            material_data.normal.normal_strength = info.scale;
            if (auto* tex = load_texture(info.textureIndex, image_load_flags))
            {
                material_data.normal.map_index = tex->srv_index;
                material_resource->normal_map = tex;
            }
            else {
                auto normal_mapD = resource_cache->GetDefaultNormalTexture();
                material_data.normal.map_index = normal_mapD->srv_index;
                material_resource->normal_map = normal_mapD;
            }
        }
        else {
            auto normal_mapD = resource_cache->GetDefaultNormalTexture();
            material_data.normal.map_index = normal_mapD->srv_index;
            material_resource->normal_map = normal_mapD;
        }

        auto* white_texture = resource_cache->GetDefaultWhiteTexture();
        auto* black_texture = resource_cache->GetDefaultBlackTexture();
        if (material->occlusionTexture.has_value())
        {
            auto& info = material->occlusionTexture.value();
            material_data.ao.ao_strength = info.strength;
            material_data.ao.channel = ImageTextureChannel::R;

            if (auto* tex = load_texture(info.textureIndex, image_load_flags)) {
                material_data.ao.map_index = tex->srv_index;
                material_resource->ao_map = tex;
            }
            else {
                material_data.ao.map_index = white_texture->srv_index;
                material_resource->ao_map = white_texture;
            }
        }
        else {
            material_data.ao.map_index = white_texture->srv_index;
            material_resource->ao_map = white_texture;
        }

        if (material->pbrData.metallicRoughnessTexture.has_value())
        {
            auto& info = material->pbrData.metallicRoughnessTexture.value();
            material_data.metallic.metallic_strength   = material->pbrData.metallicFactor;
            material_data.metallic.channel = ImageTextureChannel::B;

            material_data.roughness.roughness_strength = material->pbrData.roughnessFactor;
            material_data.roughness.channel = ImageTextureChannel::G;
            if (auto* tex = load_texture(info.textureIndex, image_load_flags))
            {
                material_data.metallic.map_index  = tex->srv_index;
                material_data.roughness.map_index = tex->srv_index;
                material_resource->roughness_map = tex;
                material_resource->metallic_map = tex;
            }
            else {
                material_data.metallic.map_index  = black_texture->srv_index;
                material_data.roughness.map_index = white_texture->srv_index;
                material_resource->metallic_map = black_texture;
                material_resource->roughness_map = white_texture;
            }
        }
        else {
            material_data.metallic.map_index = black_texture->srv_index;
            material_data.roughness.map_index = white_texture->srv_index;
            material_resource->metallic_map = black_texture;
            material_resource->roughness_map = white_texture;
        }

        auto material_allocator = resource_cache->GetMaterialAllocator();
        MemoryBlock<MaterialData> material_block = material_allocator->Allocate(1);
        CopyToMemoryBlock(material_block, &material_data);

        material_resource->material_data = material_block.data();

        LoadImageTexturesAsync(resource_cache->texture_loader.get(), material_resource.get(), material_load_desc);

        return material_resource;
    }

    std::unique_ptr<Material> MaterialIO::LoadAssimpMaterial(MaterialLoadDesc* load_desc)
    {
        CESAR_FEATURE_NO_IMPL("Material Load for Assimp has not been implemented yet.");
        return nullptr;
    }

}