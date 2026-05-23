#include "MaterialIO.h"
#include "../ResourceCache.h"

#include <tinyobj/tiny_obj_loader.h>
#include <fastgltf/types.hpp>
#include <fastgltf/core.hpp>

#include <fstream>
#include <sstream>

namespace cesar
{
	std::unique_ptr<Resource> MaterialIO::LoadFromFile(ResourceLoadDesc& load_desc)
	{
        std::unique_ptr<Material> material_resource = std::make_unique<Material>();

        MaterialLoadDesc* mtl_load_desc = (MaterialLoadDesc*)(&load_desc);
        if (HasFlag(mtl_load_desc->flags, MaterialLoadFlags::LoadFromMtl)) {
			material_resource->material_data = LoadMaterialFromMtl(load_desc.file_path.string(), "");
        }
        else if (HasFlag(mtl_load_desc->flags, MaterialLoadFlags::LoadFromGlb_Gltf)) {
            material_resource->material_data = LoadMaterialFromGlbGltf(mtl_load_desc);
        }
        
        return material_resource;
	}


    MaterialData* MaterialIO::LoadMaterialFromMtl(const std::string& mtlPath, const std::string& materialName)
    {
        std::vector<tinyobj::material_t> materials;
        std::map<std::string, int> matMap;
        std::ifstream mtlStream(mtlPath);

        std::string warn, err;
        tinyobj::LoadMtl(&matMap, &materials, &mtlStream, &warn, &err);

        const tinyobj::material_t& m = materials[0];
        MaterialData data{};
       
		
        ImageLoadDesc image_load_desc{};
		image_load_desc.file_path = m.diffuse_texname;
		image_load_desc.flags = ImageLoadFlags::FlipUV | ImageLoadFlags::GenerateMips;
		ImageTexture* diffuse_texture = resource_cache->LoadResource<ImageTexture>(image_load_desc); 
        if (diffuse_texture == nullptr) {
			LOG_ERROR("Failed to load diffuse texture for material - {}", m.diffuse_texname.c_str());
			LOG_ERROR("Using default invalid texture.");
			diffuse_texture = resource_cache->GetDefaultInvalidTexture();
        }
        data.albedo.map_index = diffuse_texture->srv_index; 
        data.albedo.color = { m.diffuse[0], m.diffuse[1], m.diffuse[2], m.dissolve };

		image_load_desc.file_path = m.bump_texname;
		ImageTexture* normal_texture = resource_cache->LoadResource<ImageTexture>(image_load_desc);
        if (normal_texture == nullptr) {
			LOG_ERROR("Failed to load default normal texture.");
			LOG_ERROR("Using default Normal texture.");
            normal_texture = resource_cache->GetDefaultNormalTexture();
        }
		data.normal.map_index = normal_texture->srv_index; 
        data.normal.normal_strength = 1.0f; 

		image_load_desc.file_path = m.ambient_texname;
		ImageTexture* ao_texture = resource_cache->LoadResource<ImageTexture>(image_load_desc);
        if (ao_texture == nullptr) {
			LOG_ERROR("Failed to load default ambient occlusion texture.");
			LOG_ERROR("Using default Ambient Occlusion texture.");
			ao_texture = resource_cache->GetDefaultWhiteTexture();
        }
		data.ao.map_index = ao_texture->srv_index;
        data.ao.ao_strength = 1.0f;

		image_load_desc.file_path = m.metallic_texname; 
		ImageTexture* metallic_texture = resource_cache->LoadResource<ImageTexture>(image_load_desc);
        if (metallic_texture == nullptr) {
			LOG_ERROR("Failed to load metallic texture for material - {}", m.metallic_texname.c_str());
			LOG_ERROR("Using default metallic texture.");
			metallic_texture = resource_cache->GetDefaultBlackTexture();
        }
        data.metallic.map_index = metallic_texture->srv_index;
        data.metallic.metallic_strength = m.metallic;

        image_load_desc.file_path = m.roughness_texname;
		ImageTexture* roughness_texture = resource_cache->LoadResource<ImageTexture>(image_load_desc);
        if (roughness_texture == nullptr) {
			LOG_ERROR("Failed to load roughness texture for material - {}", m.roughness_texname.c_str());
			LOG_ERROR("Using Roughness texture.");
			roughness_texture = resource_cache->GetDefaultWhiteTexture();
        }
        data.roughness.roughness_strength = m.roughness;
		data.roughness.map_index = roughness_texture->srv_index;

        data.map_scale = { 1.0f, 1.0f };

        LinearAllocator<MaterialData>* material_data_allocator = resource_cache->GetMaterialAllocator();
        MemoryBlock<MaterialData> material_data_block = material_data_allocator->Allocate(1);
        CopyToMemoryBlock(material_data_block, &data);

        return material_data_block.data();
    }

