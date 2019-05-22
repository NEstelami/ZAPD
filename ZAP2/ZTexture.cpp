#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "ZTexture.h"

#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;
using namespace tinyxml2;

// EXTRACT MODE
ZTexture::ZTexture(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex)
{
	ParseXML(reader);
	rawData = vector<uint8_t>(rawData.data() + rawDataIndex, rawData.data() + rawDataIndex + GetRawDataSize());

	FixRawData();
	PrepareBitmap();
}

ZTexture::ZTexture(XMLElement* reader, string inFolder)
{
	ParseXML(reader);

	PrepareRawData(inFolder);
}

void ZTexture::ParseXML(XMLElement* reader)
{
	name = reader->Attribute("Name");
	width = atoi(reader->Attribute("Width"));
	height = atoi(reader->Attribute("Height"));

	string formatStr = reader->Attribute("Format");

	if (formatStr == "rgba32")
		type = TextureType::RGBA32bpp;
	else if (formatStr == "rgb5a1")
		type = TextureType::RGBA16bpp;
	else if (formatStr == "i4")
		type = TextureType::Grayscale4bpp;
	else if (formatStr == "i8")
		type = TextureType::Grayscale8bpp;
	else if (formatStr == "ia4")
		type = TextureType::GrayscaleAlpha4bpp;
	else if (formatStr == "ia8")
		type = TextureType::GrayscaleAlpha8bpp;
	else if (formatStr == "ia16")
		type = TextureType::GrayscaleAlpha16bpp;
	else if (formatStr == "ci4")
		type = TextureType::Palette4bpp;
	else if (formatStr == "ci8")
		type = TextureType::Palette8bpp;
	else
		throw "Format " + formatStr + " is not supported!";
}

void ZTexture::FixRawData()
{
	if (type == TextureType::RGBA32bpp)
	{
		for (int i = 0; i < rawData.size(); i += 4)
		{
			uint8_t tmp = rawData[i];
			rawData[i] = rawData[i + 2];
			rawData[i + 2] = tmp;
		}
	}
	else if (type == TextureType::RGBA16bpp)
	{
		for (int i = 0; i < rawData.size(); i += 2)
		{
			uint8_t tmp = rawData[i];
			rawData[i] = rawData[i + 1];
			rawData[i + 1] = tmp;
		}
	}
}

void ZTexture::PrepareBitmap()
{
	bmpRgb = new uint8_t[width * height * 3];
	bmpAlpha = new uint8_t[width * height * 3];

	switch (type)
	{
	case TextureType::RGBA16bpp: PrepareBitmapRGBA16(); break;
	case TextureType::RGBA32bpp: PrepareBitmapRGBA32(); break;
	case TextureType::Grayscale4bpp: PrepareBitmapGrayscale4(); break;
	case TextureType::Grayscale8bpp: PrepareBitmapGrayscale8(); break;
	case TextureType::GrayscaleAlpha4bpp: PrepareBitmapGrayscaleAlpha4(); break;
	case TextureType::GrayscaleAlpha8bpp: PrepareBitmapGrayscaleAlpha8(); break;
	case TextureType::GrayscaleAlpha16bpp: PrepareBitmapGrayscaleAlpha16(); break;
	case TextureType::Palette4bpp: PrepareBitmapPalette4(); break;
	case TextureType::Palette8bpp: PrepareBitmapPalette8(); break;
	}
}

void ZTexture::PrepareBitmapRGBA16()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x) * 2;
			short data = (short)((rawData[pos + 1] << 8) + rawData[pos]);
			uint8_t r = (uint8_t)((data & 0xF800) >> 11);
			uint8_t g = (uint8_t)((data & 0x07C0) >> 6);
			uint8_t b = (uint8_t)((data & 0x003E) >> 1);
			uint8_t alpha = (uint8_t)(data & 0x01);
			bmpRgb[(((y * width) + x) * 3) + 0] = r * 8;
			bmpRgb[(((y * width) + x) * 3) + 1] = g * 8;
			bmpRgb[(((y * width) + x) * 3) + 2] = b * 8;
			
			bmpAlpha[(((y * width) + x) * 3) + 0] = alpha * 255;
			bmpAlpha[(((y * width) + x) * 3) + 1] = alpha * 255;
			bmpAlpha[(((y * width) + x) * 3) + 2] = alpha * 255;

			//Color c = Color.FromArgb(255, r * 8, g * 8, b * 8);
			//Color a = Color.FromArgb(255, alpha * 255, alpha * 255, alpha * 255);
			//bmpRgb.SetPixel(x, y, c);
			//bmpAlpha.SetPixel(x, y, a);
		}
	}
}

