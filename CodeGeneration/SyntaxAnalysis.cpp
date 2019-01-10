#include "stdafx.h"
#include "SyntaxAnalysis.h"
#include "LexicalAnalysis.h"
#include "SymbolTable.h"
#include "IntermediateCode.h"
#include "LabelGenerator.h"
#include "TempVarGenerator.h"

using namespace std;

SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis *la, SymbolTable &_symbolTable, IntermediateCode &_intermediateCode, LabelGenerator &_labelGenerator, TempVarGenerator &_tempVarGenerator) :
	symbolTable{_symbolTable}, intermediateCode{_intermediateCode}, labelGenerator{ _labelGenerator }, tempVarGenerator{ _tempVarGenerator }
{
	cout << "===============Syntax ANALYSIS STARTED====================" << endl;
	lexicalAnalysis = la;
}

inline bool SyntaxAnalysis::getSym() const
{
	return lexicalAnalysis->nextSym();
}

inline int SyntaxAnalysis::getSymType() const
{
	return lexicalAnalysis->getSymbolType();
}

inline string SyntaxAnalysis::getToken() const
{
	return lexicalAnalysis->getToken();
}

inline int SyntaxAnalysis::getIntValue() const
{
	return lexicalAnalysis->getIntValue();
}

inline int SyntaxAnalysis::getLineNum() const
{
	return lexicalAnalysis->getLineNum();
}

inline int SyntaxAnalysis::getIndexInLine() const
{
	return lexicalAnalysis->getIndexInLine();
}

void SyntaxAnalysis::skipTo(int symType)
{
	do
	{
		if (!getSym())
		{
			return;
		}
	}
	while (getSymType() != symType);
}


void SyntaxAnalysis::run()
{
	if (!getSym())
		error.printError(EMPTY_FILE);
	// handle const
	if (getSymType() == CONST_SY)
	{
		constDeclare();
	}
	// handle global variable
	if (getSymType() == INT_SY || getSymType() == CHAR_SY)
	{
		globalVarHandler();
	}
// #ifndef INTERMEDIATE_CODE_PRINT
// 	symbolTable.printGlobal();
// #endif
	// handle func
	while (getSymType() == INT_SY || getSymType() == CHAR_SY || getSymType() == VOID_SY)
	{
		int returnType = getSymType() == INT_SY ? RETURN_INT : (getSymType() == CHAR_SY ? RETURN_CHAR : RETURN_VOID);
		getSym();
		if (getSymType() == IDEN_SY)
		{
			this->identifier = getToken();
			getSym();
			if (getSymType() == OPEN_PAREN_SY)
			{
				funcDeclare(returnType, true);
			}
			else if (getSymType() == OPEN_CURLY_SY)
			{
				funcDeclare(returnType, false);
			}
			else
			{
				error.printError(getLineNum(), getIndexInLine(), ERROR_TOKEN_IN_FUNC, getToken());
				cout <<
					"Expected [{] or [(] after the declare of func. This func will be treated as no parameter func as default."
					<< endl;
				funcDeclare(returnType, false);
			}
		}
		else if (getSymType() == MAIN_SY)
		{
			mainDeclare();
			break;
		}
		else
		{
			error.printError(getLineNum(), getIndexInLine(), INVALID_NAME_OF_FUNC, getToken());
			cout << "Invalid name of func, jumping to next [}]" << endl;
			skipTo(CLOSE_CURLY_SY);
			getSym();
		}
	}
	cout << "===============Syntax ANALYSIS FINISHED====================" << endl;
}

inline void SyntaxAnalysis::constDeclare()
{
	constDeclare(true);
}

void SyntaxAnalysis::constDeclare(bool isGlobal)
{
	while (true)
	{
		getSym();
		if (getSymType() == INT_SY || getSymType() == CHAR_SY)
		{
			if (getSymType() == INT_SY)
			{
				constIntDefine(isGlobal);
			}
			else
			{
				constCharDefine(isGlobal);
			}

			if (getSymType() != SEMI_SY)
			{
				error.printError(getLineNum(), getIndexInLine(), MISSING_SEMI_SY, getToken());
			}
		}
		else
		{
			error.printWarning(getLineNum(), getIndexInLine(), INVALID_CONST_TYPE, getToken());
			cout << "Invalid const type, jumping to next [;] and continue to compile." << endl;
			skipTo(SEMI_SY);
		}
		getSym();
		if (getSymType() != CONST_SY)
			return;
	}
}

void SyntaxAnalysis::constIntDefine(bool isGlobal)
{
	while (true)
	{
		getSym();
		if (getSymType() == IDEN_SY)
		{
			string constName = getToken();
			getSym();
			if (getSymType() == ASSIGN_SY)
			{
				getSym();
				int sign = 1;
				if (getSymType() == ADD_SY)
				{
					getSym();
				}
				else if (getSymType() == MINUS_SY)
				{
					sign = -1;
					getSym();
				}
				else
				{
					sign = 1;
				}
				if (getSymType() != DIGIT)
				{
					error.printError(getLineNum(), getIndexInLine(), INVALID_VALUE_TO_INITIALIZE_CONST, getToken());
					cout << "Invalid value to initialize a const int, jumping to next [;]" << endl;
					skipTo(SEMI_SY);
					return;;
				}
				// TODO: add const int: name [constName], value [getIntValue] to const table
				
				if (isGlobal)
				{
					SymbolTableElement newConstInt;
					newConstInt.name = constName;
					newConstInt.accessType = CONST_TYPE;
					newConstInt.dataType = INT_TYPE;
					newConstInt.isArray = false;
					newConstInt.arrayLengthOrConstVal = sign * getIntValue();
					if (!symbolTable.addGlobal(newConstInt))
					{
						error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, constName);
					}
				}
				else
				{
					if (!symbolTable.addData(constName, CONST_TYPE, INT_TYPE, false, sign*getIntValue()))
					{
						error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, constName);
					}
				}
#ifndef SYNTAX_DEBUG_PRINT
				cout << "Const Int Define: " << constName << ",\tint value: " << sign * getIntValue() << endl;
#endif // !SYNTAX_DEBUG_PRINT
				getSym();
				if (getSymType() != COMMA_SY)
				{
					return;
				}
			}
			else
			{
				error.printError(getLineNum(), getIndexInLine(), MISSING_ASSIGN_SY_IN_CONST_DEFINE, getToken());
				skipTo(SEMI_SY);
				return;;
			}
		}
		else
		{
			error.printError(getLineNum(), getIndexInLine(), INVALID_CONST_NAME, getToken());
			cout << "Invalid name of const, jumping to [;]" << endl;
			skipTo(SEMI_SY);
			return;
		}
	}
}

