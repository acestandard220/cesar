#pragma once

#include "pch.h"
#include "FrameResources.h"

using namespace Helpers;

void CreateAppResources_000()
{
    if (g_constructionNeeded)
    {
        g_captureTimeAdapterDesc = LR"(NVIDIA GeForce RTX 5050 Laptop GPU)";
        g_minFeatureLevel = D3D_FEATURE_LEVEL_12_1;
        ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&g_dxgiFactory)));
        ThrowIfFailed(D3D12CreateDevice(GetPlaybackAdapter(g_captureTimeAdapterDesc).Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&g_device)));
        g_uploadBuffers = std::make_unique<::UploadBuffers::Allocator>(g_device.Get());
    }

    
    
    auto utilityCmdListType = (g_minFeatureLevel == D3D_FEATURE_LEVEL_1_0_CORE) ? D3D12_COMMAND_LIST_TYPE_COMPUTE : D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed(g_device->CreateCommandAllocator(utilityCmdListType, IID_PPV_ARGS(&g_utilityCommandAllocator)));
    ThrowIfFailed(g_device->CreateCommandList(0, utilityCmdListType, g_utilityCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&g_utilityCommandList)));
    {
        D3D12_COMMAND_QUEUE_DESC qDesc = { utilityCmdListType, 0, D3D12_COMMAND_QUEUE_FLAG_NONE, 0 };
        ThrowIfFailed(g_device->CreateCommandQueue(&qDesc, IID_PPV_ARGS(&g_utilityCommandQueue)));
    };
    
    ThrowIfFailed(g_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&g_utilityComputeCommandAllocator)));
    ThrowIfFailed(g_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, g_utilityComputeCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&g_utilityComputeCommandList)));
    {
        D3D12_COMMAND_QUEUE_DESC qDesc = { D3D12_COMMAND_LIST_TYPE_COMPUTE, 0, D3D12_COMMAND_QUEUE_FLAG_NONE, 0 };
        ThrowIfFailed(g_device->CreateCommandQueue(&qDesc, IID_PPV_ARGS(&g_utilityComputeCommandQueue)));
    };
    
    ThrowIfFailed(g_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)));
            
    
    if (g_minFeatureLevel != D3D_FEATURE_LEVEL_1_0_CORE)
    {   // Create descriptor heaps and compute descriptors size.
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(g_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_rtvHeap)));
    
        // Describe and create a depth stencil view (DSV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(g_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&g_dsvHeap)));
    }
    g_rtvDescriptorSize = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    g_cbvSrvUavDescriptorSize = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    g_samplerDescriptorSize = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    g_dsvDescriptorSize = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                
    if (g_constructionNeeded)
    {
        // ApiObjectId     = 1
        D3D12_COMMAND_QUEUE_DESC qDesc = { D3D12_COMMAND_LIST_TYPE_DIRECT, 0, D3D12_COMMAND_QUEUE_FLAG_NONE, 0 };
        CreateAndTrackCommandQueue(1, g_device.Get(), &qDesc);
    }

    if (g_constructionNeeded)
    {
        // ApiObjectId     = 6
        D3D12_COMMAND_QUEUE_DESC qDesc = { D3D12_COMMAND_LIST_TYPE_COPY, 0, D3D12_COMMAND_QUEUE_FLAG_NONE, 1 };
        CreateAndTrackCommandQueue(6, g_device.Get(), &qDesc);
    }

    if (g_constructionNeeded)
    {
        // ApiObjectId     = 12
        D3D12_COMMAND_QUEUE_DESC qDesc = { D3D12_COMMAND_LIST_TYPE_COPY, 0, D3D12_COMMAND_QUEUE_FLAG_NONE, 0 };
        CreateAndTrackCommandQueue(12, g_device.Get(), &qDesc);
    }

    if (g_constructionNeeded)
    {
        // ApiObjectId     = 78
        D3D12_COMMAND_QUEUE_DESC qDesc = { D3D12_COMMAND_LIST_TYPE_COMPUTE, 0, D3D12_COMMAND_QUEUE_FLAG_NONE, 0 };
        CreateAndTrackCommandQueue(78, g_device.Get(), &qDesc);
    }

    // ApiObjectId     = 8
    CreateAndTrackCommandAllocator(8, g_device.Get(), D3D12_COMMAND_LIST_TYPE_COPY);

    // ApiObjectId     = 14
    CreateAndTrackCommandAllocator(14, g_device.Get(), D3D12_COMMAND_LIST_TYPE_COPY);

    // ApiObjectId     = 64
    CreateAndTrackCommandAllocator(64, g_device.Get(), D3D12_COMMAND_LIST_TYPE_DIRECT);

    // ApiObjectId     = 80
    CreateAndTrackCommandAllocator(80, g_device.Get(), D3D12_COMMAND_LIST_TYPE_COMPUTE);

    // ApiObjectId     = 7
    CreateAndTrackCommandList1(7, g_device.Get(), 1, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE);

    // ApiObjectId     = 13
    CreateAndTrackCommandList1(13, g_device.Get(), 0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE);

    // ApiObjectId     = 63
    CreateAndTrackCommandList1(63, g_device.Get(), 0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE);

    // ApiObjectId     = 79
    CreateAndTrackCommandList1(79, g_device.Get(), 0, D3D12_COMMAND_LIST_TYPE_COMPUTE, D3D12_COMMAND_LIST_FLAG_NONE);

    // ApiObjectId     = 3
    CreateAndTrackFence(3, g_device.Get(), D3D12_FENCE_FLAG_NONE, 919);

    // ApiObjectId     = 4
    CreateAndTrackFence(4, g_device.Get(), D3D12_FENCE_FLAG_SHARED | D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER, 918);

    // ApiObjectId     = 5
    CreateAndTrackFence(5, g_device.Get(), D3D12_FENCE_FLAG_NONE, 919);

    // ApiObjectId     = 61
    CreateAndTrackFence(61, g_device.Get(), D3D12_FENCE_FLAG_SHARED | D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER, 919);

    // ApiObjectId     = 62
    CreateAndTrackFence(62, g_device.Get(), D3D12_FENCE_FLAG_NONE, 920);

    // ApiObjectId     = 81
    CreateAndTrackFence(81, g_device.Get(), D3D12_FENCE_FLAG_NONE, 920);

    // ApiObjectId     = 71
    {
        static D3D12_ROOT_PARAMETER1 rootParameters[2];
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[0].Descriptor = { 0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE };
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        rootParameters[1].Constants = { 1, 0, 10 };
        D3D12_STATIC_SAMPLER_DESC samplers[1];
        samplers[0] = { D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.00000f, 16, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.00000f, FLT_MAX, 1, 0, D3D12_SHADER_VISIBILITY_ALL };
        D3D12_ROOT_SIGNATURE_DESC1 rootSignatureDesc = { 2, rootParameters, 1, samplers, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED };
        D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSigDesc = {};
        versionedRootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        versionedRootSigDesc.Desc_1_1 = rootSignatureDesc;
        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3D12SerializeVersionedRootSignature(&versionedRootSigDesc, &signature, &error));
        CreateAndTrackRootSignature(71, g_device.Get(), 0, signature.Get());
    }

    // ApiObjectId     = 77
    {
        static D3D12_INDIRECT_ARGUMENT_DESC argumentDescs[1] = {};
        argumentDescs[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
        static D3D12_COMMAND_SIGNATURE_DESC commandSignatureDesc = {};
        commandSignatureDesc.pArgumentDescs = argumentDescs;
        commandSignatureDesc.NumArgumentDescs = _countof(argumentDescs);
        commandSignatureDesc.ByteStride = 12;
        CreateAndTrackCommandSignature(77, GetRootSignature(0), g_device.Get(), &commandSignatureDesc);
    }

    CreateComputePipelineState_70();

    CreateComputePipelineState_72();

    CreateComputePipelineState_73();

    CreatePipelineState_74();

    // ApiObjectId     = 15
    {
        static D3D12_HEAP_PROPERTIES heapProperties { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0 };
        D3D12_HEAP_DESC heapDesc = {};
        heapDesc.SizeInBytes = 8388608;
        heapDesc.Properties = heapProperties;
        heapDesc.Alignment = 4194304;
        heapDesc.Flags = D3D12HeapFlags(D3D12_HEAP_FLAG_NONE);
        CreateAndTrackHeap(15, g_device.Get(), &heapDesc);
    }
    // ApiObjectId     = 35
    {
        static D3D12_HEAP_PROPERTIES heapProperties { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0 };
        D3D12_HEAP_DESC heapDesc = {};
        heapDesc.SizeInBytes = 8388608;
        heapDesc.Properties = heapProperties;
        heapDesc.Alignment = 4194304;
        heapDesc.Flags = D3D12HeapFlags(D3D12_HEAP_FLAG_NONE);
        CreateAndTrackHeap(35, g_device.Get(), &heapDesc);
    }
    // ApiObjectId     = 65
    {
        static D3D12_HEAP_PROPERTIES heapProperties { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0 };
        D3D12_HEAP_DESC heapDesc = {};
        heapDesc.SizeInBytes = 16777216;
        heapDesc.Properties = heapProperties;
        heapDesc.Alignment = 4194304;
        heapDesc.Flags = D3D12HeapFlags(D3D12_HEAP_FLAG_NONE);
        CreateAndTrackHeap(65, g_device.Get(), &heapDesc);
    }
    CreateAndInitResource_10();
    CreateAndInitResource_11();
    CreateAndInitResource_16();
    CreateAndInitResource_17();
    CreateAndInitResource_18();
    CreateAndInitResource_19();
    CreateAndInitResource_20();
    CreateAndInitResource_21();
    CreateAndInitResource_22();
    CreateAndInitResource_23();
    CreateAndInitResource_24();
    CreateAndInitResource_25();
    CreateAndInitResource_26();
    CreateAndInitResource_27();
    CreateAndInitResource_28();
    CreateAndInitResource_29();
    CreateAndInitResource_30();
    CreateAndInitResource_31();
    CreateAndInitResource_32();
    CreateAndInitResource_33();
    CreateAndInitResource_34();
    CreateAndInitResource_36();
    CreateAndInitResource_37();
    CreateAndInitResource_38();
    CreateAndInitResource_39();
    CreateAndInitResource_40();
    CreateAndInitResource_41();
    CreateAndInitResource_42();
    CreateAndInitResource_43();
    CreateAndInitResource_44();
    CreateAndInitResource_45();
    CreateAndInitResource_46();
    CreateAndInitResource_47();
    CreateAndInitResource_48();
    CreateAndInitResource_49();
    CreateAndInitResource_50();
    CreateAndInitResource_51();
    CreateAndInitResource_52();
    CreateAndInitResource_53();
    CreateAndInitResource_54();
    CreateAndInitResource_55();
    CreateAndInitResource_56();
    CreateAndInitResource_57();
    CreateAndInitResource_58();
    CreateAndInitResource_59();
    CreateAndInitResource_60();
    CreateAndInitResource_66();
    CreateAndInitResource_67();
    CreateAndInitResource_68();
    CreateAndInitResource_69();
    CreateAndInitResource_82();
    {
        HANDLE handle{};
        ThrowIfFailed(g_device->CreateSharedHandle(GetResource(10).Get(), nullptr, GENERIC_ALL, nullptr, &handle));
        ComPtr<ID3D12Resource> resource;
        ThrowIfFailed(g_device->OpenSharedHandle(handle, IID_PPV_ARGS(&resource)));
        g_resources[9] = resource;
    }

    // ApiObjectId     = 75
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1 };
        ThrowIfFailed(g_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&g_descriptorHeap_75)));
    }
    CreateDescriptors_75_0();

    // ApiObjectId     = 76
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1024, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1 };
        ThrowIfFailed(g_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&g_descriptorHeap_76)));
    }
    CreateDescriptors_76_0();

    g_device->SetName(LR"(DirectX 12 Device)");

    GetObject(1)->SetName(LR"(GraphicsCommandQueue)");

    GetObject(3)->SetName(LR"(Internal DXGI Fence)");

    GetObject(5)->SetName(LR"(Internal DXGI Fence)");

    GetObject(6)->SetName(LR"(Internal DXGI CommandQueue)");

    GetObject(8)->SetName(LR"(Internal DXGI CommandAllocator)");

    GetObject(9)->SetName(LR"(DXGI cross-adapter flip proxy (dGPU opened))");

    GetObject(11)->SetName(LR"(Backbuffer_1)");

    GetObject(12)->SetName(LR"(CopyCommandQueue)");

    GetObject(14)->SetName(LR"(Copy Command List - 1_Allocator)");

    GetObject(16)->SetName(LR"(UploadMeshletVertex_Buffer)");

    GetObject(19)->SetName(LR"(UploadMeshlet_Buffer)");

    GetObject(20)->SetName(LR"(UploadInstanceSubmesh_Buffer)");

    GetObject(22)->SetName(LR"(UploadMeshletTriangle_Buffer)");

    GetObject(23)->SetName(LR"(UploadMeshFilter_Buffer)");

    GetObject(25)->SetName(LR"(Frame Constant Buffer)");

    GetObject(26)->SetName(LR"(Upload MeshData Buffer)");

    GetObject(27)->SetName(LR"(Upload Index Buffer)");

    GetObject(28)->SetName(LR"(Upload Submesh Buffer)");

    GetObject(29)->SetName(LR"(Upload Vertex Buffer)");

    GetObject(36)->SetName(LR"(Meshlet_Buffer)");

    GetObject(37)->SetName(LR"(InstanceSubmesh_Buffer)");

    GetObject(38)->SetName(LR"(MeshFilter_Buffer)");

    GetObject(40)->SetName(LR"(MeshletTriangle_Buffer)");

    GetObject(41)->SetName(LR"(MeshletVertex_Buffer)");

    GetObject(44)->SetName(LR"(ActiveInstanceSubmeshPrefixCountBuffer)");

    GetObject(45)->SetName(LR"(SubmeshDrawCandidateBuffer)");

    GetObject(46)->SetName(LR"(SubmeshCullExecuteArgBuffer)");

    GetObject(47)->SetName(LR"(ActiveInstanceSubmeshCount)");

    GetObject(48)->SetName(LR"(SubmeshDrawCandidateCounter)");

    GetObject(49)->SetName(LR"(Submesh_Buffer)");

    GetObject(50)->SetName(LR"(Index_Buffer)");

    GetObject(51)->SetName(LR"(MeshData_Buffer)");

    GetObject(52)->SetName(LR"(Vertex_Buffer)");

    GetObject(55)->SetName(LR"(ActiveMeshFilterBuffer)");

    GetObject(56)->SetName(LR"(ActiveMeshFilterCount)");

    GetObject(64)->SetName(LR"(Graphics Command List - 2_Allocator)");

    g_descriptorHeap_75->SetName(LR"(Descriptor Heap - CBV_SRV_UAV)");

    g_descriptorHeap_76->SetName(LR"(Descriptor Heap - Sampler)");

    GetObject(77)->SetName(LR"(DispatchCompute_CommandSignature)");

    GetObject(78)->SetName(LR"(ComputeCommandQueue)");

    GetObject(80)->SetName(LR"(Compute Command List - 2_Allocator)");

    GetObject(82)->SetName(LR"(Backbuffer_2)");

    CreateIndirectArgumentBuffer_46_1();
}
