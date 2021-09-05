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
ZTextureAnimationParams::ZTextureAnimationParams(ZFile* parent) : ZResource::ZResource(parent)
{
}
TextureScrollingParams::TextureScrollingParams(ZFile* parent)
	: ZTextureAnimationParams::ZTextureAnimationParams(parent)
{
}
TextureColorChangingParams::TextureColorChangingParams(ZFile* parent)
	: ZTextureAnimationParams::ZTextureAnimationParams(parent)
{
}
TextureCyclingParams::TextureCyclingParams(ZFile* parent)
	: ZTextureAnimationParams::ZTextureAnimationParams(parent)
{
}

void ZTextureAnimationParams::ExtractFromBinary(uint32_t paramsOffset)
{
}
void ZTextureAnimationParams::ExtractFromBinary(uint32_t paramsOffset, int count)
{
}
std::string ZTextureAnimationParams::GetDefaultName(const std::string& prefix,
                                                    uint32_t address) const
{
}

/* TextureAnimationParams */

ZResourceType ZTextureAnimationParams::GetResourceType() const
{
	return ZResourceType::TextureAnimationParams;
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
	printf("TextureScrollingParams::GetDefaultName");
	return StringHelper::Sprintf("%sTexScrollParams_%06X", prefix.c_str(), address);
}

size_t TextureScrollingParams::GetRawDataSize() const
{
	printf("TextureScrollingParams::GetRawDataSize");
	return 4 * count;
}

void TextureScrollingParams::DeclareVar(const std::string& prefix, const std::string& bodyStr) const
{
	printf("TextureScrollingParams::DeclareVar");
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix, rawDataIndex);

	parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
	                            GetSourceTypeName(), auxName, count, bodyStr);
}

std::string TextureScrollingParams::GetBodySourceCode() const
{
	printf("TextureScrollingParams::GetBodySourceCode\n");

	std::string bodyStr = "";

	for (int i = 0; i < count; i++)
	{
		bodyStr += StringHelper::Sprintf("    { %d, %d, 0x%02X, 0x%02X },\n", rows[i].xStep,
		                                 rows[i].yStep, rows[i].width, rows[i].height);
	}

	bodyStr.pop_back();

	printf("bodyStr = %s", bodyStr.c_str());
	return bodyStr;
}

/* TextureColorChangingParams */

void TextureColorChangingParams::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	count1 = BitConverter::ToUInt16BE(rawData, rawDataIndex);
	count2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	uint16_t listLength = ((type == TextureAnimationParamsType::ColorChange) ? count1 : count2);

	if (listLength == 0)
		throw std::runtime_error(
			"TextureColorChangingParams::ParseRawData: error: color list length cannot be 0");

	primColorListAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
	envColorListAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
	frameDataListAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0xC);

	uint32_t primColorListOffset = GETSEGOFFSET(primColorListAddress);
	uint32_t envColorListOffset = GETSEGOFFSET(envColorListAddress);
	uint32_t frameDataListOffset = GETSEGOFFSET(frameDataListAddress);

	uint32_t currentPtr;

	printf("cycleLength: %d\n", listLength);

	F3DPrimColor currentPrimColor;

	for (currentPtr = primColorListOffset; currentPtr < primColorListOffset + 5 * listLength;
	     currentPtr += 5)
	{
		currentPrimColor = {BitConverter::ToUInt8BE(rawData, currentPtr),
		                    BitConverter::ToUInt8BE(rawData, currentPtr + 1),
		                    BitConverter::ToUInt8BE(rawData, currentPtr + 2),
		                    BitConverter::ToUInt8BE(rawData, currentPtr + 3),
		                    BitConverter::ToUInt8BE(rawData, currentPtr + 4)};
		primColorList.push_back(currentPrimColor);
	}

	F3DEnvColor currentEnvColor;

	for (currentPtr = envColorListOffset; currentPtr < envColorListOffset + 4 * listLength;
	     currentPtr += 4)
	{
		currentEnvColor = {BitConverter::ToUInt8BE(rawData, currentPtr),
		                   BitConverter::ToUInt8BE(rawData, currentPtr + 1),
		                   BitConverter::ToUInt8BE(rawData, currentPtr + 2),
		                   BitConverter::ToUInt8BE(rawData, currentPtr + 3)};
		envColorList.push_back(currentEnvColor);
	}

	uint16_t currentFrameData;

	for (currentPtr = frameDataListOffset; currentPtr < frameDataListOffset + 2 * listLength;
	     currentPtr += 2)
	{
		currentFrameData = BitConverter::ToUInt16BE(rawData, currentPtr);
		frameDataList.push_back(currentFrameData);
	}
}

// TODO: Consider putting this in the parent class, since it's not doing anything specific here
void TextureColorChangingParams::ExtractFromBinary(uint32_t nRawDataIndex)
{
	rawDataIndex = nRawDataIndex;

	ParseRawData();
}

