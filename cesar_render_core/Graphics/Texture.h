
// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.

#pragma once
#include "../cesar_core/cesar_core.h"
#include "Resource.h"
#include "ResourceFormat.h"

#include <D3D12MA/D3D12MemAlloc.h>

#include <d3d12.h>

namespace cesar
{

	enum class TextureType
	{
		Texture1D,
		Texture2D,
		Texture3D
	};
	CESAR_ENABLE_ENUM_EQUAL_COMP_OP(TextureType);

	struct __declspec(dllexport) TextureDesc
	{
		TextureType type = TextureType::Texture2D;
		cesar::Uint32 width = 0;
		cesar::Uint32 height = 0;

		cesar::Uint32 mips = 1;
		Uint32 array_size = 1;
		ResourceFormat format;
		ResourceUsage usage = ResourceUsage::Default;
		ResourceBindFlag bind_flag = ResourceBindFlag::None;
		TextureMiscFlag misc_flag = TextureMiscFlag::SRGB;
		ClearValue clear_value{};
		ResourceState intial_state;

		Bool IsCompatible(const TextureDesc& desc) const{
			return type   == desc.type
				&& width  == desc.width
				&& height == desc.height
				&& mips   == desc.mips
				&& format == desc.format
				&& usage  == desc.usage

				&& bind_flag == desc.bind_flag
				&& misc_flag == desc.misc_flag;
		}

		Bool operator==(const TextureDesc& other) const = default;
	};

	inline TextureDesc RenderTargetDesc(Uint32 w, Uint32 h, ResourceBindFlag bind_flag = ResourceBindFlag::None,
		ClearValue clear_value = {0.0f,0.0f,0.0f,1.0f}, ResourceFormat format = ResourceFormat::RGBA8_UNORM) {
		TextureDesc render_target_desc{ };
		render_target_desc.bind_flag = ResourceBindFlag::RenderTarget | bind_flag;
		render_target_desc.format = format;
		render_target_desc.width = w;
		render_target_desc.height = h;
		render_target_desc.intial_state = ResourceState::Present;
		render_target_desc.clear_value = clear_value;
		
		return render_target_desc;
	}

	inline TextureDesc DepthStencilTargetDesc(Uint32 w, Uint32 h, ResourceBindFlag bind_flag = ResourceBindFlag::None,
		ClearValue clear_value = ClearValue(1.0f, 1), ResourceFormat format = ResourceFormat::D32_FLOAT) {

		TextureDesc depth_stencil_target_desc{};
		depth_stencil_target_desc.bind_flag = ResourceBindFlag::DepthTarget | bind_flag;
		depth_stencil_target_desc.format = format;
		depth_stencil_target_desc.width = w;
		depth_stencil_target_desc.height = h;
		depth_stencil_target_desc.intial_state = ResourceState::DSV;
		depth_stencil_target_desc.clear_value = clear_value;

		return depth_stencil_target_desc;
	}
	
	/// <summary>
	/// mip_levels: number of mip maps per array slice
	/// mip: which mip to create the view for
	/// 
	/// array size: number of members/slices in the texture array
	/// slice: slice to create view for
	/// 
	/// Default Ctor assumes first mip and slice.
	/// </summary>
	struct __declspec(dllexport) TextureViewDesc
	{
		cesar::Uint32 mip_levels = cesar::Uint32(-1);
		cesar::Uint32 mip = 0;

		cesar::Uint32 array_size = cesar::Uint32(-1);
		cesar::Uint32 slice = 0;

		Bool operator==(const TextureViewDesc& other)const = default;
	};

    #define CESAR_DEFAULT_TEXTURE_VIEW_DESC TextureViewDesc{}


	class GPUContext;
	class _declspec(dllexport) Texture
	{
	public:
		Texture(GPUContext* gpu_context, const TextureDesc& desc, const cesar::Char* name);
		Texture(GPUContext* gpu_context, const TextureDesc& desc, ComPtr<ID3D12Resource> backbuffer, const cesar::Char* name);
		~Texture();

		void SetName(const cesar::Char* name);

		ID3D12Resource* GetTexture() { return resource.Get(); }
		const TextureDesc& GetDesc()const { return desc; }

		void Resize(Uint32 width, Uint32 height);

	private:
		GPUContext* gpu_context;
		ComPtr<ID3D12Resource> resource;
		D3D12MA::Allocation* allocation;
		TextureDesc desc;
	};

}