void SyntaxAnalysis::constCharDefine(bool isGlobal)
{
	while (true)
	{
		getSym();
		if (getSymType() == IDEN_SY)
		{
			string constName = getToken();
			getSym();
			if (getSymType() == ASSIGN_SY)
			{
				getSym();
				if (getSymType() != CHARACTER)
				{
					error.printError(getLineNum(), getIndexInLine(), INVALID_VALUE_TO_INITIALIZE_CONST, getToken());
					cout << "Invalid value to initialize a const int, jumping to next [;]" << endl;
					skipTo(SEMI_SY);
					return;
				}
				// TODO: add const char: name [constName], value [getIntValue] to const table
				// auto &newConstChar = symbolTable.addGlobal();
				if (isGlobal)
				{
					SymbolTableElement newConstChar;
					newConstChar.name = constName;
					newConstChar.accessType = CONST_TYPE;
					newConstChar.dataType = CHAR_TYPE;
					newConstChar.isArray = false;
					newConstChar.arrayLengthOrConstVal = getIntValue();
					if (!symbolTable.addGlobal(newConstChar))
					{
						error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, constName);
					}
				}
				else
				{
					if (!symbolTable.addData(constName, CONST_TYPE, CHAR_TYPE, false, getIntValue()))
					{
						error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, constName);
					}
				}
				
#ifndef SYNTAX_DEBUG_PRINT
				cout << "Const Char Define: " << constName << ",\tchar: " << getToken() << endl;
#endif // !SYNTAX_DEBUG_PRINT
				getSym();
				if (getSymType() != COMMA_SY)
				{
					return;
				}
			}
			else
			{
				error.printError(getLineNum(), getIndexInLine(), MISSING_ASSIGN_SY_IN_CONST_DEFINE, getToken());
				skipTo(SEMI_SY);
				return;;
			}
		}
		else
		{
			error.printError(getLineNum(), getIndexInLine(), INVALID_CONST_NAME, getToken());
			cout << "Invalid name of const, jumping to [;]" << endl;
			skipTo(SEMI_SY);
			return;
		}
	}
}

void SyntaxAnalysis::globalVarHandler()
{
	while (true)
	{
		if (getSymType() != INT_SY && getSymType() != CHAR_SY && getSymType() != MAIN_SY && getSymType() != VOID_SY)
		{
			cout << "Invalid token: " << getToken() << " , maybe wrong statement outside any function." << endl;
			cout << "Jumping to next [int], [char], [void], or [main]." << endl;
			do
			{
				if (!getSym())
				{
					break;
				}
			}
			while (getSymType() != INT_SY && getSymType() != CHAR_SY && getSymType() != MAIN_SY && getSymType() !=
				VOID_SY);
		}
		if (getSymType() != INT_SY && getSymType() != CHAR_SY)
		{
			return;
		}
		int type = ((getSymType() == INT_SY) ? 0 : 1);
		bool isArray = false;
		getSym();
		if (getSymType() == IDEN_SY)
		{
			this->identifier = getToken();
			getSym();
			if (getSymType() == OPEN_BRACKET_SY)
			{
				getSym();
				if (getSymType() == DIGIT)
				{
					dimOfArray = getIntValue();
					getSym();
					if (getSymType() == CLOSE_BRACKET_SY)
					{
						isArray = true;
						getSym();
					}
					else
					{
						error.printError(getLineNum(), getIndexInLine(), MISSING_CLOSE_BRACKET_OF_ARRAY_DEFINE,
						                 getToken());
						cout << "Missing \"[\" of the define of array dim, this will be treated as a norm." << endl;
						skipTo(CLOSE_BRACKET_SY);
						getSym();
					}
				}
				else
				{
					error.printError(getLineNum(), getIndexInLine(), INVALID_DIM_OF_ARRAY, getToken());
					cout << "Invalid dim of array, this will be treated as a norm." << endl;
					skipTo(CLOSE_BRACKET_SY);
					getSym();
				}
			}
			if (getSymType() == COMMA_SY)
			{
				// TODO: add variable to variable table: name:[this->identifier], type[type = 0:int, 1:char], isArray[isArray], dim[this->dimOfArray]
				// auto &newVariable = symbolTable.addGlobal();
				SymbolTableElement newVariable;
				newVariable.name = this->identifier;
				newVariable.dataType = type;
				newVariable.accessType = VARIABLE_TYPE;
				newVariable.isArray = isArray;
				newVariable.arrayLengthOrConstVal = (isArray ? this->dimOfArray : 0);
				if (!symbolTable.addGlobal(newVariable))
				{
					error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, this->identifier);
				}
#ifndef SYNTAX_DEBUG_PRINT
				if (!isArray)
					cout << "Global Variable Define: " << this->identifier << ",\ttype: " << (type == 0 ? "int" : "char"
					) << endl;
				else
					cout << "Global Array Define: " << this->identifier << ",\ttype: " << (type == 0 ? "int" : "char")
						<< ",\tlength: " << dimOfArray << endl;
#endif // !SYNTAX_DEBUG_PRINT
				while (getSymType() == COMMA_SY)
				{
					getSym();
					if (getSymType() == IDEN_SY)
					{
						this->identifier = getToken();
						getSym();
						if (getSymType() == OPEN_BRACKET_SY)
						{
							getSym();
							if (getSymType() == DIGIT)
							{
								dimOfArray = getIntValue();
								getSym();
								if (getSymType() == CLOSE_BRACKET_SY)
								{
									isArray = true;
									getSym();
								}
								else
								{
									error.printError(getLineNum(), getIndexInLine(),
									                 MISSING_CLOSE_BRACKET_OF_ARRAY_DEFINE, getToken());
									cout << "Missing \"[\" of the define of array dim, this will be treated as a norm."
										<< endl;
									skipTo(CLOSE_BRACKET_SY);
									getSym();
								}
							}
							else
							{
								error.printError(getLineNum(), getIndexInLine(), INVALID_DIM_OF_ARRAY, getToken());
								cout << "Invalid dim of array, this will be treated as a norm." << endl;
								skipTo(CLOSE_BRACKET_SY);
								getSym();
							}
						}
						else
							isArray = false;
						// TODO: add variable to variable table: name:[this->identifier], type[type = 0:int, 1:char], isArray[isArray], dim[this->dimOfArray]
						// auto &newVariable = symbolTable.addGlobal();
						SymbolTableElement newVariable;
						newVariable.name = this->identifier;
						newVariable.dataType = type;
						newVariable.accessType = VARIABLE_TYPE;
						newVariable.isArray = isArray;
						newVariable.arrayLengthOrConstVal = (isArray ? this->dimOfArray : 0);
						if (!symbolTable.addGlobal(newVariable))
						{
							error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, this->identifier);
						}
#ifndef SYNTAX_DEBUG_PRINT
						if (!isArray)
							cout << "Global Variable Define: " << this->identifier << ",\ttype: " << (
								type == 0 ? "int" : "char") << endl;
						else
							cout << "Global Array Define: " << this->identifier << ",\ttype: " << (
								type == 0 ? "int" : "char") << ",\tlength: " << dimOfArray << endl;
#endif // !SYNTAX_DEBUG_PRINT
					}
					else
					{
						error.printError(getLineNum(), getIndexInLine(), INVALID_NAME_OF_VARIABLE, getToken());
						cout << "Invalid name of a variable define, jumping to [;]" << endl;
						skipTo(SEMI_SY);
						break;
					}
				}
				if (getSymType() == SEMI_SY)
				{
					getSym();
					continue;
				}
				else
				{
					error.printError(getLineNum(), getIndexInLine(), MISSING_SEMI_SY_AFTER_VARIABLE_DEFINE, getToken());
					cout << "Expected [;] after the variable define, jumping to next [;]" << endl;
					skipTo(SEMI_SY);
					getSym();
				}
			}
			else if (getSymType() == OPEN_PAREN_SY)
			{
				if (type == 0)
					funcDeclare(RETURN_INT, true);
				else
					funcDeclare(RETURN_CHAR, true);
				return;
			}
			else if (getSymType() == OPEN_CURLY_SY)
			{
				if (type == 0)
					funcDeclare(RETURN_INT, false);
				else
					funcDeclare(RETURN_CHAR, false);
				return;
			}
			else if (getSymType() == SEMI_SY)
			{
				// TODO: add variable to variable table: name:[this->identifier], type[type = 0:int, 1:char], isArray[isArray], dim[this->dimOfArray]
				// auto &newVariable = symbolTable.addGlobal();
				SymbolTableElement newVariable;
				newVariable.name = this->identifier;
				newVariable.dataType = type;
				newVariable.accessType = VARIABLE_TYPE;
				newVariable.isArray = isArray;
				newVariable.arrayLengthOrConstVal = (isArray ? this->dimOfArray : 0);
				if (!symbolTable.addGlobal(newVariable))
				{
					error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, this->identifier);
				}
#ifndef SYNTAX_DEBUG_PRINT
				if (!isArray)
					cout << "Global Variable Define: " << this->identifier << ",\ttype: " << (type == 0 ? "int" : "char"
					) << endl;
				else
					cout << "Global Array Define: " << this->identifier << ",\ttype: " << (type == 0 ? "int" : "char")
						<< ",\tlength: " << dimOfArray << endl;
#endif // !SYNTAX_DEBUG_PRINT
				// cout << "---- " << getToken() << endl;
				getSym();
				// cout << "---- " << getToken() << endl;
			}
			else if (getSymType() == ASSIGN_SY)
			{
				error.printError(getLineNum(), getIndexInLine(), INITIALIZE_VARIABLE, getToken());
				cout << "Variable should not be initialized, jumping to [;]" << endl;
				skipTo(SEMI_SY);
				getSym();
			}
			else
			{
				error.printError(getLineNum(), getIndexInLine(), UNABLE_TO_JUDGE_VARIABLE_OR_FUNC, getToken());
				cout <<
					"Invalid token after a define of variable, unable to judge this is a variable of function, jumping to [;]"
					<< endl;
				skipTo(SEMI_SY);
				getSym();
			}
		}
		else
		{
			error.printError(getLineNum(), getIndexInLine(), INVALID_NAME_OF_VARIABLE, getToken());
			cout << "Invalid name of int/char variable or int/char func, jumping to [;]" << endl;
			skipTo(SEMI_SY);
			return;
		}
	}
}

