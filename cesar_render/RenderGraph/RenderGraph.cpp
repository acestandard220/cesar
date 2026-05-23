// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "RenderGraph.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphContext.h"

#include <ranges>

namespace cesar::render_graph
{
	RenderGraph::RenderGraph(RenderContext* render_context, ResourcePool* resource_pool)
		:render_context(render_context), resource_pool(resource_pool)
	{
		
	}

	RenderGraph::~RenderGraph()
	{
		CleanUp();
	}

	void RenderGraph::Compile()
	{
		//To be cleared
		texture_desc_map.clear();
		buffer_desc_map.clear();

		dependency_levels.clear();
		topologically_sorted_list.clear();
		adj_list.clear();

		BuildAdjacencyList();
		TopologicalSort();
		BuildDependencyLevels();
		ResolveAsync();
		CalculateResourceLifeTime();

		for (auto& dep_lv : dependency_levels) {
			dep_lv.SetUp();
		}
	}

	void RenderGraph::Execute()
	{
		ZoneScopedN("RenderGraph::Execute");

		resource_pool->Update();

		RenderGraphExecutionContext execution_context{};
		execution_context.render_context = render_context;
		execution_context.graphics_command_list = render_context->GetGraphicsCommandList();
		execution_context.compute_command_list = render_context->GetComputeCommandList();
		
		execution_context.graphics_fence = render_context->GetGraphicsFence();
		execution_context.compute_fence = render_context->GetComputeFence();

		for (auto& dep_levels : dependency_levels)
		{
			dep_levels.Execute(execution_context);
		}

	}

	void RenderGraph::CleanUp()
	{
		ZoneScoped("RenderGraph::CleanUp");

		for (auto& descriptor : _texture_views_to_destroy) {
			render_context->FreeCPUDescriptor(descriptor);
		}
		_texture_views_to_destroy.clear();
	}

	TextureID RenderGraph::ImportTexture(RGResourceName name, Texture* texture) {
		textures.emplace_back(std::make_unique<RGTexture>(textures.size(), texture, name));
		TextureID texture_id(textures.size() - 1);
		texture_name_id_map[name] = texture_id;
		return texture_id;
	}

	BufferID RenderGraph::ImportBuffer(RGResourceName name, Buffer* buffer) {
		buffers.emplace_back(std::make_unique<RGBuffer>(buffers.size(), buffer, name));
		BufferID buffer_id(buffers.size() - 1);
		buffer_name_id_map[name] = buffer_id;
		return buffer_id;
	}

	TextureID RenderGraph::DeclareTexture(RenderGraphResourceName name, const TextureDesc& texture_desc)
	{
		SOL_ASSERT_MSG(texture_name_id_map.find(name) == texture_name_id_map.end(), "The specified texture has already been declared");
		textures.emplace_back(std::make_unique<RGTexture>(textures.size(), texture_desc, name));
		texture_name_id_map[name] = textures.size() - 1;
		return textures.size() - 1;
	}

	BufferID RenderGraph::DeclareBuffer(RenderGraphResourceName name, const BufferDesc& buffer_desc)
	{
		buffers.emplace_back(std::make_unique<RGBuffer>(buffers.size(), buffer_desc, name));
		buffer_name_id_map[name] = buffers.size() - 1;
		return buffers.size() - 1;
	}

	RenderTargetID RenderGraph::RenderTarget(RGResourceName name, const TextureViewDesc& desc)
	{
		TextureID texture_id = texture_name_id_map[name];
		CESAR_ASSERT(IsTextureHandleValid(texture_id) && "Cannot write to invalide texture.");
		RGTexture* texture_resource = GetTextureResource(texture_id);
		texture_resource->desc.bind_flag |= ResourceBindFlag::RenderTarget;
		auto& view_descs = texture_view_desc_map[texture_id];
		for (Uint32 i = 0; i < view_descs.size(); i++) {

			auto& [_desc, res_desc_type] = view_descs[i];
			if (_desc == desc && res_desc_type == ResourceDescriptorType::RenderTarget) {
				return RenderTargetID(i, texture_id);
			}
		}

		view_descs.emplace_back(std::pair(desc, ResourceDescriptorType::RenderTarget));
		return RenderTargetID(view_descs.size() - 1, texture_id);
	}

