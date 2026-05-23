#pragma once
#include "../Core/Types.h"
#include <queue>

namespace cesar {

	class RingAllocator {
		struct FrameEntry {
			Uint32 frame_index;
			Uint32 tail;
			Uint32 allocation_size;
		};
	public:
		RingAllocator(Uint32 capacity, Uint32 reserved)
			:head(0), tail(0), reserved(reserved), capacity(capacity-reserved), used(reserved), current_frame_size(0)
		{

		}
		~RingAllocator() {};

		Uint32 Allocate(Uint32 size) {
			if (IsFull()) {
				return INVALID_INDEX;
			}
            
			if (tail >= head) {
				if (tail + size <= capacity)
				{
					Uint32 offset = tail;
					tail += size;
					current_frame_size += size;
					used += size;
					return offset + reserved;
				}
				else if (size < head) {
					Uint32 add_size = (capacity - tail) + size;
					used += add_size;
					tail = size;
					current_frame_size += add_size;
					return 0 + reserved;
				}
			}
			else if (tail + size <= head) {
				Uint32 offset = tail;
				tail += size;
				current_frame_size += size;
				return offset + reserved;
			}
		}

		void FinishCurrentFrame(Uint32 frame) {

			completed_frames.push({
				.frame_index = frame,
				.tail = tail,
				.allocation_size = current_frame_size
				});

			current_frame_size = 0;
		}

		void ReleaseCompletedFrame(Uint32 frame) {
			while (completed_frames.size() && completed_frames.front().frame_index <= frame)
			{
				FrameEntry& frame_entry = completed_frames.front();
				used -= frame_entry.allocation_size;
				head = frame_entry.tail;
				completed_frames.pop();
			}
		}

		Bool IsFull()const { return capacity == used; }
		Bool IsValid(Uint32 slot)const { return slot != INVALID_INDEX; }

		Uint32 GetReserved()const { return reserved; }
		Uint32 GetCapacity()const { return capacity; }
	private:
		std::queue<FrameEntry> completed_frames;
		Uint32 reserved;

		Uint32 head;
		Uint32 tail;

		Uint32 capacity;
		Uint32 used;
		Uint32 current_frame_size;
		constexpr static Uint32 INVALID_INDEX = UINT32_MAX;
	};

}