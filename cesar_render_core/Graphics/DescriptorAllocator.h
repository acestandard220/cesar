// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../cesar_core/cesar_core.h"
#include "Descriptor.h"
#include "DescriptorHeap.h"

namespace cesar
{
	class _declspec(dllexport) DescriptorAllocator
	{
		struct DescriptorRange
		{
			Descriptor start;
			Descriptor end;
		};
	public:
		DescriptorAllocator(std::unique_ptr<DescriptorHeap> heap);
		~DescriptorAllocator();

		Descriptor Allocate();
		void FreeDescriptor(Descriptor descriptor);
		void FreeDescriptor(Uint32 index);
		void ClearDescriptorHeap();

		DescriptorHeap* GetHeap() { return heap.get(); }

	private:
		std::unique_ptr<DescriptorHeap> heap;
		std::list<DescriptorRange> freelist;
	};

}