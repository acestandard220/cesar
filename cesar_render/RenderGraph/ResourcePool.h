// Portions of this file are derived from Adria Renderer (MIT License)
// Copyright (c) <https://github.com/mateeeeeee>
// See ext/Adria/LICENSE for details.

#pragma once
#include "../../cesar_core/cesar_core.h"
#include "../../cesar_render_core/RenderContext.h"

namespace cesar {

	class ResourcePool {

		struct PooledTexture {
			std::unique_ptr<Texture> texture = nullptr;
			Uint64 last_frame_used = 0;
		};

		struct PooledBuffer {
			std::unique_ptr<Buffer> buffer = nullptr;
			Uint64 last_frame_used = 0;
		};

	public:
		ResourcePool(RenderContext* render_context) :render_context(render_context) {}
		~ResourcePool() {};


		void Update() {
			for (Uint32 i = 0; i < textures.size();) {
				PooledTexture& pooled_texture = textures[i].first;
				Bool& active = textures[i].second;

				if (!active && pooled_texture.last_frame_used + 4 < frame_index) {
					std::swap(textures[i], textures.back());
					textures.pop_back();
				} else{
					i++;
				}

			}
			frame_index++;
		}

		Texture* AllocateTexture(const TextureDesc& texture_desc, const Char* name) {

			for (auto& [pooled_texture, active] : textures) {
				auto& texture = pooled_texture.texture;
				auto& last_frame_used = pooled_texture.last_frame_used;

				if (!active && texture->GetDesc().IsCompatible(texture_desc)) {
					active = true;
					last_frame_used = frame_index;
					return texture.get();
				}
			}
			GPUContext* gpu_context = render_context->GetGPUContext();
			textures.emplace_back(std::pair{ PooledTexture{ gpu_context->CreateTexture(texture_desc, "Unnamed Resource - RG"),frame_index} ,true});
			return textures.back().first.texture.get();
		}

		void ReleaseTexture(Texture* texture) {

			for (auto& [pooled_texture, active] : textures) {
				if (active && pooled_texture.texture.get() == texture) {
					active = false;
					return;
				}
			}
		}

		Buffer* AllocateBuffer(const BufferDesc& buffer_desc, const Char* name) {
			for (auto& [pooled_buffer, active] : buffers) {
				auto& buffer = pooled_buffer.buffer;
				auto& last_frame_used = pooled_buffer.last_frame_used;

				if (!active && buffer->GetDesc() == buffer_desc) {
					active = true;
					last_frame_used = frame_index;
					return buffer.get();
				}
			}
			GPUContext* gpu_context = render_context->GetGPUContext();
			buffers.emplace_back(std::pair{ PooledBuffer{ gpu_context->CreateBuffer(buffer_desc, name),frame_index },true });
			return buffers.back().first.buffer.get();
		}

		void ReleaseBuffer(Buffer* buffer) {
			for (auto& [pooled_buffer, active] : buffers) {
				if (active && pooled_buffer.buffer.get() == buffer) {
					active = false;
					return;
				}
			}
		}

	private:
		RenderContext* render_context = nullptr;

		Uint64 frame_index = 0;
		std::vector<std::pair<PooledTexture, Bool>> textures;
		std::vector<std::pair<PooledBuffer, Bool>>buffers;

		const Uint32 frame_limit = 4;
	};

}