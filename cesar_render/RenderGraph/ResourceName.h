// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.

#pragma once
#include "../../cesar_core/cesar_core.h"


#if _DEBUG
#define RG_NAME(name) RGResourceName(#name, crc64(#name)) 
#else 
#define RG_NAME(name) RGResourceName(crc64(#name)) 
#endif

namespace cesar::render_graph
{
	struct RenderGraphResourceName
	{
#ifdef _DEBUG
		RenderGraphResourceName(const Char* name, Uint64 hash)
			:name(name), hash(hash)
		{

		}

		RenderGraphResourceName()
			:hash(invalid_hash), name("unnamed rg resource")
		{

		}
#else
		RenderGraphResourceName(Uint64 hash)
			:hash(hash)
		{

		}

		RenderGraphResourceName()
			:hash(invalid_hash)
		{

		}
#endif // _DEBUG


		Uint64 hash;
#ifdef _DEBUG
		std::string name;

		operator const Char*()const {
			return name.c_str();
		}
#else
		operator const Char* ()const {
			return "";
		}
#endif // _DEBUG
		

		inline static Uint64 invalid_hash = Uint64(-1);

		Bool operator==(const RenderGraphResourceName& name1)const {
			return name1.hash == hash;
		}
	};
}

namespace std
{
	template <> struct hash<cesar::render_graph::RenderGraphResourceName>
	{
		cesar::Uint64 operator()(cesar::render_graph::RenderGraphResourceName const& res_name) const
		{
			return hash<decltype(res_name.hash)>()(res_name.hash);
		}
	};
}

namespace cesar {
	using RGResourceName = render_graph::RenderGraphResourceName;
}