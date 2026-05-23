#pragma once

#include "pch.h"
#include "CommandLists.h"
#include "ShaderTableReconstruction.h"

using namespace Helpers;

void PopulateCommandList_7_1_0()
{
    ThrowIfFailed(GetCommandList(7)->Reset(GetCommandAllocator(8).Get(), GetPipelineState(0)));

    // GlobalId        = 5
    {
        std::vector<D3D12_RESOURCE_BARRIER> barriers;
        barriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(GetResource(9).Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, 4294967295, D3D12_RESOURCE_BARRIER_FLAG_NONE));
        GetCommandList(7)->ResourceBarrier(1, barriers.data());
    }

    // GlobalId        = 6
    GetCommandList(7)->CopyResource(GetResource(9).Get(), GetResource(11).Get());

    // GlobalId        = 7
    {
        std::vector<D3D12_RESOURCE_BARRIER> barriers;
        barriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(GetResource(9).Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON, 4294967295, D3D12_RESOURCE_BARRIER_FLAG_NONE));
        GetCommandList(7)->ResourceBarrier(1, barriers.data());
    }

    GetCommandList(7)->Close();
}

void PopulateCommandList_13_1_0()
{
    ThrowIfFailed(GetCommandList(13)->Reset(GetCommandAllocator(14).Get(), GetPipelineState(0)));

    // GlobalId        = 9
    {
        std::vector<D3D12_BUFFER_BARRIER> barrier_0;
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_SOURCE, GetResource(21).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_DEST, GetResource(42).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_SOURCE, GetResource(24).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_DEST, GetResource(43).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_SOURCE, GetResource(17).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_DEST, GetResource(39).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_SOURCE, GetResource(18).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_DEST, GetResource(53).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_SOURCE, GetResource(31).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_DEST, GetResource(60).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_SOURCE, GetResource(32).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_DEST, GetResource(54).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_SOURCE, GetResource(33).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_DEST, GetResource(58).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_SOURCE, GetResource(34).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_DEST, GetResource(59).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_SOURCE, GetResource(30).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_COPY_DEST, GetResource(57).Get(), 0, 18446744073709551615 });
        D3D12_BARRIER_GROUP groups[1];
        groups[0].Type = D3D12_BARRIER_TYPE_BUFFER;
        groups[0].NumBarriers = (UINT)barrier_0.size();
        groups[0].pBufferBarriers = barrier_0.data();
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(13).As(&cl7));
        cl7->Barrier(1, groups);
    }

    // GlobalId        = 10
    GetCommandList(13)->CopyResource(GetResource(43).Get(), GetResource(24).Get());

    // GlobalId        = 11
    GetCommandList(13)->CopyResource(GetResource(42).Get(), GetResource(21).Get());

    // GlobalId        = 12
    GetCommandList(13)->CopyResource(GetResource(39).Get(), GetResource(17).Get());

    // GlobalId        = 13
    GetCommandList(13)->CopyResource(GetResource(53).Get(), GetResource(18).Get());

    // GlobalId        = 14
    GetCommandList(13)->CopyResource(GetResource(60).Get(), GetResource(31).Get());

    // GlobalId        = 15
    GetCommandList(13)->CopyResource(GetResource(54).Get(), GetResource(32).Get());

    // GlobalId        = 16
    GetCommandList(13)->CopyResource(GetResource(58).Get(), GetResource(33).Get());

    // GlobalId        = 17
    GetCommandList(13)->CopyResource(GetResource(59).Get(), GetResource(34).Get());

    // GlobalId        = 18
    GetCommandList(13)->CopyResource(GetResource(57).Get(), GetResource(30).Get());

    // GlobalId        = 19
    {
        std::vector<D3D12_BUFFER_BARRIER> barrier_0;
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_SOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(21).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_SOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(24).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_DEST, D3D12_BARRIER_ACCESS_COMMON, GetResource(42).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_DEST, D3D12_BARRIER_ACCESS_COMMON, GetResource(43).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_SOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(17).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_DEST, D3D12_BARRIER_ACCESS_COMMON, GetResource(39).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_SOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(18).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_DEST, D3D12_BARRIER_ACCESS_COMMON, GetResource(53).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_SOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(31).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_DEST, D3D12_BARRIER_ACCESS_COMMON, GetResource(60).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_SOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(32).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_DEST, D3D12_BARRIER_ACCESS_COMMON, GetResource(54).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_SOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(33).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_DEST, D3D12_BARRIER_ACCESS_COMMON, GetResource(58).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_SOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(34).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_DEST, D3D12_BARRIER_ACCESS_COMMON, GetResource(59).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_SOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(30).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COPY, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_COPY_DEST, D3D12_BARRIER_ACCESS_COMMON, GetResource(57).Get(), 0, 18446744073709551615 });
        D3D12_BARRIER_GROUP groups[1];
        groups[0].Type = D3D12_BARRIER_TYPE_BUFFER;
        groups[0].NumBarriers = (UINT)barrier_0.size();
        groups[0].pBufferBarriers = barrier_0.data();
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(13).As(&cl7));
        cl7->Barrier(1, groups);
    }

    // GlobalId        = 20
    {
        D3D12_BARRIER_GROUP groups[0];
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(13).As(&cl7));
        cl7->Barrier(0, groups);
    }

    GetCommandList(13)->Close();
}

