// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../../cesar_core/cesar_core.h"
#include "ResourceFormat.h"

#include <d3d12.h>

namespace cesar
{
	enum class ResourceUsage
	{
		Default,
		Upload,
		Readback
	};

	enum class ResourceBindFlag : cesar::Uint32
	{
		None = 0,
		ShaderResource = BIT(1),
		RenderTarget = BIT(2),
		DepthTarget = BIT(3),
		UnorderedAccess = BIT(4)
	};
	CESAR_ENABLE_ENUM_OPS(ResourceBindFlag);


	enum class TextureMiscFlag : cesar::Uint32
	{
		None = 0,
		SRGB = BIT(1),
		TextureCube = BIT(2)
	};
	CESAR_ENABLE_ENUM_OPS(TextureMiscFlag);

	enum class BufferMiscFlag : Uint32
	{
		None = 0,
		IndirectArgs = BIT(0),
		BufferRaw = BIT(1),
		BufferStructured = BIT(2),
		ConstantBuffer = BIT(3),
		VertexBuffer = BIT(4),
		IndexBuffer = BIT(5),
	};
	CESAR_ENABLE_ENUM_OPS(BufferMiscFlag);

	enum class ResourceState : cesar::Uint64
	{
		None = 0,
		Common = BIT(0),
		Present = BIT(1),
		RTV = BIT(2),
		DSV = BIT(3),
		DSV_ReadOnly = BIT(4),
		VertexSRV = BIT(5),
		PixelSRV = BIT(6),
		ComputeSRV = BIT(7),
		VertexUAV = BIT(8),
		PixelUAV = BIT(9),
		ComputeUAV = BIT(10),
		ClearUAV = BIT(11),
		CopyDst = BIT(12),
		CopySrc = BIT(13),
		ShadingRate = BIT(14),
		VertexBuffer = BIT(15),
		IndexBuffer = BIT(16),
		IndirectArgs = BIT(17),

		AllVertex = VertexSRV | VertexUAV,
		AllPixel = PixelSRV | PixelUAV,
		AllCompute = ComputeSRV | ComputeUAV,
		AllSRV = VertexSRV | PixelSRV | ComputeSRV,
		AllUAV = VertexUAV | PixelUAV | ComputeUAV,
		AllDSV = DSV | DSV_ReadOnly,
		AllCopy = CopyDst | CopySrc,
		GenericRead = CopySrc | AllSRV,
		GenericWrite = CopyDst | AllUAV,
		AllShading = AllSRV | AllUAV | ShadingRate
	};
	CESAR_ENABLE_ENUM_OPS(ResourceState);

	constexpr D3D12_RESOURCE_STATES ToD3D12State(ResourceState state) {
		if (state == ResourceState::Common)
			return D3D12_RESOURCE_STATE_COMMON;

		D3D12_RESOURCE_STATES res = static_cast<D3D12_RESOURCE_STATES>(0);

		if (HasFlag(state, ResourceState::Present))      res |= D3D12_RESOURCE_STATE_PRESENT;
		if (HasFlag(state, ResourceState::RTV))          res |= D3D12_RESOURCE_STATE_RENDER_TARGET;
		if (HasFlag(state, ResourceState::DSV))          res |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
		if (HasFlag(state, ResourceState::DSV_ReadOnly)) res |= D3D12_RESOURCE_STATE_DEPTH_READ;
		if (HasFlag(state, ResourceState::CopyDst))      res |= D3D12_RESOURCE_STATE_COPY_DEST;
		if (HasFlag(state, ResourceState::CopySrc))      res |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		if (HasFlag(state, ResourceState::VertexBuffer)) res |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		if (HasFlag(state, ResourceState::IndexBuffer))  res |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
		if (HasFlag(state, ResourceState::IndirectArgs)) res |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		if (HasFlag(state, ResourceState::PixelSRV))     res |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		if (HasFlag(state, ResourceState::VertexSRV) || HasFlag(state, ResourceState::ComputeSRV)) 
			res |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		if (HasFlag(state, ResourceState::VertexUAV) || HasFlag(state, ResourceState::PixelUAV) ||
			HasFlag(state, ResourceState::ComputeUAV) || HasFlag(state, ResourceState::ClearUAV))
			res |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

		return res;
	}