void SyntaxAnalysis::funcDeclare(int returnType, bool hasParaList)
{
	if (!symbolTable.addFuc(this->identifier, returnType))
	{
		error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, this->identifier);
		skipTo(CLOSE_CURLY_SY);
		return;
	}
	
	int paraNum = 0;
	if (hasParaList)
	{
		paraNum = paraList();
	}
	// TODO: add function to symbol table, name[this->identifier], return type: [returnType], parameter number:[paraNum]
#ifndef SYNTAX_DEBUG_PRINT
	cout << "Function: " << this->identifier << ", return type: " << returnType << ", parameter number: " << paraNum <<
		endl;
#endif

	getSym();
	if (getSymType() == CONST_SY)
	{
		constDeclare(false);
	}

	if (getSymType() == INT_SY || getSymType() == CHAR_SY)
	{
		autoVariableDeclare();
	}

#ifndef INTERMEDIATE_CODE_PRINT
	symbolTable.printFunc();
#endif
	Quotation newQuotation;
	newQuotation.opType = FUNC_LABEL;
	newQuotation.dst = symbolTable.funcSymbolTables.back().name;
	intermediateCode.addQuotation(newQuotation);

	statementsList();

	Quotation defaultReturnQuotation;
	defaultReturnQuotation.opType = RETURN;
	defaultReturnQuotation.op = "DEFAULT_RETURN";
	intermediateCode.addQuotation(defaultReturnQuotation);

	if (getSymType() != CLOSE_CURLY_SY)
	{
		error.printError(MISSING_CLOSE_CURLY_OF_FUNCTION_DEFINE);
		skipTo(CLOSE_CURLY_SY);
	}
	getSym();
}

int SyntaxAnalysis::paraList()
{
	int paraNum = 0;
	while (true)
	{
		int type = 0;
		getSym();
		if (getSymType() == CHAR_SY)
		{
			type = 1;
		}
		else
		{
			if (getSymType() != INT_SY)
			{
				error.printError(getLineNum(), getIndexInLine(), INVALID_TYPE_OF_PARAMETER, getToken());
				cout << "Invalid parameter type, set to [int] type as default." << endl;
			}
		}
		getSym();
		if (getSymType() != IDEN_SY)
		{
			error.printError(getLineNum(), getIndexInLine(), INVALID_NAME_OF_PARAMETER, getToken());
			cout << "Invalid name of parameter, this parameter will be ignored." << endl;
		}
		else
		{
			// TODO: add parameter to symbol table: name: [getToken()], type: [type = 0:int, 1:char]
			if (!symbolTable.addPara(getToken(), type))
			{
				error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, getToken());
			} else
			{
				paraNum++;
			}
#ifndef SYNTAX_DEBUG_PRINT
			cout << "parameter" << paraNum << ", name: " << getToken() << ", type: " << (type == 0 ? "int" : "char") <<
				endl;
#endif
		}
		getSym();
		if (getSymType() == COMMA_SY)
			continue;
		else if (getSymType() == CLOSE_PAREN_SY)
		{
			getSym();
			break;
		}
		else
		{
			error.printError(getLineNum(), getIndexInLine(), INVALID_TOKEN_IN_PARALIST, getToken());
			cout << "Invalid token in parameter list, jump to next [)]" << endl;
			skipTo(CLOSE_PAREN_SY);
			getSym();
			break;
		}
	}
	if (getSymType() != OPEN_CURLY_SY)
	{
		error.printError(getLineNum(), getIndexInLine(), MISSING_OPEN_CURLY_OF_FUNCTION, getToken());
		cout << "Missing the open curly of the func, ignore this open curly as default." << endl;
	}
	return paraNum;
}

void SyntaxAnalysis::autoVariableDeclare()
{
	while (getSymType() == INT_SY || getSymType() == CHAR_SY)
	{
		autoVariableDefine();
	}
}

void SyntaxAnalysis::autoVariableDefine()
{
	int type = (getSymType() == INT_SY ? 0 : 1);
	getSym();
	if (getSymType() == IDEN_SY)
	{
		while (getSymType() == IDEN_SY)
		{
			this->identifier = getToken();
			bool isArray = false;
			getSym();
			if (getSymType() == OPEN_BRACKET_SY)
			{
				getSym();
				if (getSymType() == DIGIT)
				{
					dimOfArray = getIntValue();
					getSym();
					if (getSymType() == CLOSE_BRACKET_SY)
					{
						isArray = true;
						getSym();
					}
					else
					{
						error.printError(getLineNum(), getIndexInLine(), MISSING_CLOSE_BRACKET_OF_ARRAY_DEFINE,
						                 getToken());
						cout << "Missing \"]\" of the define of array dim, jump to next \';\'." << endl;
						skipTo(SEMI_SY);
						break;
					}
				}
				else
				{
					error.printError(getLineNum(), getIndexInLine(), INVALID_DIM_OF_ARRAY, getToken());
					cout << "Invalid dim of array, this will be treated as a norm." << endl;
					skipTo(CLOSE_BRACKET_SY);
					getSym();
				}
			}
			if (getSymType() == COMMA_SY)
			{
				// TODO: add variable to variable table: name:[this->identifier], type[type = 0:int, 1:char], isArray[isArray], dim[this->dimOfArray]
				if (!symbolTable.addData(this->identifier, VARIABLE_TYPE, type, isArray, this->dimOfArray))
				{
					error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, this->identifier);
				}
#ifndef SYNTAX_DEBUG_PRINT
				if (!isArray)
					cout << "Auto Variable Define: " << this->identifier << ",\ttype: " << (type == 0 ? "int" : "char")
						<< endl;
				else
					cout << "Auto Array Define: " << this->identifier << ",\ttype: " << (type == 0 ? "int" : "char") <<
						",\tlength: " << dimOfArray << endl;
#endif // !SYNTAX_DEBUG_PRINT
				getSym();
			}
			else if (getSymType() == SEMI_SY)
			{
				// TODO: add variable to variable table: name:[this->identifier], type[type = 0:int, 1:char], isArray[isArray], dim[this->dimOfArray]
				if (!symbolTable.addData(this->identifier, VARIABLE_TYPE, type, isArray, this->dimOfArray))
				{
					error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, this->identifier);
				}
#ifndef SYNTAX_DEBUG_PRINT
				if (!isArray)
					cout << "Auto Variable Define: " << this->identifier << ",\ttype: " << (type == 0 ? "int" : "char")
						<< endl;
				else
					cout << "Auto Array Define: " << this->identifier << ",\ttype: " << (type == 0 ? "int" : "char") <<
						",\tlength: " << dimOfArray << endl;
#endif // !SYNTAX_DEBUG_PRINT
			}
			else
			{
				error.printError(getLineNum(), getIndexInLine(), INVALID_SIGNAL_IN_AUTO_DEFINE, getToken());
				skipTo(SEMI_SY);
			}
		}
	}
	else
	{
		error.printError(getLineNum(), getIndexInLine(), INVALID_NAME_OF_VARIABLE, getToken());
		cout << "Invalid name of an auto variable, jump to next [;]" << endl;
		skipTo(SEMI_SY);
	}

	if (getSymType() != SEMI_SY)
	{
		error.printError(getLineNum(), getIndexInLine(), MISSING_SEMI_SY, getToken());
		cout << "Missing semicolon, ignore this and continue." << endl;
	}
	getSym();
}

