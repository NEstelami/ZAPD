#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include "ZFile.h"

enum class ZLimbType
{
	Standard,
	LOD,
	Skin
};

// TODO: check if more types exists
enum class ZLimbSkinType
{
	SkinType_0, // Segment = 0
	SkinType_4 = 4, // Segment = segmented address // dunno what is it // Struct_800A5E28
	SkinType_5 = 5, // Segment = 0
	SkinType_DList = 11,
};

class ZLimb : public ZResource
{
protected:
	segptr_t segAddress;
	ZLimbType type = ZLimbType::Standard;

	int16_t transX, transY, transZ;
	uint8_t childIndex, siblingIndex;
	segptr_t dListPtr = 0;

	std::vector<ZLimb*> children;

	segptr_t farDListPtr = 0; // LOD only

	ZLimbSkinType skinSegmentType = ZLimbSkinType::SkinType_0; // Skin only
	segptr_t skinSegment = 0; // Skin only


	std::string GetSourceOutputCodeSkin(const std::string& prefix);
	std::string GetSourceOutputCodeSkin_Type_4(const std::string& prefix);

	
	std::string GetSourceOutputCodeSkin_Type_4_StructA5E28_Entry(const std::string& prefix, uint32_t fileOffset, uint16_t index);
	std::string GetSourceOutputCodeSkin_Type_4_StructA57C0_Entry(const std::string& prefix, uint32_t fileOffset, uint16_t index);
	std::string GetSourceOutputCodeSkin_Type_4_StructA598C_2_Entry(const std::string& prefix, uint32_t fileOffset, uint16_t index);

public:
	ZLimb(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int rawDataIndex, ZFile* parent);
	~ZLimb();

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	static ZLimb* FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* parent);
	int GetRawDataSize() override;
	std::string GetSourceOutputCode(const std::string& prefix) override;
	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;

	ZLimbType GetLimbType();
	static const char* GetSourceTypeName(ZLimbType limbType);

	uint32_t GetFileAddress();

	// protected: // ?
	static std::string MakeLimbDListSourceOutputCode(const std::string& prefix, const std::string& limbPrefix, segptr_t dListPtr, const std::vector<uint8_t>& rawData, ZFile* parent);
};


enum ZSkeletonType
{
	Normal,
	Flex,
	Skin
};

class ZSkeleton : public ZResource
{
public:
	ZSkeletonType type = ZSkeletonType::Normal;
	ZLimbType limbType = ZLimbType::Standard;
	std::vector<ZLimb*> limbs;
	ZLimb* rootLimb;
	uint8_t dListCount; // FLEX SKELETON ONLY

	ZSkeleton();
	static ZSkeleton* FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* nParent);
	void Save(const std::string& outFolder) override;
	virtual void GenerateHLIntermediette(HLFileIntermediette& hlFile);

	int GetRawDataSize() override;
	std::string GetSourceOutputCode(const std::string& prefix) override;

	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;
};
