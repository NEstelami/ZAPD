#pragma once

#include "../../ZDisplayList.h"
#include "../ZRoomCommand.h"
#include "ZBackground.h"

class PolygonDlist
{
public:
	PolygonDlist() = default;
	PolygonDlist(const std::string& prefix, const std::vector<uint8_t>& nRawData, int nRawDataIndex,
	             ZFile* nParent, ZRoom* nRoom);

	void ParseRawData();
	void DeclareReferences(const std::string& prefix);

	int GetRawDataSize();
	void SetPolyType(uint8_t nPolyType);

	void DeclareVar(const std::string& prefix, const std::string& bodyStr);

	std::string GetBodySourceCode(bool arrayElement);
	void DeclareAndGenerateOutputCode();

	static std::string GetDefaultName(const std::string& prefix, uint32_t address);
	std::string GetSourceTypeName();
	std::string GetName();

protected:
	int16_t x, y, z; // polyType == 2
	int16_t unk_06; // polyType == 2

	segptr_t opa = 0;  // Gfx*
	segptr_t xlu = 0;  // Gfx*

	uint8_t polyType;

	ZDisplayList* opaDList = nullptr;  // Gfx*
	ZDisplayList* xluDList = nullptr;  // Gfx*

	std::vector<uint8_t> rawData;
	int rawDataIndex;
	ZFile* parent;
	ZRoom* room;
	std::string name;

	ZDisplayList* MakeDlist(segptr_t ptr, const std::string& prefix);
};

class BgImage
{
protected:
	uint16_t unk_00;
	uint8_t id;
	segptr_t source;
	uint32_t unk_0C;
	uint32_t tlut;
	uint16_t width;
	uint16_t height;
	uint8_t fmt;
	uint8_t siz;
	uint16_t mode0;
	uint16_t tlutCount;

	ZBackground* sourceBackground;

	std::vector<uint8_t> rawData;
	int rawDataIndex;
	ZFile* parent;
	std::string name;
	bool isSubStruct;

	void ParseRawData();
	ZBackground* MakeBackground(segptr_t ptr, const std::string& prefix);

public:
	BgImage() = default;
	BgImage(bool nIsSubStruct, const std::string& prefix, const std::vector<uint8_t>& nRawData,
	        int nRawDataIndex, ZFile* nParent);

	static int GetRawDataSize();

	std::string GetBodySourceCode(bool arrayElement);

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
	segptr_t list;  // BgImage*
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

class PolygonType2
{
public:
	PolygonType2(const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZFile* nParent, ZRoom* nRoom);

	void DeclareReferences(const std::string& prefix);

	std::string GetBodySourceCode();

	std::string GetSourceTypeName() const;
	int GetRawDataSize() const;

protected:
	uint8_t type;
	uint8_t num;
	segptr_t start;
	segptr_t end;

	std::vector<PolygonDlist> polyDLists;

	ZFile* parent;
};

class SetMesh : public ZRoomCommand
{
public:
	SetMesh(ZRoom* nZRoom, const std::vector<uint8_t>& nRawData, int nRawDataIndex, int segAddressOffset);

	std::string GetBodySourceCode() override;
	std::string GenerateExterns() override;

	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() override;

private:
	uint8_t meshHeaderType;

	std::string GenDListExterns(ZDisplayList* dList);
};
