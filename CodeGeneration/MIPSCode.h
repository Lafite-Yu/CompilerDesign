#pragma once
#include "stdafx.h"
#include "SymbolTable.h"
#include "IntermediateCode.h"
#include "RegisterManager.h"
#include "Quotation.h"

extern Error error;

class MIPSCode
{
public:
	MIPSCode(SymbolTable &inSymbolTable, IntermediateCode &inIntermediateCode, RegisterManager &inRegisterManager, std::ofstream &inMipsCodeFile):
		intermediateCode{ inIntermediateCode }, symbolTable{ inSymbolTable }, registerManager{ inRegisterManager }, MIPSCodeFile{ inMipsCodeFile }
	{
		std::cout << "===============MIPS CODE GENERATION STARTED====================" << std::endl;
		std::cout << error.errCount << "errors and " << error.warningCount << "warnings." << std::endl;
#ifndef IGNORE_ERRORS
		if (error.errCount > 0)
		{
			std::cout << "MIPS Code generation terminated." << std::endl;
			system("pause");
			exit(EXIT_SUCCESS);
		}
#endif
	};
	void run();

private:
	IntermediateCode &intermediateCode;
	SymbolTable &symbolTable;
	RegisterManager &registerManager;
	std::ofstream &MIPSCodeFile;

	int quotationIndex = 0;
	int quotationSize = intermediateCode.quotations.size();
	void generateGlobal();
	void generateString();
	void funcGenerator();
	void autoGenerator(int funcIndex);
	
	

	
};

