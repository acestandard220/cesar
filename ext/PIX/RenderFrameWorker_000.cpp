#pragma once

#include "pch.h"
#include "RenderFrame.h"
#include "FrameResources.h"
#include "ResourceModifications.h"
#include "Helpers.h"

using namespace Helpers;

void RenderFrame_000()
{
    // ApiObjectId     = 9
    // GlobalId        = 1
    // GetCommandQueue(1)->Wait(GetFence(3).Get(), 918);

    // GlobalId        = 2
    // GetCommandQueue(1)->Wait(GetFence(4).Get(), 917);

    // GlobalId        = 3
    ThrowIfFailed(GetCommandQueue(1)->Signal(GetFence(5).Get(), 920));

    // GlobalId        = 4
    // GetCommandQueue(6)->Wait(GetFence(5).Get(), 920);

    PopulateCommandList_7_1_0();
    {
        ID3D12CommandList* commandLists[1];
        commandLists[0] = GetCommandList(7).Get();
        GetCommandQueue(6)->ExecuteCommandLists(_countof(commandLists), commandLists);
        FlushCommandQueue(6);
    }

    // GlobalId        = 8
    ThrowIfFailed(GetCommandQueue(6)->Signal(GetFence(3).Get(), 920));

    PopulateCommandList_13_1_0();
    {
        ID3D12CommandList* commandLists[1];
        commandLists[0] = GetCommandList(13).Get();
        GetCommandQueue(12)->ExecuteCommandLists(_countof(commandLists), commandLists);
        FlushCommandQueue(12);
    }

    // GlobalId        = 21
    ThrowIfFailed(GetCommandQueue(6)->Signal(GetFence(61).Get(), 920));

    // GlobalId        = 22
    ThrowIfFailed(GetCommandQueue(12)->Signal(GetFence(62).Get(), 921));

    PopulateCommandList_63_1_0();
    {
        ID3D12CommandList* commandLists[1];
        commandLists[0] = GetCommandList(63).Get();
        GetCommandQueue(1)->ExecuteCommandLists(_countof(commandLists), commandLists);
        FlushCommandQueue(1);
    }

    PopulateCommandList_79_1_0();
    {
        ID3D12CommandList* commandLists[1];
        commandLists[0] = GetCommandList(79).Get();
        GetCommandQueue(78)->ExecuteCommandLists(_countof(commandLists), commandLists);
        FlushCommandQueue(78);
    }

    // GlobalId        = 41
    ThrowIfFailed(GetCommandQueue(1)->Signal(GetFence(81).Get(), 921));

    // GlobalId        = 42
    {
        ThrowIfFailed(g_utilityCommandList->Reset(g_utilityCommandAllocator.Get(), nullptr));
        ComPtr<ID3D12Resource> backBuffer;
        GetSwapChain(1)->GetBuffer(GetSwapChain(1)->GetCurrentBackBufferIndex(), IID_PPV_ARGS(&backBuffer));

        D3D12_RESOURCE_BARRIER barrier { D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE, { backBuffer.Get(), 0, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST } };
        g_utilityCommandList->ResourceBarrier(1, &barrier);
        g_utilityCommandList->CopyResource(backBuffer.Get(), GetResource(82).Get());
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
        g_utilityCommandList->ResourceBarrier(1, &barrier);
        g_utilityCommandList->Close();

        ID3D12CommandList* commandLists[1];
        commandLists[0] = g_utilityCommandList.Get();
        GetCommandQueue(1)->ExecuteCommandLists(_countof(commandLists), commandLists);
        ThrowIfFailed(GetSwapChain(1)->Present(1, 0));
        FlushCommandQueue(1);
        FlushUtilityCommandQueue(g_utilityCommandQueue.Get());
    }

    }
