#include "stdafx.h"
#include "Error.h"

using namespace std;

void Error::printError(int line, int indexInLine, int reason, string errToken)
{
	errCount++;
	cerr << "[ERROR] Line: " << line << "\tcolomn: " << indexInLine;
	cerr << "\tReason: " << reason << "\tWrong Token: " << errToken << endl;
}

void Error::printError(int reason)
{	
	errCount++;
	cerr << "[Error] Reason: " << reason << endl;
}

void Error::printWarning(int line, int indexInLine, int reason, std::string errToken)
{
	warningCount++;
	cerr << "[WARNING] Line: " << line << "\tcolomn: " << indexInLine;
	cerr << "\tReason: " << reason << "\tWrong Token: " << errToken << endl;
}
