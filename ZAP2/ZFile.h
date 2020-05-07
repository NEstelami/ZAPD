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
	std::map<int32_t, Declaration*> declarations;

	ZFile(std::string nOutPath, std::string nName);
	ZFile(ZFileMode mode, tinyxml2::XMLElement* reader, std::string nBasePath, std::string nOutPath);

	void ExtractResources(std::string outputDir);
	void BuildResources();
	void AddResource(ZResource* res);

protected:
	std::vector<ZResource*> resources;
	std::string name;
	std::string basePath;
	std::string outputPath;
	std::string sourceOutput;

	ZFile();
	void ParseXML(ZFileMode mode, tinyxml2::XMLElement* reader);
	void GenerateSourceFiles(std::string outputDir);
	std::string ProcessDeclarations();
};