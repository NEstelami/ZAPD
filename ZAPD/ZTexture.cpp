#define TINYGLTF_IMPLEMENTATION

#include "ZTexture.h"
#include "BitConverter.h"
#include "CRC32.h"
#include "File.h"
#include "Globals.h"
#include "Path.h"

#include <Directory.h>
#include <png.h>

using namespace std;
using namespace tinyxml2;

REGISTER_ZFILENODE(Texture, ZTexture);

ZTexture::ZTexture(ZFile* nParent) : ZResource(nParent)
{
	width = 0;
	height = 0;
}

void ZTexture::FromBinary(TextureType nType, std::vector<uint8_t> nRawData, uint32_t nRawDataIndex,
                          std::string nName, int32_t nWidth, int32_t nHeight)
{
	width = nWidth;
	height = nHeight;
	type = nType;
	name = nName;
	outName = nName;
	rawDataIndex = nRawDataIndex;

	rawData.assign(nRawData.begin(), nRawData.end());

	ParseRawData();
}

void ZTexture::FromPNG(string pngFilePath, TextureType texType)
{
	type = texType;
	name = StringHelper::Split(Path::GetFileNameWithoutExtension(pngFilePath), ".")[0];
	PrepareRawData(pngFilePath);
}

void ZTexture::FromHLTexture(HLTexture* hlTex)
{
	width = hlTex->width;
	height = hlTex->height;
	type = (TextureType)hlTex->type;
}

void ZTexture::ParseXML(XMLElement* reader)
{
	ZResource::ParseXML(reader);

	if (reader->Attribute("Width") != nullptr)
	{
		width = atoi(reader->Attribute("Width"));
	}
	else
	{
		throw std::runtime_error("Width == nullptr for asset " + (string)reader->Attribute("Name"));
	}

	if (reader->Attribute("Height") != nullptr)
	{
		height = atoi(reader->Attribute("Height"));
	}
	else
	{
		throw std::runtime_error("Height == nullptr for asset " +
		                         (string)reader->Attribute("Name"));
	}

	string formatStr = reader->Attribute("Format");

	type = GetTextureTypeFromString(formatStr);

	if (type == TextureType::Error)
		throw std::runtime_error("Format " + formatStr + " is not supported!");
}

void ZTexture::ParseRawData()
{
	switch (type)
	{
	case TextureType::RGBA16bpp:
		PrepareBitmapRGBA16();
		break;
	case TextureType::RGBA32bpp:
		PrepareBitmapRGBA32();
		break;
	case TextureType::Grayscale4bpp:
		PrepareBitmapGrayscale4();
		break;
	case TextureType::Grayscale8bpp:
		PrepareBitmapGrayscale8();
		break;
	case TextureType::GrayscaleAlpha4bpp:
		PrepareBitmapGrayscaleAlpha4();
		break;
	case TextureType::GrayscaleAlpha8bpp:
		PrepareBitmapGrayscaleAlpha8();
		break;
	case TextureType::GrayscaleAlpha16bpp:
		PrepareBitmapGrayscaleAlpha16();
		break;
	case TextureType::Palette4bpp:
		PrepareBitmapPalette4();
		break;
	case TextureType::Palette8bpp:
		PrepareBitmapPalette8();
		break;
	default:
		throw std::runtime_error("Format is not supported!");
	}
}

void ZTexture::PrepareBitmapRGBA16()
{
	textureData.InitEmptyImage(width, height, PNG_COLOR_TYPE_RGBA, 8);
	// auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			int32_t pos = rawDataIndex + ((y * width) + x) * 2;
			uint16_t data = rawData.at(pos + 1) | (rawData.at(pos) << 8);
			uint8_t r = (data & 0xF800) >> 11;
			uint8_t g = (data & 0x07C0) >> 6;
			uint8_t b = (data & 0x003E) >> 1;
			uint8_t alpha = data & 0x01;

			textureData.SetRGBPixel(y, x, r * 8, g * 8, b * 8, alpha * 255);
		}
	}
}

