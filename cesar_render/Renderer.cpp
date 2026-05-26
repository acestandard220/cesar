#include "Renderer.h"
#include "../cesar_core/Event/Input.h"

namespace cesar {

	Renderer::Renderer(Window* window, RenderContext* render_context, Scene* scene)
		:scene(scene), render_context(render_context)
	{
		window->GetClientDimensions(width, height);

		resource_pool = new ResourcePool(render_context);
		
		gbuffer_pass    = std::make_unique<GBufferPass>(render_context, width, height);
		scene_cull_pass = std::make_unique<SceneCullPass>(render_context);
		light_cull_pass = std::make_unique<LightCullPass>(render_context, width, height);

		visualizer = std::make_unique<Visualizer>(render_context, width, height);


		GPUContext* gpu_context = render_context->GetGPUContext();
		frame_data_buffer = gpu_context->CreateConstantBuffer<FrameData>("Frame Constant Buffer");

		CreatePersistentResource();
		PrepareSceneBuffers(scene);
	}

	Renderer::~Renderer()
	{

	}

	void Renderer::NewFrame()
	{
		
	}

	void Renderer::Update(Scene* scene)
	{
	
	}

	void Renderer::Render()
	{
		ZoneScopedN("Renderer::Render");

		render_graph::RenderGraph render_graph(render_context, resource_pool);
		RenderGraphImpl(render_graph);
		render_graph.Compile();
		render_graph.Execute();
	}

	void Renderer::RenderGraphImpl(render_graph::RenderGraph& render_graph) 
	{
		Camera* camera = scene->GetEditorCamera();

		frame_data.view            = camera->View();
		frame_data.projection      = camera->Projection();
		frame_data.view_projection = camera->ViewProjection();

		frame_data.inverse_view            = camera->InverseView();
		frame_data.inverse_projection      = camera->InverseProjection();
		frame_data.inverse_view_projection = camera->InverseViewProjection();

		frame_data.camera_far      = camera->Far();
		frame_data.camera_near     = camera->Near();
		frame_data.camera_fov      = camera->Fov();
		frame_data.camera_position = { camera->Position().x,camera->Position().y,camera->Position().z,1.0f };

		frame_data.screen_width  = width;
		frame_data.screen_height = height;

		frame_data_buffer->Upload<FrameData>({ &frame_data ,1 });

		render_graph.ImportBuffer(RG_NAME(FrameConstants), frame_data_buffer.get());

		render_graph.ImportBuffer(RG_NAME(SubMeshDataBuffer), submeshdata_buffer.get());
		render_graph.ImportBuffer(RG_NAME(VertexBuffer),      vertex_buffer.get());
		render_graph.ImportBuffer(RG_NAME(IndexBuffer),       index_buffer.get());

		render_graph.ImportBuffer(RG_NAME(MeshletBuffer),         meshlet_buffer.get());
		render_graph.ImportBuffer(RG_NAME(MeshletVertexBuffer),   meshlet_vertex_buffer.get());
		render_graph.ImportBuffer(RG_NAME(MeshletTriangleBuffer), meshlet_triangle_buffer.get());

		render_graph.ImportBuffer(RG_NAME(MeshInstanceBuffer),    mesh_instance_buffer.get());
		render_graph.ImportBuffer(RG_NAME(SubmeshInstanceBuffer), submesh_instance_buffer.get());

		render_graph.ImportTexture(RG_NAME(FinalTexture), final_texture.get());
		render_graph.ImportTexture(RG_NAME(Backbuffer),   render_context->GetBackbuffer());

		render_graph::RenderGraph& rg = render_graph;

		//This is not ideal
		const Uint32 element_count = scene->GetTotalMeshInstance();
		const Uint32 submesh_count = scene->GetTotalInstanceSubmesh();
		const Uint32 meshlet_count = scene->GetTotalMeshlet();

		scene_cull_pass->AddPass(rg, element_count, submesh_count, meshlet_count);
		gbuffer_pass->AddPass(rg);
		light_cull_pass->AddPass(rg);
		visualizer->AddPass(render_graph, scene);

		
		using namespace render_graph;

		struct PassData {
			render_graph::TextureCopyDstID final_texture;
			render_graph::TextureCopySrcID src_texture;
		};
		render_graph.AddPass<PassData>("_CopyFinalTexturePass_", RGPassType::Copy, RGPassFlags::None,
			[&](PassData& data, RGBuilder& builder)
			{
				data.final_texture = builder.WriteTextureCopyDst(RG_NAME(FinalTexture));
				data.src_texture = builder.ReadTextureCopySrc(RG_NAME(PositionMap));
			},
			[&](PassData& data, RGContext& context)
			{
				CommandList& cmd_list = context.GetCommandList();

				RGTexture& final_texture = context.GetTextureResourceCopyDst(data.final_texture);
				RGTexture& src_texture = context.GetTextureResourceCopySrc(data.src_texture);

				cmd_list.CopyTexture(src_texture.resource, final_texture);
			});

		if (editor_add_pass) {
			editor_add_pass(render_graph);
		}
		else {
			//CopyToBackbuffer(render_graph);
		}
	}

