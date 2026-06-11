#include "LightCullPass.h"
#include <../cesar_render_core/Graphics/Buffer.h>

using namespace cesar::render_graph;

namespace cesar
{
	constexpr Uint32 TILE_SIZE = 16;
	constexpr Uint32 TILE_Z    = 32;

	LightCullPass::LightCullPass(RenderContext* render_context, Uint32 width, Uint32 height)
		:render_context(render_context), width(width), height(height)
	{
		CreatePSO();
	}

	LightCullPass::~LightCullPass()
	{}
	
	void LightCullPass::AddPass(render_graph::RenderGraph & render_graph)
	{
		
		struct PassData
		{
			BufferReadWrite clusters_buffer;
			BufferID frame_constants;

			Uint32 tile_count_x;
			Uint32 tile_count_y;
			Uint32 tile_z;

			Uint32 cluster_count;
		};

		render_graph.AddPass<PassData>("Generate Cluster Pass", RGPassType::AsyncCompute, RGPassFlags::None,
			[&](PassData& data, RGBuilder& builder)
			{
				data.tile_count_x = (width + TILE_SIZE - 1) / TILE_SIZE;
				data.tile_count_y = (height + TILE_SIZE - 1) / TILE_SIZE;
				data.tile_z       = TILE_Z;

				data.cluster_count = data.tile_count_x * data.tile_count_y * TILE_Z;

				struct Cluster { Vector3 min, max; };
				builder.DeclareBuffer(RG_NAME(ClustersBuffer), StructuredBufferDesc<Cluster>(data.cluster_count, ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess));

				data.clusters_buffer = builder.WriteBuffer(RG_NAME(ClustersBuffer), {});
				data.frame_constants = render_graph.GetBufferID(RG_NAME(FrameConstants));
			},
			[&](PassData& data, RGContext& context)
			{
				struct Constants
				{
					Uint32 clusters_idx;

					Uint32 tile_x;
					Uint32 tile_y;
					Uint32 tile_z;
				}constants = {
					.clusters_idx = context.GetBufferReadWriteIndex(data.clusters_buffer),
					.tile_x = data.tile_count_x,
					.tile_y = data.tile_count_y,
					.tile_z = data.tile_z
				};

				CommandList& cmd_list = context.GetCommandList();

				RGBuffer* cbv = render_graph.GetBufferResource(data.frame_constants);

				cmd_list.SetComputeCBV(0, cbv->resource);
				cmd_list.SetComputeConstants(std::span<Constants>(&constants, 1));
				cmd_list.SetPipelineState(generate_cluster_pso.get());

				cmd_list.DispatchCompute((data.tile_count_x + 15) / 16, (data.tile_count_y + 15)/16, data.tile_z);
			}
		);

		AddCullClusteraPass(render_graph);
		AddCompactClustersPass(render_graph);
	}

	void LightCullPass::AddCullClusteraPass(render_graph::RenderGraph& render_graph)
	{
		struct PassData
		{
			TextureReadOnly depth_map;
			BufferReadWrite active_clusters;

			Uint32 tile_size;
			Uint32 tile_count_x;
			Uint32 tile_count_y;

			Uint32 depth_slice_scale;
			Uint32 depth_slice_bias;

			BufferID frame_constants;
		};
	
		const FrameData& frame_data = render_graph.GetFrameData();

		render_graph.AddPass<PassData>("Cull Clusters", RGPassType::AsyncCompute, RGPassFlags::None,
			[&](PassData& data, RGBuilder& builder)
			{
				data.depth_slice_scale = TILE_Z / log2(frame_data.camera_far / frame_data.camera_near);
				data.depth_slice_bias = -(TILE_Z * log2(frame_data.camera_near) / log2(frame_data.camera_far / frame_data.camera_near));

				data.tile_size = TILE_SIZE;
				data.tile_count_x = (width + TILE_SIZE - 1) / TILE_SIZE;
				data.tile_count_y = (height + TILE_SIZE - 1) / TILE_SIZE;

				const Uint32 cluster_count = data.tile_count_x * data.tile_count_y * TILE_Z;

				builder.DeclareBuffer(RG_NAME(ActiveClustersBuffer), StructuredBufferDesc<Uint32>(cluster_count, ResourceBindFlag::UnorderedAccess));

				data.depth_map = builder.ReadTexture(RG_NAME(DepthPrePass_Map), ReadAccessType::NonPixelShader, CESAR_DEFAULT_TEXTURE_VIEW_DESC);
				data.active_clusters = builder.WriteBuffer(RG_NAME(ActiveClustersBuffer), CESAR_DEFAULT_BUFFER_VIEW_DESC);

				data.frame_constants = render_graph.GetBufferID(RG_NAME(FrameConstants));
			},
			[&, frame_data](PassData& data, RGContext& context)
			{
				struct Constants
				{
					Uint32 depth_map_idx;
					Uint32 active_clusters_idx;

					Uint32 tile_size;
					Uint32 tile_count_x;
					Uint32 tile_count_y;

					Uint32 depth_slice_scale;
					Uint32 depth_slice_bias;
				}constants = {
					.depth_map_idx = context.GetTextureReadOnlyIndex(data.depth_map),
					.active_clusters_idx = context.GetBufferReadWriteIndex(data.active_clusters),

					.tile_size = data.tile_size,
					.tile_count_x = data.tile_count_x,
					.tile_count_y = data.tile_count_y,
					.depth_slice_scale = data.depth_slice_scale,
					.depth_slice_bias = data.depth_slice_bias
				};

				CommandList& cmd_list = context.GetCommandList();

				RGBuffer* cbv = render_graph.GetBufferResource(data.frame_constants);
				cmd_list.SetComputeCBV(0, cbv->resource);
				cmd_list.SetComputeConstants(std::span<Constants>(&constants, 1));

				cmd_list.SetPipelineState(cull_cluster_pso.get());
				cmd_list.DispatchCompute((frame_data.screen_width + 31)/32, (frame_data.screen_height + 31)/32, 1);
			}
		);


	}