void SyntaxAnalysis::statementsList()
{
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[STATEMENTS LIST] This is the statements list." << endl;
	int statementsCount = 0;
#endif
	while (getSymType() == IF_SY || getSymType() == DO_SY || getSymType() == OPEN_CURLY_SY ||
		getSymType() == SWITCH_SY || getSymType() == IDEN_SY || getSymType() == SCANF_SY ||
		getSymType() == PRINTF_SY || getSymType() == SEMI_SY || getSymType() == RETURN_SY)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[STATEMENTS LIST] This is statement" << ++statementsCount << endl;
#endif
		statement();
	}
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[STATEMENTS LIST] " << statementsCount << " statements in total." << endl;
#endif
}

void SyntaxAnalysis::statement()
{
	if (getSymType() == IF_SY)
	{
		conditionStatement();
	}
	else if (getSymType() == DO_SY)
	{
		loopStatement();
	}
	else if (getSymType() == OPEN_CURLY_SY)
	{
		getSym();
		statementsList();
		if (getSymType() != CLOSE_CURLY_SY)
		{
			error.printError(getLineNum(), getIndexInLine(), MISSING_CLOSE_CURLY_OF_CODE_BLOCK, getToken());
		}
		getSym();
	}
	else if (getSymType() == SWITCH_SY)
	{
		switchStatement();
	}
	else if (getSymType() == IDEN_SY)
	{
		this->identifier = getToken();
		string src1 = getToken();
		// TODO
		getSym();
		if (getSymType() == OPEN_PAREN_SY)
		{
#ifndef SYNTAX_DEBUG_PRINT
			cout << "[STATEMENT FUNCTION CALL]This calls a function \"" << this->identifier <<
				"\" with parameter list. " << endl;
#endif
			int funcIndex = symbolTable.funcSearch(src1);

			if (funcIndex == -1)
			{
				error.printError(getLineNum(), getIndexInLine(), UNDEFINED_FUNCTION, getToken());
			}
			if (symbolTable.funcSymbolTables.at(funcIndex).parameterTable.size() == 0)
			{
				error.printError(getLineNum(), getIndexInLine(), WRONG_FUNC_CALL_OF_NO_PARA_FUNC, getToken());
			}

			paraValueList(funcIndex);
			Quotation funcCallQuotation;
			funcCallQuotation.opType = FUNC_CALL;
			funcCallQuotation.dst = src1;
			intermediateCode.addQuotation(funcCallQuotation);
			// string funcRes = tempVarGenerator.generateTempVar();
			// Quotation retValueQuotaion;
			// retValueQuotaion.opType = RET_VAL;
			// retValueQuotaion.dst = funcRes;
			// intermediateCode.addQuotation(retValueQuotaion);
			if (getSymType() != CLOSE_PAREN_SY)
			{
				error.printError(getLineNum(), getIndexInLine(), MISSING_THE_CLOSE_PAREN_OF_FUNC_CALL, getToken());
				return;
			}
			getSym();
		}
		else if (getSymType() == SEMI_SY)
		{
#ifndef SYNTAX_DEBUG_PRINT
			cout << "[STATEMENT FUNCTION CALL]This calls a function \"" << this->identifier <<
				"\" without parameter list. " << endl;
#endif
			int funcIndex = symbolTable.funcSearch(src1);
			if (funcIndex != -1)
			{
				Quotation funcCallStartQuotation;
				funcCallStartQuotation.opType = FUNC_CALL_START_LABEL;
				intermediateCode.addQuotation(funcCallStartQuotation);
				Quotation funcCallQuotation;
				funcCallQuotation.opType = FUNC_CALL;
				funcCallQuotation.dst = src1;
				intermediateCode.addQuotation(funcCallQuotation);
				// string funcRes = tempVarGenerator.generateTempVar();
				// Quotation retValQuotation;
				// retValQuotation.opType = RET_VAL;
				// retValQuotation.dst = funcRes;
				// intermediateCode.addQuotation(retValQuotation);
			}
			else
			{
				SymbolTableElement normElement;
				if (!symbolTable.dataSearch(normElement, src1))
					error.printError(getLineNum(), getIndexInLine(), NO_DECLARATION, src1);
				else
					error.printWarning(getLineNum(), getIndexInLine(), THROWABLE_DATA, getToken());
			}
		}
		else
		{
			if (getSymType() == OPEN_BRACKET_SY)
			{
#ifndef SYNTAX_DEBUG_PRINT
				cout << "[STATEMENT ASSIGNMENT]This is a element of an array. " << endl;
#endif
				SymbolTableElement arrayElement;
				if (!symbolTable.dataSearch(arrayElement, src1))
					error.printError(getLineNum(), getIndexInLine(), NO_DECLARATION, src1);
				if (!arrayElement.isArray)
					error.printError(getLineNum(), getIndexInLine(), NOT_A_ARRAY, src1);
				getSym();

				// TODO ARRAY ASSIGN ORDER
				string src2 = expression();
				string dst = tempVarGenerator.generateTempVar();
				Quotation arrayAssignQuotation;
				arrayAssignQuotation.opType = ARRAY_ASSIGN;
				arrayAssignQuotation.dst = dst;
				arrayAssignQuotation.src1 = src1;
				arrayAssignQuotation.src2 = src2;
				intermediateCode.addQuotation(arrayAssignQuotation);


				if (getSymType() != CLOSE_BRACKET_SY)
				{
					error.printError(getLineNum(), getIndexInLine(), MISSING_THE_CLOSE_BRACKET_OF_ELEMENT_OF_ARRAY,
					                 getToken());
					skipTo(SEMI_SY);
					
				}
				else
					getSym();
				if (getSymType() != ASSIGN_SY)
				{
					error.printError(INVALID_SYMBOL_IN_ASSIGNMENT);
					skipTo(SEMI_SY);
				}
				else
				{
					getSym();
					string newVal = expression();
					// TODO EDITED
					// intermediateCode.addQuotation(arrayAssignQuotation);
					Quotation newValQuotation;
					newValQuotation.opType = ASSIGN;
					newValQuotation.dst = dst;
					newValQuotation.src1 = newVal;
					newValQuotation.op = "ASSIGN";
					intermediateCode.addQuotation(newValQuotation);
				}
			}
			else if (getSymType() == ASSIGN_SY)
			{
#ifndef SYNTAX_DEBUG_PRINT
				cout << "[STATEMENT ASSIGNMENT]This might be a norm. " << endl;
#endif
				SymbolTableElement normElemeant;
				if (!symbolTable.dataSearch(normElemeant, src1))
					error.printError(getLineNum(), getIndexInLine(), NO_DECLARATION, src1);
				if (normElemeant.isArray)
					error.printError(getLineNum(), getIndexInLine(), NOT_A_NORM, src1);
				getSym();
				string newVal = expression();
				Quotation newValQuotation;
				newValQuotation.opType = ASSIGN;
				newValQuotation.dst = src1;
				newValQuotation.src1 = newVal;
				newValQuotation.op = "ASSIGN";
				intermediateCode.addQuotation(newValQuotation);
			}
			else
			{
				error.printError(INVALID_SYMBOL_IN_ASSIGNMENT);
				skipTo(SEMI_SY);
			}
		}
		if (getSymType() != SEMI_SY)
		{
			error.printError(MISSING_SEMI_IN_ASSIGN_OR_FUNC_CALL);
			skipTo(SEMI_SY);
		}
		getSym();
	}
	else if (getSymType() == SCANF_SY)
	{
		scanfStatement();
	}
	else if (getSymType() == PRINTF_SY)
	{
		printfStatement();
	}
	else if (getSymType() == SEMI_SY)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[STATEMENT]Empty statement." << endl;
#endif
		getSym();
	}
	else if (getSymType() == RETURN_SY)
	{
		returnStatement();
	}
	else
	{
		error.printError(getLineNum(), getIndexInLine(), INVALID_STATEMENT, getToken());
		skipTo(SEMI_SY);
	}
}