	DepthStencilID RenderGraph::DepthStencilTarget(RGResourceName name, const TextureViewDesc& desc)
	{
		TextureID texture_id = texture_name_id_map[name];
		CESAR_ASSERT(IsTextureHandleValid(texture_id) && "Cannot write to invalide texture.");
		RGTexture* texture_resource = GetTextureResource(texture_id);
		texture_resource->desc.bind_flag |= ResourceBindFlag::DepthTarget;

		auto& view_descs = texture_view_desc_map[texture_id];
		for (Uint32 i = 0; i < view_descs.size(); i++) {

			auto& [_desc, res_desc_type] = view_descs[i];
			if (_desc == desc && res_desc_type == ResourceDescriptorType::DepthStencil) {
				return DepthStencilID(i, texture_id);
			}
		}

		view_descs.emplace_back(std::pair(desc, ResourceDescriptorType::DepthStencil));
		return DepthStencilID(view_descs.size() - 1, texture_id);
	}

	TextureReadWrite RenderGraph::WriteTexture(RGResourceName name, TextureViewDesc desc)
	{
		TextureID texture_id = texture_name_id_map[name];
		CESAR_ASSERT(IsTextureHandleValid(texture_id) && "Cannot write to invalide texture.");
		RGTexture* texture_resource = GetTextureResource(texture_id);
		texture_resource->desc.bind_flag |= ResourceBindFlag::UnorderedAccess;

		auto& view_descs = texture_view_desc_map[texture_id];
		int i = 0;
		for (auto& [_desc, res_desc_id] : view_descs) {

			if (desc == _desc && res_desc_id == ResourceDescriptorType::ReadWrite) {
				return TextureReadWrite(i, texture_id);
			}
			i++;
		}
		view_descs.emplace_back(std::pair{ desc,ResourceDescriptorType::ReadWrite });
		return TextureReadWrite(view_descs.size() - 1, texture_id);
	}

	TextureReadOnly RenderGraph::ReadTexture(RGResourceName name, const TextureViewDesc& desc)
	{
		TextureID texture_id = GetTextureID(name);
		CESAR_ASSERT(IsTextureHandleValid(texture_id) && "Cannot read to invalide texture.");
		RGTexture* texture_resource = GetTextureResource(texture_id);
		texture_resource->desc.bind_flag |= ResourceBindFlag::ShaderResource;

		auto& view_desc = texture_view_desc_map[texture_id];
		for (Uint32 i = 0; i < view_desc.size(); i++) {
			const auto& [_desc, res_desc_type] = view_desc[i];
			if (_desc == desc && res_desc_type == ResourceDescriptorType::ReadOnly) {
			    return TextureReadOnly(i, texture_id);
			}
		}

		view_desc.emplace_back(std::pair{ desc,ResourceDescriptorType::ReadOnly });
		return TextureReadOnly(view_desc.size() - 1, texture_id);
	}



	BufferReadWrite RenderGraph::WriteBuffer(RGResourceName name, BufferViewDesc desc)
	{
		BufferID buffer_id = buffer_name_id_map[name];
		CESAR_ASSERT(IsBufferHandleValid(buffer_id) && "Cannot write to invalid buffer.");
		RGBuffer* buffer_resource = GetBufferResource(buffer_id);
		buffer_resource->desc.bind_flag |= ResourceBindFlag::UnorderedAccess;

		auto& view_descs = buffer_view_desc_map[buffer_id];
		for (Uint32 i = 0; i < view_descs.size(); i++) {
			const auto& [_desc, res_desc_id] = view_descs[i];
			if (_desc == desc && res_desc_id == ResourceDescriptorType::ReadWrite) {
				return BufferReadWrite(i, buffer_id);
			}
		}

		view_descs.emplace_back(std::pair{ desc,ResourceDescriptorType::ReadWrite });
		return BufferReadWrite(view_descs.size() - 1, buffer_id);
	}

