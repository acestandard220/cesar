#include "GBufferPass.h"
// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.

using namespace cesar::render_graph;

namespace cesar
{
	GBufferPass::GBufferPass(RenderContext* render_context, Uint32 width, Uint32 height)
		:render_context(render_context), width(width), height(height)
	{
		CreatePSO();
	}

	GBufferPass::~GBufferPass()
	{}

	void GBufferPass::AddPass(render_graph::RenderGraph & render_graph) {
		struct DrawMeshletData
		{
			BufferReadOnly meshlets_idx;
			BufferReadOnly vertices_idx;
			BufferReadOnly meshlet_vertices_idx;
			BufferReadOnly meshlet_triangles_idx;

			BufferReadOnly meshlet_candidates_idx;
			BufferReadOnly submeshes_idx;
			BufferReadOnly submesh_candidates_idx;
			BufferReadOnly mesh_instances_idx;
			BufferReadOnly submesh_instances_idx;

			BufferReadOnly execute_buffer;
			render_graph::BufferID frame_constants;

		};
		render_graph.AddPass<DrawMeshletData>("Draw Meshlet Pass", RGPassType::Graphics, RGPassFlags::None,
			[&](DrawMeshletData& data, RGBuilder& builder)
			{
				TextureDesc gbuffer_texture_desc = RenderTargetDesc(width, height, ResourceBindFlag::ShaderResource);

				BufferViewDesc view_desc{
					.size = 0,
					.offset = 0
				};

				builder.DeclareTexture(RG_NAME(PositionMap), gbuffer_texture_desc);
				builder.DeclareTexture(RG_NAME(NormalMap), gbuffer_texture_desc);
				builder.DeclareTexture(RG_NAME(ARMMap), gbuffer_texture_desc);
				builder.DeclareTexture(RG_NAME(AlbedoMap), gbuffer_texture_desc);

				TextureViewDesc texture_view_desc{};
				builder.WriteRenderTarget(RG_NAME(PositionMap), ResourceLoadStoreFlags::ClearPreserve, {});
				builder.WriteRenderTarget(RG_NAME(NormalMap), ResourceLoadStoreFlags::ClearPreserve, {});
				builder.WriteRenderTarget(RG_NAME(ARMMap), ResourceLoadStoreFlags::ClearPreserve, {});
				builder.WriteRenderTarget(RG_NAME(AlbedoMap), ResourceLoadStoreFlags::ClearPreserve, {});

				TextureDesc gbuffer_depth_stencil_texture_desc = DepthStencilTargetDesc(width, height);
				builder.DeclareTexture(RG_NAME(DepthStencilMap), gbuffer_depth_stencil_texture_desc);
				builder.WriteDepthStencilTarget(RG_NAME(DepthStencilMap), ResourceLoadStoreFlags::ClearPreserve, ResourceLoadStoreFlags::ClearPreserve, {});


				data.meshlets_idx = builder.ReadBuffer(RG_NAME(MeshletBuffer), ReadAccessType::AllShader, {});
				data.vertices_idx = builder.ReadBuffer(RG_NAME(VertexBuffer), ReadAccessType::AllShader, {});
				data.meshlet_vertices_idx = builder.ReadBuffer(RG_NAME(MeshletVertexBuffer), ReadAccessType::AllShader, {});
				data.meshlet_triangles_idx = builder.ReadBuffer(RG_NAME(MeshletTriangleBuffer), ReadAccessType::AllShader, {});

				data.meshlet_candidates_idx = builder.ReadBuffer(RG_NAME(MeshletCandidateBuffer), ReadAccessType::AllShader, {});
				data.submeshes_idx = builder.ReadBuffer(RG_NAME(SubMeshDataBuffer), ReadAccessType::AllShader, {});

				data.submesh_candidates_idx = builder.ReadBuffer(RG_NAME(SubMeshCandidatesBuffer), ReadAccessType::AllShader, {});
				data.mesh_instances_idx = builder.ReadBuffer(RG_NAME(MeshInstanceBuffer), ReadAccessType::AllShader, {});
				data.submesh_instances_idx = builder.ReadBuffer(RG_NAME(SubmeshInstanceBuffer), ReadAccessType::AllShader, {});

				data.execute_buffer = builder.ReadBuffer(RG_NAME(DrawMeshletExecuteArgsBuffer), ReadAccessType::NonPixelShader, {});
				data.frame_constants = render_graph.GetBufferID(RG_NAME(FrameConstants));

				builder.SetViewport(width, height);
			},
			[&](DrawMeshletData& data, RGContext& context)
			{
				struct Constants
				{
					Uint32 meshlets_idx;
					Uint32 vertices_idx;
					Uint32 meshlet_vertices_idx;
					Uint32 meshlet_triangles_idx;

					Uint32 meshlet_candidates_idx;
					Uint32 submeshes_idx;

					Uint32 submesh_candidates_idx;
					Uint32 mesh_instances_idx;
					Uint32 submesh_instances_idx;

				}constants = {
					.meshlets_idx           = context.GetBufferReadOnlyIndex(data.meshlets_idx),
					.vertices_idx           = context.GetBufferReadOnlyIndex(data.vertices_idx),
					.meshlet_vertices_idx   = context.GetBufferReadOnlyIndex(data.meshlet_vertices_idx),
					.meshlet_triangles_idx  = context.GetBufferReadOnlyIndex(data.meshlet_triangles_idx),
					.meshlet_candidates_idx = context.GetBufferReadOnlyIndex(data.meshlet_candidates_idx),
					.submeshes_idx          = context.GetBufferReadOnlyIndex(data.submeshes_idx),
					.submesh_candidates_idx = context.GetBufferReadOnlyIndex(data.submesh_candidates_idx),
					.mesh_instances_idx     = context.GetBufferReadOnlyIndex(data.mesh_instances_idx),
					.submesh_instances_idx  = context.GetBufferReadOnlyIndex(data.submesh_instances_idx)
				};

				CommandList* cmd_list = &context.GetCommandList();
				cmd_list->SetPipelineState(pipeline_state.get());

				Buffer* cbuffer = render_graph.GetBufferResource(data.frame_constants)->resource;
				cmd_list->SetGraphicsCBV(0, cbuffer);

				RGBuffer* execute_buffer = render_graph.GetBufferResource(data.execute_buffer.id);

				cmd_list->SetGraphicsConstants(std::span(&constants, 1));
				cmd_list->DispatchMeshIndirect(execute_buffer->resource);
				//cmd_list->DispatchMesh(10000, 1, 1);
			}
		);
	}

