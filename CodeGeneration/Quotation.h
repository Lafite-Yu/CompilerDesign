#pragma once
#include "stdafx.h"

#define ASSIGN 0
#define LABEL 1
#define PUSH 2
#define FUNC_CALL 3
#define RET_VAL 4 // i = RET
#define RETURN 5 // ret x
#define COMPARE 6

#define GOTO 7
#define ARRAY_GET 8
#define LI 9
#define NEGATIVE 10
#define ARRAY_ASSIGN 11
#define SCANF 12
#define PRINTF 13 // OP:"INT" "CHAR" "STRING"
#define FUNC_LABEL 14
#define FUNC_CALL_START_LABEL 15

struct Quotation
{
	int opType;
	std::string dst;
	std::string src1, src2;
	std::string op;
	int srcNum1 = 0, srcNum2 = 0;
};