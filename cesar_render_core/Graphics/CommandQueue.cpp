// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "CommandQueue.h"
#include "../GPUContext.h"

namespace cesar
{
	using namespace cesar;
	static constexpr D3D12_COMMAND_LIST_TYPE ToD3D12CommandListType(CommandListType type)
	{
		switch (type)
		{
			case CommandListType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
			case CommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			case CommandListType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
			default: CESAR_DEBUGBREAK();
		}
	}

	CommandQueue::CommandQueue(GPUContext* gpu_context, CommandListType type, const Char* name)
		:type(type)
	{
		ID3D12Device* device = gpu_context->GetDevice();
		D3D12_COMMAND_QUEUE_DESC command_queue_desc{};
		command_queue_desc.Type = ToD3D12CommandListType(type);

		CESAR_D3D12_CHECK(device->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(&command_queue)));
		CESAR_NAME_D3D12_OBJECT(command_queue, name);
	}

	CommandQueue::~CommandQueue()
	{
		command_queue.Reset();
	}

	void CommandQueue::Execute(const std::span<CommandList*>& command_lists)
	{
		if (command_lists.empty())
			return;

		auto list = command_lists |
			std::views::transform([](auto& cmd_list) {return (ID3D12CommandList*)cmd_list->GetCommandList(); }) |
			std::ranges::to<std::vector>();

		command_queue->ExecuteCommandLists(list.size(), list.data());
	}

	void CommandQueue::Signal(Fence* fence, Uint64 value)
	{
		command_queue->Signal(fence->GetFence(), value);
	}

	void CommandQueue::Wait(Fence* fence, Uint64 value)
	{
		command_queue->Wait(fence->GetFence(), value);
	}
	
	CommandListType CommandQueue::GetCommandListType() const
	{
		return type;
	}

	ID3D12CommandQueue* CommandQueue::GetCommandQueue()
	{
		return command_queue.Get();
	}
}
