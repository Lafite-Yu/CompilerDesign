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
#define EMPTY_FILE 6 // ������ǿ��ļ�
#define INVALID_CONST_TYPE 7 // �Ƿ��ĳ�����������
#define MISSING_SEMI_SY 8 // ��ĩ��ʧ�ֺ�
#define INVALID_CONST_NAME 9 // ���Զ���ĳ������ֲ����ڱ�ʶ��
#define MISSING_ASSIGN_SY_IN_CONST_DEFINE 10 // ���峣��ʱ��ʧ��ֵ��
#define INVALID_VALUE_TO_INITIALIZE_CONST 11 // ʹ�÷Ƿ�ֵ��ʼ������
#define INVALID_NAME_OF_VARIABLE 12 // ������������һ�����Ϸ�������
#define MISSING_SEMI_SY_AFTER_VARIABLE_DEFINE 13 // ���������ʧ�ֺ�
#define INITIALIZE_VARIABLE 14 // ������Ӧ����ʼ��
#define UNABLE_TO_JUDGE_VARIABLE_OR_FUNC 15 // ���ڱ�ʶ���󲻺Ϸ���token���޷��ж��Ǳ���������Ǻ�������
#define INVALID_TYPE_OF_PARAMETER 16 // ���Ϸ��Ĳ������Ͷ���
#define INVALID_TOKEN_IN_PARALIST 17 // �������еķǷ�token
#define MISSING_OPEN_CURLY_OF_FUNCTION 18 // ȱ�ٺ����忪ʼ�Ĵ�����
#define INVALID_NAME_OF_FUNC 19 //�������ֲ��Ϸ��������뱣���ֳ�ͻ
#define ERROR_TOKEN_IN_FUNC 20 // �����б�ͷ�����忪ͷ�ķǷ��ַ������������������void
#define INVALID_NAME_OF_PARAMETER 21 // �Ƿ�������
#define INVALID_DIM_OF_ARRAY 22 // �Ƿ����鳤��
#define MISSING_CLOSE_BRACKET_OF_ARRAY_DEFINE 23 // ���鶨�嶪ʧ��������
#define INVALID_VARIABLE_TYPE 24 // ���Ϸ����Զ���������
#define INVALID_SIGNAL_IN_AUTO_DEFINE 25 // ����',' '[' ';'֮һ����������Ϊ������ʼ��ֵ
#define INVALID_STATEMENT 26 // �Ƿ����
#define MISSING_OPEN_CURLY_OF_CONDITION 27 // ����ж���䶪ʧ������
#define MISSING_CLOSE_CURLY_OF_CONDITION 28 // ����ж���䶪ʧ������
#define INVALID_COMPARE_SIMBOL 29 // ���Ϸ��ıȽ������
#define MISSING_THE_CLOSE_BRACKET_OF_ELEMENT_OF_ARRAY 30 // ����Ԫ�ط���ȱʧ�Ҵ�����
#define MISSING_CLOSE_PAREN_OF_FACTOR 31 // ����ȱʧ������
#define MISSING_THE_CLOSE_PAREN_OF_FUNC_CALL 32 // ���κ�������ȱʧ������
#define MISSING_CLOSE_CURLY_OF_CODE_BLOCK 33 // ����鶪ʧ�Ҵ�����
#define MISSING_WHILE 34 // do��ȱʧwhile
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

#define THROWABLE_DATA 0 // int a; a;���͵��������


class Error
{
public:
	void printError(int line, int indexInLine, int reason, std::string errToken);
	void printError(int reason);
	void printWarning(int line, int indexInLine, int reason, std::string errToken);
	int errCount = 0;
	int warningCount = 0;
};

