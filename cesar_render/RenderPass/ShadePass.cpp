#include "ShadePass.h"

namespace cesar
{
	using namespace render_graph;

	ShadePass::ShadePass(RenderContext* render_context, Uint32 width, Uint32 height)
		:render_context(render_context), width(width), height(height)
	{
		CreatePSO();
	}

	ShadePass::~ShadePass()
	{}

	void ShadePass::AddSolidShadePass(render_graph::RenderGraph & render_graph)
	{
	
		struct PassData {
			TextureReadOnly position_map;
			TextureReadOnly normal_map;

			BufferID frame_constants;
		};
		render_graph.AddPass<PassData>("Solid Shade Pass", RGPassType::Graphics, RGPassFlags::None,
			[&](PassData& data, RGBuilder& builder)
			{
				builder.DeclareTexture(RG_NAME(SolidShadeMap), RenderTargetDesc(width, height));
				builder.WriteRenderTarget(RG_NAME(SolidShadeMap), ResourceLoadStoreFlags::ClearPreserve, CESAR_DEFAULT_TEXTURE_VIEW_DESC);
				data.position_map = builder.ReadTexture(RG_NAME(PositionMap), ReadAccessType::PixelShader, CESAR_DEFAULT_TEXTURE_VIEW_DESC);
				data.normal_map = builder.ReadTexture(RG_NAME(NormalMap), ReadAccessType::PixelShader, CESAR_DEFAULT_TEXTURE_VIEW_DESC);

				data.frame_constants = render_graph.GetBufferID(RG_NAME(FrameConstants));

				builder.SetViewport(width, height);
			},
			[&](PassData& data, RGContext& context)
			{
				struct Constants
				{
					Uint32 position_map_idx;
					Uint32 normal_map_idx;
				}constants = {
					.position_map_idx = context.GetTextureReadOnlyIndex(data.position_map),
					.normal_map_idx = context.GetTextureReadOnlyIndex(data.normal_map)
				};

				CommandList& cmd_list = context.GetCommandList();
				cmd_list.SetPipelineState(solid_shade_pso.get());

				RGBuffer* cbv = render_graph.GetBufferResource(data.frame_constants);
				cmd_list.SetGraphicsCBV(0, cbv->resource);
				cmd_list.SetGraphicsConstants(std::span<Constants>(&constants, 1));
				cmd_list.DispatchMesh(1, 1, 1);
			}
		);

	}

	void ShadePass::OnResize(Uint32 w, Uint32 h)
	{
		width = w;
		height = h;
	}

	void ShadePass::CreatePSO()
	{
		GPUContext* gpu_context = render_context->GetGPUContext();

		MeshPipelineStateDesc desc{};
		desc.primitive_topology_type = PrimitiveTypologyType::Triangle;
		desc.raster_state.cull_mode = CullMode::None;
		desc.raster_state.fill_mode = FillMode::Solid;
		desc.render_target_count = 1;
		desc.reformat[0] = ResourceFormat::RGBA32_FLOAT;

		desc.as = ShaderID::NoShader;
		desc.ms = ShaderID::DrawScreenTriangle_MS;
		desc.ps = ShaderID::SolidShade_PS;

		solid_shade_pso = gpu_context->CreateMeshShaderPipelineState(desc, "Solid Shade PSO");
	}
}
