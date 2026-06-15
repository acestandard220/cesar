#pragma once
#include "../cesar_core.h"

namespace cesar
{
	namespace extensions
	{
		constexpr const Char* cooked_asset   = ".csa";
		constexpr const Char* cooked_scene   = ".csc";
		constexpr const Char* cooked_project = ".csr";

		constexpr const Char* dds = ".dds";
		constexpr const Char* ktx2 = ".ktx2";
		constexpr const Char* cooked_image_raw = ".csa_image";

		constexpr const Char* glb  = ".glb";
		constexpr const Char* gltf = ".gltf";
		constexpr const Char* fbx  = ".fbx";
		constexpr const Char* abc  = ".abc";
	}

	namespace filespace
	{

		using filepath = std::filesystem::path;

		inline Bool Exists(filepath& path) {
			return std::filesystem::exists(path);
		}

		inline filepath GetCookedScenePath(const filepath& raw_path)
		{
			filepath new_path = raw_path;
			new_path.replace_extension(extensions::cooked_scene);
			return new_path;
		}

		inline filepath GetCookedProjectPath(const filepath& raw_path)
		{
			filepath new_path = raw_path;
			new_path.replace_extension(extensions::cooked_project);
			return new_path;
		}



	}
}