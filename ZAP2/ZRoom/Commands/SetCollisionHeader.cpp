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
	char waterBoxStr[2048];

	if (collisionHeader.waterBoxSegmentOffset != 0)
		sprintf(waterBoxStr, "_%s_waterBoxes_%08X", zRoom->GetName().c_str(), collisionHeader.waterBoxSegmentOffset);
	else
		sprintf(waterBoxStr, "0");

	sprintf(line, "CollisionHeader _%s_collisionHeader_%08X = { 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, %i, _%s_vertices_%08X, 0x%04X, _%s_polygons_%08X, _%s_polygonTypes_%08X, &_%s_camData_%08X, 0x%04X, %s };\n",
		zRoom->GetName().c_str(), segmentOffset,
		(uint16_t)collisionHeader.absMinX, (uint16_t)collisionHeader.absMinY, (uint16_t)collisionHeader.absMinZ,
		(uint16_t)collisionHeader.absMaxX, (uint16_t)collisionHeader.absMaxY, (uint16_t)collisionHeader.absMaxZ,
		collisionHeader.numVerts, zRoom->GetName().c_str(), collisionHeader.vtxSegmentOffset, collisionHeader.numPolygons,
		zRoom->GetName().c_str(), collisionHeader.polySegmentOffset, zRoom->GetName().c_str(), collisionHeader.polyTypeDefSegmentOffset,
		zRoom->GetName().c_str(), collisionHeader.camDataSegmentOffset, collisionHeader.numWaterBoxes, waterBoxStr);

	declaration += line;

	zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, DeclarationPadding::Pad16, 44, declaration);
}

