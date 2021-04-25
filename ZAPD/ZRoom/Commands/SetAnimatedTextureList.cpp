#include "SetAnimatedTextureList.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetAnimatedTextureList::SetAnimatedTextureList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

void SetAnimatedTextureList::ParseRawData()
{
	int32_t currentPtr = segmentOffset;
	bool keepGoing = true;

	do
	{
		AnimatedTexture lastTexture(rawData, currentPtr);
		keepGoing = (lastTexture.segment != 0) && (lastTexture.segment > -1);
		currentPtr += 8;
		textures.push_back(lastTexture);
	} while (keepGoing);
}

void SetAnimatedTextureList::DeclareReferences(const std::string& prefix)
{
	std::string nameStr =
		StringHelper::Sprintf("%sAnimatedTextureList0x%06X", prefix.c_str(), segmentOffset);

	for (auto& texture : textures)
	{
		size_t declSize = 0;
		std::string declTypeName = "";
		std::string declName = StringHelper::Sprintf("%sAnimatedTextureParams0x%06X",
		                                             prefix.c_str(), texture.segmentOffset);
		std::string declaration = "";
		int index = 0;

		switch (texture.type)
		{
		case 0:
		case 1:
			for (const auto& param : texture.params)
			{
				declaration += param->GenerateSourceCode(zRoom, texture.segmentOffset);

				if (index < texture.params.size() - 1)
					declaration += "\n";

				index++;
			}

			declSize = texture.params.size() * 4;
			declTypeName = "ScrollingTextureParams";

			parent->AddDeclarationArray(texture.segmentOffset, DeclarationAlignment::Align4,
			                            declSize, declTypeName, declName, texture.params.size(),
			                            declaration);
			break;
		case 2:
		case 3:
		case 4:
			declSize = texture.params.at(0)->GetParamsSize();
			declTypeName = "FlashingTextureParams";
			declaration = texture.params.at(0)->GenerateSourceCode(zRoom, texture.segmentOffset);

			parent->AddDeclaration(texture.segmentOffset, DeclarationAlignment::Align4, declSize,
			                       declTypeName, declName,
			                       StringHelper::Sprintf("\n\t%s\n", declaration.c_str()));
			break;
		case 5:
			declSize = texture.params.at(0)->GetParamsSize();
			declTypeName = "CyclingTextureParams";
			declaration = texture.params.at(0)->GenerateSourceCode(zRoom, texture.segmentOffset);

			parent->AddDeclaration(texture.segmentOffset, DeclarationAlignment::Align4, declSize,
			                       declTypeName, declName,
			                       StringHelper::Sprintf("\n\t%s\n", declaration.c_str()));
			break;
		case 6:
			continue;

		default:
			throw std::runtime_error(
				StringHelper::Sprintf("Error in SetAnimatedTextureList::DeclareReferences (%s)\n"
			                          "\t Unknown texture.type: %i\n",
			                          nameStr.c_str(), texture.type));
		}
	}

	if (!textures.empty())
	{
		std::string declaration = "";

		for (size_t i = 0; i < textures.size(); i++)
		{
			std::string textureName = parent->GetDeclarationPtrName(textures.at(i).segmentOffset);

			declaration += StringHelper::Sprintf("\t{ %2i, %2i, %s },", textures.at(i).segment,
			                                     textures.at(i).type, textureName.c_str());

			if (i + 1 < textures.size())
				declaration += "\n";
		}

		parent->AddDeclarationArray(segmentOffset, DeclarationAlignment::Align4,
		                            DeclarationPadding::Pad16, textures.size() * 8,
		                            "AnimatedTexture", nameStr, textures.size(), declaration);
	}
}

std::string SetAnimatedTextureList::GetBodySourceCode() const
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("SCENECMD_TEXTURE_ANIM_LIST(%s)", listName.c_str());
}

size_t SetAnimatedTextureList::GetRawDataSize()
{
	size_t paramsSize = 0;
	for (const auto& texture : textures)
	{
		for (const auto& param : texture.params)
		{
			paramsSize += param->GetParamsSize();
		}
	}

	return ZRoomCommand::GetRawDataSize() + paramsSize;
}

string SetAnimatedTextureList::GetCommandCName() const
{
	return "SCmdTextureAnimations";
}

RoomCommand SetAnimatedTextureList::GetRoomCommand() const
{
	return RoomCommand::SetAnimatedTextureList;
}

