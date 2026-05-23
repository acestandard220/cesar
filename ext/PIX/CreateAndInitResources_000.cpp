#pragma once

#include "pch.h"
#include "FrameResources.h"

using namespace Helpers;

// ApiObjectId     = 10
void CreateAndInitResource_10()
{
    static D3D12_HEAP_PROPERTIES heapProperties { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 65536, 800, 800, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS };
    CreateAndTrackCommittedResource(10, g_device.Get(), &heapProperties, D3D12HeapFlags(D3D12_HEAP_FLAG_SHARED), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 43);

    {
        auto destAlloc = g_uploadBuffers->Allocate(2662272);
        CopyBytesToMappableResource(&uncompressedData[0], 2662272, destAlloc.GetResource(), destAlloc.GetOffset());
        for (auto i = 0u; i < 1; ++i)
        {
            const auto& resourceInitInfo = g_resourceInitInfo_10_0[i];
            CreateTextureCopyLocation_PlacedFootprint(g_textureCopyLocationPlacedFootprint, destAlloc.GetResource(), resourceInitInfo.Offset + destAlloc.GetOffset(), resourceInitInfo.SubresourceFootprint);
            CreateTextureCopyLocation_SubresourceIndex(g_textureCopyLocationSubresourceIndex, GetResource(10).Get(), resourceInitInfo.SubresourceIndex);
            g_utilityCommandList->CopyTextureRegion(&g_textureCopyLocationSubresourceIndex, 0, 0, 0, &g_textureCopyLocationPlacedFootprint, nullptr);
        }
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 11
void CreateAndInitResource_11()
{
    static D3D12_HEAP_PROPERTIES heapProperties { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0 };
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, 800, 800, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
    CreateAndTrackCommittedResource(11, g_device.Get(), &heapProperties, D3D12HeapFlags(D3D12_HEAP_FLAG_SHARED | D3D12_HEAP_FLAG_ALLOW_DISPLAY), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 43);

    D3D12_RESOURCE_BARRIER resourceTransitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(GetResource(11).Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, 4294967295, D3D12_RESOURCE_BARRIER_FLAG_NONE);
    g_utilityCommandList->ResourceBarrier(1, &resourceTransitionBarrier);

    {
        auto destAlloc = g_uploadBuffers->Allocate(2662272);
        CopyBytesToMappableResource(&uncompressedData[0], 2662272, destAlloc.GetResource(), destAlloc.GetOffset());
        for (auto i = 0u; i < 1; ++i)
        {
            const auto& resourceInitInfo = g_resourceInitInfo_11_0[i];
            CreateTextureCopyLocation_PlacedFootprint(g_textureCopyLocationPlacedFootprint, destAlloc.GetResource(), resourceInitInfo.Offset + destAlloc.GetOffset(), resourceInitInfo.SubresourceFootprint);
            CreateTextureCopyLocation_SubresourceIndex(g_textureCopyLocationSubresourceIndex, GetResource(11).Get(), resourceInitInfo.SubresourceIndex);
            g_utilityCommandList->CopyTextureRegion(&g_textureCopyLocationSubresourceIndex, 0, 0, 0, &g_textureCopyLocationPlacedFootprint, nullptr);
        }
    }
    std::vector<D3D12_RESOURCE_BARRIER> barriers;
    for (auto i = 0u; i < 1; ++i)
    {
        if (g_resourceFinalStates_11[i] != D3D12_RESOURCE_STATE_COPY_DEST)
        {
            barriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(GetResource(11).Get(), D3D12_RESOURCE_STATE_COPY_DEST, g_resourceFinalStates_11[i], i, D3D12_RESOURCE_BARRIER_FLAG_NONE));
        }
    }
    g_utilityCommandList->ResourceBarrier((UINT)barriers.size(), barriers.data());
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 16
void CreateAndInitResource_16()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 256, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(16, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 393216);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 37);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 256, GetResource(16).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 17
void CreateAndInitResource_17()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 108, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(17, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 786432);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 123);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 108, GetResource(17).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 18
void CreateAndInitResource_18()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 552, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(18, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 851968);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 267);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 552, GetResource(18).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 19
void CreateAndInitResource_19()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 32, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(19, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 327680);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 60);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 32, GetResource(19).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 20
void CreateAndInitResource_20()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 68, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(20, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 524288);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 37);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 68, GetResource(20).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 21
void CreateAndInitResource_21()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 1564, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(21, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 655360);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 57);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 1564, GetResource(21).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 22
void CreateAndInitResource_22()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 124, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(22, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 458752);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 37);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 124, GetResource(22).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 23
void CreateAndInitResource_23()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 72, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(23, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 589824);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 37);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 72, GetResource(23).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 24
void CreateAndInitResource_24()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 216, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(24, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 720896);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 68);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 216, GetResource(24).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 25
void CreateAndInitResource_25()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 144, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(25, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 0);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 37);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 144, GetResource(25).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 26
void CreateAndInitResource_26()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 36, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(26, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 65536);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 64);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 36, GetResource(26).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 27
void CreateAndInitResource_27()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 496, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(27, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 262144);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 39);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 496, GetResource(27).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 28
void CreateAndInitResource_28()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 24, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(28, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 131072);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 52);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 24, GetResource(28).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 29
void CreateAndInitResource_29()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 2304, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(29, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 196608);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 39);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 2304, GetResource(29).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 30
void CreateAndInitResource_30()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 289134, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(30, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 5701632);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 261843);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 289134, GetResource(30).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 31
void CreateAndInitResource_31()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 2252160, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(31, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 917504);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 1821819);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 2252160, GetResource(31).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 32
void CreateAndInitResource_32()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 1156536, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(32, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 3211264);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 600912);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 1156536, GetResource(32).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 33
void CreateAndInitResource_33()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 39040, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(33, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 4390912);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 32848);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 39040, GetResource(33).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 34
void CreateAndInitResource_34()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 1196544, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE };
    CreateAndTrackPlacedResource(34, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 15, 4456448);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 225687);

    {
        CopyBytesToMappableResource(&uncompressedData[0], 1196544, GetResource(34).Get(), 0u);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 36
void CreateAndInitResource_36()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 32, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(36, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 262144);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 60);

    {
        auto destAlloc = g_uploadBuffers->Allocate(32);
        CopyBytesToMappableResource(&uncompressedData[0], 32, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(36).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 32);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 37
void CreateAndInitResource_37()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 68, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(37, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 458752);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 37);

    {
        auto destAlloc = g_uploadBuffers->Allocate(68);
        CopyBytesToMappableResource(&uncompressedData[0], 68, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(37).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 68);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 38
void CreateAndInitResource_38()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 72, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(38, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 524288);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 37);

    {
        auto destAlloc = g_uploadBuffers->Allocate(72);
        CopyBytesToMappableResource(&uncompressedData[0], 72, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(38).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 72);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 39
void CreateAndInitResource_39()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 108, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(39, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 720896);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 123);

    {
        auto destAlloc = g_uploadBuffers->Allocate(108);
        CopyBytesToMappableResource(&uncompressedData[0], 108, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(39).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 108);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 40
void CreateAndInitResource_40()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 124, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(40, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 393216);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 37);

    {
        auto destAlloc = g_uploadBuffers->Allocate(124);
        CopyBytesToMappableResource(&uncompressedData[0], 124, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(40).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 124);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 41
void CreateAndInitResource_41()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 256, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(41, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 327680);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 37);

    {
        auto destAlloc = g_uploadBuffers->Allocate(256);
        CopyBytesToMappableResource(&uncompressedData[0], 256, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(41).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 256);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 42
void CreateAndInitResource_42()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 1564, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(42, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 589824);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 57);

    {
        auto destAlloc = g_uploadBuffers->Allocate(1564);
        CopyBytesToMappableResource(&uncompressedData[0], 1564, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(42).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 1564);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 43
void CreateAndInitResource_43()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 216, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(43, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 655360);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 68);

    {
        auto destAlloc = g_uploadBuffers->Allocate(216);
        CopyBytesToMappableResource(&uncompressedData[0], 216, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(43).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 216);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 44
void CreateAndInitResource_44()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 4, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(44, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 6160384);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 32);

    {
        auto destAlloc = g_uploadBuffers->Allocate(4);
        CopyBytesToMappableResource(&uncompressedData[0], 4, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(44).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 4);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 45
void CreateAndInitResource_45()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 8, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(45, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 6291456);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 36);

    {
        auto destAlloc = g_uploadBuffers->Allocate(8);
        CopyBytesToMappableResource(&uncompressedData[0], 8, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(45).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 8);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 46
void CreateAndInitResource_46()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 12, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(46, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 6225920);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 40);

    {
        auto destAlloc = g_uploadBuffers->Allocate(12);
        CopyBytesToMappableResource(&uncompressedData[0], 12, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(46).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 12);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 47
void CreateAndInitResource_47()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 4, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(47, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 6094848);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 32);

    {
        auto destAlloc = g_uploadBuffers->Allocate(4);
        CopyBytesToMappableResource(&uncompressedData[0], 4, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(47).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 4);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 48
void CreateAndInitResource_48()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 4, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(48, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 6356992);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 32);

    {
        auto destAlloc = g_uploadBuffers->Allocate(4);
        CopyBytesToMappableResource(&uncompressedData[0], 4, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(48).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 4);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 49
void CreateAndInitResource_49()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 24, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(49, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 65536);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 52);

    {
        auto destAlloc = g_uploadBuffers->Allocate(24);
        CopyBytesToMappableResource(&uncompressedData[0], 24, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(49).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 24);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 50
void CreateAndInitResource_50()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 496, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(50, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 196608);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 39);

    {
        auto destAlloc = g_uploadBuffers->Allocate(496);
        CopyBytesToMappableResource(&uncompressedData[0], 496, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(50).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 496);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 51
void CreateAndInitResource_51()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 36, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(51, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 0);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 64);

    {
        auto destAlloc = g_uploadBuffers->Allocate(36);
        CopyBytesToMappableResource(&uncompressedData[0], 36, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(51).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 36);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 52
void CreateAndInitResource_52()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 2304, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(52, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 131072);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 39);

    {
        auto destAlloc = g_uploadBuffers->Allocate(2304);
        CopyBytesToMappableResource(&uncompressedData[0], 2304, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(52).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 2304);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 53
void CreateAndInitResource_53()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 552, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(53, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 786432);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 267);

    {
        auto destAlloc = g_uploadBuffers->Allocate(552);
        CopyBytesToMappableResource(&uncompressedData[0], 552, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(53).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 552);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 54
void CreateAndInitResource_54()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 1156536, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(54, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 3145728);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 600912);

    {
        auto destAlloc = g_uploadBuffers->Allocate(1156536);
        CopyBytesToMappableResource(&uncompressedData[0], 1156536, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(54).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 1156536);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 55
void CreateAndInitResource_55()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 4, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(55, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 5963776);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 32);

    {
        auto destAlloc = g_uploadBuffers->Allocate(4);
        CopyBytesToMappableResource(&uncompressedData[0], 4, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(55).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 4);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 56
void CreateAndInitResource_56()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, 4, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(56, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 6029312);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 32);

    {
        auto destAlloc = g_uploadBuffers->Allocate(4);
        CopyBytesToMappableResource(&uncompressedData[0], 4, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(56).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 4);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 57
void CreateAndInitResource_57()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 289134, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(57, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 5636096);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 261843);

    {
        auto destAlloc = g_uploadBuffers->Allocate(289134);
        CopyBytesToMappableResource(&uncompressedData[0], 289134, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(57).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 289134);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 58
void CreateAndInitResource_58()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 39040, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(58, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 4325376);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 32848);

    {
        auto destAlloc = g_uploadBuffers->Allocate(39040);
        CopyBytesToMappableResource(&uncompressedData[0], 39040, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(58).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 39040);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 59
void CreateAndInitResource_59()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 1196544, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(59, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 4390912);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 225687);

    {
        auto destAlloc = g_uploadBuffers->Allocate(1196544);
        CopyBytesToMappableResource(&uncompressedData[0], 1196544, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(59).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 1196544);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 60
void CreateAndInitResource_60()
{
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 65536, 2252160, 1, 1, 1, DXGI_FORMAT_UNKNOWN, { 1, 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS };
    CreateAndTrackPlacedResource(60, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, 35, 851968);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 1821819);

    {
        auto destAlloc = g_uploadBuffers->Allocate(2252160);
        CopyBytesToMappableResource(&uncompressedData[0], 2252160, destAlloc.GetResource(), destAlloc.GetOffset());
        g_utilityCommandList->CopyBufferRegion(GetResource(60).Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), 2252160);
    }
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 66
void CreateAndInitResource_66()
{
    static D3D12_CLEAR_VALUE clearValName = {};
    clearValName.Format = DXGI_FORMAT_D32_FLOAT;
    clearValName.DepthStencil = { 1.00000f, 1 };
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, 800, 800, 1, 1, DXGI_FORMAT_D32_FLOAT, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL };
    CreateAndTrackPlacedResource(66, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValName, 65, 8650752);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 81729);

    D3D12_RESOURCE_BARRIER resourceTransitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(GetResource(66).Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, 4294967295, D3D12_RESOURCE_BARRIER_FLAG_NONE);
    g_utilityCommandList->ResourceBarrier(1, &resourceTransitionBarrier);

    {
        auto destAlloc = g_uploadBuffers->Allocate(2662272);
        CopyBytesToMappableResource(&uncompressedData[0], 2662272, destAlloc.GetResource(), destAlloc.GetOffset());
        for (auto i = 0u; i < 1; ++i)
        {
            const auto& resourceInitInfo = g_resourceInitInfo_66_0[i];
            CreateTextureCopyLocation_PlacedFootprint(g_textureCopyLocationPlacedFootprint, destAlloc.GetResource(), resourceInitInfo.Offset + destAlloc.GetOffset(), resourceInitInfo.SubresourceFootprint);
            CreateTextureCopyLocation_SubresourceIndex(g_textureCopyLocationSubresourceIndex, GetResource(66).Get(), resourceInitInfo.SubresourceIndex);
            g_utilityCommandList->CopyTextureRegion(&g_textureCopyLocationSubresourceIndex, 0, 0, 0, &g_textureCopyLocationPlacedFootprint, nullptr);
        }
    }
    TransitionToFinalStates_EnhancedBarriers(GetResource(66).Get(), D3D12_RESOURCE_STATE_COPY_DEST, g_resourceFinalStates_66, 1);
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 67
void CreateAndInitResource_67()
{
    static D3D12_CLEAR_VALUE clearValName = {};
    clearValName.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    clearValName.Color[0] = 0.00000f;
    clearValName.Color[1] = 0.00000f;
    clearValName.Color[2] = 0.00000f;
    clearValName.Color[3] = 1.00000f;
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, 800, 800, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
    CreateAndTrackPlacedResource(67, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValName, 65, 2883584);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 81729);

    D3D12_RESOURCE_BARRIER resourceTransitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(GetResource(67).Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, 4294967295, D3D12_RESOURCE_BARRIER_FLAG_NONE);
    g_utilityCommandList->ResourceBarrier(1, &resourceTransitionBarrier);

    {
        auto destAlloc = g_uploadBuffers->Allocate(2662272);
        CopyBytesToMappableResource(&uncompressedData[0], 2662272, destAlloc.GetResource(), destAlloc.GetOffset());
        for (auto i = 0u; i < 1; ++i)
        {
            const auto& resourceInitInfo = g_resourceInitInfo_67_0[i];
            CreateTextureCopyLocation_PlacedFootprint(g_textureCopyLocationPlacedFootprint, destAlloc.GetResource(), resourceInitInfo.Offset + destAlloc.GetOffset(), resourceInitInfo.SubresourceFootprint);
            CreateTextureCopyLocation_SubresourceIndex(g_textureCopyLocationSubresourceIndex, GetResource(67).Get(), resourceInitInfo.SubresourceIndex);
            g_utilityCommandList->CopyTextureRegion(&g_textureCopyLocationSubresourceIndex, 0, 0, 0, &g_textureCopyLocationPlacedFootprint, nullptr);
        }
    }
    TransitionToFinalStates_EnhancedBarriers(GetResource(67).Get(), D3D12_RESOURCE_STATE_COPY_DEST, g_resourceFinalStates_67, 1);
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 68
void CreateAndInitResource_68()
{
    static D3D12_CLEAR_VALUE clearValName = {};
    clearValName.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    clearValName.Color[0] = 0.00000f;
    clearValName.Color[1] = 0.00000f;
    clearValName.Color[2] = 0.00000f;
    clearValName.Color[3] = 1.00000f;
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, 800, 800, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
    CreateAndTrackPlacedResource(68, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValName, 65, 0);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 85179);

    D3D12_RESOURCE_BARRIER resourceTransitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(GetResource(68).Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, 4294967295, D3D12_RESOURCE_BARRIER_FLAG_NONE);
    g_utilityCommandList->ResourceBarrier(1, &resourceTransitionBarrier);

    {
        auto destAlloc = g_uploadBuffers->Allocate(2662272);
        CopyBytesToMappableResource(&uncompressedData[0], 2662272, destAlloc.GetResource(), destAlloc.GetOffset());
        for (auto i = 0u; i < 1; ++i)
        {
            const auto& resourceInitInfo = g_resourceInitInfo_68_0[i];
            CreateTextureCopyLocation_PlacedFootprint(g_textureCopyLocationPlacedFootprint, destAlloc.GetResource(), resourceInitInfo.Offset + destAlloc.GetOffset(), resourceInitInfo.SubresourceFootprint);
            CreateTextureCopyLocation_SubresourceIndex(g_textureCopyLocationSubresourceIndex, GetResource(68).Get(), resourceInitInfo.SubresourceIndex);
            g_utilityCommandList->CopyTextureRegion(&g_textureCopyLocationSubresourceIndex, 0, 0, 0, &g_textureCopyLocationPlacedFootprint, nullptr);
        }
    }
    TransitionToFinalStates_EnhancedBarriers(GetResource(68).Get(), D3D12_RESOURCE_STATE_COPY_DEST, g_resourceFinalStates_68, 1);
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 69
void CreateAndInitResource_69()
{
    static D3D12_CLEAR_VALUE clearValName = {};
    clearValName.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    clearValName.Color[0] = 0.00000f;
    clearValName.Color[1] = 0.00000f;
    clearValName.Color[2] = 0.00000f;
    clearValName.Color[3] = 1.00000f;
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, 800, 800, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
    CreateAndTrackPlacedResource(69, g_device.Get(), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValName, 65, 5767168);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 81729);

    D3D12_RESOURCE_BARRIER resourceTransitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(GetResource(69).Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, 4294967295, D3D12_RESOURCE_BARRIER_FLAG_NONE);
    g_utilityCommandList->ResourceBarrier(1, &resourceTransitionBarrier);

    {
        auto destAlloc = g_uploadBuffers->Allocate(2662272);
        CopyBytesToMappableResource(&uncompressedData[0], 2662272, destAlloc.GetResource(), destAlloc.GetOffset());
        for (auto i = 0u; i < 1; ++i)
        {
            const auto& resourceInitInfo = g_resourceInitInfo_69_0[i];
            CreateTextureCopyLocation_PlacedFootprint(g_textureCopyLocationPlacedFootprint, destAlloc.GetResource(), resourceInitInfo.Offset + destAlloc.GetOffset(), resourceInitInfo.SubresourceFootprint);
            CreateTextureCopyLocation_SubresourceIndex(g_textureCopyLocationSubresourceIndex, GetResource(69).Get(), resourceInitInfo.SubresourceIndex);
            g_utilityCommandList->CopyTextureRegion(&g_textureCopyLocationSubresourceIndex, 0, 0, 0, &g_textureCopyLocationPlacedFootprint, nullptr);
        }
    }
    TransitionToFinalStates_EnhancedBarriers(GetResource(69).Get(), D3D12_RESOURCE_STATE_COPY_DEST, g_resourceFinalStates_69, 1);
    CloseAndExecuteUtilityCommandListIfPossible();
}

