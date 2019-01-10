#include "stdafx.h"
#include "IntermediateCode.h"

using namespace std;

void IntermediateCode::addQuotation(Quotation inQuotation)
{
#ifndef INTERMEDIATE_CODE_PRINT
	if (inQuotation.opType == LABEL)
	{
		intermediateCodeFile << inQuotation.dst << ":" << endl;
	}
	else if (inQuotation.opType == ASSIGN)
	{
		if (inQuotation.op == "ASSIGN")
			intermediateCodeFile << inQuotation.dst << " = " << inQuotation.src1 << endl;
		else
			intermediateCodeFile << inQuotation.dst << " = " << inQuotation.src1 << " " << inQuotation.op << " " << inQuotation.src2 << endl;
	} 
	else if (inQuotation.opType == PUSH)
	{
		intermediateCodeFile << "push " << inQuotation.dst << endl;
	} 
	else if (inQuotation.opType == FUNC_CALL)
	{
		intermediateCodeFile << "call " << inQuotation.dst << endl;
	}
	else if (inQuotation.opType == RET_VAL)
	{
		intermediateCodeFile << inQuotation.dst << " = RET" << endl;
	}
	else if (inQuotation.opType == RETURN)
	{
		intermediateCodeFile << "ret " << inQuotation.op << " " << inQuotation.dst << endl;
	}
	else if	(inQuotation.opType == COMPARE)
	{
		if (inQuotation.op == "BEZ" || inQuotation.op == "BNZ")
			intermediateCodeFile << inQuotation.op << " " << inQuotation.src1 << ", " << inQuotation.dst << endl;
		else
			intermediateCodeFile << inQuotation.op << " " << inQuotation.src1 << ", " << inQuotation.src2 << ", " << inQuotation.dst << endl;
	}
	else if (inQuotation.opType == GOTO)
	{
		intermediateCodeFile << "j " << inQuotation.dst << endl;
	}
	else if	(inQuotation.opType == ARRAY_GET)
	{
		intermediateCodeFile << inQuotation.dst << " = " << inQuotation.src1 << "[" << inQuotation.src2 << "]" << endl;
	}
	else if (inQuotation.opType == LI)
	{
		intermediateCodeFile << "li " << inQuotation.dst << ", " << inQuotation.srcNum1 << endl;
	}
	else if	(inQuotation.opType == NEGATIVE)
	{
		intermediateCodeFile << "neg " << inQuotation.dst << ", " << inQuotation.src1 << endl; 
	}
	else if (inQuotation.opType == ARRAY_ASSIGN)
	{
		intermediateCodeFile << inQuotation.dst << " = &" << inQuotation.src1 << "[" << inQuotation.src2 << "]" << endl;
	}
	else if	(inQuotation.opType == SCANF)
	{
		intermediateCodeFile << "scanf " << inQuotation.dst << endl;
	}
	else if (inQuotation.opType == PRINTF)
	{
		if (inQuotation.op == "STRING")
		{
			stringList.push_back(inQuotation.dst);
			inQuotation.srcNum1 = stringList.size()-1;
			intermediateCodeFile << "printf " << inQuotation.op << " " << stringList.at(inQuotation.srcNum1) << endl;
		} 
		else
			intermediateCodeFile << "printf " << inQuotation.op << " " << inQuotation.dst << endl;
	}
	else if (inQuotation.opType == FUNC_LABEL)
	{
		
		intermediateCodeFile << inQuotation.dst << ":" << endl;
	}
	else if (inQuotation.opType == FUNC_CALL_START_LABEL)
	{
		intermediateCodeFile << "A FUNC CALL BEGIN BELOW:" << endl;
	}
	else
	{
		intermediateCodeFile << "FATAL ERROR: UNKNOWN QUOTATION TYPE" << endl; 
	}
#endif
	quotations.push_back(inQuotation);
}
