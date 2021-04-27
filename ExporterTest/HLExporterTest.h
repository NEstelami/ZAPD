#pragma once

#include "ZResource.h"
#include "ZTexture.h"

class HLExporterTest_Texture : public ZResourceExporter
{
public:
	friend class ZTexture;

	// Inherited via ZResourceExporter
	virtual void Save(ZResource* res, std::string outPath) override;
};