void ZTexture::PrepareBitmapRGBA32()
{
	textureData.InitEmptyImage(width, height, PNG_COLOR_TYPE_RGBA, 8);
	// auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t pos = rawDataIndex + ((y * width) + x) * 4;
			uint8_t r = rawData.at(pos + 0);
			uint8_t g = rawData.at(pos + 1);
			uint8_t b = rawData.at(pos + 2);
			uint8_t alpha = rawData.at(pos + 3);

			textureData.SetRGBPixel(y, x, r, g, b, alpha);
		}
	}
}

void ZTexture::PrepareBitmapGrayscale4()
{
	textureData.InitEmptyImage(width, height, PNG_COLOR_TYPE_RGB, 8);
	// auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x += 2)
		{
			for (uint8_t i = 0; i < 2; i++)
			{
				size_t pos = rawDataIndex + ((y * width) + x) / 2;
				uint8_t grayscale = 0;

				if (i == 0)
					grayscale = rawData.at(pos) & 0xF0;
				else
					grayscale = (rawData.at(pos) & 0x0F) << 4;

				textureData.SetGrayscalePixel(y, x + i, grayscale);
			}
		}
	}
}

void ZTexture::PrepareBitmapGrayscale8()
{
	textureData.InitEmptyImage(width, height, PNG_COLOR_TYPE_RGB, 8);
	// auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t pos = rawDataIndex + ((y * width) + x) * 1;

			textureData.SetGrayscalePixel(y, x, rawData.at(pos));
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha4()
{
	textureData.InitEmptyImage(width, height, PNG_COLOR_TYPE_RGBA, 8);
	// auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x += 2)
		{
			for (uint16_t i = 0; i < 2; i++)
			{
				size_t pos = rawDataIndex + ((y * width) + x) / 2;
				uint8_t data = 0;

				if (i == 0)
					data = (rawData.at(pos) & 0xF0) >> 4;
				else
					data = rawData.at(pos) & 0x0F;

				uint8_t grayscale = ((data & 0x0E) >> 1) * 32;
				uint8_t alpha = (data & 0x01) * 255;

				textureData.SetGrayscalePixel(y, x + i, grayscale, alpha);
			}
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha8()
{
	textureData.InitEmptyImage(width, height, PNG_COLOR_TYPE_RGBA, 8);
	// auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t pos = rawDataIndex + ((y * width) + x) * 1;
			uint8_t grayscale = rawData.at(pos) & 0xF0;
			uint8_t alpha = (rawData.at(pos) & 0x0F) << 4;

			textureData.SetGrayscalePixel(y, x, grayscale, alpha);
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha16()
{
	textureData.InitEmptyImage(width, height, PNG_COLOR_TYPE_RGBA, 8);
	// auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t pos = rawDataIndex + ((y * width) + x) * 2;
			uint8_t grayscale = rawData.at(pos + 0);
			uint8_t alpha = rawData.at(pos + 1);

			textureData.SetGrayscalePixel(y, x, grayscale, alpha);
		}
	}
}

void ZTexture::PrepareBitmapPalette4()
{
	textureData.InitEmptyImage(width, height, PNG_COLOR_TYPE_RGB, 8);
	// auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x += 2)
		{
			for (uint16_t i = 0; i < 2; i++)
			{
				size_t pos = rawDataIndex + ((y * width) + x) / 2;
				uint8_t paletteIndex = 0;

				if (i == 0)
					paletteIndex = (rawData.at(pos) & 0xF0) >> 4;
				else
					paletteIndex = (rawData.at(pos) & 0x0F);

				textureData.SetGrayscalePixel(y, x, paletteIndex * 16);
			}
		}
	}
}

void ZTexture::PrepareBitmapPalette8()
{
	textureData.InitEmptyImage(width, height, PNG_COLOR_TYPE_RGB, 8);
	// auto parentRawData = parent->GetRawData();
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			size_t pos = rawDataIndex + ((y * width) + x) * 1;

			textureData.SetGrayscalePixel(y, x, rawData.at(pos));
		}
	}
}

