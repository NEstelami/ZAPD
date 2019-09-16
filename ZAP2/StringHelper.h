#pragma once

#include <string>
#include <vector>

class StringHelper
{
public:
	static std::vector<std::string> Split(std::string s, std::string delimiter)
	{
		std::vector<std::string> result;

		size_t pos = 0;
		std::string token;

		while ((pos = s.find(delimiter)) != std::string::npos)
		{
			token = s.substr(0, pos);
			result.push_back(token);
			s.erase(0, pos + delimiter.length());
		}

		if (s.length() != 0)
			result.push_back(s);

		return result;
	}

	static std::string Strip(std::string s, std::string delimiter)
	{
		size_t pos = 0;
		std::string token;

		while ((pos = s.find(delimiter)) != std::string::npos)
		{
			token = s.substr(0, pos);
			s.erase(pos, pos + delimiter.length());
		}

		return s;
	}
};