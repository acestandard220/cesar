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

	void OfflineContext::Begin()
	{
		fence->Wait(fence_value);
		gfx_cmd_list->Begin();
	}

	void OfflineContext::End()
	{
		gfx_cmd_list->End();

		gfx_cmd_list->Signal(fence.get(), ++fence_value);
		gfx_cmd_list->Submit();
	}

	std::unique_ptr<Texture> OfflineContext::CreatePersistentTexture(const TextureDesc& desc, const Char* name)
	{
		return gpu_context->CreateTexture(desc, name);
	}

	Buffer* OfflineContext::CreateReadbackBuffer(Uint32 element_count, const Char* name)
	{
		return gpu_context->CreateReadbackBuffer<Uint8>(element_count, name);
	}

	Buffer* OfflineContext::CreateUploadbuffer(Uint32 element_count, const Char* name)
	{
		return new Buffer(gpu_context, UploadBufferDesc<Uint8>(element_count), name);
	}

	void OfflineContext::UploadTextureData(Buffer* buffer, Texture* texture) {
		gfx_cmd_list->BufferBarrier(buffer, ResourceState::Common, ResourceState::CopySrc);
		gfx_cmd_list->FlushBarriers();

		gfx_cmd_list->CopyBufferToTexture(buffer,0,0, texture, false);

		gfx_cmd_list->TextureBarrier(texture, ResourceState::CopyDst, ResourceState::ComputeSRV);
		gfx_cmd_list->BufferBarrier(buffer, ResourceState::CopySrc, ResourceState::Common);

		gfx_cmd_list->FlushBarriers();
	}

	void OfflineContext::UploadTextureData(Buffer* buffer, Subresource* subresources, Uint32 subresource_count, Texture* texture) {
		gfx_cmd_list->BufferBarrier(buffer, ResourceState::Common, ResourceState::CopySrc);
		gfx_cmd_list->FlushBarriers();

		for (Uint32 i = 0; i < subresource_count; i++)
		{
			gfx_cmd_list->CopyBufferToTexture(buffer, subresources[i].offset, i, texture, true);
		}

		gfx_cmd_list->TextureBarrier(texture, ResourceState::CopyDst, ResourceState::ComputeSRV);
		gfx_cmd_list->BufferBarrier(buffer, ResourceState::CopySrc, ResourceState::Common);

		gfx_cmd_list->FlushBarriers();
	}
	
	void OfflineContext::GenerateMips(Texture* texture, Uint32 srv_index)
	{
		struct Constants
		{
			Uint32 src_texture_idx;
			Uint32 dst_texture_idx;

			DirectX::XMFLOAT2 TexelSize; 
		} constants = {
			.src_texture_idx = srv_index
		};

	    gfx_cmd_list->SetPipelineState(generate_mips_pso.get());
		
		for (Uint32 i = 1; i < texture->GetDesc().mips; i++)
		{
			Descriptor descriptor = gpu_context->CreateTextureUAV(texture, { .mip_levels = 1, .mip = i, .array_size = 1, .slice = 0 }, false, true);

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
	}

	//Probably move to Texture.h
	inline const Uint64 ComputeSubresourceOffset(const TextureDesc& desc, Uint32 subresource_index)
	{
		Uint64 offset = 0;
		Uint32 pixel_size = GetFormatStride(desc.format);

		for (Uint32 i = 0; i < subresource_index; ++i)
		{
			Uint32 mip_index = i % desc.mips;
			Uint32 mip_width = std::max(1u, desc.width >> mip_index);
			Uint32 mip_height = std::max(1u, desc.height >> mip_index);

			Uint32 row_pitch = (mip_width * pixel_size + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1)
				& ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1);

			Uint64 subresource_size = (Uint64)row_pitch * mip_height;
			offset += (subresource_size + D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1)
				& ~(Uint64)(D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1);
		}

		return offset;
	}

	

	Buffer* OfflineContext::GetTextureSubRegionPixels(Texture* texture, const TextureViewDesc& view_desc)
	{
		TextureDesc texture_desc = texture->GetDesc();
		Buffer* buffer = gpu_context->CreateReadbackBuffer<Float>(texture_desc.width * texture_desc.height);

		gfx_cmd_list->TextureBarrier(texture, ResourceState::PixelSRV, ResourceState::CopySrc, {});
		gfx_cmd_list->FlushBarriers();

		Uint32 max_subresource_count = view_desc.mip_levels * view_desc.array_size;
		Uint32 subresource_index = view_desc.mip + (view_desc.slice * view_desc.mip_levels);

		Uint64 offset = ComputeSubresourceOffset(texture_desc, subresource_index);
		gfx_cmd_list->CopyTextureToBuffer(texture, buffer, subresource_index, offset);

		gfx_cmd_list->TextureBarrier(texture, ResourceState::CopySrc, ResourceState::PixelSRV, {});
		gfx_cmd_list->FlushBarriers();

		return buffer;
	}

	Buffer* OfflineContext::GetTexturePixels(Texture* texture)
	{
		TextureDesc texture_desc = texture->GetDesc();
		Buffer* buffer = gpu_context->CreateReadbackBuffer<Float>(texture->GetTextureCopyableSize());

		gfx_cmd_list->TextureBarrier(texture, ResourceState::PixelSRV, ResourceState::CopySrc, {});
		gfx_cmd_list->FlushBarriers();
		
		Uint32 max_subresource_count = texture_desc.mips * texture_desc.array_size;

		for (Uint32 i = 0; i < max_subresource_count; i++)
		{
			Uint64 offset = ComputeSubresourceOffset(texture_desc, i);
			gfx_cmd_list->CopyTextureToBuffer(texture, buffer, i, offset);
		}

		gfx_cmd_list->TextureBarrier(texture, ResourceState::CopySrc, ResourceState::PixelSRV, {});
		gfx_cmd_list->FlushBarriers();

		return buffer;
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