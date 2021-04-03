#pragma once

#include "../../ZDisplayList.h"
#include "../ZRoomCommand.h"
#include "ZPrerender.h"


class PolygonDlist
{
protected:
	segptr_t opa = 0;  // Gfx*
	segptr_t xlu = 0;  // Gfx*

	std::vector<uint8_t> rawData;
	int rawDataIndex;
	ZFile* parent;
	ZRoom* room;
	std::string name;

	void ParseRawData();
	ZDisplayList* MakeDlist(segptr_t ptr, const std::string& prefix);

public:
	PolygonDlist() = default;
	PolygonDlist(const std::string& prefix, const std::vector<uint8_t>& nRawData, int nRawDataIndex,
	             ZFile* nParent, ZRoom* nRoom);

	int GetRawDataSize();

	void DeclareVar(const std::string& prefix, const std::string& bodyStr);

	std::string GetBodySourceCode(bool arrayElement);
	void DeclareAndGenerateOutputCode();

	static std::string GetDefaultName(const std::string& prefix, uint32_t address);
	std::string GetSourceTypeName();
	std::string GetName();

	ZDisplayList* opaDList = nullptr;  // Gfx*
	ZDisplayList* xluDList = nullptr;  // Gfx*
};

class MeshHeaderBase
{
public:
	int8_t headerType;  // 0x00
};

class MeshHeader0 : public MeshHeaderBase
{
public:
	std::vector<PolygonDlist> entries;
	uint32_t dListStart;
	uint32_t dListEnd;
};

class BackgroundRecord
{
public:
	uint16_t unknown;  // 0x00
	int8_t bgID;       // 0x02

	uint32_t imagePtr;  // 0x04
	uint32_t unknown2;  // 0x08
	uint32_t unknown3;  // 0x0C

	uint16_t bgWidth;   // 0x10
	uint16_t bgHeight;  // 0x12

	uint8_t imageFmt;    // 0x14
	uint8_t imageSize;   // 0x15
	uint16_t imagePal;   // 0x16
	uint16_t imageFlip;  // 0x18
};

class MeshEntry2
{
public:
	int16_t playerXMax, playerZMax;
	int16_t playerXMin, playerZMin;

	int32_t opaqueDListAddr;
	int32_t translucentDListAddr;

	ZDisplayList* opaqueDList;
	ZDisplayList* translucentDList;
};

class MeshHeader2 : public MeshHeaderBase
{
public:
	std::vector<MeshEntry2*> entries;
	uint32_t dListStart;
	uint32_t dListEnd;
};

class BgImage
{
protected:
    uint16_t   unk_00;
    uint8_t    id;
    segptr_t   source;
    uint32_t   unk_0C;
    uint32_t   tlut;
    uint16_t   width;
    uint16_t   height;
    uint8_t    fmt;
    uint8_t    siz;
    uint16_t   mode0;
    uint16_t   tlutCount;

	ZPrerender* sourceBackground;

	std::vector<uint8_t> rawData;
	int rawDataIndex;
	ZFile* parent;
	std::string name;
	bool isSubStruct;

	void ParseRawData();
	ZPrerender* MakeBackground(segptr_t ptr, const std::string& prefix);

public:
	BgImage() = default;
	BgImage(bool nIsSubStruct, const std::string& prefix, const std::vector<uint8_t>& nRawData, int nRawDataIndex,
	             ZFile* nParent);

	static int GetRawDataSize();

	//void DeclareVar(const std::string& prefix, const std::string& bodyStr);

	std::string GetBodySourceCode(bool arrayElement);
	//void DeclareAndGenerateOutputCode();

	static std::string GetDefaultName(const std::string& prefix, uint32_t address);
	static std::string GetSourceTypeName();
	std::string GetName();
};

class PolygonType1
{
protected:
	uint8_t type;
	uint8_t format;
	segptr_t dlist;

	// single
	BgImage single;

	// multi
	uint8_t count;
	segptr_t list; // BgImage*
	std::vector<BgImage> multiList;


	std::vector<uint8_t> rawData;
	int rawDataIndex;
	ZFile* parent;
	std::string name;

	void ParseRawData();

public:
	PolygonType1(const std::string& prefix, const std::vector<uint8_t>& nRawData, int nRawDataIndex,
	             ZFile* nParent, ZRoom* nRoom);

	int GetRawDataSize();

	void DeclareVar(const std::string& prefix, const std::string& bodyStr);

	std::string GetBodySourceCode();
	void DeclareAndGenerateOutputCode();

	static std::string GetDefaultName(const std::string& prefix, uint32_t address);
	std::string GetSourceTypeName();
	std::string GetName();

	PolygonDlist polyGfxList;
};

class SetMesh : public ZRoomCommand
{
public:
	SetMesh(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex, int segAddressOffset);
	~SetMesh();

	virtual std::string GenerateSourceCodePass1(std::string roomName, int baseAddress);
	// virtual std::string GenerateSourceCodePass2(std::string roomName, int baseAddress);
	// virtual std::string GenerateSourceCodePass3(std::string roomName);
	virtual std::string GenerateExterns();
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();
	virtual int32_t GetRawDataSize();

private:
	MeshHeaderBase* meshHeader;
	uint32_t segmentOffset;
	uint8_t data;
	uint8_t meshHeaderType;

	void GenDListDeclarations(std::vector<uint8_t> rawData, ZDisplayList* dList);
	std::string GenDListExterns(ZDisplayList* dList);
};
