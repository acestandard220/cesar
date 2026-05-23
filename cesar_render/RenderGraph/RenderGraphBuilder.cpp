// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "RenderGraphBuilder.h"

namespace cesar
{
	namespace render_graph
	{
		void RenderGraphBuilder::DummyReadTexture(RGResourceName name) {
			render_pass.texture_reads.insert(render_graph.GetTextureID(name));
		}

		void RenderGraphBuilder::DummyReadBuffer(RGResourceName name)
		{
			render_pass.buffer_reads.insert(render_graph.GetBufferID(name));
		}

		void RenderGraphBuilder::DummyWriteTexture(RGResourceName name)
		{
			render_pass.texture_writes.insert(render_graph.GetTextureID(name));
		}

		void RenderGraphBuilder::DummyWriteBuffer(RGResourceName name)
		{
			render_pass.buffer_writes.insert(render_graph.GetBufferID(name));
		}
		
		void RenderGraphBuilder::DeclareTexture(RGResourceName name, TextureDesc texture_desc)
		{
			TextureID texture_id = render_graph.DeclareTexture(name, texture_desc);
			render_pass.texture_creates.insert(texture_id);
		}

		void RenderGraphBuilder::DeclareBuffer(RGResourceName name, BufferDesc buffer_desc)
		{
			BufferID buffer_id = render_graph.DeclareBuffer(name, buffer_desc);
			render_pass.buffer_creates.insert(buffer_id);
		}

		void RenderGraphBuilder::WriteRenderTarget(RGResourceName name, ResourceLoadStoreFlags load_store_flags, const TextureViewDesc& view_desc)
		{
			RenderTargetID render_target_id = render_graph.RenderTarget(name, view_desc);
			TextureID texture_id = render_target_id.GetResourceID();
			render_pass.texture_writes.insert(texture_id);
			render_pass.render_targets.emplace_back(RGPassBase::RenderTargetInfo{ render_target_id,load_store_flags });
			render_pass.texture_state[texture_id] = ResourceState::RTV;
			if (!render_pass.texture_creates.contains(texture_id))
			{
				DummyReadTexture(name);
			}
		}

		void RenderGraphBuilder::WriteDepthStencilTarget(RGResourceName name, ResourceLoadStoreFlags depth_load_store_flags, ResourceLoadStoreFlags stencil_load_store_flags, const TextureViewDesc& view_desc)
		{
			DepthStencilID depth_stencil_id = render_graph.DepthStencilTarget(name, view_desc);
			TextureID texture_id = depth_stencil_id.GetResourceID();
			render_pass.texture_writes.insert(texture_id);
			render_pass.depth_targets = RGPassBase::DepthStencilTargetInfo{ depth_stencil_id,depth_load_store_flags,stencil_load_store_flags };
			render_pass.texture_state[texture_id] = ResourceState::DSV;
			if (!render_pass.texture_creates.contains(texture_id))
			{
				DummyReadTexture(name);
			}
		}

		void RenderGraphBuilder::WriteTexture(RGResourceName name, TextureViewDesc view_desc)
		{
			TextureReadWrite texture_read_write = render_graph.WriteTexture(name, view_desc);
			TextureID texture_id = texture_read_write.GetResourceID();
			render_pass.texture_writes.insert(texture_id);
			render_pass.texture_state[texture_id] = ResourceState::ComputeUAV;
			if (!render_pass.texture_creates.contains(texture_id))
			{
				DummyReadTexture(name);
			}
		}

		BufferReadWrite RenderGraphBuilder::WriteBuffer(RGResourceName name, BufferViewDesc view_desc)
		{
			BufferReadWrite buffer_read_write = render_graph.WriteBuffer(name, view_desc);
			BufferID buffer_id = buffer_read_write.GetResourceID();
			render_pass.buffer_writes.insert(buffer_id);
			render_pass.buffer_state[buffer_id] = ResourceState::ComputeUAV;

			if (!render_pass.buffer_creates.contains(buffer_id)) {
				DummyReadBuffer(name);
			}
			return buffer_read_write;
		}

		BufferCopyDstID RenderGraphBuilder::WriteBufferCopyDst(RGResourceName name)
		{
			BufferCopyDstID buffer_id = render_graph.WriteBufferCopyDst(name);
			render_pass.buffer_writes.insert(buffer_id);
			render_pass.buffer_state[buffer_id] = ResourceState::CopyDst;

			if (!render_pass.buffer_creates.contains(buffer_id)) {
				DummyReadBuffer(name);
			}
			return buffer_id;
		}

