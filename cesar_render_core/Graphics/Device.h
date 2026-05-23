// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../../cesar/cesar_core/cesar_core.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#define FRAMES_IN_FLIGHT 3

namespace cesar
{
	enum ShaderModel
	{
		SM_6_5 = D3D_SHADER_MODEL_6_5,
		SM_6_6 = D3D_SHADER_MODEL_6_6,
		SM_6_7 = D3D_SHADER_MODEL_6_7,
		SM_6_8 = D3D_SHADER_MODEL_6_8,

		Unknown
	};

	class Device;
	struct GPUDeviceCapabilities
	{
		ShaderModel GetDeviceShaderModel()const { return shader_model;        }
		Bool SupportEnhanceBarriers()const      { return enhanced_barriers;   }
		Bool SupportRayTracing()const           { return raytracing_support;  }
		Bool SupportMeshShaders()const          { return mesh_shader_support; }
		Bool SupportWorkGraphs()const           { return work_graph_support;  }
		Bool EnhancedBarrierSupport()const      { return enhanced_barriers;   }
	protected:
		friend class Device;
		ShaderModel shader_model;
		Bool        raytracing_support = true;
		Bool        mesh_shader_support = true;
		Bool        work_graph_support = true;
		Bool        enhanced_barriers = true;

		Uint8 min_wave = 0;
		Uint8 max_wave = 0;
	};

	class Device
	{
	public:
		Device();
		~Device();
	
		const GPUDeviceCapabilities& GetGPUDeviceCapabilities()const { return capabilities; }

		ID3D12Device6* GetDevice();
		IDXGIAdapter* GetAdapter();
		IDXGIFactory4* GetFactory();

	private:
		Bool CheckDeviceCapabilites();
		void ChooseAdapter();
	private:
		ComPtr<ID3D12Device6> device;

		ComPtr<IDXGIFactory6> factory;
		ComPtr<IDXGIAdapter> adapter;
	
		ComPtr<ID3D12Debug> debug;

		GPUDeviceCapabilities capabilities;
	};

}
