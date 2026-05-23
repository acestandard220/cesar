#include "Engine.h"
#include "../../cesar_core/Event/Input.h"

namespace cesar
{
	Engine::Engine(Window* window)
		:window(window)
	{

		gpu_context = std::make_unique<GPUContext>();
		render_context = std::make_unique<RenderContext>(window, gpu_context.get());
		offline_context = std::make_unique<OfflineContext>(gpu_context.get());

		resource_cache = std::make_unique<ResourceCache>(offline_context.get());

		//Possible Cross Depp.
		scene = std::make_shared<Scene>(window, resource_cache.get());
		scene->AddMeshEntity("Z:\\Models\\Bistro\\Bistro_v5_2\\BistroInt.glb");
		scene->AddMeshEntity("Z:\\Models\\cube.gltf");

		renderer = std::make_unique<Renderer>(window, render_context.get(),scene.get());

		editor = std::make_unique<Editor>(window, this, gpu_context.get());
	}

	Engine::~Engine()
	{}
	
	void Engine::Begin()
	{
		render_context->Begin();
	}

	void Engine::Update()
	{
		if (gInput.IsKeyDown(Key::F1))
			editor->ToggleVisibilty();

		scene->GetEditorCamera()->Update(0.2);
	}

	void Engine::Run()
	{
		if (editor->IsVisible()) {
			renderer->AddEditorPass(std::bind(&Editor::AddPass, editor.get(), std::placeholders::_1));
		}
		else {
			renderer->AddEditorPass(nullptr);
		}

		renderer->Render();
	}

	void Engine::End()
	{
		render_context->End();
	}

	void Engine::OnWindowEvent(const WindowEventInfo& event_info)
	{
		editor->OnWindowEvent(event_info);
		scene->OnWindowEvent(event_info);
	}
	
	void Engine::OnResize(Uint32 width, Uint32 height)
	{
		if (editor->IsVisible())
			return;

		render_context->OnWindowResize(width, height); // Must Always be first to guarantee all GPU work was done before resizing.
		renderer->OnResize(width, height);
	}
	
	void Engine::OnViewportResize(Uint32 width, Uint32 height)
	{
		//Swapchain must resize to window size not viewport
		{
			Uint32 width, height;
			window->GetClientDimensions(width, height);
			render_context->OnWindowResize(width, height); // Must Always be first to guarantee all GPU work was done before resizing.
		}
		renderer->OnResize(width, height);
		scene->OnResize(width, height);
	}
}
