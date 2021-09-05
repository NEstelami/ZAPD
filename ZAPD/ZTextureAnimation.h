#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "ZResource.h"
#include "tinyxml2.h"

typedef enum class TextureAnimationParamsType
{
	SingleScroll,
	DualScroll,
	ColorChange,
	ColorChangeLERP,
	ColorChangeLagrange,
	TextureCycle,
	Unknown
} TextureAnimationParamsType;

struct ZTextureAnimationParams : public ZResource
{
	ZTextureAnimationParams(ZFile* parent);

	virtual void ExtractFromBinary(uint32_t paramsOffset);
	virtual void ExtractFromBinary(uint32_t paramsOffset, int count);
	
	std::string GetSourceOutputCode(const std::string& prefix);

	virtual std::string GetDefaultName(const std::string& prefix, uint32_t address) const;
	virtual void DeclareVar(const std::string& prefix, const std::string& bodyStr) const;
	ZResourceType GetResourceType() const;
};

struct TextureScrollingParamsEntry
{
	int8_t xStep;
	int8_t yStep;
	uint8_t width;
	uint8_t height;
};

struct TextureScrollingParams : public ZTextureAnimationParams
{
	TextureScrollingParams(ZFile* parent);

	void ParseRawData() override;
	void ExtractFromBinary(uint32_t nRawDataIndex, int count) override;

	std::string GetSourceTypeName() const override;
	std::string GetDefaultName(const std::string& prefix, uint32_t address) const override;
	size_t GetRawDataSize() const override;

	void DeclareVar(const std::string& prefix,
                                         const std::string& bodyStr) const override;
	std::string GetBodySourceCode() const override;

	int count; // 1 for Single, 2 for Dual
	TextureScrollingParamsEntry rows[2];
};





// class F3DPrimColor
// {
// public:
// 	F3DPrimColor(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

// 	uint8_t r;
// 	uint8_t g;
// 	uint8_t b;
// 	uint8_t a;
// 	uint8_t lodFrac;
// };

// class F3DEnvColor
// {
// public:
// 	F3DEnvColor(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

// 	uint8_t r;
// 	uint8_t g;
// 	uint8_t b;
// 	uint8_t a;
// };

// class TextureColorChangingParams : public TextureAnimationParams
// {
// public:
// 	TextureColorChangingParams(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex, int32_t type);
// 	std::string GetSourceOutputCode() override;
// 	size_t GetParamsSize() override;

// 	uint16_t cycleLength;
// 	uint16_t numKeyFrames;

// 	segptr_t primColorSegmentAddr;
// 	segptr_t envColorSegmentAddr;
// 	segptr_t keyFrameSegmentAddr;

// 	uint32_t primColorSegmentOffset;
// 	uint32_t envColorSegmentOffset;
// 	uint32_t keyFrameSegmentOffset;

// 	std::vector<F3DPrimColor> primColors;
// 	std::vector<F3DEnvColor> envColors;
// 	std::vector<uint16_t> keyFrames;
// };

// class TextureCyclingParams : public TextureAnimationParams
// {
// public:
// 	TextureCyclingParams(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
// 	std::string GetSourceOutputCode() override;
// 	size_t GetParamsSize() override;

// 	uint16_t cycleLength;

// 	segptr_t textureSegmentOffsetsSegmentAddress;
// 	segptr_t textureIndicesSegmentAddress;
// 	uint32_t textureSegmentOffsetsSegmentOffset;
// 	uint32_t textureIndicesSegmentOffset;

// 	std::vector<uint32_t> textureSegmentOffsets;
// 	std::vector<uint8_t> textureIndices;
// };


struct TextureCyclingParams : public ZTextureAnimationParams
{

	TextureCyclingParams(ZFile* parent);

	void ParseRawData() override;
	void ExtractFromBinary(uint32_t nRawDataIndex) override;

	std::string GetSourceTypeName() const override;
	std::string GetDefaultName(const std::string& prefix, uint32_t address) const override;
	size_t GetRawDataSize() const override;

	void DeclareReferences(const std::string& prefix) override;

	// void DeclareVar(const std::string& prefix,
    //                                      const std::string& bodyStr) const override;
	std::string GetBodySourceCode() const override;

	uint16_t cycleLength;
	segptr_t textureListAddress;
	segptr_t textureIndexListAddress;
	std::vector<segptr_t> textureList;
	std::vector<uint8_t> textureIndexList;
};


class TextureAnimationEntry
{
public:
	/* void */ TextureAnimationEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	// segptr_t segmentAddress;
	// uint32_t segmentOffset;

	int8_t segment;
	TextureAnimationParamsType type;
	segptr_t paramsPtr;
	std::vector<std::shared_ptr<ZTextureAnimationParams>> paramsVec; // Not convinced we want this now
};

class ZTextureAnimation : public ZResource
{
public:
	ZTextureAnimation(ZFile* nParent);

	void ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex) override;
	void ParseRawData() override;

	void DeclareReferences(const std::string& prefix) override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;
	size_t GetRawDataSize() const override;
	std::string GetDefaultName(const std::string& prefix, uint32_t address) const;

	void DeclareVar(const std::string& prefix, const std::string& bodyStr) const;
	std::string GetBodySourceCode() const override;
	std::string GetSourceOutputCode(const std::string& prefix) override;



private:
	std::vector<TextureAnimationEntry> entries;
};