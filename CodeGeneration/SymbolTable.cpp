#include "stdafx.h"
#include "SymbolTable.h"

using namespace std;

// SymbolTableElement& SymbolTable::addGlobal()
// {
// 	SymbolTableElement newElement;
// 	globalDataTable.push_back(newElement);
// 	// cout << globalDataTable.size() << endl;
// 	return globalDataTable.back();
// }

bool FuncSymbolTable::isParaExist(std::string identifier)
{
	for (auto element: parameterTable)
	{
		if (element.name == identifier)
		{
			return true;
		}
	}
	return false;
}

bool FuncSymbolTable::isDataExist(std::string identifier)
{
	for (auto element : dataTable)
	{
		if (element.name == identifier)
		{
			return true;
		}
	}
	return false;
}

bool SymbolTable::addGlobal(SymbolTableElement &newElement)
{
	if (isGlobalExist(newElement.name))
	{
		return false;
	}
	globalDataTable.push_back(newElement);
	return true;
}

bool SymbolTable::isGlobalExist(std::string identifier)
{
	for (auto element : globalDataTable)
	{
		if (element.name == identifier)
		{
			return true;
		}
	}
	return false;
}

void SymbolTable::printGlobal()
{
	intermediateCodeFile << "Global: " << endl;
	for (auto element: globalDataTable)
	{
		if (element.accessType == VARIABLE_TYPE)
		{
			intermediateCodeFile << "var " << (element.dataType == INT_TYPE ? "int " : "char ") << element.name;
			if (element.isArray)
				intermediateCodeFile << "[" << element.arrayLengthOrConstVal << "]";
			intermediateCodeFile << endl;
		}
		else
		{
			intermediateCodeFile << "const " << (element.dataType == INT_TYPE ? "int " : "char ") << element.name << " = " << element.arrayLengthOrConstVal << endl;
		}
	}
}

bool SymbolTable::addFuc(std::string inName, int inReturnType)
{
	if (isFuncExist(inName) || isGlobalExist(inName))
	{
		return false;
	}
	FuncSymbolTable newFunc;
	newFunc.name = inName;
	newFunc.returnType = inReturnType;
	funcSymbolTables.push_back(newFunc);
	return true;
}

bool SymbolTable::isFuncExist(std::string identifier)
{
	for (auto element : funcSymbolTables)
	{
		if (element.name == identifier)
		{
			return true;
		}
	}
	return false;
}

int SymbolTable::funcSearch(string identifier)
{
	for (auto begin = funcSymbolTables.begin(); begin < funcSymbolTables.end(); ++begin)
	{
		if (begin->name == identifier)
			return begin - funcSymbolTables.begin();
	}
	return -1;
}

bool SymbolTable::dataSearch(SymbolTableElement &element, string name)
{
	for (auto tableElement: funcSymbolTables.back().dataTable)
	{
		if (tableElement.name == name)
		{
			element = tableElement;
			return true;
		}
	}
	for (auto tableElement : funcSymbolTables.back().parameterTable)
	{
		if (tableElement.name == name)
		{
			element = tableElement;
			return true;
		}
	}
	for (auto tableElement : globalDataTable)
	{
		if (tableElement.name == name)
		{
			element = tableElement;
			return true;
		}
	}
	return false;
}

bool SymbolTable::addPara(std::string name, int inDataType)
{
	auto &prevFunc = funcSymbolTables.back();
	if (prevFunc.isParaExist(name))
		return false;
	SymbolTableElement newPara;
	newPara.name = name;
	newPara.accessType = PARA_TYPE;
	newPara.dataType = inDataType;
	prevFunc.parameterTable.push_back(newPara);
	return true; 
}

bool SymbolTable::addData(std::string name, int inAccessType, int inDataType, bool isArray, int arrayDimOrConstVal)
{
	auto &prevFunc = funcSymbolTables.back();
	if (prevFunc.isParaExist(name) || prevFunc.isDataExist(name))
		return false;
	SymbolTableElement newData;
	newData.name = name;
	newData.accessType = inAccessType;
	newData.dataType = inDataType;
	newData.isArray = isArray;
	newData.arrayLengthOrConstVal = arrayDimOrConstVal;
	prevFunc.dataTable.push_back(newData);
	return true;
}

void SymbolTable::printFunc()
{
	static bool isGlobalPrinted = false;
	if (!isGlobalPrinted)
	{
		printGlobal();
		isGlobalPrinted = true;
	}
	auto &func = funcSymbolTables.back();
	intermediateCodeFile << endl << (func.returnType == RETURN_INT ? "int " : (func.returnType == RETURN_CHAR ? "char " : "void ")) << func.name << "()" << endl;
	for (auto element: func.parameterTable)
	{
		intermediateCodeFile << "para " << (element.dataType == INT_TYPE ? "int " : "char ") << element.name << endl;
	}
	for (auto element : func.dataTable)
	{
		if (element.accessType == VARIABLE_TYPE)
		{
			intermediateCodeFile << "var " << (element.dataType == INT_TYPE ? "int " : "char ") << element.name;
			if (element.isArray)
				intermediateCodeFile << "[" << element.arrayLengthOrConstVal << "]";
			intermediateCodeFile << endl;
		}
		else
		{
			intermediateCodeFile << "const " << (element.dataType == INT_TYPE ? "int " : "char ") << element.name << " = " << element.arrayLengthOrConstVal << endl;
		}
	}
}

