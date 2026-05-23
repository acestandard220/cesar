#include "RenderGraphContext.h"
#include "RenderGraph.h"

namespace cesar
{
    namespace render_graph
    {
        RGBuffer& RenderGraphContext::GetReadWriteBuffer(BufferReadWrite buffer_id)
        {
            return *(render_graph.GetBufferResource(buffer_id.GetResourceID()));
        }
        RGBuffer& RenderGraphContext::GetReadOnlyBuffer(BufferReadOnly buffer_id)
        {
            return *(render_graph.GetBufferResource(buffer_id.GetResourceID()));
        }
        RGBuffer& RenderGraphContext::GetBufferResourceCopyDst(BufferCopyDstID buffer_id)
        {
            RGBuffer* buffer_resource = render_graph.GetBufferResource(buffer_id);
            return *buffer_resource;
        }
        RGBuffer& RenderGraphContext::GetBufferResourceCopySrc(BufferCopySrcID buffer_id)
        {
            RGBuffer* buffer_resource = render_graph.GetBufferResource(buffer_id);
            return *buffer_resource;
        }
        RGTexture& RenderGraphContext::GetTextureResourceCopyDst(TextureCopyDstID texture_id)
        {
            return *render_graph.GetTextureResource(texture_id.id);
        }
        RGTexture& RenderGraphContext::GetTextureResourceCopySrc(TextureCopySrcID texture_id)
        {
            return *render_graph.GetTextureResource(texture_id.id);
        }

        RGBuffer& RenderGraphContext::GetVertexBuffer(BufferVertexID vertex_id)
        {
            return *render_graph.GetBufferResource(vertex_id.id);
        }

        RGBuffer& RenderGraphContext::GetIndexBuffer(BufferIndexID index_id)
        {
            return *render_graph.GetBufferResource(index_id.id);
        }
        Uint32 RenderGraphContext::GetBufferReadOnlyIndex(BufferReadOnly buffer_id)
        {
            return render_graph.GetBufferReadOnlyIndex(buffer_id);
        }
        Uint32 RenderGraphContext::GetBufferReadWriteIndex(BufferReadWrite buffer_id)
        {
            return render_graph.GetBufferReadWriteIndex(buffer_id);
        }
        Descriptor RenderGraphContext::GetBufferReadOnlyDescriptor(BufferReadOnly buffer_id)
        {
            return render_graph.GetBufferReadOnlyDescriptor(buffer_id);
        }
        Descriptor RenderGraphContext::GetBufferReadWriteDescriptor(BufferReadWrite buffer_id)
        {
            return render_graph.GetBufferReadWriteDescriptor(buffer_id);
        }
        Descriptor RenderGraphContext::GetTextureReadOnlyDescriptor(TextureReadOnly texture_id)
        {
            return render_graph.GetTextureReadOnlyDescriptor(texture_id);
        }

        void RenderGraphContext::ClearCounterBuffer(BufferReadWrite buffer_id)
        {
            Buffer* _buffer = render_graph.GetBufferResource(buffer_id.GetResourceID())->resource;
            Descriptor _descriptor = GetBufferReadWriteDescriptor(buffer_id);
            const Uint32 clear_values[4] = { 0,0 ,0 ,0 };

            command_list.ClearUAVUint(_buffer, _descriptor, clear_values);
            command_list.GlobalBarrier(ResourceState::ComputeUAV, ResourceState::ComputeUAV);
            command_list.FlushBarriers();
        }
    }
}
