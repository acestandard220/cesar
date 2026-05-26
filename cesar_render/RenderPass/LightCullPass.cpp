#include "LightCullPass.h"
// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.

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

		render_graph.AddPass<PassData>("Light Cull Pass", RGPassType::AsyncCompute, RGPassFlags::None,
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

				cmd_list.SetPipelineState(generate_cluster_pso.get());
				cmd_list.SetComputeCBV(0, cbv->resource);
				cmd_list.SetComputeConstants(std::span<Constants>(&constants, 1));

				cmd_list.DispatchCompute(data.tile_count_x, data.tile_count_y, data.tile_z);
			}
		);
	
	}

	void LightCullPass::CreatePSO()
	{
		GPUContext* gpu_context = render_context->GetGPUContext();

		ComputePipelineStateDesc desc{};
		desc.cs = ShaderID::GenerateClusters;

		generate_cluster_pso = gpu_context->CreateComputePipelineState(desc, "Generate Clusters PSO");
	}
}