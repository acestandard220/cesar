// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "Device.h"

namespace cesar
{
	using namespace cesar;
	
	extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_SDK_VERSION; }
	extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; }

	Device::Device()
	{
		UINT dxgi_flags = 0;
#ifdef _DEBUG
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
		{
			debug->EnableDebugLayer();
			dxgi_flags |= DXGI_CREATE_FACTORY_DEBUG;
			LOG_INFO("D3D12 Debug Layer Enabled!");
		}
		else {
			LOG_ERROR("Failed to enable DirectX12 Debug Layer.");
		}

		ComPtr<ID3D12Debug1> debug_controller = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debug_controller.GetAddressOf()))))
		{
			debug_controller->SetEnableGPUBasedValidation(true);
			LOG_INFO("D3D12 GPU Based Validation Enabled!");
		}
#endif

		if (FAILED(CreateDXGIFactory2(dxgi_flags, IID_PPV_ARGS(&factory))))
		{
			LOG_FATAL("Failed to create DXGI Factory.");
			return;
		}

		ChooseAdapter();

		if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device))))
		{
			LOG_FATAL("Failed to create device.");
			return;
		}

		CheckDeviceCapabilites();

		device->SetName(L"DirectX 12 Device");
	}

	Device::~Device()
	{
		device.Reset();
	}

	void Device::ChooseAdapter()
	{

		UINT adapter_count = 0;
		IDXGIAdapter* current_adapter = nullptr;
		std::vector<IDXGIAdapter*> adapters;

		DXGI_GPU_PREFERENCE gpu_preference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
		while (factory->EnumAdapterByGpuPreference(adapter_count, gpu_preference, IID_PPV_ARGS(&current_adapter)) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC adapter_desc{};
			current_adapter->GetDesc(&adapter_desc);

			std::wstring text = L"***Adapter: ";
			text += adapter_desc.Description;
			LOG_INFO("{}", WideToUtf8(text.c_str()));

			adapters.push_back({ current_adapter });
			adapter_count++;
		}

		//Choosing the primary adapter
		factory->EnumAdapterByGpuPreference(0, gpu_preference, IID_PPV_ARGS(adapter.GetAddressOf()));

	}

	Bool Device::CheckDeviceCapabilites()
	{
		D3D12_FEATURE_DATA_ARCHITECTURE arch = {};
		device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &arch, sizeof(arch));
		

		// Max Feature Level 
		constexpr D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_12_2,
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};

		D3D12_FEATURE_DATA_FEATURE_LEVELS flData = {};
		flData.NumFeatureLevels = _countof(featureLevels);
		flData.pFeatureLevelsRequested = featureLevels;

		device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &flData, sizeof(flData));
		CESAR_ASSERT(flData.MaxSupportedFeatureLevel >= D3D_FEATURE_LEVEL_12_2, "Device does not meet minimum feature level requirements.");

		// Max Shader Model 
		constexpr D3D_SHADER_MODEL shaderModels[] = {
			D3D_SHADER_MODEL_6_9,
			D3D_SHADER_MODEL_6_8,
			D3D_SHADER_MODEL_6_7,
			D3D_SHADER_MODEL_6_6,
			D3D_SHADER_MODEL_6_5,
			D3D_SHADER_MODEL_6_4,
			D3D_SHADER_MODEL_6_0,
			D3D_SHADER_MODEL_5_1,
		};

		for (auto sm : shaderModels)
		{
			D3D12_FEATURE_DATA_SHADER_MODEL smData = { sm };
			if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &smData, sizeof(smData))))
			{
				capabilities.shader_model = (ShaderModel)smData.HighestShaderModel;
				SOL_ASSERT_MSG((smData.HighestShaderModel >= D3D_SHADER_MODEL_6_5), "Device does not support Shader Model 6_5 or higher.");
				break;
			}
		}

		//Enhanced Barriers
		D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12 = {};
		if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, sizeof(options12))))
			SOL_ASSERT_MSG(options12.EnhancedBarriersSupported, "Device does not support enhanced barriers.");

		D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
		if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7))))
			SOL_ASSERT_MSG((options7.MeshShaderTier != D3D12_MESH_SHADER_TIER_NOT_SUPPORTED), "Device does not support mesh shaders");

		//Raytracing
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
		if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5))))
			SOL_ASSERT_MSG((options5.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED), "Device does not support raytracing.");

		//Workgraphs
		D3D12_FEATURE_DATA_D3D12_OPTIONS21 options21 = {};
		if (SUCCEEDED(device->CheckFeatureSupport(
			D3D12_FEATURE_D3D12_OPTIONS21,
			&options21,
			sizeof(options21))))
		{
			if (options21.WorkGraphsTier != D3D12_WORK_GRAPHS_TIER_NOT_SUPPORTED)
				capabilities.work_graph_support = true;
			else
				capabilities.work_graph_support = false;
		}
		
		D3D12_FEATURE_DATA_D3D12_OPTIONS1 opts1{};
		device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &opts1, sizeof(opts1));
		capabilities.min_wave = opts1.WaveLaneCountMin;
		capabilities.max_wave = opts1.WaveLaneCountMax;

		return true;
	}

	void ValidateDeviceCapabilities(const GPUDeviceCapabilities& caps)
	{
	}

	ID3D12Device6* Device::GetDevice()
	{
		return device.Get();
	}
	IDXGIAdapter* Device::GetAdapter()
	{
		return adapter.Get();
	}
	IDXGIFactory4* Device::GetFactory()
	{
		return factory.Get();
	}
}