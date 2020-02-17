#pragma once

#include "ZResource.h"
#include "ZTexture.h"
#include "ZRoom/ZRoom.h"
#include "tinyxml2.h"

#include <vector>
#include <map>
#include <string>

enum F3DZEXOpcode
{
	G_NOOP = 0x00,
	G_VTX = 0x01,
	G_MODIFYVTX = 0x02,
	G_CULLDL = 0x03,
	G_BRANCH_Z = 0x04,
	G_TRI1 = 0x05,
	G_TRI2 = 0x06,
	G_QUAD = 0x07,
	G_SPECIAL_3 = 0xD3,
	G_SPECIAL_2 = 0xD4,
	G_SPECIAL_1 = 0xD5,
	G_DMA_IO = 0xD6,
	G_TEXTURE = 0xD7,
	G_POPMTX = 0xD8,
	G_GEOMETRYMODE = 0xD9,
	G_MTX = 0xDA,
	G_MOVEWORD = 0xDB,
	G_MOVEMEM = 0xDC,
	G_LOAD_UCODE = 0xDD,
	G_DL = 0xDE,
	G_ENDDL = 0xDF,
	G_SPNOOP = 0xE0,
	G_RDPHALF_1 = 0xE1,
	G_SETOTHERMODE_L = 0xE2,
	G_SETOTHERMODE_H = 0xE3,
	G_TEXRECT = 0xE4,
	G_TEXRECTFLIP = 0xE5,
	G_RDPLOADSYNC = 0xE6,
	G_RDPPIPESYNC = 0xE7,
	G_RDPTILESYNC = 0xE8,
	G_RDPFULLSYNC = 0xE9,
	G_SETKEYGB = 0xEA,
	G_SETKEYR = 0xEB,
	G_SETCONVERT = 0xEC,
	G_SETSCISSOR = 0xED,
	G_SETPRIMDEPTH = 0xEE,
	G_RDPSETOTHERMODE = 0xEF,
	G_LOADTLUT = 0xF0,
	G_RDPHALF_2 = 0xF1,
	G_SETTILESIZE = 0xF2,
	G_LOADBLOCK = 0xF3,
	G_LOADTILE = 0xF4,
	G_SETTILE = 0xF5,
	G_FILLRECT = 0xF6,
	G_SETFILLCOLOR = 0xF7,
	G_SETFOGCOLOR = 0xF8,
	G_SETBLENDCOLOR = 0xF9,
	G_SETPRIMCOLOR = 0xFA,
	G_SETENVCOLOR = 0xFB,
	G_SETCOMBINE = 0xFC,
	G_SETTIMG = 0xFD,
	G_SETZIMG = 0xFE,
	G_SETCIMG = 0xFF
};

enum F3DZEXTexFormats
{
	G_IM_FMT_RGBA,
	G_IM_FMT_YUV,
	G_IM_FMT_CI,
	G_IM_FMT_IA,
	G_IM_FMT_I
};

enum F3DZEXTexSizes
{
	G_IM_SIZ_4b,
	G_IM_SIZ_8b,
	G_IM_SIZ_16b,
	G_IM_SIZ_32b
};

enum Segments
{
	DirectReference = 0,
	TitleStatic = 1,
	Scene = 2,
	Room = 3,
	GameplayKeep = 4,
	FieldDungeonKeep = 5,
	Object = 6,
	LinkAnimation = 7,
	IconItemStatic = 8,
	IconItem24Static = 9,
	Unknown10 = 10,
	Unknown11 = 11,
	Unknown12 = 12,
	IconFieldDungeonStatic = 13,
	IconItemLanguageStatic = 14,
	ZBuffer = 15,
	FrameBuffer = 16,
};


class Vertex
{
public:
	int16_t x, y, z;
	uint16_t flag;
	int16_t s, t;
	uint8_t r, g, b, a;
	
	Vertex(std::vector<uint8_t> rawData, int rawDataIndex);
};

class ZDisplayList : public ZResource
{
protected:
	std::vector<uint64_t> instructions;

	// Test
	uint32_t lastTexWidth, lastTexHeight, lastTexAddr, lastTexSeg;
	F3DZEXTexFormats lastTexFmt;
	F3DZEXTexSizes lastTexSiz, lastTexSizTest, lastCISiz;
	bool lastTexLoaded;

	//void ParseXML(tinyxml2::XMLElement* reader);
	static TextureType TexFormatToTexType(F3DZEXTexFormats fmt, F3DZEXTexSizes siz);
	void ParseRawData();
public:
	ZDisplayList(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath);
	ZDisplayList(std::vector<uint8_t> nRawData, int rawDataIndex, int rawDataSize);

	void TextureGenCheck(std::string prefix);
	static bool TextureGenCheck(std::vector<uint8_t> fileData, std::map<uint32_t, ZTexture*>& textures, ZRoom* scene, std::string prefix, uint32_t texWidth, uint32_t texHeight, uint32_t texAddr, uint32_t texSeg, F3DZEXTexFormats texFmt, F3DZEXTexSizes texSiz, bool texLoaded);
	static int GetDListLength(std::vector<uint8_t> rawData, int rawDataIndex);

	std::string sceneSegName;
	ZRoom* scene;

	int dListAddress;

	std::map<uint32_t, std::vector<Vertex*>> vertices;
	std::map<uint32_t, std::string> vtxDeclarations;
	std::vector<ZDisplayList*> otherDLists;
	
	std::map<uint32_t, ZTexture*> textures;
	std::map<uint32_t, std::string> texDeclarations;
	
	std::string defines; // Hack for special cases where vertex arrays intersect...

	std::vector<uint8_t> fileData;
	std::vector<uint8_t> GetRawData();
	int GetRawDataSize();
	std::string GetSourceOutputHeader(std::string prefix);
	std::string GetSourceOutputCode(std::string prefix);
	void Save(std::string outFolder);
};