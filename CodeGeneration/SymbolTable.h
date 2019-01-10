#pragma once
#include <vector>
#include "stdafx.h"
#include "Error.h"

#define VARIABLE_TYPE 0
#define CONST_TYPE 1
#define PARA_TYPE 2
#define INT_TYPE 0
#define CHAR_TYPE 1
#define RETURN_INT 0
#define RETURN_CHAR 1
#define RETURN_VOID 2

extern Error error;


struct SymbolTableElement
{
	std::string name;
	int accessType;
	int dataType;
	bool isArray = false;
	int arrayLengthOrConstVal;

	bool isInitialized = false;
};

struct FuncSymbolTable
{
	std::string name;
	int returnType;
	std::vector<SymbolTableElement> parameterTable;
	std::vector<SymbolTableElement> dataTable;
	bool isParaExist(std::string identifier);
	bool isDataExist(std::string identifier);
};

class SymbolTable
{
public:
	SymbolTable(std::ofstream &inIntermediateCodeFile): intermediateCodeFile(inIntermediateCodeFile) {}
	std::ofstream &intermediateCodeFile;
	std::vector<SymbolTableElement> globalDataTable;
	std::vector<FuncSymbolTable> funcSymbolTables;
	bool addGlobal(SymbolTableElement &newElement);
	bool isGlobalExist(std::string identifier);
	void printGlobal();

	bool addFuc(std::string name, int inReturnType);
	bool isFuncExist(std::string identifier);
	int funcSearch(std::string identifier);
	bool dataSearch(SymbolTableElement & element, std::string name);
	bool addPara(std::string name, int inDataType);
	bool addData(std::string name, int accessType, int inDataType, bool isArray, int arrayDimOrConstVal);
	void printFunc();
};