void SyntaxAnalysis::conditionStatement()
{
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[IF CONDITION]This is a \"if\" statement." << endl;
#endif
	static int count = 0;
	count++;
	string ifLabel = labelGenerator.getLabel("if_" + std::to_string(count));
	Quotation newQuotation;
	newQuotation.opType = LABEL;
	newQuotation.dst = ifLabel;
	intermediateCode.addQuotation(newQuotation);
	string elseLabel = labelGenerator.getLabel("else_" + std::to_string(count));
	string endifLabel = labelGenerator.getLabel("endif_" + std::to_string(count));
	getSym();
	if (getSymType() != OPEN_PAREN_SY)
	{
		error.printError(getLineNum(), getIndexInLine(), MISSING_OPEN_CURLY_OF_CONDITION, getToken());
		cout << "Miss the \'(\' of condition, ignore this and continue." << endl;
	}
	getSym();
	compareConditionStatement(elseLabel);
	// TODO decide: jump to "if" unsatisfied(else) label?
	if (getSymType() != CLOSE_PAREN_SY)
	{
		error.printError(getLineNum(), getIndexInLine(), MISSING_CLOSE_CURLY_OF_CONDITION, getToken());
		cout << "Miss the \')\' of condition, ignore this and continue." << endl;
	}
	getSym();
	statement();
	// TODO jump to endif label
	Quotation jumpQuotation;
	jumpQuotation.opType = GOTO;
	jumpQuotation.dst = endifLabel;
	intermediateCode.addQuotation(jumpQuotation);
	// TODO make else label
	Quotation elseQuotation;
	elseQuotation.opType = LABEL;
	elseQuotation.dst = elseLabel;
	intermediateCode.addQuotation(elseQuotation);

	if (getSymType() == ELSE_SY)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[IF CONDITION]The \"if\" statement has \"else\" statement." << endl;
#endif
		getSym();
		statement();
	}
	else
	{
		error.printError(getLineNum(), getIndexInLine(), NO_ELSE, getToken());
	}
	// TODO make endif label
	Quotation endifQuotation;
	endifQuotation.opType = LABEL;
	endifQuotation.dst = endifLabel;
	intermediateCode.addQuotation(endifQuotation);
}

void SyntaxAnalysis::compareConditionStatement(string elseLabel)
{
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[COMPARE CONDITION]This is the condition statement of the \"if\" statement." << endl;
#endif
	string src1 = expression();
	if (getSymType() == CLOSE_PAREN_SY)
	{
		// TODO beqz, if expression is zero, jump to else label or endif
		Quotation newQuotation;
		newQuotation.opType = COMPARE;
		newQuotation.dst = elseLabel;
		newQuotation.src1 = src1;
		newQuotation.op = "bez"; // src1为0跳转至else，否则继续执行
		intermediateCode.addQuotation(newQuotation);
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[COMPARE CONDITION]BEQZ, if the expression equals to zero, jump." << endl;
#endif
	}
	else if (getSymType() >= LESS_SY && getSymType() <= EQUAL_SY)
	{
		auto compareSymbol = getSymType();
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[COMPARE CONDITION]If the relation between expression1 and expression2 satisfy " << compareSymbol <<
			", then don\'t jump to else/endif." << endl;
#endif
		getSym();
		string src2 = expression();
		Quotation newQuotation;
		newQuotation.opType = COMPARE;
		newQuotation.dst = elseLabel;
		newQuotation.src1 = src1;
		newQuotation.src2 = src2;
		switch(compareSymbol)
		{
		case LESS_SY: newQuotation.op = "bge"; break;
		case GREATER_SY: newQuotation.op = "ble"; break;
		case LESS_OR_EQUAL_SY: newQuotation.op = "bgt"; break;
		case GREATER_OR_EQUAL_SY: newQuotation.op = "blt"; break;
		case EQUAL_SY: newQuotation.op = "bne"; break;
		case NOT_EQUAL_SY: newQuotation.op = "beq"; break;
		}
		intermediateCode.addQuotation(newQuotation);
	}
	else
	{
		error.printError(getLineNum(), getIndexInLine(), INVALID_COMPARE_SIMBOL, getToken());
		skipTo(SEMI_SY);
	}
}

string SyntaxAnalysis::expression()
{
	int sign = 1;
	if (getSymType() == ADD_SY)
	{
		sign = 1;
		getSym();
	}
	else if (getSymType() == MINUS_SY)
	{
		sign = -1;
		getSym();
	}
	string src1 = item();
	if (sign == -1)
	{
		string temp = tempVarGenerator.generateTempVar();
		// TODO NEG ORDER
		Quotation negativeQuotation;
		negativeQuotation.opType = NEGATIVE;
		negativeQuotation.dst = temp;
		negativeQuotation.src1 = src1;
		intermediateCode.addQuotation(negativeQuotation);
		src1 = temp;
	}
	if (getSymType() != ADD_SY && getSymType() != MINUS_SY)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[EXPRESSION]The expression is " << (sign > 0 ? "positive " : "negative ") << "value of the item." <<
			endl;
#endif
		return src1;
	}
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[EXPRESSION]The expression has more than one item. The first is " << (sign > 0 ? "positive " : "negative ")
		<< "value of the item1." << endl;
	int itemCount = 1;
#endif
	string dst = tempVarGenerator.generateTempVar();
	while (getSymType() == ADD_SY || getSymType() == MINUS_SY)
	{
		if (getSymType() == ADD_SY)
			sign = 1;
		else
			sign = -1;
		getSym();
		string src2 = item();
		Quotation calcQuotation;
		calcQuotation.opType = ASSIGN;
		calcQuotation.dst = dst;
		calcQuotation.src1 = src1;
		calcQuotation.src2 = src2;
		calcQuotation.op = (sign > 0 ? "ADD" : "MINUS");
		intermediateCode.addQuotation(calcQuotation);
		src1 = dst;
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[EXPRESSION]The item" << itemCount << " of the expression is " << (sign > 0 ? "add " : "minus ") <<
			"to the previous ones." << endl;
		itemCount++;
#endif
	}
	return dst;
}