	void GBufferPass::AddNonMeshShaderPass(render_graph::RenderGraph& render_graph)
	{
	
		struct NonMeshShaderPassData {
			BufferVertexID vertex_buffer;
			BufferIndexID index_buffer;

			BufferID frame_constants;
		};

		render_graph.AddPass<NonMeshShaderPassData>("Non Mesh Shader Pass", RGPassType::Graphics, RGPassFlags::None,
			[&](NonMeshShaderPassData& data, RGBuilder& builder) 
			{
				TextureDesc gbuffer_texture_desc = RenderTargetDesc(width, height, ResourceBindFlag::ShaderResource);

				BufferViewDesc view_desc{
					.size = 0,
					.offset = 0
				};

				builder.DeclareTexture(RG_NAME(PositionMap), gbuffer_texture_desc);
				builder.DeclareTexture(RG_NAME(NormalMap), gbuffer_texture_desc);
				builder.DeclareTexture(RG_NAME(ARMMap), gbuffer_texture_desc);
				builder.DeclareTexture(RG_NAME(AlbedoMap), gbuffer_texture_desc);

				TextureViewDesc texture_view_desc{};
				builder.WriteRenderTarget(RG_NAME(PositionMap), ResourceLoadStoreFlags::ClearPreserve, {});
				builder.WriteRenderTarget(RG_NAME(NormalMap), ResourceLoadStoreFlags::ClearPreserve, {});
				builder.WriteRenderTarget(RG_NAME(ARMMap), ResourceLoadStoreFlags::ClearPreserve, {});
				builder.WriteRenderTarget(RG_NAME(AlbedoMap), ResourceLoadStoreFlags::ClearPreserve, {});

				TextureDesc gbuffer_depth_stencil_texture_desc = DepthStencilTargetDesc(width, height);
				builder.DeclareTexture(RG_NAME(DepthStencilMap), gbuffer_depth_stencil_texture_desc);
				builder.WriteDepthStencilTarget(RG_NAME(DepthStencilMap), ResourceLoadStoreFlags::ClearPreserve, ResourceLoadStoreFlags::ClearPreserve, {});

				data.vertex_buffer = builder.ReadVertexBuffer(RG_NAME(VertexBuffer));
				data.index_buffer  = builder.ReadIndexBuffer(RG_NAME(IndexBuffer));

				data.frame_constants = render_graph.GetBufferID(RG_NAME(FrameConstants));

				builder.SetViewport(width, height);

			},
			[&](NonMeshShaderPassData& data, RGContext& context) 
			{
				CommandList& cmd_list = context.GetCommandList();
				cmd_list.SetPipelineState(pipeline_state2.get());
				
				Buffer* vertex_buffer = context.GetVertexBuffer(data.vertex_buffer).resource;
				Buffer* index_buffer = context.GetIndexBuffer(data.index_buffer).resource;

				//Please Start using the right methods for constant buffeers
				Buffer* cbv = render_graph.GetBufferResource(data.frame_constants)->resource;

				cmd_list.SetVertexBuffer(vertex_buffer);
				cmd_list.SetIndexBuffer(index_buffer);
				cmd_list.SetGraphicsCBV(0, cbv);

				cmd_list.SetPrimitiveTopology(PrimitiveTopology::TriangleList);

				auto& index_desc = index_buffer->GetDesc();
				cmd_list.DrawIndexed(index_desc.size / index_desc.stride, 1, 0, 0, 0);
			});
	}

