#pragma once

#include <string>
#include <iostream>

#ifdef _MSC_VER
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

class Path
{
public:
	static std::string GetFileNameWithoutExtension(std::string input)
	{
		return input.substr(0, input.find_last_of("."));
	};

	static std::string GetDirectoryName(std::string path)
	{
		return fs::path(path).parent_path().u8string();
	};
};