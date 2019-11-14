#include "Globals.h"

Globals* Globals::Instance;

Globals::Globals()
{
	Instance = this;

	genSourceFile = true;
}