void ZTexture::PrepareRawData(string pngFilePath)
{
	switch (type)
	{
	case TextureType::RGBA16bpp:
		PrepareRawDataRGBA16(pngFilePath);
		break;
	case TextureType::RGBA32bpp:
		PrepareRawDataRGBA32(pngFilePath);
		break;
	case TextureType::Grayscale4bpp:
		PrepareRawDataGrayscale4(pngFilePath);
		break;
	case TextureType::Grayscale8bpp:
		PrepareRawDataGrayscale8(pngFilePath);
		break;
	case TextureType::GrayscaleAlpha4bpp:
		PrepareRawDataGrayscaleAlpha4(pngFilePath);
		break;
	case TextureType::GrayscaleAlpha8bpp:
		PrepareRawDataGrayscaleAlpha8(pngFilePath);
		break;
	case TextureType::GrayscaleAlpha16bpp:
		PrepareRawDataGrayscaleAlpha16(pngFilePath);
		break;
	case TextureType::Palette4bpp:
		PrepareRawDataPalette4(pngFilePath);
		break;
	case TextureType::Palette8bpp:
		PrepareRawDataPalette8(pngFilePath);
		break;
	default:
		throw std::runtime_error("Format is not supported!");
	}
}

void ZTexture::PrepareRawDataRGBA16(string rgbaPath)
{
	textureData.ReadPng(rgbaPath.c_str());

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = ((y * width) + x) * 2;
			RGBAPixel pixel = textureData.GetPixel(y, x);

			uint8_t r = pixel.r / 8;
			uint8_t g = pixel.g / 8;
			uint8_t b = pixel.b / 8;

			uint8_t alphaBit = pixel.a != 0;

			uint16_t data = (r << 11) + (g << 6) + (b << 1) + alphaBit;

			textureDataRaw[pos + 0] = (data & 0xFF00) >> 8;
			textureDataRaw[pos + 1] = (data & 0x00FF);
		}
	}
}

void ZTexture::PrepareRawDataRGBA32(string rgbaPath)
{
	textureData.ReadPng(rgbaPath.c_str());

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = ((y * width) + x) * 4;
			RGBAPixel pixel = textureData.GetPixel(y, x);

			textureDataRaw[pos + 0] = pixel.r;
			textureDataRaw[pos + 1] = pixel.g;
			textureDataRaw[pos + 2] = pixel.b;
			textureDataRaw[pos + 3] = pixel.a;
		}
	}
}

void ZTexture::PrepareRawDataGrayscale4(string grayPath)
{
	textureData.ReadPng(grayPath.c_str());

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x += 2)
		{
			size_t pos = ((y * width) + x) / 2;
			uint8_t r1 = textureData.GetPixel(y, x).r;
			uint8_t r2 = textureData.GetPixel(y, x + 1).r;

			textureDataRaw[pos] = (uint8_t)(((r1 / 16) << 4) + (r2 / 16));
		}
	}
}

void ZTexture::PrepareRawDataGrayscale8(string grayPath)
{
	textureData.ReadPng(grayPath.c_str());

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = (y * width) + x;
			RGBAPixel pixel = textureData.GetPixel(y, x);
			textureDataRaw[pos] = pixel.r;
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha4(string grayAlphaPath)
{
	textureData.ReadPng(grayAlphaPath.c_str());

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x += 2)
		{
			size_t pos = ((y * width) + x) / 2;
			uint8_t data = 0;

			for (uint16_t i = 0; i < 2; i++)
			{
				RGBAPixel pixel = textureData.GetPixel(y, x + i);
				uint8_t cR = pixel.r;
				uint8_t alphaBit = pixel.a != 0;

				if (i == 0)
					data |= (((cR / 32) << 1) + alphaBit) << 4;
				else
					data |= ((cR / 32) << 1) + alphaBit;
			}

			textureDataRaw[pos] = data;
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha8(string grayAlphaPath)
{
	textureData.ReadPng(grayAlphaPath.c_str());

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = ((y * width) + x) * 1;
			RGBAPixel pixel = textureData.GetPixel(y, x);

			uint8_t r = pixel.r;
			uint8_t a = pixel.a;

			textureDataRaw[pos] = ((r / 16) << 4) + (a / 16);
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha16(string grayAlphaPath)
{
	textureData.ReadPng(grayAlphaPath.c_str());

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = ((y * width) + x) * 2;
			RGBAPixel pixel = textureData.GetPixel(y, x);

			uint8_t cR = pixel.r;
			uint8_t aR = pixel.a;

			textureDataRaw[pos + 0] = cR;
			textureDataRaw[pos + 1] = aR;
		}
	}
}

void ZTexture::PrepareRawDataPalette4(string palPath)
{
	textureData.ReadPng(palPath.c_str());

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x += 2)
		{
			size_t pos = ((y * width) + x) / 2;

			uint8_t cR1 = textureData.GetPixel(y, x).r;
			uint8_t cR2 = textureData.GetPixel(y, x + 1).r;

			textureDataRaw[pos] = ((cR1 / 16) << 4) + (cR2 / 16);
		}
	}
}