AnimatedTexture::AnimatedTexture(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: segment(rawData.at(rawDataIndex)), type(BitConverter::ToInt16BE(rawData, rawDataIndex + 2)),
	  segmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4)))
{
	switch (type)
	{
	case 0:
		params.push_back(std::make_shared<ScrollingTexture>(rawData, segmentOffset));
		break;
	case 1:
		params.push_back(std::make_shared<ScrollingTexture>(rawData, segmentOffset));
		params.push_back(std::make_shared<ScrollingTexture>(rawData, segmentOffset + 4));
		break;
	case 2:
	case 3:
	case 4:
		params.push_back(std::make_shared<FlashingTexture>(rawData, segmentOffset, type));
		break;
	case 5:
		params.push_back(std::make_shared<CyclingTextureParams>(rawData, segmentOffset));
		break;
	case 6:  // Some terminator when there are no animated textures?
		break;
	}
}

ScrollingTexture::ScrollingTexture(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: xStep(rawData.at(rawDataIndex + 0)), yStep(rawData.at(rawDataIndex + 1)),
	  width(rawData.at(rawDataIndex + 2)), height(rawData.at(rawDataIndex + 3))
{
}

std::string ScrollingTexture::GenerateSourceCode(ZRoom* zRoom, uint32_t baseAddress)
{
	return StringHelper::Sprintf("    { %i, %i, 0x%02X, 0x%02X },", xStep, yStep, width, height);
}

size_t ScrollingTexture::GetParamsSize()
{
	return 4;
}

FlashingTexturePrimColor::FlashingTexturePrimColor(const std::vector<uint8_t>& rawData,
                                                   uint32_t rawDataIndex)
	: r(rawData.at(rawDataIndex + 0)), g(rawData.at(rawDataIndex + 1)),
	  b(rawData.at(rawDataIndex + 2)), a(rawData.at(rawDataIndex + 3)),
	  lodFrac(rawData.at(rawDataIndex + 4))
{
}

FlashingTextureEnvColor::FlashingTextureEnvColor(const std::vector<uint8_t>& rawData,
                                                 uint32_t rawDataIndex)
	: r(rawData.at(rawDataIndex + 0)), g(rawData.at(rawDataIndex + 1)),
	  b(rawData.at(rawDataIndex + 2)), a(rawData.at(rawDataIndex + 3))
{
}

FlashingTexture::FlashingTexture(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex, int32_t type)
	: cycleLength(BitConverter::ToUInt16BE(rawData, rawDataIndex + 0)),
	  numKeyFrames(BitConverter::ToUInt16BE(rawData, rawDataIndex + 2)),
	  primColorSegmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4))),
	  envColorSegmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 8))),
	  keyFrameSegmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 12)))
{
	uint16_t length = (type == 2) ? cycleLength : numKeyFrames;

	int32_t currentPtr = primColorSegmentOffset;
	for (uint16_t i = 0; i < length; i++)
	{
		primColors.push_back(FlashingTexturePrimColor(rawData, currentPtr));
		currentPtr += 5;
	}

	currentPtr = envColorSegmentOffset;
	for (uint16_t i = 0; i < length; i++)
	{
		envColors.push_back(FlashingTextureEnvColor(rawData, currentPtr));
		currentPtr += 4;
	}

	currentPtr = keyFrameSegmentOffset;
	for (uint16_t i = 0; i < length; i++)
	{
		keyFrames.push_back(BitConverter::ToUInt16BE(rawData, currentPtr));
		currentPtr += 2;
	}
}