// ApiObjectId     = 82
void CreateAndInitResource_82()
{
    static D3D12_HEAP_PROPERTIES heapProperties { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0 };
    static D3D12_RESOURCE_DESC resourceDesc = { D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, 800, 800, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, { 1, 0 }, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET };
    CreateAndTrackCommittedResource(82, g_device.Get(), &heapProperties, D3D12HeapFlags(D3D12_HEAP_FLAG_SHARED | D3D12_HEAP_FLAG_ALLOW_DISPLAY), &resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr);

    std::vector<BYTE> uncompressedData;
    g_resourceReader->Read(uncompressedData, 43);

    D3D12_RESOURCE_BARRIER resourceTransitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(GetResource(82).Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST, 4294967295, D3D12_RESOURCE_BARRIER_FLAG_NONE);
    g_utilityCommandList->ResourceBarrier(1, &resourceTransitionBarrier);

    {
        auto destAlloc = g_uploadBuffers->Allocate(2662272);
        CopyBytesToMappableResource(&uncompressedData[0], 2662272, destAlloc.GetResource(), destAlloc.GetOffset());
        for (auto i = 0u; i < 1; ++i)
        {
            const auto& resourceInitInfo = g_resourceInitInfo_82_0[i];
            CreateTextureCopyLocation_PlacedFootprint(g_textureCopyLocationPlacedFootprint, destAlloc.GetResource(), resourceInitInfo.Offset + destAlloc.GetOffset(), resourceInitInfo.SubresourceFootprint);
            CreateTextureCopyLocation_SubresourceIndex(g_textureCopyLocationSubresourceIndex, GetResource(82).Get(), resourceInitInfo.SubresourceIndex);
            g_utilityCommandList->CopyTextureRegion(&g_textureCopyLocationSubresourceIndex, 0, 0, 0, &g_textureCopyLocationPlacedFootprint, nullptr);
        }
    }
    std::vector<D3D12_RESOURCE_BARRIER> barriers;
    for (auto i = 0u; i < 1; ++i)
    {
        if (g_resourceFinalStates_82[i] != D3D12_RESOURCE_STATE_COPY_DEST)
        {
            barriers.emplace_back(CD3DX12_RESOURCE_BARRIER::Transition(GetResource(82).Get(), D3D12_RESOURCE_STATE_COPY_DEST, g_resourceFinalStates_82[i], i, D3D12_RESOURCE_BARRIER_FLAG_NONE));
        }
    }
    g_utilityCommandList->ResourceBarrier((UINT)barriers.size(), barriers.data());
    CloseAndExecuteUtilityCommandListIfPossible();
}

