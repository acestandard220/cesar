// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../../cesar_core/cesar_core.h"

#include "Device.h"
#include "Texture.h"
#include "Buffer.h"
#include "Descriptor.h"
#include "PipelineState.h"
#include "CommandSignature.h"

#include <ranges>
#include <algorithm>
#include <utility>
#include <vector>

namespace cesar
{

	enum class ReadAccessType :Uint8 {
		PixelShader,
		NonPixelShader,
		AllShader
	};

	enum class LoadResourceFlag : Uint8
	{
		Preserve = 0,
		Discard = 1,
		Clear = 2
	};

	enum class StoreResourceFlag : Uint8
	{
		Preserve = 0,
		Discard = 1
	};

	inline constexpr D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE ToD3D12LoadAccessFlag(LoadResourceFlag flag) {
		switch (flag) {
		case LoadResourceFlag::Preserve: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
		case LoadResourceFlag::Discard:  return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		case LoadResourceFlag::Clear:    return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		default:
			LOG_TRACE("Shun dey fool");
		}
	}

	inline constexpr D3D12_RENDER_PASS_ENDING_ACCESS_TYPE ToD3D12StoreAccessFlag(StoreResourceFlag flag) {
		switch (flag) {
		case StoreResourceFlag::Preserve: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
		case StoreResourceFlag::Discard:  return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		}
	}


	struct ColorAttachment {
		Descriptor descriptor;
		LoadResourceFlag begin_flags;
		StoreResourceFlag end_flags;
		ClearValue clear_value;
	};

	struct DepthStencilAttachment {
		Descriptor descriptor;
		LoadResourceFlag depth_begin_flag;
		StoreResourceFlag depth_end_flag;
		LoadResourceFlag stencil_begin_flag;
		StoreResourceFlag stencil_end_flag;
		ClearValue clear_value;
	};

	struct RenderPassDesc
	{
		std::vector<ColorAttachment> render_targets;
		std::optional<DepthStencilAttachment> depth_stencil_targerts;
		Uint32 width, height;
		Bool legacy = false;
	};



	enum class CommandListType
	{
		Graphics,
		Compute,
		Copy
	};

	class GPUContext;
	class CommandQueue;
	class Fence;

	class _declspec(dllexport) CommandList
	{
	public:
		struct CommandListExecuteContext {
			ID3D12RootSignature* root_signature = nullptr;
			DescriptorHeap* descriptor_heap = nullptr;
		};

	public:
		CommandList(GPUContext* gpu_context, CommandListType type,const cesar::Char* name = "CommandList");
		~CommandList();

		void SetCommandQueue(CommandQueue* command_queue);
		void Submit();

		void Begin(const CommandListExecuteContext& execute_context = {});
		void End();

		void BeginRenderPass(RenderPassDesc& render_pass_desc);
		void EndRenderPass();

		void Signal(Fence* fence, Uint64 signal_value);
		void FlushSignals();

		void Waits(Fence* fence, Uint64 wait_value);
		void FlushWaits();

		void DrawIndexed(cesar::Uint32 index_count, cesar::Uint32 instance_count, cesar::Uint32 index_start_location, cesar::Uint32 vertex_start_location, cesar::Uint32 instance_start);
		void DispatchMesh(Uint32 x, Uint32 y, Uint32 z);
		void DispatchCompute(Uint32 x, Uint32 y, Uint32 z);

		void DrawIndexedIndirect(Buffer* buffer);
		void DispatchMeshIndirect(Buffer* buffer);
		void DispatchComputeIndirect(Buffer* buffer);

		void TextureBarrier(Texture* texture, ResourceState before_state, ResourceState after_state, const TextureViewDesc& view_desc = CESAR_DEFAULT_TEXTURE_VIEW_DESC);
		void BufferBarrier(Buffer* buffer, ResourceState before_state, ResourceState after_state);
		void GlobalBarrier(ResourceState before_state, ResourceState after_state);

		void FlushBarriers();

		void CopyTexture(Texture* src_texture, Texture* dst_texture);

		void CopyBuffer(Buffer* src_buffer, Buffer* dst_buffer);
		void CopyBuffer(Buffer* src_buffer, cesar::Uint64 src_offset, Buffer* dst_buffer, cesar::Uint64 dst_offset, cesar::Uint64 size);
		void CopyBufferToTexture(Buffer* src_buffer, Texture* dst_texture, Uint32 subresource_index = 0);

		void ClearRenderTargetView(const std::span<Descriptor>& rtv, cesar::Float rgba[4]);
		void ClearDepthTargetView(const Descriptor& dsv, cesar::Float depth);
		void SetRenderTarget(const std::span<Descriptor>& rtv, const Descriptor& dsv);

		void SetPipelineState(PipelineState* pipeline_state);
		void SetViewport(cesar::Float x, cesar::Float y, cesar::Float width, cesar::Float height);
		void SetPrimitiveTopology(PrimitiveTopology topology);
		void SetVertexBuffer(Buffer* buffer);
		void SetIndexBuffer(Buffer* buffer);

		void SetComputeCBV(Uint32 index, Buffer* buffer) {
			command_list->SetComputeRootConstantBufferView(index, buffer->GetGPUAddress());
		}

		void SetGraphicsCBV(Uint32 index, Buffer* buffer) {
			command_list->SetGraphicsRootConstantBufferView(index, buffer->GetGPUAddress());
		}

		template<typename T>
		void SetGraphicsConstants(std::span<T> element_span) {
			const Uint32 push_size    = element_span.size_bytes();
			const Uint32 uint32_count = push_size / 4;
			CESAR_ASSERT((push_size <= 12 * 4) && "Render Context Configuration only allows for 10 root constant.");
		
			command_list->SetGraphicsRoot32BitConstants(1, uint32_count, element_span.data(), 0);
		}

		template<typename T>
		void SetComputeConstants(std::span<T> element_span) {
			const Uint32 push_size    = element_span.size_bytes();
			const Uint32 uint32_count = push_size / 4;
			CESAR_ASSERT((push_size <= 12 * 4) && "Render Context Configuration only allows for 10 root constant.");

			command_list->SetComputeRoot32BitConstants(1, uint32_count, element_span.data(), 0);
		}

		void ClearUAVUint(Buffer* buffer, Descriptor descriptor, const Uint32* clear_values);

		ID3D12GraphicsCommandList7* GetCommandList();
		ID3D12CommandAllocator* GetCommandAllocator();

	private:
		GPUContext* gpu_context;
		ComPtr<ID3D12GraphicsCommandList7> command_list;
		ComPtr<ID3D12CommandAllocator> command_allocator;
		CommandListType type;

		CommandQueue* command_queue = nullptr;

		PipelineState* current_pipeline_state = nullptr;

		cesar::Bool use_legacy_barriers = false;
		std::vector<D3D12_BUFFER_BARRIER>   buffer_barriers;
		std::vector<D3D12_TEXTURE_BARRIER>  texture_barriers;
		std::vector<D3D12_GLOBAL_BARRIER>   global_barriers;
		std::vector<D3D12_RESOURCE_BARRIER> legacy_barriers;

		std::vector<std::pair<Fence*, Uint64>> pending_signals;
		std::vector<std::pair<Fence*, Uint64>> pending_waits;

		std::unique_ptr<CommandSignature> draw_indexed_cmd_signature;
		std::unique_ptr<CommandSignature> dispatch_compute_cmd_signature;
		std::unique_ptr<CommandSignature> dispatch_mesh_cmd_signature;
	};

}
