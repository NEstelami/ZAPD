#include <Windows.h>
#include <HLExporterTest.h>

typedef void(AddExporterFunc)(ZResourceType, ZResourceExporter*);
typedef void(__stdcall* ImportExporterFunc)(AddExporterFunc*);

extern "C"
{
	__declspec(dllexport) void ImportExporters(AddExporterFunc* addExporter)
	{
		addExporter(ZResourceType::Texture, new HLExporterTest_Texture());
	}
}