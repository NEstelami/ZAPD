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

	void PrepareRawData(const fs::path& inFolder);
	void PrepareRawDataRGBA16(const fs::path& rgbaPath);
	void PrepareRawDataRGBA32(const fs::path& rgbaPath);
	void PrepareRawDataGrayscale4(const fs::path& grayPath);
	void PrepareRawDataGrayscale8(const fs::path& grayPath);
	void PrepareRawDataGrayscaleAlpha4(const fs::path& grayAlphaPath);
	void PrepareRawDataGrayscaleAlpha8(const fs::path& grayAlphaPath);
	void PrepareRawDataGrayscaleAlpha16(const fs::path& grayAlphaPath);
	void PrepareRawDataPalette4(const fs::path& palPath);
	void PrepareRawDataPalette8(const fs::path& palPath);

public:
	ZTexture(ZFile* nParent);

	bool isPalette = false;

	void ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
	                            const uint32_t nRawDataIndex, const std::string& nRelPath) override;
	void FromBinary(TextureType nType, std::vector<uint8_t> nRawData, uint32_t rawDataIndex,
	                std::string nName, int32_t nWidth, int32_t nHeight);
	void FromPNG(const fs::path& pngFilePath, TextureType texType);
	void FromHLTexture(HLTexture* hlTex);
	static TextureType GetTextureTypeFromString(std::string str);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	std::string GetBodySourceCode() const;
	void CalcHash() override;
	void Save(const fs::path& outFolder) override;

	bool IsExternalResource() override;
	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;
	std::string GetExternalExtension() override;

	size_t GetRawDataSize() override;
	std::string GetIMFmtFromType();
	std::string GetIMSizFromType();
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	void SetDimensions(uint32_t nWidth, uint32_t nHeight);
	float GetPixelMultiplyer() const;
	TextureType GetTextureType();
	fs::path GetPoolOutPath(const fs::path& defaultValue);
};
