#pragma once
#include "../cesar_core/cesar_core.h"
#include "../cesar_core/Platform/Window.h"

#include "../cesar_render_core/RenderContext.h"
#include "../cesar_scene/Camera.h"
#include "../cesar_scene/Scene.h"

#include "FrameData.h"

#include "RenderPass/GBufferPass.h"
#include "RenderPass/SceneCullPass.h"
#include "RenderPass/VisualizerPass.h"

namespace cesar
{
	class _declspec(dllexport) Renderer
	{
	public:
		Renderer(Window* window, RenderContext* render_context, Scene* scene);
		~Renderer();

		void PrepareSceneBuffers(Scene* scene);

		void NewFrame();

		void Update(Scene* scene);
		void Render();

		void RenderGraphImpl(render_graph::RenderGraph& render_graph);
		void AddEditorPass(std::function<void(render_graph::RenderGraph&)>);

		void OnResize(Uint32 width, Uint32 height);

	private:
		void CreatePersistentPSO();
		void CreatePersistentResource();

		void UploadLightData();

		void CopyToBackbuffer(render_graph::RenderGraph& render_graph);
		void PrepareBackbuffer(render_graph::RenderGraph& render_graph);
	private:
		Scene* scene = nullptr;
		Uint32 width, height;
		RenderContext* render_context = nullptr;

		FrameData frame_data;

		std::function<void(render_graph::RenderGraph&)> editor_add_pass = nullptr;

		ResourcePool* resource_pool = nullptr;

		//Passes
		std::unique_ptr<GBufferPass> gbuffer_pass = nullptr;
		std::unique_ptr<SceneCullPass> scene_cull_pass = nullptr;
		std::unique_ptr<Visualizer> visualizer = nullptr;

		//Global Pass Resources
		std::unique_ptr<Buffer> frame_data_buffer;

		std::shared_ptr<Buffer> submeshdata_buffer;
		std::shared_ptr<Buffer> vertex_buffer;
		std::shared_ptr<Buffer> index_buffer;

		std::shared_ptr<Buffer> upload_submeshdata_buffer;
		std::shared_ptr<Buffer> upload_vertex_buffer;
		std::shared_ptr<Buffer> upload_index_buffer;

		std::shared_ptr<Buffer> meshlet_buffer;
		std::shared_ptr<Buffer> meshlet_vertex_buffer;
		std::shared_ptr<Buffer> meshlet_triangle_buffer;

		std::shared_ptr<Buffer> upload_meshlet_buffer;
		std::shared_ptr<Buffer> upload_meshlet_vertex_buffer;
		std::shared_ptr<Buffer> upload_meshlet_triangle_buffer;

		std::shared_ptr<Buffer> mesh_instance_buffer;
		std::shared_ptr<Buffer> submesh_instance_buffer;
		
		std::shared_ptr<Buffer> upload_mesh_filter_buffer;
		std::shared_ptr<Buffer> upload_submesh_instance_buffer;

		std::shared_ptr<Buffer> upload_lights_buffer;
		std::shared_ptr<Buffer> lights_buffer;


		std::shared_ptr<Texture> final_texture;
	};

}
