#include "SetCollisionHeader.h"
#include "../ZRoom.h"
#include "../../BitConverter.h"

using namespace std;

SetCollisionHeader::SetCollisionHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;

	collisionHeader = CollisionHeader(nZRoom, rawData, segmentOffset);

	string declaration = "";
	char line[2048];

	sprintf(line, "ColHeader collisionHeader_%08X = { 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, vertices_%08X, 0x%04X, polygons_%08X, polygonTypes_%08X, &camData_%08X, 0x%04X, waterBoxes_%08X };",
		segmentOffset,
		(uint16_t)collisionHeader.absMinX, (uint16_t)collisionHeader.absMinY, (uint16_t)collisionHeader.absMinZ,
		(uint16_t)collisionHeader.absMaxX, (uint16_t)collisionHeader.absMaxY, (uint16_t)collisionHeader.absMaxZ,
		collisionHeader.numVerts, collisionHeader.vtxSegmentOffset, collisionHeader.numPolygons,
		collisionHeader.polySegmentOffset, collisionHeader.polyTypeDefSegmentOffset,
		collisionHeader.camDataSegmentOffset, collisionHeader.numWaterBoxes, collisionHeader.waterBoxSegmentOffset);

	declaration += line;

	zRoom->declarations[segmentOffset] = declaration;
}

string SetCollisionHeader::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x00, (u32)&collisionHeader_%08X};", ZRoomCommand::GenerateSourceCodePass1(roomName).c_str(), segmentOffset);
	sourceOutput = line;

	return sourceOutput;
}

string SetCollisionHeader::GenerateSourceCodePass2(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	return sourceOutput;
}

string SetCollisionHeader::GetCommandCName()
{
	return "SCmdColHeader";
}

string SetCollisionHeader::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "extern ColHeader collisionHeader_%08X;\n", segmentOffset);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetCollisionHeader::GetRoomCommand()
{
	return RoomCommand::SetCollisionHeader;
}

CollisionHeader::CollisionHeader()
{

}

CollisionHeader::CollisionHeader(ZRoom* zRoom, std::vector<uint8_t> rawData, int rawDataIndex)
{
	absMinX = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	absMinY = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	absMinZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);

	absMaxX = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	absMaxY = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	absMaxZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);

	numVerts = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);
	vtxSegmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 16) & 0x00FFFFFF;

	numPolygons = BitConverter::ToInt16BE(rawData, rawDataIndex + 20);
	polySegmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 24) & 0x00FFFFFF;
	polyTypeDefSegmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 28) & 0x00FFFFFF;
	camDataSegmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 32) & 0x00FFFFFF;

	numWaterBoxes = BitConverter::ToInt16BE(rawData, rawDataIndex + 36);
	waterBoxSegmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 40) & 0x00FFFFFF;

	for (int i = 0; i < 2; i++)
		polygonTypes.push_back(BitConverter::ToInt64BE(rawData, polyTypeDefSegmentOffset + (i * 8)));

	for (int i = 0; i < numVerts; i++)
		vertices.push_back(new VertexEntry(zRoom, rawData, vtxSegmentOffset + (i * 6)));

	for (int i = 0; i < numPolygons; i++)
		polygons.push_back(new PolygonEntry(zRoom, rawData, polySegmentOffset + (i * 16)));

	if (camDataSegmentOffset != 0)
		camData = new CameraData(zRoom, rawData, camDataSegmentOffset);

	for (int i = 0; i < numWaterBoxes; i++)
		waterBoxes.push_back(new WaterBoxHeader(zRoom, rawData, waterBoxSegmentOffset + (i * 16)));

	string declaration = "";
	char line[2048];

	if (waterBoxes.size() > 0)
	{
		sprintf(line, "WaterBoxHeader waterBoxes_%08X[] = \n{\n", waterBoxSegmentOffset);
		declaration += line;

		for (int i = 0; i < waterBoxes.size(); i++)
		{
			sprintf(line, "\t{ %i, %i, %i, %i, %i, 0x%08X },\n", waterBoxes[i]->xMin, waterBoxes[i]->ySurface, waterBoxes[i]->zMin, waterBoxes[i]->xLength, waterBoxes[i]->zLength, waterBoxes[i]->properties);
			declaration += line;
		}

		declaration += "};\n";
	}

	zRoom->declarations[waterBoxSegmentOffset] = declaration;

	if (polygons.size() > 0)
	{
		declaration = "";

		sprintf(line, "RoomPoly polygons_%08X[] = \n{\n", polySegmentOffset);
		declaration += line;

		for (int i = 0; i < polygons.size(); i++)
		{
			sprintf(line, "\t{ 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X },\n",
				(uint16_t)polygons[i]->type, (uint16_t)polygons[i]->vtxA, (uint16_t)polygons[i]->vtxB, (uint16_t)polygons[i]->vtxC,
				(uint16_t)polygons[i]->a, (uint16_t)polygons[i]->b, (uint16_t)polygons[i]->c, (uint16_t)polygons[i]->d);
			declaration += line;
		}

		declaration += "};\n";

		zRoom->declarations[polySegmentOffset] = declaration;

		declaration = "";
	}

	declaration = "";

	sprintf(line, "u64 polygonTypes_%08X[] = \n{\n", polyTypeDefSegmentOffset);
	declaration += line;

	for (int i = 0; i < polygonTypes.size(); i++)
	{
		sprintf(line, "\t0x%016llX,\n", polygonTypes[i]);
		declaration += line;
	}

	declaration += "};\n";

	zRoom->declarations[polyTypeDefSegmentOffset] = declaration;

	declaration = "";

	if (vertices.size() > 0)
	{
		declaration = "";

		sprintf(line, "Vec3s vertices_%08X[] = \n{\n", vtxSegmentOffset);
		declaration += line;

		for (int i = 0; i < vertices.size(); i++)
		{
			sprintf(line, "\t{ %i, %i, %i },\n", vertices[i]->x, vertices[i]->y, vertices[i]->z);
			declaration += line;
		}

		declaration += "};\n";

		zRoom->declarations[vtxSegmentOffset] = declaration;

		declaration = "";
	}
}

