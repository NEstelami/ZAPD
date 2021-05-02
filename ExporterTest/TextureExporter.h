#pragma once

#include "ZResource.h"
#include "ZTexture.h"
#include <BinaryWriter.h>

class ExporterExample_Texture : public ZResourceExporter
{
public:
	virtual void Save(ZResource* res, std::string outPath, BinaryWriter* writer) override;
};