string SyntaxAnalysis::item()
{
	string src1 = factor();
	if (getSymType() != MULT_SY && getSymType() != DIV_SY)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[ITEM]The item has only one factor." << endl;
#endif
		return src1;
	}
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[ITEM]The item has more than one factor." << endl;
	int factorCount = 1;
#endif
	string dst = tempVarGenerator.generateTempVar();
	while (getSymType() == MULT_SY || getSymType() == DIV_SY)
	{
		int sign = 1;
		if (getSymType() == DIV_SY)
			sign = -1;
		getSym();
		string src2 = factor();
		Quotation calcQuotation;
		calcQuotation.opType = ASSIGN;
		calcQuotation.dst = dst;
		calcQuotation.src1 = src1;
		calcQuotation.src2 = src2;
		calcQuotation.op = (sign > 0 ? "MULT" : "DIV");
		intermediateCode.addQuotation(calcQuotation);
		src1 = dst;
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[ITEM]The previous factors are " << (sign > 0 ? "multiplied " : "divided ") << "by factor" <<
			factorCount << endl;
		factorCount++;
#endif
	}
	return dst;
}

string SyntaxAnalysis::factor()
{
	if (getSymType() == IDEN_SY)
	{
		string src1 = getToken();
		getSym();
		if (getSymType() == OPEN_PAREN_SY) // (
		{
#ifndef SYNTAX_DEBUG_PRINT
			cout << "[FACTOR]This calls a function with parameter list. " << endl;
#endif
			// TODO FUNC ORDER
			int funcIndex = symbolTable.funcSearch(src1);

			if (funcIndex == -1)
			{
				error.printError(getLineNum(), getIndexInLine(), UNDEFINED_FUNCTION, getToken());
			}
			if (symbolTable.funcSymbolTables.at(funcIndex).parameterTable.size() == 0)
			{
				error.printError(getLineNum(), getIndexInLine(), WRONG_FUNC_CALL_OF_NO_PARA_FUNC , getToken());
			}
			if (symbolTable.funcSymbolTables.at(funcIndex).returnType == RETURN_VOID)
			{
				error.printError(getLineNum(), getIndexInLine(), ASSIGN_VOID_FUNC_VAL_TO_OTHERS, getToken());
			}

			paraValueList(funcIndex);
			Quotation funcCallQuotation;
			funcCallQuotation.opType = FUNC_CALL;
			funcCallQuotation.dst = src1;
			intermediateCode.addQuotation(funcCallQuotation);
			string funcRes = tempVarGenerator.generateTempVar();
			Quotation retValueQuotaion;
			retValueQuotaion.opType = RET_VAL;
			retValueQuotaion.dst = funcRes;
			intermediateCode.addQuotation(retValueQuotaion);
			if (getSymType() != CLOSE_PAREN_SY)
			{
				error.printError(getLineNum(), getIndexInLine(), MISSING_THE_CLOSE_PAREN_OF_FUNC_CALL, getToken());
				return funcRes;
			}
			getSym();
			return funcRes;
		}
		else if (getSymType() == OPEN_BRACKET_SY) // [
		{
#ifndef SYNTAX_DEBUG_PRINT
			cout << "[FACTOR]This is a element of an array. " << endl;
#endif
			// TODO ARRAY GET ORDER
			SymbolTableElement arrayElement;
			if (!symbolTable.dataSearch(arrayElement, src1))
				error.printError(getLineNum(), getIndexInLine(), NO_DECLARATION, src1);
			if (!arrayElement.isArray)
				error.printError(getLineNum(), getIndexInLine(), NOT_A_ARRAY, src1);
			getSym();
			string src2 = expression();
			string dst = tempVarGenerator.generateTempVar();
			Quotation arrayGetQuotation;
			arrayGetQuotation.opType = ARRAY_GET;
			arrayGetQuotation.dst = dst;
			arrayGetQuotation.src1 = src1;
			arrayGetQuotation.src2 = src2;
			intermediateCode.addQuotation(arrayGetQuotation);
			if (getSymType() != CLOSE_BRACKET_SY)
			{
				error.printError(getLineNum(), getIndexInLine(), MISSING_THE_CLOSE_BRACKET_OF_ELEMENT_OF_ARRAY,
				                 getToken());
				return dst;
			}
			getSym();
			return dst;
		}
		else
		{
#ifndef SYNTAX_DEBUG_PRINT
			cout << "[FACTOR]Unable to judge this is a norm value, or a function with out parameter list. " << endl;
#endif
			int funcIndex = symbolTable.funcSearch(src1);
			if (funcIndex != -1)
			{
				Quotation funcCallStartQuotation;
				funcCallStartQuotation.opType = FUNC_CALL_START_LABEL;
				intermediateCode.addQuotation(funcCallStartQuotation);
				// TODO FUNC CALL ORDER
				Quotation funcCallQuotation;
				funcCallQuotation.opType = FUNC_CALL;
				funcCallQuotation.dst = src1;
				intermediateCode.addQuotation(funcCallQuotation);
				string funcRes = tempVarGenerator.generateTempVar();
				Quotation retValQuotation;
				retValQuotation.opType = RET_VAL;
				retValQuotation.dst = funcRes;
				intermediateCode.addQuotation(retValQuotation);
				return funcRes;
			}
			else
			{
				SymbolTableElement normElement;
				if (!symbolTable.dataSearch(normElement, src1))
					error.printError(getLineNum(), getIndexInLine(), NO_DECLARATION, src1);
				return src1;
			}
		}
	}
	else if (getSymType() == OPEN_PAREN_SY)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[FACTOR]This is \"(expression)\" type." << endl;
#endif
		getSym();
		string dst = expression();
		if (getSymType() != CLOSE_PAREN_SY)
		{
			error.printError(getLineNum(), getIndexInLine(), MISSING_CLOSE_PAREN_OF_FACTOR, getToken());
			return dst;
		}
		getSym();
		return dst;
	}
	else if (getSymType() == DIGIT)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[FACTOR]This is a digit: " << getIntValue() << endl;
#endif
		int src1 = getIntValue();
		string dst = tempVarGenerator.generateTempVar();
		Quotation liQuotation;
		liQuotation.opType = LI;
		liQuotation.dst = dst;
		liQuotation.srcNum1 = src1;
		intermediateCode.addQuotation(liQuotation);
		getSym();
		return dst;
	}
	else if (getSymType() == CHARACTER)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[FACTOR]This is a character: " << getToken() << endl;
#endif
		int src1 = getIntValue();
		string dst = tempVarGenerator.generateTempVar();
		Quotation liQuotation;
		liQuotation.opType = LI;
		liQuotation.dst = dst;
		liQuotation.srcNum1 = src1;
		intermediateCode.addQuotation(liQuotation);
		getSym();
		return dst;
	}
	else if (getSymType() == ADD_SY || getSymType() == MINUS_SY)
	{
		int sign = 1;
		if (getSymType() == MINUS_SY)
			sign = -1;
		getSym();
		if (getSymType() != DIGIT) 
		{
			error.printError(getLineNum(), getIndexInLine(), INVALID_FACTOR_TYPE, getToken());
			return "";
		}
		int src1 = getIntValue();
		string dst = tempVarGenerator.generateTempVar();
		Quotation liQuotation;
		liQuotation.opType = LI;
		liQuotation.dst = dst;
		liQuotation.srcNum1 = sign*src1;
		intermediateCode.addQuotation(liQuotation);
		getSym();
		return dst;
	}
	else
	{
		error.printError(getLineNum(), getIndexInLine(), INVALID_FACTOR_TYPE, getToken());
		return "";
	}
}

