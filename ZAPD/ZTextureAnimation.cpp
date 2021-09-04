#include "ZTextureAnimation.h"
#include <memory>
#include <vector>
#include "Globals.h"
#include "Utils/BitConverter.h"
#include "ZFile.h"
#include "ZResource.h"
#include "tinyxml2.h"

REGISTER_ZFILENODE(TextureAnimation, ZTextureAnimation);

/* Constructors */
ZTextureAnimationParams::ZTextureAnimationParams(ZFile* parent) : ZResource::ZResource(parent) {}
TextureScrollingParams::TextureScrollingParams(ZFile* parent) : ZTextureAnimationParams::ZTextureAnimationParams(parent) {}

void ZTextureAnimationParams::ExtractFromBinary(uint32_t paramsOffset) {}
void ZTextureAnimationParams::ExtractFromBinary(uint32_t paramsOffset, int count) {}
std::string ZTextureAnimationParams::GetDefaultName(const std::string& prefix, uint32_t address) const {}

/* TextureAnimationParams */

ZResourceType ZTextureAnimationParams::GetResourceType() const
{
	return ZResourceType::TextureAnimationParams;
}

std::string ZTextureAnimationParams::GetSourceOutputCode(const std::string& prefix)
{
	printf("TextureAnimationParams::GetSourceOutputCode\n");
	std::string bodyStr = GetBodySourceCode();

	Declaration* decl = parent->GetDeclaration(rawDataIndex);

	if (decl == nullptr)
		DeclareVar(prefix, bodyStr);
	else
		decl->text = bodyStr;

	return "";
}

void ZTextureAnimationParams::DeclareVar(const std::string& prefix,
                                         const std::string& bodyStr) const
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix, rawDataIndex);

	parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
	                       GetSourceTypeName(), auxName, bodyStr);
}

/* TextureScrollingParams */

void TextureScrollingParams::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	for (int i = 0; i < count; i++)
	{
		rows[i].xStep = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4 * i);
		rows[i].yStep = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4 * i + 1);
		rows[i].width = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4 * i + 2);
		rows[i].height = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4 * i + 3);
	}
}

void TextureScrollingParams::ExtractFromBinary(uint32_t nRawDataIndex, int nCount)
{
	rawDataIndex = nRawDataIndex;
	count = nCount;

	ParseRawData();
}

std::string TextureScrollingParams::GetSourceTypeName() const
{
	printf("TextureScrollingParams::GetSourceTypeName\n");
	return "AnimatedMatTexScrollParams";  // TODO: Better name
}

std::string TextureScrollingParams::GetDefaultName(const std::string& prefix,
                                                   uint32_t address) const
{
	return StringHelper::Sprintf("%sTexScrollParams_%06X", prefix.c_str(), address);
}

size_t TextureScrollingParams::GetRawDataSize() const
{
	return 4 * count;
}

void TextureScrollingParams::DeclareVar(const std::string& prefix,
                                         const std::string& bodyStr) const
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix, rawDataIndex);

	parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
	                            GetSourceTypeName(), auxName, count, bodyStr);
}

std::string TextureScrollingParams::GetBodySourceCode() const
{
	printf("TextureScrollingParams::GetBodySourceCode\n");

	std::string bodyStr = "\n";

	for (int i = 0; i < count; i++)
	{
		bodyStr += StringHelper::Sprintf("    { %d, %d, 0x%02X, 0x%02X },\n", rows[i].xStep, rows[i].yStep, rows[i].width, rows[i].height);

	}

	printf("bodyStr = %s", bodyStr.c_str());
	return bodyStr;
}

// TextureScrollingParamsEntry::TextureScrollingParamsEntry(const std::vector<uint8_t>& rawData,
//                                                          uint32_t rawDataIndex)
// 	: xStep(rawData.at(rawDataIndex + 0)), yStep(rawData.at(rawDataIndex + 1)),
// 	  width(rawData.at(rawDataIndex + 2)), height(rawData.at(rawDataIndex + 3))
// {
// 	printf("Asinine TextureScrollingParams constructor\n");
// 	printf("    { %d, %d, 0x%02X, 0x%02X },\n", xStep, yStep, width, height);
// }

