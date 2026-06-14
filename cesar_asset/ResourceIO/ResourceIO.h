#pragma once
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"
#include "ResourceLoadDesc.h"

#include <fstream>

namespace cesar {

	struct CesarAssetHeader
	{
		const char   magic[4] = { 'C','S','A','R' }; 
		Uint32       version;                             
		ResourceType type;                        
		Uint32       ref_count; //Add a flag for loading ref counts if you are loading an entire scene                             
		Uint64       last_write;                        
		UUID         uuid;                             

		void operator<<(std::ofstream& o)const {};
	};

	inline std::ofstream& operator<<(std::ofstream& stream, const CesarAssetHeader& header){
		stream << header.magic;
		stream << header.version;
		stream << static_cast<Uint32>(header.type);
		stream << CESAR_UUID_STRING(header.uuid);
		return stream;
	}

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
	    	virtual void SaveToDisk(const ResourceLoadDesc& load_desc, void* resource) = 0;
	protected:
		ResourceCache* resource_cache = nullptr;
	};

}
