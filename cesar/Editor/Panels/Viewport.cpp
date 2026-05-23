#include "Viewport.h"
#include "../Editor.h"
#include "../../../cesar_core/Event/Input.h"

#include <ImGui/imgui.h>

namespace cesar
{
	Viewport::Viewport(Editor* editor)
		:editor(editor), name("Viewport")
	{
		active = true;
	}

	Viewport::~Viewport()
	{
	
	}
	
	void Viewport::DrawViewport(Uint64 viewport_texture)
	{
		ImGui::Begin(name, &active);

		ImVec2 canvas_size = ImGui::GetContentRegionAvail();
		if (canvas_size.x != width || canvas_size.y != height) {
			width = canvas_size.x;
			height = canvas_size.y;
			gInput.DeferResize(width, height);
		}

		ImGui::Image(viewport_texture, ImVec2(width, height));
		ImGui::End();
	}
} 
