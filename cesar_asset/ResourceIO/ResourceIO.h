#pragma once
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"
#include "ResourceLoadDesc.h"

namespace cesar {
	class ResourceCache;
	class IResourceIO 
	{
	    public:
			IResourceIO() = default;
			IResourceIO(ResourceCache* resource_cache)
				:resource_cache(resource_cache)
			{};
	    	virtual ~IResourceIO() = default;
	    
	    	virtual std::unique_ptr<Resource> LoadFromFile(ResourceLoadDesc& load_desc) = 0;
	    	virtual void SaveToDisk(const std::filesystem::path& file_path) = 0;
	protected:
		ResourceCache* resource_cache = nullptr;
	};

}
