#pragma once

#include "ZResource.h"
#include "ZDisplayList.h"

class ExporterExample_DisplayList : public ZResourceExporter
{
public:
	virtual void Save(ZResource* res, std::string outPath, BinaryWriter* writer) override;
};