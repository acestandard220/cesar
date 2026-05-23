#include "GPUContext.h"

namespace cesar
{
	GPUContext::GPUContext()
	{
		device = std::make_unique<Device>();
		ID3D12Device6* d3d12_device6 = device->GetDevice();

		graphics_command_queue = std::make_unique<CommandQueue>(this, CommandListType::Graphics, "Graphics Command Queue");
		compute_command_queue  = std::make_unique<CommandQueue>(this, CommandListType::Compute, "Compute Command Queue");
		copy_command_queue     = std::make_unique<CommandQueue>(this, CommandListType::Copy, "Copy Command Queue");

		D3D12MA::ALLOCATOR_DESC allocator_desc{};
		allocator_desc.Flags    = D3D12MA::ALLOCATOR_FLAGS::ALLOCATOR_FLAG_NONE;
		allocator_desc.pDevice  = d3d12_device6;
		allocator_desc.pAdapter = device->GetAdapter();
		CESAR_D3D12_CHECK(D3D12MA::CreateAllocator(&allocator_desc, &allocator));

		for (Uint32 i = 0; i < static_cast<Uint32>(DescriptorType::Unknown); i++)
		{
			DescriptorHeapDesc desc{};
			desc.type              = static_cast<DescriptorType>(i);
			desc.descriptor_count  = 1024;
			desc.is_shader_visible = false;
			descriptor_pool[i] = std::make_unique<DescriptorAllocator>(std::make_unique<DescriptorHeap>(this, desc, std::format("Descriptor Heap - {}", DescriptorTypeToString(desc.type)).c_str()));
		}

		DescriptorHeapDesc heap_desc{};
		heap_desc.type              = DescriptorType::CBV_SRV_UAV;
		heap_desc.descriptor_count  = 4096;
		heap_desc.is_shader_visible = true;
		online_descriptor_pool = std::make_unique<OnlineDescriptorAllocator>(std::make_unique<DescriptorHeap>(this, heap_desc, "Online CBV_SRV_UAV Heap"), 1024);

		CreateCommonRootSignature();
	}
	
	GPUContext::~GPUContext()
	{
	
	}

	std::unique_ptr<CommandList> GPUContext::CreateGraphicsCommandList(const Char* name) {
		return CreateCommandListImpl(CommandListType::Graphics, name);
	}

	std::unique_ptr<CommandList> GPUContext::CreateComputeCommandList(const Char* name){
		return CreateCommandListImpl(CommandListType::Compute, name);
	}

	std::unique_ptr<CommandList> GPUContext::CreateCopyCommandList(const Char* name){
		return CreateCommandListImpl(CommandListType::Copy, name);
	}

	std::unique_ptr<PipelineState> GPUContext::CreateGraphicsPipelineState(const GraphicsPipelineStateDesc& desc, const Char* name) {
		return std::make_unique<PipelineState>(this, desc, name);
	}

	std::unique_ptr<PipelineState> GPUContext::CreateMeshShaderPipelineState(const MeshPipelineStateDesc& desc, const Char* name)
	{
		return std::make_unique<PipelineState>(this, desc, name);
	}

	std::unique_ptr<PipelineState> GPUContext::CreateComputePipelineState(const ComputePipelineStateDesc& desc, const Char* name)
	{
		return std::make_unique<PipelineState>(this, desc, name);
	}

	std::unique_ptr<Texture> GPUContext::CreateTexture(const TextureDesc& desc, ComPtr<ID3D12Resource> d3d12_texture, const Char* name)
	{
		return std::make_unique<Texture>(this, desc, d3d12_texture, name);
	}

	std::unique_ptr<Texture> GPUContext::CreateTexture(const TextureDesc& desc, const Char* name)
	{
		return std::make_unique<Texture>(this, desc, name);
	}

	std::unique_ptr<Buffer> GPUContext::CreateBuffer(const BufferDesc& desc, const Char* name) {
		return std::make_unique<Buffer>(this, desc, name);
	}


	Descriptor GPUContext::CreateTextureRTV(Texture* backbuffer, const TextureViewDesc& view_desc)
	{
		return CreateTextureViewImpl(backbuffer, DescriptorType::RTV, view_desc, true, false);
	}

	Descriptor GPUContext::CreateTextureDSV(Texture* depthmap, const TextureViewDesc& view_desc)
	{
		return CreateTextureViewImpl(depthmap, DescriptorType::DSV, view_desc, true, false);
	}

