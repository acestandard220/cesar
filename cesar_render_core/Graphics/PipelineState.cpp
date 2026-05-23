// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "PipelineState.h"
#include "../GPUContext.h"

#include <DX12 Helper/d3dx12.h>

namespace cesar
{

	PipelineState::PipelineState(GPUContext* gpu_context, const GraphicsPipelineStateDesc& desc, const Char* name)
		:type(PipelineStateType::Graphics)
	{
		ID3D12Device6* device = gpu_context->GetDevice();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_desc{};

		pipeline_desc.RasterizerState.FillMode = ToD3DFillMode(desc.raster_state.fill_mode);
		pipeline_desc.RasterizerState.CullMode = ToD3DCullMode(desc.raster_state.cull_mode);
		pipeline_desc.RasterizerState.DepthBias = desc.raster_state.depth_bias;
		pipeline_desc.RasterizerState.DepthBiasClamp = desc.raster_state.depth_bias_clamp;

		pipeline_desc.DepthStencilState.DepthEnable = desc.depth_stencil_state.DepthEnable;
		pipeline_desc.DepthStencilState.DepthFunc = ToD3DComparisonFunc(desc.depth_stencil_state.DepthFunc);
		pipeline_desc.DepthStencilState.DepthWriteMask = ToD3DDepthWriteMask(desc.depth_stencil_state.write_mask);

		pipeline_desc.SampleDesc.Count = 1;
		pipeline_desc.SampleMask = UINT_MAX;

		pipeline_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipeline_desc.PrimitiveTopologyType = ToD3DPrimitiveTopologyType(desc.primitive_topology_type);
		pipeline_desc.InputLayout;
		pipeline_desc.pRootSignature = gpu_context->GetGlobalRootSignature();

		pipeline_desc.NumRenderTargets = desc.render_target_count;
		pipeline_desc.DSVFormat = ToDXGIFormat(desc.depth_format);

		for (int i = 0; i < desc.render_target_count; i++)
		{
			pipeline_desc.RTVFormats[i] = ToDXGIFormat(desc.reformat[i]);
		}

		const Shader& vertex_shader = GetShader(desc.vs);
		const Shader& pixel_shader  = GetShader(desc.ps);

		pipeline_desc.VS = GetShaderByteCode(vertex_shader);
		pipeline_desc.PS = GetShaderByteCode(pixel_shader);

		D3D12_SHADER_DESC vertex_shader_desc{};
		ID3D12ShaderReflection* vertex_shader_reflection = GetShaderReflection(vertex_shader, vertex_shader_desc);

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements(vertex_shader_desc.InputParameters);

		for (int i = 0; i < vertex_shader_desc.InputParameters; i++)
		{
			D3D12_SIGNATURE_PARAMETER_DESC parameterSignature{};

			vertex_shader_reflection->GetInputParameterDesc(i, &parameterSignature);

			D3D12_INPUT_ELEMENT_DESC inputElement{};
			inputElement.SemanticName = parameterSignature.SemanticName;
			inputElement.SemanticIndex = parameterSignature.SemanticIndex;
			inputElement.InputSlot = 0;
			inputElement.InstanceDataStepRate = 0;
			inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			inputElement.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

			if (parameterSignature.Mask == 1)
			{
				if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_UINT32) inputElement.Format = DXGI_FORMAT_R32_UINT;
				else if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_SINT32) inputElement.Format = DXGI_FORMAT_R32_SINT;
				else if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) inputElement.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (parameterSignature.Mask <= 3)
			{
				if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_UINT32) inputElement.Format = DXGI_FORMAT_R32G32_UINT;
				else if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_SINT32) inputElement.Format = DXGI_FORMAT_R32G32_SINT;
				else if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) inputElement.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (parameterSignature.Mask <= 7)
			{
				if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_UINT32) inputElement.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_SINT32) inputElement.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) inputElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (parameterSignature.Mask <= 15)
			{
				if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_UINT32) inputElement.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_SINT32) inputElement.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (parameterSignature.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) inputElement.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			inputElements[i] = inputElement;
		}

		pipeline_desc.InputLayout = { inputElements.data(), static_cast<cesar::Uint32>(inputElements.size()) };

		CESAR_D3D12_CHECK(device->CreateGraphicsPipelineState(&pipeline_desc, IID_PPV_ARGS(&pipeline_state)));
		CESAR_NAME_D3D12_OBJECT(pipeline_state, name);
	}

	PipelineState::PipelineState(GPUContext* gpu_context, const ComputePipelineStateDesc& desc, const Char* name) {
		ID3D12Device6* device = gpu_context->GetDevice();

		D3D12_COMPUTE_PIPELINE_STATE_DESC compute_pipeline_state_desc{};

		Shader compute_shader = GetShader(desc.cs);

		compute_pipeline_state_desc.CS = GetShaderByteCode(compute_shader);
		compute_pipeline_state_desc.pRootSignature = gpu_context->GetGlobalRootSignature();

		CESAR_D3D12_CHECK(device->CreateComputePipelineState(&compute_pipeline_state_desc, IID_PPV_ARGS(&pipeline_state)));
		CESAR_NAME_D3D12_OBJECT(pipeline_state, name);
	}

	PipelineState::PipelineState(GPUContext* gpu_context, const MeshPipelineStateDesc& desc, const Char* name) {
		ID3D12Device6* device = gpu_context->GetDevice();

		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC mesh_pipeline_state_desc{};
		mesh_pipeline_state_desc.RasterizerState = {
			.FillMode = ToD3DFillMode(desc.raster_state.fill_mode),
			.CullMode = ToD3DCullMode(desc.raster_state.cull_mode),
		};

		mesh_pipeline_state_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		mesh_pipeline_state_desc.DepthStencilState = {
			.DepthEnable = desc.depth_stencil_state.DepthEnable,
			.DepthWriteMask = ToD3DDepthWriteMask(desc.depth_stencil_state.write_mask),
			.DepthFunc = ToD3DComparisonFunc(desc.depth_stencil_state.DepthFunc),
		};


		mesh_pipeline_state_desc.PrimitiveTopologyType = ToD3DPrimitiveTopologyType(desc.primitive_topology_type);

		Shader amplification_shader;
		if(desc.as!= ShaderID::NoShader)
		{
			amplification_shader = GetShader(desc.as);
			mesh_pipeline_state_desc.AS = GetShaderByteCode(amplification_shader);
		}

		Shader mesh_shader = GetShader(desc.ms);
		Shader pixel_shader = GetShader(desc.ps);


		mesh_pipeline_state_desc.MS = GetShaderByteCode(mesh_shader);
		mesh_pipeline_state_desc.PS = GetShaderByteCode(pixel_shader);
		mesh_pipeline_state_desc.pRootSignature = gpu_context->GetGlobalRootSignature();
		mesh_pipeline_state_desc.NumRenderTargets = desc.render_target_count;

		for (Uint32 i = 0; i < desc.render_target_count; i++) {
			mesh_pipeline_state_desc.RTVFormats[i] = ToDXGIFormat(desc.reformat[i]);
		}
		mesh_pipeline_state_desc.DSVFormat = ToDXGIFormat(desc.depth_format);
		mesh_pipeline_state_desc.SampleDesc = {
			.Count = 1,
			.Quality = 0
		};
		mesh_pipeline_state_desc.SampleMask = UINT64_MAX;

		CD3DX12_PIPELINE_MESH_STATE_STREAM pso_stream(mesh_pipeline_state_desc);
		D3D12_PIPELINE_STATE_STREAM_DESC stream_desc{};
		stream_desc.pPipelineStateSubobjectStream = &pso_stream;
		stream_desc.SizeInBytes = sizeof(pso_stream);

		CESAR_D3D12_CHECK(device->CreatePipelineState(&stream_desc, IID_PPV_ARGS(&pipeline_state)));
		CESAR_NAME_D3D12_OBJECT(pipeline_state, name);
	}
}
