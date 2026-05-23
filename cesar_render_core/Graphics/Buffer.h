// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../cesar_core/cesar_core.h"
#include "Resource.h"
#include "ResourceFormat.h"
#include "CommandSignature.h"

#include <D3D12MA/D3D12MemAlloc.h>

#include <d3d12.h>
#include <DX12 Helper/d3dx12.h>

/// <summary>
/// All Buffers unless special are created in common state. [Intial state for all buffer resources is D3D12_RESOURCE_STATE_COMMON]
/// </summary>
namespace cesar
{
	//You are not using the BufferViewDesc ooo
	struct BufferViewDesc
	{
		Uint64 size;
		Uint64 offset;
		Bool operator==(const BufferViewDesc& other) const = default;
	};

	struct BufferDesc
	{
		cesar::Uint64 size;
		cesar::Uint32 stride;

		ResourceUsage usage;
		ResourceBindFlag bind_flag;
		ResourceFormat format;
		BufferMiscFlag misc_flag;
		ResourceState initial_state = ResourceState::Common;

		Bool persistent = false;

		bool operator==(const BufferDesc& desc) const{
			return desc.size      == size
				&& desc.usage     == usage
				&& desc.format    == format
				&& desc.stride    == stride
				&& desc.bind_flag == bind_flag
				&& desc.misc_flag == misc_flag;
		}
	};

	template<typename T>
	inline BufferDesc VertexBufferDesc(cesar::Uint64 vertex_count, cesar::Uint32 stride) {
		BufferDesc vertex_buffer_desc = {
			.size = CESAR_SIZEOF_BUFFER(T, vertex_count),
			.stride = stride,
			.usage = ResourceUsage::Default,
			.bind_flag = ResourceBindFlag::None,
			.misc_flag = BufferMiscFlag::VertexBuffer
		};
		return vertex_buffer_desc;
	}

	template<typename T>
	inline BufferDesc IndexBufferDesc(cesar::Uint64 index_count) {
		BufferDesc index_buffer_desc = {
			.size = CESAR_SIZEOF_BUFFER(T, index_count),
			.stride = CESAR_SIZEOF(T),
			.usage = ResourceUsage::Default,
			.bind_flag = ResourceBindFlag::None,
			.format = (CESAR_SIZEOF(T) == 2) ? ResourceFormat::R16_UINT : ResourceFormat::R32_UINT,
			.misc_flag = BufferMiscFlag::IndexBuffer
		};
		return index_buffer_desc;
	}

	inline BufferDesc ConstantBufferDesc(cesar::Uint64 size) {
		BufferDesc constant_buffer_desc = {
			.size = size,
			.stride = 256,
			.usage = ResourceUsage::Upload,
			.bind_flag = ResourceBindFlag::None,
			.format = ResourceFormat::UNKNOWN,
			.misc_flag = BufferMiscFlag::ConstantBuffer
		};
		return constant_buffer_desc;
	}

	template<typename T>
	inline BufferDesc UploadBufferDesc(cesar::Uint64 element_count) {
		BufferDesc upload_buffer_desc = {
			.size = CESAR_SIZEOF_BUFFER(T, element_count),
			.stride = CESAR_SIZEOF(T),
			.usage = ResourceUsage::Upload,
			.bind_flag = ResourceBindFlag::None,
			.format = ResourceFormat::UNKNOWN,
			.misc_flag = BufferMiscFlag::None
		};
		return upload_buffer_desc;
	}

	template<typename T>
	inline BufferDesc StructuredBufferDesc(Uint32 element_count, ResourceBindFlag bind_flag)
	{
		BufferDesc default_generic_desc{
		.size = CESAR_SIZEOF_BUFFER(T,element_count),
		.stride = CESAR_SIZEOF(T),
		.usage = ResourceUsage::Default,
		.bind_flag = bind_flag,
		.format = ResourceFormat::UNKNOWN,
		.misc_flag = BufferMiscFlag::BufferStructured
		};
		return default_generic_desc;
	}

	inline BufferDesc RawBufferDesc(ResourceBindFlag bind_flag = ResourceBindFlag::None, Uint32 custom_count = 1) {
		return BufferDesc {
			.size = CESAR_SIZEOF(Uint32) * custom_count,
			.stride = CESAR_SIZEOF(Uint32),
			.usage = ResourceUsage::Default,
			.bind_flag = ResourceBindFlag::UnorderedAccess | bind_flag,
			.format = ResourceFormat::R32_TYPELESS,
			.misc_flag = BufferMiscFlag::BufferRaw
		};
	}

	template<typename T>
	inline BufferDesc ReadbackBufferDesec(Uint32 element_count) {
		BufferDesc readback_desc{
			.size = CESAR_SIZEOF_BUFFER(T,element_count),
			.stride = CESAR_SIZEOF(T),
			.usage = ResourceUsage::Readback,
			.bind_flag = ResourceBindFlag::None,
			.format = ResourceFormat::UNKNOWN,
			.misc_flag = BufferMiscFlag::None
		};
		return readback_desc;
	}

	inline BufferDesc IndirectArgumentBufferDesc(CommandArgumentType type) {
		BufferDesc indirect_desc{
			.size      = GetCommandArgumentStride(type),
			.stride    = GetCommandArgumentStride(type),
			.usage     = ResourceUsage::Default,
			.bind_flag = ResourceBindFlag::UnorderedAccess,
			.format = ResourceFormat::UNKNOWN,
			.misc_flag = BufferMiscFlag::IndirectArgs
		};
		return indirect_desc;
	}

	class GPUContext;
	class __declspec(dllexport) Buffer {
	public:
		Buffer(GPUContext* gpu_context, const BufferDesc& desc, const cesar::Char* name = "Unnamed Buffer");
		~Buffer();

		void Resize(Uint64 resize);

		template<typename T>
		void Upload(const std::span<T>& source) {
			T* buffer_ptr = nullptr;

			HRESULT hr = resource->Map(0,0,reinterpret_cast<void**>(&buffer_ptr));
			CESAR_D3D12_CHECK(hr);
			memcpy(buffer_ptr, source.data(), source.size_bytes());
			resource->Unmap(0, 0);
		}

		template<typename T>
		void Read(std::span<T>& destination)
		{
			void* mapped_ptr = nullptr;

			HRESULT hr = resource->Map(0, nullptr, &mapped_ptr);
			CESAR_D3D12_CHECK(hr);

			memcpy(destination.data(), mapped_ptr, destination.size_bytes());

			resource->Unmap(0, nullptr);
		}

		void SetName(const cesar::Char* name);

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress()const { return resource->GetGPUVirtualAddress(); }

		const BufferDesc& GetDesc()const { return desc; }
		ResourceFormat GetFormat()const { return desc.format; }
		cesar::Uint64 GetSize()const { return desc.size; }
		cesar::Uint32 GetStride()const { return desc.stride; }
		
		ID3D12Resource* GetBuffer()const { return resource.Get(); }

	private:
		GPUContext* gpu_context;
		D3D12MA::Allocation* allocation;
		ComPtr<ID3D12Resource> resource;

		BufferDesc desc;
	};

}