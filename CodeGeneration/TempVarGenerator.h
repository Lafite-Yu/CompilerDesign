#pragma once
#include"stdafx.h"
#include <vector>

class TempVarGenerator
{
public:
	int count = 0;
	// std::vector<int> tempVars;

	std::string generateTempVar()
	{
		std::string retVar = "#temp" + std::to_string(++count);
		return retVar;
	}

	// std::string getTempVar()
	// {
	// 	std::string retVar = "temp" + std::to_string(count--);
	// 	return retVar;
	// }
};