	BufferReadOnly RenderGraph::ReadBuffer(RGResourceName name,const BufferViewDesc& desc)
	{
		BufferID buffer_id = GetBufferID(name);
		CESAR_ASSERT(IsBufferHandleValid(buffer_id) && "Cannot read to invalide buffer.");
		RGBuffer* buffer_resource = GetBufferResource(buffer_id);
		buffer_resource->desc.bind_flag |= ResourceBindFlag::ShaderResource;

		auto& view_desc = buffer_view_desc_map[buffer_id];
		for (Uint32 i = 0; i < view_desc.size(); i++) {
			const auto& [_desc, res_desc_type] = view_desc[i];
			if (_desc == desc && res_desc_type == ResourceDescriptorType::ReadOnly) {
				return BufferReadOnly(i, buffer_id);
			}
		}

		view_desc.emplace_back(std::pair{ desc,ResourceDescriptorType::ReadOnly });
		return BufferReadOnly(view_desc.size() - 1, buffer_id);
	}

	BufferCopyDstID RenderGraph::WriteBufferCopyDst(RGResourceName name)
	{
		BufferID buffer_id = GetBufferID(name);
		CESAR_ASSERT(IsBufferHandleValid(buffer_id) && "Cannot read to invalide buffer.");
		return BufferCopyDstID(buffer_id.id);
	}

	BufferCopySrcID RenderGraph::ReadBufferCopySrc(RGResourceName name)
	{
		BufferID buffer_id = GetBufferID(name);
		CESAR_ASSERT(IsBufferHandleValid(buffer_id) && "Cannot read to invalide texture.");
		return BufferCopySrcID(buffer_id.id);
	}

	TextureCopyDstID RenderGraph::WriteTextureCopyDst(RGResourceName name)
	{
		TextureID texture_id = GetTextureID(name);
		CESAR_ASSERT(IsTextureHandleValid(texture_id) && "Cannot write to an invalid texture.");
		return TextureCopyDstID(texture_id.id);
	}

	TextureCopySrcID RenderGraph::ReadTextureCopySrc(RGResourceName name)
	{
		TextureID texture_id = GetTextureID(name);
		CESAR_ASSERT(IsTextureHandleValid(texture_id) && "Cannot read from an invalid texture.");
		return TextureCopySrcID(texture_id.id);
	}

	BufferVertexID RenderGraph::ReadVertexBuffer(RGResourceName name)
	{
		BufferID buffer_id = GetBufferID(name);
		CESAR_ASSERT(IsBufferHandleValid(buffer_id) && "Cannot read from an invalid buffer.");
		return BufferVertexID(buffer_id.id);
	}

	BufferIndexID RenderGraph::ReadIndexBuffer(RGResourceName name)
	{
		BufferID buffer_id = GetBufferID(name);
		CESAR_ASSERT(IsBufferHandleValid(buffer_id) && "Cannot read from an invalid buffer.");
		return BufferIndexID(buffer_id.id);
	}

	BufferConstantID RenderGraph::ReadConstantBuffer(RGResourceName name)
	{
		BufferID buffer_id = GetBufferID(name);
		CESAR_ASSERT(IsBufferHandleValid(buffer_id) && "Cannot read from an invalid buffer.");
		return BufferConstantID(buffer_id.id);
	}

	Bool RenderGraph::IsTextureHandleValid(TextureID texture_id)
	{
		return texture_id.IsValid() && textures.size() > texture_id.id;
	}

	Bool RenderGraph::IsBufferHandleValid(BufferID buffer_id)
	{
		return buffer_id.IsValid() && buffers.size() > buffer_id.id;
	}

	RGTexture* RenderGraph::GetTextureResource(TextureID texture_id)
	{
		return textures[texture_id.id].get();
	}

	RGBuffer* RenderGraph::GetBufferResource(BufferID buffer_id)
	{
		return buffers[buffer_id.id].get();
	}

	TextureID RenderGraph::GetTextureID(RGResourceName name)
	{
		return texture_name_id_map[name];
	}

	BufferID RenderGraph::GetBufferID(RGResourceName name)
	{
		return buffer_name_id_map[name];
	}

