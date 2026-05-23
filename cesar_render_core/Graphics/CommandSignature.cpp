#include "CommandSignature.h"
#include "../GPUContext.h"


namespace cesar {


	CommandSignature::CommandSignature(GPUContext* gpu_context, CommandArgumentType type, const Char* name) {
		ID3D12Device6* device = gpu_context->GetDevice();

		D3D12_INDIRECT_ARGUMENT_DESC arg_descs;
		arg_descs.Type = ToD3D12IndirectArgument(type);

		D3D12_COMMAND_SIGNATURE_DESC cmd_signature_desc{};
		cmd_signature_desc.NumArgumentDescs = 1;
		cmd_signature_desc.pArgumentDescs = &arg_descs;
		cmd_signature_desc.ByteStride = GetCommandArgumentStride(type);

		ID3D12RootSignature* root_signature = gpu_context->GetGlobalRootSignature();
		HRESULT hr = device->CreateCommandSignature(&cmd_signature_desc, nullptr, IID_PPV_ARGS(&cmd_signature));
		CESAR_D3D12_CHECK(hr);
		CESAR_NAME_D3D12_OBJECT(cmd_signature, name);
	}

}
