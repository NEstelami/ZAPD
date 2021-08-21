#include "Globals.h"
#include <Utils/File.h>
#include <Utils/Path.h>
#include <algorithm>
#include "tinyxml2.h"

using namespace tinyxml2;

Globals* Globals::Instance;

Globals::Globals()
{
	Instance = this;

	files = std::vector<ZFile*>();
	segments = std::vector<int32_t>();
	game = ZGame::OOT_RETAIL;
	genSourceFile = true;
	testMode = false;
	profile = false;
	useLegacyZDList = false;
	useExternalResources = true;
	lastScene = nullptr;
	verbosity = VerbosityLevel::VERBOSITY_SILENT;
	currentExporter = "";
	outputPath = Directory::GetCurrentDirectory();
}

std::string Globals::FindSymbolSegRef(int32_t segNumber, uint32_t symbolAddress)
{
	if (cfg.segmentRefs.find(segNumber) != cfg.segmentRefs.end())
	{
		if (cfg.segmentRefFiles.find(segNumber) == cfg.segmentRefFiles.end())
		{
			XMLDocument doc;
			std::string filePath = cfg.segmentRefs[segNumber];
			XMLError eResult = doc.LoadFile(filePath.c_str());

			if (eResult != tinyxml2::XML_SUCCESS)
				return "ERROR";

			tinyxml2::XMLNode* root = doc.FirstChild();

			if (root == nullptr)
				return "ERROR";

			for (XMLElement* child = root->FirstChildElement(); child != NULL;
			     child = child->NextSiblingElement())
			{
				if (std::string(child->Name()) == "File")
				{
					ZFile* file = new ZFile(fileMode, child, "", "", filePath, true);
					file->GeneratePlaceholderDeclarations();
					cfg.segmentRefFiles[segNumber] = file;
					break;
				}
			}
		}

		return cfg.segmentRefFiles[segNumber]->GetDeclarationName(symbolAddress, "ERROR");
	}

	return "ERROR";
}

void Globals::AddSegment(int32_t segment, ZFile* file)
{
	if (std::find(segments.begin(), segments.end(), segment) == segments.end())
		segments.push_back(segment);

	cfg.segmentRefs[segment] = file->GetXmlFilePath().string();
	cfg.segmentRefFiles[segment] = file;
}

bool Globals::HasSegment(int32_t segment)
{
	return std::find(segments.begin(), segments.end(), segment) != segments.end();
}

std::map<std::string, ExporterSet*>* Globals::GetExporterMap()
{
	static std::map<std::string, ExporterSet*> exporters;
	return &exporters;
}

void Globals::AddExporter(std::string exporterName, ExporterSet* exporterSet)
{
	auto exporters = GetExporterMap();
	(*exporters)[exporterName] = exporterSet;
}

ZResourceExporter* Globals::GetExporter(ZResourceType resType)
{
	auto exporters = *GetExporterMap();

	if (currentExporter != "" && exporters[currentExporter]->exporters.find(resType) !=
	                                 exporters[currentExporter]->exporters.end())
		return exporters[currentExporter]->exporters[resType];
	else
		return nullptr;
}

ExporterSet* Globals::GetExporterSet()
{
	auto exporters = *GetExporterMap();

	if (currentExporter != "")
		return exporters[currentExporter];
	else
		return nullptr;
}