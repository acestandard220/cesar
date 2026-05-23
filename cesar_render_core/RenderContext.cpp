// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "RenderContext.h"
#include "GPUContext.h"

namespace cesar
{
	RenderContext::RenderContext(Window* _window, GPUContext* gpu_context)
		:window(_window), gpu_context(gpu_context)
	{
		window->GetClientDimensions(frame_width, frame_height);

		for (Uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			graphics_command_list[i] = gpu_context->CreateGraphicsCommandList(std::format("Graphics Command List - {}", i).c_str());
			compute_command_list[i]  = gpu_context->CreateComputeCommandList(std::format("Compute Command List - {}", i).c_str());
			copy_command_list[i]     = gpu_context->CreateCopyCommandList(std::format("Copy Command List - {}", i).c_str());
		}

		frame_fence_value = 0;
		frame_fence = gpu_context->CreateFence("Frame Fence");
		frame_fence_values.fill(0);

		graphics_fence = gpu_context->CreateFence("Graphics Fence");
		graphics_fence_value = 0;

		compute_fence = gpu_context->CreateFence("Compute Fence");
		compute_fence_value = 0;

		copy_fence = gpu_context->CreateFence("Copy Fence");
		copy_fence_value = 0;

		wait_fence = gpu_context->CreateFence("Wait Fence");
		wait_fence_value = 0;

		SwapchainDesc swapchain_desc{};
		swapchain_desc.width  = frame_width;
		swapchain_desc.height = frame_height;

		swapchain = std::make_unique<Swapchain>(this, swapchain_desc);
	}

	RenderContext::~RenderContext()
	{

	}

	void RenderContext::OnWindowResize(cesar::Uint32 width, cesar::Uint32 height)
	{
		WaitAll(); // Force Sync All CommandQueues to this point. Then resize.
		swapchain->OnResize(width, height);
	}

	Uint32 RenderContext::GetBindlesDescriptorIndex(Descriptor const& descriptor)
	{
		return gpu_context->GetBindlesDescriptorIndex(descriptor);
	}

	void RenderContext::FreeCPUDescriptor(Descriptor descriptor)
	{
		gpu_context->FreeCPUDescriptor(descriptor);
	}

	void RenderContext::Begin()
	{
		ZoneScopedN("RenderContext::Begin");

		frame_index = swapchain->GetBackbufferIndex();
		frame_fence->Wait(frame_fence_values[frame_index]);
		copy_fence->Wait(copy_fence_value);
			
		gpu_context->GetOnlineDescriptorAllocator()->ReleaseFinishedCompletedFrames(frame_fence_values[frame_index]);

		graphics_command_list[frame_index]->Begin();
		compute_command_list[frame_index]->Begin();
	}

	void RenderContext::End()
	{
		ZoneScopedN("RenderContext::End");

		graphics_command_list[frame_index]->End();
		compute_command_list[frame_index]->End();

		graphics_command_list[frame_index]->Submit();
		compute_command_list[frame_index]->Submit();

		gpu_context->SignalGraphicsCommandQueue(frame_fence.get(), ++frame_fence_value);
		frame_fence_values[frame_index] = frame_fence_value;

		gpu_context->GetOnlineDescriptorAllocator()->FinishCurrentFrame(frame_fence_value);

		swapchain->Present();
	}

	HWND RenderContext::GetWindowHandle()
	{
		return window->GetNative();
	}

	void RenderContext::WaitAll()
	{
		wait_fence_value++;
		gpu_context->SignalGraphicsCommandQueue(wait_fence.get(), wait_fence_value);
		wait_fence->Wait(wait_fence_value);

		wait_fence_value++;
		gpu_context->SignalComputeCommandQueue(wait_fence.get(), wait_fence_value);
		wait_fence->Wait(wait_fence_value);
		
		wait_fence_value++;
		gpu_context->SignalCopyCommandQueue(wait_fence.get(), wait_fence_value);
		wait_fence->Wait(wait_fence_value);
	}


	Uint32 RenderContext::GetFrameIndex() const
	{
		return frame_index;
	}

	Fence* RenderContext::GetGraphicsFence() const
	{
		return graphics_fence.get();
	}

	Fence* RenderContext::GetComputeFence() const
	{
		return compute_fence.get();
	}

	Fence* RenderContext::GetCopyFence() const
	{
		return copy_fence.get();
	}

	Uint64 RenderContext::GetGraphicsFenceValue() const
	{
		return graphics_fence_value;
	}

	Uint64 RenderContext::GetComputeFenceValue() const
	{
		return compute_fence_value;
	}

	Uint64 RenderContext::GetCopyFenceValue() const
	{
		return copy_fence_value;
	}

	void RenderContext::SetGraphicsFenceValue(Uint64 fence_value)
	{
		graphics_fence_value = fence_value;
	}

	void RenderContext::SetComputeFenceValue(Uint64 fence_value)
	{
		compute_fence_value = fence_value;
	}
	void RenderContext::SetCopyFenceValue(Uint64 fence_value)
	{
		copy_fence_value = fence_value;
	}

	CommandList* RenderContext::GetGraphicsCommandList() { return graphics_command_list[frame_index].get(); }
	CommandList* RenderContext::GetCopyCommandList()     { return copy_command_list[frame_index].get(); }
	CommandList* RenderContext::GetComputeCommandList()  { return compute_command_list[frame_index].get(); }

	GPUContext* RenderContext::GetGPUContext() const
	{
		return gpu_context;
	}
}