void ZTexture::PrepareRawDataPalette8(string palPath)
{
	textureData.ReadPng(palPath.c_str());

	width = textureData.GetWidth();
	height = textureData.GetHeight();

	textureDataRaw.clear();
	textureDataRaw.resize(GetRawDataSize());
	for (uint16_t y = 0; y < height; y++)
	{
		for (uint16_t x = 0; x < width; x++)
		{
			size_t pos = ((y * width) + x);
			RGBAPixel pixel = textureData.GetPixel(y, x);

			uint8_t cR = pixel.r;
			textureDataRaw[pos] = cR;
		}
	}
}

float ZTexture::GetPixelMultiplyer()
{
	switch (type)
	{
	case TextureType::Grayscale4bpp:
	case TextureType::GrayscaleAlpha4bpp:
	case TextureType::Palette4bpp:
		return 0.5f;
	case TextureType::Grayscale8bpp:
	case TextureType::GrayscaleAlpha8bpp:
	case TextureType::Palette8bpp:
		return 1;
	case TextureType::GrayscaleAlpha16bpp:
	case TextureType::RGBA16bpp:
		return 2;
	case TextureType::RGBA32bpp:
		return 4;
	default:
		return -1;
	}
}

size_t ZTexture::GetRawDataSize()
{
	return (width * height * GetPixelMultiplyer());
}

std::string ZTexture::GetIMFmtFromType()
{
	switch (type)
	{
	case TextureType::RGBA32bpp:
	case TextureType::RGBA16bpp:
		return "G_IM_FMT_RGBA";
	case TextureType::Grayscale4bpp:
	case TextureType::Grayscale8bpp:
		return "G_IM_FMT_I";
	case TextureType::Palette4bpp:
	case TextureType::Palette8bpp:
		return "G_IM_FMT_CI";
	case TextureType::GrayscaleAlpha4bpp:
	case TextureType::GrayscaleAlpha8bpp:
	case TextureType::GrayscaleAlpha16bpp:
		return "G_IM_FMT_IA";
	default:
		return "ERROR";
	}
}

std::string ZTexture::GetIMSizFromType()
{
	switch (type)
	{
	case TextureType::Grayscale4bpp:
	case TextureType::Palette4bpp:
	case TextureType::GrayscaleAlpha4bpp:
		return "G_IM_SIZ_4b";
	case TextureType::Palette8bpp:
	case TextureType::Grayscale8bpp:
		return "G_IM_SIZ_8b";
	case TextureType::GrayscaleAlpha16bpp:
	case TextureType::RGBA16bpp:
		return "G_IM_SIZ_16b";
	case TextureType::RGBA32bpp:
		return "G_IM_SIZ_32b";
	default:
		return "ERROR";
	}
}

uint32_t ZTexture::GetWidth()
{
	return width;
}

uint32_t ZTexture::GetHeight()
{
	return height;
}

void ZTexture::SetDimensions(uint32_t nWidth, uint32_t nHeight)
{
	width = nWidth;
	height = nHeight;
	ParseRawData();
}

