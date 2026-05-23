// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../../cesar_core/cesar_core.h"
#include "ResourceID.h"
#include "ResourceName.h"

namespace cesar {

	namespace render_graph
	{
		class RenderGraphPassBase;
		struct RenderGraphResource
		{
			RenderGraphResource(Uint64 id, Bool imported, RenderGraphResourceName& name)
				:id(id), imported(imported), name(name)
			{

			}

			Uint64 id = 0;
			Bool imported;

			RenderGraphPassBase* owner = nullptr;
			RenderGraphPassBase* last_writer = nullptr;
			RenderGraphResourceName name;
		};

		template<RenderGraphResourceType type>
		struct ResourceTypeTrait;

		template<>
		struct ResourceTypeTrait<RenderGraphResourceType::Buffer>
		{
			using Resource = Buffer;
			using ResourceDesc = BufferDesc;
		};

		template<>
		struct ResourceTypeTrait<RenderGraphResourceType::Texture>
		{
			using Resource = Texture;
			using ResourceDesc = TextureDesc;
		};

		template<RenderGraphResourceType type>
		struct TypedResource : RenderGraphResource {
			using Resource = typename ResourceTypeTrait<type>::Resource;
			using ResourceDesc = typename ResourceTypeTrait<type>::ResourceDesc;

			TypedResource(Uint64 id, Resource* resource, RenderGraphResourceName& name)
				:RenderGraphResource(id, true, name), resource(resource), desc(resource->GetDesc())
			{

			}
			TypedResource(Uint64 id, const ResourceDesc& desc, RenderGraphResourceName& name)
				:RenderGraphResource(id, false, name), desc(desc), resource(nullptr)
			{

			}

			operator Resource* () const
			{
				return resource;
			}

			Resource* resource;
			ResourceDesc desc;
		};

		using RGTexture = TypedResource<RenderGraphResourceType::Texture>;
		using RGBuffer = TypedResource<RenderGraphResourceType::Buffer>;
	}

}