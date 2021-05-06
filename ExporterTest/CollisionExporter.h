#pragma once

#include "ZResource.h"
#include "ZCollision.h"

class ExporterExample_Collision : public ZResourceExporter
{
public:
	virtual void Save(ZResource* res, std::filesystem::path outPath, BinaryWriter* writer) override;
};