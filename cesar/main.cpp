#include "../cesar_core/Platform/Window.h"
#include "../cesar_core/Event/Input.h"
#include "../cesar_render_core/RenderContext.h"
#include "../cesar_render/Renderer.h"

#include "../cesar_asset/ResourceCache.h"
#include "../cesar_scene/Scene.h"

#include "Engine/Engine.h"

#include <Tracy/tracy\Tracy.hpp>

int main() {

	SetConsoleOutputCP(CP_UTF8);

#ifdef NDEBUG
	LOG_LEVEL(Warning);
#endif // NDEBUG

	cesar::WindowCreationParams window_creation_param{};
	window_creation_param.height = 800;
	window_creation_param.width = 800;
	window_creation_param.title = "Cesar";

	cesar::Window window(window_creation_param);
	cesar::gInput.Initialize(&window);
	window.GetWindowEvent().AddMemberFunction(&cesar::Input::OnWindowEvent, cesar::gInput);

	cesar::Engine engine(&window);
	cesar::gInput.GetInputEvent().windowResizeEvent.AddMemberFunction(&cesar::Engine::OnResize, engine);
	cesar::gInput.GetInputEvent().viewport_resize_event.AddMemberFunction(&cesar::Engine::OnViewportResize, engine);
	window.GetWindowEvent().AddMemberFunction(&cesar::Engine::OnWindowEvent, engine);
	

	while (window.Run())
	{
		ZoneScopedN("RUNLOOP");

		cesar::gInput.Tick();

		engine.Begin();
		engine.Update();
		engine.Run();
		
		engine.End();
	}

	window.Shutdown();
}