// std::string TextureScrollingParamsEntry::GetSourceOutputCode()
// {
// 	printf("TextureScrollingParams::GetSourceOutputCode\n");
// 	printf("    { %d, %d, 0x%02X, 0x%02X },\n", xStep, yStep, width, height);
// 	// 	return StringHelper::Sprintf("    { %d, %d, 0x%02X, 0x%02X },", xStep, yStep, width,
// 	// height);
// 	return "FrankerZ,\n";
// }

// size_t TextureScrollingParamsEntry::GetEntrySize()
// {
// 	printf("TextureScrollingParamsEntry::GetEntrySize\n");
// 	return 4;
// }

/* TextureAnimationEntry */

/**
 * TextureAnimationEntry constructor
 */
TextureAnimationEntry::TextureAnimationEntry(const std::vector<uint8_t>& rawData,
                                             uint32_t rawDataIndex)
	: segment(rawData.at(rawDataIndex)),
	  type((TextureAnimationParamsType)BitConverter::ToInt16BE(rawData, rawDataIndex + 2)),
	  paramsPtr(BitConverter::ToUInt32BE(rawData, rawDataIndex + 4))
{
	printf("Dumb TextureAnimationEntry constructor\n");
	uint32_t paramsOffset = GETSEGOFFSET(paramsPtr);

	// switch (type)
	// {
	// case TextureAnimationParamsType::SingleScroll:
	// 	paramsVec.push_back(std::make_shared<TextureScrollingParams>(rawData, paramsOffset));
	// case TextureAnimationParamsType::DualScroll:
	// 	paramsVec.push_back(std::make_shared<TextureScrollingParams>(rawData, paramsOffset));
	// 	paramsVec.push_back(std::make_shared<TextureScrollingParams>(rawData, paramsOffset + 4));
	// 	break;
	// case TextureAnimationParamsType::ColorChange:
	// case TextureAnimationParamsType::ColorChangeLERP:
	// case TextureAnimationParamsType::ColorChangeLagrange:
	// 	break;
	// case TextureAnimationParamsType::TextureCycle:
	// 	break;
	// case TextureAnimationParamsType::Unknown:
	// 	printf("Warning: unknown params type in TextureAnimationEntry");
	// 	break;
	// }
	// TODO: paramsVec initialisation
}

// TextureAnimationEntry::TextureAnimationEntry(const std::vector<uint8_t>& rawData,
//                                              uint32_t rawDataIndex)
// 	: segment(rawData.at(rawDataIndex)),
// 	  type((TextureAnimationParamsType)BitConverter::ToInt16BE(rawData, rawDataIndex + 2))
// {
// 	printf("%s\n", "Constructors are stupid.");
// 	segmentAddress = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
// 	segmentOffset = GETSEGOFFSET(segmentAddress);

// 	switch (type)
// 	{
// 	case TextureAnimationParamsType::SingleScroll:
// 		params.push_back(std::make_shared<TextureScrollingParams>(rawData, segmentOffset));
// 		break;
// 	case TextureAnimationParamsType::DualScroll:
// 		params.push_back(std::make_shared<TextureScrollingParams>(rawData, segmentOffset));
// 		params.push_back(std::make_shared<TextureScrollingParams>(rawData, segmentOffset + 4));
// 		break;
// 	// case TextureAnimationParamsType::ColorChange:
// 	// case TextureAnimationParamsType::ColorChangeLERP:
// 	// case TextureAnimationParamsType::ColerChangeLagrange:
// 	// 	params = std::make_shared<TextureColorChangingParams>(rawData, segmentOffset, type);
// 	// 	break;
// 	// case TextureAnimationParamsType::TextureCycle:
// 	// 	params = std::make_shared<TextureCyclingParams>(rawData, segmentOffset);
// 	// 	break;
// 	// case TextureAnimationParamsType::Unknown:  // Some terminator when there are no animated
// 	//                                            // textures?
// 	// 	break;
// 	}
// }

/* ZTextureAnimation */

/**
 * ZTextureAnimation constructor, probably doesn't need to do anything extra
 */
ZTextureAnimation::ZTextureAnimation(ZFile* nParent) : ZResource(nParent)
{
	printf("Stupid ZTextureAnimation constructor\n");
	printf("%s\n", __PRETTY_FUNCTION__);
}

/**
 * ??? and creates a placeholder for the actual declaration to slot into later
 */