	void Renderer::AddEditorPass(std::function<void(render_graph::RenderGraph&)> func)
	{
		editor_add_pass = func;
	}

	void Renderer::OnResize(Uint32 width, Uint32 height)
	{
		final_texture->Resize(width, height);
	}


	//CPU's Linear Allocator expands the buffer to twice the size required. 
	//All GPU buffers that copy from CPU LinearAllocators will query the new size and match it.
	void Renderer::PrepareSceneBuffers(Scene* scene)
	{
		auto copy_cmd_list = render_context->GetCopyCommandList();
		auto copy_fence = render_context->GetCopyFence();
		auto copy_fence_value = render_context->GetCopyFenceValue();

		copy_cmd_list->Begin();

		const Uint64& total_mesh_instance_count    = scene->GetTotalMeshInstance();
		const Uint64& total_instance_submesh_count = scene->GetTotalInstanceSubmesh();

		ResourceCache* resource_cache = scene->GetResourceCache();

		{
			Uint64 required_size = CESAR_SIZEOF_BUFFER(SubmeshInstance, total_instance_submesh_count);
			if (upload_submesh_instance_buffer->GetSize() < required_size) {
				upload_submesh_instance_buffer->Resize(required_size);
				submesh_instance_buffer->Resize(required_size);
			}

			MemoryBlock<SubmeshInstance> instance_submeshdata_span = scene->GetSubmeshInstanceAllocator()->GetMemoryPool();
			upload_submesh_instance_buffer->Upload(instance_submeshdata_span);
			copy_cmd_list->BufferBarrier(upload_submesh_instance_buffer.get(), ResourceState::Common, ResourceState::CopySrc);
			copy_cmd_list->BufferBarrier(submesh_instance_buffer.get(),        ResourceState::Common, ResourceState::CopyDst);

			std::vector<Mesh*> mesh_resources = resource_cache->GetResource<Mesh>();
			std::vector<MeshInstanceComponent> mesh_instances;
			mesh_instances.reserve(total_mesh_instance_count);

			for (const auto& mesh_resource : mesh_resources) {
				const UUID& uuid = mesh_resource->GetUUID();
				decltype(auto) instances = scene->GetMeshInsances(uuid);
				
				auto x = instances | std::views::transform([](Entity& entt) { return entt.GetComponent<MeshInstanceComponent>(); }) | 
					std::ranges::to<std::vector>();

				mesh_instances.insert(mesh_instances.end(), x.begin(), x.end());
			}

			required_size = CESAR_SIZEOF_BUFFER(MeshInstanceComponent, total_mesh_instance_count);
			if (upload_mesh_filter_buffer->GetSize() < required_size) {
				upload_mesh_filter_buffer->Resize(required_size);
				mesh_instance_buffer->Resize(required_size);
			}
			upload_mesh_filter_buffer->Upload(std::span(mesh_instances));
			copy_cmd_list->BufferBarrier(upload_mesh_filter_buffer.get(), ResourceState::Common, ResourceState::CopySrc);
			copy_cmd_list->BufferBarrier(mesh_instance_buffer.get(),        ResourceState::Common, ResourceState::CopyDst);
		}

		{
			auto meshdata_allocator = resource_cache->GetMeshAllocator();
			auto meshdata_span      = meshdata_allocator->GetMemoryPool();
			if (upload_submeshdata_buffer->GetSize() < meshdata_span.size_bytes()) {
				upload_submeshdata_buffer->Resize(meshdata_span.size_bytes());
				submeshdata_buffer->Resize(meshdata_span.size_bytes());
			}
			upload_submeshdata_buffer->Upload(meshdata_span);
			copy_cmd_list->BufferBarrier(upload_submeshdata_buffer.get(), ResourceState::Common, ResourceState::CopySrc);
			copy_cmd_list->BufferBarrier(submeshdata_buffer.get(),        ResourceState::Common, ResourceState::CopyDst);

			auto vertex_allocator = resource_cache->GetVertexAllocator();
			auto vertex_span      = vertex_allocator->GetMemoryPool();
			if (upload_vertex_buffer->GetSize() < vertex_span.size_bytes()) {
				upload_vertex_buffer->Resize(vertex_span.size_bytes());
				vertex_buffer->Resize(vertex_span.size_bytes());
			}
			upload_vertex_buffer->Upload(vertex_span);
			copy_cmd_list->BufferBarrier(upload_vertex_buffer.get(), ResourceState::Common, ResourceState::CopySrc);
			copy_cmd_list->BufferBarrier(vertex_buffer.get(),        ResourceState::Common, ResourceState::CopyDst);

			auto index_allocator = resource_cache->GetIndexAllocator();
			auto index_span = index_allocator->GetMemoryPool();
			if (upload_index_buffer->GetSize() < index_span.size_bytes()) {
				upload_index_buffer->Resize(index_span.size_bytes());
				index_buffer->Resize(index_span.size_bytes());
			}
			upload_index_buffer->Upload(index_span);
			copy_cmd_list->BufferBarrier(upload_index_buffer.get(), ResourceState::Common, ResourceState::CopySrc);
			copy_cmd_list->BufferBarrier(index_buffer.get(),        ResourceState::Common, ResourceState::CopyDst);
		}

		{
			auto meshlet_allocator = resource_cache->GetMeshletAllocator();
			auto meshlet_span = meshlet_allocator->GetMemoryPool();
			if (upload_meshlet_buffer->GetSize() < meshlet_span.size_bytes()) {
				upload_meshlet_buffer->Resize(meshlet_span.size_bytes());
				meshlet_buffer->Resize(meshlet_span.size_bytes());
			}
			upload_meshlet_buffer->Upload(meshlet_span);
			copy_cmd_list->BufferBarrier(upload_meshlet_buffer.get(), ResourceState::Common, ResourceState::CopySrc);
			copy_cmd_list->BufferBarrier(meshlet_buffer.get(),        ResourceState::Common, ResourceState::CopyDst);

			auto meshlet_vertex_allocator = resource_cache->GetMeshletVertexAllocator();
			auto meshlet_vertex_span = meshlet_vertex_allocator->GetMemoryPool();
			if (upload_meshlet_vertex_buffer->GetSize() < meshlet_vertex_span.size_bytes()) {
				upload_meshlet_vertex_buffer->Resize(meshlet_vertex_span.size_bytes());
				meshlet_vertex_buffer->Resize(meshlet_vertex_span.size_bytes());
			}
			upload_meshlet_vertex_buffer->Upload(meshlet_vertex_span);
			copy_cmd_list->BufferBarrier(upload_meshlet_vertex_buffer.get(), ResourceState::Common, ResourceState::CopySrc);
			copy_cmd_list->BufferBarrier(meshlet_vertex_buffer.get(),        ResourceState::Common, ResourceState::CopyDst);

			auto meshlet_index_allocator = resource_cache->GetMeshletTriangleAllocator();
			auto meshlet_index_span = meshlet_index_allocator->GetMemoryPool();
			if (upload_meshlet_triangle_buffer->GetSize() < meshlet_index_span.size_bytes()) {
				upload_meshlet_triangle_buffer->Resize(meshlet_index_span.size_bytes());
				meshlet_triangle_buffer->Resize(meshlet_index_span.size_bytes());
			}
			upload_meshlet_triangle_buffer->Upload(meshlet_index_span);
			copy_cmd_list->BufferBarrier(upload_meshlet_triangle_buffer.get(), ResourceState::Common, ResourceState::CopySrc);
			copy_cmd_list->BufferBarrier(meshlet_triangle_buffer.get(),        ResourceState::Common, ResourceState::CopyDst);
		}

		copy_cmd_list->FlushBarriers();

		{
			copy_cmd_list->CopyBuffer(upload_mesh_filter_buffer.get(),      mesh_instance_buffer.get());
			copy_cmd_list->CopyBuffer(upload_submesh_instance_buffer.get(), submesh_instance_buffer.get());
	
			copy_cmd_list->CopyBuffer(upload_submeshdata_buffer.get(), submeshdata_buffer.get());
			copy_cmd_list->CopyBuffer(upload_vertex_buffer.get(),   vertex_buffer.get());
			copy_cmd_list->CopyBuffer(upload_index_buffer.get(),    index_buffer.get());

			copy_cmd_list->CopyBuffer(upload_meshlet_buffer.get(),          meshlet_buffer.get());
			copy_cmd_list->CopyBuffer(upload_meshlet_vertex_buffer.get(),   meshlet_vertex_buffer.get());
			copy_cmd_list->CopyBuffer(upload_meshlet_triangle_buffer.get(), meshlet_triangle_buffer.get());
		}

		{
			copy_cmd_list->BufferBarrier(upload_submesh_instance_buffer.get(), ResourceState::CopySrc, ResourceState::Common);
			copy_cmd_list->BufferBarrier(upload_mesh_filter_buffer.get(),      ResourceState::CopySrc, ResourceState::Common);

			copy_cmd_list->BufferBarrier(submesh_instance_buffer.get(), ResourceState::CopyDst, ResourceState::Common);
			copy_cmd_list->BufferBarrier(mesh_instance_buffer.get(),      ResourceState::CopyDst, ResourceState::Common);

			copy_cmd_list->BufferBarrier(upload_submeshdata_buffer.get(), ResourceState::CopySrc, ResourceState::Common);
			copy_cmd_list->BufferBarrier(submeshdata_buffer.get(),        ResourceState::CopyDst, ResourceState::Common );

			copy_cmd_list->BufferBarrier(upload_vertex_buffer.get(), ResourceState::CopySrc, ResourceState::Common);
			copy_cmd_list->BufferBarrier(vertex_buffer.get(),        ResourceState::CopyDst, ResourceState::Common);

			copy_cmd_list->BufferBarrier(upload_index_buffer.get(), ResourceState::CopySrc, ResourceState::Common);
			copy_cmd_list->BufferBarrier(index_buffer.get(),        ResourceState::CopyDst, ResourceState::Common);

			copy_cmd_list->BufferBarrier(upload_meshlet_buffer.get(), ResourceState::CopySrc, ResourceState::Common);
			copy_cmd_list->BufferBarrier(meshlet_buffer.get(),        ResourceState::CopyDst, ResourceState::Common);

			copy_cmd_list->BufferBarrier(upload_meshlet_vertex_buffer.get(), ResourceState::CopySrc, ResourceState::Common);
			copy_cmd_list->BufferBarrier(meshlet_vertex_buffer.get(),        ResourceState::CopyDst, ResourceState::Common);

			copy_cmd_list->BufferBarrier(upload_meshlet_triangle_buffer.get(), ResourceState::CopySrc, ResourceState::Common);
			copy_cmd_list->BufferBarrier(meshlet_triangle_buffer.get(),        ResourceState::CopyDst, ResourceState::Common);
		}

		copy_cmd_list->FlushBarriers();

		UploadLightData();

		copy_cmd_list->End();

		copy_cmd_list->Signal(copy_fence, ++copy_fence_value);
		render_context->SetCopyFenceValue(copy_fence_value);

		copy_cmd_list->Submit();
	}
	
