#include "DepthPrePass.h"


namespace cesar
{
	using namespace cesar::render_graph;
	
	DepthPrePass::DepthPrePass(RenderContext* render_context, Uint32 width, Uint32 height)
		:render_context(render_context), width(width), height(height)
	{
		CreatePSO();
	}

	DepthPrePass::~DepthPrePass()
	{}

	void DepthPrePass::AddPass(render_graph::RenderGraph & render_graph)
	{
		struct PassData
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
			BufferID frame_constants;
		};

		render_graph.AddPass<PassData>("Depth Pre Pass", RGPassType::Graphics, RGPassFlags::None,
			[&](PassData& data, RGBuilder& builder)
			{
				builder.DeclareTexture(RG_NAME(DepthPrePass_Map), DepthStencilTargetDesc(width, height));

				builder.WriteDepthStencilTarget(RG_NAME(DepthPrePass_Map), ResourceLoadStoreFlags::ClearPreserve, ResourceLoadStoreFlags::ClearPreserve, {});

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
			[&](PassData& data, RGContext& context)
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

				CommandList& cmd_list = context.GetCommandList();
				cmd_list.SetPipelineState(depth_pre_pass_pso.get());

				RGBuffer* frame_constants = render_graph.GetBufferResource(data.frame_constants);
				RGBuffer* execute_buffer  = render_graph.GetBufferResource(data.execute_buffer.id);

				cmd_list.SetGraphicsCBV(0, frame_constants->resource);
				cmd_list.SetGraphicsConstants(std::span<Constants>(&constants, 1));
				cmd_list.DispatchMeshIndirect(execute_buffer->resource);
			}
		);
	}

	void DepthPrePass::CreatePSO()
	{
		GPUContext* gpu_context = render_context->GetGPUContext();

		MeshPipelineStateDesc desc;
		desc.raster_state = {
			.fill_mode = FillMode::Solid,
			.cull_mode = CullMode::None,
		};
		desc.depth_stencil_state = {
			.DepthEnable = true,
			.DepthFunc = ComparisonFunc::LessEqual,
			.write_mask = DepthWriteMask::All
		};
		desc.primitive_topology_type = PrimitiveTypologyType::Triangle;
		desc.render_target_count = 0;
		desc.depth_format = ResourceFormat::D32_FLOAT;

		desc.as = ShaderID::NoShader;
		desc.ms = ShaderID::DrawMeshlet;
		desc.ps = ShaderID::GBufferPixel;

		depth_pre_pass_pso = gpu_context->CreateMeshShaderPipelineState(desc);
	}
}
