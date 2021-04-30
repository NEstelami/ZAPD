#include <Windows.h>
#include <TextureExporter.h>
#include <RoomExporter.h>
#include <CollisionExporter.h>
#include <DisplayListExporter.h>
#include <Globals.h>

void ImportExporters()
{
	Globals::AddExporter("EXAMPLE", ZResourceType::Texture, new ExporterExample_Texture());
	Globals::AddExporter("EXAMPLE", ZResourceType::Room, new ExporterExample_Room());
	Globals::AddExporter("EXAMPLE", ZResourceType::CollisionHeader, new ExporterExample_Collision());
}

REGISTER_EXPORTER(ImportExporters)