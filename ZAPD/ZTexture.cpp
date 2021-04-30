#define TINYGLTF_IMPLEMENTATION

#include "ZTexture.h"
#include "BitConverter.h"
#include "CRC32.h"
#include "File.h"
#include "Globals.h"
#include "Path.h"

#include <Directory.h>
//#include "stb_image.h"
//#include "stb_image_write.h"
#include <png.h>

using namespace std;
using namespace tinyxml2;

REGISTER_ZFILENODE(Texture, ZTexture);

ZTexture::ZTexture(ZFile* nParent) : ZResource(nParent)
{
	//bmpRgb = nullptr;
	//bmpRgba = nullptr;
	//pixelMatrix = nullptr;
	width = 0;
	height = 0;
	type = TextureType::Error;
	isPalette = false;
	isRawDataFixed = false;
}

ZTexture::~ZTexture()
{
	/*
	if (pixelMatrix != nullptr)
	{
		//stbi_image_free(bmpRgb);
		pixelMatrix = nullptr;
	}

	if (pixelMatrix != nullptr)
	{
		//stbi_image_free(bmpRgba);
		pixelMatrix = nullptr;
	}

	width = 0;
	height = 0;
	type = TextureType::Error;
	*/
}

void ZTexture::ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
                              const uint32_t nRawDataIndex, const std::string& nRelPath)
{
	ParseXML(reader);
	rawDataIndex = nRawDataIndex;
	rawData = vector<uint8_t>(nRawData.data() + rawDataIndex,
	                          nRawData.data() + rawDataIndex + GetRawDataSize());

	relativePath = nRelPath;

	FixRawData();
	PrepareBitmap();
}

ZTexture* ZTexture::FromBinary(TextureType nType, std::vector<uint8_t> nRawData,
                               uint32_t nRawDataIndex, std::string nName, int32_t nWidth,
                               int32_t nHeight, ZFile* nParent)
{
	ZTexture* tex = new ZTexture(nParent);

	tex->width = nWidth;
	tex->height = nHeight;
	tex->type = nType;
	tex->name = nName;
	tex->outName = nName;
	tex->rawDataIndex = nRawDataIndex;

	size_t dataEnd = tex->rawDataIndex + tex->GetRawDataSize();
	tex->rawData = vector<uint8_t>(nRawData.data() + tex->rawDataIndex, nRawData.data() + dataEnd);

	tex->FixRawData();
	tex->CalcHash();
	tex->PrepareBitmap();

	return tex;
}

ZTexture* ZTexture::FromPNG(string pngFilePath, TextureType texType)
{
	int32_t comp;
	ZTexture* tex = new ZTexture(nullptr);
	tex->type = texType;
	tex->name = StringHelper::Split(Path::GetFileNameWithoutExtension(pngFilePath), ".")[0];

	//tex->bmpRgb = (uint8_t*)stbi_load((pngFilePath).c_str(), (int*)&tex->width, (int*)&tex->height,
	//                                  &comp, STBI_rgb);
	//stbi_image_free(tex->bmpRgb);
	//tex->bmpRgb = nullptr;
	//tex->rawData = vector<uint8_t>(tex->GetRawDataSize());

	switch (texType)
	{
	case TextureType::RGBA16bpp:
		tex->PrepareRawDataRGBA16(pngFilePath);
		break;
	case TextureType::RGBA32bpp:
		tex->PrepareRawDataRGBA32(pngFilePath);
		break;
	case TextureType::Grayscale4bpp:
		tex->PrepareRawDataGrayscale4(pngFilePath);
		break;
	case TextureType::Grayscale8bpp:
		tex->PrepareRawDataGrayscale8(pngFilePath);
		break;
	case TextureType::GrayscaleAlpha4bpp:
		tex->PrepareRawDataGrayscaleAlpha4(pngFilePath);
		break;
	case TextureType::GrayscaleAlpha8bpp:
		tex->PrepareRawDataGrayscaleAlpha8(pngFilePath);
		break;
	case TextureType::GrayscaleAlpha16bpp:
		tex->PrepareRawDataGrayscaleAlpha16(pngFilePath);
		break;
	case TextureType::Palette4bpp:
		tex->PrepareRawDataPalette4(pngFilePath);
		break;
	case TextureType::Palette8bpp:
		tex->PrepareRawDataPalette8(pngFilePath);
		break;
	default:
		throw std::runtime_error("Format is not supported!");
	}

	tex->FixRawData();

	return tex;
}

