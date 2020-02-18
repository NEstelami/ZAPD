#include "ZDisplayList.h"
#include "BitConverter.h"
#include <algorithm>

using namespace std;
using namespace tinyxml2;

// EXTRACT MODE
ZDisplayList::ZDisplayList(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath)
{
	relativePath = nRelPath;
	rawData = vector<uint8_t>(rawData.data() + rawDataIndex, rawData.data() + rawDataIndex + GetRawDataSize());
	defines = "";
	sceneSegName = "";
	lastTexWidth = 0;
	lastTexHeight = 0;
	lastTexAddr = 0;
	lastTexFmt = F3DZEXTexFormats::G_IM_FMT_RGBA;
	lastTexSiz = F3DZEXTexSizes::G_IM_SIZ_16b;
	lastTexSizTest = F3DZEXTexSizes::G_IM_SIZ_16b;
	lastTexLoaded = false;
	ParseRawData();
}

ZDisplayList::ZDisplayList(vector<uint8_t> nRawData, int rawDataIndex, int rawDataSize)
{
	fileData = nRawData;
	dListAddress = rawDataIndex;
	rawData = vector<uint8_t>(nRawData.data() + rawDataIndex, nRawData.data() + rawDataIndex + rawDataSize);
	defines = "";
	sceneSegName = "";
	lastTexWidth = 0;
	lastTexHeight = 0;
	lastTexAddr = 0;
	lastTexFmt = F3DZEXTexFormats::G_IM_FMT_RGBA;
	lastTexSiz = F3DZEXTexSizes::G_IM_SIZ_16b;
	lastTexLoaded = false;
	ParseRawData();
}

void ZDisplayList::ParseRawData()
{
	int numInstructions = rawData.size() / 8;

	for (int i = 0; i < numInstructions; i++)
	{
		uint64_t data = BitConverter::ToInt64BE(rawData, (i * 8));
		instructions.push_back(data);
	}
}

int ZDisplayList::GetDListLength(vector<uint8_t> rawData, int rawDataIndex)
{
	int i = 0;

	while (true)
	{
		F3DZEXOpcode opcode = (F3DZEXOpcode)rawData[rawDataIndex + (i * 8)];

		i++;

		if (opcode == F3DZEXOpcode::G_ENDDL)
			return i * 8;
	}
}

string ZDisplayList::GetSourceOutputHeader(string prefix)
{
	char line[4096];
	string sourceOutput = "";

	sprintf(line, "extern Gfx _%s_dlist_%08X[];\n", prefix.c_str(), dListAddress);
	sourceOutput += line;

	return "";
}

