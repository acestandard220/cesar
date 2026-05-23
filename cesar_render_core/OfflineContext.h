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

		std::unique_ptr<Texture> CreateTexture(const void* data, const TextureDesc& desc);
		void GenerateMips(Texture* texture, Uint32 srv_index);

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