void PopulateCommandList_63_1_0()
{
    ThrowIfFailed(GetCommandList(63)->Reset(GetCommandAllocator(64).Get(), GetPipelineState(0)));

    {
        ID3D12DescriptorHeap* ppHeaps[] = { g_descriptorHeap_75.Get(), g_descriptorHeap_76.Get() };
        GetCommandList(63)->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    }
    GetCommandList(63)->SetComputeRootSignature(GetRootSignature(71));

    GetCommandList(63)->SetGraphicsRootSignature(GetRootSignature(71));

    // GlobalId        = 23
    {
        std::vector<D3D12_TEXTURE_BARRIER> barrier_0(3);
        barrier_0[0].SyncBefore = D3D12_BARRIER_SYNC_ALL;
        barrier_0[0].SyncAfter = D3D12_BARRIER_SYNC_RENDER_TARGET;
        barrier_0[0].AccessBefore = D3D12_BARRIER_ACCESS_COMMON;
        barrier_0[0].AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET;
        barrier_0[0].LayoutBefore = D3D12_BARRIER_LAYOUT_COMMON;
        barrier_0[0].LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
        barrier_0[0].pResource = GetResource(68).Get();
        barrier_0[0].Subresources.IndexOrFirstMipLevel = 4294967295;
        barrier_0[0].Subresources.NumMipLevels = 0;
        barrier_0[0].Subresources.FirstArraySlice = 0;
        barrier_0[0].Subresources.NumArraySlices = 0;
        barrier_0[0].Subresources.FirstPlane = 0;
        barrier_0[0].Subresources.NumPlanes = 0;
        barrier_0[0].Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;
        barrier_0[1].SyncBefore = D3D12_BARRIER_SYNC_ALL;
        barrier_0[1].SyncAfter = D3D12_BARRIER_SYNC_RENDER_TARGET;
        barrier_0[1].AccessBefore = D3D12_BARRIER_ACCESS_COMMON;
        barrier_0[1].AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET;
        barrier_0[1].LayoutBefore = D3D12_BARRIER_LAYOUT_COMMON;
        barrier_0[1].LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
        barrier_0[1].pResource = GetResource(67).Get();
        barrier_0[1].Subresources.IndexOrFirstMipLevel = 4294967295;
        barrier_0[1].Subresources.NumMipLevels = 0;
        barrier_0[1].Subresources.FirstArraySlice = 0;
        barrier_0[1].Subresources.NumArraySlices = 0;
        barrier_0[1].Subresources.FirstPlane = 0;
        barrier_0[1].Subresources.NumPlanes = 0;
        barrier_0[1].Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;
        barrier_0[2].SyncBefore = D3D12_BARRIER_SYNC_ALL;
        barrier_0[2].SyncAfter = D3D12_BARRIER_SYNC_RENDER_TARGET;
        barrier_0[2].AccessBefore = D3D12_BARRIER_ACCESS_COMMON;
        barrier_0[2].AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET;
        barrier_0[2].LayoutBefore = D3D12_BARRIER_LAYOUT_COMMON;
        barrier_0[2].LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
        barrier_0[2].pResource = GetResource(69).Get();
        barrier_0[2].Subresources.IndexOrFirstMipLevel = 4294967295;
        barrier_0[2].Subresources.NumMipLevels = 0;
        barrier_0[2].Subresources.FirstArraySlice = 0;
        barrier_0[2].Subresources.NumArraySlices = 0;
        barrier_0[2].Subresources.FirstPlane = 0;
        barrier_0[2].Subresources.NumPlanes = 0;
        barrier_0[2].Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;
        std::vector<D3D12_BUFFER_BARRIER> barrier_1;
        barrier_1.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_ALL_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(43).Get(), 0, 18446744073709551615 });
        barrier_1.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_ALL_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(39).Get(), 0, 18446744073709551615 });
        barrier_1.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, GetResource(55).Get(), 0, 18446744073709551615 });
        barrier_1.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_ALL_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(60).Get(), 0, 18446744073709551615 });
        barrier_1.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, GetResource(56).Get(), 0, 18446744073709551615 });
        barrier_1.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_ALL_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(54).Get(), 0, 18446744073709551615 });
        barrier_1.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, GetResource(47).Get(), 0, 18446744073709551615 });
        D3D12_BARRIER_GROUP groups[2];
        groups[0].Type = D3D12_BARRIER_TYPE_TEXTURE;
        groups[0].NumBarriers = (UINT)barrier_0.size();
        groups[0].pTextureBarriers = barrier_0.data();
        groups[1].Type = D3D12_BARRIER_TYPE_BUFFER;
        groups[1].NumBarriers = (UINT)barrier_1.size();
        groups[1].pBufferBarriers = barrier_1.data();
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(63).As(&cl7));
        cl7->Barrier(2, groups);
    }

    GetCommandList(63)->SetPipelineState(GetPipelineState(72));
    {
        std::vector<BYTE> srcData;
        g_resourceReader->Read(srcData, 32);
        GetCommandList(63)->SetGraphicsRoot32BitConstants(1, 1, srcData.data(), 0);
    }

    // GlobalId        = 24
    {
        const UINT values[] = { 0, 3435973836, 3435973836, 3435973836 };
        D3D12_GPU_DESCRIPTOR_HANDLE gpuUavHandle = g_descriptorHeap_75->GetGPUDescriptorHandleForHeapStart();
        gpuUavHandle.ptr += g_cbvSrvUavDescriptorSize * 8;

        D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        desc.Buffer = { 0, 1, 12, 0, D3D12_BUFFER_UAV_FLAG_NONE };
        D3D12_CPU_DESCRIPTOR_HANDLE cpuUavHandle = CreateTemporaryCpuDescriptor_Uav(GetResource(46).Get(), nullptr, &desc, 0);

        GetCommandList(63)->ClearUnorderedAccessViewUint(gpuUavHandle, cpuUavHandle, GetResource(56).Get(), values, 0, nullptr);
    }
    // GlobalId        = 25
    GetCommandList(63)->Dispatch(1, 1, 1);

    // GlobalId        = 26
    {
        D3D12_BARRIER_GROUP groups[0];
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(63).As(&cl7));
        cl7->Barrier(0, groups);
    }

    {
        D3D12_VIEWPORT viewports[1];
        viewports[0] = { 0.00000f, 0.00000f, 800.000f, 800.000f, 0.00000f, 1.00000f };

        GetCommandList(63)->RSSetViewports(1, viewports);
    }

    {
        D3D12_RECT rects[1];
        rects[0] = { 0, 0, 800, 800 };

        GetCommandList(63)->RSSetScissorRects(1, rects);
    }

    // GlobalId        = 27
    {
        D3D12_RENDER_PASS_RENDER_TARGET_DESC renderTargets[3];
        auto rtvHandle = g_rtvHeap->GetCPUDescriptorHandleForHeapStart();
        static D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_0 {};
        rtvDesc_0.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc_0.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc_0.Texture2DArray = { 0, 0, 1, 0 };
        g_device->CreateRenderTargetView(GetResource(68).Get(), &rtvDesc_0, rtvHandle);
        renderTargets[0].cpuDescriptor = rtvHandle;
        rtvHandle.ptr += g_rtvDescriptorSize;
        renderTargets[0].BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
        renderTargets[0].BeginningAccess.Clear.ClearValue.Format = DXGI_FORMAT_UNKNOWN;
        renderTargets[0].BeginningAccess.Clear.ClearValue.Color[0] = 0.00000f;
        renderTargets[0].BeginningAccess.Clear.ClearValue.Color[1] = 0.00000f;
        renderTargets[0].BeginningAccess.Clear.ClearValue.Color[2] = 0.00000f;
        renderTargets[0].BeginningAccess.Clear.ClearValue.Color[3] = 0.00000f;
        renderTargets[0].EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

        static D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_1 {};
        rtvDesc_1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc_1.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc_1.Texture2DArray = { 0, 0, 1, 0 };
        g_device->CreateRenderTargetView(GetResource(67).Get(), &rtvDesc_1, rtvHandle);
        renderTargets[1].cpuDescriptor = rtvHandle;
        rtvHandle.ptr += g_rtvDescriptorSize;
        renderTargets[1].BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
        renderTargets[1].BeginningAccess.Clear.ClearValue.Format = DXGI_FORMAT_UNKNOWN;
        renderTargets[1].BeginningAccess.Clear.ClearValue.Color[0] = 0.00000f;
        renderTargets[1].BeginningAccess.Clear.ClearValue.Color[1] = 0.00000f;
        renderTargets[1].BeginningAccess.Clear.ClearValue.Color[2] = 0.00000f;
        renderTargets[1].BeginningAccess.Clear.ClearValue.Color[3] = 0.00000f;
        renderTargets[1].EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

        static D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_2 {};
        rtvDesc_2.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc_2.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc_2.Texture2DArray = { 0, 0, 1, 0 };
        g_device->CreateRenderTargetView(GetResource(69).Get(), &rtvDesc_2, rtvHandle);
        renderTargets[2].cpuDescriptor = rtvHandle;
        rtvHandle.ptr += g_rtvDescriptorSize;
        renderTargets[2].BeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
        renderTargets[2].BeginningAccess.Clear.ClearValue.Format = DXGI_FORMAT_UNKNOWN;
        renderTargets[2].BeginningAccess.Clear.ClearValue.Color[0] = 0.00000f;
        renderTargets[2].BeginningAccess.Clear.ClearValue.Color[1] = 0.00000f;
        renderTargets[2].BeginningAccess.Clear.ClearValue.Color[2] = 0.00000f;
        renderTargets[2].BeginningAccess.Clear.ClearValue.Color[3] = 0.00000f;
        renderTargets[2].EndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

        D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depthStencil {};
        static D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
        dsvDesc.Texture2DArray = { 0, 0, 1 };
        g_device->CreateDepthStencilView(GetResource(66).Get(), &dsvDesc, g_dsvHeap->GetCPUDescriptorHandleForHeapStart());
        depthStencil.cpuDescriptor = g_dsvHeap->GetCPUDescriptorHandleForHeapStart();
        depthStencil.DepthBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
        depthStencil.DepthBeginningAccess.Clear.ClearValue.Format = DXGI_FORMAT_UNKNOWN;
        depthStencil.DepthBeginningAccess.Clear.ClearValue.DepthStencil = { 0.00000f, 0 };

        depthStencil.StencilBeginningAccess.Type = D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_NO_ACCESS;

        depthStencil.DepthEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;

        depthStencil.StencilEndingAccess.Type = D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_NO_ACCESS;

        GetCommandList(63)->BeginRenderPass(3, renderTargets, &depthStencil, D3D12_RENDER_PASS_FLAG_NONE);
    }

    GetCommandList(63)->SetPipelineState(GetPipelineState(74));
    // GlobalId        = 28
    GetCommandList(63)->DispatchMesh(1, 1, 1);

    // GlobalId        = 29
    GetCommandList(63)->EndRenderPass();

    // GlobalId        = 30
    {
        std::vector<D3D12_TEXTURE_BARRIER> barrier_0(3);
        barrier_0[0].SyncBefore = D3D12_BARRIER_SYNC_RENDER_TARGET;
        barrier_0[0].SyncAfter = D3D12_BARRIER_SYNC_ALL;
        barrier_0[0].AccessBefore = D3D12_BARRIER_ACCESS_RENDER_TARGET;
        barrier_0[0].AccessAfter = D3D12_BARRIER_ACCESS_COMMON;
        barrier_0[0].LayoutBefore = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
        barrier_0[0].LayoutAfter = D3D12_BARRIER_LAYOUT_COMMON;
        barrier_0[0].pResource = GetResource(68).Get();
        barrier_0[0].Subresources.IndexOrFirstMipLevel = 4294967295;
        barrier_0[0].Subresources.NumMipLevels = 0;
        barrier_0[0].Subresources.FirstArraySlice = 0;
        barrier_0[0].Subresources.NumArraySlices = 0;
        barrier_0[0].Subresources.FirstPlane = 0;
        barrier_0[0].Subresources.NumPlanes = 0;
        barrier_0[0].Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;
        barrier_0[1].SyncBefore = D3D12_BARRIER_SYNC_RENDER_TARGET;
        barrier_0[1].SyncAfter = D3D12_BARRIER_SYNC_ALL;
        barrier_0[1].AccessBefore = D3D12_BARRIER_ACCESS_RENDER_TARGET;
        barrier_0[1].AccessAfter = D3D12_BARRIER_ACCESS_COMMON;
        barrier_0[1].LayoutBefore = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
        barrier_0[1].LayoutAfter = D3D12_BARRIER_LAYOUT_COMMON;
        barrier_0[1].pResource = GetResource(67).Get();
        barrier_0[1].Subresources.IndexOrFirstMipLevel = 4294967295;
        barrier_0[1].Subresources.NumMipLevels = 0;
        barrier_0[1].Subresources.FirstArraySlice = 0;
        barrier_0[1].Subresources.NumArraySlices = 0;
        barrier_0[1].Subresources.FirstPlane = 0;
        barrier_0[1].Subresources.NumPlanes = 0;
        barrier_0[1].Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;
        barrier_0[2].SyncBefore = D3D12_BARRIER_SYNC_RENDER_TARGET;
        barrier_0[2].SyncAfter = D3D12_BARRIER_SYNC_ALL;
        barrier_0[2].AccessBefore = D3D12_BARRIER_ACCESS_RENDER_TARGET;
        barrier_0[2].AccessAfter = D3D12_BARRIER_ACCESS_COMMON;
        barrier_0[2].LayoutBefore = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
        barrier_0[2].LayoutAfter = D3D12_BARRIER_LAYOUT_COMMON;
        barrier_0[2].pResource = GetResource(69).Get();
        barrier_0[2].Subresources.IndexOrFirstMipLevel = 4294967295;
        barrier_0[2].Subresources.NumMipLevels = 0;
        barrier_0[2].Subresources.FirstArraySlice = 0;
        barrier_0[2].Subresources.NumArraySlices = 0;
        barrier_0[2].Subresources.FirstPlane = 0;
        barrier_0[2].Subresources.NumPlanes = 0;
        barrier_0[2].Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;
        std::vector<D3D12_BUFFER_BARRIER> barrier_1;
        barrier_1.push_back({ D3D12_BARRIER_SYNC_ALL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(60).Get(), 0, 18446744073709551615 });
        barrier_1.push_back({ D3D12_BARRIER_SYNC_ALL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(54).Get(), 0, 18446744073709551615 });
        D3D12_BARRIER_GROUP groups[2];
        groups[0].Type = D3D12_BARRIER_TYPE_TEXTURE;
        groups[0].NumBarriers = (UINT)barrier_0.size();
        groups[0].pTextureBarriers = barrier_0.data();
        groups[1].Type = D3D12_BARRIER_TYPE_BUFFER;
        groups[1].NumBarriers = (UINT)barrier_1.size();
        groups[1].pBufferBarriers = barrier_1.data();
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(63).As(&cl7));
        cl7->Barrier(2, groups);
    }

    // GlobalId        = 31
    {
        std::vector<D3D12_BUFFER_BARRIER> barrier_0;
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(56).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(47).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, GetResource(44).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, GetResource(46).Get(), 0, 18446744073709551615 });
        D3D12_BARRIER_GROUP groups[1];
        groups[0].Type = D3D12_BARRIER_TYPE_BUFFER;
        groups[0].NumBarriers = (UINT)barrier_0.size();
        groups[0].pBufferBarriers = barrier_0.data();
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(63).As(&cl7));
        cl7->Barrier(1, groups);
    }

    GetCommandList(63)->SetPipelineState(GetPipelineState(73));
    {
        std::vector<BYTE> srcData;
        g_resourceReader->Read(srcData, 32);
        GetCommandList(63)->SetGraphicsRoot32BitConstants(1, 1, srcData.data(), 0);
    }

    // GlobalId        = 32
    GetCommandList(63)->Dispatch(1, 1, 1);

    // GlobalId        = 33
    {
        D3D12_BARRIER_GROUP groups[0];
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(63).As(&cl7));
        cl7->Barrier(0, groups);
    }

    // GlobalId        = 34
    {
        std::vector<D3D12_BUFFER_BARRIER> barrier_0;
        barrier_0.push_back({ D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(47).Get(), 0, 18446744073709551615 });
        D3D12_BARRIER_GROUP groups[1];
        groups[0].Type = D3D12_BARRIER_TYPE_BUFFER;
        groups[0].NumBarriers = (UINT)barrier_0.size();
        groups[0].pBufferBarriers = barrier_0.data();
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(63).As(&cl7));
        cl7->Barrier(1, groups);
    }

    // GlobalId        = 35
    {
        std::vector<D3D12_BUFFER_BARRIER> barrier_0;
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL_SHADING, D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(43).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL_SHADING, D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(39).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(42).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(53).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(55).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(56).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(44).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, GetResource(45).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_ACCESS_COMMON, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, GetResource(48).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, GetResource(46).Get(), 0, 18446744073709551615 });
        D3D12_BARRIER_GROUP groups[1];
        groups[0].Type = D3D12_BARRIER_TYPE_BUFFER;
        groups[0].NumBarriers = (UINT)barrier_0.size();
        groups[0].pBufferBarriers = barrier_0.data();
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(63).As(&cl7));
        cl7->Barrier(1, groups);
    }

    GetCommandList(63)->SetPipelineState(GetPipelineState(70));
    {
        std::vector<BYTE> srcData;
        g_resourceReader->Read(srcData, 32);
        GetCommandList(63)->SetGraphicsRoot32BitConstants(1, 1, srcData.data(), 0);
    }

    // GlobalId        = 36
    GetCommandList(63)->ExecuteIndirect(GetCommandSignature(77), 1, g_indirectArgumentBuffers["46_1"].Get(), 0, nullptr, 0);

    // GlobalId        = 38
    {
        std::vector<D3D12_BUFFER_BARRIER> barrier_0;
        barrier_0.push_back({ D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(39).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(42).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(53).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(55).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(43).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(56).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(44).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_NON_PIXEL_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_COMMON, GetResource(46).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_COMMON, GetResource(45).Get(), 0, 18446744073709551615 });
        barrier_0.push_back({ D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_ALL, D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_COMMON, GetResource(48).Get(), 0, 18446744073709551615 });
        D3D12_BARRIER_GROUP groups[1];
        groups[0].Type = D3D12_BARRIER_TYPE_BUFFER;
        groups[0].NumBarriers = (UINT)barrier_0.size();
        groups[0].pBufferBarriers = barrier_0.data();
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(63).As(&cl7));
        cl7->Barrier(1, groups);
    }

    // GlobalId        = 39
    {
        D3D12_BARRIER_GROUP groups[0];
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(63).As(&cl7));
        cl7->Barrier(0, groups);
    }

    GetCommandList(63)->Close();
}

void PopulateCommandList_79_1_0()
{
    ThrowIfFailed(GetCommandList(79)->Reset(GetCommandAllocator(80).Get(), GetPipelineState(0)));

    {
        ID3D12DescriptorHeap* ppHeaps[] = { g_descriptorHeap_75.Get(), g_descriptorHeap_76.Get() };
        GetCommandList(79)->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    }
    GetCommandList(79)->SetComputeRootSignature(GetRootSignature(71));

    // GlobalId        = 40
    {
        D3D12_BARRIER_GROUP groups[0];
        ComPtr<ID3D12GraphicsCommandList7> cl7;
        ThrowIfFailed(GetCommandList(79).As(&cl7));
        cl7->Barrier(0, groups);
    }

    GetCommandList(79)->Close();
}