void ZTextureAnimation::ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex)
{
	ZResource::ExtractFromXML(reader, nRawDataIndex);
	DeclareVar("", "");
}

void ZTextureAnimation::ParseRawData()
{
	printf("ZTextureAnimation::ParseRawData\n");
	printf("%s\n", __PRETTY_FUNCTION__);
	ZResource::ParseRawData();

	for (uint32_t curPtr = rawDataIndex;; curPtr += 8)
	{
		printf("Parsing TextureAnimationEntry raw data\n");
		TextureAnimationEntry curEntry(parent->GetRawData(),
		                               curPtr);  // TODO: stop this allocating a whole new one every
		                                         // time: get it out of the loop somehow
		entries.push_back(curEntry);

		printf("    { %d, %X, 0x%08X },\n", curEntry.segment, curEntry.type, curEntry.paramsPtr);

		if (curEntry.segment < 0)
			break;
	}
}

void ZTextureAnimation::DeclareReferences(const std::string& prefix)
{
	printf("ZTextureAnimation::DeclareReferences\n");
	printf("%s\n", __PRETTY_FUNCTION__);

	std::string varPrefix = name;
	if (varPrefix == "")
		varPrefix = prefix;

	ZResource::DeclareReferences(varPrefix);

	for (TextureAnimationEntry& entry : entries)
	{
		if (entry.paramsPtr != 0 && GETSEGNUM(entry.paramsPtr) == parent->segment)
		{
			uint32_t paramsOffset = Seg2Filespace(entry.paramsPtr, parent->baseAddress);
			if (!parent->HasDeclaration(paramsOffset))
			{
				ZTextureAnimationParams* params;
				int count = 2;
				switch (entry.type)
				{
				case TextureAnimationParamsType::SingleScroll:
					count = 1;
				case TextureAnimationParamsType::DualScroll:
					printf("Declaring references to scrolling texture\n");

					params = new TextureScrollingParams(parent);
					params->ExtractFromBinary(paramsOffset, count);
					break;
				case TextureAnimationParamsType::ColorChange:
				case TextureAnimationParamsType::ColorChangeLERP:
				case TextureAnimationParamsType::ColorChangeLagrange:
					break;
				case TextureAnimationParamsType::TextureCycle:
					break;
				}

				params->SetName(params->GetDefaultName(varPrefix, paramsOffset));
				params->DeclareVar(varPrefix, "");
				params->DeclareReferences(varPrefix);
				parent->AddResource(params);
			}
		}
	}
}

// void ZTextureAnimation::DeclareReferences(const std::string& prefix)
// {
// 	std::string nameStr =
// 		StringHelper::Sprintf("%sAnimatedMaterialList0x%06X", prefix.c_str(), rawDataIndex);

// 	for (auto& entry : entries)
// 	{
// 		size_t declSize = 0;
// 		std::string declTypeName = "";
// 		std::string declName = StringHelper::Sprintf("%sTextureAnimationParams0x%06X",
// 		                                             prefix.c_str(), entry.segmentOffset);
// 		std::string declaration = "";
// 		size_t index = 0;

// 		switch (entry.type)
// 		{
// 		case TextureAnimationParamsType::SingleScroll:
// 		case TextureAnimationParamsType::DualScroll:
// 			declaration += entry.params.at(0)->GetSourceOutputCode();

// 			if (index < entry.params.size() - 1)
// 				declaration += "\n";

// 			index++;

// 			declSize = 4;
// 			declTypeName = "AnimatedMatTexScrollParams";

// 			printf("entry.params.size() = %d \n", entry.params.size());
// 			parent->AddDeclarationArray(entry.segmentOffset, DeclarationAlignment::Align4, declSize,
// 			                            declTypeName, declName, entry.params.size(), declaration);
// 			break;
// 		// case TextureAnimationParamsType::ColorChange:
// 		// case TextureAnimationParamsType::ColorChangeLERP:
// 		// case TextureAnimationParamsType::ColerChangeLagrange:
// 		// 	declSize = entry.params->GetParamsSize();
// 		// 	declTypeName = "AnimatedMatColorParams";
// 		// 	declaration = entry.params->GetSourceOutputCode();

