#include "ZCollision.h"

#include <cassert>
#include <cstdint>
#include <string>

#include "WarningHandler.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

REGISTER_ZFILENODE(Collision, ZCollisionHeader);

ZCollisionHeader::ZCollisionHeader(ZFile* nParent) : ZResource(nParent)
{
}

ZCollisionHeader::~ZCollisionHeader()
{
	delete camData;
}

void ZCollisionHeader::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	absMinX = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	absMinY = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	absMinZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);

	absMaxX = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	absMaxY = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	absMaxZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);

	numVerts = BitConverter::ToUInt16BE(rawData, rawDataIndex + 12);
	vtxAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 16);

	numPolygons = BitConverter::ToUInt16BE(rawData, rawDataIndex + 20);
	polyAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 24);
	polyTypeDefAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 28);
	camDataAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 32);

	numWaterBoxes = BitConverter::ToUInt16BE(rawData, rawDataIndex + 36);
	waterBoxAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 40);

	vtxSegmentOffset = Seg2Filespace(vtxAddress, parent->baseAddress);
	polySegmentOffset = Seg2Filespace(polyAddress, parent->baseAddress);
	polyTypeDefSegmentOffset = Seg2Filespace(polyTypeDefAddress, parent->baseAddress);
	camDataSegmentOffset = Seg2Filespace(camDataAddress, parent->baseAddress);
	waterBoxSegmentOffset = Seg2Filespace(waterBoxAddress, parent->baseAddress);

	vertices.reserve(numVerts);
	polygons.reserve(numPolygons);

	offset_t currentPtr = vtxSegmentOffset;

	for (uint16_t i = 0; i < numVerts; i++)
	{
		ZVector vec(parent);
		vec.ExtractFromBinary(currentPtr, ZScalarType::ZSCALAR_S16, 3);

		currentPtr += vec.GetRawDataSize();
		vertices.push_back(vec);
	}

	for (uint16_t i = 0; i < numPolygons; i++)
	{
		ZCollisionPoly poly(parent);
		poly.SetRawDataIndex(polySegmentOffset + (i * 16));
		poly.ParseRawData();
		polygons.push_back(poly);
	}

	uint16_t highestPolyType = 0;

	for (const ZCollisionPoly& poly : polygons)
	{
		if (poly.type > highestPolyType)
			highestPolyType = poly.type;
	}
	polygonTypes.reserve(highestPolyType);
	for (uint16_t i = 0; i < highestPolyType + 1; i++)
	{
		ZSurfaceType surfaceType(parent);

		surfaceType.SetRawDataIndex(polyTypeDefSegmentOffset + (i * 8));
		surfaceType.ParseRawData();
		polygonTypes.push_back(surfaceType);
	}

	if (camDataAddress != SEGMENTED_NULL)
	{
		// Try to guess how many elements the CamDataList array has.
		// The "guessing algorithm" is basically a "best effort" one and it
		// is error-prone.
		// This is based mostly on observation of how CollisionHeader data is
		// usually ordered. If for some reason the data was in some other funny
		// order, this would probably break.
		// The most common ordering is:
		// - *CamData*
		// - SurfaceType
		// - CollisionPoly
		// - Vertices
		// - WaterBoxes
		// - CollisionHeader
		upperCameraBoundary = polyTypeDefSegmentOffset;
		if (upperCameraBoundary == SEGMENTED_NULL)
		{
			upperCameraBoundary = polySegmentOffset;
		}
		if (upperCameraBoundary == SEGMENTED_NULL)
		{
			upperCameraBoundary = vtxSegmentOffset;
		}
		if (upperCameraBoundary == SEGMENTED_NULL)
		{
			upperCameraBoundary = waterBoxSegmentOffset;
		}
		if (upperCameraBoundary == SEGMENTED_NULL)
		{
			upperCameraBoundary = rawDataIndex;
		}

		if (upperCameraBoundary > camDataSegmentOffset)  // Hack for sharp ocarina
		{
			size_t numElements = (upperCameraBoundary - camDataSegmentOffset) / 8;

			camData = new ZCamData(parent, this, numElements);
			camData->SetRawDataIndex(camDataSegmentOffset);
			camData->ParseRawData();
			camData->DeclareReferences("placeholder");  // Can be anything but not empty
		}
		else
		{
			std::string placeHolder;
			if (!Globals::Instance->GetSegmentedPtrName(camDataSegmentOffset, parent, "CamData",
			                                            placeHolder))
			{
				HANDLE_WARNING_RESOURCE(
					WarningType::Always, parent, this, camDataSegmentOffset,
					"Invalid offset for CamData",
					StringHelper::Sprintf(
						"CamData is in a strange place. Is this a modded map? Try "
						"creating a node for it. It is at offset %06X.",
						camDataSegmentOffset));
			}
		}
	}

	waterBoxes.reserve(numWaterBoxes);

	for (int32_t i = 0; i < numWaterBoxes; i++)
	{
		ZWaterbox waterbox(parent);

		waterbox.SetRawDataIndex(waterBoxSegmentOffset +
		                         (i * (Globals::Instance->game == ZGame::OOT_SW97 ? 12 : 16)));
		waterbox.ParseRawData();
		waterBoxes.push_back(waterbox);
	}
}

