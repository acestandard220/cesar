#pragma once
#include "../../cesar_render_core/Graphics/CommandList.h"
#include "Resource.h"
#include "ResourceID.h"
#include "ResourceName.h"

namespace cesar
{
	namespace render_graph
	{
		class RenderGraph;
		class RenderGraphPassBase;
		class _declspec(dllexport) RenderGraphContext
		{
		public:
			RenderGraphContext(RenderGraph& render_graph, RenderGraphPassBase& pass, CommandList& command_list)
				:render_graph(render_graph), pass(pass), command_list(command_list)
			{

			}
			~RenderGraphContext() = default;

			RGBuffer& GetReadWriteBuffer(BufferReadWrite buffer_id);
			RGBuffer& GetReadOnlyBuffer(BufferReadOnly buffer_id);

			RGBuffer& GetBufferResourceCopyDst(BufferCopyDstID buffer_id);
			RGBuffer& GetBufferResourceCopySrc(BufferCopySrcID buffer_id);

			RGTexture& GetTextureResourceCopyDst(TextureCopyDstID texture_id);
			RGTexture& GetTextureResourceCopySrc(TextureCopySrcID texture_id);

			RGBuffer& GetVertexBuffer(BufferVertexID buffer_id);
			RGBuffer& GetIndexBuffer(BufferIndexID index_id);

			Uint32 GetBufferReadOnlyIndex(BufferReadOnly buffer_id);
			Uint32 GetBufferReadWriteIndex(BufferReadWrite buffer_id);

			Descriptor GetBufferReadOnlyDescriptor(BufferReadOnly buffer_id);
			Descriptor GetBufferReadWriteDescriptor(BufferReadWrite buffer_id);

			Descriptor GetTextureReadOnlyDescriptor(TextureReadOnly texture_id);

			CommandList& GetCommandList() { return command_list; }
			void ClearCounterBuffer(BufferReadWrite buffer_id);

		private:
			RenderGraph& render_graph;
			CommandList& command_list;
			RenderGraphPassBase& pass;
		};
	}
	using RGContext = render_graph::RenderGraphContext;
}