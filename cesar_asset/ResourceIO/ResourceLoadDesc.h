#pragma once
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"

namespace cesar
{
	struct _declspec(dllexport) ResourceLoadDesc
	{
		UUID uuid = CESAR_INVALID_UUID;
		filespace::filepath file_path;
		ResourceType type;
		Bool no_path;
		Bool is_cooked = false;
		void* payload = nullptr;
	};

	struct __declspec(dllexport) ResourceSaveDesc
	{
		UUID uuid = CESAR_INVALID_UUID;
		filespace::filepath save_path;
		ResourceType type;
	};

	template<typename T>
	struct LoadDesc : public ResourceLoadDesc
	{
		ResourceType GetType() {
			return ResourceTypeTrait<T>::type;
		}
	};
}