#pragma once
#include "../cesar_core/cesar_core.h"
#include "../cesar_core/Platform/Window.h"

#include "Graphics/Device.h"
#include "Graphics/Swapchain.h"

#include "Graphics/Buffer.h"
#include "Graphics/Texture.h"

#include "Graphics/Descriptor.h"
#include "Graphics/DescriptorHeap.h"
#include "Graphics/DescriptorAllocator.h"
#include "Graphics/DescriptorRingAllocator.h"

#include "Graphics/CommandList.h"
#include "Graphics/CommandQueue.h"

#include <DirectXMath.h>
#include <Tracy/tracy/Tracy.hpp>

namespace cesar
{

	class _declspec(dllexport) GPUContext
	{
	public:
		GPUContext();
		~GPUContext();

		std::unique_ptr<CommandList> CreateGraphicsCommandList(const Char* name = "Graphics Command List");
		std::unique_ptr<CommandList> CreateComputeCommandList(const Char* name = "Compute Command List");
		std::unique_ptr<CommandList> CreateCopyCommandList(const Char* name = "Copy Command List");

		std::unique_ptr<PipelineState> CreateGraphicsPipelineState(const GraphicsPipelineStateDesc& desc, const Char* name = "Unnamed Graphics Pipeline State");
		std::unique_ptr<PipelineState> CreateMeshShaderPipelineState(const MeshPipelineStateDesc& desc, const Char* name = "Unnamed Mesh Pipeline State");
		std::unique_ptr<PipelineState> CreateComputePipelineState(const ComputePipelineStateDesc& desc, const Char* name = "Unnamed Compute Pipeline State");


		std::unique_ptr<Fence> CreateFence(const Char* name);

		std::unique_ptr<Texture> CreateTexture(const TextureDesc& desc, const Char* name);
		std::unique_ptr<Texture> CreateTexture(const TextureDesc& desc, ComPtr<ID3D12Resource> d3d12_texture, const Char* name);

		std::unique_ptr<Buffer> CreateBuffer(const BufferDesc& desc, const Char* name = "Unnamed Buffer");
		template<typename T>
		std::unique_ptr<Buffer> CreateConstantBuffer(const Char* name = "Unnamed Constant Buffer") {
			return std::make_unique<Buffer>(this, ConstantBufferDesc(CESAR_SIZEOF(T)), name);
		}

		Descriptor CreateTextureRTV(Texture* backbuffer, const TextureViewDesc& view_desc);
		Descriptor CreateTextureDSV(Texture* depthmap,   const TextureViewDesc& view_desc);
		Descriptor CraeteTextureSRV(Texture* texture,    const TextureViewDesc& view_desc, Bool force_cpu_descriptor = false, Bool is_persistent = false);
		Descriptor CreateTextureUAV(Texture* texture,    const TextureViewDesc& view_desc, Bool force_cpu_descriptor = false, Bool is_persistent = false);

		Descriptor CreateConstantBufferView(Buffer* buffer, const BufferViewDesc&,           Bool force_cpu_descriptor,         Bool is_persistent);
		Descriptor CreateBufferSRV(Buffer* buffer,          const BufferViewDesc& view_desc, Bool force_cpu_descriptor = false, Bool is_persistent = false);
		Descriptor CreateBufferUAV(Buffer* buffer,          const BufferViewDesc& view_desc, Bool force_cpu_descriptor = false, Bool is_persistent = false);

		void FreeCPUDescriptor(Descriptor descriptor);
		Uint32 GetBindlesDescriptorIndex(Descriptor descriptor);

		//Misc
		void SignalGraphicsCommandQueue(Fence* fence, Uint32 fence_value);
		void SignalComputeCommandQueue(Fence* fence, Uint32 fence_value);
		void SignalCopyCommandQueue(Fence* fence, Uint32 fence_value);

		// Core Getters
		ID3D12Device6* GetDevice();
		IDXGIFactory4* GetFactory();
		const GPUDeviceCapabilities& GetGPUCapabilities();

		CommandQueue* GetGraphicsCommandQueue()const;
		CommandQueue* GetComputeCommandQueue()const;
		CommandQueue* GetCopyCommandQueue()const;

		D3D12MA::Allocator* GetAllocator()const;

		OnlineDescriptorAllocator* GetOnlineDescriptorAllocator();
		DescriptorHeap* GetGPUDescriptorHeap()const;

		ID3D12RootSignature* GetGlobalRootSignature()const;

	private:
		void CreateCommonRootSignature();

		std::unique_ptr<CommandList> CreateCommandListImpl(CommandListType type, const Char* name);

		Descriptor CreateTextureViewImpl(Texture* texture,       DescriptorType view_type, const TextureViewDesc& view_desc, Bool force_cpu_descriptor = false, Bool is_persistent = false);
		Descriptor CreateTextureReadWriteViewImpl(Texture* texture, DescriptorType view_type, const TextureViewDesc& view_desc, Bool force_cpu_descriptor = false, Bool is_persistent = false);

		Descriptor CreateBufferReadOnlyViewImpl(Buffer* buffer,  DescriptorType view_type, const BufferViewDesc& view_desc,  Bool force_cpu_descriptor = false, Bool is_persistent = false);
		Descriptor CreateBufferReadWriteViewImpl(Buffer* buffer, DescriptorType view_type, const BufferViewDesc& view_desc,  Bool force_cpu_descriptor = false, Bool is_persistent = false);
		Descriptor CreateCBVViewImpl(Buffer*& buffer);


		Descriptor AllocateGPUDescriptorImpl(Bool is_persistent);
		Descriptor AllocateCPUDescriptorImpl(DescriptorType type);

		void FreeCPUDescriptorImpl(DescriptorType type, Descriptor descriptor);
	private:
		std::unique_ptr<Device> device;

		std::unique_ptr<CommandQueue> graphics_command_queue;
		std::unique_ptr<CommandQueue> compute_command_queue;
		std::unique_ptr<CommandQueue> copy_command_queue;

		std::array<std::unique_ptr<DescriptorAllocator>, static_cast<Uint32>(DescriptorType::Unknown)> descriptor_pool;
		std::unique_ptr<OnlineDescriptorAllocator> online_descriptor_pool;

		D3D12MA::Allocator* allocator = nullptr;
		ComPtr<ID3D12RootSignature> global_root_signature = nullptr;
	};

}