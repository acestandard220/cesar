#include "VisualizerPass.h"

using namespace cesar::render_graph;

namespace cesar
{
	Visualizer::Visualizer(RenderContext* render_context, Uint32 width, Uint32 height)
		:render_context(render_context), width(width), height(height)
	{
		CreatePSO();
	}

	Visualizer::~Visualizer()
	{}

	void Visualizer::AddPass(render_graph::RenderGraph & render_graph, Scene* scene)
	{
		AddVisualizeSubmeshBound(render_graph, scene);
	}

	void Visualizer::CreatePSO()
	{
		GPUContext* gpu_context = render_context->GetGPUContext();
		MeshPipelineStateDesc desc;
		desc.raster_state = {
			.fill_mode = FillMode::Solid,
			.cull_mode = CullMode::None,
		};
		desc.depth_stencil_state = {
			.DepthEnable = true,
			.DepthFunc   = ComparisonFunc::LessEqual,
			.write_mask  = DepthWriteMask::All
		};
		desc.primitive_topology_type = PrimitiveTypologyType::Triangle;
		desc.render_target_count = 4;
		desc.reformat[0]  = ResourceFormat::RGBA8_UNORM;
		desc.reformat[1]  = ResourceFormat::RGBA8_UNORM;
		desc.reformat[2]  = ResourceFormat::RGBA8_UNORM;
		desc.reformat[3]  = ResourceFormat::RGBA8_UNORM;
		desc.depth_format = ResourceFormat::D32_FLOAT;

		desc.as = ShaderID::NoShader;
		desc.ms = ShaderID::VizSubMeshBounds_MS;
		desc.ps = ShaderID::VizSubMeshBounds_PS;

		viz_submesh_bounds_pso = gpu_context->CreateMeshShaderPipelineState(desc);

	}

	void Visualizer::AddVisualizeSubmeshBound(RenderGraph& render_graph, Scene* scene)
	{
		struct PassData
		{
			BufferReadOnly submesh_candidates_idx;
			BufferReadOnly mesh_instances_idx;
			BufferReadOnly submeshes_idx;
			BufferReadOnly submesh_instances_idx;
			BufferReadOnly submesh_candidates_counter;

			BufferConstantID frame_constants;

			Scene* scene;
		};

		render_graph.AddPass<PassData>("Visualize Submesh Bounds", RGPassType::Graphics, RGPassFlags::None,
			[&](PassData& data, RGBuilder& builder)
			{
				builder.DeclareTexture(RG_NAME(SubmeshBoundsMap), RenderTargetDesc(width, height));
				builder.WriteRenderTarget(RG_NAME(SubmeshBoundsMap), ResourceLoadStoreFlags::ClearPreserve, {});

				data.submesh_candidates_idx = builder.ReadBuffer(RG_NAME(SubMeshCandidatesBuffer), ReadAccessType::AllShader, {});
				data.mesh_instances_idx = builder.ReadBuffer(RG_NAME(MeshInstanceBuffer), ReadAccessType::AllShader, {});
				data.submeshes_idx = builder.ReadBuffer(RG_NAME(SubMeshDataBuffer), ReadAccessType::AllShader, {});
				data.submesh_instances_idx = builder.ReadBuffer(RG_NAME(SubmeshInstanceBuffer), ReadAccessType::AllShader, {});
				data.submesh_candidates_counter = builder.ReadBuffer(RG_NAME(SubMeshCandidatesCounter), ReadAccessType::AllShader, {});

				data.frame_constants = builder.ReadConstantBuffer(RG_NAME(FrameConstants));

				data.scene = scene;

				builder.SetViewport(width, height);
			},
			[&](PassData& data, RGContext& context)
			{
				struct Constants
				{
					Uint32 submesh_candidates_idx;
					Uint32 mesh_instances_idx;
					Uint32 submeshes_idx;
					Uint32 submesh_instances_idx;
					Uint32 submesh_candidates_counter;
				}constants = {
					.submesh_candidates_idx = context.GetBufferReadOnlyIndex(data.submesh_candidates_idx),
					.mesh_instances_idx = context.GetBufferReadOnlyIndex(data.mesh_instances_idx),
					.submeshes_idx = context.GetBufferReadOnlyIndex(data.submeshes_idx),
					.submesh_instances_idx = context.GetBufferReadOnlyIndex(data.submesh_instances_idx),
					.submesh_candidates_counter = context.GetBufferReadOnlyIndex(data.submesh_candidates_counter)
				};

				CommandList& cmd_list = context.GetCommandList();
				cmd_list.SetPipelineState(viz_submesh_bounds_pso.get());
				cmd_list.SetGraphicsConstants(std::span<Constants>(&constants, 1));
				RGBuffer* cbv = render_graph.GetBufferResource(data.frame_constants.id);
				cmd_list.SetGraphicsCBV(0, cbv->resource);

				cmd_list.DispatchMesh((data.scene->GetTotalInstanceSubmesh() + 20) / 21, 1, 1);
			}
		);

	}

}
