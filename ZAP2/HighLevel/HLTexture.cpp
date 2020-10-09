#include "HLTexture.h"
#include "../StringHelper.h"
#include "../stb_image.h"

using namespace std;

HLTexture* HLTexture::FromPNG(std::string pngFilePath, HLTextureType texType)
{
	int comp;
	HLTexture* tex = new HLTexture();
	tex->type = texType;
	//tex->name = StringHelper::Split(Path::GetFileNameWithoutExtension(pngFilePath), ".")[0];

	tex->bmpRgba = (uint8_t*)stbi_load((pngFilePath).c_str(), (int*)&tex->width, (int*)&tex->height, &comp, STBI_rgb_alpha);
	
	return tex;
}
