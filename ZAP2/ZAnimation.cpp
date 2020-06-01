#include "ZAnimation.h"
#include "ZFile.h"
#include "BitConverter.h"
#include "StringHelper.h"

using namespace std;

ZAnimation::ZAnimation() : ZResource()
{
	frameCount = 0;
	rotationValues = vector<uint16_t>();
	rotationIndices = vector<RotationIndex>();
	limit = 0;
}


ZAnimation* ZAnimation::ExtractFromXML(tinyxml2::XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath)
{
	ZAnimation* anim = new ZAnimation();
	anim->rawData = nRawData;
	anim->rawDataIndex = rawDataIndex;
	anim->ParseXML(reader);
	anim->ParseRawData();

	return anim;
}

void ZAnimation::ParseRawData()
{
	uint8_t* data = rawData.data();

	// Read the header
	frameCount = BitConverter::ToInt16BE(data, rawDataIndex + 0);
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
		rotationIndices.push_back(RotationIndex(BitConverter::ToInt16BE(data, currentPtr + 0), BitConverter::ToInt16BE(data, currentPtr + 2), BitConverter::ToInt16BE(data, currentPtr + 4)));
		currentPtr += 6;
	}
}

void ZAnimation::ParseXML(tinyxml2::XMLElement* reader)
{
	name = reader->Attribute("Name");
	limbCount = reader->IntAttribute("LimbCount");
}

string ZAnimation::GetSourceOutputCode(string prefix)
{
	if (parent != nullptr)
	{
		string headerStr = StringHelper::Sprintf("AnimationHeader _%s_%s = { { %i, %i }, _%s_%s_values, _%s_%s_indices, %i };\n",
			prefix.c_str(), name.c_str(), frameCount, 0, prefix.c_str(), name.c_str(), prefix.c_str(), name.c_str(), limit);
		parent->declarations[rawDataIndex] = new Declaration(DeclarationAlignment::None, 16, headerStr);
		parent->externs[rawDataIndex] = StringHelper::Sprintf("extern AnimationHeader _%s_%s;", prefix.c_str(), name.c_str());

		string valuesStr = "";

		valuesStr += StringHelper::Sprintf("AnimationRotationValue _%s_%s_values[%i] = \n{\n\t", prefix.c_str(), name.c_str(), rotationValues.size());

		for (int i = 0; i < rotationValues.size(); i++)
		{
			valuesStr += StringHelper::Sprintf("0x%04X, ", rotationValues[i]);

			if (i % 16 == 15)
				valuesStr += "\n\t";
		}

		valuesStr += "};\n";

		parent->declarations[rotationValuesSeg] = new Declaration(DeclarationAlignment::Align16, rotationValues.size() * 2, valuesStr);
		parent->externs[rotationValuesSeg] = StringHelper::Sprintf("extern AnimationRotationValue _%s_%s_values[%i];", prefix.c_str(), name.c_str(), rotationValues.size());

		string indicesStr = "";

		indicesStr += StringHelper::Sprintf("AnimationRotationIndex _%s_%s_indices[%i] = \n{\n", prefix.c_str(), name.c_str(), rotationIndices.size());

		for (int i = 0; i < rotationIndices.size(); i++)
		{
			indicesStr += StringHelper::Sprintf("\t{ 0x%04X, 0x%04X, 0x%04X },\n", rotationIndices[i].x, rotationIndices[i].y, rotationIndices[i].z);
		}

		indicesStr += "};\n";

		parent->declarations[rotationIndicesSeg] = new Declaration(DeclarationAlignment::Align16, rotationIndices.size() * 6, indicesStr);
		parent->externs[rotationIndicesSeg] = StringHelper::Sprintf("extern AnimationRotationIndex _%s_%s_indices[%i];", prefix.c_str(), name.c_str(), rotationIndices.size());
	}

	return "";
}