	void LightCullPass::AddCompactClustersPass(render_graph::RenderGraph& render_graph)
	{
		struct PassData
		{
			BufferReadOnly active_clusters;
			BufferReadWrite compacted_clusters;
			BufferReadWrite active_clusters_counter;

			Uint32 tile_count_x;
			Uint32 tile_count_y;
			Uint32 tile_count_z;
		};

		render_graph.AddPass<PassData>("Compact Clusters Pass", RGPassType::AsyncCompute, RGPassFlags::None,
			[&](PassData& data, RGBuilder& builder)
			{
				data.tile_count_x = (width + TILE_SIZE - 1) / TILE_SIZE;
				data.tile_count_y = (height + TILE_SIZE - 1) / TILE_SIZE;
				data.tile_count_z = TILE_Z;

				const Uint32 cluster_count = data.tile_count_x * data.tile_count_y * TILE_Z;

				builder.DeclareBuffer(RG_NAME(ActiveClustersCounter), RawBufferDesc());
				builder.DeclareBuffer(RG_NAME(CompactedActiveClusters), StructuredBufferDesc<Uint32>(cluster_count, ResourceBindFlag::UnorderedAccess));
				
				data.active_clusters = builder.ReadBuffer(RG_NAME(ActiveClustersBuffer), ReadAccessType::NonPixelShader, CESAR_DEFAULT_BUFFER_VIEW_DESC);
				data.active_clusters_counter = builder.WriteBuffer(RG_NAME(ActiveClustersCounter), CESAR_DEFAULT_BUFFER_VIEW_DESC);
				data.compacted_clusters = builder.WriteBuffer(RG_NAME(CompactedActiveClusters), CESAR_DEFAULT_BUFFER_VIEW_DESC);
			},
			[&](PassData& data, RGContext& context)
			{
				struct Constants
				{
					Uint32 active_clusters_idx;
					Uint32 compacted_clusters_idx;
					Uint32 active_clusters_counter_idx;

					Uint32 tile_count_x;
					Uint32 tile_count_y;
					Uint32 tile_count_z;
				}constants = {
					.active_clusters_idx = context.GetBufferReadOnlyIndex(data.active_clusters),
					.compacted_clusters_idx = context.GetBufferReadWriteIndex(data.compacted_clusters),
					.active_clusters_counter_idx = context.GetBufferReadWriteIndex(data.active_clusters_counter),

					.tile_count_x = data.tile_count_x,
					.tile_count_y = data.tile_count_y,
					.tile_count_z = data.tile_count_z
				};

				const Uint32 cluster_count = data.tile_count_x * data.tile_count_y * data.tile_count_z;

				CommandList& cmd_list = context.GetCommandList();
				cmd_list.SetPipelineState(compact_cluster_pso.get());
				cmd_list.SetComputeConstants(std::span<Constants>(&constants, 1));

				context.ClearCounterBuffer(data.active_clusters_counter);
				cmd_list.DispatchCompute((cluster_count + 31) / 32, 1, 1);
			}
		);

	}

	void LightCullPass::OnResize(Uint32 w, Uint32 h) 
	{
		width = w;
		height = h;
	}

	void LightCullPass::CreatePSO()
	{
		GPUContext* gpu_context = render_context->GetGPUContext();

		ComputePipelineStateDesc desc{};

		desc.cs = ShaderID::GenerateClusters;
		generate_cluster_pso = gpu_context->CreateComputePipelineState(desc, "Generate Clusters PSO");

		desc.cs = ShaderID::CullClusters;
		cull_cluster_pso     = gpu_context->CreateComputePipelineState(desc, "Cull Clusters PSO");

		desc.cs = ShaderID::CompactClusters;
		compact_cluster_pso  = gpu_context->CreateComputePipelineState(desc, "Compact Clusters PSO");
	}
}