#include "stdafx.h"
#include "RegisterManager.h"

using namespace std;

int RegisterManager::getTReg(std::string identifier)
{
	if (tInUse < 8)
	{
		regSet.at(tCount + Reg(t0)).first = identifier;
		tCount++;
		tInUse++;
		// MIPSCodeFile << "==get==tInUse:" << tInUse << endl;
		return tInUse-1 + Reg(t0);
	}
	else
	{
		int i = tCount % 8;
		MIPSCodeFile << "sw $t" << i << " , 0($fp)" << endl;
		MIPSCodeFile << "addi $fp, $fp, 4" << endl;
		regSet.at(i + Reg(t0)).first = identifier;
		tCount++;
		return i + Reg(t0);
	}
}

int RegisterManager::releaseTReg(std::string identifier)
{
	tCount--;
	if (tInUse > 0)
	{
		tInUse--;
		regSet.at(tInUse + Reg(t0)).first = "";
		// MIPSCodeFile << "==release==tInUse:" << tInUse << endl;
		return tInUse + Reg(t0);
	}
	else
	{
		MIPSCodeFile << "#get stored $t" << endl;
		MIPSCodeFile << "subi $fp, $fp, 4" << endl;
		if (funcCallSPRegMoved)
			MIPSCodeFile << "subi $sp, $sp, 4" << endl;
		MIPSCodeFile << "lw $t9, 0($fp)" << endl;
		return Reg(t9);
	}
}

int RegisterManager::getDataValue(std::string identifier, int funcIndex)
{
	auto &funcTable = symbolTable.funcSymbolTables.at(funcIndex);
	int offset = 0;
	bool find = false;
	for (size_t i = 0; i < funcTable.parameterTable.size(); i++)
	{
		if (funcTable.parameterTable.at(i).name == identifier)
		{
			find = true;
			break;
		}
		offset += (funcTable.parameterTable.at(i).isArray ? (funcTable.parameterTable.at(i).arrayLengthOrConstVal) : 1);
	}
	if (!find)
	{
		for (size_t i = 0; i < funcTable.dataTable.size(); i++)
		{
			if (funcTable.dataTable.at(i).name == identifier)
			{
				// offset = funcTable.parameterTable.size() + i;
				find = true;
				break;
			}
			offset += (funcTable.dataTable.at(i).isArray ? (funcTable.dataTable.at(i).arrayLengthOrConstVal) : 1);
		}
	}
	if (find)
	{
		if (funcCallSPRegMoved)
			MIPSCodeFile << "lw $s" << sInUse << ", " << to_string(offset * 4) << "($t8)" << endl;
		else
			MIPSCodeFile << "lw $s" << sInUse << ", " << to_string(offset * 4) << "($sp)" << endl;
	}
	else
	{
		MIPSCodeFile << "lw $s" << sInUse << " ," << identifier << endl;
	}
	sInUse++;
	return  sInUse - 1 + Reg(s0);
}

int RegisterManager::getVarType(std::string identifier, int funcIndex)
{
	auto &funcTable = symbolTable.funcSymbolTables.at(funcIndex);
	int res = 0;
	for (size_t i = 0; i < funcTable.parameterTable.size(); i++)
	{
		if (funcTable.parameterTable.at(i).name == identifier)
		{
			return funcTable.parameterTable.at(i).dataType;
		}
	}

	for (size_t i = 0; i < funcTable.dataTable.size(); i++)
	{
		if (funcTable.dataTable.at(i).name == identifier)
		{
			return funcTable.dataTable.at(i).dataType;
		}
	}

	for (size_t i = 0; i < symbolTable.globalDataTable.size(); i++)
	{
		if (symbolTable.globalDataTable[i].name == identifier)
		{
			return symbolTable.globalDataTable[i].dataType;
		}
	}

	return INT_TYPE;
}

