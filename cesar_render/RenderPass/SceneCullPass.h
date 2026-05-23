#pragma once
// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.

#include "../../cesar_render_core/RenderContext.h"
#include "../RenderGraph/RenderGraph.h"
#include "../RenderGraph/RenderGraphBuilder.h"
#include "../RenderGraph/RenderGraphPass.h"
#include "../RenderGraph/RenderGraphContext.h"

namespace cesar {

	class SceneCullPass
	{

	public:
		SceneCullPass(RenderContext* render_context);
		~SceneCullPass();

		void AddPass(render_graph::RenderGraph& render_graph, Uint32 total_instance_count, Uint32 total_submesh, Uint32 total_meshlet);
		void OnResize(Uint32, Uint32) {}

	private:
		void CreatePSO();
	private:
		RenderContext* render_context = nullptr;
		std::unique_ptr<PipelineState> cull_instance_pso = nullptr;
		std::unique_ptr<PipelineState> cull_meshlet_pso  = nullptr;

		std::unique_ptr<PipelineState> build_cull_meshlet_args_pso = nullptr;
		std::unique_ptr<PipelineState> build_submesh_meshlet_execute_indirect_pso = nullptr;
	};
}
