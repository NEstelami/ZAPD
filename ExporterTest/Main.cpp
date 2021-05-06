#include <Windows.h>
#include <TextureExporter.h>
#include <RoomExporter.h>
#include <CollisionExporter.h>
#include <DisplayListExporter.h>
#include <Globals.h>

void ImportExporters()
{
	// In this example we set up a new exporter called "EXAMPLE".
	// By running ZAPD with the argument -sexp EXAMPLE, we tell it that we want to use this exporter for our resources.
	Globals::AddExporter("EXAMPLE", ZResourceType::Texture, new ExporterExample_Texture());
	Globals::AddExporter("EXAMPLE", ZResourceType::Room, new ExporterExample_Room());
	Globals::AddExporter("EXAMPLE", ZResourceType::CollisionHeader, new ExporterExample_Collision());
}

// When ZAPD starts up, it will automatically call the below function, which in turn sets up our exporters.
REGISTER_EXPORTER(ImportExporters)