void ZTexture::PrepareBitmapRGBA32()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x) * 4;

			bmpRgb[(((y * width) + x) * 3) + 0] = rawData[pos + 2];
			bmpRgb[(((y * width) + x) * 3) + 1] = rawData[pos + 1];
			bmpRgb[(((y * width) + x) * 3) + 2] = rawData[pos + 0];

			bmpAlpha[(((y * width) + x) * 3) + 0] = rawData[pos + 3];
			bmpAlpha[(((y * width) + x) * 3) + 1] = rawData[pos + 3];
			bmpAlpha[(((y * width) + x) * 3) + 2] = rawData[pos + 3];

			
			
			//Color c = Color.FromArgb(255, rawData[pos + 2], rawData[pos + 1], rawData[pos + 0]);
			//Color a = Color.FromArgb(255, rawData[pos + 3], rawData[pos + 3], rawData[pos + 3]);
			//bmpRgb.SetPixel(x, y, c);
			//bmpAlpha.SetPixel(x, y, a);
		}
	}
}

void ZTexture::PrepareBitmapGrayscale8()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x) * 1;
			
			bmpRgb[(((y * width) + x) * 3) + 0] = rawData[pos];
			bmpRgb[(((y * width) + x) * 3) + 1] = rawData[pos];
			bmpRgb[(((y * width) + x) * 3) + 2] = rawData[pos];
			
			//Color c = Color.FromArgb(255, rawData[pos], rawData[pos], rawData[pos]);
			//bmpRgb.SetPixel(x, y, c);
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha8()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x) * 1;
			uint8_t grayscale = (uint8_t)(rawData[pos] & 0xF0);
			uint8_t alpha = (uint8_t)((rawData[pos] & 0x0F) << 4);
			
			bmpRgb[(((y * width) + x) * 3) + 0] = grayscale;
			bmpRgb[(((y * width) + x) * 3) + 1] = grayscale;
			bmpRgb[(((y * width) + x) * 3) + 2] = grayscale;

			bmpAlpha[(((y * width) + x) * 3) + 0] = alpha;
			bmpAlpha[(((y * width) + x) * 3) + 1] = alpha;
			bmpAlpha[(((y * width) + x) * 3) + 2] = alpha;
			
			//Color c = Color.FromArgb(255, grayscale, grayscale, grayscale);
			//Color a = Color.FromArgb(255, alpha, alpha, alpha);
			//bmpRgb.SetPixel(x, y, c);
			//bmpAlpha.SetPixel(x, y, a);
		}
	}
}

void ZTexture::PrepareBitmapGrayscale4()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x += 2)
		{
			for (int i = 0; i < 2; i++)
			{
				int pos = ((y * width) + x) / 2;
				uint8_t grayscale = 0;

				if (i == 0)
					grayscale = (uint8_t)(rawData[pos] & 0xF0);
				else
					grayscale = (uint8_t)((rawData[pos] & 0x0F) << 4);

				bmpRgb[(((y * width) + x + i) * 3) + 0] = grayscale;
				bmpRgb[(((y * width) + x + i) * 3) + 1] = grayscale;
				bmpRgb[(((y * width) + x + i) * 3) + 2] = grayscale;

				//Color c = Color.FromArgb(255, grayscale, grayscale, grayscale);
				//bmpRgb.SetPixel(x + i, y, c);
			}
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha4()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x += 2)
		{
			for (int i = 0; i < 2; i++)
			{
				int pos = ((y * width) + x) / 2;
				uint8_t data = 0;

				if (i == 0)
					data = (uint8_t)((rawData[pos] & 0xF0) >> 4);
				else
					data = (uint8_t)(rawData[pos] & 0x0F);

				uint8_t grayscale = (uint8_t)(((data & 0x0E) >> 1) * 32);
				uint8_t alpha = (uint8_t)((data & 0x01) * 255);

				bmpRgb[(((y * width) + x + i) * 3) + 0] = grayscale;
				bmpRgb[(((y * width) + x + i) * 3) + 1] = grayscale;
				bmpRgb[(((y * width) + x + i) * 3) + 2] = grayscale;

				bmpAlpha[(((y * width) + x + i) * 3) + 0] = alpha;
				bmpAlpha[(((y * width) + x + i) * 3) + 1] = alpha;
				bmpAlpha[(((y * width) + x + i) * 3) + 2] = alpha;

				//Color c = Color.FromArgb(255, grayscale, grayscale, grayscale);
				//Color a = Color.FromArgb(255, alpha, alpha, alpha);
				//bmpRgb.SetPixel(x + i, y, c);
				//bmpAlpha.SetPixel(x + i, y, a);
			}
		}
	}
}

