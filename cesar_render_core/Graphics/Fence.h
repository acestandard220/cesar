// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../cesar_core/cesar_core.h"

#include <d3d12.h>
namespace cesar
{
	class GPUContext;
	class _declspec(dllexport) Fence
	{
	public:
		Fence(GPUContext* render_context,const cesar::Char* name);
		~Fence();

		void Signal(cesar::Uint64 value);
		void Wait(cesar::Uint64 value);

		Bool   IsCompleted(cesar::Uint64 value);
		Uint64 GetCompletedValue();

		ID3D12Fence* GetFence();

		operator ID3D12Fence* ()const { return fence.Get(); }

	private:
		ComPtr<ID3D12Fence> fence;
		HANDLE event;

	};

}
