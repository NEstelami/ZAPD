#include "ZTextureHeader.h"
#include "Globals.h"
#include "ZResource.h"
#include "ZFile.h"

REGISTER_ZFILENODE(TextureHeader, ZTextureHeader);

void ZTextureHeader::ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex)
{
	ZResource::ExtractFromXML(reader, nRawDataIndex);

	parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::Align4, pathways.size() * 8,
	                            GetSourceTypeName(), name, pathways.size(), "");
}