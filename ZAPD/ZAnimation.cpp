#include "ZAnimation.h"
#include <utility>
#include "ZFile.h"
#include "BitConverter.h"
#include "StringHelper.h"
#include "File.h"
#include "HighLevel/HLAnimationIntermediette.h"
#include "Globals.h"

using namespace std;

ZAnimation::ZAnimation() : ZResource()
{
	frameCount = 0;
}

void ZAnimation::ParseRawData()
{
	const uint8_t* data = rawData.data();

	// Read the header
	frameCount = BitConverter::ToInt16BE(data, rawDataIndex + 0);
}

void ZAnimation::Save(const std::string& outFolder)
{
	if (Globals::Instance->testMode)
	{
		HLAnimationIntermediette* anim = HLAnimationIntermediette::FromZAnimation(this);
		string xml = anim->OutputXML();
		File::WriteAllText(outFolder + "/" + name + ".anmi", xml);

		delete anim;
	}
}

void ZAnimation::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);
}

string ZAnimation::GetSourceOutputCode(const std::string& prefix)
{
	return "";
}

ZResourceType ZAnimation::GetResourceType()
{
	return ZResourceType::Animation;
}

ZNormalAnimation::ZNormalAnimation() : ZAnimation()
{
	rotationValues = vector<uint16_t>();
	rotationIndices = vector<RotationIndex>();
	limit = 0;
}

std::string ZNormalAnimation::GetSourceOutputCode(const std::string& prefix)
{
	if (parent != nullptr)
	{
		string defaultPrefix = name.c_str();
		defaultPrefix.replace(0, 1, "s"); // replace g prefix with s for local variables

		string headerStr = StringHelper::Sprintf("{ %i }, %sFrameData, %sJointIndices, %i",
			frameCount, defaultPrefix.c_str(), defaultPrefix.c_str(), limit);
		parent->AddDeclaration(rawDataIndex, DeclarationAlignment::None, 16, "AnimationHeader", StringHelper::Sprintf("%s", name.c_str()), headerStr);

		string indicesStr = "";
		string valuesStr = "    ";
		const int lineLength = 14;
		const int offset = 0;

		for (int i = 0; i < rotationValues.size(); i++)
		{
			valuesStr += StringHelper::Sprintf("0x%04X, ", rotationValues[i]);

			if ((i - offset + 1) % lineLength == 0)
				valuesStr += "\n    ";
		}

		for (int i = 0; i < rotationIndices.size(); i++)
		{
			indicesStr += StringHelper::Sprintf("    { 0x%04X, 0x%04X, 0x%04X },", rotationIndices[i].x, rotationIndices[i].y, rotationIndices[i].z);

			if (i != (rotationIndices.size() - 1))
				indicesStr += "\n";
		}

		parent->AddDeclarationArray(rotationValuesSeg, DeclarationAlignment::Align16, (int)rotationValues.size() * 2, "static s16",
			StringHelper::Sprintf("%sFrameData", defaultPrefix.c_str()), rotationValues.size(), valuesStr);

		parent->AddDeclarationArray(rotationIndicesSeg, DeclarationAlignment::Align16, (int)rotationIndices.size() * 6, "static JointIndex",
			StringHelper::Sprintf("%sJointIndices", defaultPrefix.c_str()), rotationIndices.size(), indicesStr);
	}

	return "";
}

int ZNormalAnimation::GetRawDataSize()
{
	return 16;
}

ZNormalAnimation* ZNormalAnimation::ExtractFromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, const std::string& nRelPath)
{
	ZNormalAnimation* anim = new ZNormalAnimation();
	anim->rawData = std::move(nRawData);
	anim->rawDataIndex = rawDataIndex;
	anim->ParseXML(reader);
	anim->ParseRawData();

	return anim;
}

