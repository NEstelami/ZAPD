#include "OpenFBX/ofbx.h"
#include "File.h"
#include "Directory.h"
#include "Path.h"
#include "ZFile.h"
#include "ZDisplayList.h"
#include <stdio.h>

using namespace std;

void ModelTest()
{
	auto fileData = File::ReadAllBytes("test.fbx");
	ofbx::IScene* scene = ofbx::load((ofbx::u8*)fileData.data(), (int)fileData.size(), (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
	ZFile* file = new ZFile(Directory::GetCurrentDirectory() + "\\Test", "test");

	int declarationIndex = 0;

	for (int i = 0; i < scene->getMeshCount(); i++)
	{
		ZDisplayList* dList = new ZDisplayList();
		dList->SetName("cube");
		dList->parent = file;
		vector<Vertex> dListVertices;
		
		auto mesh = scene->getMesh(i);
		auto geo = mesh->getGeometry();
		auto vertices = geo->getVertices();
		auto uvs = geo->getUVs();
		auto colors = geo->getColors();

		for (int j = 0; j < geo->getVertexCount(); j++)
		{
			Vertex vtx = Vertex();
			vtx.x = (int16_t)vertices[j].x;
			vtx.y = (int16_t)vertices[j].y;
			vtx.z = (int16_t)vertices[j].z;

			if (uvs != nullptr)
			{
				vtx.s = (int16_t)uvs[j].x;
				vtx.t = (int16_t)uvs[j].y;
			}

			if (colors != nullptr)
			{
				vtx.r = (uint8_t)(colors[j].x / 255.0f);
				vtx.g = (uint8_t)(colors[j].y / 255.0f);
				vtx.b = (uint8_t)(colors[j].z / 255.0f);
				vtx.a = (uint8_t)(colors[j].w / 255.0f);
			}

			dListVertices.push_back(vtx);
			declarationIndex += 16;

			if (dListVertices.size() >= 32)
			{
				dList->vertices[declarationIndex - (dListVertices.size() * 16)] = dListVertices;
				dListVertices = vector<Vertex>();
			}
		}

		dList->vertices[declarationIndex - (dListVertices.size() * 16)] = dListVertices;

		file->AddResource(dList);
	}
	
	file->ExtractResources("Test");
}