	Uint32 RenderGraph::GetBufferReadOnlyIndex(BufferReadOnly buffer_id)
	{
		Descriptor descriptor = buffer_desc_map[buffer_id.GetResourceID()][buffer_id.GetViewID()];
		return render_context->GetBindlesDescriptorIndex(descriptor);
	}

	Uint32 RenderGraph::GetBufferReadWriteIndex(BufferReadWrite buffer_id)
	{
		Descriptor descriptor = buffer_desc_map[buffer_id.GetResourceID()][buffer_id.GetViewID()];
		return render_context->GetBindlesDescriptorIndex(descriptor);
	}

	void RenderGraph::BuildAdjacencyList()
	{
		const Uint64 passes_count = passes.size();
		adj_list.resize(passes_count);

		for (Uint64 i = 0; i < adj_list.size(); i++) {
			auto current_pass = passes[i].get();
			decltype(auto) current_adj_list = adj_list[i];

			for (Uint64 j = 0; j < passes_count; j++) {
				if (j == i)
					continue;

				auto other_pass = passes[j].get();
				Bool depends = false;
				for (auto other_texture_reads : other_pass->texture_reads) {
					depends = current_pass->texture_writes.contains(other_texture_reads);
					if (depends) {
						current_adj_list.push_back(j);
						break;
					}
				}

				if (depends)
					continue;

				for (auto other_buffer_reads : other_pass->buffer_reads) {
					if (depends = current_pass->buffer_writes.contains(other_buffer_reads)) {
						current_adj_list.push_back(j);
						break;
					}
				}
			}
		}
	}

	void RenderGraph::TopologicalSort()
	{
		const Uint64 passes_count = passes.size();
		topologically_sorted_list.clear();
		topologically_sorted_list.reserve(passes_count);

		std::vector<Bool> visited(passes_count, false);

		std::function<void(Uint64)> depth_first_search = [&](Uint64 pass_idx)->void {
			visited[pass_idx] = true;

			for (auto j : adj_list[pass_idx]) {
				if (!visited[j]) {
					depth_first_search(j);
				}
			}

			topologically_sorted_list.push_back(pass_idx);
			};

		for (Uint64 i = 0; i < passes_count; i++) {
			if (!visited[i]) {
				depth_first_search(i);
			}
		}

		std::reverse(topologically_sorted_list.begin(), topologically_sorted_list.end());
	}

	void RenderGraph::BuildDependencyLevels()
	{
		std::vector<Uint64> distance(topologically_sorted_list.size(), 0);

		for (Uint64 i : topologically_sorted_list) {
			decltype(auto) current_adj_list = adj_list[i];
			for (Uint64 j : current_adj_list) {
				if (distance[j] < distance[i] + 1) {
					distance[j] = distance[i] + 1;
				}
			}
		}

		const Uint64 max_level = *std::max_element(distance.begin(), distance.end()) + 1;
		for (Uint64 i = 0; i < max_level; i++) {
			dependency_levels.emplace_back(DependencyLevel(*this, i));
		}

		for (Uint64 i = 0; i < distance.size(); i++) {
			Uint64 level = distance[i];
			dependency_levels[level].AddPass(passes[i].get());
		}
	}

