#pragma once

#include "../ZRoomCommand.h"

class PolygonEntry
{
public:
	int16_t type;
	int16_t vtxA, vtxB, vtxC;
	int16_t a, b, c, d;

	PolygonEntry(ZRoom* zRoom, std::vector<uint8_t> rawData, int rawDataIndex);
};

class VertexEntry
{
public:
	int16_t x, y, z;

	VertexEntry(ZRoom* zRoom, std::vector<uint8_t> rawData, int rawDataIndex);
};

class WaterBoxHeader
{
public:
	int16_t xMin;
	int16_t ySurface;
	int16_t zMin;
	int16_t xLength;
	int16_t zLength;
	int16_t pad;
	int32_t properties;

	WaterBoxHeader(ZRoom* zRoom, std::vector<uint8_t> rawData, int rawDataIndex);
};

class CameraPositionData
{
public:
	int16_t posX, posY, posZ;
	int16_t rotX, rotY, rotZ;
	int16_t fov;
	int16_t jfifId;
	int16_t unk;

	CameraPositionData(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);
};

class CameraPosDataEntry
{
public:
	uint32_t cameraPosDataSeg;
	uint32_t unknown;
};

class CameraData
{
public:
	int16_t cameraSType;
	int16_t numCameras;
	int32_t cameraPosDataSeg;
	std::vector<CameraPosDataEntry*> entries;
	//uint32_t unknown;


	std::vector<CameraPositionData*> cameraPositionData;

	CameraData(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);
};

class CollisionHeader
{
public:
	int16_t absMinX, absMinY, absMinZ;
	int16_t absMaxX, absMaxY, absMaxZ;
	int16_t numVerts;
	int32_t vtxSegmentOffset;
	int16_t numPolygons;
	int32_t polySegmentOffset;
	int32_t polyTypeDefSegmentOffset;
	int32_t camDataSegmentOffset;

	int32_t numWaterBoxes;
	int32_t waterBoxSegmentOffset;

	std::vector<VertexEntry*> vertices;
	std::vector<PolygonEntry*> polygons;
	std::vector<uint64_t> polygonTypes;
	std::vector<WaterBoxHeader*> waterBoxes;
	CameraData* camData;

	CollisionHeader();
	CollisionHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);
};

class SetCollisionHeader : public ZRoomCommand
{
public:
	SetCollisionHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, int baseAddress);
	virtual std::string GenerateSourceCodePass2(std::string roomName);
	virtual std::string GetCommandCName();
	virtual std::string GenerateExterns();
	virtual RoomCommand GetRoomCommand();

private:
	CollisionHeader collisionHeader;
	uint32_t segmentOffset;
};