void ZNormalAnimation::ParseRawData()
{
	ZAnimation::ParseRawData();

	const uint8_t* data = rawData.data();

	rotationValuesSeg = BitConverter::ToInt32BE(data, rawDataIndex + 4) & 0x00FFFFFF;
	rotationIndicesSeg = BitConverter::ToInt32BE(data, rawDataIndex + 8) & 0x00FFFFFF;
	limit = BitConverter::ToInt16BE(data, rawDataIndex + 12);

	uint32_t currentPtr = rotationValuesSeg;

	// Read the Rotation Values
	for (int i = 0; i < ((rotationIndicesSeg - rotationValuesSeg) / 2); i++)
	{
		rotationValues.push_back(BitConverter::ToInt16BE(data, currentPtr));
		currentPtr += 2;
	}

	currentPtr = rotationIndicesSeg;

	// Read the Rotation Indices
	for (int i = 0; i < ((rawDataIndex - rotationIndicesSeg) / 6); i++)
	{
		rotationIndices.push_back(RotationIndex(BitConverter::ToInt16BE(data, currentPtr), BitConverter::ToInt16BE(data, currentPtr + 2), BitConverter::ToInt16BE(data, currentPtr + 4)));
		currentPtr += 6;
	}
}

ZLinkAnimation::ZLinkAnimation() : ZAnimation()
{
	segmentAddress = 0;
}

std::string ZLinkAnimation::GetSourceOutputCode(const std::string& prefix)
{
	if (parent != nullptr)
	{
		string segSymbol = segmentAddress == 0 ? "NULL" : parent->GetDeclarationName(segmentAddress, StringHelper::Sprintf("%sSeg%06X", name.c_str(), segmentAddress));
		string headerStr = StringHelper::Sprintf("{ %i }, 0x%08X",
			frameCount, segmentAddress);
		parent->AddDeclaration(rawDataIndex, DeclarationAlignment::None, 16, "LinkAnimationHeader", StringHelper::Sprintf("%s", name.c_str()), headerStr);
	}

	return "";
}

int ZLinkAnimation::GetRawDataSize()
{
	return 8;
}

ZLinkAnimation* ZLinkAnimation::ExtractFromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, const std::string& nRelPath)
{
	ZLinkAnimation* anim = new ZLinkAnimation();
	anim->rawData = std::move(nRawData);
	anim->rawDataIndex = rawDataIndex;
	anim->ParseXML(reader);
	anim->ParseRawData();

	return anim;
}

void ZLinkAnimation::ParseRawData()
{
	ZAnimation::ParseRawData();

	const uint8_t* data = rawData.data();

	//segmentAddress = GETSEGOFFSET(BitConverter::ToInt32BE(data, rawDataIndex + 4));
	segmentAddress = (BitConverter::ToInt32BE(data, rawDataIndex + 4));
}


/* ZCurveAnimation */

TransformData::TransformData(ZFile* parent, const std::vector<uint8_t>& rawData, uint32_t fileOffset)
	: parent(parent)
{
	unk_00 = BitConverter::ToUInt16BE(rawData, fileOffset + 0);
	unk_02 = BitConverter::ToUInt16BE(rawData, fileOffset + 2);
	unk_04 = BitConverter::ToInt16BE(rawData, fileOffset + 4);
	unk_06 = BitConverter::ToInt16BE(rawData, fileOffset + 6);
	unk_08 = BitConverter::ToFloatBE(rawData, fileOffset + 8);
}

TransformData::TransformData(ZFile* parent, const std::vector<uint8_t>& rawData, uint32_t fileOffset, size_t index)
	: TransformData(parent, rawData, fileOffset + index * GetRawDataSize())
{
}

std::string TransformData::GetBody(const std::string& prefix) const
{
	return StringHelper::Sprintf("0x%04X, 0x%04X, %i, %i, %ff", 
		unk_00, unk_02, unk_04, unk_06, unk_08);
}

size_t TransformData::GetRawDataSize()
{
	return 0x0C;
}

std::string TransformData::GetSourceTypeName()
{
	return "TransformData";
}