void CreateIndirectArgumentBuffer_46_1()
{
    ComPtr<ID3D12Resource> argumentBuffer;
    D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(12);
    ThrowIfFailed(g_device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, nullptr, IID_PPV_ARGS(&argumentBuffer)));
    g_indirectArgumentBuffers["46_1"] = argumentBuffer;

    std::vector<byte> commands(12);
    byte* dstCommand = commands.data();
    byte* dstPtr = nullptr;
    dstPtr = dstCommand;
    {
        auto* dstArg = reinterpret_cast<D3D12_DISPATCH_ARGUMENTS*>(dstPtr);
        *dstArg = { 0, 0, 0 };
        dstPtr += sizeof(*dstArg);
    }
    dstCommand += 12;

    if (commands.size() > 0)
    {
        auto destAlloc = g_uploadBuffers->Allocate(commands.size());
        CopyBytesToMappableResource(commands.data(), commands.size(), destAlloc.GetResource(), destAlloc.GetOffset());
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(argumentBuffer.Get(), D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, D3D12_RESOURCE_STATE_COPY_DEST, 0);
        g_utilityCommandList->ResourceBarrier(1, &barrier);
        g_utilityCommandList->CopyBufferRegion(argumentBuffer.Get(), 0, destAlloc.GetResource(), destAlloc.GetOffset(), commands.size());
        barrier = CD3DX12_RESOURCE_BARRIER::Transition(argumentBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, 0);
        g_utilityCommandList->ResourceBarrier(1, &barrier);
    }
}

