#pragma once
#include "../cesar_core/cesar_core.h"

namespace cesar
{
	using TextureHandle = Uint32;

	static constexpr TextureHandle INVALID_TEXTURE_HANDLE       = TextureHandle(0);
	static constexpr TextureHandle WHITE_TEXTURE_HANDLE         = TextureHandle(1);
	static constexpr TextureHandle BLACK_TEXTURE_HANDLE         = TextureHandle(2);
	static constexpr TextureHandle DEFAULT_NORMAL_TEXURE_HANDLE = TextureHandle(4);

	class TextureManager
	{
	public:
		TextureManager();
		~TextureManager();

		TextureHandle LoadImageTexture(Image)

	private:
		
	};

}