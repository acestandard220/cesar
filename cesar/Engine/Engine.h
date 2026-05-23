#pragma once
#include "../../cesar_core/cesar_core.h"
#include "../../cesar_render_core/RenderContext.h"
#include "../../cesar_render_core/OfflineContext.h"
#include "../../cesar_render/Renderer.h"

#include "../Editor/Editor.h"

namespace cesar 
{
	class Engine {
	public:
		Engine(Window* window);
		~Engine();


		void Begin();
		void Update();
		void Run();
		void End();

		void OnWindowEvent(const WindowEventInfo& event_info);
		void OnResize(Uint32 width, Uint32 height);
		void OnViewportResize(Uint32 width, Uint32 height);

	private:
		Window* window = nullptr;

		std::unique_ptr<Editor> editor = nullptr;

		std::unique_ptr<GPUContext>     gpu_context     = nullptr;
		std::unique_ptr<RenderContext>  render_context  = nullptr;
		std::unique_ptr<OfflineContext> offline_context = nullptr;

		std::unique_ptr<Renderer> renderer = nullptr;

		std::shared_ptr<Scene> scene;
		std::unique_ptr<ResourceCache> resource_cache;
	};

}