ZCurveAnimation::ZCurveAnimation(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZFile* nParent)
{
	rawData.assign(nRawData.begin(), nRawData.end());
	rawDataIndex = nRawDataIndex;
	parent = nParent;

	ParseXML(reader);
	ParseRawData();

	if (transformData != 0) {
		uint32_t transformDataOffset = Seg2Filespace(transformData, parent->baseAddress);
		//for () { // TODO: Figure out how big is this array
			transformDataArr.emplace_back(parent, nRawData, transformDataOffset, 0);
		//}
	}
}

void ZCurveAnimation::ParseRawData()
{
	ZAnimation::ParseRawData();

	refIndex = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);
	transformData = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
	copyValues = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
	unk_0C = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);
	unk_10 = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
}

ZCurveAnimation* ZCurveAnimation::ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int nRawDataIndex, std::string nRelPath, ZFile* nParent)
{
	ZCurveAnimation* curve = new ZCurveAnimation(reader, nRawData, nRawDataIndex, nParent);
	curve->relativePath = std::move(nRelPath);

	curve->parent->AddDeclaration(
		curve->rawDataIndex, DeclarationAlignment::Align16, curve->GetRawDataSize(), 
		curve->GetSourceTypeName(), curve->name, "");

	return curve;
}

void ZCurveAnimation::PreGenValues(const std::string& prefix)
{
	if (transformData != 0) {
		uint32_t transformDataOffset = Seg2Filespace(transformData, parent->baseAddress);
		string transformDataStr = StringHelper::Sprintf("%sCurveAnime_%s_%06X", prefix.c_str(), TransformData::GetSourceTypeName().c_str(), transformDataOffset);

		string entryStr = "";
		uint16_t arrayItemCnt = 1; // TODO

		size_t i = 0;
		for (auto& child: transformDataArr) {
			entryStr += StringHelper::Sprintf("    { %s },%s", 
				child.GetBody(prefix).c_str(), 
				(++i < arrayItemCnt) ? "\n" : "");
		}

		Declaration* decl = parent->GetDeclaration(transformDataOffset);
		if (decl == nullptr) {
			parent->AddDeclarationArray(
				transformDataOffset, DeclarationAlignment::None, 
				arrayItemCnt * TransformData::GetRawDataSize(), TransformData::GetSourceTypeName(), 
				transformDataStr, arrayItemCnt, entryStr);
		}
		else {
			decl->text = entryStr;
		}
	}
}

int ZCurveAnimation::GetRawDataSize()
{
	return 0x10;
}

std::string ZCurveAnimation::GetSourceOutputCode(const std::string& prefix)
{
	string bodyStr = "";
	uint32_t address = Seg2Filespace(rawDataIndex, parent->baseAddress);

	PreGenValues(prefix);

	string transformDataStr = "NULL";
	if (transformData != 0) {
		uint32_t transformDataOffset = Seg2Filespace(transformData, parent->baseAddress);
		Declaration* decl = parent->GetDeclaration(transformDataOffset);
		if (decl == nullptr) {
			transformDataStr = StringHelper::Sprintf("%sCurveAnime_%s_%06X", prefix.c_str(), TransformData::GetSourceTypeName().c_str(), transformDataOffset);
		}
		else {
			transformDataStr = decl->varName;
		}
	}


	bodyStr = StringHelper::Sprintf("0x%08X, %s, 0x%08X, %i, %i", 
		refIndex, transformDataStr.c_str(), copyValues, unk_0C, unk_10);

	Declaration* decl = parent->GetDeclaration(address);
	if (decl == nullptr) {
		parent->AddDeclaration(address, DeclarationAlignment::None, 
			GetRawDataSize(), GetSourceTypeName(), name, bodyStr);
	}
	else {
		decl->text = bodyStr;
	}

	return "";
}

std::string ZCurveAnimation::GetSourceTypeName()
{
	return "TransformUpdateIndex";
}
