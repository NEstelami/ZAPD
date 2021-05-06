#include <Windows.h>
#include <TextureExporter.h>
#include <RoomExporter.h>
#include <CollisionExporter.h>
#include <DisplayListExporter.h>
#include <Globals.h>

static void ExporterFileBegin(ZFile* file)
{
	printf("ExporterFileBegin() called on ZFile %s.\n", file->GetName().c_str());
}

static void ExporterFileEnd(ZFile* file)
{
	printf("ExporterFileEnd() called on ZFile %s.\n", file->GetName().c_str());
}

static void ImportExporters()
{
	// In this example we set up a new exporter called "EXAMPLE".
	// By running ZAPD with the argument -sexp EXAMPLE, we tell it that we want to use this exporter for our resources.
	ExporterSet* exporterSet = new ExporterSet();
	exporterSet->BeginFunc = ExporterFileBegin;
	exporterSet->EndFunc = ExporterFileEnd;
	exporterSet->exporters[ZResourceType::Texture] = new ExporterExample_Texture();
	exporterSet->exporters[ZResourceType::Room] = new ExporterExample_Room();
	exporterSet->exporters[ZResourceType::CollisionHeader] = new ExporterExample_Collision();

	Globals::AddExporter("EXAMPLE", exporterSet);
}

// When ZAPD starts up, it will automatically call the below function, which in turn sets up our exporters.
REGISTER_EXPORTER(ImportExporters)