	void RenderGraph::ResolveAsync()
	{
		std::vector<RGPassBase*> compute_passes;
		std::pair<RGPassBase*, Bool> pre_graphics_passes  = { passes[0].get(), false };                    
		std::pair<RGPassBase*, Bool> post_graphics_passes = { passes[passes.size() - 1].get(), false };   

		Uint64 graphics_fence_value = 0;
		Uint64 compute_fence_value  = 0;

		for (auto& pass_index : topologically_sorted_list) {
			RGPassBase* pass = passes[pass_index].get();

			if (pass->type == RenderGraphPassType::AsyncCompute) {
				for (auto& texture_reads : pass->texture_reads) 
				{
					for (Int32 i = pass_index - 1; i >= 0; --i) {
						RGPassBase* previous_pass = passes[i].get();

						if (previous_pass->type == RGPassType::AsyncCompute) continue;

						if (previous_pass->texture_writes.contains(texture_reads)) {
							pre_graphics_passes = (pre_graphics_passes.first->id < previous_pass->id) ?
								std::pair{previous_pass, true} : pre_graphics_passes;
							break;
						}
					}
				}

				for (auto& buffer_read : pass->buffer_reads) {
					for (Int32 i = pass_index - 1; i >= 0; --i) {
						RGPassBase* previous_pass = passes[i].get();

						if (previous_pass->type == RGPassType::AsyncCompute) continue;

						if (previous_pass->buffer_writes.contains(buffer_read)) {
							pre_graphics_passes = (pre_graphics_passes.first->id < previous_pass->id) ?
								std::pair{ previous_pass, true } : pre_graphics_passes;
							break;
						}
					}
				}

				for (auto& texture_write : pass->texture_writes) {
					for (Int32 i = pass_index + 1; i < passes.size(); i++) {
						RGPassBase* next_pass = passes[i].get();

						if (next_pass->type == RGPassType::AsyncCompute) continue;

						if (next_pass->texture_reads.contains(texture_write)) {
							post_graphics_passes = (post_graphics_passes.first->id < next_pass->id) ? 
								post_graphics_passes : std::pair{next_pass, true};
							break;
						}
					}
				}

				for (auto& buffer_write : pass->buffer_writes) {
					for (Int32 i = pass_index + 1; i < passes.size(); i++) {
						RGPassBase* next_pass = passes[i].get();

						if (next_pass->type == RGPassType::AsyncCompute) continue;

						if (next_pass->buffer_reads.contains(buffer_write)) {
							post_graphics_passes = (post_graphics_passes.first->id < next_pass->id) ?
								post_graphics_passes : std::pair{ next_pass, true };
							break;
						}
					}
				}
				compute_passes.push_back(pass);
			}
			else if (compute_passes.size()) {
				if (pre_graphics_passes.second) {
					decltype(auto) pre_compute_pass = pre_graphics_passes.first;
					pre_compute_pass->signal_value = ++graphics_fence_value;
					
					decltype(auto) first_compute_pass = compute_passes.front();
					first_compute_pass->wait_value = graphics_fence_value;
				}

				if (post_graphics_passes.second) {
					decltype(auto) last_compute_pass = compute_passes.back();
					last_compute_pass->signal_value = ++compute_fence_value;

					decltype(auto) last_pass_before = post_graphics_passes.first;
					last_pass_before->wait_value = compute_fence_value;
				}
				compute_passes.clear();
				pre_graphics_passes  = { passes[0].get(), false };                
				post_graphics_passes = { passes[passes.size() - 1].get(), false };
			}
		}
	}

	void RenderGraph::CalculateResourceLifeTime()
	{
		for (auto& dep_level : dependency_levels) {
			for (auto& pass : dep_level.passes)
			{
				for (auto& texture_read : pass->texture_reads) {
					RGTexture* texture_resource = GetTextureResource(texture_read);
					texture_resource->last_writer = pass;
				}

				for (auto& texture_write : pass->texture_writes) {
					RGTexture* texture_resource = GetTextureResource(texture_write);
					texture_resource->last_writer = pass;
				}

				for (auto& buffer_read : pass->buffer_reads)
				{
					RGBuffer* buffer_resource = GetBufferResource(buffer_read);
					buffer_resource->last_writer = pass;
				}
				
				for (auto& buffer_write : pass->buffer_writes) {
					RGBuffer* buffer_resource = GetBufferResource(buffer_write);
					buffer_resource->last_writer = pass;
				}
			}
		}
	
		for (auto& texture : textures) {
			if (texture->last_writer != nullptr) {
				texture->last_writer->texture_destroys.insert(texture->id);
			}

			if (texture->imported) {
				CreateResourceView(TextureID(texture->id));
			}
		}

		for (auto& buffer : buffers) {
			if (buffer->last_writer != nullptr) {
				buffer->last_writer->buffer_destroys.insert(buffer->id);
			}

			if (buffer->imported) {
				CreateResourceView(BufferID(buffer->id));
			}
		}
	
	}

