#include "ImageBackend.h"

#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <png.h>

#include "StringHelper.h"

/* ImageBackend */

ImageBackend::~ImageBackend()
{
	FreeImageData();
}

void ImageBackend::ReadPng(const char* filename)
{
	FreeImageData();

	FILE *fp = fopen(filename, "rb");
	if (fp == nullptr)
		throw std::runtime_error(StringHelper::Sprintf("ImageBackend::ReadPng: Error.\n\t Couldn't open file '%s'.", filename));

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png)
		throw std::runtime_error("ImageBackend::ReadPng: Error.\n\t Couldn't create png struct.");

	png_infop info = png_create_info_struct(png);
	if(!info)
		throw std::runtime_error("ImageBackend::ReadPng: Error.\n\t Couldn't create png info.");

	if(setjmp(png_jmpbuf(png)))
		throw std::runtime_error("ImageBackend::ReadPng: Error.\n\t setjmp(png_jmpbuf(png)).");

	png_init_io(png, fp);

	png_read_info(png, info);

	width     = png_get_image_width(png, info);
	height    = png_get_image_height(png, info);
	colorType = png_get_color_type(png, info);
	bitDepth  = png_get_bit_depth(png, info);

	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if(bitDepth == 16)
		png_set_strip_16(png);

	if(colorType == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	/*if(*color_type == PNG_COLOR_TYPE_RGB ||
		*color_type == PNG_COLOR_TYPE_GRAY ||
		*color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);*/

	if(colorType == PNG_COLOR_TYPE_GRAY ||
		colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	pixelMatrix = (uint8_t**)malloc(sizeof(uint8_t*) * height);
	for(size_t y = 0; y < height; y++) {
		pixelMatrix[y] = (uint8_t*)malloc(png_get_rowbytes(png,info));
	}

	png_read_image(png, pixelMatrix);

	fclose(fp);

	png_destroy_read_struct(&png, &info, NULL);

    hasImageData = true;
}


void ImageBackend::WritePng(const char *filename) {
    assert(hasImageData);

	FILE *fp = fopen(filename, "wb");
	if(!fp)
		throw std::runtime_error(StringHelper::Sprintf("ImageBackend::WritePng: Error.\n\t Couldn't open file '%s' in write mode.", filename));

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png)
		throw std::runtime_error("ImageBackend::WritePng: Error.\n\t Couldn't create png struct.");

	png_infop info = png_create_info_struct(png);
	if (!info)
		throw std::runtime_error("ImageBackend::WritePng: Error.\n\t Couldn't create png info.");

	if (setjmp(png_jmpbuf(png)))
		throw std::runtime_error("ImageBackend::WritePng: Error.\n\t setjmp(png_jmpbuf(png)).");

	png_init_io(png, fp);

	png_set_IHDR(
		png,
		info,
		width, height,
		bitDepth, // 8,
		colorType, //PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	//png_set_filler(png, 0, PNG_FILLER_AFTER);

	png_write_image(png, pixelMatrix);
	png_write_end(png, NULL);

	fclose(fp);

	png_destroy_write_struct(&png, &info);
}

void ImageBackend::SetTextureData(const std::vector<std::vector<RGBAPixel>>& texData, uint32_t nWidth, uint32_t nHeight, uint8_t nColorType, uint8_t nBitDepth)
{
	FreeImageData();

    width = nWidth;
    height = nHeight;
	colorType = nColorType;
	bitDepth = nBitDepth;

	size_t bytePerPixel = GetBytesPerPixel();

	pixelMatrix = (uint8_t**)malloc(sizeof(uint8_t*) * height);
	for(size_t y = 0; y < height; y++) {
		pixelMatrix[y] = (uint8_t*)malloc(sizeof(uint8_t*) * width * bytePerPixel);
        for (size_t x = 0; x < width; x++)
        {
            pixelMatrix[y][x*bytePerPixel + 0] = texData.at(y).at(x).r;
            pixelMatrix[y][x*bytePerPixel + 1] = texData.at(y).at(x).g;
            pixelMatrix[y][x*bytePerPixel + 2] = texData.at(y).at(x).b;

			if (colorType == PNG_COLOR_TYPE_RGBA)
            	pixelMatrix[y][x*bytePerPixel + 3] = texData.at(y).at(x).a;
        }
	}
    hasImageData = true;
}

void ImageBackend::InitEmptyImage(uint32_t nWidth, uint32_t nHeight, uint8_t nColorType, uint8_t nBitDepth)
{
	FreeImageData();

    width = nWidth;
    height = nHeight;
	colorType = nColorType;
	bitDepth = nBitDepth;

	size_t bytePerPixel = GetBytesPerPixel();

	pixelMatrix = (uint8_t**)malloc(sizeof(uint8_t*) * height);
	for(size_t y = 0; y < height; y++) {
		pixelMatrix[y] = (uint8_t*)calloc(width * bytePerPixel, sizeof(uint8_t*));
	}
    hasImageData = true;
}

RGBAPixel ImageBackend::GetPixel(size_t y, size_t x) const
{
    assert(y < height);
    assert(x < width);

    RGBAPixel pixel;
	size_t bytePerPixel = GetBytesPerPixel();
    pixel.r = pixelMatrix[y][x*bytePerPixel + 0];
    pixel.g = pixelMatrix[y][x*bytePerPixel + 1];
    pixel.b = pixelMatrix[y][x*bytePerPixel + 2];
	if (colorType == PNG_COLOR_TYPE_RGBA)
    	pixel.a = pixelMatrix[y][x*bytePerPixel + 3];
    return pixel;
}

void ImageBackend::SetRGBPixel(size_t y, size_t x, uint8_t nR, uint8_t nG, uint8_t nB, uint8_t nA)
{
    assert(y < height);
    assert(x < width);

	size_t bytePerPixel = GetBytesPerPixel();
    pixelMatrix[y][x*bytePerPixel + 0] = nR;
    pixelMatrix[y][x*bytePerPixel + 1] = nG;
    pixelMatrix[y][x*bytePerPixel + 2] = nB;
	if (colorType == PNG_COLOR_TYPE_RGBA)
    	pixelMatrix[y][x*bytePerPixel + 3] = nA;
}

void ImageBackend::SetGrayscalePixel(size_t y, size_t x, uint8_t grayscale, uint8_t alpha)
{
    assert(y < height);
    assert(x < width);

	size_t bytePerPixel = GetBytesPerPixel();
    pixelMatrix[y][x*bytePerPixel + 0] = grayscale;
    pixelMatrix[y][x*bytePerPixel + 1] = grayscale;
    pixelMatrix[y][x*bytePerPixel + 2] = grayscale;
	if (colorType == PNG_COLOR_TYPE_RGBA)
    	pixelMatrix[y][x*bytePerPixel + 3] = alpha;
}

uint32_t ImageBackend::GetWidth() const
{
    return width;
}

uint32_t ImageBackend::GetHeight() const
{
    return height;
}

uint8_t ImageBackend::GetColorType() const
{
    return colorType;
}

uint8_t ImageBackend::GetBitDepth() const
{
    return bitDepth;
}

double ImageBackend::GetBytesPerPixel() const
{
	switch (colorType)
	{
	case PNG_COLOR_TYPE_RGBA:
		return 4 * bitDepth / 8;

	case PNG_COLOR_TYPE_RGB:
		return 3 * bitDepth / 8;

	default:
		throw std::invalid_argument("Invalid color type.");
	}
}

void ImageBackend::FreeImageData()
{
    if (hasImageData)
    {
        for(size_t y = 0; y < height; y++)
            free(pixelMatrix[y]);
        free(pixelMatrix);
    }
}

/* RGBAPixel */

void RGBAPixel::SetRGBA(uint8_t nR, uint8_t nG, uint8_t nB, uint8_t nA)
{
    r = nR;
    g = nG;
    b = nB;
    a = nA;
}

void RGBAPixel::SetGrayscale(uint8_t grayscale, uint8_t alpha)
{
    r = grayscale;
    g = grayscale;
    b = grayscale;
    a = alpha;
}