	Descriptor GPUContext::CraeteTextureSRV(Texture* texture, const TextureViewDesc& view_desc, Bool force_cpu_descriptor, Bool is_persistent)
	{
		return CreateTextureViewImpl(texture, DescriptorType::CBV_SRV_UAV, view_desc, force_cpu_descriptor, is_persistent);
	}

	Descriptor GPUContext::CreateTextureUAV(Texture* texture, const TextureViewDesc& view_desc, Bool force_cpu_descriptor, Bool is_persistent)
	{
		return CreateTextureReadWriteViewImpl(texture, DescriptorType::CBV_SRV_UAV, view_desc, force_cpu_descriptor, is_persistent);
	}

	Descriptor GPUContext::CreateConstantBufferView(Buffer* buffer, const BufferViewDesc& view_desc, Bool force_cpu_descriptor, Bool is_persistent)
	{
		return CreateBufferReadOnlyViewImpl(buffer, DescriptorType::CBV_SRV_UAV, view_desc, force_cpu_descriptor, is_persistent);
	}

	Descriptor GPUContext::CreateBufferSRV(Buffer* buffer, const BufferViewDesc& view_desc, Bool force_cpu_descriptor, Bool is_persistent)
	{
		return CreateBufferReadOnlyViewImpl(buffer, DescriptorType::CBV_SRV_UAV, view_desc, force_cpu_descriptor, is_persistent);
	}

	Descriptor GPUContext::CreateBufferUAV(Buffer* buffer, const BufferViewDesc& view_desc, Bool force_cpu_descriptor, Bool is_persistent)
	{
		return CreateBufferReadWriteViewImpl(buffer, DescriptorType::CBV_SRV_UAV, view_desc, force_cpu_descriptor, is_persistent);
	}

	std::unique_ptr<Fence> GPUContext::CreateFence(const Char* name)
	{
		return std::make_unique<Fence>(this, name);
	}

	void GPUContext::FreeCPUDescriptor(Descriptor descriptor)
	{
		if (!descriptor.IsValid())
			return;

		DescriptorHeap* parent_heap = descriptor.parent_heap;
		if (parent_heap->IsShaderVisible())
			return;

		FreeCPUDescriptorImpl(parent_heap->GetDescriptorType(), descriptor);
	}

	Uint32 GPUContext::GetBindlesDescriptorIndex(Descriptor descriptor)
	{
		CESAR_ASSERT(descriptor.IsValid() && "Cannot get bindless index of invalid descriptor.");
		CESAR_ASSERT(descriptor.parent_heap->IsShaderVisible() && "Cannot get bindless index of non shader visible descriptor.");
		return descriptor.index;
	}

	//Misc
	void GPUContext::SignalGraphicsCommandQueue(Fence* fence, Uint32 fence_value) {
		graphics_command_queue->Signal(fence, fence_value);
	}

	void GPUContext::SignalComputeCommandQueue(Fence* fence, Uint32 fence_value) {
		compute_command_queue->Signal(fence, fence_value);
	}

	void GPUContext::SignalCopyCommandQueue(Fence* fence, Uint32 fence_value) {
		copy_command_queue->Signal(fence, fence_value);
	}

	ID3D12Device6* GPUContext::GetDevice()
	{
		return device->GetDevice();
	}

	IDXGIFactory4* GPUContext::GetFactory()
	{
		return device->GetFactory();
	}

	const GPUDeviceCapabilities& GPUContext::GetGPUCapabilities()
	{
		return device->GetGPUDeviceCapabilities();
	}

	CommandQueue* GPUContext::GetGraphicsCommandQueue() const
	{
		return graphics_command_queue.get();
	}

	CommandQueue* GPUContext::GetComputeCommandQueue() const
	{
		return compute_command_queue.get();
	}

	CommandQueue* GPUContext::GetCopyCommandQueue() const
	{
		return copy_command_queue.get();
	}

	D3D12MA::Allocator* GPUContext::GetAllocator() const
	{
		return allocator;
	}

	OnlineDescriptorAllocator* GPUContext::GetOnlineDescriptorAllocator() { return online_descriptor_pool.get(); }
	DescriptorHeap* GPUContext::GetGPUDescriptorHeap() const { return online_descriptor_pool->GetHeap(); }
	ID3D12RootSignature* GPUContext::GetGlobalRootSignature() const { return global_root_signature.Get(); }

