#pragma once
#include "../../cesar_core/Allocator/RingAllocator.h"

#include "DescriptorHeap.h"

namespace cesar
{
	class DescriptorRingAllocator
	{
	public:
		DescriptorRingAllocator(std::unique_ptr<DescriptorHeap> descriptor_heap, Uint32 reserved)
			:descriptor_heap(std::move(descriptor_heap)), ring_allocator(this->descriptor_heap->GetCapacity(), reserved)
		{}

		Descriptor Allocate()
		{
			Uint32 index = ring_allocator.Allocate(1);
			if (!ring_allocator.IsValid(index))
				CESAR_ASSERT("Descriptor Heap is probably full.");

			return descriptor_heap->GetDescriptor(index);
		}

		void FinishCurrentFrame(Uint32 frame) {
			ring_allocator.FinishCurrentFrame(frame);
		}

		void ReleaseFinishedCompletedFrames(Uint32 completed_frame_value) {
			ring_allocator.ReleaseCompletedFrame(completed_frame_value);
		}

		DescriptorHeap* GetHeap() { return descriptor_heap.get(); }
		Uint32 GetReserved()const { return ring_allocator.GetReserved(); }
		Uint32 GetCapacity()const { return ring_allocator.GetCapacity(); }
	private:
		std::unique_ptr<DescriptorHeap> descriptor_heap;
		RingAllocator ring_allocator;
	};

	using OnlineDescriptorAllocator = DescriptorRingAllocator;
}