TextureType ZTexture::GetTextureType()
{
	return type;
}

void ZTexture::Save(const std::string& outFolder)
{
	// Optionally generate text file containing CRC information. This is going to be a one time
	// process for generating the Texture Pool XML.
	if (Globals::Instance->outputCrc)
	{
		if (hash == 0)
		{
			CalcHash();
		}
		File::WriteAllText(StringHelper::Sprintf("%s/%s.txt", Globals::Instance->outputPath.c_str(),
		                                         outName.c_str()),
		                   StringHelper::Sprintf("%08lX", hash));
	}

	std::string outPath = GetPoolOutPath(outFolder);

	if (!Directory::Exists(outPath))
		Directory::CreateDirectory(outPath);

	textureData.WritePng((outPath + "/" + outName + "." + GetExternalExtension() + ".png").c_str());
}

std::string ZTexture::GetBodySourceCode() const
{
	std::string sourceOutput = "";

	for (size_t i = 0; i < textureDataRaw.size(); i += 8)
	{
		if (i % 32 == 0)
			sourceOutput += "    ";

		sourceOutput +=
			StringHelper::Sprintf("0x%016llX, ", BitConverter::ToUInt64BE(textureDataRaw, i));

		if (i % 32 == 24)
			sourceOutput += StringHelper::Sprintf(" // 0x%06X \n", rawDataIndex + ((i / 32) * 32));
	}

	// Ensure there's always a trailing line feed to prevent dumb warnings.
	// Please don't remove this line, unless you somehow made a way to prevent
	// that warning when building the OoT repo.
	sourceOutput += "\n";

	return sourceOutput;
}

bool ZTexture::IsExternalResource()
{
	return true;
}

ZResourceType ZTexture::GetResourceType()
{
	return ZResourceType::Texture;
}

std::string ZTexture::GetSourceTypeName()
{
	return "u64";
}

void ZTexture::CalcHash()
{
	// auto parentRawData = parent->GetRawData();
	hash = CRC32B(rawData.data() + rawDataIndex, GetRawDataSize());
}

std::string ZTexture::GetExternalExtension()
{
	switch (type)
	{
	case TextureType::RGBA32bpp:
		return "rgba32";
	case TextureType::RGBA16bpp:
		return "rgb5a1";
	case TextureType::Grayscale4bpp:
		return "i4";
	case TextureType::Grayscale8bpp:
		return "i8";
	case TextureType::GrayscaleAlpha4bpp:
		return "ia4";
	case TextureType::GrayscaleAlpha8bpp:
		return "ia8";
	case TextureType::GrayscaleAlpha16bpp:
		return "ia16";
	case TextureType::Palette4bpp:
		return "ci4";
	case TextureType::Palette8bpp:
		return "ci8";
	default:
		return "ERROR";
	}
}

std::string ZTexture::GetPoolOutPath(std::string defaultValue)
{
	if (Globals::Instance->cfg.texturePool.find(hash) != Globals::Instance->cfg.texturePool.end())
		return Path::GetDirectoryName(Globals::Instance->cfg.texturePool[hash].path);

	return defaultValue;
}

TextureType ZTexture::GetTextureTypeFromString(string str)
{
	TextureType texType = TextureType::Error;

	if (str == "rgba32")
		texType = TextureType::RGBA32bpp;
	else if (str == "rgb5a1")
		texType = TextureType::RGBA16bpp;
	else if (str == "i4")
		texType = TextureType::Grayscale4bpp;
	else if (str == "i8")
		texType = TextureType::Grayscale8bpp;
	else if (str == "ia4")
		texType = TextureType::GrayscaleAlpha4bpp;
	else if (str == "ia8")
		texType = TextureType::GrayscaleAlpha8bpp;
	else if (str == "ia16")
		texType = TextureType::GrayscaleAlpha16bpp;
	else if (str == "ci4")
		texType = TextureType::Palette4bpp;
	else if (str == "ci8")
		texType = TextureType::Palette8bpp;
	else
		fprintf(stderr, "Encountered Unknown Texture Type %s \n", str.c_str());
	return texType;
}