PolygonEntry::PolygonEntry(ZRoom* zRoom, std::vector<uint8_t> rawData, int rawDataIndex)
{
	type = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	vtxA = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	vtxB = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	vtxC = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	a = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	b = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
	c = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);
	d = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
}

VertexEntry::VertexEntry(ZRoom* zRoom, std::vector<uint8_t> rawData, int rawDataIndex)
{
	x = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	y = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	z = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
}

WaterBoxHeader::WaterBoxHeader(ZRoom* zRoom, std::vector<uint8_t> rawData, int rawDataIndex)
{
	xMin = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	ySurface = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	zMin = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	xLength = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	zLength = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	properties = BitConverter::ToInt32BE(rawData, rawDataIndex + 12);
}

CameraData::CameraData(ZRoom* zRoom, std::vector<uint8_t> rawData, int rawDataIndex)
{
	cameraSType = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	numCameras = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	cameraPositionDataSeg = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;
	unknown = BitConverter::ToInt32BE(rawData, rawDataIndex + 8);

	string declaration = "";
	char line[2048];

	sprintf(line, "CamData camData_%08X = ", rawDataIndex);
	declaration += line;

	sprintf(line, "{ 0x%04X, 0x%04X, ", cameraSType, numCameras);
	declaration += line;

	if (cameraPositionDataSeg != 0)
		sprintf(line, "camPosData_%08X, ", cameraPositionDataSeg);
	else
		sprintf(line, "0, ");
		
	declaration += line;

	sprintf(line, "0x%08X };\n", unknown);
	declaration += line;

	zRoom->declarations[rawDataIndex] = declaration;

	if (cameraPositionDataSeg != 0)
		cameraPositionData = new CameraPositionData(zRoom, rawData, cameraPositionDataSeg);
}

CameraPositionData::CameraPositionData(ZRoom* zRoom, std::vector<uint8_t> rawData, int rawDataIndex)
{
	posX = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	posY = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	posZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);

	rotX = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	rotY = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	rotZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);

	fov = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);
	jfifId = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
	unk = BitConverter::ToInt16BE(rawData, rawDataIndex + 16);

	string declaration = "";
	char line[2048];

	sprintf(line, "CamPosData camPosData_%08X = ", rawDataIndex);
	declaration += line;

	sprintf(line, "{ %i, %i, %i, %i, %i, %i, %i, %i, 0x%04X };\n", posX, posY, posZ, rotX, rotY, rotZ, fov, jfifId, unk);
	declaration += line;

	zRoom->declarations[rawDataIndex] = declaration;
}