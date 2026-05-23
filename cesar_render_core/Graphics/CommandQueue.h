// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../../cesar_core/cesar_core.h"

#include "Device.h"
#include "Fence.h"
#include "CommandList.h"

namespace cesar
{
	class GPUContext;
	class _declspec(dllexport) CommandQueue
	{
	public:
		CommandQueue(GPUContext* render_context, CommandListType type, const cesar::Char* name = "CommandQueue");
		~CommandQueue();

		void Execute(const std::span<CommandList*>& command_lists);

		void Signal(Fence* fence, cesar::Uint64 value);
		void Wait(Fence* fence, cesar::Uint64 value);

		CommandListType GetCommandListType()const;
		ID3D12CommandQueue* GetCommandQueue();

	private:
		ComPtr<ID3D12CommandQueue> command_queue;		
		CommandListType type;
	};

}
