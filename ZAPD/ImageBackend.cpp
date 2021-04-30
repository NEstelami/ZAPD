#include "ImageBackend.h"

#include <cassert>
#include <cstdlib>
#include <png.h>

ImageBackend::~ImageBackend()
{
    if (hasImageData)
    {
        for(size_t y = 0; y < height; y++)
            free(pixelMatrix[y]);
        free(pixelMatrix);
    }
}

void ImageBackend::ReadPng(const char* filename) {
    assert(!hasImageData);
	FILE *fp = fopen(filename, "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    // TODO: Change to exceptions
	if(!png) abort();

	png_infop info = png_create_info_struct(png);
	if(!info) abort();

	if(setjmp(png_jmpbuf(png))) abort();

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

	//int y;

	FILE *fp = fopen(filename, "wb");
	if(!fp) abort();

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	// Output is 8bit depth, RGBA format.
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

	//if (!row_pointers) abort();

	png_write_image(png, pixelMatrix);
	png_write_end(png, NULL);

	// TODO!
	/*for(int y = 0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);*/

	fclose(fp);

	png_destroy_write_struct(&png, &info);
}

void ImageBackend::SetTextureData(const std::vector<std::vector<RGBAPixel>>& texData, uint32_t nWidth, uint32_t nHeight)
{
    assert(!hasImageData);

    width = nWidth;
    height = nHeight;

	pixelMatrix = (uint8_t**)malloc(sizeof(uint8_t*) * height);
	for(size_t y = 0; y < height; y++) {
		pixelMatrix[y] = (uint8_t*)malloc(sizeof(uint8_t*) * width * 4);
        for (size_t x = 0; x < width; x++)
        {
            pixelMatrix[y][x*4 + 0] = texData.at(y).at(x).r;
            pixelMatrix[y][x*4 + 1] = texData.at(y).at(x).g;
            pixelMatrix[y][x*4 + 2] = texData.at(y).at(x).b;
            pixelMatrix[y][x*4 + 3] = texData.at(y).at(x).a;
        }
	}
    hasImageData = true;

    colorType = PNG_COLOR_TYPE_RGBA;
    bitDepth = 8;
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

RGBAPixel ImageBackend::GetPixel(size_t y, size_t x) const
{
    assert(y < height);
    assert(x < width);
    RGBAPixel pixel;
    pixel.r = pixelMatrix[y][x*4 + 0];
    pixel.g = pixelMatrix[y][x*4 + 1];
    pixel.b = pixelMatrix[y][x*4 + 2];
    pixel.a = pixelMatrix[y][x*4 + 3];
    return pixel;
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

