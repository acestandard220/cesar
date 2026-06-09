#pragma once
#include "../../../cesar_core/cesar_core.h"
#include "../../../cesar_render_core/RenderContext.h"
#include "../../../cesar_render/Renderer.h"

namespace cesar
{
	class Scene;
	class Editor;
	class Hierarchy
	{
	public:
		Hierarchy() = delete;
		Hierarchy(Editor* editor);
		~Hierarchy();

		void DrawHierarchy(Scene* scene);
	private:
		Editor* editor;

		Bool active;
		const Char* name;

		Entity selected_entity;
	};

}