	void GPUContext::CreateCommonRootSignature()
	{
		ID3D12Device6* device = this->device->GetDevice();

		D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data{};
		feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(feature_data))))
		{
			feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_ROOT_PARAMETER1 root_parameter[2]{};
		root_parameter[0].InitAsConstantBufferView(0); //Frame_Constants
		root_parameter[1].InitAsConstants(12, 1); //Pass Data Slot 1

		CD3DX12_STATIC_SAMPLER_DESC static_sampler_desc[1];
		static_sampler_desc[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP); //Linear Clamp Sampler : register(0)

		D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags{};
		root_signature_flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc{};
		root_signature_desc.Init_1_1(ARRAYSIZE(root_parameter), root_parameter, ARRAYSIZE(static_sampler_desc), static_sampler_desc, root_signature_flags);

		ID3D10Blob* signature;
		ID3D10Blob* error;
		CESAR_D3D12_CHECK(D3D12SerializeVersionedRootSignature(&root_signature_desc, &signature, &error));
		device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&global_root_signature));

	}

	std::unique_ptr<CommandList> GPUContext::CreateCommandListImpl(CommandListType type, const Char* name)
	{
		auto cmd_list = std::make_unique<CommandList>(this, type, name);

		switch (type) 
		{
			case CommandListType::Graphics:
				cmd_list->SetCommandQueue(graphics_command_queue.get());
				break;
			case CommandListType::Compute:
				cmd_list->SetCommandQueue(compute_command_queue.get());
				break;
			case CommandListType::Copy:
				cmd_list->SetCommandQueue(copy_command_queue.get());
				break;
			default: 
				CESAR_DEBUGBREAK();
		}

		return cmd_list;
	}


	Descriptor GPUContext::CreateTextureViewImpl(Texture* texture, DescriptorType view_type, const TextureViewDesc& view_desc, Bool force_cpu_descriptors, Bool is_persistent)
	{
		ID3D12Device6* device = this->device->GetDevice();

		const TextureDesc& texture_desc = texture->GetDesc();

		switch (view_type)
		{
		case DescriptorType::RTV: {

			D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};
			switch (texture_desc.format) {
			case ResourceFormat::RGBA8_UNORM:
			{
				rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				break;
			}
			}

			switch (texture_desc.type)
			{
			case TextureType::Texture2D:
			{
				if (view_desc.array_size > 1)
				{
					rtv_desc.Texture2DArray.ArraySize = 1;
					rtv_desc.Texture2DArray.FirstArraySlice = view_desc.slice;
					rtv_desc.Texture2DArray.MipSlice = view_desc.mip;
					rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				}
				else
				{
					rtv_desc.Texture2D.MipSlice = 0;
					rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				}
				break;
			}
			}

			Descriptor descriptor = AllocateCPUDescriptorImpl(view_type);
			device->CreateRenderTargetView(texture->GetTexture(), &rtv_desc, GetCPUHandle(descriptor));
			return descriptor;
		}

		case DescriptorType::DSV: {
			D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
			switch (texture_desc.format) {
			case ResourceFormat::D32_FLOAT: {
				dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
				break;
			}
			}

			switch (texture_desc.type) {
			case TextureType::Texture2D:
				if (view_desc.array_size > 1) {
					dsv_desc.Texture2DArray.ArraySize = 1;
					dsv_desc.Texture2DArray.FirstArraySlice = view_desc.slice;
					dsv_desc.Texture2DArray.MipSlice = view_desc.mip;
					dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				}
				else {
					dsv_desc.Texture2D.MipSlice = 0;
					dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				}
				break;
			}

			Descriptor descriptor = AllocateCPUDescriptorImpl(view_type);
			device->CreateDepthStencilView(texture->GetTexture(), &dsv_desc, GetCPUHandle(descriptor));
			return descriptor;
		}
		case DescriptorType::CBV_SRV_UAV: {
			D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
			srv_desc.Format = ToDXGIFormat(texture_desc.format);
			switch (texture_desc.type)
			{
			case TextureType::Texture2D:
			{
				if (view_desc.array_size > 1)
				{
					srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
					srv_desc.Texture2DArray.ArraySize = view_desc.array_size;
					srv_desc.Texture2DArray.FirstArraySlice = view_desc.slice;
					srv_desc.Texture2DArray.MipLevels = view_desc.mip_levels;
				}
				else
				{
					srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					srv_desc.Texture2D.MipLevels = view_desc.mip_levels;
					srv_desc.Texture2D.MostDetailedMip = view_desc.mip;
				}
			}
			}
			srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			Descriptor descriptor;
			if (force_cpu_descriptors)
				descriptor = AllocateCPUDescriptorImpl(view_type);
			else
				descriptor = AllocateGPUDescriptorImpl(is_persistent);

			device->CreateShaderResourceView(texture->GetTexture(), &srv_desc, GetCPUHandle(descriptor));
			return descriptor;
		}
		}
	}

	Descriptor GPUContext::CreateTextureReadWriteViewImpl(Texture* texture, DescriptorType view_type, const TextureViewDesc& view_desc, Bool force_cpu_descriptor, Bool is_persistent)
	{
		CESAR_ASSERT(view_type == DescriptorType::CBV_SRV_UAV && "Only UAV views are supported for read/write texture views.");
		ID3D12Device6* device = GetDevice();
		Descriptor descriptor;
		if (force_cpu_descriptor)
			descriptor = AllocateCPUDescriptorImpl(view_type);
		else
			descriptor = AllocateGPUDescriptorImpl(is_persistent);

		const TextureDesc& texture_desc = texture->GetDesc();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc{};
		uav_desc.Format = ToDXGIFormat(texture_desc.format);
		switch (texture_desc.type) {
		case TextureType::Texture2D:
			if (view_desc.array_size > 1) {
				uav_desc.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				uav_desc.Texture2DArray.ArraySize       = view_desc.array_size;
				uav_desc.Texture2DArray.FirstArraySlice = view_desc.slice;
				uav_desc.Texture2DArray.MipSlice        = view_desc.mip;
			}
			else {
				uav_desc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
				uav_desc.Texture2D.MipSlice = view_desc.mip;
			}
			break;
		}

		device->CreateUnorderedAccessView(texture->GetTexture(), nullptr, &uav_desc, GetCPUHandle(descriptor));

		return descriptor;
	}

	Descriptor GPUContext::CreateBufferReadOnlyViewImpl(Buffer* buffer, DescriptorType view_type, const BufferViewDesc& view_desc, Bool force_cpu_descriptor, Bool is_persistent)
	{
		ID3D12Device6* device = GetDevice();
		Descriptor descriptor;
		if (force_cpu_descriptor)
			descriptor = AllocateCPUDescriptorImpl(view_type);
		else
			descriptor = AllocateGPUDescriptorImpl(is_persistent);

		BufferDesc buffer_desc = buffer->GetDesc();

		const Uint32 element_count = (view_desc.size) ? ((view_desc.size - view_desc.offset) / buffer_desc.stride) : ((buffer_desc.size - view_desc.offset) / buffer_desc.stride);

		if (HasFlag(buffer_desc.misc_flag, BufferMiscFlag::ConstantBuffer))
		{
			descriptor = CreateCBVViewImpl(buffer);
		}
		else if (HasFlag(buffer_desc.misc_flag, BufferMiscFlag::BufferStructured)) {
			D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
			srv_desc.Format = ToDXGIFormat(buffer_desc.format);
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srv_desc.Buffer.NumElements = element_count;
			srv_desc.Buffer.FirstElement = view_desc.offset;
			srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srv_desc.Buffer.StructureByteStride = buffer_desc.stride;

			device->CreateShaderResourceView(buffer->GetBuffer(), &srv_desc, GetCPUHandle(descriptor));
		}
		else if (HasFlag(buffer_desc.misc_flag, BufferMiscFlag::IndirectArgs)) {
			D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
			srv_desc.Format = ToDXGIFormat(buffer_desc.format);
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srv_desc.Buffer.NumElements = element_count;
			srv_desc.Buffer.FirstElement = view_desc.offset;
			srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srv_desc.Buffer.StructureByteStride = buffer_desc.stride;

			device->CreateShaderResourceView(buffer->GetBuffer(), &srv_desc, GetCPUHandle(descriptor));
		}
		else if (HasFlag(buffer_desc.misc_flag, BufferMiscFlag::BufferRaw)) {
			D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
			srv_desc.Format = ToDXGIFormat(buffer_desc.format);
			srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			srv_desc.Buffer.NumElements = element_count;
			srv_desc.Buffer.FirstElement = view_desc.offset;
			srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

			device->CreateShaderResourceView(buffer->GetBuffer(), &srv_desc, GetCPUHandle(descriptor));
		}
		else {
			CESAR_FEATURE_NO_IMPL("Other View creations have not been implemented yet.");
			return Descriptor{};
		}

		return descriptor;
	}

	Descriptor GPUContext::CreateBufferReadWriteViewImpl(Buffer* buffer, DescriptorType view_type, const BufferViewDesc& view_desc, Bool force_cpu_descriptor, Bool is_persistent)
	{
		ID3D12Device6* device = GetDevice();
		Descriptor descriptor;
		if (force_cpu_descriptor)
			descriptor = AllocateCPUDescriptorImpl(view_type);
		else
			descriptor = AllocateGPUDescriptorImpl(is_persistent);
		if (!descriptor.IsValid())
			return Descriptor{};
		BufferDesc buffer_desc = buffer->GetDesc();

		const Uint32 element_count = (view_desc.size) ? ((view_desc.size - view_desc.offset) / buffer_desc.stride) : ((buffer_desc.size - view_desc.offset) / buffer_desc.stride);

		if (HasFlag(buffer_desc.misc_flag, BufferMiscFlag::BufferRaw)) {
			D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc{};
			uav_desc.Buffer.FirstElement = view_desc.offset;
			uav_desc.Buffer.NumElements = element_count;
			uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
			uav_desc.Format = DXGI_FORMAT_R32_TYPELESS;
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

			device->CreateUnorderedAccessView(buffer->GetBuffer(), nullptr, &uav_desc, GetCPUHandle(descriptor));
		}
		else if (HasFlag(buffer_desc.misc_flag, BufferMiscFlag::BufferStructured)) {
			D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc{};
			uav_desc.Buffer.FirstElement = view_desc.offset;
			uav_desc.Buffer.NumElements = element_count;
			uav_desc.Buffer.StructureByteStride = buffer_desc.stride;
			uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
			uav_desc.Format = DXGI_FORMAT_UNKNOWN;
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

			device->CreateUnorderedAccessView(buffer->GetBuffer(), nullptr, &uav_desc, GetCPUHandle(descriptor));
		}
		else if (HasFlag(buffer_desc.misc_flag, BufferMiscFlag::IndirectArgs)) {
			D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc{};
			uav_desc.Buffer.FirstElement = view_desc.offset;
			uav_desc.Buffer.NumElements = element_count;
			uav_desc.Buffer.StructureByteStride = buffer_desc.stride;
			uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
			uav_desc.Format = DXGI_FORMAT_UNKNOWN;
			uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

			device->CreateUnorderedAccessView(buffer->GetBuffer(), nullptr, &uav_desc, GetCPUHandle(descriptor));
		}
		else {
			CESAR_FEATURE_NO_IMPL("There are no implementations for other view creation.");
		}
		return descriptor;
	}


	Descriptor GPUContext::CreateCBVViewImpl(Buffer*& buffer)
	{
		Descriptor descriptor;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{  };
		cbv_desc.BufferLocation = buffer->GetGPUAddress();
		cbv_desc.SizeInBytes = buffer->GetSize();

		device->GetDevice()->CreateConstantBufferView(&cbv_desc, GetCPUHandle(descriptor));
		return descriptor;
	}

	void GPUContext::FreeCPUDescriptorImpl(DescriptorType type, Descriptor descriptor)
	{
		descriptor_pool[static_cast<Uint32>(type)]->FreeDescriptor(descriptor);
	}

	Descriptor GPUContext::AllocateGPUDescriptorImpl(Bool is_persistent)
	{
		static Uint32 persistent_index = 0;
		if (is_persistent) {
			CESAR_ASSERT(persistent_index <= online_descriptor_pool->GetReserved() && "Persistent Pool has reached it's limit");
			return online_descriptor_pool->GetHeap()->GetDescriptor(persistent_index++);
		}
		else
			return online_descriptor_pool->Allocate();
	}

	Descriptor GPUContext::AllocateCPUDescriptorImpl(DescriptorType type)
	{
		return descriptor_pool[static_cast<Uint32>(type)]->Allocate();
	}
}
