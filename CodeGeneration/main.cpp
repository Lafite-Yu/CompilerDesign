//XXXKXXXXXXXXKXXXXXXXXXKKKKKKKKKKKKKKKKKKKKKKKKKKXXXXXXKKXXXKKKKKKKKKKKXXXXXXKKXXNNNNNXXXXNNNNNNNNXXK
//XXXKKXXXXKKXXXXXXXXXXXXXXXXXXXXXKKKKKKKKKKKKKKXXXXXXXXXXXXXKKKKKKKKKKKKXXXXXXXXXNNNNNNNNXNNNNNNNXXKK
//XXXXXXXXXKOkxdooooollllllllllooodkOKXXKKKKKKKKKKXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXNNNNNNNNNXXNNNXXKKKKK
//XXXXXX0d:'..';ccllooooooooollllc:::::ldk0KKKKKXXXXXXXXXKOxxdoolcccccccccccccloodxkOKXNNNNNNNXXXKKKKK
//XXKX0o.  .cx0XXXXXXXXXKKKKKXXXXXKK0kdl:,':d0KKXXXXXXXkc'..,;:lloodddxxxxxddddolc:;,,,:oxKNXXKKXKKKKK
//XKXKc  .:OXXXXXXKKKKKKKK00OkO0KKKKKKKKK0o. 'dKXXXXX0c. 'okKXXXXXXXXXXXXXXXXXXNNNNXKko:. 'oKXKKKKKKKK
//XXNx. .lKXXXXXXKKKKKKOOxoKKKKcdk0KKKKKKKXx.  ,clllc'  :0XXXXXKXXKXXXXXXXXXXXXNNNNNNNNNO;  ;OXKKKKKKK
//XXXl  cKXXXXXXKKKKKK0xKKKKKKKKcllOKKKKKKKK:          ,OXXXKXXKXK0kxdoddxkOKXXNNNNNNNNNN0;  :0XKKKKKK
//XX0; ,0XKXXXXXKKKK0OkxdO0xllokK0xkKKKKKKXK:          :KXXXXKKXKklKKKKKKKKlOKXXNNNNXNNNXXx. .xXKKKKKK
//KXO'.oXXKXXXKKKKKKKKXXKKKKKKKKKKKKKKKKKKXk'   ..     :XXKXXXXXOdxOdKKKKddldOKXNNNNNNXXXX0;  oKKKKKKK
//XXO,.kXXXKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKl  'dOOo.   ;KXXXXXXK0KXXXXXXXXNNXXXXXNNNNXKKKKKc  lKKKKKKK
//XXK;'OXKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKXk' ,OXXXXo.. .kXXXXXXXKKXXXKKXXXXXXXNNNNNXXXKKKKKl .dKKKKKKK
//KXXc.xXKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK0; ;OXXXKXKO:  :KXXXXXXXXXXKKKXXXXXXXNNNNXXKKKKKKKc 'kXKKKKKK
//KKXx.cKXKKKKKKKKKKKKKKKKKKKKKKKKKKKKKXO:.;0XXXXXXXNO; .oXXXXXXXXKKXKKXXXXXXXXXXXKKKKKKKKK: ;0KKKKKKK
//KKXK:.dKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKO;.:0XXXXXXXXXX0; .xXXXXXXXXXXXXXXXXXXXXXXKKKKKKKKK0,.dXKKKKKKK
//KXXXO,.oKKKKKKKKKKKKKKKKKKKKKKKKKKKKd'.lKXXXXXXXXXXXX0c..xXXXXXXKKKXXXXXXXXXXKKKKKKKKKKKo'lKKKKKKKKK
//KKXKKOc':x0KKKKKKKKKKKKKKKKKKKKKKKk;.,kXXXXXXXXXXXXXKXKd..l0KKXXKKXXXXXXXXXXKKKKKKKKKK0o,lKKKKKKKKKK
//KKKKKKKOoccldk0KXXKKKKKKKKKXK0Odl:;;d0XXXXXKXKKXKKKXXXXXOc.;xKXKKKXXXXXXXXKKKKKKKKKKOo;:xKKKKKKKKKKK
//KKKKKKKKKK0kdooooddddddddodollllox0KKKKXXXKKKKKKXKKKKXKKKKk:;codkO0KXXXXXXKKK0OOxolc:cd0KKKKKKKKKKKK
//XKKKKKKKKKKKXXKK0OkxxdddxxkO0KKXKKKKKKXXXKKXKKXXKKKKKKKKKKKK0Oxdddddddoollcccc::clok0KKKKKKKKKKKKKKK
//KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKXXXXKKXXKXKKKKKKKKKKKKKKKKKXNNNXXK0OOOO00KKXXKKKKKKKKKKKKKKKKKK

#include "stdafx.h"
#include "LexicalAnalysis.h"
#include "Error.h"
#include "SyntaxAnalysis.h"
#include "SymbolTable.h"
#include "LabelGenerator.h"
#include "TempVarGenerator.h"
#include "MIPSCode.h"

// TODO ��������������
// TODO assignʱ���ͼ�� 
// char c = ('a'); char b = 'a'+1; ���Ϸ�������ô������������ܲ�ʵ�֣�����ANSI CĬ������������ISO C��ʽ����ת��
// TODO �Զ�������ʼ�����
// ѧC��ʱ��˵Ҫ����Ա��֤��Ȼ��ѧ��������Ҫ����������ߣ��Լ�����֤������ô

// TODO LIST ���ȼ�
// DONE_TODO Ĭ�Ϸ���
// DONE_TODO �������������
// DONE_TODO print(char) scanf(char) 
// TODO case +1: ; case -1: ;
// TODO �������õĲ�������һ���������� // �漰�Ĵ����������/�������ñ���ͻָ��ֳ�����������ƣ���ʱ����

using namespace std;

Error error;
int main()
{
	cout << "===============COMPLIER STARTED====================" << endl;
	string inputFile;
	cout << "Please input the file name:" << endl;
	getline(cin, inputFile);


	ofstream intermediateCodeFile("intermediateCode.txt", fstream::out);
	if (!intermediateCodeFile)
	{
		cerr << "Unable to create intermediate code file." << endl;
		system("pause");
		exit(EXIT_SUCCESS);
	}
	ofstream mipsCodeFile("mipsCode.txt", fstream::out);
	if (!mipsCodeFile)
	{
		cerr << "Unable to create mips code file." << endl;
		system("pause");
		exit(EXIT_SUCCESS);
	}
	LexicalAnalysis lexicalAnalysis(inputFile);
	SymbolTable symbolTable(intermediateCodeFile);
	IntermediateCode intermediateCode(intermediateCodeFile);
	LabelGenerator labelGenerator;
	TempVarGenerator tempVarGenerator;
	SyntaxAnalysis syntaxAnalysis(&lexicalAnalysis, symbolTable, intermediateCode, labelGenerator, tempVarGenerator);
	
	
	syntaxAnalysis.run();

	RegisterManager registerManager(symbolTable, intermediateCode, mipsCodeFile);
	MIPSCode mipsCode(symbolTable, intermediateCode, registerManager, mipsCodeFile);
	mipsCode.run();
	

	system("pause");
	return 0;
}