	void Renderer::CreatePersistentResource()
	{
		TextureDesc texture_desc{};
		texture_desc.usage        = ResourceUsage::Default;
		texture_desc.type         = TextureType::Texture2D;
		texture_desc.format       = ResourceFormat::RGBA8_UNORM;
		texture_desc.width        = width;
		texture_desc.height       = height;
		texture_desc.intial_state = ResourceState::Common;
		texture_desc.array_size   = 1;
		texture_desc.mips         = 1;
		texture_desc.bind_flag    = ResourceBindFlag::None;
		texture_desc.misc_flag    = TextureMiscFlag::SRGB;

		GPUContext* gpu_context = render_context->GetGPUContext();

		final_texture = gpu_context->CreateTexture(texture_desc, "Final Texture");

		submeshdata_buffer = gpu_context->CreateBuffer(StructuredBufferDesc<SubMeshData>(START_MESH_COUNT,   ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess), "SubMeshData_Buffer");
		vertex_buffer   = gpu_context->CreateBuffer(StructuredBufferDesc<Vertex>( START_VERTEX_COUNT,  ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess), "Vertex_Buffer");
		index_buffer    = gpu_context->CreateBuffer(IndexBufferDesc<Uint32>(1), "Index_Buffer");

		upload_submeshdata_buffer = gpu_context->CreateBuffer(UploadBufferDesc<SubMeshData>(START_MESH_COUNT), "Upload SubMeshData Buffer");
		upload_vertex_buffer   = gpu_context->CreateBuffer(UploadBufferDesc<Vertex>(START_VERTEX_COUNT), "Upload Vertex Buffer");
		upload_index_buffer    = gpu_context->CreateBuffer(UploadBufferDesc<Uint32>(START_INDEX_COUNT),  "Upload Index Buffer");

		meshlet_buffer = gpu_context->CreateBuffer(StructuredBufferDesc<Meshlet>(START_MESHLETS_COUNT,                               ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess), "Meshlet_Buffer");
		meshlet_vertex_buffer = gpu_context->CreateBuffer(StructuredBufferDesc<Uint32>(MAX_MESHLET_VERTICE * START_MESHLETS_COUNT,   ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess), "MeshletVertex_Buffer");
		meshlet_triangle_buffer = gpu_context->CreateBuffer(StructuredBufferDesc<Uint32>(MAX_MESHLET_TRIANGLE * START_MESHLETS_COUNT, ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess), "MeshletTriangle_Buffer");

		upload_meshlet_buffer          = gpu_context->CreateBuffer(UploadBufferDesc<Meshlet>(START_MESHLETS_COUNT),                      "UploadMeshlet_Buffer");
		upload_meshlet_vertex_buffer   = gpu_context->CreateBuffer(UploadBufferDesc<Uint32>(MAX_MESHLET_VERTICE * START_MESHLETS_COUNT), "UploadMeshletVertex_Buffer");
		upload_meshlet_triangle_buffer = gpu_context->CreateBuffer(UploadBufferDesc<Uint32>(MAX_MESHLET_TRIANGLE * START_MESHLETS_COUNT), "UploadMeshletTriangle_Buffer");

		upload_submesh_instance_buffer = gpu_context->CreateBuffer(UploadBufferDesc<SubmeshInstance>(START_INSTANCE_PER_MESH),      "UploadInstanceSubmesh_Buffer");
		upload_mesh_filter_buffer      = gpu_context->CreateBuffer(UploadBufferDesc<MeshInstanceComponent>(START_INSTANCE_PER_MESH),  "UploadMeshFilter_Buffer");

		submesh_instance_buffer = gpu_context->CreateBuffer(StructuredBufferDesc<SubmeshInstance>(START_INSTANCE_PER_MESH,      ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess), "InstanceSubmesh_Buffer");
		mesh_instance_buffer      = gpu_context->CreateBuffer(StructuredBufferDesc<MeshInstanceComponent>(START_INSTANCE_PER_MESH,  ResourceBindFlag::ShaderResource | ResourceBindFlag::UnorderedAccess), "MeshFilter_Buffer");

		upload_lights_buffer = gpu_context->CreateBuffer(UploadBufferDesc<LightComponent>(START_LIGHT_COUNT), "UploadLights_Buffer");
		lights_buffer = gpu_context->CreateBuffer(StructuredBufferDesc<LightComponent>(START_LIGHT_COUNT, ResourceBindFlag::UnorderedAccess | ResourceBindFlag::ShaderResource), "Lights_Buffer");

	}

