#pragma once
#include "stdafx.h"

class LabelGenerator
{
public:
	int count = 0;
	std::string getLabel(std::string name)
	{
		auto retLabel = name;
		retLabel += "_Label_" + std::to_string(++count);
		return retLabel;
	}
};