string SetCollisionHeader::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x00, (u32)&_%s_collisionHeader_%08X};", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), zRoom->GetName().c_str(), segmentOffset);
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

	sprintf(line, "extern CollisionHeader _%s_collisionHeader_%08X;\n", zRoom->GetName().c_str(), segmentOffset);
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

	for (int i = 0; i < numVerts; i++)
		vertices.push_back(new VertexEntry(zRoom, rawData, vtxSegmentOffset + (i * 6)));

	for (int i = 0; i < numPolygons; i++)
		polygons.push_back(new PolygonEntry(zRoom, rawData, polySegmentOffset + (i * 16)));

	int highestPolyType = 0;

	for (PolygonEntry* poly : polygons)
	{
		if (poly->type > highestPolyType)
			highestPolyType = poly->type;
	}

	for (int i = 0; i < highestPolyType + 1; i++)
		polygonTypes.push_back(BitConverter::ToInt64BE(rawData, polyTypeDefSegmentOffset + (i * 8)));

	if (camDataSegmentOffset != 0)
		camData = new CameraData(zRoom, rawData, camDataSegmentOffset);

	for (int i = 0; i < numWaterBoxes; i++)
		waterBoxes.push_back(new WaterBoxHeader(zRoom, rawData, waterBoxSegmentOffset + (i * 16)));

	string declaration = "";
	char line[2048];

	if (waterBoxes.size() > 0)
	{
		sprintf(line, "WaterBoxHeader _%s_waterBoxes_%08X[] = \n{\n", zRoom->GetName().c_str(), waterBoxSegmentOffset);
		declaration += line;

		for (int i = 0; i < waterBoxes.size(); i++)
		{
			sprintf(line, "\t{ %i, %i, %i, %i, %i, 0x%08X },\n", waterBoxes[i]->xMin, waterBoxes[i]->ySurface, waterBoxes[i]->zMin, waterBoxes[i]->xLength, waterBoxes[i]->zLength, waterBoxes[i]->properties);
			declaration += line;
		}

		declaration += "};\n";
	}

	if (waterBoxSegmentOffset != 0)
		zRoom->declarations[waterBoxSegmentOffset] = new Declaration(DeclarationAlignment::None, 16 * waterBoxes.size(), declaration);

	if (polygons.size() > 0)
	{
		declaration = "";

		sprintf(line, "RoomPoly _%s_polygons_%08X[] = \n{\n", zRoom->GetName().c_str(), polySegmentOffset);
		declaration += line;

		for (int i = 0; i < polygons.size(); i++)
		{
			sprintf(line, "\t{ 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X }, // 0x%08X\n",
				(uint16_t)polygons[i]->type, (uint16_t)polygons[i]->vtxA, (uint16_t)polygons[i]->vtxB, (uint16_t)polygons[i]->vtxC,
				(uint16_t)polygons[i]->a, (uint16_t)polygons[i]->b, (uint16_t)polygons[i]->c, (uint16_t)polygons[i]->d, polySegmentOffset + (i * 16));
			declaration += line;
		}

		declaration += "};\n";

		if (polySegmentOffset != 0)
			zRoom->declarations[polySegmentOffset] = new Declaration(DeclarationAlignment::None, polygons.size() * 16, declaration);

		declaration = "";
	}

	declaration = "";

	sprintf(line, "u32 _%s_polygonTypes_%08X[] = \n{\n", zRoom->GetName().c_str(), polyTypeDefSegmentOffset);
	declaration += line;

	for (int i = 0; i < polygonTypes.size(); i++)
	{
		//sprintf(line, "\t0x%016llX,\n", polygonTypes[i]);
		sprintf(line, "\t 0x%08X, 0x%08X, \n",  polygonTypes[i] >> 32, polygonTypes[i] & 0xFFFFFFFF);
		declaration += line;
	}

	declaration += "};\n";

	if (polyTypeDefSegmentOffset != 0)
		zRoom->declarations[polyTypeDefSegmentOffset] = new Declaration(DeclarationAlignment::None, polygonTypes.size() * 8, declaration);

	declaration = "";

	if (vertices.size() > 0)
	{
		declaration = "";

		sprintf(line, "Vec3s _%s_vertices_%08X[%i] = \n{\n", zRoom->GetName().c_str(), vtxSegmentOffset, vertices.size());
		declaration += line;

		for (int i = 0; i < vertices.size(); i++)
		{
			sprintf(line, "\t{ %i, %i, %i }, // 0x%08X\n", vertices[i]->x, vertices[i]->y, vertices[i]->z, vtxSegmentOffset + (i * 6));
			declaration += line;
		}

		declaration += "};\n";

		if (vtxSegmentOffset != 0)
			zRoom->declarations[vtxSegmentOffset] = new Declaration(DeclarationAlignment::None, vertices.size() * 6, declaration);

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
	cameraPosDataSeg = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);

	int cameraPosDataAddr = cameraPosDataSeg & 0x00FFFFFF;

	string declaration = "";
	char line[2048];

	sprintf(line, "CamData _%s_camData_%08X = ", zRoom->GetName().c_str(), rawDataIndex);
	declaration += line;

	char camPosDataStr[2048];

	if (numCameras > 0)
		sprintf(camPosDataStr, "(u32)_%s_camPosData_%08X", zRoom->GetName().c_str(), cameraPosDataAddr);
	else
		sprintf(camPosDataStr, "0x%08X", cameraPosDataSeg);

	sprintf(line, "{ 0x%04X, 0x%04X, %s };\n", cameraSType, numCameras, camPosDataStr);
	declaration += line;

	//if (cameraPosDataSeg != 0)
		//sprintf(line, "(u32)&_%s_camPosData_%08X, ", zRoom->GetName().c_str(), cameraPosDataSeg);
	//else
		//sprintf(line, "0, ");

	//declaration += line;

	//sprintf(line, "0x%08X };\n", unknown);
	//declaration += line;

	zRoom->declarations[rawDataIndex] = new Declaration(DeclarationAlignment::None, 8, declaration);

	declaration = "";

	if (numCameras > 0)
	{
		sprintf(line, "CamPosData _%s_camPosData_%08X[] = \n{\n", zRoom->GetName().c_str(), cameraPosDataAddr);
		declaration += line;

		for (int i = 0; i < numCameras; i++)
		{
			CameraPositionData* data = new CameraPositionData(zRoom, rawData, cameraPosDataAddr + (i * 0x12));
			cameraPositionData.push_back(data);

			sprintf(line, "\t{ %i, %i, %i, %i, %i, %i, %i, %i, 0x%04X }, // 0x%08X\n", data->posX, data->posY, data->posZ, data->rotX, data->rotY, data->rotZ, data->fov, data->jfifId, (uint16_t)data->unk, cameraPosDataSeg + (i * 0x12));
			declaration += line;
		}

		declaration += "};\n";

		zRoom->declarations[cameraPosDataSeg & 0x00FFFFFF] = new Declaration(DeclarationAlignment::None, numCameras * 0x12, declaration);
	}


	declaration = "";

	//if (numCameras > 0)
	{
		sprintf(line, "CamPosDataEntry _%s_camPosDataEntries_%08X[] = \n{\n", zRoom->GetName().c_str(), rawDataIndex + 8);
		declaration += line;

		while (true)
		{
			char camSegLine[2048];
			CameraPosDataEntry* entry = new CameraPosDataEntry();

			entry->unknown = BitConverter::ToInt32BE(rawData, rawDataIndex + 8 + (entries.size() * 8) + 0);
			entry->cameraPosDataSeg = BitConverter::ToInt32BE(rawData, rawDataIndex + 8 + (entries.size() * 8) + 4);

			if (entry->cameraPosDataSeg != 0)
			{
				int index = ((entry->cameraPosDataSeg & 0x00FFFFFF) - cameraPosDataAddr) / 0x12;
				sprintf(camSegLine, "(u32)&_%s_camPosData_%08X[%i]", zRoom->GetName().c_str(), cameraPosDataAddr, index);
			}
			else
				sprintf(camSegLine, "0x%08X", entry->cameraPosDataSeg);

			sprintf(line, "\t{ 0x%08X, %s }, // 0x%08X\n", entry->unknown, camSegLine, rawDataIndex + 8 + (entries.size() * 8));
			declaration += line;

			entries.push_back(entry);

			if (entry->unknown == 0x00010000 || entry->unknown == 0x00030000 || entry->unknown == 0x00050000)
				break;
		}

		declaration += "};\n";
	}

	zRoom->declarations[rawDataIndex + 8] = new Declaration(DeclarationAlignment::None, entries.size() * 8, declaration);
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
}