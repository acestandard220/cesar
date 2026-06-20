#include "ResourceCache.h"

using namespace cesar;

namespace cesar
{
	ImageTexture* default_invalid_texture = nullptr;
	ImageTexture* default_white_texture   = nullptr;
	ImageTexture* default_black_texture   = nullptr;
	ImageTexture* default_normal_texture  = nullptr;
	
	Material* default_material = nullptr;

	ResourceCache::ResourceCache(OfflineContext* offline_context, const filespace::filepath& asset_path)
		:offline_context(offline_context), assets_path(asset_path)
	{		
		if (!InitializeAllocators())
			return;

		texture_loader = std::make_unique<TextureJobManager>(offline_context);

		resource_io[static_cast<Uint32>(ResourceType::ImageTexture)] = std::make_unique<ImageTextureIO>(this);	
		resource_io[static_cast<Uint32>(ResourceType::Mesh)] = std::make_unique<MeshIO>(this);
		resource_io[static_cast<Uint32>(ResourceType::Material)] = std::make_unique<MaterialIO>(this);

		LoadDefaultResources();
	}
	
	IResourceIO* ResourceCache::GetResourceIO(ResourceType type) {
		auto idx = static_cast<cesar::Uint32>(type);
		if (idx >= resource_io.size()) {
			LOG_ERROR("Invalid resource type index.");
			return nullptr;
		}

		if (!resource_io[idx]) {
			switch (type) {
			case ResourceType::ImageTexture:
				resource_io[idx] = std::make_unique<ImageTextureIO>();
				break;
				 case ResourceType::Mesh:
				     resource_io[idx] = std::make_unique<MeshIO>();
				     break;
				// Add cases here for other ResourceType values when corresponding IO classes are available:
				// case ResourceType::Material:
				//     resource_io[idx] = std::make_unique<MaterialIO>();
				//     break;
			default:
				LOG_ERROR("No IO implementation for requested resource type.");
				return nullptr;
			}
		}

		return resource_io[idx].get();
	}
	
	const filespace::filepath& ResourceCache::GetAssetsPath() const
	{
		return assets_path;
	}

	ImageTexture* ResourceCache::GetDefaultTexture(MapType type)
	{
		switch (type)
		{
		    case cesar::MapType::Albedo:
		    	return default_invalid_texture;
		    case cesar::MapType::Normal:
		    	return default_normal_texture;
		    case cesar::MapType::AO:
		    	return default_white_texture;
		    case cesar::MapType::Roughness:
		    	return default_white_texture;
		    case cesar::MapType::Metallic:
		    	return default_black_texture;
			default: return default_invalid_texture;
		}
	}

	ImageTexture* ResourceCache::GetDefaultInvalidTexture() const { return default_invalid_texture; }
	ImageTexture* ResourceCache::GetDefaultWhiteTexture()   const { return default_white_texture; }
	ImageTexture* ResourceCache::GetDefaultBlackTexture()   const { return default_black_texture; }
	ImageTexture* ResourceCache::GetDefaultNormalTexture()  const { return default_normal_texture; }

	Bool ResourceCache::InitializeAllocators()
	{
		vertices = std::make_unique<LinearAllocator<Vertex>>();
		if (!vertices->Reserve(START_VERTEX_COUNT)) {
			LOG_FATAL("Failed to initial vertex pool.");
			return false;
		}

		indices = std::make_unique<LinearAllocator<Uint32>>();
		if (!indices->Reserve(START_INDEX_COUNT)) {
			LOG_FATAL("Failed to initialize index pool.");
			return false;
		}

		submesh_data = std::make_unique<LinearAllocator<SubMeshData>>();
		if (!submesh_data->Reserve(START_SUBMESH_COUNT)) {
			LOG_FATAL("Failed to initialize submesh data pool.");
			return false;
		}

		material_data = std::make_unique<LinearAllocator<MaterialData>>();
		if (!material_data->Reserve(START_MATERIAL_COUNT)) {
			LOG_FATAL("Failed to initialize material data pool.");
			return false;
		}

		meshlet_data = std::make_unique<LinearAllocator<Meshlet>>();
		if (!meshlet_data->Reserve(START_MESHLETS_COUNT)) {
			LOG_FATAL("Failed to initialize meshlet data pool.");
			return false;
		}

		meshlet_vertices = std::make_unique<LinearAllocator<Uint32>>();
		if (!meshlet_vertices->Reserve(START_MESHLETS_COUNT * MAX_MESHLET_VERTICE)) {
			LOG_FATAL("Failed to initialize meshlet vertex pool.");
			return false;
		}

		meshlet_triangles = std::make_unique<LinearAllocator<Uint32>>();
		if (!meshlet_triangles->Reserve(START_MESHLETS_COUNT * MAX_MESHLET_TRIANGLE)) {
			LOG_FATAL("Failed to initialize meshlet triangle pool");
			return false;
		}
	}

	void ResourceCache::LoadDefaultResources()
	{
		{
			ImageLoadDesc load_desc{};
			load_desc.file_path = "D:\\Project\\cesar\\cesar\\resources\\Default Textures\\invalid.png";
			load_desc.type = ResourceType::ImageTexture;
			load_desc.flags |= ImageLoadFlags::FlipUV;
			load_desc.flags |= ImageLoadFlags::GenerateMips;
			load_desc.payload = nullptr;
			load_desc.is_cooked = false;
			load_desc.no_path = false;
			default_invalid_texture = LoadResource<ImageTexture>(load_desc);
		}
		
		{
			ImageLoadDesc load_desc{};
			load_desc.type = ResourceType::ImageTexture;
			load_desc.flags |= ImageLoadFlags::FlipUV;
			load_desc.flags |= ImageLoadFlags::GenerateMips;
			load_desc.file_path = "D:\\Project\\cesar\\cesar\\resources\\Default Textures\\white.png";
			load_desc.payload = nullptr;
			load_desc.is_cooked = false;
			load_desc.no_path = false;
			default_white_texture = LoadResource<ImageTexture>(load_desc);
		}
		
		{
			ImageLoadDesc load_desc{};
			load_desc.type = ResourceType::ImageTexture;
			load_desc.flags |= ImageLoadFlags::FlipUV;
			load_desc.flags |= ImageLoadFlags::GenerateMips;
			load_desc.file_path = "D:\\Project\\cesar\\cesar\\resources\\Default Textures\\black.png";
			load_desc.payload = nullptr;
			load_desc.is_cooked = false;
			load_desc.no_path = false;
			default_black_texture = LoadResource<ImageTexture>(load_desc);
		}

		{
			ImageLoadDesc load_desc{};
			load_desc.type = ResourceType::ImageTexture;
			load_desc.flags |= ImageLoadFlags::FlipUV;
			load_desc.flags |= ImageLoadFlags::GenerateMips;
			load_desc.file_path = "D:\\Project\\cesar\\cesar\\resources\\Default Textures\\normal.png";
			load_desc.payload = nullptr;
			load_desc.is_cooked = false;
			load_desc.no_path = false;
			default_normal_texture = LoadResource<ImageTexture>(load_desc);
		}

		texture_loader->ExecuteAll();
	}
}
