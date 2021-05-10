#include "BuildInfo.h"
#include "Directory.h"
#include "File.h"
#include "Globals.h"
#include "Overlays/ZOverlay.h"
#include "Path.h"
#include "ZAnimation.h"
#include "ZBackground.h"
#include "ZBlob.h"
#include "ZFile.h"
#include "ZTexture.h"

#if !defined(_MSC_VER) && !defined(__CYGWIN__)
#include <csignal>
#include <cxxabi.h>  // for __cxa_demangle
#include <dlfcn.h>   // for dladdr
#include <execinfo.h>
#include <unistd.h>
#endif

#include <string>
#include "tinyxml2.h"
//#include <HighLevel/HLExporterTest.h>

using namespace tinyxml2;

bool Parse(const fs::path& xmlFilePath, const fs::path& basePath, const fs::path& outPath,
           ZFileMode fileMode);

void BuildAssetTexture(const fs::path& pngFilePath, TextureType texType, const fs::path& outPath);
void BuildAssetBackground(const fs::path& imageFilePath, const fs::path& outPath);
void BuildAssetBlob(const fs::path& blobFilePath, const fs::path& outPath);
void BuildAssetModelIntermediette(const fs::path& outPath);
void BuildAssetAnimationIntermediette(const fs::path& animPath, const fs::path& outPath);

#if !defined(_MSC_VER) && !defined(__CYGWIN__)
void ErrorHandler(int sig)
{
	void* array[4096];
	const size_t nMaxFrames = sizeof(array) / sizeof(array[0]);
	size_t size = backtrace(array, nMaxFrames);
	char** symbols = backtrace_symbols(array, nMaxFrames);

	fprintf(stderr, "\nZAPD crashed. (Signal: %i)\n", sig);

	fprintf(stderr, "\n\t\tYou've met with a terrible fate, haven't you?\n\n");
	/**
	 * Other possible options:
	 * - SEA BEARS FOAM. SLEEP BEARS DREAMS. \n BOTH END IN THE SAME WAY: CRASSSH!
	 */

	fprintf(stderr, "Traceback:\n");
	for (size_t i = 1; i < size; i++)
	{
		Dl_info info;
		uint32_t gotAddress = dladdr(array[i], &info);
		std::string functionName(symbols[i]);

		if (gotAddress != 0 && info.dli_sname != nullptr)
		{
			int32_t status;
			char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
			const char* nameFound = info.dli_sname;

			if (status == 0)
			{
				nameFound = demangled;
			}

			functionName = StringHelper::Sprintf("%s (+0x%X)", nameFound,
			                                     (char*)array[i] - (char*)info.dli_saddr);
			free(demangled);
		}

		fprintf(stderr, "%-3zd %s\n", i, functionName.c_str());
	}

	fprintf(stderr, "\n");

	free(symbols);
	exit(1);
}
#endif