std::string TextureColorChangingParams::GetSourceTypeName() const
{
	printf("TextureColorChangingParams::GetSourceTypeName\n");
	return "AnimatedMatColorParams";  // TODO: Better name
}

std::string TextureColorChangingParams::GetDefaultName(const std::string& prefix,
                                                       uint32_t address) const
{
	printf("TextureColorChangingParams::GetDefaultName\n");
	return StringHelper::Sprintf("%sColorParams_%06X", prefix.c_str(), address);
}

size_t TextureColorChangingParams::GetRawDataSize() const
{
	printf("TextureColorChangingParams::GetRawDataSize\n");
	return 0x10;
}

void TextureColorChangingParams::DeclareReferences(const std::string& prefix)
{
	printf("TextureColorChangingParams::DeclareReferences\n");

	std::string primColorBodyStr = "";

	for (auto color : primColorList)
	{
		primColorBodyStr += StringHelper::Sprintf("    { %d, %d, %d, %d, %d },\n", color.r, color.g,
		                                          color.b, color.a, color.lodFrac);
	}

	primColorBodyStr.pop_back();

	parent->AddDeclarationArray(GETSEGOFFSET(primColorListAddress), DeclarationAlignment::None,
	                            primColorList.size() * 5, "F3DPrimColor",
	                            StringHelper::Sprintf("%s_TexColorChangingPrimColors_%06X",
	                                                  parent->GetName().c_str(),
	                                                  GETSEGOFFSET(primColorListAddress)),
	                            primColorList.size(), primColorBodyStr);

	std::string envColorBodyStr = "";

	for (auto color : envColorList)
	{
		envColorBodyStr +=
			StringHelper::Sprintf("    { %d, %d, %d, %d, %d },\n", color.r, color.g, color.b, color.a);
	}

	envColorBodyStr.pop_back();

	parent->AddDeclarationArray(GETSEGOFFSET(envColorListAddress), DeclarationAlignment::None,
	                            envColorList.size() * 4, "F3DPrimColor",
	                            StringHelper::Sprintf("%s_TexColorChangingEnvColors_%06X",
	                                                  parent->GetName().c_str(),
	                                                  GETSEGOFFSET(envColorListAddress)),
	                            envColorList.size(), envColorBodyStr);

	std::string frameDataBodyStr = "    ";

	for (auto frame : frameDataList)
	{
		frameDataBodyStr += StringHelper::Sprintf("%d, ", frame);
	}

	frameDataBodyStr.pop_back();

	parent->AddDeclarationArray(GETSEGOFFSET(frameDataListAddress), DeclarationAlignment::None,
	                            frameDataList.size() * 2, "u16",
	                            StringHelper::Sprintf("%s_TexColorChangingFrameData_%06X",
	                                                  parent->GetName().c_str(),
	                                                  GETSEGOFFSET(frameDataListAddress)),
	                            frameDataList.size(), frameDataBodyStr);
}

std::string TextureColorChangingParams::GetBodySourceCode() const
{
	printf("TextureColorChangingParams::GetBodySourceCode\n");

	std::string bodyStr =
		StringHelper::Sprintf("\n    %d, %d, %s, %s, %s,\n", count1, count2,
	                          parent->GetDeclarationPtrName(primColorListAddress).c_str(),
	                          parent->GetDeclarationPtrName(envColorListAddress).c_str(),
	                          parent->GetDeclarationPtrName(frameDataListAddress).c_str());

	printf("bodyStr = %s", bodyStr.c_str());
	return bodyStr;
}

/* TextureCyclingParams */

void TextureCyclingParams::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	cycleLength = BitConverter::ToUInt16BE(rawData, rawDataIndex);
	if (cycleLength == 0)
		throw std::runtime_error(
			"TextureCyclingParams::ParseRawData: error: cycleLength cannot be 0");

	textureListAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
	textureIndexListAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);

	uint32_t textureListOffset = GETSEGOFFSET(textureListAddress);
	uint32_t textureIndexListOffset = GETSEGOFFSET(textureIndexListAddress);

	uint32_t currentPtr = textureIndexListOffset;

	uint8_t currentIndex;
	uint8_t maxIndex = 0;

	printf("cycleLength: %d\n", cycleLength);

	for (; currentPtr < textureIndexListOffset + cycleLength; currentPtr++)
	{
		currentIndex = BitConverter::ToUInt8BE(rawData, currentPtr);
		printf("currentPtr: 0x%X\n", currentPtr);
		printf("currentIndex: %d\n", currentIndex);
		textureIndexList.push_back(currentIndex);
		if (currentIndex > maxIndex)
			maxIndex = currentIndex;
	}

	for (currentPtr = textureListOffset; currentPtr <= textureListOffset + 4 * maxIndex;
	     currentPtr += 4)
	{
		textureList.push_back(BitConverter::ToUInt32BE(rawData, currentPtr));
	}
}

