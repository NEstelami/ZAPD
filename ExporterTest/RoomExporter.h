#pragma once

#include "ZResource.h"
#include "ZRoom/ZRoom.h"

class ExporterExample_Room : public ZResourceExporter
{
public:
	virtual void Save(ZResource* res, std::filesystem::path outPath, BinaryWriter* writer) override;
};