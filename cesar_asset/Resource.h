#pragma once
#include "../cesar_core/cesar_core.h"
#include "../cesar_core/Core/UUID.h"
#include "../cesar_core/Core/Filespace.h"
#include <concepts>

namespace cesar {

	enum class ResourceType
	{
		Mesh,
		Material,
		ImageTexture,

		Unknown
	};

	struct ResourceDesc
	{
		ResourceType type;
	};

	class ResourceCache;
	class _declspec(dllexport) Resource
	{
	    public:
	    	Resource() = default;
			virtual ~Resource() = default;

			const UUID& GetUUID()const { return uuid; }
	    
	    	void SetResourceName(const std::string& name);
	    
	    	const ResourceType& GetResourceType()const { return resource_type; }
	    	const std::string& GetResourceName()const { return name; }
			const filespace::filepath& GetCookedPath()const { return cooked_path; }

			void Use() { ref_count++; }
			void Unuse() {
				CESAR_ASSERT((ref_count > 0) && "Cannot unrefence resource which is not in use.");
				ref_count--; 
			}

			Uint32 GetRefCount()const { return ref_count; }
	    private:
			friend class ResourceCache;
	    	std::string name;
			filespace::filepath cooked_path;

	    	ResourceType resource_type;
			Uint32 ref_count = 0;
			UUID uuid;

	};


	template<typename T>
	concept IsResourceType = std::derived_from<T, Resource>;

	template<typename T>
	struct ResourceTypeTrait;
}