int main(int argc, char* argv[])
{
	// Syntax: ZAPD.exe [mode (btex/bovl/e)] (Arbritrary Number of Arguments)

	if (argc < 2)
	{
		printf("ZAPD.exe (%s) [mode (btex/bovl/bsf/bblb/bmdlintr/bamnintr/e)] ...\n", gBuildHash);
		return 1;
	}

	Globals* g = new Globals();

	// Parse other "commands"
	for (int32_t i = 2; i < argc; i++)
	{
		std::string arg = argv[i];

		if (arg == "-o" || arg == "--outputpath")  // Set output path
		{
			Globals::Instance->outputPath = argv[++i];

			if (Globals::Instance->sourceOutputPath == "")
				Globals::Instance->sourceOutputPath = Globals::Instance->outputPath;
		}
		else if (arg == "-i" || arg == "--inputpath")  // Set input path
		{
			Globals::Instance->inputPath = argv[++i];
		}
		else if (arg == "-b" || arg == "--baserompath")  // Set baserom path
		{
			Globals::Instance->baseRomPath = argv[++i];
		}
		else if (arg == "-osf")  // Set source output path
		{
			Globals::Instance->sourceOutputPath = argv[++i];
		}
		else if (arg == "-gsf")  // Generate source file during extraction
		{
			Globals::Instance->genSourceFile = string(argv[++i]) == "1";
		}
		else if (arg == "-tm")  // Test Mode (enables certain experimental features)
		{
			Globals::Instance->includeFilePrefix = string(argv[++i]) == "1";
		}
		else if (arg == "-crc" ||
		         arg == "--output-crc")  // Outputs a CRC file for each extracted texture.
		{
			Globals::Instance->testMode = string(argv[++i]) == "1";
		}
		else if (arg == "-ulzdl")  // Use Legacy ZDisplay List
		{
			Globals::Instance->useLegacyZDList = string(argv[++i]) == "1";
		}
		else if (arg == "-profile")  // Enable profiling
		{
			Globals::Instance->profile = string(argv[++i]) == "1";
		}
		else if (arg ==
		         "-uer")  // Split resources into their individual components (enabled by default)
		                  // TODO: We may wish to make this a part of the config file...
		{
			Globals::Instance->useExternalResources = string(argv[++i]) == "1";
		}
		else if (arg == "-tt")  // Set texture type
		{
			Globals::Instance->texType = ZTexture::GetTextureTypeFromString(argv[++i]);
		}
		else if (arg == "-cfg")  // Set cfg path (for overlays)
		                         // TODO: Change the name of this to something else so it doesn't
		                         // get confused with XML config files.
		{
			Globals::Instance->cfgPath = argv[++i];
		}
		else if (arg == "-rconf")  // Read Config File
		{
			Globals::Instance->ReadConfigFile(argv[++i]);
		}
		else if (arg == "-eh")  // Enable Error Handler
		{
#if !defined(_MSC_VER) && !defined(__CYGWIN__)
			signal(SIGSEGV, ErrorHandler);
			signal(SIGABRT, ErrorHandler);
#else
			fprintf(stderr,
			        "Warning: Tried to set error handler, but this build lacks support for one.\n");
#endif
		}
		else if (arg == "-v")  // Verbose
		{
			Globals::Instance->verbosity = static_cast<VerbosityLevel>(strtol(argv[++i], NULL, 16));
		}
		else if (arg == "-wu" || arg == "--warn-unaccounted")  // Warn unaccounted
		{
			Globals::Instance->warnUnaccounted = true;
		}
		else if (arg == "-sexp" || arg == "--set-exporter")  // Set Current Exporter
		{
			Globals::Instance->currentExporter = argv[++i];
		}
	}

	// Parse File Mode
	ExporterSet* exporterSet = Globals::Instance->GetExporterSet();
	string buildMode = argv[1];
	ZFileMode fileMode = ZFileMode::Invalid;

	if (buildMode == "btex")
		fileMode = ZFileMode::BuildTexture;
	else if (buildMode == "bren")
		fileMode = ZFileMode::BuildBackground;
	else if (buildMode == "bovl")
		fileMode = ZFileMode::BuildOverlay;
	else if (buildMode == "bsf")
		fileMode = ZFileMode::BuildSourceFile;
	else if (buildMode == "bblb")
		fileMode = ZFileMode::BuildBlob;
	else if (buildMode == "bmdlintr")
		fileMode = ZFileMode::BuildModelIntermediette;
	else if (buildMode == "bamnintr")
		fileMode = ZFileMode::BuildAnimationIntermediette;
	else if (buildMode == "e")
		fileMode = ZFileMode::Extract;
	else if (exporterSet != nullptr && exporterSet->parseFileModeFunc != nullptr)
		exporterSet->parseFileModeFunc(buildMode, fileMode);

	if (fileMode == ZFileMode::Invalid)
	{
		printf("Error: Invalid file mode '%s'\n", buildMode.c_str());
		return 1;
	}

	// We've parsed through our commands once. If an exporter exists, it's been set by now. 
	// Now we'll parse through them again but pass them on to our exporter if one is available.

	if (exporterSet != nullptr && exporterSet->parseArgsFunc != nullptr)
	{
		for (int32_t i = 2; i < argc; i++)
			exporterSet->parseArgsFunc(argc, argv, i);
	}



	if (Globals::Instance->verbosity >= VERBOSITY_INFO)
		printf("ZAPD: Zelda Asset Processor For Decomp: %s\n", gBuildHash);

	if (fileMode == ZFileMode::Extract || fileMode == ZFileMode::BuildSourceFile)
	{
		bool procFileModeSuccess = false;

		if (exporterSet != nullptr && exporterSet->processFileModeFunc != nullptr)
			procFileModeSuccess = exporterSet->processFileModeFunc(fileMode);

		if (!procFileModeSuccess)
		{
			if (fileMode == ZFileMode::Extract || fileMode == ZFileMode::BuildSourceFile)
			{
				bool parseSuccessful =
					Parse(Globals::Instance->inputPath, Globals::Instance->baseRomPath,
						Globals::Instance->outputPath, fileMode);

				if (!parseSuccessful)
					return 1;
			}
			else if (fileMode == ZFileMode::BuildTexture)
			{
				TextureType texType = Globals::Instance->texType;

				BuildAssetTexture(Globals::Instance->inputPath, texType, Globals::Instance->outputPath);
			}
			else if (fileMode == ZFileMode::BuildBackground)
			{
				BuildAssetBackground(Globals::Instance->inputPath, Globals::Instance->outputPath);
			}
			else if (fileMode == ZFileMode::BuildBlob)
			{
				BuildAssetBlob(Globals::Instance->inputPath, Globals::Instance->outputPath);
			}
			else if (fileMode == ZFileMode::BuildOverlay)
			{
				ZOverlay* overlay =
					ZOverlay::FromBuild(Path::GetDirectoryName(Globals::Instance->inputPath.string()),
						Path::GetDirectoryName(Globals::Instance->cfgPath.string()));

				if (overlay)
					File::WriteAllText(Globals::Instance->outputPath.string(), overlay->GetSourceOutputCode(""));
			}
		}
	}
	else if (fileMode == ZFileMode::BuildTexture)
	{
		TextureType texType = Globals::Instance->texType;
		BuildAssetTexture(Globals::Instance->inputPath, texType, Globals::Instance->outputPath);
	}
	else if (fileMode == ZFileMode::BuildBackground)
	{
		BuildAssetBackground(Globals::Instance->inputPath, Globals::Instance->outputPath);
	}
	else if (fileMode == ZFileMode::BuildBlob)
	{
		BuildAssetBlob(Globals::Instance->inputPath, Globals::Instance->outputPath);
	}
	else if (fileMode == ZFileMode::BuildModelIntermediette)
	{
		BuildAssetModelIntermediette(Globals::Instance->outputPath);
	}
	else if (fileMode == ZFileMode::BuildAnimationIntermediette)
	{
		BuildAssetAnimationIntermediette(Globals::Instance->inputPath,
		                                 Globals::Instance->outputPath);
	}
	else if (fileMode == ZFileMode::BuildOverlay)
	{
		ZOverlay* overlay =
			ZOverlay::FromBuild(Path::GetDirectoryName(Globals::Instance->inputPath),
		                        Path::GetDirectoryName(Globals::Instance->cfgPath));

		if (overlay)
			File::WriteAllText(Globals::Instance->outputPath.string(),
			                   overlay->GetSourceOutputCode(""));
	}

	delete g;
	return 0;
}