	void Renderer::UploadLightData()
	{
		CommandList& cmd_list = *render_context->GetCopyCommandList();

		const auto required_size = CESAR_SIZEOF_BUFFER(LightComponent, scene->GetTotalLight());
		const MemoryBlock<LightComponent> light_storage = scene->GetComponentStorage<LightComponent>();


		if (upload_lights_buffer->GetSize() < required_size)
		{
			upload_lights_buffer->Resize(required_size);
			lights_buffer->Resize(required_size);
		}
		upload_lights_buffer->Upload<LightComponent>(light_storage);
		
		cmd_list.BufferBarrier(upload_lights_buffer.get(), ResourceState::Common, ResourceState::CopySrc);
		cmd_list.BufferBarrier(lights_buffer.get(), ResourceState::Common, ResourceState::CopyDst);

		cmd_list.FlushBarriers();

		cmd_list.CopyBuffer(upload_lights_buffer.get(), lights_buffer.get());

		cmd_list.BufferBarrier(upload_lights_buffer.get(), ResourceState::CopySrc, ResourceState::Common);
		cmd_list.BufferBarrier(lights_buffer.get(), ResourceState::CopyDst, ResourceState::Common);

		cmd_list.FlushBarriers();
	}

	void Renderer::CopyToBackbuffer(render_graph::RenderGraph& render_graph)
	{
		using namespace render_graph;

		struct PassData {
			TextureCopyDstID dst;
			TextureCopySrcID src;
		};
		render_graph.AddPass<PassData>("Copy To Backbuffer", RGPassType::Graphics, RGPassFlags::None,
			[&](PassData& data, RGBuilder& builder)
			{
				data.dst = builder.WriteTextureCopyDst(RG_NAME(Backbuffer));
				data.src = builder.ReadTextureCopySrc(RG_NAME(FinalTexture));
			},
			[&](PassData& data, RGContext& context)
			{
				CommandList& cmd_list = context.GetCommandList();
				auto copy_src = context.GetTextureResourceCopySrc(data.src);
				auto copy_dst = context.GetTextureResourceCopyDst(data.dst);
				cmd_list.CopyTexture(copy_src, copy_dst);
			}
		);

		//Explicit Backbuffer Transition
		PrepareBackbuffer(render_graph);
	}

	void Renderer::PrepareBackbuffer(render_graph::RenderGraph& render_graph)
	{
		using namespace render_graph;

		render_graph.AddPass<void>("_Last_", RGPassType::Graphics, RGPassFlags::None,
			[&](RGBuilder& builder)
			{
				builder.WriteRenderTarget(RG_NAME(Backbuffer), ResourceLoadStoreFlags::ClearPreserve, {});
			},
			[&](RGContext& context)
			{
			
			});
	}

}

//TODO:
//Optimize Scene Copying to only copy required bytes