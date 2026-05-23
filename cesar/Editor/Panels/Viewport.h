#pragma once
#include "../../../cesar_core/cesar_core.h"
#include "../../../cesar_render_core/RenderContext.h"
#include "../../../cesar_render/Renderer.h"

namespace cesar
{
	class Editor;
	class Viewport
	{
	public:
		Viewport() = delete;
		Viewport(Editor* editor);
		~Viewport();

		void DrawViewport(Uint64 viewport_texture);
	private:
		Editor* editor;

		Bool active;
		const Char* name;
		Uint32 width, height;
	};

}