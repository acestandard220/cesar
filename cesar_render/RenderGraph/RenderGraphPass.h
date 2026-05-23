// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../../cesar_core/cesar_core.h"
#include "../../cesar_render_core/Graphics/Resource.h"
#include "../../cesar_render_core/Graphics/Descriptor.h"
#include "../../cesar_render_core/Graphics/CommandList.h"
#include "RenderGraphContext.h"

#include "ResourceID.h"

namespace cesar {
	// 0000 0000

	namespace render_graph {

		enum class RenderGraphPassType : Uint8
		{
			Graphics,
			Compute,
			Copy,
			AsyncCompute
		};

		enum class RenderGraphPassFlags :Uint8 {
			None = 0,
			ForceNoCull,
			UseLegacyRenderPass,
		};

		inline constexpr Uint8 CombineResourceLoadStoreFlags(LoadResourceFlag load_flag, StoreResourceFlag store_flag)
		{
			return (static_cast<Uint8>(load_flag) << 2) | static_cast<Uint8>(store_flag);
		}
		enum class ResourceLoadStoreFlags : Uint8
		{
			Discard = CombineResourceLoadStoreFlags(LoadResourceFlag::Discard, StoreResourceFlag::Discard),
			Preserve = CombineResourceLoadStoreFlags(LoadResourceFlag::Preserve, StoreResourceFlag::Preserve),
			PreserveDiscard = CombineResourceLoadStoreFlags(LoadResourceFlag::Preserve, StoreResourceFlag::Discard),
			DiscardPreserve = CombineResourceLoadStoreFlags(LoadResourceFlag::Discard, StoreResourceFlag::Preserve),
			ClearPreserve = CombineResourceLoadStoreFlags(LoadResourceFlag::Clear, StoreResourceFlag::Preserve),
			ClearDiscard = CombineResourceLoadStoreFlags(LoadResourceFlag::Clear, StoreResourceFlag::Discard)
		};

		inline void SplitResourceLoadStoreFlags(ResourceLoadStoreFlags load_store_flag, LoadResourceFlag& load_flag, StoreResourceFlag& store_flag)
		{
			Uint8 combined = static_cast<Uint8>(load_store_flag);
			load_flag = static_cast<LoadResourceFlag>(combined >> 2);
			store_flag = static_cast<StoreResourceFlag>(combined & 0b11);
		}

		class RenderGraphBuilder;
		class RenderGraphPassBase {



			struct RenderTargetInfo {
				RenderTargetID id;
				ResourceLoadStoreFlags load_store_flags;
			};

			struct DepthStencilTargetInfo {
				DepthStencilID id;
				ResourceLoadStoreFlags depth_load_store_flags;
				ResourceLoadStoreFlags stencil_load_store_flags;
			};


		public:
			RenderGraphPassBase(const Char* name, RenderGraphPassType type, RenderGraphPassFlags flags)
				: name(name), ref_count(1), type(type), flags(flags)
			{

			}
			~RenderGraphPassBase() {}

			virtual void Setup(RenderGraphBuilder&) = 0;
			virtual void Execute(RenderGraphContext&) = 0;

			Bool CanBeCulled()const { return !HasFlag(flags, RenderGraphPassFlags::ForceNoCull); }

		private:
			friend class RenderGraph;
			friend class RenderGraphBuilder;
			friend class RenderGraphContext;

			std::string name;
			Uint32 ref_count;
			Uint32 id;

			RenderGraphPassType type;
			RenderGraphPassFlags flags;

			std::unordered_set<TextureID> texture_writes;
			std::unordered_set<TextureID> texture_reads;
			std::unordered_set<TextureID> texture_creates;
			std::unordered_set<TextureID> texture_destroys;
			std::unordered_map<TextureID, ResourceState> texture_state;

			std::unordered_set<BufferID> buffer_writes;
			std::unordered_set<BufferID> buffer_reads;
			std::unordered_set<BufferID> buffer_creates;
			std::unordered_set<BufferID> buffer_destroys;
			std::unordered_map<BufferID, ResourceState> buffer_state;

			std::vector<RenderTargetInfo> render_targets;
			std::optional<DepthStencilTargetInfo> depth_targets;

			Uint32 width; Uint32 height;

			Uint64 wait_value = Uint64(-1);
			Uint64 signal_value = Uint64(-1);
		};

		template<typename PassData>
		class RenderGraphPass : public RenderGraphPassBase
		{
			using SetupFunc = std::function<void(PassData&, RenderGraphBuilder&)>;
			using ExecuteFunc = std::function<void(PassData&, RenderGraphContext&)>;


		public:
			RenderGraphPass(const Char* name, RenderGraphPassType pass_type, RenderGraphPassFlags pass_flags, SetupFunc&& setup_func, ExecuteFunc&& execute_func)
				:RenderGraphPassBase(name, pass_type, pass_flags), setup_func(std::move(setup_func)), execute_func(std::move(execute_func))
			{

			}

			PassData& GetPass(){
				return pass_data;
			}

		private:
			void Setup(RenderGraphBuilder& render_graph_builder)override {
				CESAR_ASSERT(setup_func != nullptr && "Setup function cannot be null");
				setup_func(pass_data, render_graph_builder);
			}

			void Execute(RenderGraphContext& render_graph_context)override {
				CESAR_ASSERT(execute_func != nullptr && "Execute Function cannot be null");
				execute_func(pass_data, render_graph_context);
			}
			PassData pass_data;

			SetupFunc setup_func;
			ExecuteFunc execute_func;
		};

		template<> 
		class RenderGraphPass<void> :public RenderGraphPassBase {
			using SetupFunc = std::function<void(RenderGraphBuilder&)>;
			using ExecuteFunc = std::function<void(RenderGraphContext&)>;

		public:
			RenderGraphPass(const Char* name, RenderGraphPassType pass_type, RenderGraphPassFlags pass_flags, SetupFunc&& setup_func, ExecuteFunc&& execute_func)
				:RenderGraphPassBase(name, pass_type, pass_flags), setup_func(std::move(setup_func)), execute_func(std::move(execute_func))
			{

			}

		private:
			void Setup(RenderGraphBuilder& render_graph_builder)override {
				CESAR_ASSERT(setup_func != nullptr && "Setup function cannot be null");
				setup_func(render_graph_builder);
			}

			void Execute(RenderGraphContext& render_graph_context)override {
				CESAR_ASSERT(execute_func != nullptr && "Execute Function cannot be null");
				execute_func(render_graph_context);
			}

			SetupFunc setup_func;
			ExecuteFunc execute_func;
		};

	}

	using RGPassBase = render_graph::RenderGraphPassBase;

	template<typename T>
	using RGPass = render_graph::RenderGraphPass<T>;

	using RGPassType = render_graph::RenderGraphPassType;
	using RGPassFlags = render_graph::RenderGraphPassFlags;

}