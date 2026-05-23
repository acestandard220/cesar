// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../cesar_core/cesar_core.h"
#include "Descriptor.h"

#include <d3d12.h>

namespace cesar
{
	enum class DescriptorType
	{
		CBV_SRV_UAV,
		RTV,
		DSV,
		Sampler,

		Unknown
	};
	
	struct DescriptorHeapDesc
	{
		DescriptorType type;
		cesar::Uint32 descriptor_count;
		Bool is_shader_visible;
	};

	class GPUContext;
	class _declspec(dllexport) DescriptorHeap
	{
	public:
		DescriptorHeap(GPUContext* gpu_context, const DescriptorHeapDesc& desc, const cesar::Char* name= "DescriptorHeap");
		~DescriptorHeap();

		const DescriptorType& GetDescriptorType()const { return type; }
		Bool IsShaderVisible()const { return shader_visible; }

		cesar::Uint32 GetCapacity()const { return descriptor_count; }
		cesar::Uint32 GetHandleSize()const { return handle_size; }

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(cesar::Uint32 index) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(cesar::Uint32 index) const;

		//For ImGui purposes.
		Uint32 GetDescriptorIndex(D3D12_CPU_DESCRIPTOR_HANDLE handle);

		Descriptor GetDescriptor(cesar::Uint32 index) const;

		ID3D12DescriptorHeap* GetDescriptorHeap()const { return heap.Get(); }
	private:
		ComPtr<ID3D12DescriptorHeap> heap;
		
		D3D12_CPU_DESCRIPTOR_HANDLE cpu_start;
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_start;

		DescriptorType type;
		cesar::Uint32 descriptor_count;
		cesar::Uint32 handle_size;
		cesar::Bool shader_visible;
	};

	_declspec(dllexport) D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(Descriptor descriptor);
	_declspec(dllexport) D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(Descriptor descriptor);
	
	static const cesar::Char* DescriptorTypeToString(DescriptorType type) {
		switch (type) {
		    case DescriptorType::CBV_SRV_UAV: return "CBV_SRV_UAV";
		    case DescriptorType::RTV: return "RTV";
		    case DescriptorType::DSV: return "DSV";
		    case DescriptorType::Sampler: return "Sampler";
		}
	}

}