string ZDisplayList::GetSourceOutputCode(std::string prefix)
{
	char line[4096];
	string sourceOutput = "";

	sprintf(line, "Gfx _%s_dlist_%08X[] =\n{\n", prefix.c_str(), dListAddress);
	sourceOutput += line;

	for (int i = 0; i < instructions.size(); i++) 
	{
		F3DZEXOpcode opcode = (F3DZEXOpcode)rawData[(i * 8) + 0];
		uint64_t data = instructions[i];

		sourceOutput += "\t";

		switch (opcode)
		{
		case F3DZEXOpcode::G_NOOP:
			sprintf(line, "gsDPNoOpTag(0x%08X),", data & 0xFFFFFFFF);
			break;
		case F3DZEXOpcode::G_DL:
		{
			int pp = (data & 0x00FF000000000000) >> 56;

			if (pp != 0)
				sprintf(line, "gsSPBranchList(_%s_dlist_%08X),", prefix.c_str(), data & 0x00FFFFFF);
			else
				sprintf(line, "gsSPDisplayList(_%s_dlist_%08X),", prefix.c_str(), data & 0x00FFFFFF);

			int segmentNumber = (data & 0xFF000000) >> 24;

			if (segmentNumber == 8 || segmentNumber == 9 || segmentNumber == 10) // Used for runtime-generated display lists
			{
				if (pp != 0)
					sprintf(line, "gsSPBranchList(0x%08X),", data & 0xFFFFFFFF);
				else
					sprintf(line, "gsSPDisplayList(0x%08X),", data & 0xFFFFFFFF);
			}
			else
			{
				ZDisplayList* nList = new ZDisplayList(fileData, data & 0x00FFFFFF, GetDListLength(fileData, data & 0x00FFFFFF));
				nList->scene = scene;
				otherDLists.push_back(nList);
			}
		}
			break;
		case F3DZEXOpcode::G_CULLDL:
		{
			int vvvv = (data & 0xFFFF00000000) >> 32;
			int wwww = (data & 0x0000FFFF);

			sprintf(line, "gsSPCullDisplayList(%i, %i),", vvvv / 2, wwww / 2);
		}
		break;
		case F3DZEXOpcode::G_TRI1:
		{
			int aa = ((data & 0x00FF000000000000ULL) >> 48) / 2;
			int bb = ((data & 0x0000FF0000000000ULL) >> 40) / 2;
			int cc = ((data & 0x000000FF00000000ULL) >> 32) / 2;
			sprintf(line, "gsSP1Triangle(%i, %i, %i, 0),", aa, bb, cc);
		}
			break;
		case F3DZEXOpcode::G_TRI2:
		{
			int aa = ((data & 0x00FF000000000000ULL) >> 48) / 2;
			int bb = ((data & 0x0000FF0000000000ULL) >> 40) / 2;
			int cc = ((data & 0x000000FF00000000ULL) >> 32) / 2;
			int dd = ((data & 0x00000000FF0000ULL) >> 16) / 2;
			int ee = ((data & 0x0000000000FF00ULL) >> 8) / 2;
			int ff = ((data & 0x000000000000FFULL) >> 0) / 2;
			sprintf(line, "gsSP2Triangles(%i, %i, %i, 0, %i, %i, %i, 0),", aa, bb, cc, dd, ee, ff);
		}
		break;
		case F3DZEXOpcode::G_VTX:
		{
			int nn = (data & 0x000FF00000000000ULL) >> 44;
			int aa = (data & 0x000000FF00000000ULL) >> 32;
			sprintf(line, "gsSPVertex(_%s_vertices_%08X, %i, %i),", prefix.c_str(), data & 0x00FFFFFF, nn, ((aa >> 1) - nn));
			
			{
				uint32_t currentPtr = data & 0x00FFFFFF;

				vector<Vertex*> vtxList = vector<Vertex*>();

				for (int i = 0; i < nn; i++)
				{
					Vertex* vtx = new Vertex(fileData, currentPtr);
					vtxList.push_back(vtx);
					
					currentPtr += 16;
				}

				vertices[data & 0x00FFFFFF] = vtxList;
			}

		}
			break;
		case F3DZEXOpcode::G_SETTIMG:
		{
			int __ = (data & 0x00FF000000000000) >> 48;
			int www = (data & 0x00000FFF00000000) >> 32;
			string fmtTbl[] = { "G_IM_FMT_RGBA", "G_IM_FMT_YUV", "G_IM_FMT_CI", "G_IM_FMT_IA", "G_IM_FMT_I" };
			string sizTbl[] = { "G_IM_SIZ_4b", "G_IM_SIZ_8b", "G_IM_SIZ_16b", "G_IM_SIZ_32b" };

			uint32_t fmt = (__ & 0xE0) >> 5;
			uint32_t siz = (__ & 0x18) >> 3;

			TextureGenCheck(prefix);

			lastTexFmt = (F3DZEXTexFormats)fmt;
			lastTexSiz = (F3DZEXTexSizes)siz;
			lastTexSeg = data;
			lastTexAddr = data & 0x00FFFFFF;

			int segmentNumber = (data >> 24) & 0xFF;

			if (segmentNumber != 2)
			{
				char texStr[2048];

				int32_t texAddress = data & 0x00FFFFFF;

				if (texAddress != 0)
					sprintf(texStr, "_%s_tex_%08X", prefix.c_str(), texAddress);
				else if (segmentNumber != 3)
					sprintf(texStr, "0x%08X", data);
				else
					sprintf(texStr, "0");

				sprintf(line, "gsDPSetTextureImage(%s, %s, %i, %s),", fmtTbl[fmt].c_str(), sizTbl[siz].c_str(), www + 1, texStr);
			}
			else
			{
				//sprintf(line, "gsDPSetTextureImage(%s, %s, %i, 0x%08X),", fmtTbl[fmt].c_str(), sizTbl[siz].c_str(), www + 1, data & 0xFFFFFFFF);
				sprintf(line, "gsDPSetTextureImage(%s, %s, %i, _%s_tex_%08X),", fmtTbl[fmt].c_str(), sizTbl[siz].c_str(), www + 1, scene->GetName().c_str(), data & 0x00FFFFFF);
			}
		}
			break;
		case F3DZEXOpcode::G_GEOMETRYMODE:
		{
			int cccccc = (data & 0x00FFFFFF00000000) >> 32;
			int ssssssss = (data & 0xFFFFFFFF);
			sprintf(line, "gsSPGeometryMode(0x%08X, 0x%08X),", ~cccccc, ssssssss);
		}
		break;
		case F3DZEXOpcode::G_SETPRIMCOLOR:
		{
			int mm = (data & 0x0000FF0000000000) >> 40;
			int ff = (data & 0x000000FF00000000) >> 32;
			int rr = (data & 0x00000000FF000000) >> 24;
			int gg = (data & 0x0000000000FF0000) >> 16;
			int bb = (data & 0x000000000000FF00) >> 8;
			int aa = (data & 0x00000000000000FF) >> 0;
			sprintf(line, "gsDPSetPrimColor(%i, %i, %i, %i, %i, %i),", mm, ff, rr, gg, bb, aa);
		}
		break;
		case F3DZEXOpcode::G_SETOTHERMODE_L:
		{
			int ss = (data & 0x0000FF0000000000) >> 40;
			int nn = (data & 0x000000FF00000000) >> 32;
			int dd = (data & 0xFFFFFFFF);

			sprintf(line, "gsSPSetOtherMode(0xE2, %i, %i, 0x%08X),", 32 - (nn + 1) - ss, nn + 1, dd);
		}
			break;
		case F3DZEXOpcode::G_SETOTHERMODE_H:
		{
			int ss = (data & 0x0000FF0000000000) >> 40;
			int nn = (data & 0x000000FF00000000) >> 32;
			int dd = (data & 0xFFFFFFFF);

			sprintf(line, "gsSPSetOtherMode(0xE3, %i, %i, 0x%08X),", 32 - (nn + 1) - ss, nn + 1, dd);
		}
		break;
		case F3DZEXOpcode::G_SETTILE:
		{
			int fff =          (data & 0b0000000011100000000000000000000000000000000000000000000000000000) >> 53;
			int ii =           (data & 0b0000000000011000000000000000000000000000000000000000000000000000) >> 51;
			int nnnnnnnnn =    (data & 0b0000000000000011111111100000000000000000000000000000000000000000) >> 41;
			int mmmmmmmmm =    (data & 0b0000000000000000000000011111111100000000000000000000000000000000) >> 32;
			int ttt =          (data & 0b0000000000000000000000000000000000000111000000000000000000000000) >> 24;
			int pppp =         (data & 0b0000000000000000000000000000000000000000111100000000000000000000) >> 20;
			int cc =           (data & 0b0000000000000000000000000000000000000000000011000000000000000000) >> 18;
			int aaaa =         (data & 0b0000000000000000000000000000000000000000000000111100000000000000) >> 14;
			int ssss =         (data & 0b0000000000000000000000000000000000000000000000000011110000000000) >> 10;
			int dd =           (data & 0b0000000000000000000000000000000000000000000000000000001100000000) >> 8;
			int bbbb =         (data & 0b0000000000000000000000000000000000000000000000000000000011110000) >> 4;
			int uuuu =         (data & 0b0000000000000000000000000000000000000000000000000000000000001111);

			string fmtTbl[] = { "G_IM_FMT_RGBA", "G_IM_FMT_YUV", "G_IM_FMT_CI", "G_IM_FMT_IA", "G_IM_FMT_I" };
			string sizTbl[] = { "G_IM_SIZ_4b", "G_IM_SIZ_8b", "G_IM_SIZ_16b", "G_IM_SIZ_32b" };

			if (fff == (int)F3DZEXTexFormats::G_IM_FMT_CI)
				lastCISiz = (F3DZEXTexSizes)ii;

			lastTexSizTest = (F3DZEXTexSizes)ii;

			sprintf(line, "gsDPSetTile(%s, %s, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),", fmtTbl[fff].c_str(), sizTbl[ii].c_str(), nnnnnnnnn, mmmmmmmmm, ttt, pppp, cc, aaaa, ssss, dd, bbbb, uuuu);
		}
			break;
		case F3DZEXOpcode::G_SETTILESIZE:
		{
			int sss = (data & 0x00FFF00000000000) >> 48;
			int ttt = (data & 0x00000FFF00000000) >> 36;
			int uuu = (data & 0x0000000000FFF000) >> 12;
			int vvv = (data & 0x0000000000000FFF);
			int i = (data & 0x000000000F000000) >> 24;

			int shiftAmtW = 2;
			int shiftAmtH = 2;

			if (lastTexSizTest == F3DZEXTexSizes::G_IM_SIZ_8b && lastTexFmt == F3DZEXTexFormats::G_IM_FMT_IA)
			{
				shiftAmtW = 3;
			}

			//if (lastTexFmt == F3DZEXTexFormats::G_IM_FMT_I || lastTexFmt == F3DZEXTexFormats::G_IM_FMT_CI)
			if (lastTexSizTest == F3DZEXTexSizes::G_IM_SIZ_4b)
				shiftAmtW = 3;

			lastTexWidth = (uuu >> shiftAmtW) + 1;
			lastTexHeight = (vvv >> shiftAmtH) + 1;
			//printf("lastTexWidth: %i lastTexHeight: %i\n", lastTexWidth, lastTexHeight);

			//TextureGenCheck(prefix);

			sprintf(line, "gsDPSetTileSize(%i, %i, %i, %i, %i),", i, sss, ttt, uuu, vvv);
		}
			break;
		case F3DZEXOpcode::G_LOADBLOCK:
		{
			int sss = (data & 0x00FFF00000000000) >> 48;
			int ttt = (data & 0x00000FFF00000000) >> 36;
			int i = (data & 0x000000000F000000) >> 24;
			int xxx = (data & 0x0000000000FFF000) >> 12;
			int ddd = (data & 0x0000000000000FFF);

			//lastTexHeight = (ddd + 1) / 16;

			lastTexLoaded = true;

			//TextureGenCheck(prefix);

			sprintf(line, "gsDPLoadBlock(%i, %i, %i, %i, %i),", i, sss, ttt, xxx, ddd);
		}
		break;
		case F3DZEXOpcode::G_TEXTURE:
		{
			int ____ = (data & 0x0000FFFF00000000) >> 32;
			int ssss = (data & 0x00000000FFFF0000) >> 16;
			int tttt = (data & 0x000000000000FFFF);
			int lll = (____ & 0x3800) >> 7;
			int ddd = (____ & 0x700) >> 4;
			int nnnnnnn = (____ & 0xFE) >> 1;

			sprintf(line, "gsSPTexture(%i, %i, %i, %i, %i),", ssss, tttt, lll, ddd, nnnnnnn);
		}
		break;
		case F3DZEXOpcode::G_LOADTLUT:
		{
			int t =   (data & 0x0000000007000000) >> 24;
			int ccc = (data & 0x00000000003FF000) >> 14;

			if (lastCISiz == F3DZEXTexSizes::G_IM_SIZ_8b)
			{
				lastTexWidth = 16;
				lastTexHeight = 16;
			}
			else
			{
				lastTexWidth = 4;
				lastTexHeight = 4;
			}

			lastTexLoaded = true;

			TextureGenCheck(prefix);

			sprintf(line, "gsDPLoadTLUTCmd(%i, %i),", t, ccc);
		}
			break;
		case F3DZEXOpcode::G_SETCOMBINE:
		{
			int a0 = (data & 0b000000011110000000000000000000000000000000000000000000000000000) >> 52;
			int c0 = (data & 0b000000000001111100000000000000000000000000000000000000000000000) >> 47;
			int aa0 = (data & 0b00000000000000011100000000000000000000000000000000000000000000) >> 44;
			int ac0 = (data & 0b00000000000000000011100000000000000000000000000000000000000000) >> 41;
			int a1 = (data & 0b000000000000000000000011110000000000000000000000000000000000000) >> 37;
			int c1 = (data & 0b000000000000000000000000001111100000000000000000000000000000000) >> 32;
			int b0 = (data & 0b000000000000000000000000000000011110000000000000000000000000000) >> 28;
			int b1 = (data & 0b000000000000000000000000000000000001111000000000000000000000000) >> 24;
			int aa1 = (data & 0b00000000000000000000000000000000000000111000000000000000000000) >> 21;
			int ac1 = (data & 0b00000000000000000000000000000000000000000111000000000000000000) >> 18;
			int d0 = (data & 0b000000000000000000000000000000000000000000000111000000000000000) >> 15;
			int ab0 = (data & 0b00000000000000000000000000000000000000000000000111000000000000) >> 12;
			int ad0 = (data & 0b00000000000000000000000000000000000000000000000000111000000000) >> 9;
			int d1 = (data & 0b000000000000000000000000000000000000000000000000000000111000000) >> 6;
			int ab1 = (data & 0b00000000000000000000000000000000000000000000000000000000111000) >> 3;
			int ad1 = (data & 0b00000000000000000000000000000000000000000000000000000000000111) >> 0;

			string modes[] = { "COMBINED", "TEXEL0", "TEXEL1", "PRIMITIVE", "SHADE", "ENVIRONMENT", "1", "COMBINED_ALPHA", 
				"TEXEL0_ALPHA", "TEXEL1_ALPHA", "PRIMITIVE_ALPHA", "SHADE_ALPHA", "ENV_ALPHA", "LOD_FRACTION", "PRIM_LOD_FRAC", "K5",
				"17", "18", "19", "20", "21", "22", "23", "24", 
				"25", "26", "27", "28", "29", "30", "31", "0" };

			string modes2[] = { "COMBINED", "TEXEL0", "TEXEL1", "PRIMITIVE", "SHADE", "ENVIRONMENT", "1", "0" };

			sprintf(line, "gsDPSetCombineLERP(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s),", 
				modes[a0].c_str(), modes[b0].c_str(), modes[c0].c_str(), modes[d0].c_str(), 
				modes2[aa0].c_str(),  modes2[ab0].c_str(), modes2[ac0].c_str(), modes2[ad0].c_str(), 
				modes[a1].c_str(), modes[b1].c_str(), modes[c1].c_str(), modes[d1].c_str(), 
				modes2[aa1].c_str(), modes2[ab1].c_str(), modes2[ac1].c_str(), modes2[ad1].c_str());
		}
		break;
		case F3DZEXOpcode::G_RDPLOADSYNC:
			sprintf(line, "gsDPLoadSync(),");
			break;
		case F3DZEXOpcode::G_RDPPIPESYNC:
			sprintf(line, "gsDPPipeSync(),");
			break;
		case F3DZEXOpcode::G_RDPTILESYNC:
			sprintf(line, "gsDPTileSync(),");
			break;
		case F3DZEXOpcode::G_RDPFULLSYNC:
			sprintf(line, "gsDPFullSync(),");
			break;
		case F3DZEXOpcode::G_ENDDL:
			sprintf(line, "gsSPEndDisplayList(),");

			TextureGenCheck(prefix);
			break;
		default:
			sprintf(line, "// Opcode 0x%02X unimplemented!", opcode);
			break;
		}

		sourceOutput += line;

		sprintf(line, " // 0x%08X", dListAddress + (i * 8));
		sourceOutput += line;

		sourceOutput += "\n";
	}

	sourceOutput += "};\n";

	// Iterate through our vertex lists, connect intersecting lists.
	if (vertices.size() > 0)
	{
		char definesLine[2048];
		vector<pair<int32_t, vector<Vertex*>>> verticesSorted(vertices.begin(), vertices.end());

		sort(verticesSorted.begin(), verticesSorted.end(), [](const auto& lhs, const auto& rhs)
		{
			return lhs.first < rhs.first;
		});

		for (int i = 0; i < verticesSorted.size() - 1; i++)
		{
			//int vtxSize = verticesSorted[i].second.size() * 16;
			int vtxSize = vertices[verticesSorted[i].first].size() * 16;

			if ((verticesSorted[i].first + vtxSize) > verticesSorted[i + 1].first)
			{
				int intersectAmt = (verticesSorted[i].first + vtxSize) - verticesSorted[i + 1].first;
				int intersectIndex = intersectAmt / 16;

				for (int j = intersectIndex; j < verticesSorted[i + 1].second.size(); j++)
				{
					vertices[verticesSorted[i].first].push_back(verticesSorted[i + 1].second[j]);
				}

				sprintf(definesLine, "#define _%s_vertices_%08X ((u32)_%s_vertices_%08X + 0x%08X)\n", prefix.c_str(), verticesSorted[i + 1].first, prefix.c_str(), verticesSorted[i].first, verticesSorted[i + 1].first - verticesSorted[i].first);
				defines += definesLine;
				
				int nSize = vertices[verticesSorted[i].first].size();

				vertices.erase(verticesSorted[i + 1].first);
				verticesSorted.erase(verticesSorted.begin() + i + 1);

				i--;
			}
		}

		// Generate Vertex Declarations
		for (pair<int32_t, vector<Vertex*>> item : vertices)
		{
			char declLine[2048];
			string declaration = "";

			sprintf(declLine, "Vtx_t _%s_vertices_%08X[%i] = \n{\n", prefix.c_str(), item.first, item.second.size());
			declaration += declLine;

			int curAddr = item.first;

			for (Vertex* vtx : item.second)
			{
				sprintf(declLine, "\t { %i, %i, %i, %i, %i, %i, %i, %i, %i, %i }, // 0x%08X\n",
					vtx->x, vtx->y, vtx->z, vtx->flag, vtx->s, vtx->t, vtx->r, vtx->g, vtx->b, vtx->a, curAddr);

				//sprintf(declLine, "\t { 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X },\n",
					//(uint16_t)vtx->x, (uint16_t)vtx->y, (uint16_t)vtx->z, (uint16_t)vtx->flag, (uint16_t)vtx->s, (uint16_t)vtx->t, (uint16_t)vtx->r, (uint16_t)vtx->g, (uint16_t)vtx->b, (uint16_t)vtx->a);

				declaration += declLine;

				curAddr += 16;
			}

			declaration += "};\n";
			vtxDeclarations[item.first] = declaration;
		}

		// Generate Texture Declarations
		for (pair<int32_t, ZTexture*> item : textures)
		{
			char declLine[2048];
			string declaration = "";

			declaration += item.second->GetSourceOutputCode(prefix);

			texDeclarations[item.first] = declaration;
		}
	}

	return sourceOutput;
}

