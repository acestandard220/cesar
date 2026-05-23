#include "SceneCullPass.h"
#include "../RenderGraph/RenderGraphContext.h"

using namespace cesar::render_graph;

namespace cesar
{
	SceneCullPass::SceneCullPass(RenderContext* render_context)
		:render_context(render_context)
	{
		CreatePSO();
	}
	SceneCullPass::~SceneCullPass()
	{
	}

	void SceneCullPass::AddPass(RenderGraph& render_graph, Uint32 total_instance_count, Uint32 total_submesh, Uint32 total_meshlet)
	{
		struct CullInstancesPassData
		{
			BufferReadOnly mesh_instances_idx;
			BufferReadOnly submesh_instances_idx;

			BufferReadWrite submesh_candidates_counter_idx;
			BufferReadWrite submesh_candidates_idx;

			Uint32 instance_count;
			Uint32 submesh_count;
		};
		render_graph.AddPass<CullInstancesPassData>("Cull Instances Pass Data", RGPassType::Compute, RGPassFlags::None,
			[&, total_instance_count](CullInstancesPassData& data, RGBuilder& builder)
			{
				struct SubMeshCandidate { Uint32 instance_idx, submesh_idx; };
				builder.DeclareBuffer(RG_NAME(SubMeshCandidatesBuffer),  StructuredBufferDesc<SubMeshCandidate>(total_submesh, ResourceBindFlag::UnorderedAccess));
				builder.DeclareBuffer(RG_NAME(SubMeshCandidatesCounter), RawBufferDesc(ResourceBindFlag::UnorderedAccess));

				data.mesh_instances_idx    = builder.ReadBuffer(RG_NAME(MeshInstanceBuffer),    ReadAccessType::NonPixelShader, {});
				data.submesh_instances_idx = builder.ReadBuffer(RG_NAME(SubmeshInstanceBuffer), ReadAccessType::NonPixelShader, {});

				data.submesh_candidates_counter_idx = builder.WriteBuffer(RG_NAME(SubMeshCandidatesCounter), {});
				data.submesh_candidates_idx         = builder.WriteBuffer(RG_NAME(SubMeshCandidatesBuffer),  {});

				data.instance_count = total_instance_count;
				data.submesh_count  = total_submesh;
			},
			[&](CullInstancesPassData& data, RGContext& context)
			{
				struct Constants
				{
					Uint32 mesh_instances_idx;
					Uint32 submesh_instances_idx;

					Uint32 submesh_candidates_counter_idx;
					Uint32 submesh_candidates_idx;
				}constants = {
					.mesh_instances_idx    = context.GetBufferReadOnlyIndex(data.mesh_instances_idx),
					.submesh_instances_idx = context.GetBufferReadOnlyIndex(data.submesh_instances_idx),

					.submesh_candidates_counter_idx = context.GetBufferReadWriteIndex(data.submesh_candidates_counter_idx),
					.submesh_candidates_idx         = context.GetBufferReadWriteIndex(data.submesh_candidates_idx)
				};

				CommandList& cmd_list = context.GetCommandList();
				cmd_list.SetComputeConstants(std::span<Constants>(&constants, 1));
				cmd_list.SetPipelineState(cull_instance_pso.get());

				context.ClearCounterBuffer(data.submesh_candidates_counter_idx);
				cmd_list.DispatchCompute(data.instance_count, 1, 1);
			}
		);

		struct BuildConstants
		{
			BufferReadWrite dispatch_arguments_idx;
			BufferReadOnly  candidate_counter_idx;
		};

		render_graph.AddPass<BuildConstants>("Build Cull Meshlet Execute Indirect Pass", RGPassType::Compute, RGPassFlags::None,
			[&](BuildConstants& data, RGBuilder& builder)
			{
				builder.DeclareBuffer(RG_NAME(CullMeshletExecuteArgsBuffer), IndirectArgumentBufferDesc(CommandArgumentType::DispatchCompute));
				
				data.dispatch_arguments_idx = builder.WriteBuffer(RG_NAME(CullMeshletExecuteArgsBuffer), {});
				data.candidate_counter_idx = builder.ReadBuffer(RG_NAME(SubMeshCandidatesCounter), ReadAccessType::NonPixelShader, {});
			},
			[&](BuildConstants& data, RGContext& context)
			{
				struct BuildConstants
				{
					Uint32 dispatch_arguments_idx;
					Uint32 submesh_candidate_counter_idx;
				} constants = {
					.dispatch_arguments_idx = context.GetBufferReadWriteIndex(data.dispatch_arguments_idx),
					.submesh_candidate_counter_idx = context.GetBufferReadOnlyIndex(data.candidate_counter_idx)
				};

				CommandList& cmd_list = context.GetCommandList();
				cmd_list.SetComputeConstants(std::span<BuildConstants>(&constants, 1));
				cmd_list.SetPipelineState(build_cull_meshlet_args_pso.get());

				cmd_list.DispatchCompute(1, 1, 1);
			}
		);

		struct CullMeshletPassData
		{
			BufferReadOnly submesh_candidates_idx;
			BufferReadOnly mesh_instances_idx;
			BufferReadOnly submeshes_idx;
			BufferReadOnly meshlets_idx;

			BufferReadWrite meshlet_candidates_idx;
			BufferReadWrite meshlet_candidates_counter_idx;

			//Extras
			BufferReadOnly execute_buffer;
		};

		render_graph.AddPass<CullMeshletPassData>("Cull Meshlets Pass", RGPassType::Compute, RGPassFlags::None,
			[&](CullMeshletPassData& data, RGBuilder& builder)
			{
				struct MeshletCandidate { Uint32 ss, ll; };
				builder.DeclareBuffer(RG_NAME(MeshletCandidateBuffer), StructuredBufferDesc<MeshletCandidate>(total_meshlet, ResourceBindFlag::UnorderedAccess));
				builder.DeclareBuffer(RG_NAME(MeshletCandidateCounter), RawBufferDesc(ResourceBindFlag::UnorderedAccess, 1));

				data.submesh_candidates_idx = builder.ReadBuffer(RG_NAME(SubMeshCandidatesBuffer), ReadAccessType::NonPixelShader, {});
				data.mesh_instances_idx = builder.ReadBuffer(RG_NAME(MeshInstanceBuffer), ReadAccessType::NonPixelShader, {});

				data.submeshes_idx = builder.ReadBuffer(RG_NAME(SubMeshDataBuffer), ReadAccessType::NonPixelShader, {});
				data.meshlets_idx = builder.ReadBuffer(RG_NAME(MeshletBuffer), ReadAccessType::NonPixelShader, {});

				data.meshlet_candidates_idx = builder.WriteBuffer(RG_NAME(MeshletCandidateBuffer), {});
				data.meshlet_candidates_counter_idx = builder.WriteBuffer(RG_NAME(MeshletCandidateCounter), {});

				data.execute_buffer = builder.ReadBuffer(RG_NAME(CullMeshletExecuteArgsBuffer), ReadAccessType::NonPixelShader, {});
			},
			[&](CullMeshletPassData& data, RGContext& context)
			{
				struct Constants
				{
					Uint32 submesh_candidates_idx;
					Uint32 mesh_instances_idx;
					Uint32 submeshes_idx;
					Uint32 meshlets_idx;

					Uint32 meshlet_candidates_idx;
					Uint32 meshlet_candidates_counter_idx;
				}constants = {
					.submesh_candidates_idx         = context.GetBufferReadOnlyIndex(data.submesh_candidates_idx),
					.mesh_instances_idx             = context.GetBufferReadOnlyIndex(data.mesh_instances_idx),
					.submeshes_idx                  = context.GetBufferReadOnlyIndex(data.submeshes_idx),
					.meshlets_idx                   = context.GetBufferReadOnlyIndex(data.meshlets_idx),
					.meshlet_candidates_idx         = context.GetBufferReadWriteIndex(data.meshlet_candidates_idx),
					.meshlet_candidates_counter_idx = context.GetBufferReadWriteIndex(data.meshlet_candidates_counter_idx)
				};

				CommandList& cmd_list = context.GetCommandList();
				cmd_list.SetPipelineState(cull_meshlet_pso.get());
				cmd_list.SetComputeConstants(std::span<Constants>(&constants, 1));

				context.ClearCounterBuffer(data.meshlet_candidates_counter_idx);
				RGBuffer* execute_buffer = render_graph.GetBufferResource(data.execute_buffer.id);
				cmd_list.DispatchComputeIndirect(execute_buffer->resource);
			});

		render_graph.AddPass<BuildConstants>("Build Draw Meshlet Execute Indirect Pass", RGPassType::Compute, RGPassFlags::None,
			[&](BuildConstants& data, RGBuilder& builder)
			{
				builder.DeclareBuffer(RG_NAME(DrawMeshletExecuteArgsBuffer), IndirectArgumentBufferDesc(CommandArgumentType::DispatchCompute));

				data.dispatch_arguments_idx = builder.WriteBuffer(RG_NAME(DrawMeshletExecuteArgsBuffer), {});
				data.candidate_counter_idx  = builder.ReadBuffer(RG_NAME(MeshletCandidateCounter), ReadAccessType::NonPixelShader, {});
			},
			[&](BuildConstants& data, RGContext& context)
			{
				struct BuildConstants
				{
					Uint32 dispatch_arguments_idx;
					Uint32 submesh_candidate_counter_idx;
				} constants = {
					.dispatch_arguments_idx = context.GetBufferReadWriteIndex(data.dispatch_arguments_idx),
					.submesh_candidate_counter_idx = context.GetBufferReadOnlyIndex(data.candidate_counter_idx)
				};

				CommandList& cmd_list = context.GetCommandList();
				cmd_list.SetComputeConstants(std::span<BuildConstants>(&constants, 1));
				cmd_list.SetPipelineState(build_cull_meshlet_args_pso.get());

				cmd_list.DispatchCompute(1, 1, 1);
			}
		);


	}

	void SceneCullPass::CreatePSO()
	{
		GPUContext* gpu_context = render_context->GetGPUContext();

		ComputePipelineStateDesc desc{};
		
		desc.cs = ShaderID::CullInstances;
		cull_instance_pso = gpu_context->CreateComputePipelineState(desc);

		desc.cs = ShaderID::BuildCullMeshletArgs;
		build_cull_meshlet_args_pso = gpu_context->CreateComputePipelineState(desc);

		desc.cs = ShaderID::CullMeshlets;
		cull_meshlet_pso = gpu_context->CreateComputePipelineState(desc);
	}
}
