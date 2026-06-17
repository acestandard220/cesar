#pragma once
#include "../cesar_core/cesar_core.h"
#include "GPUContext.h"

namespace cesar
{


	class _declspec(dllexport) OfflineContext
	{
	public:
		OfflineContext(GPUContext* gpu_context);
		~OfflineContext();

		void Begin();
		void End();

		std::unique_ptr<Texture> CreatePersistentTexture(const TextureDesc& desc, const Char* name);

		Buffer* CreateReadbackBuffer(Uint32 element_count, const Char* name);
		Buffer* CreateUploadbuffer(Uint32 element_count,   const Char* name);

		void UploadTextureData(Buffer* data, Texture* texture);
		void UploadTextureData(Buffer* data, Subresource* subresources, Uint32 subresource_count, Texture* texture);
		void GenerateMips(Texture* texture, Uint32 srv_index);

		Buffer* GetTextureSubRegionPixels(Texture* texture, const TextureViewDesc& view_desc);
		Buffer* GetTexturePixels(Texture* texture);

		Uint32 AllocateBindlessTextureSRV(Texture* texture);

	private:
		void CreatePipelineStates();
	private:
		GPUContext* gpu_context;

		std::unique_ptr<CommandList> gfx_cmd_list;
		std::unique_ptr<CommandList> compute_cmd_list;
		std::unique_ptr<CommandList> copy_cmd_list;

		std::unique_ptr<Fence> fence;
		Uint64 fence_value;


		std::unique_ptr<PipelineState> generate_mips_pso;
	};
}