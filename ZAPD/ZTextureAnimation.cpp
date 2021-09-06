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

ZResourceType ZTextureAnimationParams::GetResourceType() const
{
	return ZResourceType::TextureAnimationParams;
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
	return StringHelper::Sprintf("%s_TexScrollParams_%06X", prefix.c_str(), address);
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

	uint32_t primColorListOffset = Seg2Filespace(primColorListAddress, 0);
	uint32_t envColorListOffset = Seg2Filespace(envColorListAddress, 0);
	uint32_t frameDataListOffset = Seg2Filespace(frameDataListAddress, 0);

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

	if (primColorListAddress != 0)  // NULL
	{
		std::string primColorBodyStr = "";

		for (auto color : primColorList)
		{
			primColorBodyStr += StringHelper::Sprintf("    { %d, %d, %d, %d, %d },\n", color.r,
			                                          color.g, color.b, color.a, color.lodFrac);
		}

		primColorBodyStr.pop_back();

		parent->AddDeclarationArray(
			Seg2Filespace(primColorListAddress, 0), DeclarationAlignment::None,
			primColorList.size() * 5, "F3DPrimColor",
			StringHelper::Sprintf("%s_TexColorChangingPrimColors_%06X", parent->GetName().c_str(),
		                          Seg2Filespace(primColorListAddress, 0)),
			primColorList.size(), primColorBodyStr);
	}

	if (envColorListAddress != 0)  // NULL
	{
		std::string envColorBodyStr = "";

		for (auto color : envColorList)
		{
			envColorBodyStr += StringHelper::Sprintf("    { %d, %d, %d, %d },\n", color.r, color.g,
			                                         color.b, color.a);
		}

		envColorBodyStr.pop_back();

		parent->AddDeclarationArray(
			Seg2Filespace(envColorListAddress, 0), DeclarationAlignment::None,
			envColorList.size() * 4, "F3DEnvColor",
			StringHelper::Sprintf("%s_TexColorChangingEnvColors_%06X", parent->GetName().c_str(),
		                          Seg2Filespace(envColorListAddress, 0)),
			envColorList.size(), envColorBodyStr);
	}

	if (frameDataListAddress != 0)  // NULL
	{
		std::string frameDataBodyStr = "    ";

		for (auto frame : frameDataList)
		{
			frameDataBodyStr += StringHelper::Sprintf("%d, ", frame);
		}

		frameDataBodyStr.pop_back();

		parent->AddDeclarationArray(Seg2Filespace(frameDataListAddress, 0),
		                            DeclarationAlignment::None, frameDataList.size() * 2, "u16",
		                            StringHelper::Sprintf("%s_TexColorChangingFrameData_%06X",
		                                                  parent->GetName().c_str(),
		                                                  Seg2Filespace(frameDataListAddress, 0)),
		                            frameDataList.size(), frameDataBodyStr);
	}
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

	uint32_t textureListOffset = Seg2Filespace(textureListAddress, 0);
	uint32_t textureIndexListOffset = Seg2Filespace(textureIndexListAddress, 0);

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

	if (textureListAddress != 0)  // NULL
	{
		std::string texturesbodyStr = "";

		for (auto tex : textureList)
		{
			texturesbodyStr +=
				StringHelper::Sprintf("    %s,\n", parent->GetDeclarationPtrName(tex).c_str());
		}

		texturesbodyStr.pop_back();

		printf("Declaring texture pointer array\n");

		// N.B. currently need these textures to be declared separately to get the proper pointers
		// (will print segmented addresses without). Can we search the segments in DLists to find
		// the formats and declare the textures automatically?
		parent->AddDeclarationArray(
			Seg2Filespace(textureListAddress, 0), DeclarationAlignment::Align4,
			textureList.size() * 4, "TexturePtr",
			StringHelper::Sprintf("%s_TexCycleTexPtrs_%06X", parent->GetName().c_str(),
		                          Seg2Filespace(textureListAddress, 0)),
			textureList.size(), texturesbodyStr);
	}

	if (textureIndexListAddress != 0)  // NULL
	{
		std::string indicesbodyStr = "    ";

		for (uint8_t index : textureIndexList)
		{
			indicesbodyStr += StringHelper::Sprintf("%d, ", index);
		}

		indicesbodyStr.pop_back();

		printf("Declaring texture index array\n");
		parent->AddDeclarationArray(
			Seg2Filespace(textureIndexListAddress, 0), DeclarationAlignment::None,
			textureIndexList.size(), "u8",
			StringHelper::Sprintf("%s_TexCycleTexIndices_%06X", parent->GetName().c_str(),
		                          Seg2Filespace(textureIndexListAddress, 0)),
			textureIndexList.size(), indicesbodyStr);
	}
}

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

	TextureAnimationEntry currentEntry;
	auto rawData = parent->GetRawData();
	int16_t type;

	for (uint32_t curPtr = rawDataIndex;; curPtr += 8)
	{
		printf("Parsing TextureAnimationEntry raw data\n");
		type = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
		if ((type < 0) || (type > 5))
			throw std::runtime_error("error: unknown TextureAnimationParams type");

		currentEntry.segment = BitConverter::ToInt8BE(rawData, rawDataIndex);
		currentEntry.type = (TextureAnimationParamsType)type;
		currentEntry.paramsPtr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
		entries.push_back(currentEntry);

		printf("    { %d, %X, 0x%08X },\n", currentEntry.segment, currentEntry.type,
		       currentEntry.paramsPtr);

		if (currentEntry.segment <= 0)
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
