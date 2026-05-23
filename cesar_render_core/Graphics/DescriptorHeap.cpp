// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "DescriptorHeap.h"
#include "../GPUContext.h"

namespace cesar
{
	using namespace cesar;
	inline D3D12_DESCRIPTOR_HEAP_TYPE ToD3DDescriptorHeapType(DescriptorType type)
	{
		switch (type) {
		    case DescriptorType::CBV_SRV_UAV:return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		    case DescriptorType::RTV:return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		    case DescriptorType::DSV:return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		    case DescriptorType::Sampler:return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		    default: CESAR_DEBUGBREAK();
		}
	}

	constexpr Bool CanBeShaderVisible(DescriptorType type) {
		switch (type) {
		    case DescriptorType::Sampler: [[fallthrough]];
		    case DescriptorType::CBV_SRV_UAV:return true;
		    default: return false;
		}
	}

	DescriptorHeap::DescriptorHeap(GPUContext* gpu_context, const DescriptorHeapDesc& desc, const Char* name)
		:descriptor_count(desc.descriptor_count), type(desc.type), shader_visible(desc.is_shader_visible)
	{
		if (shader_visible)
		{
			CESAR_ASSERT(CanBeShaderVisible(type) && "Descriptor Type is incompatible for shader visible heap.");
		}

		D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
		heap_desc.Flags = shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		heap_desc.NumDescriptors = desc.descriptor_count;
		heap_desc.Type = ToD3DDescriptorHeapType(desc.type);

		ID3D12Device6* device = gpu_context->GetDevice();
		HRESULT hr = device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&heap));
		if (FAILED(hr))
		{
			LOG_FATAL("Failed to create descriptor heap");
			return;;
		}
		handle_size = device->GetDescriptorHandleIncrementSize(heap_desc.Type);
		
		cpu_start = heap->GetCPUDescriptorHandleForHeapStart();
		if (shader_visible)
		{
			gpu_start = heap->GetGPUDescriptorHandleForHeapStart();
		}

		CESAR_NAME_D3D12_OBJECT(heap, name);
	}

	DescriptorHeap::~DescriptorHeap()
	{
		heap.Reset();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(cesar::Uint32 index) const
	{
		CESAR_ASSERT(index < descriptor_count);
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = cpu_start.ptr + (static_cast<SIZE_T>(index) * handle_size);
		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandle(cesar::Uint32 index) const
	{
		CESAR_ASSERT(index < descriptor_count);
		CESAR_ASSERT(shader_visible);

		D3D12_GPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = gpu_start.ptr + (static_cast<SIZE_T>(index) * handle_size);
		return handle;
	}

	Uint32 DescriptorHeap::GetDescriptorIndex(D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		return handle.ptr - cpu_start.ptr;
	}

	Descriptor DescriptorHeap::GetDescriptor(cesar::Uint32 index) const
	{
		CESAR_ASSERT(index < descriptor_count);
		
		Descriptor descriptor{};
		descriptor.parent_heap = const_cast<DescriptorHeap*>(this);
		descriptor.index = index;
		return descriptor;
	}



	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(Descriptor descriptor)
	{
		return descriptor.parent_heap->GetCPUHandle(descriptor.index);
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(Descriptor descriptor)
	{
		return descriptor.parent_heap->GetGPUHandle(descriptor.index);
	}
}