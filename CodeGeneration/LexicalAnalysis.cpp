#include "stdafx.h"
#include "LexicalAnalysis.h"
#include <cctype>

using namespace std;

#define REVERSED_WORDS_NUM 15
#define OUTPUT_TYPES_NUM 41
const string reversedWords[REVERSED_WORDS_NUM] = {
	"const",	"int",		"char",		"void",		"main",
	"if",		"else", 	"do",		"while",	"switch",
	"case",		"default",	"scanf", 	"printf",	"return" };

const string outputTypes[OUTPUT_TYPES_NUM] = {
	"CONST_SY(const)",	"INT_SY(int)",	"CHAR_SY(char)",	"VOID_SY(void)",
	"MAIN_SY(main)",	"IF_SY(if)",	"ELSE_SY(else)",	"DO_SY(do)",
	"WHILE_SY(while)",	"SWITCH_SY(switch)",	"CASE_SY(case)",	"DEFAULT_SY(default)",
	"SCANF_SY(scanf)",	"PRINTF_SY(printf)",	"RETURN_SY(return)",	"IDEN_SY",
	"ADD_SY(+)",	"MINUS_SY(-)",	"MULT_SY(*)",	"DIV_SY(/)",
	"LESS_SY(<)",	"LESS_OR_EQUAL_SY(<=)",	"GREATER_SY(>)",	"GREATER_OR_EQUAL_SY(>=)",
	"NOT_EQUAL_SY(!=)",	"EQUAL_SY(==)",		"SEMI_SY(;)",	"COMMA_SY(,)",
	"ASSIGN_SY(=)",		"COLON_SY(:)",		"SINGLE_QUOTATION_SY(\')", "DOUBLE_QUOTATION(\")",
	"OPEN_CURLY_SY({)",	"CLOSE_CURLY_SY(})",	"OPEN_PAREN_SY[(]",		"CLOSE_PAREN_SY[)]",
	"OPEN_BRACKET_SY([)",	"CLOSE_BRACKET_SY(])",	"DIGIT(eg:123)",	"STRING(eg:\"abc\")",
	"CHATACTER_SY(eg:'a')" };

LexicalAnalysis::LexicalAnalysis(string fileName)
{
	cout << "===============LEXICAL ANALYSIS STARTED====================" << endl;
	inFile.open(fileName, ios::in);
	if (!inFile)
	{
		cout << "Can't open file: " << fileName << endl;
		system("pause"); 
		exit(EXIT_SUCCESS);
	}
	cout << "File Name: " << fileName << endl;
}

bool LexicalAnalysis::getLine()
{
	if (inFile.getline(textLine, MAX_LENGTH_OF_TEXTLINE))
	{
		//cout << textLine << endl;
		lineNum++;
		indexInLine = 0;
		lineLength = strlen(textLine);
#ifndef DEBUG_PRINT_TEXTLINE
		cout << "Line"<< lineNum << ":\t" << textLine <<endl;
#endif // !DEBUG_PRINT_TEXTLINE
		return true;
	} else
		return false;
}

bool LexicalAnalysis::getChar()
{
	if (symbolType == -1)
	{
		return false;
	}

	while (indexInLine >= lineLength)
	{
		if (!getLine())
		{
			cout << "===============LEXICAL ANALYSIS FINISHED====================" << endl;
			ch = 0;
			return false;
		}
	}
	if (capitalMark)
		ch = textLine[indexInLine];
	else
		ch = std::tolower(textLine[indexInLine]);
	if (ch == '\'' || ch == '\"')
	{
		capitalMark = capitalMark ? false : true;
	}
	//cout << "\t" << ch << endl;
	indexInLine++;
	return true;
}

bool LexicalAnalysis::isLetter()
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_'))
		return true;
	else
		return false;
}

bool LexicalAnalysis::isLetNum()
{
	if (isLetter() || (ch >= '0' && ch <= '9'))
		return true;
	else
		return false;
}

bool LexicalAnalysis::isChar()
{
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || isLetNum())
		return true;
	else
		return false;
}

bool LexicalAnalysis::isPartOfString()
{
	if (ch == 32 || ch == 33 || (ch >= 35 && ch <= 126))
		return true;
	else
		return false;
}

int LexicalAnalysis::isReserve()
{
	for (int i = 0; i < REVERSED_WORDS_NUM; i++)
	{
		if (token.compare(reversedWords[i]) == 0)
		{
			return i;
		}
	}
	return IDEN_SY;
}