void ZDisplayList::TextureGenCheck(string prefix)
{
	if (TextureGenCheck(fileData, textures, scene, prefix, lastTexWidth, lastTexHeight, lastTexAddr, lastTexSeg, lastTexFmt, lastTexSiz, lastTexLoaded))
	{
		//lastTexWidth = 0;
		//lastTexHeight = 0;
		lastTexAddr = 0;
		lastTexLoaded = false;
	}
}

bool ZDisplayList::TextureGenCheck(vector<uint8_t> fileData, map<uint32_t, ZTexture*>& textures, ZRoom* scene, string prefix, uint32_t texWidth, uint32_t texHeight, uint32_t texAddr, uint32_t texSeg, F3DZEXTexFormats texFmt, F3DZEXTexSizes texSiz, bool texLoaded)
{
	char buffer[4096];
	int segmentNumber = (texSeg & 0xFF000000) >> 24;

	//printf("TextureGenCheck seg=%i width=%i height=%i addr=0x%08X\n", segmentNumber, texWidth, texHeight, texAddr);

	if (texAddr != 0 && texWidth != 0 && texHeight != 0 && texLoaded)
	{
		if (segmentNumber != 2)
		{
			sprintf(buffer, "_%s_tex_%08X", prefix.c_str(), texAddr);
			ZTexture* tex = new ZTexture(TexFormatToTexType(texFmt, texSiz), fileData, texAddr, string(buffer), texWidth, texHeight);
			
#ifdef _WIN32 // TEST
			tex->Save("dump");
#endif	
			textures[texAddr] = tex;

			return true;
		}
		else
		{
			sprintf(buffer, "_%s_tex_%08X", scene->GetName().c_str(), texAddr);
			ZTexture* tex = new ZTexture(TexFormatToTexType(texFmt, texSiz), scene->GetRawData(), texAddr, string(buffer), texWidth, texHeight);
			
#ifdef _WIN32 // TEST
			tex->Save("dump"); // TEST
#endif
			
			scene->declarations[texAddr] = new Declaration(DeclarationAlignment::None, tex->GetRawDataSize(), tex->GetSourceOutputCode(scene->GetName()));
			scene->externs[texAddr] = tex->GetSourceOutputHeader(scene->GetName());

			return true;
		}
	}

	return false;
}

