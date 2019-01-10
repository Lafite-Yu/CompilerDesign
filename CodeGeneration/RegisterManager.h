#pragma once
#include "stdafx.h"
#include <vector>
#include <array>
#include <queue>
#include "SymbolTable.h"
#include "IntermediateCode.h"
#include "Quotation.h"

enum Reg {zero, 
			at, 
			v0, v1, 
			a0, a1, a2, a3, 
			t0, t1, t2, t3, t4, t5, t6, t7,
			s0, s1, s2, s3, s4, s5, s6, s7,
			t8, t9,
			gp=28, sp, fp, ra};

class RegisterManager
{
public:
	RegisterManager(SymbolTable &inSymbolTable,IntermediateCode &inIntermediaCode, std::ofstream &inMIPSCodeFile) :
		symbolTable{ inSymbolTable }, intermediateCode{ inIntermediaCode },  MIPSCodeFile { inMIPSCodeFile } {};

	int getTReg(std::string identifier);
	int releaseTReg(std::string identifier);
	int getDataValue(std::string identifier, int funxIndex);
	int getVarType(std::string identifier, int funcIndex);
	int assignDataValue(std::string identifier, int funxIndex, int srcReg1, int srcReg2, std::string op);
	void storeAll(int fpMovemrnt);
	void recoverAll(int funcIndex);
	int sInUse = 0;
	bool funcCallSPRegMoved = false;

private:
	std::ofstream &MIPSCodeFile;
	std::vector<std::string> tempVarRegSet;
	std::array<std::pair<std::string, int>, 32> regSet;

	SymbolTable &symbolTable;
	IntermediateCode &intermediateCode;
	int tCount = 0;
	int tInUse = 0;

	int funcCallDepth = 0;
};

