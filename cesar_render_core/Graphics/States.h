// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include <d3d12.h>
#include <DX12 Helper/d3dx12.h>

namespace cesar {

	//Primitive 
	enum class PrimitiveTypologyType
	{
		Point,
		Line,
		Triangle,
		Patch,
		Unknown
	};

	inline constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE ToD3DPrimitiveTopologyType(PrimitiveTypologyType primitive_topology_type) {
		switch (primitive_topology_type) {
		    case PrimitiveTypologyType::Point:    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		    case PrimitiveTypologyType::Line:     return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		    case PrimitiveTypologyType::Triangle: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		    case PrimitiveTypologyType::Patch:    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		    default:CESAR_DEBUGBREAK();
		}
	}

	enum class PrimitiveTopology
	{
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TrianglStrip
	};

	inline constexpr D3D12_PRIMITIVE_TOPOLOGY ToD3DPrimitiveTopology(PrimitiveTopology primitive_topology) {
		switch (primitive_topology) {
		    case PrimitiveTopology::PointList:    return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		    case PrimitiveTopology::LineList:     return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		    case PrimitiveTopology::LineStrip:    return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		    case PrimitiveTopology::TriangleList: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		    case PrimitiveTopology::TrianglStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		    default: CESAR_DEBUGBREAK();
		}
	}

}