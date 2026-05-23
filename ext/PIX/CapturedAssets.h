#pragma once

#include "ResourceReader.h"
#include "UploadBuffers.h"

using Microsoft::WRL::ComPtr;

struct D3D12Heap
{
    ComPtr<ID3D12Heap> Heap;
    std::optional<D3D12_GPU_VIRTUAL_ADDRESS> BaseGpuva;
};

struct ResourceInitInfo
{
    UINT64 Offset;
    D3D12_SUBRESOURCE_FOOTPRINT SubresourceFootprint;
    UINT SubresourceIndex;
};

extern UINT g_windowWidth;
extern UINT g_windowHeight;
extern UINT g_fenceValue;
extern UINT g_frameIndex;
extern UINT g_rtvDescriptorSize;
extern UINT g_cbvSrvUavDescriptorSize;
extern UINT g_samplerDescriptorSize;
extern UINT g_dsvDescriptorSize;
extern UINT g_resourcesCount;
extern UINT g_maxTmpCbvDescriptors;
extern UINT g_maxTmpSrvDescriptors;
extern UINT g_maxTmpUavDescriptors;
extern UINT64 g_lastFenceSignaled;
extern bool g_constructionNeeded;
extern std::wstring g_captureTimeAdapterDesc;
extern D3D_FEATURE_LEVEL g_minFeatureLevel;

extern D3D12_TEXTURE_COPY_LOCATION g_textureCopyLocationPlacedFootprint;
extern D3D12_TEXTURE_COPY_LOCATION g_textureCopyLocationSubresourceIndex;

extern ComPtr<IDXGIFactory> g_dxgiFactory;
extern ComPtr<ID3D12Device5> g_device;
extern ComPtr<ID3D12CommandAllocator> g_utilityCommandAllocator;
extern ComPtr<ID3D12GraphicsCommandList4> g_utilityCommandList;
extern ComPtr<ID3D12CommandQueue> g_utilityCommandQueue;
extern ComPtr<ID3D12CommandAllocator> g_utilityComputeCommandAllocator;
extern ComPtr<ID3D12GraphicsCommandList4> g_utilityComputeCommandList;
extern ComPtr<ID3D12CommandQueue> g_utilityComputeCommandQueue;
extern ComPtr<ID3D12Fence> g_fence;
extern ComPtr<ID3D12DescriptorHeap> g_rtvHeap;
extern ComPtr<ID3D12DescriptorHeap> g_dsvHeap;
extern ComPtr<ID3D12DescriptorHeap> g_tmpUavDescriptorHeap;
extern ComPtr<ID3D12DescriptorHeap> g_tmpCbvDescriptorHeap;
extern ComPtr<ID3D12DescriptorHeap> g_tmpSrvDescriptorHeap;
extern std::unordered_map<ApiObjectId, std::pair<ComPtr<ID3D12Fence>, UINT64>> g_utilityFencesInfo;

extern std::unique_ptr<ResourceReader> g_resourceReader;
extern std::unique_ptr<UploadBuffers::Allocator> g_uploadBuffers;

extern std::vector<ComPtr<ID3D12Resource>> g_stagingResources;

extern std::unordered_map<ApiObjectId, ComPtr<ID3D12Resource>> g_resources;
extern std::unordered_map<std::string, ComPtr<ID3D12Resource>> g_indirectArgumentBuffers;
extern std::unordered_map<std::string, ComPtr<ID3D12Resource>> g_countBuffers;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12GraphicsCommandList6>> g_commandLists;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12CommandAllocator>> g_commandAllocators;
extern std::unordered_set<ApiObjectId> g_bundleAllocatorsHandles;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12CommandQueue>> g_commandQueues;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12Fence>> g_fences;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12RootSignature>> g_rootSignatures;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12CommandSignature>> g_commandSignatures;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12PipelineState>> g_psos;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12DescriptorHeap>> g_descHeaps;
extern std::unordered_map<ApiObjectId, D3D12Heap> g_heaps;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12QueryHeap>> g_queryHeaps;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12StateObject>> g_stateObjects;
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12MetaCommand>> g_metaCommands;
extern std::unordered_map<ApiObjectId, ComPtr<IDXGISwapChain3>> g_swapChains;
extern std::unordered_map<ApiObjectId, HWND> g_hWnds;
#if defined(DIRECT3D_EXPERIMENTAL_BUILD)
extern std::unordered_map<ApiObjectId, ComPtr<ID3D12Extension>> g_d3dExtensions;
#endif

extern std::vector<ComPtr<ID3D12Resource>> g_perFrameBuffers;
        
static ResourceInitInfo g_resourceInitInfo_10_0[] = 
{
    { 0, { DXGI_FORMAT_R8G8B8A8_UNORM, 800, 800, 1, 3328 }, 0 }
};

static ResourceInitInfo g_resourceInitInfo_11_0[] = 
{
    { 0, { DXGI_FORMAT_R8G8B8A8_UNORM, 800, 800, 1, 3328 }, 0 }
};

static D3D12_RESOURCE_STATES g_resourceFinalStates_11[] = 
{
    D3D12_RESOURCE_STATE_COMMON
};

static ResourceInitInfo g_resourceInitInfo_66_0[] = 
{
    { 0, { DXGI_FORMAT_D32_FLOAT, 800, 800, 1, 3328 }, 0 }
};

static std::variant<D3D12_RESOURCE_STATES, D3D12_BARRIER_LAYOUT> g_resourceFinalStates_66[] = 
{
    D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE
};

static ResourceInitInfo g_resourceInitInfo_67_0[] = 
{
    { 0, { DXGI_FORMAT_R8G8B8A8_UNORM, 800, 800, 1, 3328 }, 0 }
};

static std::variant<D3D12_RESOURCE_STATES, D3D12_BARRIER_LAYOUT> g_resourceFinalStates_67[] = 
{
    D3D12_BARRIER_LAYOUT_COMMON
};

static ResourceInitInfo g_resourceInitInfo_68_0[] = 
{
    { 0, { DXGI_FORMAT_R8G8B8A8_UNORM, 800, 800, 1, 3328 }, 0 }
};

static std::variant<D3D12_RESOURCE_STATES, D3D12_BARRIER_LAYOUT> g_resourceFinalStates_68[] = 
{
    D3D12_BARRIER_LAYOUT_COMMON
};

static ResourceInitInfo g_resourceInitInfo_69_0[] = 
{
    { 0, { DXGI_FORMAT_R8G8B8A8_UNORM, 800, 800, 1, 3328 }, 0 }
};

static std::variant<D3D12_RESOURCE_STATES, D3D12_BARRIER_LAYOUT> g_resourceFinalStates_69[] = 
{
    D3D12_BARRIER_LAYOUT_COMMON
};

static ResourceInitInfo g_resourceInitInfo_82_0[] = 
{
    { 0, { DXGI_FORMAT_R8G8B8A8_UNORM, 800, 800, 1, 3328 }, 0 }
};

static D3D12_RESOURCE_STATES g_resourceFinalStates_82[] = 
{
    D3D12_RESOURCE_STATE_COMMON
};

extern ComPtr<ID3D12DescriptorHeap> g_descriptorHeap_75;
extern ComPtr<ID3D12DescriptorHeap> g_descriptorHeap_76;