int RegisterManager::assignDataValue(std::string identifier, int funcIndex, int srcReg1, int srcReg2, string op)
{
	if (op == "ADD")
		MIPSCodeFile << "add $s" << sInUse << ", $" << srcReg1 << ", $" << srcReg2 << endl;
	else if (op == "MINUS")
		MIPSCodeFile << "sub $s" << sInUse << ", $" << srcReg1 << ", $" << srcReg2 << endl;
	else if (op == "MULT")
		MIPSCodeFile << "mul $s" << sInUse << ", $" << srcReg1 << ", $" << srcReg2 << endl;
	else if (op == "DIV")
	{
		MIPSCodeFile << "div $" << srcReg1 << ", $" << srcReg2 << endl;
		MIPSCodeFile << "mflo $s" << sInUse << endl;
	}
	else if (op == "ASSIGN")
	{
		MIPSCodeFile << "move $s" << sInUse << ", $" << srcReg1 << endl;

	}

	auto &funcTable = symbolTable.funcSymbolTables.at(funcIndex);
	int offset = 0;
	bool find = false;
	for (size_t i = 0; i < funcTable.parameterTable.size(); i++)
	{
		if (funcTable.parameterTable.at(i).name == identifier)
		{
			find = true;
			break;
		}
		offset += (funcTable.parameterTable[i].isArray ? funcTable.parameterTable[i].arrayLengthOrConstVal : 1);
	}
	if (!find)
	{
		for (size_t i = 0; i < funcTable.dataTable.size(); i++)
		{
			if (funcTable.dataTable.at(i).name == identifier)
			{
				find = true;
				break;
			}
			offset += (funcTable.dataTable[i].isArray ? funcTable.dataTable[i].arrayLengthOrConstVal : 1);
		}
	}
	if (find)
	{
		MIPSCodeFile << "sw $s" << sInUse << ", " << to_string(offset * 4) << "($sp)" << endl;
	}
	else
	{
		MIPSCodeFile << "sw $s" << sInUse << " ," << identifier << endl;
	}

	sInUse = 0;
	return 0;
}

void RegisterManager::storeAll(int fpMovement)
{
	MIPSCodeFile << "#STORE: tInUse:" << tInUse << " ,tCount:" << tCount << endl;
	funcCallDepth++;
	int startIndex = (tCount+1) % 8;
	int count = 0;
	for (int i = 0; i < 8; i++)
	{
		if (tInUse >= 8)
		{
			if (regSet[Reg(t0) + startIndex].first != "")
			{
				MIPSCodeFile << "sw $" << startIndex + Reg(t0) << ", " << to_string(count * 4) << "($fp)" << endl;
				count++;
			}
			startIndex = (startIndex + 1) % 8;
		}
		else
		{
			if (i >= tInUse)
				break;
			MIPSCodeFile << "sw $" << i + Reg(t0) << ", " << to_string(count * 4) << "($fp)" << endl;
			count++;
		}
	}
	if (funcCallDepth == 1)
	{
		MIPSCodeFile << "sw $sp, " << to_string(tInUse * 4) << "($fp)" << endl;
		MIPSCodeFile << "move $t8, $sp" << endl;
		MIPSCodeFile << "addi $sp, $fp, " << to_string((tInUse + 1) * 4) << endl;
		MIPSCodeFile << "move $fp, $sp" << endl;
		funcCallSPRegMoved = true;
	}
	else
	{
		MIPSCodeFile << "lw $t8, " << -4-fpMovement << "($fp)" << endl;
		MIPSCodeFile << "sw $t8, " << to_string(tInUse * 4) << "($fp)" << endl;
		MIPSCodeFile << "addi $sp, $fp, " << to_string((tInUse + 1) * 4) << endl;
		MIPSCodeFile << "move $fp, $sp" << endl;
		funcCallSPRegMoved = true;
	}
	// tCount = 0;
	tInUse = 0;
}

void RegisterManager::recoverAll(int funcIndex)
{
	funcCallDepth--;
	if (funcCallDepth == 0)
	{
		funcCallSPRegMoved = false;
	}
	MIPSCodeFile << "subi $fp, $sp, 4" << endl;
	MIPSCodeFile << "lw $sp, 0($fp)" << endl;
	auto &funcTable = symbolTable.funcSymbolTables.at(funcIndex);
	int offset = 0;
	for (size_t i = 0; i < funcTable.parameterTable.size(); i++)
	{
		offset += (funcTable.parameterTable[i].isArray ? funcTable.parameterTable[i].arrayLengthOrConstVal : 1);
	}
	for (size_t i = 0; i < funcTable.dataTable.size(); i++)
	{
		offset += (funcTable.dataTable[i].isArray ? funcTable.dataTable[i].arrayLengthOrConstVal : 1);
	}
	MIPSCodeFile << "lw $ra, " << 4*offset << "($sp)" << endl;
	MIPSCodeFile << "#tInUse:" << tInUse << " ,tCount:" << tCount << endl;
	if (funcCallSPRegMoved)
	{
		MIPSCodeFile << "move $t8, $sp" << endl;
		MIPSCodeFile << "move $sp, $fp" << endl;

	}
}

