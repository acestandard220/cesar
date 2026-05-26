#pragma once
#include "../cesar_core.h"


namespace cesar
{


	template<typename T>
	inline void CopyToMemoryBlock(MemoryBlock<T> block, void* source, Uint32 size) {
		memcpy(block.data(), source, size);
	}

	template<typename T>
	inline void CopyToMemoryBlock(MemoryBlock<T> block, void* source) {
		CopyToMemoryBlock(block, source, block.size_bytes());
	}

	template<typename T>
	concept ContiniousContainer = requires(T t) {
		t.data();
		t.size();
	};

	template<typename T, ContiniousContainer C>
	inline void CopyToMemoryBlock(MemoryBlock<T> block, C source) {
		CopyToMemoryBlock(block, source.data(), block.size_bytes());
	}

	template<typename T>
	class LinearAllocator {
	    private:
			class FreeList {
			    public:
			    	struct MemoryBlockRaw {
			    		uint32_t offset;
			    		uint32_t size; //byte size
			    	};
			    
			    	struct SpanCompare {
			    		bool operator()(const MemoryBlockRaw& a, const MemoryBlockRaw& b) const {
			    			return a.offset < b.offset;
			    		}
			    	};
			    
			    	void Optimize() {
			    		if (freelist.size() < 2) return;
			    
			    		std::set<MemoryBlockRaw, SpanCompare> merged;
			    		auto it = freelist.begin();
			    		MemoryBlockRaw current = *it;
			    		++it;
			    
			    		while (it != freelist.end()) {
			    			uint32_t currentEnd = current.offset + current.size;
			    			if (currentEnd == it->offset) {
			    				// Merge by creating a new MemoryBlockRaw with combined size
			    				current = MemoryBlockRaw{ current.offset, static_cast<uint32_t>(current.size + it->size) };
			    			}
			    			else {
			    				merged.insert(current);
			    				current = *it;
			    			  }
			    			++it;
			    		}
			    		merged.insert(current);
			    		freelist = std::move(merged);
			    	}
			    
			    	std::set<MemoryBlockRaw, SpanCompare> freelist;
			};
	    
	    public:
			LinearAllocator() 
				:element_size(CESAR_SIZEOF(T))
			{
				base_ptr = nullptr;
				top_ptr = nullptr;
				element_count = 0;

				expansion_rate = 1.5f;
			}

			~LinearAllocator() = default;

			Bool Reserve(Uint32 element_count)
			{
				if (base_ptr != nullptr) {
					this->element_count = element_count;
					T* new_ptr = (T*)realloc(base_ptr, CESAR_SIZEOF_BUFFER(T, element_count));
					
					Uint32 offset = top_ptr - base_ptr;
					base_ptr = new_ptr;
					top_ptr = base_ptr + offset;

					return new_ptr;
				}

				this->element_count = element_count;
				base_ptr = (T*)(malloc(CESAR_SIZEOF_BUFFER(T, element_count)));
				top_ptr = base_ptr;

				return base_ptr;
			}

			void Reset() {
				free(base_ptr);
				base_ptr = nullptr;
				top_ptr = nullptr;
				element_count = 0;
			}

			MemoryBlock<T> Allocate(Uint32 count) {
				size_t required_size = count * element_size;

				for (auto it = freelist.freelist.begin(); it != freelist.freelist.end(); ++it) {
					if (it->size >= required_size) {
						uint32_t ptr = it->offset;
						if (it->size == required_size) {
							freelist.freelist.erase(it);
						}
						else {
							typename FreeList::MemoryBlockRaw modified = { static_cast<uint32_t>(it->offset + required_size), static_cast<uint32_t>(it->size - required_size) };
							freelist.freelist.erase(it);
							freelist.freelist.insert(modified);
						}
						return MemoryBlock<T>((T*)base_ptr + ptr, static_cast<uint32_t>(required_size));
					}
				}

				Uint64 used = (top_ptr - base_ptr) * element_size;
				const Uint64 capacity = element_size * element_count;

				if (used + required_size > capacity) {
					auto expanded_required_count = GetNewExpansionSize(count);
					if (!Reserve(expanded_required_count) {
						CESAR_DEBUGBREAK();
						LOG_ERROR("Not enough memory for allocation.");
						return { (T*)top_ptr, 0 };
					}
				}

				void* returnPtr = top_ptr;
				top_ptr = (T*)((uint8_t*)top_ptr + required_size);
				used += required_size;
				MemoryBlock<T> returnSpan((T*)returnPtr, static_cast<uint32_t>(count));
				return returnSpan;
			}
	    


			inline Bool IsValid(MemoryBlock<T> block) {
				return (block.data() >= base_ptr && block.data() < top_ptr);
			}

			inline Bool IsValid(Uint32 index, Uint32 count) {
				return (base_ptr + index) < top_ptr;
			}

			Uint32 GetIndex(MemoryBlock<T> block) {
				T* block_ptr = block.data();
				CESAR_ASSERT(IsValid(block) && "Cannot get index of block that does not belong to pool.");
				return block_ptr - base_ptr;
			}

			Uint32 GetOffset()const {
				return static_cast<Uint32>(top_ptr - base_ptr);
			}

			MemoryBlock<T> GetMemoryBlock(Uint32 index, Uint32 count) {
				CESAR_ASSERT(IsValid(index, count) && "Cannot get MemoryBlock<T> of index that does not belong to pool.");
				return MemoryBlock(base_ptr + index, count);
			}

			MemoryBlock<T> GetMemoryPool() {
				const Uint32 count = top_ptr - base_ptr;
				return MemoryBlock(base_ptr, count);
			}

			Uint32 GetNewExpansionSize(Uint32 required_count)
			{
				CESAR_ASSERT(expansion_rate >= 1 && "Allocator Pool cannot be expanded below a factor of 1.0f. Possible loss of data.");
				Uint32 expanded_required_count = static_cast<Uint32>(required_count * expansion_rate);
				return element_count + expanded_required_count;
			}


			void SetExpansionRate(Float rate) { expansion_rate = rate; }
			Float GetExpansionRate()const { return expansion_rate; }

	    private:
			Float expansion_rate;

			T* base_ptr;
			T* top_ptr;

			Uint64 element_size;
			Uint32 element_count; // capacity

			FreeList freelist;
	};
}