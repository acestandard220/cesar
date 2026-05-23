#include "pch.h"
#include "FrameResources.h"

using namespace Helpers;


// ApiObjectId     = 70
void CreateComputePipelineState_70()
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC cpsoDesc = {};
    cpsoDesc.pRootSignature = GetRootSignature(71);
    cpsoDesc.NodeMask = 0;
    cpsoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    UINT offset = 0u;
    std::vector<BYTE> data;
    g_resourceReader->Read(data, 11425);
    cpsoDesc.CS = { reinterpret_cast<BYTE*>(&data[offset]), 16820 };
    offset += 16820;

    CreateAndTrackComputePipelineState(70, g_device.Get(), &cpsoDesc);
}

// ApiObjectId     = 72
void CreateComputePipelineState_72()
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC cpsoDesc = {};
    cpsoDesc.pRootSignature = GetRootSignature(71);
    cpsoDesc.NodeMask = 0;
    cpsoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    UINT offset = 0u;
    std::vector<BYTE> data;
    g_resourceReader->Read(data, 9032);
    cpsoDesc.CS = { reinterpret_cast<BYTE*>(&data[offset]), 13064 };
    offset += 13064;

    CreateAndTrackComputePipelineState(72, g_device.Get(), &cpsoDesc);
}

// ApiObjectId     = 73
void CreateComputePipelineState_73()
{
    D3D12_COMPUTE_PIPELINE_STATE_DESC cpsoDesc = {};
    cpsoDesc.pRootSignature = GetRootSignature(71);
    cpsoDesc.NodeMask = 0;
    cpsoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    UINT offset = 0u;
    std::vector<BYTE> data;
    g_resourceReader->Read(data, 6487);
    cpsoDesc.CS = { reinterpret_cast<BYTE*>(&data[offset]), 10004 };
    offset += 10004;

    CreateAndTrackComputePipelineState(73, g_device.Get(), &cpsoDesc);
}

// ApiObjectId     = 74
void CreatePipelineState_74()
{
    CD3DX12_PIPELINE_STATE_STREAM2 pssDesc;
    CD3DX12_BLEND_DESC blendDesc;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.RenderTarget[0] = { FALSE, FALSE, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_LOGIC_OP_NOOP, 15 };
    blendDesc.RenderTarget[1] = { FALSE, FALSE, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_LOGIC_OP_NOOP, 15 };
    blendDesc.RenderTarget[2] = { FALSE, FALSE, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_LOGIC_OP_NOOP, 15 };
    blendDesc.RenderTarget[3] = { FALSE, FALSE, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_LOGIC_OP_NOOP, 15 };
    blendDesc.RenderTarget[4] = { FALSE, FALSE, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_LOGIC_OP_NOOP, 15 };
    blendDesc.RenderTarget[5] = { FALSE, FALSE, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_LOGIC_OP_NOOP, 15 };
    blendDesc.RenderTarget[6] = { FALSE, FALSE, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_LOGIC_OP_NOOP, 15 };
    blendDesc.RenderTarget[7] = { FALSE, FALSE, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_LOGIC_OP_NOOP, 15 };
    pssDesc.BlendState = blendDesc;
    D3D12_STREAM_OUTPUT_DESC streamoutputDesc = {};
    streamoutputDesc.NumEntries = 0;
    streamoutputDesc.NumStrides = 0;
    streamoutputDesc.RasterizedStream = 0;
    pssDesc.StreamOutput = streamoutputDesc;
    pssDesc.SampleMask = 4294967295;
    pssDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
    pssDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pssDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pssDesc.SampleDesc = { 1, 0 };
    pssDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    pssDesc.NodeMask = 0;
    pssDesc.pRootSignature = GetRootSignature(71);
    pssDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC1(TRUE, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_GREATER_EQUAL, FALSE, 0, 0, (D3D12_STENCIL_OP)0, (D3D12_STENCIL_OP)0, (D3D12_STENCIL_OP)0, (D3D12_COMPARISON_FUNC)0 /* D3D12_COMPARISON_FUNC_NONE if using Agility SDK 1.606.2 or later */, (D3D12_STENCIL_OP)0, (D3D12_STENCIL_OP)0, (D3D12_STENCIL_OP)0, (D3D12_COMPARISON_FUNC)0 /* D3D12_COMPARISON_FUNC_NONE if using Agility SDK 1.606.2 or later */, FALSE);
    pssDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE, FALSE, 0, 0.00000f, 0.00000f, FALSE, FALSE, FALSE, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);
    D3D12_RT_FORMAT_ARRAY rtFormatArray = {};
    rtFormatArray.NumRenderTargets = 1;
    rtFormatArray.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pssDesc.RTVFormats = rtFormatArray;
    pssDesc.ViewInstancingDesc = CD3DX12_VIEW_INSTANCING_DESC(0, nullptr, D3D12_VIEW_INSTANCING_FLAG_NONE);

    UINT offset = 0u;
    std::vector<BYTE> data;
    g_resourceReader->Read(data, 11902);
    pssDesc.PS = { reinterpret_cast<BYTE*>(&data[offset]), 5752 };
    offset += 5752;
    pssDesc.AS = { reinterpret_cast<BYTE*>(&data[offset]), 5184 };
    offset += 5184;
    pssDesc.MS = { reinterpret_cast<BYTE*>(&data[offset]), 8712 };
    offset += 8712;

    D3D12_PIPELINE_STATE_STREAM_DESC streamDesc{ sizeof(pssDesc), &pssDesc };

    ComPtr<ID3D12Device2> device2;
    ThrowIfFailed(g_device->QueryInterface(IID_PPV_ARGS(&device2)));
    CreateAndTrackPipelineState(74, device2.Get(), &streamDesc);
}

