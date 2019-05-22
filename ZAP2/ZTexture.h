#pragma once

#include "ZResource.h"
#include "tinyxml2.h"

#include <vector>

enum TextureType
{
	RGBA32bpp,
	RGBA16bpp,
	Palette4bpp,
	Palette8bpp,
	Grayscale4bpp,
	Grayscale8bpp,
	GrayscaleAlpha4bpp,
	GrayscaleAlpha8bpp,
	GrayscaleAlpha16bpp
};

class ZTexture : public ZResource
{
protected:
	TextureType type;
	int width, height;
	uint8_t* bmpRgb;
	uint8_t* bmpAlpha;

	void ParseXML(tinyxml2::XMLElement* reader);
	void FixRawData();
	void PrepareBitmap();
	void PrepareBitmapRGBA16();
	void PrepareBitmapRGBA32();
	void PrepareBitmapGrayscale8();
	void PrepareBitmapGrayscaleAlpha8();
	void PrepareBitmapGrayscale4();
	void PrepareBitmapGrayscaleAlpha4();
	void PrepareBitmapGrayscaleAlpha16();
	void PrepareBitmapPalette4();
	void PrepareBitmapPalette8();
	void PrepareRawData(std::string inFolder);
	void PrepareRawDataRGBA32(std::string inFolder);
	void PrepareRawDataRGBA16(std::string inFolder);
	void PrepareRawDataGrayscale8(std::string inFolder);
	void PrepareRawDataGrayscaleAlpha8(std::string inFolder);
	void PrepareRawDataGrayscale4(std::string inFolder);
	void PrepareRawDataGrayscaleAlpha4(std::string inFolder);
	void PrepareRawDataGrayscaleAlpha16(std::string inFolder);
	void PrepareRawDataPalette4(std::string inFolder);
	void PrepareRawDataPalette8(std::string inFolder);
	float GetPixelMultiplyer();

public:
	ZTexture(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex);
	ZTexture(tinyxml2::XMLElement* reader, std::string inFolder);
	ZTexture(TextureType nType, std::vector<uint8_t> rawData, std::string nName, int nWidth, int nHeight);

	std::vector<uint8_t> GetRawData();
	int GetRawDataSize();
	void Save(std::string outFolder);
};