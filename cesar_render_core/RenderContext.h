// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../cesar_core/cesar_core.h"
#include "../cesar_core/Platform/Window.h"

#include "Graphics/Device.h"
#include "Graphics/Swapchain.h"

#include "GPUContext.h"

#include <DirectXMath.h>
#include <Tracy/tracy/Tracy.hpp>

namespace cesar {

	class GPUContext;
	class _declspec(dllexport) RenderContext
	{
	public:
		RenderContext(Window* window, GPUContext* gpu_context);
		~RenderContext();

		void OnWindowResize(cesar::Uint32 width, cesar::Uint32 height);

		void Begin();
		void End();

		Uint32 GetBindlesDescriptorIndex(Descriptor const& descriptor);
		void FreeCPUDescriptor(Descriptor descriptor);

		HWND GetWindowHandle();
		
		void WaitAll();

		Uint32 GetFrameIndex()const;
		Texture* GetBackbuffer() { return swapchain->GetBackbuffer(); }

		Fence* GetGraphicsFence()const;
		Fence* GetComputeFence()const;
		Fence* GetCopyFence()const;

		Uint64 GetGraphicsFenceValue()const;
		Uint64 GetComputeFenceValue()const;
		Uint64 GetCopyFenceValue()const;

		void SetGraphicsFenceValue(Uint64 fence_value);
		void SetComputeFenceValue(Uint64 fence_value);
		void SetCopyFenceValue(Uint64 fence_value);

		CommandList* GetGraphicsCommandList();
		CommandList* GetCopyCommandList();
		CommandList* GetComputeCommandList();

		GPUContext* GetGPUContext()const;

	private:
		Window* window;
		GPUContext* gpu_context;

		Uint32 frame_index = 0;

		Uint32 frame_width = 0;
		Uint32 frame_height = 0;

		std::unique_ptr<Swapchain> swapchain;

		std::array<std::shared_ptr<CommandList>, FRAMES_IN_FLIGHT> graphics_command_list;
		std::array<std::shared_ptr<CommandList>, FRAMES_IN_FLIGHT> copy_command_list;
		std::array<std::shared_ptr<CommandList>, FRAMES_IN_FLIGHT> compute_command_list;

		Uint64 frame_fence_value;
		std::unique_ptr<Fence> frame_fence = nullptr;
		std::array<Uint64, FRAMES_IN_FLIGHT> frame_fence_values;

		std::unique_ptr<Fence> graphics_fence = nullptr;
		Uint64 graphics_fence_value;

		std::unique_ptr<Fence> compute_fence = nullptr;
		Uint64 compute_fence_value;

		std::unique_ptr<Fence> copy_fence = nullptr;
		Uint64 copy_fence_value;

		std::unique_ptr<Fence> wait_fence = nullptr;
		Uint64 wait_fence_value;
	};

}