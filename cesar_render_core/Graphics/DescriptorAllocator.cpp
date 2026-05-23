// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "DescriptorAllocator.h"

namespace cesar
{
	using namespace cesar;
	DescriptorAllocator::DescriptorAllocator(std::unique_ptr<DescriptorHeap> _heap)
		:heap(std::move(_heap))
	{
		DescriptorRange range{ 
			heap->GetDescriptor(0), 
			heap->GetDescriptor(heap->GetCapacity() - 1) 
		};

		freelist.push_back(range);	
	}

	DescriptorAllocator::~DescriptorAllocator()
	{
		
	}

	Descriptor DescriptorAllocator::Allocate()
	{
		if (freelist.empty())
		{
			LOG_ERROR("Descriptor Heap limit has been reached");
			return Descriptor();
		}

		DescriptorRange& range = freelist.front();
		Descriptor descriptor = range.start;
		range.start.Increment(1);
		if (range.start == range.end)
		{
			freelist.pop_front();
		}
		return descriptor;		
	}

	void DescriptorAllocator::FreeDescriptor(Descriptor handle)
	{
		Descriptor next_handle = handle;
		next_handle.Increment(1);

		DescriptorRange new_range{ .start = handle, .end = next_handle };
		Bool merged = false;
		for (auto it = freelist.begin(); it != freelist.end(); ++it)
		{
			if (it->end == handle)
			{
				it->end.Increment(1);
				auto next_it = std::next(it);
				if (next_it != freelist.end() && next_it->start == it->end)
				{
					it->end = next_it->end;
					freelist.erase(next_it);
				}
				merged = true;
				break;
			}
			else if (it->start == next_handle)
			{
				it->start = handle;
				merged = true;
				break;
			}
			else if (it->start.index > handle.index)
			{
				freelist.insert(it, new_range);
				merged = true;
				break;
			}
		}

		if (!merged)
		{
			freelist.push_back(new_range);
		}

	}
	void DescriptorAllocator::FreeDescriptor(Uint32 index)
	{
		Descriptor descriptor;
		descriptor.index = index;
		descriptor.parent_heap = this->heap.get();
		FreeDescriptor(descriptor);
	}
	void DescriptorAllocator::ClearDescriptorHeap()
	{
		freelist.clear();
		DescriptorRange range{
			heap->GetDescriptor(0),
			heap->GetDescriptor(heap->GetCapacity() - 1)
		};

		freelist.push_back(range);
	}
}
