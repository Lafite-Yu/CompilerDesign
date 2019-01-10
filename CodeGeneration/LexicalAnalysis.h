#pragma once
#include "stdafx.h"
#include "Error.h"

//using namespace std;

#define MAX_LENGTH_OF_TEXTLINE 1024

#define CONST_SY 0
#define INT_SY 1
#define CHAR_SY 2
#define VOID_SY  3
#define MAIN_SY 4
#define IF_SY 5
#define ELSE_SY 6
#define DO_SY 7
#define WHILE_SY 8
#define SWITCH_SY 9
#define CASE_SY 10
#define DEFAULT_SY 11
#define SCANF_SY 12
#define PRINTF_SY 13
#define RETURN_SY 14
#define IDEN_SY 15
#define ADD_SY 16
#define MINUS_SY 17
#define MULT_SY 18
#define DIV_SY 19
#define LESS_SY 20
#define LESS_OR_EQUAL_SY 21
#define GREATER_SY 22
#define GREATER_OR_EQUAL_SY 23
#define NOT_EQUAL_SY 24
#define EQUAL_SY 25
#define SEMI_SY 26
#define COMMA_SY 27
#define ASSIGN_SY 28
#define COLON_SY 29
#define SINGLE_QUOTATION_SY 30
#define DOUBLE_QUOTATION_SY 31
#define OPEN_CURLY_SY 32
#define CLOSE_CURLY_SY 33
#define OPEN_PAREN_SY 34
#define CLOSE_PAREN_SY 35
#define OPEN_BRACKET_SY 36
#define CLOSE_BRACKET_SY 37
#define DIGIT 38
#define STRING 39
#define CHARACTER 40

extern Error error;

class LexicalAnalysis
{
private:
	std::ifstream inFile;
	char textLine[MAX_LENGTH_OF_TEXTLINE];

	char ch = ' ';
	std::string token;
	int intValue = 0;
	int symbolType = 0;

	bool getLine();
	bool getChar();
	int lineNum = 0;
	int indexInLine = 0;
	int lineLength = 0;

	bool isLetter();
	bool isLetNum();
	bool isChar();
	bool isPartOfString();

	int isReserve();
	bool capitalMark = false;
public:
	LexicalAnalysis(std::string fileName);
	bool nextSym();

	void printRes();

	int getSymbolType();
	int getIntValue();
	std::string getToken();
	int getLineNum();
	int getIndexInLine();

};

