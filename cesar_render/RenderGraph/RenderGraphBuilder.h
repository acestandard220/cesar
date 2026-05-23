// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../../cesar_core/cesar_core.h"
#include "RenderGraph.h"
#include "RenderGraphPass.h"

namespace cesar {

	namespace render_graph
	{

		class _declspec(dllexport) RenderGraphBuilder {
		public:
			CESAR_NO_DEFAULT_CONSTRUCTOR(RenderGraphBuilder);
			CESAR_NONCOPYABLE(RenderGraphBuilder);


			void DeclareTexture(RGResourceName name, TextureDesc texture_desc);
			void DeclareBuffer(RGResourceName name, BufferDesc buffer_desc);

			void WriteRenderTarget(RGResourceName name, ResourceLoadStoreFlags load_store_flags, const TextureViewDesc& view_desc);
			void WriteDepthStencilTarget(RGResourceName name, ResourceLoadStoreFlags depth_load_store_flags, ResourceLoadStoreFlags stencil_load_store_flags, const TextureViewDesc& view_desc);

			void WriteTexture(RGResourceName name, TextureViewDesc view_desc);
			BufferReadWrite WriteBuffer(RGResourceName name, BufferViewDesc view_desc);

			BufferCopyDstID WriteBufferCopyDst(RGResourceName name);
			BufferCopySrcID ReadBufferCopySrc(RGResourceName name);

			TextureCopyDstID WriteTextureCopyDst(RGResourceName name);
			TextureCopySrcID ReadTextureCopySrc(RGResourceName name);

			BufferVertexID ReadVertexBuffer(RGResourceName name);
			BufferIndexID ReadIndexBuffer(RGResourceName name);
			BufferConstantID ReadConstantBuffer(RGResourceName name);

			TextureReadOnly ReadTexture(RGResourceName name, ReadAccessType read_access, const TextureViewDesc& view_desc);
			BufferReadOnly ReadBuffer(RGResourceName name, ReadAccessType access_type, const BufferViewDesc& view_desc);

			void SetViewport(Uint32 width, Uint32 height);
		private:
			friend RenderGraph;
			RenderGraphBuilder(RenderGraph& render_graph, RenderGraphPassBase& render_pass)
				:render_graph(render_graph), render_pass(render_pass)
			{
			
			}

			void DummyReadTexture(RGResourceName name);
			void DummyReadBuffer(RGResourceName name);

			void DummyWriteTexture(RGResourceName name);
			void DummyWriteBuffer(RGResourceName name);	

		private:
			RenderGraph& render_graph;
			RenderGraphPassBase& render_pass;
		};
	}

	using RGBuilder = render_graph::RenderGraphBuilder;
}