#pragma once
#include "stdafx.h"

//using namespace std;
#define UNDEFINED_ERROR -1
#define INVALID_CHAR_AFTER 0
#define INVALID_CHARACTER 1
#define MISSING_ANOTHER_SINGLE_QUOTATION 2
#define INVALID_FILECONTENT 3
#define INVALID_STRING 4
#define EMPTY_CHARACTER 5 
#define EMPTY_FILE 6 // 编译的是空文件
#define INVALID_CONST_TYPE 7 // 非法的常量定义类型
#define MISSING_SEMI_SY 8 // 句末丢失分号
#define INVALID_CONST_NAME 9 // 尝试定义的常量名字不属于标识符
#define MISSING_ASSIGN_SY_IN_CONST_DEFINE 10 // 定义常量时丢失赋值符
#define INVALID_VALUE_TO_INITIALIZE_CONST 11 // 使用非法值初始化常量
#define INVALID_NAME_OF_VARIABLE 12 // 给变量定义了一个不合法的名字
#define MISSING_SEMI_SY_AFTER_VARIABLE_DEFINE 13 // 常量定义后丢失分号
#define INITIALIZE_VARIABLE 14 // 变量不应被初始化
#define UNABLE_TO_JUDGE_VARIABLE_OR_FUNC 15 // 由于标识符后不合法的token，无法判断是变量定义或是函数定义
#define INVALID_TYPE_OF_PARAMETER 16 // 不合法的参数类型定义
#define INVALID_TOKEN_IN_PARALIST 17 // 参数表中的非法token
#define MISSING_OPEN_CURLY_OF_FUNCTION 18 // 缺少函数体开始的大括号
#define INVALID_NAME_OF_FUNC 19 //函数名字不合法，可能与保留字冲突
#define ERROR_TOKEN_IN_FUNC 20 // 参数列表开头或函数体开头的非法字符，这个函数将被视作void
#define INVALID_NAME_OF_PARAMETER 21 // 非法参数名
#define INVALID_DIM_OF_ARRAY 22 // 非法数组长度
#define MISSING_CLOSE_BRACKET_OF_ARRAY_DEFINE 23 // 数组定义丢失右中括号
#define INVALID_VARIABLE_TYPE 24 // 不合法的自动变量类型
#define INVALID_SIGNAL_IN_AUTO_DEFINE 25 // 期望',' '[' ';'之一，可能是因为变量初始化值
#define INVALID_STATEMENT 26 // 非法语句
#define MISSING_OPEN_CURLY_OF_CONDITION 27 // 情况判断语句丢失左括号
#define MISSING_CLOSE_CURLY_OF_CONDITION 28 // 情况判断语句丢失右括号
#define INVALID_COMPARE_SIMBOL 29 // 不合法的比较运算符
#define MISSING_THE_CLOSE_BRACKET_OF_ELEMENT_OF_ARRAY 30 // 数组元素访问缺失右大括号
#define MISSING_CLOSE_PAREN_OF_FACTOR 31 // 因子缺失右括号
#define MISSING_THE_CLOSE_PAREN_OF_FUNC_CALL 32 // 含参函数调用缺失右括号
#define MISSING_CLOSE_CURLY_OF_CODE_BLOCK 33 // 代码块丢失右大括号
#define MISSING_WHILE 34 // do后缺失while
#define MISSING_OPEN_PAREN_OF_SWITCH 35 // switch _a)
#define MISSING_CLOSE_PAREN_OF_SWITCH 36 //switch (a_
#define MISSING_OPEN_CURLY_OF_CASE_LIST 37 // switch (a) _ case...... }
#define INVALID_TYPE_OF_CASE 38 // case rrr: 
#define MISSING_COLON_OF_CASE 39 // case 'a'_ {...}
#define MISSING_COLON_OF_DEFAULT 40 // default _ {...}
#define MISSING_CLOSE_CURLY_OF_CASE_LIST 41 // switch (a) { case...... _
#define MISSING_OPEN_PAREN_OF_SCANF 42
#define INVALID_SCANF_TARGET 43
#define MISSING_CLOSE_PAREN_OF_SCANF 44
#define MISSING_SEMI_OF_SCANF 45
#define MISSING_OPEN_PAREN_OF_PRINTF 46
#define MISSING_CLOSE_PAREN_OF_PRINTF 47
#define MISSING_SEMI_OF_PRINTF 48
#define MISSING_CLOSE_PAREN_OF_RETURN_EXPRESSION 49
#define MISSING_SEMI_OF_RETURN 50
#define INVALID_SYMBOL_IN_ASSIGNMENT 51
#define MISSING_SEMI_IN_ASSIGN_OR_FUNC_CALL 52
#define MISSING_CLOSE_CURLY_OF_FUNCTION_DEFINE 53
#define DUPLICATE_DEFINE 54
#define UNDEFINED_FUNCTION 55
#define ASSIGN_VOID_FUNC_VAL_TO_OTHERS 56
#define MORE_PARAMETERS_THAN_EXPECTED 57
#define NO_DECLARATION 58
#define NOT_A_ARRAY 59
#define INVALID_FACTOR_TYPE 60
#define NO_ELSE 61
#define NO_DEFAULT_BRANCH 62
#define WRONG_FUNC_CALL_OF_NO_PARA_FUNC 63
#define NOT_A_NORM 64
#define PARAMETERS_NUM_NOT_MATCH 65

#define THROWABLE_DATA 0 // int a; a;类型的无用语句


class Error
{
public:
	void printError(int line, int indexInLine, int reason, std::string errToken);
	void printError(int reason);
	void printWarning(int line, int indexInLine, int reason, std::string errToken);
	int errCount = 0;
	int warningCount = 0;
};

