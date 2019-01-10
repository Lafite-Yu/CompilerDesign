#include "stdafx.h"
#include "MIPSCode.h"
#include <locale>

using namespace std;

void MIPSCode::run()
{
	MIPSCodeFile << ".data" << endl << endl;
	generateGlobal();
	generateString();

	MIPSCodeFile << endl << ".text\nsubi $sp, $sp, 16000\nj main" << endl << endl;
	while (quotationIndex < quotationSize)
	{
		funcGenerator();
	}
}

void MIPSCode::generateGlobal()
{
	for (size_t i = 0; i < symbolTable.globalDataTable.size(); i++)
	{
		auto &globalData = symbolTable.globalDataTable.at(i);
		if (globalData.isArray)
			MIPSCodeFile << globalData.name << ":\t.space " << to_string(globalData.arrayLengthOrConstVal*4) << endl;
		else
			MIPSCodeFile << globalData.name << ":\t.word " << globalData.arrayLengthOrConstVal << endl;
	}
}

void MIPSCode::generateString()
{
	for (size_t i = 0; i < intermediateCode.stringList.size(); i++)
		MIPSCodeFile << "stringToPrint" << to_string(i) << ":\t.asciiz " << intermediateCode.stringList.at(i) << endl;
}

void MIPSCode::funcGenerator()
{

	MIPSCodeFile << intermediateCode.quotations.at(quotationIndex).dst << ":" << endl;
	int funcIndex = symbolTable.funcSearch(intermediateCode.quotations.at(quotationIndex).dst);
	autoGenerator(funcIndex);
	quotationIndex++;
	// int paraCount = 0;
	vector<int> paraCounts;
	string paraPushInstrs;
	vector<string> arrayAddressReg;
	while (quotationIndex < quotationSize && intermediateCode.quotations.at(quotationIndex).opType != FUNC_LABEL)
	{
		Quotation &quotation = intermediateCode.quotations.at(quotationIndex);
		registerManager.sInUse = 0;
		switch (quotation.opType)
		{
		case(ASSIGN):
			{
				// src2
				int srcReg2 = 0;
				if (quotation.op != "ASSIGN")
				{
					if (quotation.src2[0] == '#')
						srcReg2 = registerManager.releaseTReg(quotation.src2);
					else
						srcReg2 = registerManager.getDataValue(quotation.src2, funcIndex);
				}
				// src1
				int srcReg1;
				if (quotation.src1[0] == '#')
					srcReg1 = registerManager.releaseTReg(quotation.src1);
				else
					srcReg1 = registerManager.getDataValue(quotation.src1, funcIndex);
				// dst
				if (quotation.dst[0] == '#')
				{
					int dstReg = registerManager.getTReg(quotation.dst);
					if (quotation.op == "ADD")
						MIPSCodeFile << "add $" << dstReg << ", $" << srcReg1 << ", $" << srcReg2 << endl;
					else if (quotation.op == "MINUS")
						MIPSCodeFile << "sub $" << dstReg << ", $" << srcReg1 << ", $" << srcReg2 << endl;
					else if (quotation.op == "MULT")
					{
						MIPSCodeFile << "mul $" << dstReg << ", $" << srcReg1 << ", $" << srcReg2 << endl;
					}
					else if (quotation.op == "DIV")
					{
						MIPSCodeFile << "div $" << srcReg1 << ", $" << srcReg2 << endl;
						MIPSCodeFile << "mflo $" << dstReg << endl;
					}
					else if (quotation.op == "ASSIGN")
					{
						MIPSCodeFile << "#ASSIGN\nmove $" << dstReg << ", $" << srcReg1 << endl;

					}

					if (arrayAddressReg.size() > 0 && quotation.dst == arrayAddressReg.back())
					{
						arrayAddressReg.pop_back();
						int valueReg = registerManager.releaseTReg(quotation.dst);
						int addressReg = registerManager.releaseTReg(quotation.dst);
						MIPSCodeFile << "sw $" << valueReg << ", 0($" << addressReg << ")" << endl;
					}
				}
				else
				{
					registerManager.assignDataValue(quotation.dst, funcIndex, srcReg1, srcReg2, quotation.op);
				}
			break;
			}
		case(LABEL):
			{
				MIPSCodeFile << quotation.dst << ": " << endl;
			break;
			}
		case (FUNC_CALL_START_LABEL):
			{
				int fpMovement = 0;
				MIPSCodeFile << "#STORE" << endl;
				if (!paraCounts.empty())
				{
					MIPSCodeFile << "addi $fp, $fp, " << paraCounts.back() * 4 << endl;
					fpMovement = paraCounts.back() * 4;
				}
				paraCounts.push_back(0);
				registerManager.storeAll(fpMovement);
			break;
			}
		case (PUSH):
			{
				if (quotation.dst[0] == '#')
					MIPSCodeFile << "#PUSH\nsw $" << to_string(registerManager.releaseTReg(quotation.dst)) << ", " << to_string(paraCounts.back() * 4) << "($sp)" << endl;
				else
					MIPSCodeFile << "#PUSH\nsw $" << to_string(registerManager.getDataValue(quotation.dst, funcIndex)) << ", " << to_string(paraCounts.back() * 4) << "($sp)" << endl;
				// paraCount++;
				paraCounts.back() = paraCounts.back() + 1;
			break;
			}
		case(FUNC_CALL):
			{
				// MIPSCodeFile << "#STORE" << endl;
				// registerManager.storeAll();
				// MIPSCodeFile << paraPushInstrs;
				// paraPushInstrs = "";
				// paraCount = 0;
				paraCounts.pop_back();
				MIPSCodeFile << "jal " << quotation.dst << endl;
				MIPSCodeFile << "#RECOVER" << endl;
				registerManager.recoverAll(funcIndex);
				if (!paraCounts.empty())
				{
					MIPSCodeFile << "subi $sp, $sp, " << paraCounts.back() * 4 << endl;
				}
				MIPSCodeFile << "#FUNC CALL END" << endl;
				
			break;
			}
		case (RET_VAL):
			{
				int dstReg = 0;
				if (quotation.dst[0] == '#')
				{
					dstReg = registerManager.getTReg(quotation.dst);
					MIPSCodeFile << "move $" << dstReg << ", $v0" << endl;
				}
				else
					dstReg = registerManager.assignDataValue(quotation.dst, funcIndex, Reg(v0), 0, "ASSIGN");
			break;
			}
		case (RETURN):
			{		
				if (quotation.op == "DEFAULT_RETURN")
				{
					MIPSCodeFile << "move $v0, $0" << endl;
				}
				else if (quotation.op != "VOID")
				{
					int dstReg = 0;
					if (quotation.dst[0] == '#')
						dstReg = registerManager.releaseTReg(quotation.dst);
					else
						dstReg = registerManager.getDataValue(quotation.dst, funcIndex);
					MIPSCodeFile << "move $v0, $" << dstReg << endl;
				}
				MIPSCodeFile << "jr $ra" << endl;
			break;
			}
		case (COMPARE):
			{
				// src1.
				int srcReg1;
				if (quotation.src1[0] == '#')
					srcReg1 = registerManager.releaseTReg(quotation.src1);
				else
					srcReg1 = registerManager.getDataValue(quotation.src1, funcIndex);
				if (quotation.op == "bez" || quotation.op == "bnz")
				{
					MIPSCodeFile << (quotation.op == "bez" ? "beq" : "bne") << " $" << srcReg1 << ", $0, " << quotation.dst << endl;
				}
				else
				{
					// src2
					int srcReg2 = 0;
					if (quotation.src2[0] == '#')
						srcReg2 = registerManager.releaseTReg(quotation.src2);
					else
						srcReg2 = registerManager.getDataValue(quotation.src2, funcIndex);
					MIPSCodeFile << quotation.op << " $" << srcReg1 << ", $" << srcReg2 << ", " << quotation.dst << endl;
				}
			break;
			}
		case (GOTO):
			{
				MIPSCodeFile << "j " << quotation.dst << endl;
			break;
			}
		case (ARRAY_GET):
			{
				// array
				auto &funcTable = symbolTable.funcSymbolTables.at(funcIndex);
				int offset = 0;
				bool find = false;
				for (size_t i = 0; i < funcTable.parameterTable.size(); i++)
				{
					if (funcTable.parameterTable.at(i).name == quotation.src1)
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
						if (funcTable.dataTable.at(i).name == quotation.src1)
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
					if (registerManager.funcCallSPRegMoved)
						MIPSCodeFile << "addi  $t9, $t8, " << to_string(offset * 4) << endl;
					else
						MIPSCodeFile << "addi  $t9, $sp, "<< to_string(offset * 4) << endl;
				}
				else
				{
					MIPSCodeFile << "la $t9, " << quotation.src1 << endl;
				}
				// src2
				int srcReg2 = 0;
				if (quotation.src2[0] == '#')
					srcReg2 = registerManager.releaseTReg(quotation.src2);
				else
					srcReg2 = registerManager.getDataValue(quotation.src2, funcIndex);
				MIPSCodeFile << "sll $" << srcReg2 << ", $" << srcReg2 << ", 2" << endl;
				MIPSCodeFile << "add  $t9, $t9, $" << srcReg2 << endl;

				int dstReg = registerManager.getTReg(quotation.dst);
				MIPSCodeFile << "lw $" << dstReg << ", 0($t9)" << endl;
			break;
			}
		case (LI):
			{
				int dstReg = registerManager.getTReg(quotation.dst);
				MIPSCodeFile << "li $" << dstReg << ", " << quotation.srcNum1 << endl;
			break;
			}
		case (NEGATIVE):
			{
				int srcReg1 = 0;
				if (quotation.src1[0] == '#')
					srcReg1 = registerManager.releaseTReg(quotation.src1);
				else
					srcReg1 = registerManager.getDataValue(quotation.src1, funcIndex);
				int dstReg = registerManager.getTReg(quotation.dst);
				MIPSCodeFile << "neg $" << dstReg << ", $" << srcReg1 << endl;;
			break;
			}
		case (ARRAY_ASSIGN):
			{
				// array
				auto &funcTable = symbolTable.funcSymbolTables.at(funcIndex);
				int offset = 0;
				bool find = false;
				for (size_t i = 0; i < funcTable.parameterTable.size(); i++)
				{
					if (funcTable.parameterTable.at(i).name == quotation.src1)
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
						if (funcTable.dataTable.at(i).name == quotation.src1)
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
					MIPSCodeFile << "addi  $t9, $sp, " << to_string(offset * 4) << endl;
				}
				else
				{
					MIPSCodeFile << "la $t9, " << quotation.src1 << endl;
				}

				// src2
				int srcReg2 = 0;
				if (quotation.src2[0] == '#')
					srcReg2 = registerManager.releaseTReg(quotation.src2);
				else
					srcReg2 = registerManager.getDataValue(quotation.src2, funcIndex);
				int dstReg = registerManager.getTReg(quotation.dst);
				arrayAddressReg.push_back(quotation.dst);
				MIPSCodeFile << "sll $" << srcReg2 << ", $"<< srcReg2 <<", 2" << endl;
				MIPSCodeFile << "add  $" << dstReg <<", $t9, $" << srcReg2 << endl;
			break;
			}
		case (SCANF):
			{
				int type = registerManager.getVarType(quotation.dst, funcIndex);
				if (type == INT_TYPE)
					MIPSCodeFile << "li $v0, 5" << endl;
				else
					MIPSCodeFile << "li $v0, 12" << endl;
				MIPSCodeFile << "syscall" << endl;
				int dstReg = registerManager.assignDataValue(quotation.dst, funcIndex, Reg(v0), 0, "ASSIGN");
			break;
			}
		case (PRINTF):
			{
				MIPSCodeFile << "#PRINTF" << endl;
				if (quotation.op == "STRING")
				{
					MIPSCodeFile << "la $a0, stringToPrint" << quotation.srcNum1 << endl;
					MIPSCodeFile << "li $v0, 4" << endl;
					MIPSCodeFile << "syscall" << endl;
				}
				else
				{
					int srcReg1 = 0;
					// cout << "op:" << quotation.op << "src1: " << quotation.src1 << " src2: " << quotation.src2 << endl;
					if (quotation.dst[0] == '#')
					{
						srcReg1 = registerManager.releaseTReg(quotation.dst);
						MIPSCodeFile << "li $v0, 1" << endl;
					}
					else
					{
						srcReg1 = registerManager.getDataValue(quotation.dst, funcIndex);
						int type = registerManager.getVarType(quotation.dst, funcIndex);
						if (type == INT_TYPE)
							MIPSCodeFile << "li $v0, 1" << endl;
						else
							MIPSCodeFile << "li $v0, 11" << endl;
					}
					MIPSCodeFile << "move $a0, $" << srcReg1 << endl;
					MIPSCodeFile << "syscall" << endl;
				}
				MIPSCodeFile << "#END PRINTF" << endl;
			break;
			}
		}

		quotationIndex++;
	}
	MIPSCodeFile << endl;
}

void MIPSCode::autoGenerator(int funcIndex)
{
	auto &funcTable = symbolTable.funcSymbolTables.at(funcIndex);
	int paraNum = funcTable.parameterTable.size();
	int autoNum = funcTable.dataTable.size();
	int currentOffset = paraNum*4;
	for(int i = 0; i < autoNum; i++)
	{
		auto &autoData = funcTable.dataTable.at(i);
		if (autoData.accessType == CONST_TYPE)
		{
			// autoData.addressOffset = currentOffset;
			currentOffset += 4;
			MIPSCodeFile << "li $t0, " << autoData.arrayLengthOrConstVal << endl;
			MIPSCodeFile << "sw $t0, " << to_string((paraNum + i) * 4) << "($sp)" << endl;
		}
		else
		{
			if (!autoData.isArray)
			{
				// autoData.addressOffset = currentOffset;
				currentOffset += 4;
			}
			else
			{
				// autoData.addressOffset = currentOffset;
				currentOffset += autoData.arrayLengthOrConstVal * 4;
			}
		}
	}
	MIPSCodeFile << "sw $ra, " << to_string(currentOffset) << "($sp)" << endl;
	MIPSCodeFile << "addi $fp, $sp, " << to_string(currentOffset+4) << endl;
}