void ZTexture::PrepareBitmapGrayscaleAlpha16()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x) * 2;
			uint8_t grayscale = rawData[pos];
			uint8_t alpha = rawData[pos + 1];

			bmpRgb[(((y * width) + x) * 3) + 0] = grayscale;
			bmpRgb[(((y * width) + x) * 3) + 1] = grayscale;
			bmpRgb[(((y * width) + x) * 3) + 2] = grayscale;

			bmpAlpha[(((y * width) + x) * 3) + 0] = alpha;
			bmpAlpha[(((y * width) + x) * 3) + 1] = alpha;
			bmpAlpha[(((y * width) + x) * 3) + 2] = alpha;
			
			
			//Color c = Color.FromArgb(255, grayscale, grayscale, grayscale);
			//Color a = Color.FromArgb(255, alpha, alpha, alpha);
			//bmpRgb.SetPixel(x, y, c);
			//bmpAlpha.SetPixel(x, y, a);
		}
	}
}

void ZTexture::PrepareBitmapPalette4()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x += 2)
		{
			for (int i = 0; i < 2; i++)
			{
				int pos = ((y * width) + x) / 2;
				uint8_t paletteIndex = 0;

				if (i == 0)
					paletteIndex = (uint8_t)((rawData[pos] & 0xF0) >> 4);
				else
					paletteIndex = (uint8_t)((rawData[pos] & 0x0F));

				bmpRgb[(((y * width) + x) * 3) + 0] = paletteIndex * 16;
				bmpRgb[(((y * width) + x) * 3) + 1] = paletteIndex * 16;
				bmpRgb[(((y * width) + x) * 3) + 2] = paletteIndex * 16;

				//Color c = Color.FromArgb(255, paletteIndex * 16, paletteIndex * 16, paletteIndex * 16);
				//bmpRgb.SetPixel(x + i, y, c);
			}
		}
	}
}

void ZTexture::PrepareBitmapPalette8()
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x) * 1;
			
			bmpRgb[(((y * width) + x) * 3) + 0] = rawData[pos];
			bmpRgb[(((y * width) + x) * 3) + 1] = rawData[pos];
			bmpRgb[(((y * width) + x) * 3) + 2] = rawData[pos];
			
			//Color c = Color.FromArgb(255, rawData[pos], rawData[pos], rawData[pos]);
			//bmpRgb.SetPixel(x, y, c);
		}
	}
}

void ZTexture::PrepareRawData(string inFolder)
{
	rawData = vector<uint8_t>(GetRawDataSize());

	switch (type)
	{
	case TextureType::RGBA16bpp: PrepareRawDataRGBA16(inFolder); break;
	case TextureType::RGBA32bpp: PrepareRawDataRGBA32(inFolder); break;
	case TextureType::Grayscale8bpp: PrepareRawDataGrayscale8(inFolder); break;
	case TextureType::GrayscaleAlpha8bpp: PrepareRawDataGrayscaleAlpha8(inFolder); break;
	case TextureType::Grayscale4bpp: PrepareRawDataGrayscale4(inFolder); break;
	case TextureType::GrayscaleAlpha4bpp: PrepareRawDataGrayscaleAlpha4(inFolder); break;
	case TextureType::GrayscaleAlpha16bpp: PrepareRawDataGrayscaleAlpha16(inFolder); break;
	case TextureType::Palette4bpp: PrepareRawDataPalette4(inFolder); break;
	case TextureType::Palette8bpp: PrepareRawDataPalette8(inFolder); break;
	default:
		throw "Build Mode: Format is not supported!";
	}
}