ZTexture* ZTexture::FromHLTexture(HLTexture* hlTex)
{
	ZTexture* tex = new ZTexture(nullptr);

	tex->width = hlTex->width;
	tex->height = hlTex->height;
	tex->type = (TextureType)hlTex->type;

	return tex;
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

void ZTexture::FixRawData()
{
	if (type == TextureType::RGBA32bpp)
	{
		for (size_t i = 0; i < rawData.size(); i += 4)
		{
			uint8_t tmp = rawData[i];
			rawData[i] = rawData[i + 2];
			rawData[i + 2] = tmp;
		}
	}
	else if (type == TextureType::RGBA16bpp)  // || type == TextureType::GrayscaleAlpha16bpp)
	{
		for (size_t i = 0; i < rawData.size(); i += 2)
		{
			uint8_t tmp = rawData[i];
			rawData[i] = rawData[i + 1];
			rawData[i + 1] = tmp;
		}
	}

	isRawDataFixed = !isRawDataFixed;
}

void ZTexture::PrepareBitmap()
{
	/*
	switch (type)
	{
	case TextureType::RGBA16bpp:
	case TextureType::RGBA32bpp:
	case TextureType::GrayscaleAlpha4bpp:
	case TextureType::GrayscaleAlpha8bpp:
	case TextureType::GrayscaleAlpha16bpp:
		bmpRgba = new uint8_t[width * height * 4];
		break;
	default:
		bmpRgb = new uint8_t[width * height * 3];
		break;
	}
	*/
	/*pixelMatrix = new uint8_t*[height];
	for(int y = 0; y < height; y++) {
		pixelMatrix[y] = new uint8_t[width*4];
	}*/

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
	textureData.resize(height);
	for (size_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (size_t x = 0; x < width; x++)
		{
			int32_t pos = ((y * width) + x) * 2;
			uint16_t data = (uint16_t)((rawData[pos + 1] << 8) + rawData[pos]);
			uint8_t r = (uint8_t)((data & 0xF800) >> 11);
			uint8_t g = (uint8_t)((data & 0x07C0) >> 6);
			uint8_t b = (uint8_t)((data & 0x003E) >> 1);
			uint8_t alpha = (uint8_t)(data & 0x01);

			textureData[y][x].SetRGBA(r * 8, g * 8, b * 8, alpha * 255);
		}
	}
}

void ZTexture::PrepareBitmapRGBA32()
{
	textureData.resize(height);
	for (size_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (size_t x = 0; x < width; x++)
		{
			uint16_t pos = ((y * width) + x) * 4;
			uint8_t r = rawData.at(pos + 2);
			uint8_t g = rawData.at(pos + 1);
			uint8_t b = rawData.at(pos + 0);
			uint8_t alpha = rawData.at(pos + 3);

			textureData[y][x].SetRGBA(r, g, b, alpha);
		}
	}
}

void ZTexture::PrepareBitmapGrayscale4()
{
	textureData.resize(height);
	for (size_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (size_t x = 0; x < width; x += 2)
		{
			for (uint8_t i = 0; i < 2; i++)
			{
				uint16_t pos = ((y * width) + x) / 2;
				uint8_t grayscale = 0;

				if (i == 0)
					grayscale = rawData[pos] & 0xF0;
				else
					grayscale = (rawData[pos] & 0x0F) << 4;

				textureData[y][x + i].SetGrayscale(grayscale);
			}
		}
	}
}

void ZTexture::PrepareBitmapGrayscale8()
{
	textureData.resize(height);
	for (size_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (size_t x = 0; x < width; x++)
		{
			uint16_t pos = ((y * width) + x) * 1;

			textureData[y][x].SetGrayscale(rawData.at(pos));
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha4()
{
	textureData.resize(height);
	for (size_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (size_t x = 0; x < width; x += 2)
		{
			for (uint16_t i = 0; i < 2; i++)
			{
				uint16_t pos = ((y * width) + x) / 2;
				uint8_t data = 0;

				if (i == 0)
					data = (rawData[pos] & 0xF0) >> 4;
				else
					data = rawData[pos] & 0x0F;

				uint8_t grayscale = ((data & 0x0E) >> 1) * 32;
				uint8_t alpha = (data & 0x01) * 255;

				textureData[y][x + i].SetGrayscale(grayscale, alpha);
			}
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha8()
{
	textureData.resize(height);
	for (size_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (size_t x = 0; x < width; x++)
		{
			uint16_t pos = ((y * width) + x) * 1;
			uint8_t grayscale = rawData[pos] & 0xF0;
			uint8_t alpha = (rawData[pos] & 0x0F) << 4;

			textureData[y][x].SetGrayscale(grayscale, alpha);
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha16()
{
	textureData.resize(height);
	for (size_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (size_t x = 0; x < width; x++)
		{
			uint16_t pos = ((y * width) + x) * 2;
			uint8_t grayscale = rawData[pos + 0];
			uint8_t alpha = rawData[pos + 1];

			textureData[y][x].SetGrayscale(grayscale, alpha);
		}
	}
}

void ZTexture::PrepareBitmapPalette4()
{
	textureData.resize(height);
	for (size_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (size_t x = 0; x < width; x += 2)
		{
			for (uint16_t i = 0; i < 2; i++)
			{
				uint16_t pos = ((y * width) + x) / 2;
				uint8_t paletteIndex = 0;

				if (i == 0)
					paletteIndex = (rawData[pos] & 0xF0) >> 4;
				else
					paletteIndex = (rawData[pos] & 0x0F);

				textureData[y][x + i].SetGrayscale(paletteIndex * 16);
			}
		}
	}
}

void ZTexture::PrepareBitmapPalette8()
{
	textureData.resize(height);
	for (size_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (size_t x = 0; x < width; x++)
		{
			uint16_t pos = ((y * width) + x) * 1;

			textureData[y][x].SetGrayscale(rawData.at(pos));
		}
	}
}

void ZTexture::PrepareRawData(string inFolder)
{
	rawData = vector<uint8_t>(GetRawDataSize());

	switch (type)
	{
	case TextureType::RGBA16bpp:
		PrepareRawDataRGBA16(inFolder + "/" + outName + ".rgba5a1.png");
		break;
	case TextureType::RGBA32bpp:
		PrepareRawDataRGBA32(inFolder + "/" + outName + ".rgba32.png");
		break;
	case TextureType::Grayscale4bpp:
		PrepareRawDataGrayscale4(inFolder + "/" + outName + ".i4.png");
		break;
	case TextureType::Grayscale8bpp:
		PrepareRawDataGrayscale8(inFolder + "/" + outName + ".i8.png");
		break;
	case TextureType::GrayscaleAlpha4bpp:
		PrepareRawDataGrayscaleAlpha4(inFolder + "/" + outName + ".ia4.png");
		break;
	case TextureType::GrayscaleAlpha8bpp:
		PrepareRawDataGrayscaleAlpha8(inFolder + "/" + outName + ".ia8.png");
		break;
	case TextureType::GrayscaleAlpha16bpp:
		PrepareRawDataGrayscaleAlpha16(inFolder + "/" + outName + ".ia16.png");
		break;
	case TextureType::Palette4bpp:
		PrepareRawDataPalette4(inFolder + "/" + outName + ".ci4.png");
		break;
	case TextureType::Palette8bpp:
		PrepareRawDataPalette8(inFolder + "/" + outName + ".ci8.png");
		break;
	default:
		throw std::runtime_error("Build Mode: Format is not supported!");
	}
}

void ZTexture::PrepareRawDataRGBA16(string rgbaPath)
{
	//uint32_t width;
	//uint32_t height;
	uint32_t comp;
	uint8_t color_type;
	uint8_t bit_depth;

	//bmpRgba = (uint8_t*)stbi_load(rgbaPath.c_str(), &width, &height, &comp, STBI_rgb_alpha);
	//pixelMatrix = read_png_file(rgbaPath.c_str(), &width, &height, &color_type, &bit_depth);
	ImageBackend image;
	image.ReadPng(rgbaPath.c_str());

	width = image.GetWidth();
	height = image.GetHeight();

	rawData.clear();
	rawData.resize(GetRawDataSize());

	textureData.resize(height);
	for (uint16_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (uint16_t x = 0; x < width; x++)
		{
			uint16_t pos = ((y * width) + x) * 2;
			RGBAPixel pixel = image.GetPixel(y, x);

			uint8_t r = pixel.r / 8;
			uint8_t g = pixel.g / 8;
			uint8_t b = pixel.b / 8;

			uint8_t alphaBit = pixel.a != 0;

			uint16_t data = (r << 11) + (g << 6) + (b << 1) + alphaBit;

			rawData[pos + 0] = (data & 0xFF00) >> 8;
			rawData[pos + 1] = (data & 0x00FF);
		}
	}
}

void ZTexture::PrepareRawDataRGBA32(string rgbaPath)
{
	//int32_t width;
	//int32_t height;
	//int32_t comp;
	uint8_t color_type;
	uint8_t bit_depth;

	//bmpRgba = (uint8_t*)stbi_load(rgbaPath.c_str(), &width, &height, &comp, STBI_rgb_alpha);
	// pixelMatrix = read_png_file(rgbaPath.c_str(), &width, &height, &color_type, &bit_depth);
	ImageBackend image;
	image.ReadPng(rgbaPath.c_str());

	width = image.GetWidth();
	height = image.GetHeight();

	rawData.clear();
	rawData.resize(GetRawDataSize());

	textureData.resize(height);
	for (uint16_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (uint16_t x = 0; x < width; x++)
		{
			uint16_t pos = ((y * width) + x) * 4;
			RGBAPixel pixel = image.GetPixel(y, x);

			rawData[pos + 0] = pixel.r;
			rawData[pos + 1] = pixel.g;
			rawData[pos + 2] = pixel.b;
			rawData[pos + 3] = pixel.a;
		}
	}
}

void ZTexture::PrepareRawDataGrayscale4(string grayPath)
{
	//int32_t width;
	//int32_t height;
	//int32_t comp;
	uint8_t color_type;
	uint8_t bit_depth;

	//bmpRgb = (uint8_t*)stbi_load(grayPath.c_str(), &width, &height, &comp, STBI_rgb);
	//pixelMatrix = read_png_file(grayPath.c_str(), &width, &height, &color_type, &bit_depth);
	ImageBackend image;
	image.ReadPng(grayPath.c_str());

	width = image.GetWidth();
	height = image.GetHeight();

	rawData.clear();
	rawData.resize(GetRawDataSize());

	textureData.resize(height);
	for (uint16_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (uint16_t x = 0; x < width; x += 2)
		{
			uint16_t pos = ((y * width) + x) / 2;
			uint8_t r1 = image.GetPixel(y, x).r;
			uint8_t r2 = image.GetPixel(y, x + 1).r;

			rawData[pos] = (uint8_t)(((r1 / 16) << 4) + (r2 / 16));
		}
	}
}

void ZTexture::PrepareRawDataGrayscale8(string grayPath)
{
	//int32_t width;
	//int32_t height;
	//int32_t comp;
	uint8_t color_type;
	uint8_t bit_depth;

	//bmpRgb = (uint8_t*)stbi_load(grayPath.c_str(), &width, &height, &comp, STBI_rgb);
	//pixelMatrix = read_png_file(grayPath.c_str(), &width, &height, &color_type, &bit_depth);
	ImageBackend image;
	image.ReadPng(grayPath.c_str());

	width = image.GetWidth();
	height = image.GetHeight();

	rawData.clear();
	rawData.resize(GetRawDataSize());

	textureData.resize(height);
	for (uint16_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (uint16_t x = 0; x < width; x++)
		{
			uint16_t pos = (y * width) + x;
			RGBAPixel pixel = image.GetPixel(y, x);
			rawData[pos] = pixel.r;
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha4(string grayAlphaPath)
{
	//int32_t width;
	//int32_t height;
	//int32_t comp;
	uint8_t color_type;
	uint8_t bit_depth;

	//bmpRgba = (uint8_t*)stbi_load(grayAlphaPath.c_str(), &width, &height, &comp, STBI_rgb_alpha);
	//pixelMatrix = read_png_file(grayAlphaPath.c_str(), &width, &height, &color_type, &bit_depth);
	ImageBackend image;
	image.ReadPng(grayAlphaPath.c_str());

	width = image.GetWidth();
	height = image.GetHeight();

	rawData.clear();
	rawData.resize(GetRawDataSize());

	textureData.resize(height);
	for (uint16_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (uint16_t x = 0; x < width; x += 2)
		{
			uint16_t pos = ((y * width) + x) / 2;
			uint8_t data = 0;

			for (uint16_t i = 0; i < 2; i++)
			{
				RGBAPixel pixel = image.GetPixel(y, x + i);
				uint8_t cR = pixel.r;
				uint8_t alphaBit = pixel.a != 0;

				if (i == 0)
					data |= (((cR / 32) << 1) + alphaBit) << 4;
				else
					data |= ((cR / 32) << 1) + alphaBit;
			}

			rawData[pos] = data;
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha8(string grayAlphaPath)
{
	//int32_t width;
	//int32_t height;
	//int32_t comp;
	uint8_t color_type;
	uint8_t bit_depth;

	//bmpRgba = (uint8_t*)stbi_load(grayAlphaPath.c_str(), &width, &height, &comp, STBI_rgb_alpha);
	//pixelMatrix = read_png_file(grayAlphaPath.c_str(), &width, &height, &color_type, &bit_depth);
	ImageBackend image;
	image.ReadPng(grayAlphaPath.c_str());

	width = image.GetWidth();
	height = image.GetHeight();

	rawData.clear();
	rawData.resize(GetRawDataSize());

	textureData.resize(height);
	for (uint16_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (uint16_t x = 0; x < width; x++)
		{
			uint16_t pos = ((y * width) + x) * 1;
			RGBAPixel pixel = image.GetPixel(y, x);

			uint8_t r = pixel.r;
			uint8_t a = pixel.a;

			rawData[pos] = ((r / 16) << 4) + (a / 16);
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha16(string grayAlphaPath)
{
	//int32_t width;
	//int32_t height;
	//int32_t comp;
	uint8_t color_type;
	uint8_t bit_depth;

	//bmpRgba = (uint8_t*)stbi_load(grayAlphaPath.c_str(), &width, &height, &comp, STBI_rgb_alpha);
	//pixelMatrix = read_png_file(grayAlphaPath.c_str(), &width, &height, &color_type, &bit_depth);
	ImageBackend image;
	image.ReadPng(grayAlphaPath.c_str());

	width = image.GetWidth();
	height = image.GetHeight();

	rawData.clear();
	rawData.resize(GetRawDataSize());

	textureData.resize(height);
	for (uint16_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (uint16_t x = 0; x < width; x++)
		{
			uint16_t pos = ((y * width) + x) * 2;
			RGBAPixel pixel = image.GetPixel(y, x);

			uint8_t cR = pixel.r;
			uint8_t aR = pixel.a;

			rawData[pos + 0] = cR;
			rawData[pos + 1] = aR;
		}
	}
}

void ZTexture::PrepareRawDataPalette4(string palPath)
{
	//int32_t width;
	//int32_t height;
	//int32_t comp;
	uint8_t color_type;
	uint8_t bit_depth;

	//bmpRgb = (uint8_t*)stbi_load(palPath.c_str(), &width, &height, &comp, STBI_rgb);
	//pixelMatrix = read_png_file(palPath.c_str(), &width, &height, &color_type, &bit_depth);
	ImageBackend image;
	image.ReadPng(palPath.c_str());

	width = image.GetWidth();
	height = image.GetHeight();

	rawData.clear();
	rawData.resize(GetRawDataSize());

	textureData.resize(height);
	for (uint16_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (uint16_t x = 0; x < width; x += 2)
		{
			uint16_t pos = ((y * width) + x) / 2;
			//RGBAPixel pixel = image.GetPixel(y, x);

			uint8_t cR1 = image.GetPixel(y, x).r;
			uint8_t cR2 = image.GetPixel(y, x + 1).r;

			rawData[pos] = ((cR1 / 16) << 4) + (cR2 / 16);
		}
	}
}

void ZTexture::PrepareRawDataPalette8(string palPath)
{
	//int32_t width;
	//int32_t height;
	//int32_t comp;
	uint8_t color_type;
	uint8_t bit_depth;

	//bmpRgb = (uint8_t*)stbi_load(palPath.c_str(), &width, &height, &comp, STBI_rgb);
	//pixelMatrix = read_png_file(palPath.c_str(), &width, &height, &color_type, &bit_depth);
	ImageBackend image;
	image.ReadPng(palPath.c_str());

	width = image.GetWidth();
	height = image.GetHeight();

	rawData.clear();
	rawData.resize(GetRawDataSize());

	textureData.resize(height);
	for (uint16_t y = 0; y < height; y++)
	{
		textureData.at(y).resize(width);
		for (uint16_t x = 0; x < width; x++)
		{
			uint16_t pos = ((y * width) + x);
			RGBAPixel pixel = image.GetPixel(y, x);

			uint8_t cR = pixel.r;
			rawData[pos] = cR;
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
/*
void ZTexture::SetWidth(uint32_t nWidth)
{
	width = nWidth;
}

void ZTexture::SetHeight(uint32_t nHeight)
{
	height = nHeight;
}
*/


void ZTexture::SetDimensions(uint32_t nWidth, uint32_t nHeight)
{
	width = nWidth;
	height = nHeight;
	PrepareBitmap();
}

	/*
void ZTexture::Linealize(uint32_t newDim)
{
	uint8_t** newMatrix = new uint8_t*[1];
	newMatrix[0] = new uint8_t[width * height * 4];

	for (size_t y = 0; y < height && y * width < newDim; y++)
	{
		size_t pos = y * width * 4;
		size_t bytes_to_copy = sizeof(uint8_t) * width * 4;
		std::memcpy(&newMatrix[0][pos], pixelMatrix[y], bytes_to_copy);
	}

	// TODO: Free!
	pixelMatrix = newMatrix;

	width = newDim;
	height = 1;
}
	*/

TextureType ZTexture::GetTextureType()
{
	return type;
}

void ZTexture::Save(const std::string& outFolder)
{
	//PrepareBitmap();
	// Optionally generate text file containing CRC information. This is going to be a one time
	// process for generating the Texture Pool XML.
	if (Globals::Instance->testMode)
	{
		if (hash != 0)
		{
			File::WriteAllText(StringHelper::Sprintf("%s/%s.txt",
			                                         Globals::Instance->outputPath.c_str(),
			                                         outName.c_str()),
			                   StringHelper::Sprintf("%08lX", hash));
			hash = 0;
		}
	}

	std::string outPath = GetPoolOutPath(outFolder);

	if (!Directory::Exists(outPath))
		Directory::CreateDirectory(outPath);

	uint8_t color_type = 0;
 	uint8_t bit_depth = 0;

	switch (type)
	{
	case TextureType::RGBA32bpp:
		color_type = PNG_COLOR_TYPE_RGBA;
		//bit_depth = 32;
		bit_depth = 8;
		break;
	case TextureType::RGBA16bpp:
		color_type = PNG_COLOR_TYPE_RGBA;
		//bit_depth = 16;
		bit_depth = 8;
		break;
	case TextureType::Grayscale8bpp:
		//color_type = PNG_COLOR_TYPE_GRAY;
		color_type = PNG_COLOR_TYPE_RGB;
		bit_depth = 8;
		break;
	case TextureType::Grayscale4bpp:
		//color_type = PNG_COLOR_TYPE_GRAY;
		color_type = PNG_COLOR_TYPE_RGB;
		//bit_depth = 4;
		bit_depth = 8;
		break;
	case TextureType::GrayscaleAlpha16bpp:
		//color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
		color_type = PNG_COLOR_TYPE_RGBA;
		//bit_depth = 16;
		bit_depth = 8;
		break;
	case TextureType::GrayscaleAlpha8bpp:
		//color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
		color_type = PNG_COLOR_TYPE_RGBA;
		bit_depth = 8;
		break;
	case TextureType::GrayscaleAlpha4bpp:
		//color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
		color_type = PNG_COLOR_TYPE_RGBA;
		//bit_depth = 4;
		bit_depth = 8;
		break;
	case TextureType::Palette4bpp:
		//color_type = PNG_COLOR_TYPE_PALETTE;
		//color_type = PNG_COLOR_TYPE_GRAY;
		color_type = PNG_COLOR_TYPE_RGB;
		//bit_depth = 4;
		bit_depth = 8;
		break;
	case TextureType::Palette8bpp:
		//color_type = PNG_COLOR_TYPE_PALETTE;
		//color_type = PNG_COLOR_TYPE_GRAY;
		color_type = PNG_COLOR_TYPE_RGB;
		bit_depth = 8;
		break;
	}

	ImageBackend image;
	image.SetTextureData(textureData, width, height, color_type, bit_depth);
	image.WritePng((outPath + "/" + outName + "." + GetExternalExtension() + ".png").c_str());

	//write_png_file((outPath + "/" + outName + "." + GetExternalExtension() + ".png").c_str(), width, height, pixelMatrix, color_type, bit_depth);
	#if 0
	if (type == TextureType::RGBA32bpp)
		/*stbi_write_png((outPath + "/" + outName + ".rgba32.png").c_str(), width, height, 4, bmpRgba,
		               width * 4);*/
	else if (type == TextureType::RGBA16bpp)
		/*stbi_write_png((outPath + "/" + outName + ".rgb5a1.png").c_str(), width, height, 4, bmpRgba,
		               width * 4);*/
	else if (type == TextureType::Grayscale8bpp)
		/*stbi_write_png((outPath + "/" + outName + ".i8.png").c_str(), width, height, 3, bmpRgb,
		               width * 3);*/
	else if (type == TextureType::Grayscale4bpp)
		/*stbi_write_png((outPath + "/" + outName + ".i4.png").c_str(), width, height, 3, bmpRgb,
		               width * 3);*/
	else if (type == TextureType::GrayscaleAlpha16bpp)
		/*stbi_write_png((outPath + "/" + outName + ".ia16.png").c_str(), width, height, 4, bmpRgba,
		               width * 4);*/
	else if (type == TextureType::GrayscaleAlpha8bpp)
		/*stbi_write_png((outPath + "/" + outName + ".ia8.png").c_str(), width, height, 4, bmpRgba,
		               width * 4);*/
	else if (type == TextureType::GrayscaleAlpha4bpp)
		/*stbi_write_png((outPath + "/" + outName + ".ia4.png").c_str(), width, height, 4, bmpRgba,
		               width * 4);*/
	else if (type == TextureType::Palette4bpp)
		/*stbi_write_png((outPath + "/" + outName + ".ci4.png").c_str(), width, height, 3, bmpRgb,
		               width * 3);*/
	else if (type == TextureType::Palette8bpp)
		/*stbi_write_png((outPath + "/" + outName + ".ci8.png").c_str(), width, height, 3, bmpRgb,
		               width * 3);*/
	#endif

	// if (outName != name && outName != "")
	// File::WriteAllText(outFolder + "/" + outName + ".cfg", name.c_str());
}

string ZTexture::GetSourceOutputCode(const std::string& prefix)
{
	sourceOutput = "";

	relativePath = "build/assets/" + relativePath;
	FixRawData();

	for (size_t i = 0; i < rawData.size(); i += 8)
	{
		if (i % 32 == 0)
			sourceOutput += "    ";

		sourceOutput +=
			StringHelper::Sprintf("0x%016llX, ", BitConverter::ToUInt64BE(rawData, i));

			//../ZAPD/ZAPD.out e -eh -i assets/xml/objects/object_gnd.xml -b baserom/ -o assets/objects/object_gnd -gsf 1 -ifp 0 -sm tools/ZAPD/SymbolMap_OoTMqDbg.txt


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
	// Make sure raw data is fixed before we calc the hash...
	bool fixFlag = !isRawDataFixed;

	if (fixFlag)
		FixRawData();

	hash = CRC32B(rawData.data(), GetRawDataSize());

	if (fixFlag)
		FixRawData();
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

string ZTexture::GetSourceOutputHeader(const std::string& prefix)
{
	return "";
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
