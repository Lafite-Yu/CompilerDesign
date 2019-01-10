#pragma once
#include "Error.h"
#include "LexicalAnalysis.h"
#include "SymbolTable.h"
#include "IntermediateCode.h"
#include "LabelGenerator.h"
#include "TempVarGenerator.h"

extern Error error;

class SyntaxAnalysis
{
public:
	SyntaxAnalysis(LexicalAnalysis *la, SymbolTable &_symbolTable, IntermediateCode &_intermediateCode, LabelGenerator &_labelGenerator, TempVarGenerator &_tempVarGenerator);
	void run();

private:
	LexicalAnalysis *lexicalAnalysis = nullptr;
	SymbolTable & symbolTable;
	IntermediateCode &intermediateCode;
	LabelGenerator &labelGenerator;
	TempVarGenerator &tempVarGenerator;
	inline bool getSym() const;
	inline int getSymType() const;
	inline std::string getToken() const;
	inline int getIntValue() const;
	inline int getLineNum() const;
	inline int getIndexInLine() const;
	void skipTo(int symType);

	void constDeclare();
	void constDeclare(bool isGlobal);
	void constIntDefine(bool isGlobal);
	void constCharDefine(bool isGlobal);
	void globalVarHandler();
	void funcDeclare(int returnType, bool hasParaList);
	int paraList();
	void autoVariableDeclare();
	void autoVariableDefine();
	void statementsList();
	void statement();
	void conditionStatement();
	void compareConditionStatement(std::string elseLabel);
	std::string expression();
	std::string item();
	std::string factor();
	void paraValueList(int funcIndex);
	void loopStatement();
	void compareLoopStatement(std::string doLabel);
	void switchStatement();
	void scanfStatement();
	void printfStatement();
	void returnStatement();
	void mainDeclare();

	std::string identifier;
	int dimOfArray = 0;

	bool reachedMainFunc = false;
};

