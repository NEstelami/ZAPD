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
	ofbx::IScene* scene = ofbx::load((ofbx::u8*)fileData.data(), fileData.size(), (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
	ZFile* file = new ZFile(Directory::GetCurrentDirectory() + "\\Test", "test");

	int declarationIndex = 0;

	for (int i = 0; i < scene->getMeshCount(); i++)
	{
		ZDisplayList* dList = new ZDisplayList();
		dList->SetName("cube");
		dList->parent = file;
		vector<Vertex*> dListVertices;
		
		auto mesh = scene->getMesh(i);
		auto geo = mesh->getGeometry();
		auto vertices = geo->getVertices();
		auto uvs = geo->getUVs();
		auto colors = geo->getColors();

		for (int j = 0; j < geo->getVertexCount(); j++)
		{
			Vertex* vtx = new Vertex();
			vtx->x = vertices[j].x;
			vtx->y = vertices[j].y;
			vtx->z = vertices[j].z;

			if (uvs != nullptr)
			{
				vtx->s = uvs[j].x;
				vtx->t = uvs[j].y;
			}

			if (colors != nullptr)
			{
				vtx->r = colors[j].x;
				vtx->g = colors[j].y;
				vtx->b = colors[j].z;
				vtx->a = colors[j].w;
			}

			dListVertices.push_back(vtx);
			declarationIndex += 16;

			if (dListVertices.size() >= 32)
			{
				

				dList->vertices[declarationIndex - (dListVertices.size() * 16)] = dListVertices;
				dListVertices = vector<Vertex*>();
			}
		}

		dList->vertices[declarationIndex - (dListVertices.size() * 16)] = dListVertices;

		file->AddResource(dList);
	}
	
	file->ExtractResources("Test");
}