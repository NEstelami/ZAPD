#pragma once

#include <vector>
#include <string>
#include "tinyxml2.h"
#include "ZResource.h"

enum ZFileMode
{
	Build,
	Extract
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
	std::vector<ZResource*> resources;

	void ParseXML(ZFileMode mode, tinyxml2::XMLElement* reader);
};