void SyntaxAnalysis::paraValueList(int funcIndex)
{
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[PARAMETER VALUE LIST]The is the parameter value list of a function call." << endl;
#endif
	Quotation funcCallStartQuotation;
	funcCallStartQuotation.opType = FUNC_CALL_START_LABEL;
	intermediateCode.addQuotation(funcCallStartQuotation);
	size_t paraCount = 0;
	do
	{
		getSym();
		paraCount++;
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[PARAMETER VALUE LIST]Parameter" << paraCount << endl;
#endif
		string src1 = expression();
		if (paraCount <= symbolTable.funcSymbolTables.at(funcIndex).parameterTable.size())
		{
			Quotation newQuotation;
			newQuotation.opType = PUSH;
			newQuotation.dst = src1;
			intermediateCode.addQuotation(newQuotation);
		}
		else
		{
			error.printError(getLineNum(), getIndexInLine(), MORE_PARAMETERS_THAN_EXPECTED, getToken());
		}
	} while (getSymType() == COMMA_SY);
	if (paraCount != symbolTable.funcSymbolTables.at(funcIndex).parameterTable.size())
	{
		error.printError(getLineNum(), getIndexInLine(), PARAMETERS_NUM_NOT_MATCH, getToken());
	}
}

void SyntaxAnalysis::loopStatement()
{
	getSym();
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[DO WHILE]This is the statement of do: " << endl;
#endif
	static int count = 0;
	count++;
	string doLabel = labelGenerator.getLabel("do_" + std::to_string(count));
	Quotation doQuotation;
	doQuotation.opType = LABEL;
	doQuotation.dst = doLabel;
	intermediateCode.addQuotation(doQuotation);
	statement();
	if (getSymType() != WHILE_SY)
	{
		error.printError(getLineNum(), getIndexInLine(), MISSING_WHILE, getToken());
		skipTo(SEMI_SY);
		getSym();
		return;
	}
	getSym();
	if (getSymType() != OPEN_PAREN_SY)
	{
		error.printError(getLineNum(), getIndexInLine(), MISSING_OPEN_CURLY_OF_CONDITION, getToken());
		cout << "Miss the \'(\' of condition, ignore this and continue." << endl;
	}
	getSym();
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[DO WHILE]This while condition: " << endl;
#endif
	compareLoopStatement(doLabel);
	// TODO decide: jump to "do" label?
	if (getSymType() != CLOSE_PAREN_SY)
	{
		error.printError(getLineNum(), getIndexInLine(), MISSING_CLOSE_CURLY_OF_CONDITION, getToken());
		cout << "Miss the \')\' of condition, ignore this and continue." << endl;
	}
	getSym();
}


void SyntaxAnalysis::compareLoopStatement(string doLabel)
{
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[COMPARE WHILE CONDITION]This is the condition statement of the \"while\" statement." << endl;
#endif
	string src1 = expression();
	if (getSymType() == CLOSE_PAREN_SY)
	{
		// TODO beqz, if expression is zero, jump to else label or endif
		Quotation newQuotation;
		newQuotation.opType = COMPARE;
		newQuotation.dst = doLabel;
		newQuotation.src1 = src1;
		newQuotation.op = "bnz"; // src1不为0跳转至do，否则继续执行
		intermediateCode.addQuotation(newQuotation);
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[COMPARE CONDITION]BNE, if the expression equals to zero, jump." << endl;
#endif
	}
	else if (getSymType() >= LESS_SY && getSymType() <= EQUAL_SY)
	{
		auto compareSymbol = getSymType();
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[COMPARE CONDITION]If the relation between expression1 and expression2 satisfy " << compareSymbol <<
			", then don\'t jump to else/endif." << endl;
#endif
		getSym();
		string src2 = expression();
		Quotation newQuotation;
		newQuotation.opType = COMPARE;
		newQuotation.dst = doLabel;
		newQuotation.src1 = src1;
		newQuotation.src2 = src2;
		switch (compareSymbol)
		{
		case LESS_SY: newQuotation.op = "blt"; break;
		case GREATER_SY: newQuotation.op = "bgt"; break;
		case LESS_OR_EQUAL_SY: newQuotation.op = "ble"; break;
		case GREATER_OR_EQUAL_SY: newQuotation.op = "bge"; break;
		case EQUAL_SY: newQuotation.op = "beq"; break;
		case NOT_EQUAL_SY: newQuotation.op = "bne"; break;
		}
		intermediateCode.addQuotation(newQuotation);
	}
	else
	{
		error.printError(getLineNum(), getIndexInLine(), INVALID_COMPARE_SIMBOL, getToken());
		skipTo(SEMI_SY);
	}
}


void SyntaxAnalysis::switchStatement()
{
	getSym();
	if (getSymType() != OPEN_PAREN_SY)
	{
		error.printError(MISSING_OPEN_PAREN_OF_SWITCH);
	}
	getSym();
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[SWITCH]This is the expression of switch: " << endl;
#endif
	static int count = 0;
	count++;
	string switchLabel = labelGenerator.getLabel("switch_" + to_string(count));
	Quotation switchBeginQuotation;
	switchBeginQuotation.opType = LABEL;
	switchBeginQuotation.dst = switchLabel;
	intermediateCode.addQuotation(switchBeginQuotation);
	string src1 = expression();
	string endSwitchLabel = labelGenerator.getLabel("endswitch_" + to_string(count));
	if (getSymType() != CLOSE_PAREN_SY)
	{
		error.printError(getLineNum(), getIndexInLine(), MISSING_CLOSE_PAREN_OF_SWITCH, getToken());
	}
	else
		getSym();
	if (getSymType() != OPEN_CURLY_SY)
	{
		error.printError(getLineNum(), getIndexInLine(), MISSING_OPEN_CURLY_OF_CASE_LIST, getToken());
	}
	else
		getSym();
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[SWITCH CASE]This is the condition list of switch: " << endl;
#endif
	int caseCount = 0;
	string caseLabel;
	while (getSymType() == CASE_SY)
	{
		caseCount++;
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[SWITCH CASE]Case" << caseCount << ": " << endl;
#endif

		caseLabel = labelGenerator.getLabel("case_" + to_string(count) + "_" + to_string(caseCount));
		getSym();
		if (getSymType() != DIGIT && getSymType() != CHARACTER)
		{
			error.printError(INVALID_TYPE_OF_CASE);
			skipTo(SEMI_SY);
		}
		// TODO: case supports +/-
		string src2 = tempVarGenerator.generateTempVar();
		Quotation caseQuotation;
		caseQuotation.opType = LI;
		caseQuotation.dst = src2;
		caseQuotation.srcNum1 = getIntValue();
		intermediateCode.addQuotation(caseQuotation);

		getSym();
		if (getSymType() != COLON_SY)
		{
			error.printError(MISSING_COLON_OF_CASE);
		}
		else
			getSym();

		Quotation cmpQuotation;
		cmpQuotation.opType = COMPARE;
		cmpQuotation.dst = caseLabel;
		cmpQuotation.src1 = src1;
		cmpQuotation.src2 = src2;
		cmpQuotation.op = "bne";
		intermediateCode.addQuotation(cmpQuotation);
		
		statement();

		Quotation jumpEndSwitchQuotation;
		jumpEndSwitchQuotation.opType = GOTO;
		jumpEndSwitchQuotation.dst = endSwitchLabel;
		intermediateCode.addQuotation(jumpEndSwitchQuotation);
		Quotation caseLabelQuotation;
		caseLabelQuotation.opType = LABEL;
		caseLabelQuotation.dst = caseLabel;
		intermediateCode.addQuotation(caseLabelQuotation);
	}
	if (getSymType() == DEFAULT_SY)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[SWITCH DEFAULT]Default:" << endl;
#endif
		getSym();
		if (getSymType() != COLON_SY)
		{
			error.printError(MISSING_COLON_OF_DEFAULT);
		}
		else
			getSym();
		statement();
	} else
	{
		error.printError(getLineNum(), getIndexInLine(), NO_DEFAULT_BRANCH, getToken());
		Quotation caseLabelQuotation;
		caseLabelQuotation.opType = LABEL;
		caseLabelQuotation.dst = caseLabel;
		intermediateCode.addQuotation(caseLabelQuotation);
	}
	if (getSymType() != CLOSE_CURLY_SY)
	{
		error.printError(MISSING_CLOSE_CURLY_OF_CASE_LIST);
	}
	else
		getSym();
	Quotation endSwitchLabelQuotaion;
	endSwitchLabelQuotaion.opType = LABEL;
	endSwitchLabelQuotaion.dst = endSwitchLabel;
	intermediateCode.addQuotation(endSwitchLabelQuotaion);
}

