#include "TextureExporter.h"

void ExporterExample_Texture::Save(ZResource* res, std::filesystem::path outPath, BinaryWriter* writer)
{
	ZTexture* tex = (ZTexture*)res;

	auto data = tex->GetRawData();

	for (uint8_t b : data)
		writer->Write(b);
}