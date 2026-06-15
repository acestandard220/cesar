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

		inline filepath Relative(filepath& base_path, filepath& path)
		{
			return std::filesystem::relative(path, base_path);
		}

		//Move these to their right files later
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

		inline Bool IsChild(const filepath& parent_path, const filepath& child_path)
		{
			auto child_abs = std::filesystem::weakly_canonical(child_path);
		    auto parent_abs = std::filesystem::weakly_canonical(parent_path);

		    auto mismatch_pair = std::mismatch(parent_abs.begin(), parent_abs.end(),
			child_abs.begin(), child_abs.end());

     		return mismatch_pair.first == parent_abs.end();
		}


	}
}