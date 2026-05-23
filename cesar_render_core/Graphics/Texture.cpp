// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "Texture.h"
#include "../GPUContext.h"

using namespace cesar;
namespace cesar
{
	D3D12_RESOURCE_DIMENSION ToD3D12ResourceDimension(TextureType type)
	{
		switch (type) {
		case TextureType::Texture1D:return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case TextureType::Texture2D:return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case TextureType::Texture3D:return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		default:return D3D12_RESOURCE_DIMENSION_UNKNOWN;
		}
	}
	Texture::Texture(GPUContext* gpu_context, const TextureDesc& desc, const Char* name)
		:desc(desc), gpu_context(gpu_context)
	{
		CESAR_ASSERT((desc.width && desc.height) && "Cannot create texture of width and height 0");
		D3D12MA::ALLOCATION_DESC allocation_desc{};
		allocation_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		switch (desc.type) {
		case TextureType::Texture2D: {
			D3D12_RESOURCE_DESC texture_desc = CD3DX12_RESOURCE_DESC::Tex2D(ToDXGIFormat(desc.format), desc.width, desc.height, desc.array_size, desc.mips, 1, 0);
			D3D12_CLEAR_VALUE clear_value{};
			D3D12_CLEAR_VALUE* clear_value_ptr = nullptr;

			if (HasFlag(desc.bind_flag, ResourceBindFlag::DepthTarget)) {
				texture_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
				clear_value.Format = ToDXGIFormat(desc.format);
				clear_value.DepthStencil.Depth = desc.clear_value.depth_stencil.depth;
				clear_value.DepthStencil.Stencil = desc.clear_value.depth_stencil.stencil;
				
				clear_value_ptr = &clear_value;
			} else if (HasFlag(desc.bind_flag, ResourceBindFlag::RenderTarget)) {
				texture_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
				clear_value.Format = ToDXGIFormat(desc.format);
				clear_value.Color[0] = desc.clear_value.color.color[0];
				clear_value.Color[1] = desc.clear_value.color.color[1];
				clear_value.Color[2] = desc.clear_value.color.color[2];
				clear_value.Color[3] = desc.clear_value.color.color[3];

				clear_value_ptr = &clear_value;
			}

			if (HasFlag(desc.bind_flag, ResourceBindFlag::UnorderedAccess)) {
				texture_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			}

			HRESULT hr;
			D3D12MA::Allocator* allocator = gpu_context->GetAllocator();
			if(!gpu_context->GetGPUCapabilities().SupportEnhanceBarriers())
			{
				hr = allocator->CreateResource(&allocation_desc, &texture_desc, ToD3D12State(desc.intial_state), clear_value_ptr, &allocation, IID_PPV_ARGS(&resource));
			}
			else {
				D3D12_RESOURCE_DESC1 texture_desc1 = CD3DX12_RESOURCE_DESC1(texture_desc);
				hr = allocator->CreateResource3(&allocation_desc, &texture_desc1, ToD3D12BarrierLayout(desc.intial_state), clear_value_ptr, 0, nullptr, &allocation, IID_PPV_ARGS(&resource));
			}
			CESAR_D3D12_CHECK(hr);
			break;
		}
		default:
			CESAR_FEATURE_NO_IMPL("Other texture types have not been implemented");
			return;
		}
		 
	}

	Texture::Texture(GPUContext* gpu_context, const TextureDesc& desc, ComPtr<ID3D12Resource> backbuffer, const Char* name)
		:desc(desc)
	{
		resource.Swap(backbuffer);
	}
	Texture::~Texture()
	{
		resource.Reset();
		if(allocation)
		{
			allocation->Release();
		}
	}
	void Texture::SetName(const Char* name)
	{
		CESAR_NAME_D3D12_OBJECT(resource, name);
	}

	void Texture::Resize(Uint32 width, Uint32 height)
	{
		D3D12_RESOURCE_DESC texture_desc = resource->GetDesc();
		if (texture_desc.Width == width && texture_desc.Height == height) {
			return;
		}

		resource.Reset();
		if (allocation) {
			allocation->Release();
		}

		D3D12MA::ALLOCATION_DESC allocation_desc{};
		allocation_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_CLEAR_VALUE clear_value{};
		D3D12_CLEAR_VALUE* clear_value_ptr = nullptr;

		if (HasFlag(desc.bind_flag, ResourceBindFlag::DepthTarget)) {
			texture_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			clear_value.Format = ToDXGIFormat(desc.format);
			clear_value.DepthStencil.Depth = desc.clear_value.depth_stencil.depth;
			clear_value.DepthStencil.Stencil = desc.clear_value.depth_stencil.stencil;

			clear_value_ptr = &clear_value;
		}
		else if (HasFlag(desc.bind_flag, ResourceBindFlag::RenderTarget)) {
			texture_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			clear_value.Format = ToDXGIFormat(desc.format);
			clear_value.Color[0] = desc.clear_value.color.color[0];
			clear_value.Color[1] = desc.clear_value.color.color[1];
			clear_value.Color[2] = desc.clear_value.color.color[2];
			clear_value.Color[3] = desc.clear_value.color.color[3];

			clear_value_ptr = &clear_value;
		}

		HRESULT hr;
		D3D12MA::Allocator* allocator = gpu_context->GetAllocator();
		if (!gpu_context->GetGPUCapabilities().SupportEnhanceBarriers())
		{
			hr = allocator->CreateResource(&allocation_desc, &texture_desc, ToD3D12State(this->desc.intial_state), clear_value_ptr, &allocation, IID_PPV_ARGS(&resource));
		}
		else {
			D3D12_RESOURCE_DESC1 texture_desc1 = CD3DX12_RESOURCE_DESC1(texture_desc);
			hr = allocator->CreateResource3(&allocation_desc, &texture_desc1, ToD3D12BarrierLayout(this->desc.intial_state), clear_value_ptr, 0, nullptr, &allocation, IID_PPV_ARGS(&resource));
		}
		CESAR_D3D12_CHECK(hr);

	}
}