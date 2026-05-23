// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "CommandList.h"
#include "../GPUContext.h"
#include "Fence.h"
#include "Texture.h"
#include"Resource.h"

namespace cesar
{
	using namespace cesar;
	static constexpr D3D12_COMMAND_LIST_TYPE ToD3D12CommandListType(CommandListType type)
	{
		switch (type)
		{
			case CommandListType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
			case CommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			case CommandListType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
			default: CESAR_DEBUGBREAK();
		}
	}

	CommandList::CommandList(GPUContext* gpu_context, CommandListType type, const Char* name)
		:gpu_context(gpu_context), use_legacy_barriers(!gpu_context->GetGPUCapabilities().SupportEnhanceBarriers()), type(type)
	{
		ID3D12Device* device = gpu_context->GetDevice();

		const D3D12_COMMAND_LIST_TYPE command_list_type = ToD3D12CommandListType(type);
		HRESULT hr = device->CreateCommandAllocator(command_list_type, IID_PPV_ARGS(&command_allocator));
		if (FAILED(hr))
		{
			LOG_FATAL("Failed to create Command Allocator.");
			CESAR_DEBUGBREAK();
		}
		device->CreateCommandList(0, command_list_type, command_allocator.Get(), nullptr, IID_PPV_ARGS(&command_list));
		command_list->Close();

		CESAR_NAME_D3D12_OBJECT(command_list, name);
		CESAR_NAME_D3D12_OBJECT(command_allocator, std::format("{}_Allocator", name).c_str());


		draw_indexed_cmd_signature     = std::make_unique<CommandSignature>(gpu_context, CommandArgumentType::DrawIndexed,     "DrawIndexed_CommandSignature");
		dispatch_compute_cmd_signature = std::make_unique<CommandSignature>(gpu_context, CommandArgumentType::DispatchCompute, "DispatchCompute_CommandSignature");
		dispatch_mesh_cmd_signature    = std::make_unique<CommandSignature>(gpu_context, CommandArgumentType::DispatchMesh,    "DispatchMesh_CommandSignature");
	}


	CommandList::~CommandList()
	{
		command_allocator.Reset();
		command_list.Reset();
	}

	void CommandList::SetCommandQueue(CommandQueue* command_queue)
	{
		CESAR_ASSERT(type == command_queue->GetCommandListType() && "Queue type must match list type");
		this->command_queue = command_queue;
	}

	void CommandList::Submit()
	{
		FlushWaits();
		std::vector<CommandList*> list = { this };
		command_queue->Execute(list);
		FlushSignals();
	}

	void CommandList::Begin(const CommandListExecuteContext& execute_context)
	{
		command_allocator->Reset();
		command_list->Reset(command_allocator.Get(), nullptr);

		current_pipeline_state = nullptr;

		if(type != CommandListType::Copy)
		{
			ID3D12DescriptorHeap* heaps[] = { gpu_context->GetGPUDescriptorHeap()->GetDescriptorHeap() };
			command_list->SetDescriptorHeaps(1, heaps);

			ID3D12RootSignature* root_signature = (execute_context.root_signature)? execute_context.root_signature : gpu_context->GetGlobalRootSignature();
			command_list->SetComputeRootSignature(root_signature);
			if (type == CommandListType::Graphics)
			{
				command_list->SetGraphicsRootSignature(root_signature);
			}
		}		
	}

	void CommandList::End()
	{
		FlushBarriers();
		command_list->Close();
	}

	void CommandList::BeginRenderPass(RenderPassDesc& render_pass_desc)
	{
		std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> render_targets(render_pass_desc.render_targets.size());

		Uint32 i = 0;
		for (auto& rtv : render_pass_desc.render_targets)
		{
			D3D12_RENDER_PASS_RENDER_TARGET_DESC& render_target_desc = render_targets[i];
			render_target_desc.BeginningAccess = { ToD3D12LoadAccessFlag(rtv.begin_flags), ToD3D12ClearValue(rtv.clear_value) };
			render_target_desc.EndingAccess = { ToD3D12StoreAccessFlag(rtv.end_flags) };
			render_target_desc.cpuDescriptor = GetCPUHandle(rtv.descriptor);
			i++;
		}

		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depth_stencil_targets;
		D3D12_RENDER_PASS_DEPTH_STENCIL_DESC* depth_stencil_targets_ptr = nullptr;
		if(render_pass_desc.depth_stencil_targerts.has_value())
		{
			const auto& dsv = render_pass_desc.depth_stencil_targerts.value();
			depth_stencil_targets.cpuDescriptor = GetCPUHandle(dsv.descriptor);
			depth_stencil_targets.DepthBeginningAccess = { ToD3D12LoadAccessFlag(dsv.depth_begin_flag), ToD3D12ClearValue(dsv.clear_value) };
			depth_stencil_targets.DepthEndingAccess = { ToD3D12StoreAccessFlag(dsv.depth_end_flag) };
			depth_stencil_targets.StencilBeginningAccess = { D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS };
			depth_stencil_targets.StencilEndingAccess = { D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS };
			
			depth_stencil_targets_ptr = &depth_stencil_targets;
		}

		command_list->BeginRenderPass(render_targets.size(), render_targets.data(), depth_stencil_targets_ptr, D3D12_RENDER_PASS_FLAG_NONE);
	}