void ZTexture::PrepareRawDataRGBA32(string inFolder)
{
	int width;
	int height;
	int comp;

	bmpRgb = (uint8_t*)stbi_load((inFolder + "/" + name + ".rgb.png").c_str(), &width, &height, &comp, STBI_rgb);
	bmpAlpha = (uint8_t*)stbi_load((inFolder + "/" + name + ".a.png").c_str(), &width, &height, &comp, STBI_rgb);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x) * 4;
			//Color c = bmpRgb.GetPixel(x, y);
			//Color a = bmpAlpha.GetPixel(x, y);

			rawData[pos + 0] = bmpRgb[(((y * width) + x) * 3) + 0];
			rawData[pos + 1] = bmpRgb[(((y * width) + x) * 3) + 1];
			rawData[pos + 2] = bmpRgb[(((y * width) + x) * 3) + 2];
			rawData[pos + 3] = bmpAlpha[(((y * width) + x) * 3) + 0];
		}
	}
}

void ZTexture::PrepareRawDataRGBA16(string inFolder)
{
	int width;
	int height;
	int comp;

	bmpRgb = (uint8_t*)stbi_load((inFolder + "/" + name + ".rgb.png").c_str(), &width, &height, &comp, STBI_rgb);
	bmpAlpha = (uint8_t*)stbi_load((inFolder + "/" + name + ".a.png").c_str(), &width, &height, &comp, STBI_rgb);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x) * 2;

			uint8_t r = (uint8_t)(bmpRgb[(((y * width) + x) * 3) + 0] / 8);
			uint8_t g = (uint8_t)(bmpRgb[(((y * width) + x) * 3) + 1] / 8);
			uint8_t b = (uint8_t)(bmpRgb[(((y * width) + x) * 3) + 2] / 8);

			uint8_t alphaBit = (bmpAlpha[(((y * width) + x) * 3) + 0] != 0);

			short data = (short)((r << 11) + (g << 6) + (b << 1) + alphaBit);

			rawData[pos + 0] = (uint8_t)((data & 0xFF00) >> 8);
			rawData[pos + 1] = (uint8_t)((data & 0x00FF));
		}
	}
}

void ZTexture::PrepareRawDataGrayscale8(string inFolder)
{
	int width;
	int height;
	int comp;

	bmpRgb = (uint8_t*)stbi_load((inFolder + "/" + name + ".gray.png").c_str(), &width, &height, &comp, STBI_rgb);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x);

			rawData[pos] = bmpRgb[(((y * width) + x) * 3) + 0];
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha8(string inFolder)
{
	int width;
	int height;
	int comp;

	bmpRgb = (uint8_t*)stbi_load((inFolder + "/" + name + ".gray.png").c_str(), &width, &height, &comp, STBI_rgb);
	bmpAlpha = (uint8_t*)stbi_load((inFolder + "/" + name + ".a.png").c_str(), &width, &height, &comp, STBI_rgb);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x) * 1;

			uint8_t r = (uint8_t)(bmpRgb[(((y * width) + x) * 3) + 0]);
			uint8_t a = (uint8_t)(bmpAlpha[(((y * width) + x) * 3) + 0]);

			rawData[pos] = (uint8_t)(((r / 16) << 4) + (a / 16));
		}
	}
}

void ZTexture::PrepareRawDataGrayscale4(string inFolder)
{
	int width;
	int height;
	int comp;

	bmpRgb = (uint8_t*)stbi_load((inFolder + "/" + name + ".gray.png").c_str(), &width, &height, &comp, STBI_rgb);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x += 2)
		{
			int pos = ((y * width) + x) / 2;
			uint8_t r1 = (uint8_t)(bmpRgb[(((y * width) + x) * 3) + 0]);
			uint8_t r2 = (uint8_t)(bmpRgb[(((y * width) + x + 1) * 3) + 0]);

			rawData[pos] = (uint8_t)(((r1 / 16) << 4) + (r2 / 16));
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha4(string inFolder)
{
	int width;
	int height;
	int comp;

	bmpRgb = (uint8_t*)stbi_load((inFolder + "/" + name + ".gray.png").c_str(), &width, &height, &comp, STBI_rgb);
	bmpAlpha = (uint8_t*)stbi_load((inFolder + "/" + name + ".a.png").c_str(), &width, &height, &comp, STBI_rgb);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x += 2)
		{
			int pos = ((y * width) + x) / 2;
			uint8_t data = 0;

			for (int i = 0; i < 2; i++)
			{
				uint8_t cR = bmpRgb[(((y * width) + x + i) * 3) + 0];

				uint8_t alphaBit = (bmpAlpha[(((y * width) + x + i) * 3) + 0] != 0);

				if (i == 0)
					data += (uint8_t)((((cR / 32) << 1) + alphaBit) << 4);
				else
					data += (uint8_t)(((cR / 32) << 1) + alphaBit);
			}

			rawData[pos] = data;
		}
	}
}

