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
	TextureType type;
	uint32_t width, height;

	std::vector<std::vector<RGBAPixel>> textureData; // Reading from object to PNG.
	std::vector<uint8_t> textureDataRaw; // When reading from a PNG file.

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

	bool isPalette;

	void ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
	                    const uint32_t nRawDataIndex,
	                    const std::string& nRelPath) override;  // Extract Mode
	static ZTexture* FromBinary(TextureType nType, std::vector<uint8_t> nRawData,
	                            uint32_t rawDataIndex, std::string nName, int32_t nWidth,
	                            int32_t nHeight, ZFile* nParent);
	static ZTexture* FromPNG(std::string pngFilePath, TextureType texType);
	static ZTexture* FromHLTexture(HLTexture* hlTex);
	static TextureType GetTextureTypeFromString(std::string str);

	void ParseXML(tinyxml2::XMLElement* reader) override;

	std::string GetSourceOutputCode(const std::string& prefix) override;
	std::string GetSourceOutputHeader(const std::string& prefix) override;

	size_t GetRawDataSize() override;
	std::string GetIMFmtFromType();
	std::string GetIMSizFromType();
	uint32_t GetWidth();
	uint32_t GetHeight();
	void SetDimensions(uint32_t nWidth, uint32_t nHeight);
	TextureType GetTextureType();
	void Save(const std::string& outFolder) override;
	std::string GetExternalExtension() override;
	std::string GetPoolOutPath(std::string defaultValue);
	void CalcHash() override;

	bool IsExternalResource() override;
	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;
};