	void CommandList::EndRenderPass()
	{
		command_list->EndRenderPass();
	}

	void CommandList::Signal(Fence* fence, Uint64 signal_value)
	{
		pending_signals.push_back(std::pair<Fence*,Uint64>{ fence,signal_value });
	}

	void CommandList::FlushSignals()
	{
		for (auto& [fence, value] : pending_signals)
		{
			command_queue->Signal(fence, value);
		}
		pending_signals.clear();
	}

	void CommandList::Waits(Fence* fence, Uint64 wait_value)
	{
		pending_waits.push_back({ fence,wait_value });
	}

	void CommandList::FlushWaits()
	{
		for (auto& [fence, value] : pending_waits) {
			command_queue->Wait(fence, value);
		}
		pending_waits.clear();
	}

	void CommandList::DrawIndexed(cesar::Uint32 index_count, cesar::Uint32 instance_count, cesar::Uint32 index_start_locatin, cesar::Uint32 vertex_start_location, cesar::Uint32 instance_start) {
		command_list->DrawIndexedInstanced(index_count, instance_count, index_start_locatin, vertex_start_location, instance_start);
	}

	void CommandList::DispatchMesh(Uint32 x, Uint32 y, Uint32 z)
	{
		command_list->DispatchMesh(x, y, z);
	}
	void CommandList::DispatchCompute(Uint32 x, Uint32 y, Uint32 z)
	{
		command_list->Dispatch(x, y, z);
	}

	void CommandList::DrawIndexedIndirect(Buffer* buffer) {
		command_list->ExecuteIndirect(draw_indexed_cmd_signature->GetCommandSignature(), 1, buffer->GetBuffer(), 0, nullptr, 0);
	}

	void CommandList::DispatchMeshIndirect(Buffer* buffer)
	{
		command_list->ExecuteIndirect(dispatch_mesh_cmd_signature->GetCommandSignature(), 1, buffer->GetBuffer(), 0, nullptr, 0);
	}

	void CommandList::DispatchComputeIndirect(Buffer* buffer)
	{
		command_list->ExecuteIndirect(dispatch_compute_cmd_signature->GetCommandSignature(), 1, buffer->GetBuffer(), 0, nullptr, 0);
	}

	void CommandList::TextureBarrier(Texture* texture, ResourceState before_state, ResourceState after_state, const TextureViewDesc& view_desc) {
		if (before_state == after_state)
			return;

		auto GetSubresourceFromViewDesc = [](const TextureViewDesc& view_desc, const TextureDesc& texture_desc) -> Uint32
		{
			if (view_desc.array_size == cesar::Uint32(-1) && view_desc.mip_levels == cesar::Uint32(-1))
				return D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			return D3D12CalcSubresource(view_desc.mip,view_desc.slice,0, texture_desc.mips,texture_desc.array_size);
		};

		auto GetBarrierSubresourceRange = [](const TextureViewDesc& view_desc, const TextureDesc& texture_desc) -> D3D12_BARRIER_SUBRESOURCE_RANGE
			{
				if (view_desc.array_size == cesar::Uint32(-1) && view_desc.mip_levels == cesar::Uint32(-1))
				{
					return {
						.IndexOrFirstMipLevel = 0,
						.NumMipLevels = texture_desc.mips,
						.FirstArraySlice = 0,
						.NumArraySlices = texture_desc.array_size,
						.FirstPlane = 0,
						.NumPlanes = 1
					};
				}

				return {
					.IndexOrFirstMipLevel = view_desc.mip,
					.NumMipLevels = (view_desc.mip_levels == cesar::Uint32(-1)) ? texture_desc.mips : view_desc.mip_levels,
					.FirstArraySlice = view_desc.slice,
					.NumArraySlices = (view_desc.array_size == cesar::Uint32(-1)) ? texture_desc.array_size : view_desc.array_size,
					.FirstPlane = 0,
					.NumPlanes = 1
				};
			};

		if (use_legacy_barriers) {
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

			barrier.Transition.pResource   = texture->GetTexture();
			barrier.Transition.StateBefore = ToD3D12State(before_state);
			barrier.Transition.StateAfter  = ToD3D12State(after_state);
			barrier.Transition.Subresource = GetSubresourceFromViewDesc(view_desc, texture->GetDesc());

			legacy_barriers.emplace_back(std::move(barrier));
		}
		else {
			D3D12_TEXTURE_BARRIER barrier{};
			barrier.SyncBefore   = ToD3D12BarrierSync(before_state);
			barrier.SyncAfter    = ToD3D12BarrierSync(after_state);
			barrier.AccessBefore = ToD3D12BarrierAccess(before_state);
			barrier.AccessAfter  = ToD3D12BarrierAccess(after_state);
			barrier.LayoutBefore = ToD3D12BarrierLayout(before_state);
			barrier.LayoutAfter  = ToD3D12BarrierLayout(after_state);
			barrier.pResource    = texture->GetTexture();
			barrier.Subresources = GetBarrierSubresourceRange(view_desc, texture->GetDesc());

			barrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

			texture_barriers.push_back(barrier);
		}
	}

