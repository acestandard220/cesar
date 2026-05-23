#pragma once
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"

namespace cesar
{
	struct _declspec(dllexport) ResourceLoadDesc
	{
		UUID uuid = CESAR_INVALID_UUID;
		std::filesystem::path file_path;
		ResourceType type;
		Bool no_path;
		void* payload = nullptr;
	};

	template<typename T>
	struct LoadDesc : public ResourceLoadDesc
	{
		ResourceType GetType() {
			return ResourceTypeTrait<T>::type;
		}
	};
}