	void RenderGraph::CreateResourceView(TextureID texture_id)
	{
		GPUContext* gpu_context = render_context->GetGPUContext();

		Texture* texture = GetTextureResource(texture_id)->resource;
		for (auto& [view_desc, res_desc_type] : texture_view_desc_map[texture_id]) {
			Descriptor descriptor;
			switch (res_desc_type)
			{
			case ResourceDescriptorType::RenderTarget: {
				descriptor = gpu_context->CreateTextureRTV(texture, view_desc);
				_texture_views_to_destroy.push_back(descriptor);
				break;
			}
			case ResourceDescriptorType::DepthStencil: {
				descriptor = gpu_context->CreateTextureDSV(texture, view_desc);
				_texture_views_to_destroy.push_back(descriptor);
				break;
			}
			case ResourceDescriptorType::ReadOnly: {
				descriptor = gpu_context->CraeteTextureSRV(texture, view_desc, false, false);
				break;
			}
			case ResourceDescriptorType::ReadWrite: {
				CESAR_FEATURE_NO_IMPL("There is no implementation for this yet");
			}
			default:
				break;
			}
			texture_desc_map[texture_id].push_back(descriptor);
		}
	}

	void RenderGraph::CreateResourceView(BufferID buffer_id)
	{
		GPUContext* gpu_context = render_context->GetGPUContext();

		RGBuffer* buffer_resource = GetBufferResource(buffer_id);
		for (auto& [view_desc, res_desc_type] : buffer_view_desc_map[buffer_id]) {
			Descriptor descriptor;
			switch (res_desc_type) {
			case ResourceDescriptorType::ReadOnly: {
				descriptor = gpu_context->CreateBufferSRV(buffer_resource->resource, view_desc, false, false);
				break;
			}
			case ResourceDescriptorType::ReadWrite:
			default:
				descriptor = gpu_context->CreateBufferUAV(buffer_resource->resource, view_desc, false, false);
				break;
			}
			buffer_desc_map[buffer_id].push_back(descriptor);
		}
	}

	Descriptor RenderGraph::GetRenderTargetDescriptor(RenderTargetID id)
	{
		return texture_desc_map[id.GetResourceID()][id.GetViewID()];
	}

	Descriptor RenderGraph::GetDepthStencilTargetDescriptor(DepthStencilID id)
	{
		return texture_desc_map[id.GetResourceID()][id.GetViewID()];
	}

	Descriptor RenderGraph::GetBufferReadWriteDescriptor(BufferReadWrite id)
	{
		return buffer_desc_map[id.GetResourceID()][id.GetViewID()];
	}

	Descriptor RenderGraph::GetBufferReadOnlyDescriptor(BufferReadOnly id)
	{
		return buffer_desc_map[id.GetResourceID()][id.GetViewID()];
	}

	Descriptor RenderGraph::GetTextureReadOnlyDescriptor(TextureReadOnly id)
	{
		return texture_desc_map[id.GetResourceID()][id.GetViewID()];
	}

	void RenderGraph::DependencyLevel::AddPass(RGPassBase* pass)
	{
		passes.push_back(pass);

		texture_reads.insert(pass->texture_reads.begin(), pass->texture_reads.end());
		texture_writes.insert(pass->texture_writes.begin(), pass->texture_writes.end());

		buffer_writes.insert(pass->buffer_writes.begin(), pass->buffer_writes.end());
		buffer_reads.insert(pass->buffer_reads.begin(), pass->buffer_reads.end());
	}

	void RenderGraph::DependencyLevel::SetUp()
	{
		for (auto pass : passes) {

			texture_creates.insert(pass->texture_creates.begin(), pass->texture_creates.end());
			texture_destroys.insert(pass->texture_destroys.begin(), pass->texture_destroys.end());
			for (auto& [resource, state] : pass->texture_state) {
				texture_states[resource] |= state;
			}

			buffer_creates.insert(pass->buffer_creates.begin(), pass->buffer_creates.end());
			buffer_destroys.insert(pass->buffer_destroys.begin(), pass->buffer_destroys.end());
			for (auto& [resource, state] : pass->buffer_state) {
				buffer_states[resource] |= state;
			}
		}
	}

