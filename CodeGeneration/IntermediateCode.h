#pragma once
#include "stdafx.h"
#include "Quotation.h"
#include <vector>

class IntermediateCode
{
public:
	IntermediateCode(std::ofstream &inIntermediateCodeFile) : intermediateCodeFile(inIntermediateCodeFile) {}
	std::ofstream &intermediateCodeFile;
	std::vector<Quotation> quotations;
	std::vector<std::string> stringList;
	void addQuotation(Quotation inQuotation);
};

