// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "Buffer.h"
#include "../GPUContext.h"


namespace cesar
{
	constexpr D3D12_HEAP_TYPE ToD3D12HeapType(ResourceUsage usage) {
		switch (usage) {
		case ResourceUsage::Default:return D3D12_HEAP_TYPE_DEFAULT;
		case ResourceUsage::Readback:return D3D12_HEAP_TYPE_READBACK;
		case ResourceUsage::Upload:return D3D12_HEAP_TYPE_UPLOAD;
		default:CESAR_DEBUGBREAK();
		}
	}

	Buffer::Buffer(GPUContext* gpu_context, const BufferDesc& desc, const cesar::Char* name)
		:desc(desc), gpu_context(gpu_context)
	{
		D3D12MA::ALLOCATION_DESC allocation_desc{};
		allocation_desc.HeapType = ToD3D12HeapType(desc.usage);

		D3D12_RESOURCE_DESC buffer_desc{};
		buffer_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		buffer_desc.Format = DXGI_FORMAT_UNKNOWN;
		buffer_desc.MipLevels = 1;
		buffer_desc.Alignment = 0;
		buffer_desc.DepthOrArraySize = 1;
		buffer_desc.Flags;
		buffer_desc.Height = 1;
		buffer_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		buffer_desc.Width = desc.size;
		buffer_desc.SampleDesc = { .Count = 1, .Quality = 0 };

		if (HasFlag(desc.bind_flag, ResourceBindFlag::UnorderedAccess)) {
			buffer_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		if (!HasFlag(desc.bind_flag, ResourceBindFlag::ShaderResource)) {
			buffer_desc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		}

		D3D12MA::Allocator* allocator = gpu_context->GetAllocator();
		HRESULT hr = allocator->CreateResource(&allocation_desc, &buffer_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, &allocation, IID_PPV_ARGS(&resource));
		CESAR_D3D12_CHECK(hr);
		CESAR_NAME_D3D12_OBJECT(resource, name);
	}

	Buffer::~Buffer()
	{
		resource.Reset();
		allocation->Release();
	}

	void Buffer::Resize(Uint64 resize) {
		if (desc.size == resize) {
			LOG_WARN("Buffer failed to resize. Cannot resize buffer to existing size.");
			return;
		}

		D3D12MA::ALLOCATION_DESC allocation_desc{};
		allocation_desc.HeapType = ToD3D12HeapType(desc.usage);

		D3D12_RESOURCE_DESC buffer_desc = resource->GetDesc();
		buffer_desc.Height = 1;
		buffer_desc.Width  = resize;
		desc.size = resize;

		D3D12MA::Allocator* allocator = gpu_context->GetAllocator();
		HRESULT hr = allocator->CreateResource(&allocation_desc, &buffer_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, &allocation, IID_PPV_ARGS(&resource));
		CESAR_D3D12_CHECK(hr);
	}
	
	void Buffer::SetName(const cesar::Char* name)
	{
		CESAR_NAME_D3D12_OBJECT(resource, name);
	}
}