TextureType ZDisplayList::TexFormatToTexType(F3DZEXTexFormats fmt, F3DZEXTexSizes siz)
{
	if (fmt == F3DZEXTexFormats::G_IM_FMT_RGBA)
	{
		if (siz == F3DZEXTexSizes::G_IM_SIZ_16b)
			return TextureType::RGBA16bpp;
		else if (siz == F3DZEXTexSizes::G_IM_SIZ_32b)
			return TextureType::RGBA32bpp;
	}
	else if (fmt == F3DZEXTexFormats::G_IM_FMT_CI)
	{
		//if (siz == F3DZEXTexSizes::G_IM_SIZ_8b)
			return TextureType::Palette8bpp;
	}
	else if (fmt == F3DZEXTexFormats::G_IM_FMT_IA)
	{
		if (siz == F3DZEXTexSizes::G_IM_SIZ_16b)
			return TextureType::GrayscaleAlpha16bpp;
		else if (siz == F3DZEXTexSizes::G_IM_SIZ_8b)
			return TextureType::GrayscaleAlpha8bpp;
	}
	else if (fmt == F3DZEXTexFormats::G_IM_FMT_I)
	{
		if (siz == F3DZEXTexSizes::G_IM_SIZ_8b)
			return TextureType::Grayscale8bpp;
		else if (siz == F3DZEXTexSizes::G_IM_SIZ_16b)
			return TextureType::Grayscale8bpp;
	}

	return TextureType::RGBA16bpp;
}

void ZDisplayList::Save(string outFolder)
{

}

vector<uint8_t> ZDisplayList::GetRawData()
{
	return rawData;
}

int ZDisplayList::GetRawDataSize()
{
	return instructions.size() * 8;
}

Vertex::Vertex(std::vector<uint8_t> rawData, int rawDataIndex)
{
	x = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	y = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	z = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	flag = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	s = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	t = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
	r = rawData[rawDataIndex + 12];
	g = rawData[rawDataIndex + 13];
	b = rawData[rawDataIndex + 14];
	a = rawData[rawDataIndex + 15];
}