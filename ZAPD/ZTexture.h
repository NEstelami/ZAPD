#pragma once

#include <vector>

#include "HighLevel/HLTexture.h"
#include "ImageBackend.h"
#include "ZResource.h"
#include "tinyxml2.h"

enum class TextureType
{
	RGBA32bpp,
	RGBA16bpp,
	Palette4bpp,
	Palette8bpp,
	Grayscale4bpp,
	Grayscale8bpp,
	GrayscaleAlpha4bpp,
	GrayscaleAlpha8bpp,
	GrayscaleAlpha16bpp,
	Error
};

class ZTexture : public ZResource
{
protected:
	TextureType type = TextureType::Error;
	uint32_t width, height;

	ImageBackend textureData;
	std::vector<uint8_t> textureDataRaw;  // When reading from a PNG file.

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
	void PrepareRawDataRGBA16(std::string rgbaPath);
	void PrepareRawDataRGBA32(std::string rgbaPath);
	void PrepareRawDataGrayscale4(std::string grayPath);
	void PrepareRawDataGrayscale8(std::string grayPath);
	void PrepareRawDataGrayscaleAlpha4(std::string grayAlphaPath);
	void PrepareRawDataGrayscaleAlpha8(std::string grayAlphaPath);
	void PrepareRawDataGrayscaleAlpha16(std::string grayAlphaPath);
	void PrepareRawDataPalette4(std::string palPath);
	void PrepareRawDataPalette8(std::string palPath);
	float GetPixelMultiplyer();

public:
	ZTexture(ZFile* nParent);

	bool isPalette = false;

	void FromBinary(TextureType nType, std::vector<uint8_t> nRawData, uint32_t rawDataIndex,
	                std::string nName, int32_t nWidth, int32_t nHeight);
	void FromPNG(std::string pngFilePath, TextureType texType);
	void FromHLTexture(HLTexture* hlTex);
	static TextureType GetTextureTypeFromString(std::string str);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	std::string GetBodySourceCode() const;
	void CalcHash() override;
	void Save(const std::string& outFolder) override;

	bool IsExternalResource() override;
	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;
	std::string GetExternalExtension() override;

	size_t GetRawDataSize() override;
	std::string GetIMFmtFromType();
	std::string GetIMSizFromType();
	uint32_t GetWidth();
	uint32_t GetHeight();
	void SetDimensions(uint32_t nWidth, uint32_t nHeight);
	TextureType GetTextureType();
	std::string GetPoolOutPath(std::string defaultValue);
};
