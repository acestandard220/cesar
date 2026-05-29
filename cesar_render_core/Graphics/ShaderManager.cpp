// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.
#include "ShaderManager.h"
#include "../../cesar_core/Core/FileWatcher.h"

#include <d3d12.h>

using namespace cesar;

namespace cesar{

	ShaderManager shader_manager;
	std::unordered_map<ShaderID, Shader> shader_blob;
	std::unordered_map<std::string, ShaderID> shader_id_map;
	
	FileWatcher* file_watcher = nullptr;

	constexpr const Wchar* GetShaderSource(ShaderID shader_id)
	{
		switch (shader_id)
		{
		case ShaderID::GBufferPixel:   return L"gbuffer_pixel.hlsl";
		case ShaderID::GBufferVertex:  return L"gbuffer_vertex.hlsl";
		case ShaderID::Mesh:        return L"mesh.hlsl";
		case ShaderID::Compute:     return L"compute.hlsl";

		case ShaderID::DepthPrePass_MS: return L"DepthPrePass.hlsl";
		case ShaderID::GenerateClusters: return L"Light/GenerateClusters.hlsl";
		case ShaderID::CullMeshlets:   return L"Meshlet/CullMeshlets.hlsl";
		case ShaderID::BuildCullMeshletArgs: return L"Meshlet/CullMeshlets.hlsl";
		case ShaderID::CullInstances:  return L"Meshlet/CullInstances.hlsl";
		case ShaderID::DrawMeshlet:    return L"Meshlet/DrawMeshlets.hlsl";
		case ShaderID::GenerateMipMap: return L"Other/generate_mipmap.hlsl";
		case ShaderID::VizSubMeshBounds_MS: return L"Meshlet/VizBounds.hlsl";
		case ShaderID::VizSubMeshBounds_PS: return L"Meshlet/VizBounds.hlsl";
		}
	}

	constexpr const Wchar* GetEntryPoint(ShaderID shader_id)
	{
		switch (shader_id)
		{
		case ShaderID::GBufferVertex:
			return L"VertexMain";
		case ShaderID::GBufferPixel:
			return L"PixelMain";
		case ShaderID::Compute:
			return L"ComputeMain";
		case ShaderID::Mesh:
			return L"MeshMain";
		case ShaderID::GenerateMipMap:
			return L"GenerateMipMap";
		case ShaderID::DrawMeshlet:
			return L"DrawMeshlet";
		case ShaderID::CullInstances:
			return L"CullInstances";
		case ShaderID::BuildCullMeshletArgs:
			return L"BuildCullMeshletArgs";
		case ShaderID::CullMeshlets:
			return L"CullMeshlets";
		case ShaderID::VizSubMeshBounds_MS:
			return L"VizSubMeshBounds_MS";
		case ShaderID::VizSubMeshBounds_PS:
			return L"VizSubMeshBounds_PS";
		case ShaderID::GenerateClusters:
			return L"GenerateClusters";
		case ShaderID::DepthPrePass_MS:
			return L"DepthPrePass_MS";
		default:
			return L"InvalidMain";
		}
	}

	constexpr const Wchar* GetShaderModelTargetProfileSuffix(ShaderModel shader_model)
	{
		switch (shader_model)
		{
		    case ShaderModel::SM_6_5: return L"_6_5";
		    case ShaderModel::SM_6_6: return L"_6_6";
		    case ShaderModel::SM_6_7: return L"_6_7";
		    case ShaderModel::SM_6_8: return L"_6_8";
		    default: CESAR_DEBUGBREAK();
		}
	}
	constexpr const Wchar* GetShaderTypeTargetProfilePrefix(ShaderType type)
	{
		switch (type)
		{
		    case ShaderType::Vertex:  return L"vs";
		    case ShaderType::Pixel:   return L"ps";
		    case ShaderType::Compute: return L"cs";
		    case ShaderType::Mesh:    return L"ms";
			case ShaderType::Amplification: return L"as";
		    default:CESAR_DEBUGBREAK();
		}
	}
	constexpr const std::wstring GetTargetProfile(ShaderID shader_id, ShaderModel shader_model)
	{
		std::wstring target_profile;
		switch (shader_id)
		{
		    case ShaderID::GBufferVertex:
			{
				target_profile = std::format(L"{}{}", GetShaderTypeTargetProfilePrefix(ShaderType::Vertex), GetShaderModelTargetProfileSuffix(shader_model));
				break;
			}
			case ShaderID::VizSubMeshBounds_PS:
		    case ShaderID::GBufferPixel:
			{
				target_profile = std::format(L"{}{}", GetShaderTypeTargetProfilePrefix(ShaderType::Pixel), GetShaderModelTargetProfileSuffix(shader_model));
				break;
			}

			case ShaderID::GenerateClusters:
			case ShaderID::CullMeshlets:
			case ShaderID::BuildCullMeshletArgs:
			case ShaderID::CullInstances:
			case ShaderID::GenerateMipMap:
		    case ShaderID::Compute:
			{
				target_profile = std::format(L"{}{}", GetShaderTypeTargetProfilePrefix(ShaderType::Compute), GetShaderModelTargetProfileSuffix(shader_model));
				break;
			}

			case ShaderID::DepthPrePass_MS:
			case ShaderID::VizSubMeshBounds_MS:
			case ShaderID::DrawMeshlet:
		    case ShaderID::Mesh:
			{
				target_profile = std::format(L"{}{}", GetShaderTypeTargetProfilePrefix(ShaderType::Mesh), GetShaderModelTargetProfileSuffix(shader_model));
				break;
			}
			case ShaderID::Amplification:
			{
				target_profile = std::format(L"{}{}", GetShaderTypeTargetProfilePrefix(ShaderType::Amplification), GetShaderModelTargetProfileSuffix(shader_model));
				break;
			}
		    default:CESAR_DEBUGBREAK();
		}
		std::wstring l_target_profile(target_profile.begin(), target_profile.end());
		return target_profile;
	}

