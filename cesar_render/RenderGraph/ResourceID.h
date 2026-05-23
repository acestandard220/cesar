// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../../cesar_core/cesar_core.h"

namespace cesar
{

	namespace render_graph {

		enum class RenderGraphResourceType : Uint8 {
			Texture,
			Buffer
		};

		struct ResourceID {
			inline static Uint32 invalid_id = Uint32(-1);

			ResourceID() : id(invalid_id) {}
			ResourceID(Uint32 id) : id(id) {}

			Uint32 id;

			Bool operator==(const ResourceID& other)const {
				return id == other.id;
			}
		};

		template<RenderGraphResourceType T>
		struct TypedResourceID : public ResourceID {
			TypedResourceID() : ResourceID() {}
			TypedResourceID(Uint32 id) : ResourceID(id) {}

			Bool IsValid()const {
				return id != invalid_id;
			}
		};

		using BufferID = TypedResourceID<RenderGraphResourceType::Buffer>;
		using TextureID = TypedResourceID<RenderGraphResourceType::Texture>;

		struct ResourceDescriptorID {
			inline static Uint64 invalid_id = Uint64(-1);

			ResourceDescriptorID() :id(invalid_id) {};
			ResourceDescriptorID(Uint64 descriptor_idx, ResourceID resource_id)
			{
				id = (static_cast<Uint64>(descriptor_idx) << 32) | static_cast<Uint32>(resource_id.id);
			}

		    Uint64 id;
		};

		enum class ResourceDescriptorType {
			RenderTarget,
			DepthStencil,

			ReadOnly,
			ReadWrite
		};

		template<ResourceDescriptorType type, RenderGraphResourceType T>
		struct TypedResourceDescriptorID :public ResourceDescriptorID {
			TypedResourceDescriptorID() :ResourceDescriptorID() {}
			TypedResourceDescriptorID(Uint64 _id, ResourceID resource_id) :ResourceDescriptorID(_id, resource_id) {}

			auto GetResourceID()const { 
				if constexpr (T == RenderGraphResourceType::Buffer)  return BufferID(static_cast<Uint32>(id));
				else if constexpr (T == RenderGraphResourceType::Texture)  return TextureID(Uint32(id)); 
			}

			Uint64 GetViewID() const {
				return (id >> 32);
			}
		};

		using RenderTargetID = TypedResourceDescriptorID<ResourceDescriptorType::RenderTarget, RenderGraphResourceType::Texture>;
		using DepthStencilID = TypedResourceDescriptorID<ResourceDescriptorType::DepthStencil, RenderGraphResourceType::Texture>;

		using TextureReadOnly = TypedResourceDescriptorID<ResourceDescriptorType::ReadOnly, RenderGraphResourceType::Texture>;
		using TextureReadWrite = TypedResourceDescriptorID<ResourceDescriptorType::ReadWrite, RenderGraphResourceType::Texture>;

		using BufferReadOnly = TypedResourceDescriptorID<ResourceDescriptorType::ReadOnly, RenderGraphResourceType::Buffer>;
		using BufferReadWrite = TypedResourceDescriptorID<ResourceDescriptorType::ReadWrite, RenderGraphResourceType::Buffer>;


		enum class ResourceModes {
			CopyDst,
			CopySrc,
			VertexBuffer,
			IndexBuffer,
			Constant
		};

		template<ResourceModes mode>
		struct TextureResourceModeID : public TextureID
		{
			using TextureID::TextureID;
		};

		template<ResourceModes mode>
		struct BufferResourceModeID :public BufferID {
			using BufferID::BufferID;
		};

		using TextureCopyDstID = TextureResourceModeID<ResourceModes::CopyDst>;
		using TextureCopySrcID = TextureResourceModeID<ResourceModes::CopySrc>;

		using BufferCopyDstID = BufferResourceModeID<ResourceModes::CopyDst>;
		using BufferCopySrcID = BufferResourceModeID<ResourceModes::CopySrc>;

		using BufferConstantID = BufferResourceModeID<ResourceModes::Constant>;

		using BufferVertexID = BufferResourceModeID<ResourceModes::VertexBuffer>;
		using BufferIndexID  = BufferResourceModeID<ResourceModes::IndexBuffer>;
		

	}

}

namespace std
{
	template<cesar::render_graph::RenderGraphResourceType T>
	struct hash<cesar::render_graph::TypedResourceID<T>>
	{
		size_t operator()(const cesar::render_graph::TypedResourceID<T>& id) const noexcept
		{
			return hash<cesar::Uint32>()(id.id);
		}
	};
}