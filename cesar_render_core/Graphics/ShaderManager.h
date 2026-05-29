// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#pragma once
#include "../cesar_core/cesar_core.h"
#include "Device.h"

#include <dxcapi.h>
#include <d3d12shader.h>

#include <filesystem>
#pragma comment(lib, "dxcompiler.lib")

namespace cesar
{
	enum class ShaderID
	{
		GBufferVertex, GBufferPixel, 
		Compute, Mesh, Amplification,

		CullInstances, CullMeshlets,
		BuildCullMeshletArgs,

		DepthPrePass_MS,

		DrawMeshlet,
		GenerateMipMap,

		GenerateClusters,

		VizSubMeshBounds_MS, VizSubMeshBounds_PS, 

		NoShader
	};

	enum class ShaderType
	{
		Vertex, Pixel, Compute, Amplification, Mesh
	};

	struct Shader
	{
		ComPtr<IDxcBlob> blob;
	};

	struct ShaderCompilerConfig
	{
		ShaderModel shader_model = ShaderModel::SM_6_8;
	};

	class ShaderManager
	{
	public:
		ShaderManager();
		~ShaderManager() = default;
		
		void CompileShader(ShaderID shader_id);

		ID3D12ShaderReflection* GetShaderReflectionImpl(const Shader& shader, D3D12_SHADER_DESC& desc);

		const Shader& GetShaderImpl(ShaderID shader_id);

		D3D12_SHADER_BYTECODE GetShaderByteCodeImpl(Shader shader)const;

		static ShaderManager& GetShaderManager();

	private:
		ShaderCompilerConfig config;
		std::filesystem::path root_shader_directory;
	};

#define GetShader(shader_id) ShaderManager::GetShaderManager().GetShaderImpl(shader_id)
#define GetShaderReflection(shader,desc) ShaderManager::GetShaderManager().GetShaderReflectionImpl(shader,desc)
#define GetShaderByteCode(shader) ShaderManager::GetShaderManager().GetShaderByteCodeImpl(shader)
}