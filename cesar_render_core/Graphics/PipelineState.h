// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../cesar_core/cesar_core.h"
#include "ShaderManager.h"

#include "Resource.h"
#include "ResourceFormat.h"

#include "States.h"

#include <d3d12.h>
#include <DX12 Helper/d3dx12.h>

namespace cesar
{
	//Raster 
	enum class FillMode
	{
		Solid,
		Wireframe
	};

	enum class CullMode
	{
		Front,
		Back,
		None
	};

	struct _declspec(dllexport) RasterState
	{
		FillMode fill_mode;
		CullMode cull_mode;
		cesar::Bool front_counter_clock_wise;
		cesar::Int32 depth_bias;
		cesar::Float depth_bias_clamp;
	};

	//Depth Stencil
	enum class ComparisonFunc
	{
		None = 0,
		Never = 1,
		Less = 2,
		Equal = 3,
		LessEqual = 4,
		Greater = 5,
		NotEqual = 6,
		GreaterEqual = 7,
		Always = 8
	};

	enum class DepthWriteMask {
		Zero,All
	};

	struct _declspec(dllexport) DepthStencilState
	{
		Bool DepthEnable;
		ComparisonFunc DepthFunc;
		DepthWriteMask write_mask;
	};


	inline constexpr D3D12_FILL_MODE ToD3DFillMode(FillMode fillmode) {
		switch (fillmode)
		{
		    case FillMode::Solid:     return D3D12_FILL_MODE_SOLID;
		    case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
		    default:CESAR_DEBUGBREAK();
		}
	}

	inline constexpr D3D12_CULL_MODE ToD3DCullMode(CullMode cull_mode) {
		switch (cull_mode) {
		    case CullMode::None:  return D3D12_CULL_MODE_NONE;
		    case CullMode::Back:  return D3D12_CULL_MODE_BACK;
		    case CullMode::Front: return D3D12_CULL_MODE_FRONT;
		}
	}

	inline constexpr D3D12_DEPTH_WRITE_MASK ToD3DDepthWriteMask(DepthWriteMask mask)
	{
		switch (mask) {
		case DepthWriteMask::Zero:return D3D12_DEPTH_WRITE_MASK_ZERO;
		case DepthWriteMask::All: return D3D12_DEPTH_WRITE_MASK_ALL;
		default: CESAR_DEBUGBREAK();
		}
	}


	inline constexpr D3D12_COMPARISON_FUNC ToD3DComparisonFunc(ComparisonFunc comparison_func) {
		switch (comparison_func) {
		    case ComparisonFunc::None:       return D3D12_COMPARISON_FUNC_NEVER;
		    case ComparisonFunc::Never:      return D3D12_COMPARISON_FUNC_NEVER;
		    case ComparisonFunc::Less:       return D3D12_COMPARISON_FUNC_LESS;
		    case ComparisonFunc::Equal:      return D3D12_COMPARISON_FUNC_EQUAL;
		    case ComparisonFunc::LessEqual:  return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		    case ComparisonFunc::Greater:    return D3D12_COMPARISON_FUNC_GREATER;
		    case ComparisonFunc::NotEqual:   return D3D12_COMPARISON_FUNC_NOT_EQUAL;
			case ComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
			case ComparisonFunc::Always:       return D3D12_COMPARISON_FUNC_ALWAYS;
			default: CESAR_DEBUGBREAK();     return D3D12_COMPARISON_FUNC_NEVER;
		}
	}

	struct _declspec(dllexport) GraphicsPipelineStateDesc
	{
		RasterState raster_state;
		DepthStencilState depth_stencil_state;
		PrimitiveTypologyType primitive_topology_type;

		cesar::Uint32 render_target_count;
		ResourceFormat reformat[8];

		ResourceFormat depth_format;

		ShaderID vs;
		ShaderID ps;
	};

	struct _declspec(dllexport) ComputePipelineStateDesc
	{
		ShaderID cs;
	};

	struct _declspec(dllexport) MeshPipelineStateDesc
	{
		RasterState raster_state;
		DepthStencilState depth_stencil_state;
		PrimitiveTypologyType primitive_topology_type;

		cesar::Uint32 render_target_count;
		ResourceFormat reformat[8];

		ResourceFormat depth_format;

		ShaderID as = ShaderID::NoShader;
		ShaderID ms;
		ShaderID ps;
	};

	enum class PipelineStateType : cesar::Uint8
	{
		Graphics,
		Compute,
		Mesh
	};

	class GPUContext;
	class _declspec(dllexport) PipelineState
	{
	     public:
	     	PipelineState(GPUContext* render_context, const GraphicsPipelineStateDesc& desc, const Char* name = "Unnamed Graphics Pipeline State");
			PipelineState(GPUContext* render_context, const ComputePipelineStateDesc& desc,  const Char* name = "Unnamed Compute Pipeline State");
			PipelineState(GPUContext* render_context, const MeshPipelineStateDesc& desc,     const Char* name = "Unnamed Mesh Pipeline State");
	     
	     	~PipelineState() = default;
	     
	     	ID3D12PipelineState* GetPipelineState() { return pipeline_state.Get(); }
	     	PipelineStateType GetPipelineStateType()const { return type; }
	     
	     private:
	     	ComPtr<ID3D12PipelineState> pipeline_state;
	     	PipelineStateType type;
	};
}
