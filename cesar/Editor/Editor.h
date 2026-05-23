#pragma once
#include "../../cesar_core/cesar_core.h"
#include "../../cesar_core/Platform/Window.h"
#include "../../cesar_render_core/RenderContext.h"
#include "../../cesar_render/RenderGraph/RenderGraph.h"
#include "../../cesar_render/RenderGraph/RenderGraphContext.h"
#include "../../cesar_render/RenderGraph/RenderGraphBuilder.h"

#include "Panels/Viewport.h"

namespace cesar
{
	class Engine;
	class Editor
	{
	public:
		Editor() = delete;
		Editor(Window* window, Engine* engine, GPUContext* gpu_context);
		~Editor();

		void ImGuiBegin();
		void ImGuiEnd(CommandList& cmd_list);

		void BeginDocking();
		void EndDocking();

		void AddPass(render_graph::RenderGraph& render_graph);

		Bool IsVisible()const;
		void ToggleVisibilty();

		void OnWindowEvent(const WindowEventInfo& info);
		
	private:
		Window* window;
		Engine* engine;

		std::unique_ptr<DescriptorAllocator> descriptor_pool;

		Bool is_visible;

		Viewport viewport;
	};

}