void SyntaxAnalysis::scanfStatement()
{
	getSym();
	if (getSymType() != OPEN_PAREN_SY)
	{
		error.printError(MISSING_OPEN_PAREN_OF_SCANF);
	}
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[SCANF]This is a scanf statement: " << endl;
	int scanfCount = 1;
#endif
	do
	{
		getSym();
		
		if (getSymType() != IDEN_SY)
		{
			error.printError(INVALID_SCANF_TARGET);
		}
		else
		{
			string dst = getToken();
			SymbolTableElement normElement;
			if (!symbolTable.dataSearch(normElement, dst))
				error.printError(getLineNum(), getIndexInLine(), NO_DECLARATION, dst);
			if (normElement.isArray)
				error.printError(getLineNum(), getIndexInLine(), NOT_A_NORM, dst);
			Quotation scanfQuotation;
			scanfQuotation.opType = SCANF;
			scanfQuotation.dst = dst;
			intermediateCode.addQuotation(scanfQuotation);
		}
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[SCANF]Identifier" << scanfCount++ << "." << endl;
#endif
		getSym();
	}
	while (getSymType() == COMMA_SY);
	if (getSymType() != CLOSE_PAREN_SY)
	{
		error.printError(MISSING_CLOSE_PAREN_OF_SCANF);
		skipTo(SEMI_SY);
	}
	else
		getSym();
	if (getSymType() != SEMI_SY)
	{
		error.printError(MISSING_SEMI_OF_SCANF);
		skipTo(SEMI_SY);
	}
	getSym();
}

void SyntaxAnalysis::printfStatement()
{
	getSym();
	if (getSymType() != OPEN_PAREN_SY)
	{
		error.printError(MISSING_OPEN_PAREN_OF_PRINTF);
	}
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[PRINTF]This is a printf statement: " << endl;
#endif
	getSym();
	if (getSymType() == STRING)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[PRINTF STRING]" << endl;
#endif
		string outputString = getToken();
		Quotation outputStringQuotation;
		outputStringQuotation.opType = PRINTF;
		outputStringQuotation.op = "STRING";
		outputStringQuotation.dst = outputString;
		intermediateCode.addQuotation(outputStringQuotation);
		getSym();
		if (getSymType() == COMMA_SY)
		{
#ifndef SYNTAX_DEBUG_PRINT
			cout << "[PRINTF EXPRESSION]" << endl;
#endif
			getSym();
			string dst = expression();
			// TODO type of dst
			Quotation outputDataQuotation;
			outputDataQuotation.opType = PRINTF;
			outputDataQuotation.op = "CHAR_OR_INT";
			outputDataQuotation.dst = dst;
			intermediateCode.addQuotation(outputDataQuotation);
			if (getSymType() != CLOSE_PAREN_SY)
			{
				error.printError(MISSING_CLOSE_PAREN_OF_PRINTF);
				skipTo(SEMI_SY);
			}
			else
				getSym();
		}
		else if (getSymType() == CLOSE_PAREN_SY)
		{
			getSym();
		}
		else
		{
			error.printError(MISSING_CLOSE_PAREN_OF_PRINTF);
			skipTo(SEMI_SY);
		}
	}
	else
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[PRINTF EXPRESSION]" << endl;
#endif
		string dst = expression();
		// TODO as above, judge type of 
		Quotation outputDataQuotation;
		outputDataQuotation.opType = PRINTF;
		outputDataQuotation.op = "CHAR_OR_INT";
		outputDataQuotation.dst = dst;
		intermediateCode.addQuotation(outputDataQuotation);
		if (getSymType() != CLOSE_PAREN_SY)
		{
			error.printError(MISSING_CLOSE_PAREN_OF_PRINTF);
			skipTo(SEMI_SY);
		}
		else
			getSym();
	}
	if (getSymType() != SEMI_SY)
	{
		error.printError(MISSING_SEMI_OF_PRINTF);
		skipTo(SEMI_SY);
	}
	getSym();
}

void SyntaxAnalysis::returnStatement()
{
	getSym();
	if (getSymType() == OPEN_PAREN_SY)
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[RETURN EXPRESSION]" << endl;
#endif
		getSym();
		string dst = expression();
		Quotation retQuotation;
		retQuotation.opType = RETURN;
		retQuotation.dst = dst;
		retQuotation.op = "INT_OR_CHAR";
		intermediateCode.addQuotation(retQuotation);
		if (getSymType() != CLOSE_PAREN_SY)
		{
			error.printError(MISSING_CLOSE_PAREN_OF_RETURN_EXPRESSION);
			skipTo(SEMI_SY);
		}
		else
			getSym();
	}
	else
	{
#ifndef SYNTAX_DEBUG_PRINT
		cout << "[RETURN VOID]" << endl;
#endif
		// string dst = expression();
		Quotation retQuotation;
		retQuotation.opType = RETURN;
		retQuotation.op = "VOID";
		if (!reachedMainFunc)
			intermediateCode.addQuotation(retQuotation);
	}

	if (getSymType() != SEMI_SY)
	{
		error.printError(getLineNum(), getIndexInLine(), MISSING_SEMI_OF_RETURN, getToken());
		skipTo(SEMI_SY);
	}
	getSym();
}

void SyntaxAnalysis::mainDeclare()
{
	reachedMainFunc = true;
	getSym();
	getSym();
	getSym();

	if (!symbolTable.addFuc("main", RETURN_VOID))
	{
		error.printError(getLineNum(), getIndexInLine(), DUPLICATE_DEFINE, "main");
		skipTo(CLOSE_CURLY_SY);
		return;
	}
	
#ifndef SYNTAX_DEBUG_PRINT
	cout << "[MAIN Function]" << endl;
#endif

	getSym();
	if (getSymType() == CONST_SY)
	{
		constDeclare(false);
	}

	if (getSymType() == INT_SY || getSymType() == CHAR_SY)
	{
		autoVariableDeclare();
	}

#ifndef INTERMEDIATE_CODE_PRINT
	symbolTable.printFunc();
#endif
	Quotation newQuotation;
	newQuotation.opType = FUNC_LABEL;
	newQuotation.dst = symbolTable.funcSymbolTables.back().name;
	intermediateCode.addQuotation(newQuotation);

	statementsList();
	if (getSymType() != CLOSE_CURLY_SY)
	{
		error.printError(MISSING_CLOSE_CURLY_OF_FUNCTION_DEFINE);
		skipTo(CLOSE_CURLY_SY);
	}
}
