#pragma once

class Globals
{
public:
	static Globals* Instance;

	bool genSourceFile; // Used for extraction

	Globals();
};