#pragma once

#include <vector>
#include <string>
#include "tinyxml2.h"
#include "ZResource.h"

enum ZFileMode
{
	Build,
	BuildTexture,
	BuildOverlay,
	Extract,
	Invalid
};

class ZFile
{
public:
	ZFile(ZFileMode mode, tinyxml2::XMLElement* reader, std::string nBasePath, std::string nOutPath);

	void ExtractResources(std::string outputDir);
	void BuildResources();

protected:
	std::string name;
	std::string basePath;
	std::string outputPath;
	std::string sourceOutput;
	std::vector<ZResource*> resources;

	void ParseXML(ZFileMode mode, tinyxml2::XMLElement* reader);
	void GenerateSourceFiles(std::string outputDir);
};