	constexpr D3D12_BARRIER_SYNC ToD3D12BarrierSync(ResourceState state) {
		D3D12_BARRIER_SYNC sync = (D3D12_BARRIER_SYNC)0;

		if (HasFlag(state, ResourceState::RTV))            sync = (D3D12_BARRIER_SYNC)(sync | D3D12_BARRIER_SYNC_RENDER_TARGET);
		if (HasFlag(state, ResourceState::DSV) || HasFlag(state, ResourceState::DSV_ReadOnly)) 
			sync = (D3D12_BARRIER_SYNC)(sync | D3D12_BARRIER_SYNC_DEPTH_STENCIL);
		if (HasFlag(state, ResourceState::Present) || HasFlag(state, ResourceState::Common))
			sync = (D3D12_BARRIER_SYNC)(sync | D3D12_BARRIER_SYNC_ALL);
		if (HasFlag(state, ResourceState::CopyDst) || HasFlag(state, ResourceState::CopySrc))
			sync = (D3D12_BARRIER_SYNC)(sync | D3D12_BARRIER_SYNC_COPY);
		if (HasFlag(state, ResourceState::VertexBuffer))  sync = (D3D12_BARRIER_SYNC)(sync | D3D12_BARRIER_SYNC_VERTEX_SHADING);
		if (HasFlag(state, ResourceState::IndexBuffer))   sync = (D3D12_BARRIER_SYNC)(sync | D3D12_BARRIER_SYNC_INDEX_INPUT);
		if (HasFlag(state, ResourceState::ComputeSRV))    sync = (D3D12_BARRIER_SYNC)(sync | D3D12_BARRIER_SYNC_NON_PIXEL_SHADING);
		if (HasFlag(state, ResourceState::ComputeUAV))    sync = (D3D12_BARRIER_SYNC)(sync | D3D12_BARRIER_SYNC_COMPUTE_SHADING);
		if (HasFlag(state, ResourceState::PixelSRV))      sync = (D3D12_BARRIER_SYNC)(sync | D3D12_BARRIER_SYNC_PIXEL_SHADING);

		if (sync == (D3D12_BARRIER_SYNC)0) CESAR_DEBUGBREAK();
		return sync;
	}

	constexpr D3D12_BARRIER_LAYOUT ToD3D12BarrierLayout(ResourceState state) {
		if (HasFlag(state, ResourceState::RTV))        return D3D12_BARRIER_LAYOUT_RENDER_TARGET;
		if (HasFlag(state, ResourceState::DSV))        return D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE;
		if (HasFlag(state, ResourceState::DSV_ReadOnly)) return D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_READ;
		if (HasFlag(state, ResourceState::Present))    return D3D12_BARRIER_LAYOUT_PRESENT;
		if (HasFlag(state, ResourceState::CopyDst))    return D3D12_BARRIER_LAYOUT_COPY_DEST;
		if (HasFlag(state, ResourceState::CopySrc))    return D3D12_BARRIER_LAYOUT_COPY_SOURCE;
		if (HasFlag(state, ResourceState::ComputeUAV)) return D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS;
		if (HasFlag(state, ResourceState::ComputeSRV) || HasFlag(state, ResourceState::PixelSRV))
			return D3D12_BARRIER_LAYOUT_SHADER_RESOURCE;
		if (HasFlag(state, ResourceState::Common))     return D3D12_BARRIER_LAYOUT_COMMON;

		CESAR_DEBUGBREAK();
	}