	void OnShaderChanged(const std::string& path)
	{
		shader_manager.CompileShader(shader_id_map[path]);
	}

	ShaderManager::ShaderManager()
	{
		root_shader_directory = std::filesystem::path(__FILE__).parent_path().parent_path();
		root_shader_directory /= "Shader";

		file_watcher = new FileWatcher;
		file_watcher->Watch(root_shader_directory.string(), true);
		file_watcher->GetFileChangedEvent().AddStaticFunction(OnShaderChanged);
	}

	void ShaderManager::CompileShader(ShaderID shader_id)
	{
		ComPtr<IDxcUtils> pUtils;
		ComPtr<IDxcCompiler3> pCompiler;
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));

		ComPtr<IDxcBlobEncoding> pSource;
		std::filesystem::path shader_path = root_shader_directory/GetShaderSource(shader_id);
		const std::filesystem::path shader_directory = shader_path.parent_path();
		pUtils->LoadFile(shader_path.c_str(), nullptr, &pSource);

		DxcBuffer sourceBuffer = {
			pSource->GetBufferPointer(),
			pSource->GetBufferSize(),
			CP_UTF8
		};

		const auto entry_point = GetEntryPoint(shader_id);
		const auto target_profile = GetTargetProfile(shader_id, config.shader_model);
#ifdef _DEBUG
		LPCWSTR args[] = {
			L"-E", entry_point,
			L"-T", target_profile.c_str(),
			L"-HV", L"2021",
			L"-Zi",
			L"-Qembed_debug",
			L"-Od",
			L"-I", shader_directory.c_str()
		};
#elifdef PROFILE
		LPCWSTR args[] = {
			L"-E", entry_point,
			L"-T", target_profile.c_str(),
			L"-HV", L"2021",
			L"-Zi",           
			L"-O3",           
			L"-Fd", L"shaders/pdbs/", 
			L"-I", shader_directory.c_str()
		};
#else
		LPCWSTR args[] = {
			L"-E", entry_point,
			L"-T", target_profile.c_str(),
			L"-HV", L"2021",
			L"-O3",
			L"-I", shader_directory.c_str()
		};
#endif

		ComPtr<IDxcIncludeHandler> pIncludeHandler;
		pUtils->CreateDefaultIncludeHandler(&pIncludeHandler);

		ComPtr<IDxcResult> pResult;
		pCompiler->Compile(&sourceBuffer, args, _countof(args),
			pIncludeHandler.Get(),
			IID_PPV_ARGS(&pResult));

		ComPtr<IDxcBlob> pShader;
		pResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), nullptr);
		shader_blob[shader_id].blob = pShader;

		HRESULT hrStatus;
		pResult->GetStatus(&hrStatus);

		if (FAILED(hrStatus))
		{
			ComPtr<IDxcBlobUtf8> pErrors;
			pResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);

			if (pErrors && pErrors->GetStringLength() > 0)
			{
				 LOG_ERROR("Shader compilation failed:\n{}", pErrors->GetStringPointer());
			}

			shader_blob[shader_id].blob = nullptr;
			return;
		}
	}

	ID3D12ShaderReflection* ShaderManager::GetShaderReflectionImpl(const Shader& shader, D3D12_SHADER_DESC& desc)
	{
		IDxcUtils* utils;
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));

		DxcBuffer shaderBuffer{};
		shaderBuffer.Ptr = shader.blob->GetBufferPointer();
		shaderBuffer.Size = shader.blob->GetBufferSize();
		shaderBuffer.Encoding = 0;
		ID3D12ShaderReflection* shaderReflection;
		HRESULT result = utils->CreateReflection(&shaderBuffer, IID_PPV_ARGS(&shaderReflection));
		if (FAILED(result))
		{
			LOG_FATAL("Failed to get shader reflection.");
			utils->Release();
			CESAR_DEBUGBREAK();
			return nullptr;
		}

		shaderReflection->GetDesc(&desc);
		utils->Release();
		return shaderReflection;
	}

	const Shader& ShaderManager::GetShaderImpl(ShaderID shader_id)
	{
		if (shader_id == ShaderID::NoShader) {
			return Shader{};
		}

		CompileShader(shader_id);
		return shader_blob[shader_id];
	}

	D3D12_SHADER_BYTECODE ShaderManager::GetShaderByteCodeImpl(Shader shader) const
	{
		if (shader.blob == nullptr) {
			return {};
		}

		return D3D12_SHADER_BYTECODE(shader.blob->GetBufferPointer(), shader.blob->GetBufferSize());
	}

	ShaderManager& ShaderManager::GetShaderManager()
	{
		return shader_manager;
	}

}