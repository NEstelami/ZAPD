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
	ZRoom* zRoom;
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

class PolygonTypeBase
{
public:
	PolygonTypeBase(ZFile* nParent, const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZRoom* nRoom);

	virtual void ParseRawData() = 0;
	virtual void DeclareReferences(const std::string& prefix) = 0;

	virtual std::string GetBodySourceCode() = 0;
	void DeclareVar(const std::string& prefix, const std::string& bodyStr);
	void DeclareAndGenerateOutputCode(const std::string& prefix);

	virtual std::string GetSourceTypeName() const;

	std::string GetName() const;
	void SetName(const std::string& newName);
	virtual int GetRawDataSize() const = 0;

	std::string GetDefaultName(const std::string& prefix) const;

protected:
	uint8_t type;

	std::vector<PolygonDlist> polyDLists;


	std::vector<uint8_t> rawData;
	int rawDataIndex;
	ZFile* parent;
	ZRoom* zRoom;
	std::string name;
};

class PolygonType1: public PolygonTypeBase
{
protected:
	uint8_t format;
	segptr_t dlist;

	// single
	BgImage single;

	// multi
	uint8_t count;
	segptr_t list;  // BgImage*
	std::vector<BgImage> multiList;

public:
	PolygonType1(ZFile* nParent, const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZRoom* nRoom);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() override;

	std::string GetSourceTypeName() const override;

	int GetRawDataSize() const override;
};

class PolygonType2: public PolygonTypeBase
{
public:
	PolygonType2(ZFile* nParent, const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZRoom* nRoom);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() override;

	int GetRawDataSize() const override;

protected:
	uint8_t num;
	segptr_t start;
	segptr_t end;

	//std::vector<PolygonDlist> polyDLists;
};

class SetMesh : public ZRoomCommand
{
public:
	SetMesh(ZRoom* nZRoom, const std::vector<uint8_t>& nRawData, int nRawDataIndex, int segAddressOffset);

	std::string GetBodySourceCode() override;

	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() override;

private:
	uint8_t meshHeaderType;

	std::string GenDListExterns(ZDisplayList* dList);
	void SyotesHack(int segAddressOffset);
};