bool Parse(const fs::path& xmlFilePath, const fs::path& basePath, const fs::path& outPath,
           ZFileMode fileMode)
{
	XMLDocument doc;
	XMLError eResult = doc.LoadFile(xmlFilePath.string().c_str());

	if (eResult != tinyxml2::XML_SUCCESS)
	{
		fprintf(stderr, "Invalid xml file: '%s'\n", xmlFilePath.c_str());
		return false;
	}

	XMLNode* root = doc.FirstChild();

	if (root == nullptr)
	{
		fprintf(stderr, "Missing Root tag in xml file: '%s'\n", xmlFilePath.c_str());
		return false;
	}

	for (XMLElement* child = root->FirstChildElement(); child != NULL;
	     child = child->NextSiblingElement())
	{
		if (std::string(child->Name()) == "File")
		{
			ZFile* file = new ZFile(fileMode, child, basePath, outPath, "", xmlFilePath, false);
			Globals::Instance->files.push_back(file);
		}
		else
		{
			throw std::runtime_error(
				StringHelper::Sprintf("Parse: Fatal error in '%s'.\n\t Found a resource outside of "
			                          "a File element: '%s'\n",
			                          xmlFilePath.c_str(), child->Name()));
		}
	}

	for (ZFile* file : Globals::Instance->files)
	{
		if (fileMode == ZFileMode::BuildSourceFile)
			file->BuildSourceFile(outPath);
		else
			file->ExtractResources(outPath);
	}

	// All done, free files
	for (ZFile* file : Globals::Instance->files)
		delete file;

	Globals::Instance->files.clear();

	return true;
}

void BuildAssetTexture(const fs::path& pngFilePath, TextureType texType, const fs::path& outPath)
{
	std::string name = outPath.stem().string();

	ZTexture tex(nullptr);
	tex.FromPNG(pngFilePath, texType);
	std::string cfgPath = StringHelper::Split(pngFilePath.string(), ".")[0] + ".cfg";

	if (File::Exists(cfgPath))
		name = File::ReadAllText(cfgPath);

	std::string src = tex.GetBodySourceCode();

	File::WriteAllText(outPath.string(), src);
}

void BuildAssetBackground(const fs::path& imageFilePath, const fs::path& outPath)
{
	ZBackground background(nullptr);
	background.ParseBinaryFile(imageFilePath.string(), false);

	File::WriteAllText(outPath.string(), background.GetBodySourceCode());
}

void BuildAssetBlob(const fs::path& blobFilePath, const fs::path& outPath)
{
	ZBlob* blob = ZBlob::FromFile(blobFilePath.string());
	std::string name = outPath.stem().string();  // filename without extension

	std::string src = blob->GetSourceOutputCode(name);

	File::WriteAllText(outPath.string(), src);

	delete blob;
}