	void GBufferPass::OnResize(Uint32, Uint32)
	{

	}

	void GBufferPass::CreatePSO()
	{
		MeshPipelineStateDesc desc;
		desc.raster_state = {
			.fill_mode = FillMode::Solid,
			.cull_mode = CullMode::Back,
		};
		desc.depth_stencil_state = {
			.DepthEnable = true,
			.DepthFunc = ComparisonFunc::LessEqual,
			.write_mask = DepthWriteMask::All
		};
		desc.primitive_topology_type = PrimitiveTypologyType::Triangle;
		desc.render_target_count = 4;
		desc.reformat[0] = ResourceFormat::RGBA8_UNORM;
		desc.reformat[1] = ResourceFormat::RGBA8_UNORM;
		desc.reformat[2] = ResourceFormat::RGBA8_UNORM;
		desc.reformat[3] = ResourceFormat::RGBA8_UNORM;
		desc.depth_format = ResourceFormat::D32_FLOAT;

		desc.as = ShaderID::NoShader;
		desc.ms = ShaderID::DrawMeshlet;
		desc.ps = ShaderID::GBufferPixel;

		GPUContext* gpu_context = render_context->GetGPUContext();
		pipeline_state = gpu_context->CreateMeshShaderPipelineState(desc);


		GraphicsPipelineStateDesc desc2;
		desc2.raster_state = {
			.fill_mode = FillMode::Solid,
			.cull_mode = CullMode::None,
		};
		desc2.depth_stencil_state = {
			.DepthEnable = true,
			.DepthFunc = ComparisonFunc::LessEqual,
			.write_mask = DepthWriteMask::All
		};
		desc2.primitive_topology_type = PrimitiveTypologyType::Triangle;
		desc2.render_target_count = 4;
		desc2.reformat[0] = ResourceFormat::RGBA8_UNORM;
		desc2.reformat[1] = ResourceFormat::RGBA8_UNORM;
		desc2.reformat[2] = ResourceFormat::RGBA8_UNORM;
		desc2.reformat[3] = ResourceFormat::RGBA8_UNORM;
		desc2.depth_format = ResourceFormat::D32_FLOAT;

		desc2.vs = ShaderID::GBufferVertex;
		desc2.ps = ShaderID::GBufferPixel;

		pipeline_state2 = gpu_context->CreateGraphicsPipelineState(desc2);
	}
}