	void CommandList::BufferBarrier(Buffer* buffer, ResourceState before_state, ResourceState after_state)
	{
		if (use_legacy_barriers) {
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

			barrier.Transition.pResource = buffer->GetBuffer();
			barrier.Transition.StateBefore = ToD3D12State(before_state);
			barrier.Transition.StateAfter = ToD3D12State(after_state);
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			legacy_barriers.emplace_back(std::move(barrier));
		}
		else {
			D3D12_BUFFER_BARRIER barrier{};
			barrier.SyncBefore = ToD3D12BarrierSync(before_state);
			barrier.SyncAfter = ToD3D12BarrierSync(after_state);
			barrier.AccessBefore = ToD3D12BarrierAccess(before_state);
			barrier.AccessAfter = ToD3D12BarrierAccess(after_state);;
			barrier.Offset = 0;
			barrier.Size = UINT64_MAX;
			barrier.pResource = buffer->GetBuffer();

			buffer_barriers.push_back(barrier);
		}
	}

	void CommandList::GlobalBarrier(ResourceState before_state, ResourceState after_state) {

		if (use_legacy_barriers) {
			CESAR_FEATURE_NO_IMPL("Leagacy Global Barriers have not implementation yet.");
		}
		else{
			D3D12_GLOBAL_BARRIER global_barrier{};
			global_barrier.SyncBefore = ToD3D12BarrierSync(before_state);
			global_barrier.SyncAfter = ToD3D12BarrierSync(after_state);
			global_barrier.AccessBefore = ToD3D12BarrierAccess(before_state);
			global_barrier.AccessAfter = ToD3D12BarrierAccess(after_state);

			global_barriers.push_back(global_barrier);
		}
	}

	void CommandList::FlushBarriers()
	{
		if (use_legacy_barriers) {
			command_list->ResourceBarrier(legacy_barriers.size(), legacy_barriers.data());
			legacy_barriers.clear();
			return;
		}

		std::vector<CD3DX12_BARRIER_GROUP> barrier_group;
		barrier_group.reserve(3);

		if (texture_barriers.size()) {
			barrier_group.push_back(CD3DX12_BARRIER_GROUP(texture_barriers.size(), texture_barriers.data()));
		}

		if (buffer_barriers.size()) {
			barrier_group.push_back(CD3DX12_BARRIER_GROUP(buffer_barriers.size(), buffer_barriers.data()));
		}

		if (barrier_group.size())
		{
			command_list->Barrier(barrier_group.size(), barrier_group.data());
		}

		texture_barriers.clear();
		buffer_barriers.clear();
	}

	void CommandList::CopyTexture(Texture* src_texture, Texture* dst_texture)
	{
		command_list->CopyResource(dst_texture->GetTexture(), src_texture->GetTexture());
	}

	void CommandList::CopyBuffer(Buffer* src_buffer, Buffer* dst_buffer) {
		command_list->CopyResource(dst_buffer->GetBuffer(), src_buffer->GetBuffer());
	}

	void CommandList::CopyBuffer(Buffer* src_buffer, cesar::Uint64 src_offset, Buffer* dst_buffer, 
		cesar::Uint64 dst_offset, cesar::Uint64 size)
	{
		command_list->CopyBufferRegion(dst_buffer->GetBuffer(), dst_offset, src_buffer->GetBuffer(), src_offset, size);
	}

	void CommandList::CopyBufferToTexture(Buffer* src_buffer, Texture* dst_texture, Uint32 subresource_index) {
		TextureDesc dst_desc = dst_texture->GetDesc();

		D3D12_TEXTURE_COPY_LOCATION srcLocation{};
		srcLocation.pResource = src_buffer->GetBuffer();
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		srcLocation.PlacedFootprint.Offset = 0;
		srcLocation.PlacedFootprint.Footprint = {
			.Format = ToDXGIFormat(dst_desc.format),
			.Width = dst_desc.width,
			.Height = dst_desc.height,
			.Depth = 1,
			.RowPitch = dst_desc.width * GetFormatStride(dst_desc.format)
		};

		D3D12_TEXTURE_COPY_LOCATION dstLocation{};
		dstLocation.pResource = dst_texture->GetTexture();
		dstLocation.SubresourceIndex = subresource_index;
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		command_list->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
	}

