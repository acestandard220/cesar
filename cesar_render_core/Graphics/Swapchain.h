// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../cesar_core/cesar_core.h"
#include "Device.h"
#include "Texture.h"
#include "Descriptor.h"

#include <d3d12.h>
#include <dxgi1_4.h>

namespace cesar
{
	struct SwapchainDesc
	{
		cesar::Uint64 width;
		cesar::Uint64 height;
	};

	class RenderContext;
	class Swapchain {

	public:
		Swapchain(RenderContext* render_context, const SwapchainDesc& swapchain);
		~Swapchain() = default;


		void Present(cesar::Bool vsync = false);

		void OnResize(cesar::Uint32 width, cesar::Uint32 height);

		cesar::Uint32 GetBackbufferIndex()const { return backbuffer_index; }
		Texture* GetBackbuffer()const { return backbuffers[backbuffer_index].get(); }
		const Descriptor& GetBackbufferDescriptor()const { return backbuffer_descriptors[backbuffer_index]; }

	private:
		void CreateBackBuffers();

	private:
		RenderContext* render_context;
		ComPtr<IDXGISwapChain3> swapchain;
		DXGI_FORMAT swapchain_format;

		std::unique_ptr<Texture> backbuffers[FRAMES_IN_FLIGHT];
		std::array<Descriptor, 3> backbuffer_descriptors;

		cesar::Uint32 backbuffer_index;
	};
}