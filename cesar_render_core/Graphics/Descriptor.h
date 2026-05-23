// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../../cesar_core/cesar_core.h"

namespace cesar
{
	class DescriptorHeap;
	struct Descriptor
	{
		DescriptorHeap* parent_heap = nullptr;
		cesar::Uint32 index = cesar::Uint32(-1);

		void Increment(cesar::Uint32 increment)
		{
			index += increment;
		}

		cesar::Bool operator==(const Descriptor& other)const
		{
			return (parent_heap == other.parent_heap && index == other.index);
		}

		cesar::Bool IsValid() const
		{
			return parent_heap;
		}
	};
}