void ZTexture::PrepareRawDataGrayscaleAlpha16(string inFolder)
{
	int width;
	int height;
	int comp;

	bmpRgb = (uint8_t*)stbi_load((inFolder + "/" + name + ".gray.png").c_str(), &width, &height, &comp, STBI_rgb);
	bmpAlpha = (uint8_t*)stbi_load((inFolder + "/" + name + ".a.png").c_str(), &width, &height, &comp, STBI_rgb);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x) * 2;

			uint8_t cR = bmpRgb[(((y * width) + x) * 3) + 0];
			uint8_t aR = bmpAlpha[(((y * width) + x) * 3) + 0];

			rawData[pos] = (uint8_t)(cR);
			rawData[pos + 1] = (uint8_t)(aR);
		}
	}
}

void ZTexture::PrepareRawDataPalette4(string inFolder)
{
	int width;
	int height;
	int comp;

	bmpRgb = (uint8_t*)stbi_load((inFolder + "/" + name + ".ci4.png").c_str(), &width, &height, &comp, STBI_rgb);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x += 2)
		{
			int pos = ((y * width) + x) / 2;

			uint8_t cR1 = bmpRgb[(((y * width) + x) * 3) + 0];
			uint8_t cR2 = bmpRgb[(((y * width) + x + 1) * 3) + 0];

			rawData[pos] = (uint8_t)(((cR1 / 16) << 4) + (cR2 / 16));
		}
	}
}

void ZTexture::PrepareRawDataPalette8(string inFolder)
{
	int width;
	int height;
	int comp;

	bmpRgb = (uint8_t*)stbi_load((inFolder + "/" + name + ".ci8.png").c_str(), &width, &height, &comp, STBI_rgb);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y * width) + x);

			uint8_t cR = bmpRgb[(((y * width) + x) * 3) + 0];

			rawData[pos] = cR;
		}
	}
}

float ZTexture::GetPixelMultiplyer()
{
	switch (type)
	{
		case TextureType::Grayscale4bpp: case TextureType::GrayscaleAlpha4bpp: case TextureType::Palette4bpp: return 0.5f;
		case TextureType::Grayscale8bpp: case TextureType::GrayscaleAlpha8bpp: case TextureType::Palette8bpp: return 1;
		case TextureType::GrayscaleAlpha16bpp: case TextureType::RGBA16bpp: return 2;
		case TextureType::RGBA32bpp: return 4;
	}

	return -1;
}

vector<uint8_t> ZTexture::GetRawData()
{
	return rawData;
}

int ZTexture::GetRawDataSize()
{
	return (int)(width * height * GetPixelMultiplyer());
}

void ZTexture::Save(string outFolder)
{
	if (type == TextureType::RGBA32bpp || type == TextureType::RGBA16bpp)
	{
		stbi_write_png((outFolder + "/" + name + ".rgb.png").c_str(), width, height, 24, bmpRgb, 1);
		stbi_write_png((outFolder + "/" + name + ".a.png").c_str(), width, height, 24, bmpAlpha, 1);
	}
	/*else if (type == TextureType::Grayscale8bpp || type == TextureType::Grayscale4bpp)
	{
		bmpRgb.Save(outFolder + "/" + name + ".gray.png");
	}
	else if (type == TextureType::GrayscaleAlpha8bpp || type == TextureType::GrayscaleAlpha4bpp)
	{
		bmpRgb.Save(outFolder + "/" + name + ".gray.png");
		bmpAlpha.Save(outFolder + "/" + name + ".a.png");
	}
	else if (type == TextureType::Palette4bpp)
	{
		bmpRgb.Save(outFolder + "/" + name + ".ci4.png");
	}
	else if (type == TextureType::Palette8bpp)
	{
		bmpRgb.Save(outFolder + "/" + name + ".ci8.png");
	}*/
}