#pragma once

#include "ZResource.h"


class ZBetaLimb : public ZResource
{
public:
    ZBetaLimb(ZFile* nParent);

    void ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex) override;

    void ParseRawData() override;
    void DeclareReferences(const std::string& prefix) override;
    std::string GetBodySourceCode() const override;

    std::string GetSourceOutputCode(const std::string& prefix) override;

    std::string GetSourceTypeName() const override;
    ZResourceType GetResourceType() const override;

    size_t GetRawDataSize() const override;

protected:
    segptr_t dList; // Gfx*
    float transX, transY, transZ; // Vec3f
    uint16_t rotX, rotY, rotZ; // Vec3s
    segptr_t child; // BetaLimb*
    segptr_t sibling; // BetaLimb*
};

