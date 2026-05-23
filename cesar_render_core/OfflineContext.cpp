#include "OfflineContext.h"


namespace cesar
{
	OfflineContext::OfflineContext(GPUContext* gpu_context)
		:gpu_context(gpu_context)
	{
		gfx_cmd_list = gpu_context->CreateGraphicsCommandList("Graphics Command List - Offline Context");
		compute_cmd_list = gpu_context->CreateComputeCommandList("Compute Command List - Offline Context");
		copy_cmd_list = gpu_context->CreateCopyCommandList("Copy Command List - Offline Context");

		fence = gpu_context->CreateFence("Fence - Offline Context");
		fence_value = 0;

		CreatePipelineStates();
	}
	
	OfflineContext::~OfflineContext()
	{
	
	}

	std::unique_ptr<Texture> OfflineContext::CreateTexture(const void* data, const TextureDesc& desc) {
		const Uint64 byte_size = desc.width * desc.height * GetFormatStride(desc.format);

		auto texture = gpu_context->CreateTexture(desc, "ImageTexture");
		auto buffer = gpu_context->CreateBuffer(UploadBufferDesc<Uint8>(byte_size));
		buffer->Upload<Uint8>(std::span<Uint8>((Uint8*)(data), byte_size));

		fence->Wait(fence_value);

		gfx_cmd_list->Begin();

		gfx_cmd_list->BufferBarrier(buffer.get(), ResourceState::Common, ResourceState::CopySrc);
		gfx_cmd_list->FlushBarriers();

		gfx_cmd_list->CopyBufferToTexture(buffer.get(), texture.get(), 0);

		gfx_cmd_list->TextureBarrier(texture.get(), ResourceState::CopyDst, ResourceState::ComputeSRV);
		gfx_cmd_list->BufferBarrier(buffer.get(), ResourceState::CopySrc, ResourceState::Common);

		gfx_cmd_list->FlushBarriers();
		gfx_cmd_list->End();

		gfx_cmd_list->Signal(fence.get(), ++fence_value);
		gfx_cmd_list->Submit();

		return texture;
	}
	
	void OfflineContext::GenerateMips(Texture* texture, Uint32 srv_index)
	{
		fence->Wait(fence_value);

		CommandList::CommandListExecuteContext execute_context{};
		execute_context.descriptor_heap = gpu_context->GetGPUDescriptorHeap();
		execute_context.root_signature  = gpu_context->GetGlobalRootSignature();

		gfx_cmd_list->Begin(execute_context);

		struct Constants
		{
			Uint32 src_texture_idx;
			Uint32 dst_texture_idx;

			DirectX::XMFLOAT2 TexelSize; 
		} constants = {
			.src_texture_idx = srv_index
		};

		for (Uint32 i = 1; i < texture->GetDesc().mips; i++)
		{
			Descriptor descriptor = gpu_context->CreateTextureUAV(texture, { .mip_levels = 1, .mip = i, .array_size = 1, .slice = 0 }, false, true);
			gfx_cmd_list->SetPipelineState(generate_mips_pso.get());

			constants.src_texture_idx = srv_index + (i - 1);
			constants.dst_texture_idx = descriptor.index;
			constants.TexelSize = DirectX::XMFLOAT2(1.0f / std::max(texture->GetDesc().width >> i, 1u), 1.0f / std::max(texture->GetDesc().height >> i, 1u));

			gfx_cmd_list->TextureBarrier(texture, ResourceState::ComputeSRV, ResourceState::ComputeUAV, { .mip_levels = 1,.mip = i,.array_size = 1,.slice = 0 });
			gfx_cmd_list->FlushBarriers();

			gfx_cmd_list->SetComputeConstants(std::span(&constants, 1));
			gfx_cmd_list->DispatchCompute(
				(std::max(texture->GetDesc().width  >> i, 1u) + 7) / 8,
				(std::max(texture->GetDesc().height >> i, 1u) + 7) / 8,
				1
			);

			gfx_cmd_list->TextureBarrier(texture, ResourceState::ComputeUAV, ResourceState::ComputeSRV, { .mip_levels = 1,.mip = i,.array_size = 1,.slice = 0 });
			gfx_cmd_list->FlushBarriers();
		}

		gfx_cmd_list->TextureBarrier(texture, ResourceState::ComputeSRV, ResourceState::PixelSRV, {});
		gfx_cmd_list->FlushBarriers();

		gfx_cmd_list->End();
		gfx_cmd_list->Signal(fence.get(), ++fence_value);
		gfx_cmd_list->Submit();
	}

	Uint32 OfflineContext::AllocateBindlessTextureSRV(Texture* texture) {
		Descriptor descriptor = gpu_context->CraeteTextureSRV(texture, CESAR_DEFAULT_TEXTURE_VIEW_DESC, false, true);
		return descriptor.index;
	}

	void OfflineContext::CreatePipelineStates()
	{
		ComputePipelineStateDesc compute_desc{};
		compute_desc.cs = ShaderID::GenerateMipMap;
		generate_mips_pso = gpu_context->CreateComputePipelineState(compute_desc, "Generate Mip Map PSO");
	}
}

//SubresourceIndex = MipSlice + (ArraySlice * MipLevels) + (PlaneSlice * MipLevels * ArraySize)