void TextureCyclingParams::ExtractFromBinary(uint32_t nRawDataIndex)
{
	rawDataIndex = nRawDataIndex;

	ParseRawData();
}

std::string TextureCyclingParams::GetSourceTypeName() const
{
	printf("TextureCyclingParams::GetSourceTypeName\n");
	return "AnimatedMatTexCycleParams";  // TODO: Better name
}

std::string TextureCyclingParams::GetDefaultName(const std::string& prefix, uint32_t address) const
{
	printf("TextureCyclingParams::GetDefaultName\n");
	return StringHelper::Sprintf("%sTexCycleParams_%06X", prefix.c_str(), address);
}

size_t TextureCyclingParams::GetRawDataSize() const
{
	printf("TextureCyclingParams::GetRawDataSize\n");
	return 0xC;
}

void TextureCyclingParams::DeclareReferences(const std::string& prefix)
{
	printf("TextureCyclingParams::DeclareReferences\n");

	std::string texturesbodyStr = "";

	for (auto tex : textureList)
	{
		texturesbodyStr +=
			StringHelper::Sprintf("    %s,\n", parent->GetDeclarationPtrName(tex).c_str());
	}

	texturesbodyStr.pop_back();

	printf("Declaring texture pointer array\n");

	// N.B. currently need these textures to be declared separately to get the proper pointers (will
	// print segmented addresses without). Can we search the segments in DLists to find the formats
	// and declare the textures automatically?
	parent->AddDeclarationArray(GETSEGOFFSET(textureListAddress), DeclarationAlignment::Align4,
	                            textureList.size() * 4, "TexturePtr",
	                            StringHelper::Sprintf("%s_TexCycleTexPtrs_%06X",
	                                                  parent->GetName().c_str(),
	                                                  GETSEGOFFSET(textureListAddress)),
	                            textureList.size(), texturesbodyStr);

	std::string indicesbodyStr = "    ";

	for (uint8_t index : textureIndexList)
	{
		indicesbodyStr += StringHelper::Sprintf("%d, ", index);
	}

	indicesbodyStr.pop_back();

	printf("Declaring texture index array\n");
	parent->AddDeclarationArray(GETSEGOFFSET(textureIndexListAddress), DeclarationAlignment::None,
	                            textureIndexList.size(), "u8",
	                            StringHelper::Sprintf("%s_TexCycleTexIndices_%06X",
	                                                  parent->GetName().c_str(),
	                                                  GETSEGOFFSET(textureIndexListAddress)),
	                            textureIndexList.size(), indicesbodyStr);
}

// Should be unnecessary since the same as the inherited version
// void TextureCyclingParams::DeclareVar(const std::string& prefix, const std::string& bodyStr)
// const
// {
// 	printf("TextureCyclingParams::DeclareVar");
// 	std::string auxName = name;

// 	if (name == "")
// 		auxName = GetDefaultName(prefix, rawDataIndex);

// 	parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
// 	                       GetSourceTypeName(), auxName, bodyStr);
// }

std::string TextureCyclingParams::GetBodySourceCode() const
{
	printf("TextureCyclingParams::GetBodySourceCode\n");

	std::string bodyStr =
		StringHelper::Sprintf("\n    %d, %s, %s,\n", cycleLength,
	                          parent->GetDeclarationPtrName(textureListAddress).c_str(),
	                          parent->GetDeclarationPtrName(textureIndexListAddress).c_str());

	printf("bodyStr = %s", bodyStr.c_str());
	return bodyStr;
}

/* TextureAnimationEntry */

// TODO: turn this into an ordinary struct: no need for a constructor
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
}

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

		if (curEntry.segment <= 0)
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
					printf("Declaring references to texture scrolling\n");

					params = new TextureScrollingParams(parent);
					params->ExtractFromBinary(paramsOffset, count);
					break;

				case TextureAnimationParamsType::ColorChange:
				case TextureAnimationParamsType::ColorChangeLERP:
				case TextureAnimationParamsType::ColorChangeLagrange:
					printf("Declaring references to texture color changing\n");
					params = new TextureColorChangingParams(parent);
					params->type = entry.type;
					params->ExtractFromBinary(paramsOffset);
					break;

				case TextureAnimationParamsType::TextureCycle:
					printf("Declaring references to texture cycling\n");
					params = new TextureCyclingParams(parent);
					params->ExtractFromBinary(paramsOffset);
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
		bodyStr += StringHelper::Sprintf("    { %d, %d, %s },\n", entry.segment, entry.type,
		                                 parent->GetDeclarationPtrName(entry.paramsPtr).c_str());
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