std::string FlashingTexture::GenerateSourceCode(ZRoom* zRoom, uint32_t baseAddress)
{
	if (primColorSegmentOffset != 0)
	{
		string declaration = "";
		size_t index = 0;

		for (FlashingTexturePrimColor& color : primColors)
		{
			declaration += StringHelper::Sprintf("    { 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X },",
			                                     color.r, color.g, color.b, color.a, color.lodFrac);

			if (index < primColors.size() - 1)
				declaration += "\n";

			index++;
		}

		zRoom->parent->AddDeclarationArray(
			primColorSegmentOffset, DeclarationAlignment::Align4, DeclarationPadding::None,
			primColors.size() * 5, "FlashingTexturePrimColor",
			StringHelper::Sprintf("%sAnimatedTexturePrimColor0x%06X", zRoom->GetName().c_str(),
		                          primColorSegmentOffset),
			primColors.size(), declaration);
	}

	if (envColorSegmentOffset != 0)
	{
		string declaration = "";
		size_t index = 0;

		for (FlashingTextureEnvColor& color : envColors)
		{
			declaration += StringHelper::Sprintf("    { 0x%02X, 0x%02X, 0x%02X, 0x%02X },", color.r,
			                                     color.g, color.b, color.a);

			if (index < envColors.size() - 1)
				declaration += "\n";

			index++;
		}

		zRoom->parent->AddDeclarationArray(
			envColorSegmentOffset, DeclarationAlignment::Align4, DeclarationPadding::None,
			envColors.size() * 4, "Color_RGBA8",
			StringHelper::Sprintf("%sAnimatedTextureEnvColors0x%06X", zRoom->GetName().c_str(),
		                          envColorSegmentOffset),
			envColors.size(), declaration);
	}

	if (keyFrameSegmentOffset != 0)
	{
		string declaration = "";
		size_t index = 0;

		for (uint16_t keyFrame : keyFrames)
		{
			declaration += StringHelper::Sprintf("    0x%02X,", keyFrame);

			if (index < keyFrames.size() - 1)
				declaration += "\n";

			index++;
		}

		zRoom->parent->AddDeclarationArray(keyFrameSegmentOffset, DeclarationAlignment::Align4,
		                                   DeclarationPadding::None, keyFrames.size() * 2, "u16",
		                                   StringHelper::Sprintf("%sAnimatedTextureKeyFrames0x%06X",
		                                                         zRoom->GetName().c_str(),
		                                                         keyFrameSegmentOffset),
		                                   keyFrames.size(), declaration);
	}

	std::string primName = zRoom->parent->GetDeclarationPtrName(primColorSegmentOffset);
	std::string envName = zRoom->parent->GetDeclarationPtrName(envColorSegmentOffset);
	std::string keyName = zRoom->parent->GetDeclarationPtrName(keyFrameSegmentOffset);

	return StringHelper::Sprintf("%i, %i, %s, %s, %s", cycleLength, numKeyFrames, primName.c_str(),
	                             envName.c_str(), keyName.c_str());
}

size_t FlashingTexture::GetParamsSize()
{
	return 16;
}

CyclingTextureParams::CyclingTextureParams(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: cycleLength(BitConverter::ToUInt16BE(rawData, rawDataIndex + 0)),
	  textureSegmentOffsetsSegmentOffset(
		  GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4))),
	  textureIndicesSegmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 8)))
{
	int32_t currentPtr = textureIndicesSegmentOffset;
	int32_t maxIndex = 0;

	for (uint16_t i = 0; i < cycleLength; i++)
	{
		uint8_t newIndex = rawData.at(currentPtr);
		textureIndices.push_back(newIndex);
		currentPtr++;
		if (newIndex > maxIndex)
			maxIndex = newIndex;
	}

	currentPtr = textureSegmentOffsetsSegmentOffset;
	for (int32_t i = 0; i < maxIndex + 1; i++)
	{
		textureSegmentOffsets.push_back(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, currentPtr)));
		currentPtr += 4;
	}
}

std::string CyclingTextureParams::GenerateSourceCode(ZRoom* zRoom, uint32_t baseAddress)
{
	if (textureSegmentOffsetsSegmentOffset != 0)
	{
		string declaration = "";
		size_t index = 0;

		for (uint32_t offset : textureSegmentOffsets)
		{
			declaration +=
				StringHelper::Sprintf("    %sTex_%06X,", zRoom->GetName().c_str(), offset);

			if (index < textureSegmentOffsets.size() - 1)
				declaration += "\n";

			index++;
		}

		zRoom->parent->AddDeclarationArray(
			textureSegmentOffsetsSegmentOffset, DeclarationAlignment::Align4,
			DeclarationPadding::None, textureSegmentOffsets.size() * 4, "u64*",
			StringHelper::Sprintf("%sAnimatedTextureTexSegOffsets0x%06X", zRoom->GetName().c_str(),
		                          textureSegmentOffsetsSegmentOffset),
			textureSegmentOffsets.size(), declaration);
	}

	if (textureIndicesSegmentOffset != 0)
	{
		string declaration = "";
		size_t index = 0;

		for (uint8_t textureIndex : textureIndices)
		{
			declaration += StringHelper::Sprintf("    0x%02X,", textureIndex);

			if (index < textureIndices.size() - 1)
				declaration += "\n";

			index++;
		}

		zRoom->parent->AddDeclarationArray(
			textureIndicesSegmentOffset, DeclarationAlignment::Align4, DeclarationPadding::None,
			textureIndices.size(), "u8",
			StringHelper::Sprintf("%sAnimatedTextureTexIndices0x%06X", zRoom->GetName().c_str(),
		                          textureIndicesSegmentOffset),
			textureIndices.size(), declaration);
	}

	std::string segmName = zRoom->parent->GetDeclarationPtrName(textureSegmentOffsetsSegmentOffset);
	std::string indexesName = zRoom->parent->GetDeclarationPtrName(textureIndicesSegmentOffset);

	return StringHelper::Sprintf("%i, %s, %s", cycleLength, segmName.c_str(), indexesName.c_str());
}

size_t CyclingTextureParams::GetParamsSize()
{
	return 12;
}
