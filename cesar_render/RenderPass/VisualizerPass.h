#pragma once
#include "../../cesar_render_core/RenderContext.h"
#include "../RenderGraph/RenderGraph.h"
#include "../RenderGraph/RenderGraphBuilder.h"
#include "../RenderGraph/RenderGraphPass.h"
#include "../RenderGraph/RenderGraphContext.h"

#include "../../cesar_scene/Scene.h"

namespace cesar
{
	class Visualizer
	{

	public:
		Visualizer(RenderContext* render_context, Uint32 width, Uint32 height);
		~Visualizer();

		void AddPass(render_graph::RenderGraph& render_graph, Scene*);
		void OnResize(Uint32, Uint32);

	private:
		void CreatePSO();

		void AddVisualizeSubmeshBound(render_graph::RenderGraph&, Scene*);
	private:
		RenderContext* render_context = nullptr;
		std::unique_ptr<PipelineState> viz_submesh_bounds_pso = nullptr;
		std::unique_ptr<PipelineState> pipeline_state2 = nullptr;

		Uint32 width, height;
	};
}