// 		// 	parent->AddDeclaration(entry.segmentOffset, DeclarationAlignment::Align4, declSize,
// 		// 	                       declTypeName, declName,
// 		// 	                       StringHelper::Sprintf("\n\t%s\n", declaration.c_str()));
// 		// 	break;
// 		// case TextureAnimationParamsType::TextureCycle:
// 		// 	declSize = entry.params->GetParamsSize();
// 		// 	declTypeName = "AnimatedMatTexCycleParams";
// 		// 	declaration = entry.params->GetSourceOutputCode();

// 		// 	parent->AddDeclaration(entry.segmentOffset, DeclarationAlignment::Align4, declSize,
// 		// 	                       declTypeName, declName,
// 		// 	                       StringHelper::Sprintf("\n\t%s\n", declaration.c_str()));
// 		// 	break;
// 		// case TextureAnimationParamsType::Unknown:
// 		// 	continue;

// 		default:
// 			throw std::runtime_error(
// 				StringHelper::Sprintf("Error in ZTextureAnimation::DeclareReferences (%s)\n"
// 			                          "\t Unknown entry.type: %i\n",
// 			                          nameStr.c_str(), entry.type));
// 		}
// 	}

// 	if (!entries.empty())
// 	{
// 		std::string declaration = "";

// 		for (size_t i = 0; i < entries.size(); i++)
// 		{
// 			std::string textureName = parent->GetDeclarationPtrName(entries.at(i).segmentAddress);

// 			declaration += StringHelper::Sprintf("\t{ %2i, %2i, %s },", entries.at(i).segment,
// 			                                     entries.at(i).type, textureName.c_str());

// 			if (i + 1 < entries.size())
// 				declaration += "\n";
// 		}

// 		parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::Align4,
// 		                            DeclarationPadding::Pad16, entries.size() * 8,
// 		                            "AnimatedMaterial", nameStr, entries.size(), declaration);
// 	}
// }

/**
 * The type of the resource as a C struct
 */
std::string ZTextureAnimation::GetSourceTypeName() const
{
	printf("ZTextureAnimation::GetSourceTypeName\n");
	return "AnimatedMaterial";  // TODO: Better name
}

/**
 * The type in the ZResource enum
 */
ZResourceType ZTextureAnimation::GetResourceType() const
{
	printf("ZTextureAnimation::GetResourceType\n");
	return ZResourceType::TextureAnimation;
}

/**
 * The size of the whole data structure we're extracting? Or is it just the main array?
 */
size_t ZTextureAnimation::GetRawDataSize() const
{
	printf("ZTextureAnimation::GetRawDataSize\n");
	return entries.size() * 8;
}

/**
 * Name to use if not in the XML with a name
 */
std::string ZTextureAnimation::GetDefaultName(const std::string& prefix, uint32_t address) const
{
	printf("ZTextureAnimation::GetDefaultName\n");
	return StringHelper::Sprintf("%sTextureAnimation_%06X", prefix.c_str(), address);
}

/**
 * Constructs the output's ???
 */
void ZTextureAnimation::DeclareVar(const std::string& prefix, const std::string& bodyStr) const
{
	printf("ZTextureAnimation::DeclareVar\n");
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix, rawDataIndex);

	parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::Align8, GetRawDataSize(),
	                            GetSourceTypeName(), auxName, entries.size(), bodyStr);
}

/**
 * Sets the body of the definition (i.e. the content of the struct between the outer `{}`s)
 */
std::string ZTextureAnimation::GetBodySourceCode() const
{
	printf("ZTextureAnimation::GetBodySourceCode\n");

	std::string bodyStr = "";

	for (const TextureAnimationEntry& entry : entries)
	{
		bodyStr += StringHelper::Sprintf("    { %d, %d, 0x%08X },\n", entry.segment, entry.type,
		                                 entry.paramsPtr);
	}

	bodyStr.pop_back();

	printf("bodyStr = %s", bodyStr.c_str());
	return bodyStr;
}

/**
 * Final setup of whole definition
 */
std::string ZTextureAnimation::GetSourceOutputCode(const std::string& prefix)
{
	printf("ZTextureAnimation::GetSourceOutputCode\n");
	std::string bodyStr = GetBodySourceCode();

	Declaration* decl = parent->GetDeclaration(rawDataIndex);

	if (decl == nullptr)
		DeclareVar(prefix, bodyStr);
	else
		decl->text = bodyStr;

	return "";
}
