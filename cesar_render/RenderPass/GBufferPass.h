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

	class GBufferPass 
	{

	public:
		GBufferPass(RenderContext* render_context,Uint32 width,Uint32 height);
		~GBufferPass();

		void AddPass(render_graph::RenderGraph& render_graph);
		void AddNonMeshShaderPass(render_graph::RenderGraph& render_graph);
		void OnResize(Uint32, Uint32);

	private:
		void CreatePSO() ;
	private:
		RenderContext* render_context = nullptr;
		std::unique_ptr<PipelineState> pipeline_state = nullptr;
		std::unique_ptr<PipelineState> pipeline_state2 = nullptr;

		Uint32 width, height;
	};

}