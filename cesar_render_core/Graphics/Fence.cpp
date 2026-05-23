// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "Fence.h"
#include "../GPUContext.h"

namespace cesar
{
	using namespace cesar;
	Fence::Fence(GPUContext* gpu_context, const Char* name)
	{
		ID3D12Device6* device = gpu_context->GetDevice();

		HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		if (FAILED(hr))
		{
			LOG_FATAL("Failed to create DirectX 12 Fence");
			return;
		}

		event = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		if (event == nullptr)
		{
			LOG_FATAL("Failed to create Fence Event.");
			return;
		}
	}

	Fence::~Fence()
	{
		fence.Reset();
	}

	void Fence::Signal(Uint64 value)
	{
		fence->Signal(value);
	}

	void Fence::Wait(Uint64 value)
	{
		if (!IsCompleted(value))
		{
			fence->SetEventOnCompletion(value, event);
			WaitForSingleObjectEx(event, INFINITE, false);
		}
	}


	Bool Fence::IsCompleted(Uint64 value)
	{
		return (GetCompletedValue() >= value);
	}

	Uint64 Fence::GetCompletedValue()
	{
		return fence->GetCompletedValue();
	}

	ID3D12Fence* Fence::GetFence()
	{
		return fence.Get();
	}

}