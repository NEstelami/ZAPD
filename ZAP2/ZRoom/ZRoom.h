#pragma once

#include "../ZResource.h"
#include "ZRoomCommand.h"
#include "../tinyxml2.h"

#include <vector>
#include <map>
#include <string>

class Declaration;
struct CommandSet;

class ZRoom : public ZResource
{
protected:
	std::vector<ZRoomCommand*> commands;

	std::string GetSourceOutputHeader(std::string prefix);
	std::string GetSourceOutputCode(std::string prefix);
	void GenDefinitions();
	void ProcessCommandSets();
public:
	ZRoom* scene;
	std::map<int32_t, Declaration*> declarations;
	std::map<int32_t, std::string> externs;
	std::vector<CommandSet> commandSets;

	ZRoom(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZRoom* nScene);

	void ParseCommands(std::vector<ZRoomCommand*>& commandList, CommandSet commandSet);
	int32_t GetDeclarationSizeFromNeighbor(int declarationAddress);
	int32_t GetCommandSizeFromNeighbor(ZRoomCommand* cmd);
	ZRoomCommand* FindCommandOfType(RoomCommand cmdType);
	std::vector<uint8_t> GetRawData();
	int GetRawDataSize();
};

struct CommandSet
{
	int32_t address;
	int32_t commandCount; // Only used if explicitly specified in the XML

	CommandSet(int32_t nAddress);
	CommandSet(int32_t nAddress, int32_t nCommandCount);
};

enum class DeclarationAlignment
{
	None,
	Align4,
	Align8,
	Align16
};

enum class DeclarationPadding
{
	None,
	Pad4,
	Pad8,
	Pad16
};

class Declaration
{
public:
	DeclarationAlignment alignment;
	DeclarationPadding padding;
	uint32_t size;
	std::string text;

	Declaration(DeclarationAlignment nPadding, uint32_t nSize, std::string nText);
	Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, uint32_t nSize, std::string nText);
};