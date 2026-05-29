#pragma once
#pragma once
#include "../../cesar_render_core/RenderContext.h"
#include "../RenderGraph/RenderGraph.h"
#include "../RenderGraph/RenderGraphBuilder.h"
#include "../RenderGraph/RenderGraphPass.h"
#include "../RenderGraph/RenderGraphContext.h"

#include "../../cesar_scene/Scene.h"

namespace cesar
{

	class DepthPrePass
	{
	public:
		DepthPrePass(RenderContext* render_context, Uint32 width, Uint32 height);
		~DepthPrePass();

		void AddPass(render_graph::RenderGraph& render_graph);
		void OnResize(Uint32, Uint32) {}

	private:
		void CreatePSO();
	private:
		RenderContext* render_context = nullptr;
		Uint32 width, height;

		std::unique_ptr<PipelineState> depth_pre_pass_pso = nullptr;
	};

}