    MaterialData* MaterialIO::LoadMaterialFromGlbGltf(MaterialLoadDesc* material_load_desc)
    {
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
                if (auto* arr = std::get_if<fastgltf::sources::Array>(&image.data))
                {
                    struct _data_ {
                        void* data;
                        Uint64 size;
                    }data = {
                        .data = arr->bytes.data(),
                        .size = arr->bytes.size()
                    };

                    ImageLoadDesc desc{};
                    desc.payload = &data;
                    desc.flags = flags;
                    desc.no_path = true;
                    desc.file_path = std::format("{}_image_{}", material_load_desc->file_path.stem().string(), texture.imageIndex.value());
                    return resource_cache->LoadResource<ImageTexture>(desc);
                }
                return nullptr;
            };

        auto& pbr_info = material->pbrData;
        material_data.albedo.color = {
                material->pbrData.baseColorFactor[0],
                material->pbrData.baseColorFactor[1],
                material->pbrData.baseColorFactor[2],
                material->pbrData.baseColorFactor[3]
        };

        // Base color
        if (material->pbrData.baseColorTexture.has_value())
        {
            auto& info = pbr_info.baseColorTexture.value();
            if (auto* tex = load_texture(info.textureIndex, ImageLoadFlags::FlipUV | ImageLoadFlags::GenerateMips | ImageLoadFlags::LoadFromMemory))
            {
                material_data.albedo.map_index = tex->srv_index;
            }
            else {
                material_data.albedo.map_index = resource_cache->GetDefaultInvalidTexture()->srv_index;
            }
        }

        if (material->normalTexture.has_value())
        {
            auto& info = material->normalTexture.value();
            material_data.normal.normal_strength = info.scale;
            if (auto* tex = load_texture(info.textureIndex, ImageLoadFlags::FlipUV | ImageLoadFlags::GenerateMips | ImageLoadFlags::LoadFromMemory))
            {
                material_data.normal.map_index = tex->srv_index;
            }
            else {
                material_data.normal.map_index = resource_cache->GetDefaultNormalTexture()->srv_index;
            }
        }

        auto* white_texture = resource_cache->GetDefaultWhiteTexture();
        auto* black_texture = resource_cache->GetDefaultBlackTexture();
        if (material->occlusionTexture.has_value())
        {
            auto& info = material->occlusionTexture.value();
            material_data.ao.ao_strength = info.strength;
            material_data.ao.channel = ImageTextureChannel::R;

            if (auto* tex = load_texture(info.textureIndex, ImageLoadFlags::FlipUV | ImageLoadFlags::GenerateMips | ImageLoadFlags::LoadFromMemory)) {
                material_data.ao.map_index = tex->srv_index;
            }
            else {
                material_data.ao.map_index = white_texture->srv_index;
            }
        }

        if (material->pbrData.metallicRoughnessTexture.has_value())
        {
            auto& info = material->pbrData.metallicRoughnessTexture.value();
            material_data.metallic.metallic_strength   = material->pbrData.metallicFactor;
            material_data.metallic.channel = ImageTextureChannel::B;

            material_data.roughness.roughness_strength = material->pbrData.roughnessFactor;
            material_data.roughness.channel = ImageTextureChannel::G;
            if (auto* tex = load_texture(info.textureIndex, ImageLoadFlags::FlipUV | ImageLoadFlags::GenerateMips | ImageLoadFlags::LoadFromMemory))
            {
                //material_data.ao.map_index        = tex->srv_index;
                material_data.metallic.map_index  = tex->srv_index;
                material_data.roughness.map_index = tex->srv_index;
            }
            else {
                material_data.metallic.map_index  = black_texture->srv_index;
                material_data.roughness.map_index = white_texture->srv_index;
            }
        }

        auto material_allocator = resource_cache->GetMaterialAllocator();
        MemoryBlock<MaterialData> material_block = material_allocator->Allocate(1);
        CopyToMemoryBlock(material_block, &material_data);
        return material_block.data();
    }

	void MaterialIO::SaveToDisk(const std::filesystem::path& file_path)
	{}
}