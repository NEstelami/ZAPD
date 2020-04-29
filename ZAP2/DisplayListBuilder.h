#pragma once

#include "ZResource.h"
#include "ZTexture.h"
#include "ZDisplayList.h"
#include "tinyxml2.h"
#include "OpenFBX/ofbx.h"

#include <vector>
#include <map>
#include <string>

struct DisplayListObject
{
	std::map<std::string, std::vector<Vertex*>> vertices;
	std::map<std::string, ZTexture*> textures;
	std::map<std::string, std::string> drawCommands;
};

class DisplayListBuilder
{
public:
	ofbx::IScene* scene;
protected:
	std::vector<DisplayListObject*> objects;

};