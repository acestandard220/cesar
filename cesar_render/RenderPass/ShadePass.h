#pragma once
#include "../../cesar_render_core/RenderContext.h"
#include "../RenderGraph/RenderGraph.h"
#include "../RenderGraph/RenderGraphBuilder.h"
#include "../RenderGraph/RenderGraphPass.h"
#include "../RenderGraph/RenderGraphContext.h"


namespace cesar {

	class ShadePass
	{

	public:
		ShadePass(RenderContext* render_context, Uint32 width, Uint32 height);
		~ShadePass();

		void AddSolidShadePass(render_graph::RenderGraph& render_graph);
		void OnResize(Uint32, Uint32);

	private:
		void CreatePSO();
	private:
		RenderContext* render_context = nullptr;
		std::unique_ptr<PipelineState> solid_shade_pso = nullptr;

		Uint32 width, height;
	};

}