	void RenderGraph::DependencyLevel::Execute(RenderGraphExecutionContext& execution_context)
	{
		PreExecution(execution_context);

		for (auto& pass : passes)
		{
			auto& cmd_list = (pass->type != RenderGraphPassType::AsyncCompute) ? execution_context.graphics_command_list : execution_context.compute_command_list;

			RenderGraphContext render_graph_context(rg, *pass, *cmd_list);

			if (pass->wait_value != Uint64(-1)) {
				cmd_list->End();
				cmd_list->Submit();
				cmd_list->Begin();

				if (pass->type == RGPassType::AsyncCompute)
					cmd_list->Waits(execution_context.graphics_fence, execution_context.graphics_fence_value + pass->wait_value);
				else
					cmd_list->Waits(execution_context.compute_fence, execution_context.compute_fence_value + pass->wait_value);

			}

			if (pass->type == RenderGraphPassType::Graphics) {

				RenderPassDesc render_pass_desc{};
				render_pass_desc.render_targets.resize(pass->render_targets.size());
				for (Uint32 i = 0; i < pass->render_targets.size(); i++)
				{
					const auto& pass_render_target = pass->render_targets[i];
					ColorAttachment& color_attachment = render_pass_desc.render_targets[i];

					color_attachment.descriptor = rg.GetRenderTargetDescriptor(pass_render_target.id);

					LoadResourceFlag load_flags;
					StoreResourceFlag store_flags;

					SplitResourceLoadStoreFlags(pass_render_target.load_store_flags, load_flags, store_flags);

					color_attachment.begin_flags = load_flags;
					color_attachment.end_flags = store_flags;

					RGTexture* texture_resource = rg.GetTextureResource(pass_render_target.id.GetResourceID());
					if (load_flags == LoadResourceFlag::Clear) {
						color_attachment.clear_value = texture_resource->desc.clear_value;
					}
				}

				if (pass->depth_targets.has_value()) {
					const auto& pass_depth_stencil_target = pass->depth_targets.value();
					DepthStencilAttachment depth_stencil_attachment;

					depth_stencil_attachment.descriptor = rg.GetDepthStencilTargetDescriptor(pass_depth_stencil_target.id);

					LoadResourceFlag load_flag;
					StoreResourceFlag store_flag;

					SplitResourceLoadStoreFlags(pass_depth_stencil_target.depth_load_store_flags, load_flag, store_flag);

					depth_stencil_attachment.depth_begin_flag = load_flag;
					depth_stencil_attachment.depth_end_flag = store_flag;

					SplitResourceLoadStoreFlags(pass_depth_stencil_target.stencil_load_store_flags, load_flag, store_flag);

					depth_stencil_attachment.stencil_begin_flag = load_flag;
					depth_stencil_attachment.stencil_end_flag = store_flag;

					RGTexture* texture_resource = rg.GetTextureResource(pass_depth_stencil_target.id.GetResourceID());
					depth_stencil_attachment.clear_value = texture_resource->desc.clear_value;

					render_pass_desc.depth_stencil_targerts = depth_stencil_attachment;
				}
				cmd_list->SetViewport(0, 0, static_cast<Float>(pass->width), static_cast<Float>(pass->height));

				cmd_list->BeginRenderPass(render_pass_desc);
				pass->Execute(render_graph_context);
				cmd_list->EndRenderPass();
			}
			else {
				pass->Execute(render_graph_context);
			}

			if (pass->signal_value != Uint64(-1)){
				cmd_list->End();
				if (pass->type == RGPassType::AsyncCompute)
				{
					cmd_list->Signal(execution_context.compute_fence, execution_context.compute_fence_value + pass->signal_value);
					execution_context.render_context->SetComputeFenceValue(execution_context.compute_fence_value + pass->signal_value);
				}
				else
				{
					cmd_list->Signal(execution_context.graphics_fence, execution_context.graphics_fence_value + pass->signal_value);
					execution_context.render_context->SetGraphicsFenceValue(execution_context.graphics_fence_value + pass->signal_value);
				}
				cmd_list->Submit();
				cmd_list->Begin();
			}

		}

		PostExecution(execution_context);
	}