		BufferCopySrcID RenderGraphBuilder::ReadBufferCopySrc(RGResourceName name)
		{
			BufferCopySrcID buffer_id = render_graph.ReadBufferCopySrc(name);
			render_pass.buffer_reads.insert(buffer_id);
			render_pass.buffer_state[buffer_id] = ResourceState::CopySrc;
			return buffer_id;
		}

		TextureCopyDstID RenderGraphBuilder::WriteTextureCopyDst(RGResourceName name)
		{
			TextureCopyDstID texture_id = render_graph.WriteTextureCopyDst(name);
			render_pass.texture_writes.insert(texture_id);
			render_pass.texture_state[texture_id] = ResourceState::CopyDst;
			return texture_id;
		}

		TextureCopySrcID RenderGraphBuilder::ReadTextureCopySrc(RGResourceName name)
		{
			TextureCopySrcID texture_id = render_graph.ReadTextureCopySrc(name);
			render_pass.texture_reads.insert(texture_id);
			render_pass.texture_state[texture_id] = ResourceState::CopySrc;
			return texture_id;
		}

		BufferVertexID RenderGraphBuilder::ReadVertexBuffer(RGResourceName name)
		{
			BufferVertexID buffer_id = render_graph.ReadVertexBuffer(name);
			render_pass.buffer_reads.insert(buffer_id);
			render_pass.buffer_state[buffer_id] = ResourceState::VertexBuffer;
			return buffer_id;
		}

		BufferIndexID RenderGraphBuilder::ReadIndexBuffer(RGResourceName name) {
			BufferIndexID buffer_id = render_graph.ReadIndexBuffer(name);
			render_pass.buffer_reads.insert(buffer_id);
			render_pass.buffer_state[buffer_id] = ResourceState::IndexBuffer;
			return buffer_id;
		}

		BufferConstantID RenderGraphBuilder::ReadConstantBuffer(RGResourceName name) {
			BufferConstantID buffer_id = render_graph.ReadConstantBuffer(name);
			render_pass.buffer_reads.insert(buffer_id);
			render_pass.buffer_state[buffer_id] = ResourceState::GenericRead;
			return buffer_id;
		}

		TextureReadOnly RenderGraphBuilder::ReadTexture(RGResourceName name, ReadAccessType read_access, const TextureViewDesc& view_desc)
		{
			TextureReadOnly texture_read_only = render_graph.ReadTexture(name, view_desc);
			TextureID texture_id = render_graph.GetTextureID(name);

			render_pass.texture_reads.insert(texture_id);
			switch (read_access) {
			    case ReadAccessType::PixelShader: {
			    	render_pass.texture_state[texture_id] = ResourceState::PixelSRV;
			    	break;
			    }
			    case ReadAccessType::NonPixelShader: {
			    	render_pass.texture_state[texture_id] = ResourceState::ComputeSRV;
			    	break;
			    }
			    case ReadAccessType::AllShader: {
			    	render_pass.texture_state[texture_id] = ResourceState::AllShading;
			    	break;
			    }
			}
			return texture_read_only;
		}

		BufferReadOnly RenderGraphBuilder::ReadBuffer(RGResourceName name, ReadAccessType read_access, const BufferViewDesc& view_desc)
		{
			BufferReadOnly buffer_read_only = render_graph.ReadBuffer(name, view_desc);
			BufferID  buffer_id = buffer_read_only.GetResourceID();

			render_pass.buffer_reads.insert(buffer_id);
			switch (read_access) {
			    case ReadAccessType::PixelShader:
			    {
			    	render_pass.buffer_state[buffer_id] = ResourceState::PixelSRV;
			    	break;
			    }
			    case ReadAccessType::NonPixelShader: {
			    	render_pass.buffer_state[buffer_id] = ResourceState::ComputeSRV;
			    	break;
			    }
			    case ReadAccessType::AllShader: {
			    	render_pass.buffer_state[buffer_id] = ResourceState::AllShading;
			    	break;
			    }
			}
			return buffer_read_only;
		}
		
		void RenderGraphBuilder::SetViewport(Uint32 width, Uint32 height)
		{
			render_pass.width = width;
			render_pass.height = height;
		}
	}
}
