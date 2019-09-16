#include "ZFile.h"
#include "ZTexture.h"
#include "Overlays/ZOverlay.h"
#include "Path.h"
#include "File.h"

#include <string>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

bool Parse(string xmlFilePath, ZFileMode fileMode);
void BuildAssetTexture(string pngFilePath, TextureType texType, string outPath);

int main(int argc, char* argv[])
{
	if (argc != 3 && argc != 4 && argc != 5)
	{
		cout << "ZAP2: Zelda Asset Processor (2)" << "\n";
		cout << "Usage: ZAP2.exe [mode (b/btex/bovl/e)] [Input XML/Texture File/Bin File] " << "\n";
		return 1;
	}

	//char buildMode = argv[2][0];
	string buildMode = argv[1];
	ZFileMode fileMode = ZFileMode::Invalid;

	if (buildMode == "b")
		fileMode = ZFileMode::Build;
	else if (buildMode == "btex")
		fileMode = ZFileMode::BuildTexture;
	else if (buildMode == "bovl")
		fileMode = ZFileMode::BuildOverlay;
	else
		fileMode = ZFileMode::Extract;

	if (fileMode == ZFileMode::Invalid)
	{
		cout << "Invalid build mode.\n";
		return 1;
	}

	if (fileMode == ZFileMode::Build || fileMode == ZFileMode::Extract)
		Parse(argv[2], fileMode);
	else if (fileMode == ZFileMode::BuildTexture)
	{
		// Syntax: ZAP2.exe btex [texType] [pngFilePath] [outFilePath]
		string texTypeStr = argv[2];
		string pngFilePath = argv[3];
		string outFilePath = argv[4];

		TextureType texType = ZTexture::GetTextureTypeFromString(texTypeStr);

		BuildAssetTexture(pngFilePath, texType, outFilePath);
	}
	else if (fileMode == ZFileMode::BuildOverlay)
	{
		// Syntax: ZAP2.exe bovl [elfFilePath] [cfgFolderPath] [outputFilePath]
		ZOverlay* overlay = ZOverlay::FromELF(argv[2], Path::GetDirectoryName(argv[3]));
		string source = overlay->GetSourceOutputCode();
		File::WriteAllText(argv[4], source);
	}

	//Parse(argv[1], buildMode == 'b' ? ZFileMode::Build : ZFileMode::Extract);
	
	return 0;
}

bool Parse(string xmlFilePath, ZFileMode fileMode)
{
	XMLDocument doc;

	XMLError eResult = doc.LoadFile(xmlFilePath.c_str());

	if (eResult != tinyxml2::XML_SUCCESS)
		return false;

	XMLNode* root = doc.FirstChild();

	if (root == nullptr)
		return false;

	for (XMLElement* child = root->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
	{
		if (string(child->Name()) == "File")
		{
			ZFile* file = new ZFile(fileMode, child, Path::GetDirectoryName(xmlFilePath));

			if (fileMode == ZFileMode::Build)
				file->BuildResources();
			else
				file->ExtractResources();
		}
	}

	XMLElement* element = root->FirstChildElement("File");

	if (element == nullptr)
		return false;

}

void BuildAssetTexture(string pngFilePath, TextureType texType, string outPath)
{
	ZTexture* tex = ZTexture::FromPNG(pngFilePath, texType);
	File::WriteAllBytes(outPath, tex->GetRawData());
	
	delete tex;
}