	constexpr D3D12_BARRIER_ACCESS ToD3D12BarrierAccess(ResourceState state) {
		D3D12_BARRIER_ACCESS access = (D3D12_BARRIER_ACCESS)0;

		if (HasFlag(state, ResourceState::RTV))            access = (D3D12_BARRIER_ACCESS)(access | D3D12_BARRIER_ACCESS_RENDER_TARGET);
		if (HasFlag(state, ResourceState::DSV))            access = (D3D12_BARRIER_ACCESS)(access | D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE);
		if (HasFlag(state, ResourceState::DSV_ReadOnly))   access = (D3D12_BARRIER_ACCESS)(access | D3D12_BARRIER_ACCESS_DEPTH_STENCIL_READ);
		if (HasFlag(state, ResourceState::Present) || HasFlag(state, ResourceState::Common))
			access = (D3D12_BARRIER_ACCESS)(access | D3D12_BARRIER_ACCESS_COMMON);
		if (HasFlag(state, ResourceState::CopyDst))        access = (D3D12_BARRIER_ACCESS)(access | D3D12_BARRIER_ACCESS_COPY_DEST);
		if (HasFlag(state, ResourceState::CopySrc))        access = (D3D12_BARRIER_ACCESS)(access | D3D12_BARRIER_ACCESS_COPY_SOURCE);
		if (HasFlag(state, ResourceState::VertexBuffer))   access = (D3D12_BARRIER_ACCESS)(access | D3D12_BARRIER_ACCESS_VERTEX_BUFFER);
		if (HasFlag(state, ResourceState::IndexBuffer))    access = (D3D12_BARRIER_ACCESS)(access | D3D12_BARRIER_ACCESS_INDEX_BUFFER);
		if (HasFlag(state, ResourceState::ComputeSRV) || HasFlag(state, ResourceState::PixelSRV))
			access = (D3D12_BARRIER_ACCESS)(access | D3D12_BARRIER_ACCESS_SHADER_RESOURCE);
		if (HasFlag(state, ResourceState::ComputeUAV))    access = (D3D12_BARRIER_ACCESS)(access | D3D12_BARRIER_ACCESS_UNORDERED_ACCESS);

		if (HasAnyFlag(state, ResourceState::AllSRV))       access |= D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
		if (HasAnyFlag(state, ResourceState::AllUAV))       access |= D3D12_BARRIER_ACCESS_UNORDERED_ACCESS;

		if (access == (D3D12_BARRIER_ACCESS_UNORDERED_ACCESS | D3D12_BARRIER_ACCESS_SHADER_RESOURCE))
		{
			LOG_ERROR("Founc");
		}

		return access;
	}


	struct ClearValue
	{
		enum class ActiveMember
		{
			None, Color, DepthStencil
		};

		struct ClearColor {
			ClearColor() = default;
			ClearColor(cesar::Float rgba )
			{
				color[0] = rgba;
				color[1] = rgba;
				color[2] = rgba;
				color[3] = rgba;
			}
			ClearColor(cesar::Float r, cesar::Float g, cesar::Float b, cesar::Float a)
			{
				color[0] = r;
				color[1] = g;
				color[2] = b;
				color[3] = a;
			}

			cesar::Bool operator==(const ClearColor& other)const
			{
				return memcmp(color, other.color, CESAR_SIZEOF_BUFFER(cesar::Float, 4));
			}

			cesar::Float color[4];
		};

		struct ClearDepthStencil {
			ClearDepthStencil(cesar::Float depth = 0.0f, cesar::Uint8 stencil = 1)
				:depth(depth), stencil(stencil)
			{

			}

			cesar::Bool operator==(const ClearDepthStencil& other)const
			{
				return (depth == other.depth && stencil == other.stencil);
			}

			cesar::Float depth;
			cesar::Uint8 stencil;
		};

		ClearValue()
			:active_member(ActiveMember::None)
		{

		}

		ClearValue(cesar::Float r, cesar::Float g , cesar::Float b , cesar::Float a )
			:active_member(ActiveMember::Color), color(r, g, b, a)
		{

		}

		ClearValue(cesar::Float rgba)
			:active_member(ActiveMember::Color), color(rgba)
		{

		}

		ClearValue(cesar::Float depth, cesar::Uint8 stencil )
			:active_member(ActiveMember::DepthStencil), depth_stencil(depth, stencil)
		{

		}


		cesar::Bool operator==(const ClearValue& other)const
		{
			if (active_member == other.active_member)
			{
				if (active_member == ActiveMember::Color)
					return color == other.color;
				else
					return depth_stencil == other.depth_stencil;
			}
			return false;
		}

		ResourceFormat format = ResourceFormat::UNKNOWN;
		ActiveMember active_member = ActiveMember::None;

		union {
			ClearColor color;
			ClearDepthStencil depth_stencil;
		};
	};


	inline D3D12_CLEAR_VALUE ToD3D12ClearValue(ClearValue clear_value)
	{
		D3D12_CLEAR_VALUE value;
		if (clear_value.active_member == ClearValue::ActiveMember::Color)
		{
			value.Color[0] = clear_value.color.color[0];
			value.Color[1] = clear_value.color.color[1];
			value.Color[2] = clear_value.color.color[2];
			value.Color[3] = clear_value.color.color[3];

			value.Format = ToDXGIFormat(clear_value.format);
		}
		else {
			value.DepthStencil.Depth = clear_value.depth_stencil.depth;
			value.DepthStencil.Stencil = clear_value.depth_stencil.stencil;

			value.Format = ToDXGIFormat(clear_value.format);
		}

		return value;
	}
}