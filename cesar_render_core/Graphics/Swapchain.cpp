// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "Swapchain.h"
#include "../GPUContext.h"
#include "../RenderContext.h"

namespace cesar {
	using namespace cesar;

	Swapchain::Swapchain(RenderContext* render_context, const SwapchainDesc& swapchain_info)
		:render_context(render_context), swapchain_format(DXGI_FORMAT_R8G8B8A8_UNORM)
	{
		HWND window_handle = render_context->GetWindowHandle();

		GPUContext* gpu_context = render_context->GetGPUContext();

		ID3D12Device* device = gpu_context->GetDevice();
		IDXGIFactory4* factory = gpu_context->GetFactory();

		ID3D12CommandQueue* command_queue = gpu_context->GetGraphicsCommandQueue()->GetCommandQueue();

		DXGI_SWAP_CHAIN_DESC1 swapchain_desc1{};
		swapchain_desc1.BufferCount = FRAMES_IN_FLIGHT;
		swapchain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_desc1.Format = swapchain_format;
		swapchain_desc1.Height = swapchain_info.height;
		swapchain_desc1.Width = swapchain_info.width;
		swapchain_desc1.SampleDesc.Count = 1;
		swapchain_desc1.SampleDesc.Quality = 0;
		swapchain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		ComPtr<IDXGISwapChain1>intermediate_swapchain;
		factory->CreateSwapChainForHwnd(command_queue, window_handle, &swapchain_desc1, nullptr, nullptr, intermediate_swapchain.GetAddressOf());
		intermediate_swapchain.As(&swapchain);

		backbuffer_index = swapchain->GetCurrentBackBufferIndex();
		CreateBackBuffers();		
	}

	void Swapchain::Present(Bool vsync)
	{
		HRESULT hr = swapchain->Present(vsync, 0);
		backbuffer_index = swapchain->GetCurrentBackBufferIndex();
		CESAR_D3D12_CHECK(hr);
	}

	void Swapchain::OnResize(Uint32 width, Uint32 height)
	{
		for (Uint32 i = 0; i < FRAMES_IN_FLIGHT; i++) {
			backbuffers[i].reset();
		}

		DXGI_SWAP_CHAIN_DESC swapchain_desc;
		swapchain->GetDesc(&swapchain_desc);

		HRESULT hr = swapchain->ResizeBuffers(FRAMES_IN_FLIGHT, width, height, swapchain_format, swapchain_desc.Flags);
		if (FAILED(hr)) {
			LOG_ERROR("Failed to resize swapchain.");
			return;
		}

		backbuffer_index = swapchain->GetCurrentBackBufferIndex();
		CreateBackBuffers();
	}

	void Swapchain::CreateBackBuffers()
	{
		GPUContext* gpu_context = render_context->GetGPUContext();

		for (Uint32 i = 0; i < FRAMES_IN_FLIGHT; i++) {
			ComPtr<ID3D12Resource> backbuffer = nullptr;

			HRESULT hr = swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffer));
			D3D12_RESOURCE_DESC backbuffer_desc = backbuffer->GetDesc();
			CESAR_D3D12_CHECK(hr);
			TextureDesc desc;
			desc.type = TextureType::Texture2D;
			desc.format = ResourceFormat::RGBA8_UNORM;
			desc.clear_value = ClearValue(0, 0, 0, 1);
			desc.height = backbuffer_desc.Height;
			desc.width = backbuffer_desc.Width;
			desc.bind_flag = ResourceBindFlag::RenderTarget;
			desc.intial_state = ResourceState::Present;
			backbuffers[i] = gpu_context->CreateTexture(desc, backbuffer, "Unnamed Texture");
			backbuffers[i]->SetName(std::format("Backbuffer_{}", i).c_str());

			TextureViewDesc view_desc{};
			backbuffer_descriptors[i] = gpu_context->CreateTextureRTV(backbuffers[i].get(), view_desc);
		}
	}
}