	void RenderGraph::DependencyLevel::PreExecution(RenderGraphExecutionContext& execution_context)
	{
		//Create Textures And Views
		auto cmd_list = execution_context.graphics_command_list;
		auto compute_cmd_list = execution_context.compute_command_list;

		{
			for (auto& texture : texture_creates) {
				RGTexture* texture_resource = rg.GetTextureResource(texture);
				if (!texture_resource->imported)
				{
					texture_resource->resource = rg.resource_pool->AllocateTexture(texture_resource->desc, texture_resource->name);
					texture_resource->resource->SetName(texture_resource->name);
				}
				rg.CreateResourceView(texture);
			}

			for (auto& buffer : buffer_creates) {
				RGBuffer* buffer_resource = rg.GetBufferResource(buffer);
				if (!buffer_resource->imported) {
					buffer_resource->resource = rg.resource_pool->AllocateBuffer(buffer_resource->desc, buffer_resource->name);
					buffer_resource->resource->SetName(buffer_resource->name);
				}
				rg.CreateResourceView(buffer);
			}
		}

		
		for (auto& [texture_id, new_state] : texture_states) {
			RGTexture* texture_resource = rg.GetTextureResource(texture_id);
			const TextureDesc& texture_desc = texture_resource->desc;

			if (texture_creates.contains(texture_id)) {
				cmd_list->TextureBarrier(texture_resource->resource, texture_desc.intial_state, new_state, {});
				continue;
			}

			Bool found = false;
			for (Int32 level = level_index - 1; level >= 0; --level)
			{
				decltype(auto) prev_dep_level = rg.dependency_levels[level];
				if (prev_dep_level.texture_states.contains(texture_id)) {
					cmd_list->TextureBarrier(texture_resource->resource, prev_dep_level.texture_states[texture_id], new_state, {});
					found = true;
					break;
				}
			}

			if (!found && texture_resource->imported) {
				cmd_list->TextureBarrier(texture_resource->resource, texture_desc.intial_state, new_state, {});
			}

		}

		for (auto& [buffer_id, new_state] : buffer_states) {
			RGBuffer* buffer_resource = rg.GetBufferResource(buffer_id);
			const BufferDesc& buffer_desc = buffer_resource->desc;

			if (buffer_creates.contains(buffer_id)) {
				cmd_list->BufferBarrier(buffer_resource->resource, ResourceState::Common, new_state);
				continue;
			}

			Bool found = false;
			for (Int32 level = level_index - 1; level >= 0; --level)
			{
				decltype(auto) prev_dep_level = rg.dependency_levels[level];
				if (prev_dep_level.buffer_states.contains(buffer_id)) {
					cmd_list->BufferBarrier(buffer_resource->resource, prev_dep_level.buffer_states[buffer_id], new_state);
					found = true;
					break;
				}
			}

			if (!found && buffer_resource->imported) {
				cmd_list->BufferBarrier(buffer_resource->resource, ResourceState::Common, new_state);
			}
		}

		cmd_list->FlushBarriers();
	}

	void RenderGraph::DependencyLevel::PostExecution(RenderGraphExecutionContext& execution_context)
	{
		auto cmd_list = execution_context.graphics_command_list;
		for (auto& texture_id : texture_destroys) {
			RGTexture* texture = rg.GetTextureResource(texture_id);
			TextureDesc texture_desc = texture->desc;
			ResourceState state = texture_states[texture_id];
			cmd_list->TextureBarrier(texture->resource, state, texture_desc.intial_state);
			if(!texture->imported)
			{
				rg.resource_pool->ReleaseTexture(texture->resource);
			}
		}

		for (auto& buffer_id : buffer_destroys) {
			RGBuffer* buffer = rg.GetBufferResource(buffer_id);
			ResourceState state = buffer_states[buffer_id];
			BufferDesc buffer_desc = buffer->desc;
			cmd_list->BufferBarrier(buffer->resource, state, buffer_desc.initial_state);

			if(!buffer->imported)
			{
				rg.resource_pool->ReleaseBuffer(buffer->resource);
			}
		}		
		cmd_list->FlushBarriers();
	}

}

//todo:
//Culling Passes