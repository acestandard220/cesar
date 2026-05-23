#pragma once
#include "../../cesar_core/cesar_core.h"

#include <d3d12.h>

namespace cesar
{
	enum class CommandArgumentType {
		DrawIndexed,
		DispatchCompute,
		DispatchMesh
	};


	inline Uint32 GetCommandArgumentStride(CommandArgumentType type) {
		switch (type) {
		case CommandArgumentType::DrawIndexed:     return CESAR_SIZEOF(D3D12_DRAW_INDEXED_ARGUMENTS);
		case CommandArgumentType::DispatchCompute: return CESAR_SIZEOF(D3D12_DISPATCH_ARGUMENTS);
		case CommandArgumentType::DispatchMesh:    return CESAR_SIZEOF(D3D12_DISPATCH_MESH_ARGUMENTS);
		default:CESAR_FEATURE_NO_IMPL("There is not other impleentations for other types of CommandArguments.");
		}
	}

	inline D3D12_INDIRECT_ARGUMENT_TYPE ToD3D12IndirectArgument(CommandArgumentType type) {
		switch (type) {
		case CommandArgumentType::DrawIndexed:     return D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
		case CommandArgumentType::DispatchCompute: return D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
		case CommandArgumentType::DispatchMesh:    return D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH;
		default:CESAR_FEATURE_NO_IMPL("There is not other impleentations for other types of CommandArguments.");
		}
	}

	class GPUContext;
	class _declspec(dllexport) CommandSignature {
	public:
		CommandSignature(GPUContext* gpu_context, CommandArgumentType type, const Char* name);
		
		ID3D12CommandSignature* GetCommandSignature() { return cmd_signature.Get(); }
	private:
		ComPtr<ID3D12CommandSignature> cmd_signature;
	};



}