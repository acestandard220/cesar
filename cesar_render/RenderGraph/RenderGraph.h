// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.

#pragma once
#include "../../cesar_core/cesar_core.h"
#include "ResourcePool.h"
#include "ResourceID.h"
#include "ResourceName.h"
#include "Resource.h"
#include "RenderGraphPass.h"
#include "../FrameData.h"

#include <unordered_set>

namespace cesar {

	namespace render_graph
	{

		class RenderGraphBuilder;
		class RenderGraphContext;
		class RenderGraph {
			friend class RenderGraphContext;
			struct RenderGraphExecutionContext {
				RenderContext* render_context = nullptr;
				
				CommandList* graphics_command_list = nullptr;
				CommandList* compute_command_list = nullptr;
				
				Fence* graphics_fence = nullptr;
				Fence* compute_fence = nullptr;

				Uint32 graphics_fence_value;
				Uint32 compute_fence_value;
			};

			class DependencyLevel {
				friend RenderGraph;
				public:
					DependencyLevel(RenderGraph& render_graph, Uint32 level_index)
						: rg(render_graph), level_index(level_index) 
					{
					
					}

					void AddPass(RGPassBase* pass);
					void SetUp();
					void Execute(RenderGraphExecutionContext& execution_context);

					void PreExecution(RenderGraphExecutionContext& execution_context);
					void PostExecution(RenderGraphExecutionContext& execution_context);

			    private:
			    	RenderGraph& rg;
			    	Uint32 level_index = 0;

					std::vector<RGPassBase*> passes;
			    
			    	std::unordered_set<TextureID> texture_reads;
			    	std::unordered_set<TextureID> texture_writes;
			    	std::unordered_set<TextureID> texture_creates;
			    	std::unordered_set<TextureID> texture_destroys;
					std::unordered_map<TextureID, ResourceState> texture_states;

			    	std::unordered_set<BufferID> buffer_reads;
			    	std::unordered_set<BufferID> buffer_writes;
			    	std::unordered_set<BufferID> buffer_creates;
			    	std::unordered_set<BufferID> buffer_destroys;
					std::unordered_map<BufferID, ResourceState> buffer_states;
			};

		public:
			RenderGraph(RenderContext* render_context, ResourcePool* resource_pool);
			~RenderGraph();

			void Compile();
			void Execute();

			void CleanUp();

			template<typename PassData, typename... Args>
				requires std::is_constructible<RenderGraphPass<PassData>, Args...>::value
			void AddPass(Args&&... args) {
				const Uint32 new_pass_id = passes.size();
				passes.emplace_back(std::make_unique<RenderGraphPass<PassData>>(std::forward<Args>(args)...));
				decltype(auto) inserted_pass = passes.back();
				inserted_pass->id = new_pass_id;

				RenderGraphBuilder builder(*this, *inserted_pass.get());
				inserted_pass->Setup(builder);
			}

			TextureID ImportTexture(RGResourceName name, Texture* texture);
			BufferID ImportBuffer(RGResourceName name, Buffer* buffer);

			TextureID DeclareTexture(RenderGraphResourceName name, const TextureDesc& texture_desc);
			BufferID  DeclareBuffer(RenderGraphResourceName name, const BufferDesc& buffer_desc);

			RenderTargetID RenderTarget(RGResourceName name, const TextureViewDesc& desc);
			DepthStencilID DepthStencilTarget(RGResourceName name, const TextureViewDesc& desc);

			TextureReadWrite WriteTexture(RGResourceName name, TextureViewDesc desc);
			TextureReadOnly ReadTexture(RGResourceName name, const TextureViewDesc& desc);
			BufferReadWrite WriteBuffer(RGResourceName name, BufferViewDesc desc);
			BufferReadOnly ReadBuffer(RGResourceName name,const BufferViewDesc& desc);

			BufferCopyDstID WriteBufferCopyDst(RGResourceName name);
			BufferCopySrcID ReadBufferCopySrc(RGResourceName name);

			TextureCopyDstID WriteTextureCopyDst(RGResourceName name);
			TextureCopySrcID ReadTextureCopySrc(RGResourceName name);

			BufferVertexID ReadVertexBuffer(RGResourceName name);
			BufferIndexID ReadIndexBuffer(RGResourceName name);

			BufferConstantID ReadConstantBuffer(RGResourceName name);

			Bool IsTextureHandleValid(TextureID texture_id);
			Bool IsBufferHandleValid(BufferID buffer_id);

			RGTexture* GetTextureResource(TextureID texture_id);
			RGBuffer* GetBufferResource(BufferID buffer_id);

			TextureID GetTextureID(RGResourceName name);
			BufferID GetBufferID(RGResourceName name);

			Uint32 GetBufferReadOnlyIndex(BufferReadOnly buffer_id);
			Uint32 GetBufferReadWriteIndex(BufferReadWrite buffer_id);
			
		private:
			void BuildAdjacencyList();
			void TopologicalSort();
			void BuildDependencyLevels();
			void ResolveAsync();
			void CalculateResourceLifeTime();

			void CreateResourceView(TextureID texture_id);
			void CreateResourceView(BufferID buffer_id);

			Descriptor GetRenderTargetDescriptor(RenderTargetID id);
			Descriptor GetDepthStencilTargetDescriptor(DepthStencilID id);

			Descriptor GetBufferReadWriteDescriptor(BufferReadWrite id);
			Descriptor GetBufferReadOnlyDescriptor(BufferReadOnly id);

			Descriptor GetTextureReadOnlyDescriptor(TextureReadOnly id);

		private:
			RenderContext* render_context;
			
			

			std::vector<std::unique_ptr<RenderGraphPassBase>> passes;

			std::vector<std::vector<Uint64>> adj_list;
			std::vector<Uint64> topologically_sorted_list;
			std::vector<DependencyLevel> dependency_levels;

			ResourcePool* resource_pool;

			std::vector<std::unique_ptr<RGTexture>> textures;
			std::vector<std::unique_ptr<RGBuffer>> buffers;

			std::unordered_map<RGResourceName, TextureID> texture_name_id_map;
			std::unordered_map<RGResourceName, BufferID> buffer_name_id_map;

			std::unordered_map<TextureID, std::vector<std::pair<TextureViewDesc, ResourceDescriptorType>>> texture_view_desc_map;
			std::unordered_map<BufferID, std::vector<std::pair<BufferViewDesc, ResourceDescriptorType>>> buffer_view_desc_map;

			std::unordered_map<TextureID, std::vector<Descriptor>> texture_desc_map;
			std::unordered_map<BufferID, std::vector<Descriptor>> buffer_desc_map;

			std::vector<Descriptor> _texture_views_to_destroy;
		};
	}
}