	void CommandList::ClearRenderTargetView(const std::span<Descriptor>& rtv, cesar::Float rgba[4])
	{
		auto rtv_cpu_handles = rtv |
			std::views::transform([](auto& v) {return GetCPUHandle(v); }) |
			std::ranges::to<std::vector>();

		for (const auto& handle : rtv_cpu_handles) {
			command_list->ClearRenderTargetView(handle, rgba, 0, nullptr);
		}
	}

	void CommandList::ClearDepthTargetView(const Descriptor& dsv,Float depth)
	{
		auto dsv_cpu_handle = GetCPUHandle(dsv);
		command_list->ClearDepthStencilView(dsv_cpu_handle, D3D12_CLEAR_FLAG_DEPTH, depth, 1, 0, nullptr);
	}

	void CommandList::SetRenderTarget(const std::span<Descriptor>& rtv, const Descriptor& dsv)
	{
		auto rtv_cpu_handles = rtv |
			std::views::transform([](auto& v) {return GetCPUHandle(v); }) |
			std::ranges::to<std::vector>();

		D3D12_CPU_DESCRIPTOR_HANDLE dsv_cpu_handle;
		D3D12_CPU_DESCRIPTOR_HANDLE* dsv_handle_ptr = nullptr;
		if(dsv.IsValid()) {
			dsv_cpu_handle = GetCPUHandle(dsv);
			dsv_handle_ptr = &dsv_cpu_handle;
		}

		command_list->OMSetRenderTargets(rtv.size(), rtv_cpu_handles.data(), TRUE, dsv_handle_ptr);
	}

	void CommandList::SetPipelineState(PipelineState* pipeline_state)
	{
		if (current_pipeline_state == pipeline_state)
			return;

		current_pipeline_state = pipeline_state;
		command_list->SetPipelineState(current_pipeline_state->GetPipelineState());
	}

	void CommandList::SetViewport(cesar::Float x, cesar::Float y, cesar::Float width, cesar::Float height)
	{
		D3D12_VIEWPORT viewport{
			.TopLeftX = x,
			.TopLeftY = y,
			.Width    = width,
			.Height   = height,
			.MinDepth = 0.0f,
			.MaxDepth = 1.0f
		};
		command_list->RSSetViewports(1, &viewport);

		D3D12_RECT rect{};
		rect.left   = x;
		rect.top    = y;
		rect.right  = width;
		rect.bottom = height;

		command_list->RSSetScissorRects(1, &rect);

	}

	void CommandList::SetPrimitiveTopology(PrimitiveTopology topology) {
		command_list->IASetPrimitiveTopology(ToD3DPrimitiveTopology(topology));
	}

	void CommandList::SetVertexBuffer(Buffer* buffer)
	{
		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view{};
		vertex_buffer_view.BufferLocation = buffer->GetGPUAddress();
		vertex_buffer_view.SizeInBytes = buffer->GetSize();
		vertex_buffer_view.StrideInBytes = buffer->GetStride();

		command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
	}

	void CommandList::SetIndexBuffer(Buffer* buffer) {
		D3D12_INDEX_BUFFER_VIEW index_buffer_view{};
		index_buffer_view.BufferLocation = buffer->GetGPUAddress();
		index_buffer_view.Format = ToDXGIFormat(buffer->GetFormat());
		index_buffer_view.SizeInBytes = buffer->GetSize();

		command_list->IASetIndexBuffer(&index_buffer_view);
	}

	void CommandList::ClearUAVUint(Buffer* buffer, Descriptor descriptor, const Uint32* clear_values)
	{
		CESAR_ASSERT((clear_values != nullptr) && "A clear value must be provided.");
		CESAR_ASSERT(descriptor.IsValid() && "Descriptor must be valid");
		CESAR_ASSERT(descriptor.parent_heap->IsShaderVisible() && "Descriptor must be shader visible.");

		Descriptor staging_descriptor = gpu_context->CreateBufferUAV(buffer, {}, true, false);
		command_list->ClearUnorderedAccessViewUint(GetGPUHandle(descriptor), GetCPUHandle(staging_descriptor), buffer->GetBuffer(), clear_values, 0, nullptr
		);
		gpu_context->FreeCPUDescriptor(staging_descriptor);
	}

	ID3D12GraphicsCommandList7* CommandList::GetCommandList()
	{
		return command_list.Get();
	}

	ID3D12CommandAllocator* CommandList::GetCommandAllocator()
	{
		return command_allocator.Get();
	}

}