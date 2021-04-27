#include "HLExporterTest.h"

void HLExporterTest_Texture::Save(ZResource* res, std::string outPath)
{
	ZTexture* tex = (ZTexture*)res;

	auto data = tex->GetRawData();
}