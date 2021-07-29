#include "HLTexture.h"
#include "../StringHelper.h"

HLTexture* HLTexture::FromPNG(const std::string& pngFilePath, HLTextureType texType)
{
	// int32_t comp;
	HLTexture* tex = new HLTexture();
	tex->type = texType;
	// tex->bmpRgba = (uint8_t*)stbi_load((pngFilePath).c_str(), (int32_t*)&tex->width,
	//                                   (int32_t*)&tex->height, &comp, STBI_rgb_alpha);

	return tex;
}