void ZCollisionHeader::DeclareReferences(const std::string& prefix)
{
	std::string declaration = "";
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	if (waterBoxes.size() > 0)
	{
		for (size_t i = 0; i < waterBoxes.size(); i++)
		{
			declaration +=
				StringHelper::Sprintf("\t{ %s },", waterBoxes[i].GetBodySourceCode().c_str());
			if (i + 1 < waterBoxes.size())
				declaration += "\n";
		}

		parent->AddDeclarationArray(waterBoxSegmentOffset, DeclarationAlignment::Align4,
		                            waterBoxes[0].GetRawDataSize() * waterBoxes.size(),
		                            waterBoxes[0].GetSourceTypeName().c_str(),
		                            StringHelper::Sprintf("%sWaterBoxes", auxName.c_str()),
		                            waterBoxes.size(), declaration);
	}

	if (polygons.size() > 0)
	{
		declaration.clear();

		for (size_t i = 0; i < polygons.size(); i++)
		{
			declaration += StringHelper::Sprintf("\t%s,", polygons[i].GetBodySourceCode().c_str());
			if (i + 1 < polygons.size())
				declaration += "\n";
		}

		parent->AddDeclarationArray(
			polySegmentOffset, DeclarationAlignment::Align4,
			polygons.size() * polygons[0].GetRawDataSize(), polygons[0].GetSourceTypeName().c_str(),
			StringHelper::Sprintf("%sPolygons", auxName.c_str()), polygons.size(), declaration);
	}

	declaration.clear();
	for (size_t i = 0; i < polygonTypes.size(); i++)
	{
		declaration += StringHelper::Sprintf("\t%s,", polygonTypes[i].GetBodySourceCode().c_str());

		if (i < polygonTypes.size() - 1)
			declaration += "\n";
	}

	if (polyTypeDefAddress != 0)
		parent->AddDeclarationArray(polyTypeDefSegmentOffset, DeclarationAlignment::Align4,
		                            polygonTypes.size() * 8,
		                            polygonTypes[0].GetSourceTypeName().c_str(),
		                            StringHelper::Sprintf("%sSurfaceType", auxName.c_str()),
		                            polygonTypes.size(), declaration);

	declaration.clear();

	if (camData != nullptr)
	{
		declaration += StringHelper::Sprintf("\t%s", camData->GetBodySourceCode().c_str());

		if ((camDataAddress != SEGMENTED_NULL) && (upperCameraBoundary > camDataSegmentOffset))
		{
			parent->AddDeclarationArray(camDataSegmentOffset, DeclarationAlignment::Align4,
			                            camData->GetRawDataSize(), camData->GetSourceTypeName(),
			                            StringHelper::Sprintf("%sCamData", auxName.c_str()),
			                            camData->count, declaration);
		}
		declaration.clear();
	}

	if (vertices.size() > 0)
	{
		declaration.clear();

		for (size_t i = 0; i < vertices.size(); i++)
		{
			declaration +=
				StringHelper::Sprintf("\t{ %s },", vertices[i].GetBodySourceCode().c_str());

			if (i < vertices.size() - 1)
				declaration += "\n";
		}

		const auto& first = vertices.front();
		if (vtxAddress != 0)
			parent->AddDeclarationArray(
				vtxSegmentOffset, first.GetDeclarationAlignment(),
				vertices.size() * first.GetRawDataSize(), first.GetSourceTypeName(),
				StringHelper::Sprintf("%sVertices", auxName.c_str()), vertices.size(), declaration);
	}
}

std::string ZCollisionHeader::GetBodySourceCode() const
{
	std::string declaration = "";

	declaration += "\n";

	declaration += StringHelper::Sprintf("\t{ %i, %i, %i },\n", absMinX, absMinY, absMinZ);
	declaration += StringHelper::Sprintf("\t{ %i, %i, %i },\n", absMaxX, absMaxY, absMaxZ);

	std::string vtxName;
	Globals::Instance->GetSegmentedPtrName(vtxAddress, parent, "Vec3s", vtxName);
	declaration += StringHelper::Sprintf("\t%i, %s,\n", numVerts, vtxName.c_str());

	std::string polyName;
	Globals::Instance->GetSegmentedPtrName(polyAddress, parent, "CollisionPoly", polyName);
	declaration += StringHelper::Sprintf("\t%i, %s,\n", numPolygons, polyName.c_str());

	std::string surfaceName;
	Globals::Instance->GetSegmentedPtrName(polyTypeDefAddress, parent, "SurfaceType", surfaceName);
	declaration += StringHelper::Sprintf("\t%s,\n", surfaceName.c_str());

	std::string camName;
	Globals::Instance->GetSegmentedPtrName(camDataAddress, parent, "CamData", camName);
	declaration += StringHelper::Sprintf("\t%s,\n", camName.c_str());

	std::string waterBoxName;
	Globals::Instance->GetSegmentedPtrName(waterBoxAddress, parent, "WaterBox", waterBoxName);
	declaration += StringHelper::Sprintf("\t%i, %s\n", numWaterBoxes, waterBoxName.c_str());

	return declaration;
}

std::string ZCollisionHeader::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sCol_%06X", prefix.c_str(), rawDataIndex);
}

std::string ZCollisionHeader::GetSourceTypeName() const
{
	return "CollisionHeader";
}

ZResourceType ZCollisionHeader::GetResourceType() const
{
	return ZResourceType::CollisionHeader;
}

size_t ZCollisionHeader::GetRawDataSize() const
{
	return 44;
}
