#pragma once

#include "pch.h"
#include "FrameResources.h"

using namespace Helpers;

void CreateDescriptors_75_0()
{
    CreateUnorderedAccessView_Buffer(GetResource(55).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 0), DXGI_FORMAT_UNKNOWN, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 4, 0, D3D12_BUFFER_UAV_FLAG_NONE);
    CreateShaderResourceView_Buffer(GetResource(55).Get(), GetCpuDescriptor(g_descriptorHeap_75.Get(), 1), DXGI_FORMAT_UNKNOWN, D3D12_SRV_DIMENSION_BUFFER, 5768, 0, 1, 4, D3D12_BUFFER_SRV_FLAG_NONE);
    CreateUnorderedAccessView_Buffer(GetResource(56).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 2), DXGI_FORMAT_R32_TYPELESS, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 0, 0, D3D12_BUFFER_UAV_FLAG_RAW);
    CreateShaderResourceView_Buffer(GetResource(56).Get(), GetCpuDescriptor(g_descriptorHeap_75.Get(), 3), DXGI_FORMAT_R32_TYPELESS, D3D12_SRV_DIMENSION_BUFFER, 5768, 0, 1, 0, D3D12_BUFFER_SRV_FLAG_RAW);
    CreateUnorderedAccessView_Buffer(GetResource(47).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 4), DXGI_FORMAT_UNKNOWN, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 4, 0, D3D12_BUFFER_UAV_FLAG_NONE);
    CreateShaderResourceView_Buffer(GetResource(47).Get(), GetCpuDescriptor(g_descriptorHeap_75.Get(), 5), DXGI_FORMAT_UNKNOWN, D3D12_SRV_DIMENSION_BUFFER, 5768, 0, 1, 4, D3D12_BUFFER_SRV_FLAG_NONE);
    CreateUnorderedAccessView_Buffer(GetResource(44).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 6), DXGI_FORMAT_UNKNOWN, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 4, 0, D3D12_BUFFER_UAV_FLAG_NONE);
    CreateShaderResourceView_Buffer(GetResource(44).Get(), GetCpuDescriptor(g_descriptorHeap_75.Get(), 7), DXGI_FORMAT_UNKNOWN, D3D12_SRV_DIMENSION_BUFFER, 5768, 0, 1, 4, D3D12_BUFFER_SRV_FLAG_NONE);
    CreateUnorderedAccessView_Buffer(GetResource(46).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 8), DXGI_FORMAT_UNKNOWN, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 12, 0, D3D12_BUFFER_UAV_FLAG_NONE);
    CreateShaderResourceView_Buffer(GetResource(46).Get(), GetCpuDescriptor(g_descriptorHeap_75.Get(), 9), DXGI_FORMAT_UNKNOWN, D3D12_SRV_DIMENSION_BUFFER, 5768, 0, 1, 12, D3D12_BUFFER_SRV_FLAG_NONE);
    CreateUnorderedAccessView_Buffer(GetResource(45).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 10), DXGI_FORMAT_UNKNOWN, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 8, 0, D3D12_BUFFER_UAV_FLAG_NONE);
    CreateUnorderedAccessView_Buffer(GetResource(48).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 11), DXGI_FORMAT_R32_TYPELESS, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 0, 0, D3D12_BUFFER_UAV_FLAG_RAW);
    CreateUnorderedAccessView_Buffer(GetResource(44).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 12), DXGI_FORMAT_UNKNOWN, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 4, 0, D3D12_BUFFER_UAV_FLAG_NONE);
    CreateShaderResourceView_Buffer(GetResource(44).Get(), GetCpuDescriptor(g_descriptorHeap_75.Get(), 13), DXGI_FORMAT_UNKNOWN, D3D12_SRV_DIMENSION_BUFFER, 5768, 0, 1, 4, D3D12_BUFFER_SRV_FLAG_NONE);
    CreateUnorderedAccessView_Buffer(GetResource(46).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 14), DXGI_FORMAT_UNKNOWN, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 12, 0, D3D12_BUFFER_UAV_FLAG_NONE);
    CreateShaderResourceView_Buffer(GetResource(46).Get(), GetCpuDescriptor(g_descriptorHeap_75.Get(), 15), DXGI_FORMAT_UNKNOWN, D3D12_SRV_DIMENSION_BUFFER, 5768, 0, 1, 12, D3D12_BUFFER_SRV_FLAG_NONE);
    CreateUnorderedAccessView_Buffer(GetResource(45).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 16), DXGI_FORMAT_UNKNOWN, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 8, 0, D3D12_BUFFER_UAV_FLAG_NONE);
    CreateUnorderedAccessView_Buffer(GetResource(48).Get(), nullptr, GetCpuDescriptor(g_descriptorHeap_75.Get(), 17), DXGI_FORMAT_R32_TYPELESS, D3D12_UAV_DIMENSION_BUFFER, 0, 1, 0, 0, D3D12_BUFFER_UAV_FLAG_RAW);
}
void CreateDescriptors_76_0()
{
}
