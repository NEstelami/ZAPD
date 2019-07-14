#pragma once

#include <vector>
#include <string>
#include "tinyxml2.h"
#include "ZResource.h"

enum ZFileMode
{
	Build,
	BuildTexture,
	Extract,
	Invalid
};

class ZFile
{
public:
	ZFile(ZFileMode mode, tinyxml2::XMLElement* reader, std::string nBasePath);

	void ExtractResources();
	void BuildResources();

protected:
	std::string name;
	std::string basePath;
	std::string sourceOutput;
	std::vector<ZResource*> resources;

	void ParseXML(ZFileMode mode, tinyxml2::XMLElement* reader);
	void GenerateSourceFiles();
};