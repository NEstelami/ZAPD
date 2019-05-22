#include "ZFile.h"
#include "Path.h"

#include <string>
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

bool Parse(string xmlFilePath, ZFileMode fileMode);

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout << "ZAP2: Zelda Asset Processor (2)" << "\n";
		cout << "Usage: ZAP2.exe [Input XML/Bin File] [mode (b/e)]" << "\n";
		return 1;
	}

	char buildMode = argv[2][0];

	if (buildMode != 'b' && buildMode != 'c')
	{
		cout << "Invalid build mode.\n";
		return 1;
	}

	Parse(argv[1], buildMode == 'b' ? ZFileMode::Build : ZFileMode::Extract);
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