bool LexicalAnalysis::nextSym()
{
	if (ch == 0)
	{
		symbolType = -1;
		return false;
	}
	token = "";
	intValue = 0;
	while (isspace(ch))
		getChar();

	if (isLetter())
	{
		while (isLetNum())
		{
			token += ch;
			getChar();
		}
		symbolType = isReserve();
		printRes();
	} else if (isdigit(ch))
	{
		while (isdigit(ch)) {
			token += ch;
			intValue = intValue * 10 + ch - '0';
			getChar();
		}
		symbolType = DIGIT;
		printRes();
	} else if (ch == '+')
	{
		token += ch;
		symbolType = ADD_SY;
		printRes();
		getChar();
	} else if (ch == '-')
	{
		token += ch;
		symbolType = MINUS_SY;
		printRes();
		getChar();
	} else if (ch == '*')
	{
		token += ch;
		symbolType = MULT_SY;
		printRes();
		getChar();
	} else if (ch == '/')
	{
		token += ch;
		symbolType = DIV_SY;
		printRes();
		getChar();
	} else if (ch == '<')
	{
		token += ch;
		getChar();
		if (ch == '=')
		{
			token += ch;
			symbolType = LESS_OR_EQUAL_SY;
			getChar();
		} else
		{
			symbolType = LESS_SY;
		}
		printRes();
	} else if (ch == '>')
	{
		token += ch;
		getChar();
		if (ch == '=')
		{
			token += ch;
			symbolType = GREATER_OR_EQUAL_SY;
			getChar();
		} else
		{
			symbolType = GREATER_SY;
		}
		printRes();
	} else if (ch == '!')
	{
		token += ch;
		getChar();
		if (ch == '=')
		{
			token += ch;
			symbolType = NOT_EQUAL_SY;
			getChar();
			printRes();
		} else
		{
			token += ch;
			error.printError(lineNum, indexInLine, INVALID_CHAR_AFTER, token);
			getChar(); // ¶ªÆú
		}
	} else if (ch == '=')
	{
		token += ch;
		getChar();
		if (ch == '=')
		{
			symbolType = EQUAL_SY;
			token += ch;
			printRes();
			getChar();
		} else
		{
			symbolType = ASSIGN_SY;
			printRes();
		}
	} else if (ch == ';')
	{
		token += ch;
		symbolType = SEMI_SY;
		getChar();
		printRes();
	} else if (ch == ',')
	{
		token += ch;
		symbolType = COMMA_SY;
		getChar();
		printRes();
	} else if (ch == ':')
	{
		token += ch;
		symbolType = COLON_SY;
		getChar();
		printRes();
	} else if (ch == '{')
	{
		token += ch;
		symbolType = OPEN_CURLY_SY;
		printRes();
		getChar();
	} else if (ch == '}')
	{
		token += ch;
		symbolType = CLOSE_CURLY_SY;
		printRes();
		getChar();
	} else if (ch == '(')
	{
		token += ch;
		symbolType = OPEN_PAREN_SY;
		printRes();
		getChar();
	} else if (ch == ')')
	{
		token += ch;
		symbolType = CLOSE_PAREN_SY;
		printRes();
		getChar();
	} else if (ch == '[')
	{
		token += ch;
		symbolType = OPEN_BRACKET_SY;
		printRes();
		getChar();
	} else if (ch == ']')
	{
		token += ch;
		symbolType = CLOSE_BRACKET_SY;
		printRes();
		getChar();
	} else if (ch == '\'')
	{
		symbolType = SINGLE_QUOTATION_SY;
		token += ch;
		getChar();
		if (isChar())
		{
			token += ch;
			intValue = ch;
			getChar();
			if (ch == '\'')
			{
				token += ch;
				symbolType = CHARACTER;
				printRes();
				getChar();
			} else
			{
				token += ch;
				error.printError(lineNum, indexInLine, MISSING_ANOTHER_SINGLE_QUOTATION, token);
				getChar(); // ¶ªÆú
			}
		} else if (ch == '\'')
		{
			error.printError(lineNum, indexInLine, EMPTY_CHARACTER, token);
			getChar(); // ¶ªÆú
		} else
		{
			string errorInfo = "\'" + ch;
			getChar();
			if (ch == '\'')
			{
				errorInfo += ch;
				error.printError(lineNum, indexInLine, INVALID_CHARACTER, errorInfo);
				getChar();
			} else
			{
				errorInfo += ch;
				error.printError(lineNum, indexInLine, INVALID_CHARACTER, errorInfo);
				error.printError(lineNum, indexInLine, MISSING_ANOTHER_SINGLE_QUOTATION, errorInfo);
			}
		}
	} else if (ch == '\"')
	{
		symbolType = DOUBLE_QUOTATION_SY;
		token += ch;
		getChar();
		while (isPartOfString())
		{
			token += ch;
			getChar();
		}

		if (ch == '\"')
		{
			token += ch;
			symbolType = STRING;
			printRes();
			getChar();
		} else
		{
			string errInfo = token;
			errInfo += ch;
			error.printError(lineNum, indexInLine, INVALID_STRING, errInfo);
		}
	} else
	{
		string errInfo;
		errInfo += ch;
		error.printError(lineNum, indexInLine, MISSING_ANOTHER_SINGLE_QUOTATION, errInfo);
		getChar(); // ¶ªÆú
	}
	return true;
}

void LexicalAnalysis::printRes()
{
#ifndef LEXICAL_ANALYSIS_DEBUG

	cout << "Type: " << outputTypes[symbolType] << "\t";
	if (symbolType <= IDEN_SY)
	{
		cout << "Token: " << token;
	} else if (symbolType <= DIV_SY)
	{
		cout << "Operator: " << ch;
	} else if (symbolType <= EQUAL_SY)
	{
		cout << "Comparison operator: " << token;
	} else if (symbolType <= COLON_SY)
	{
		cout << "Symbol: " << token;
	} else if (symbolType <= CLOSE_BRACKET_SY)
	{
		cout << "Bracket: " << ch;
	} else if (symbolType == DIGIT)
	{
		cout << "Digit Value: " << intValue;
	} else if (symbolType == CHARACTER)
	{
		cout << "Character: " << token << "\tValue: " << intValue;
	} else if (symbolType == STRING)
	{
		cout << "String: " << token;
	}
	cout << endl;

#endif // !LEXICAL_ANALYSIS_DEBUG
}

int LexicalAnalysis::getSymbolType()
{
	return symbolType;
}

int LexicalAnalysis::getIntValue()
{
	return intValue;
}

string LexicalAnalysis::getToken()
{
	return token;
}

int LexicalAnalysis::getLineNum()
{
	return lineNum;
}

int LexicalAnalysis::getIndexInLine()
{
	return indexInLine;
}




