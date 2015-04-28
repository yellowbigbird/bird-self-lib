#include "stdafx.h"
#include "MiniDump.h"
#include <iostream>


bool  DebutMe()
{
	std::cout<<"test";
	int *p = NULL;
	*p = 0;
	int c = 3;
	int d = c;
	std::cout<<"test"<<*p<<std::endl;

	return true;
}


int _tmain(int argc, _TCHAR* argv[])
{
	MiniDumper minDump(true);
	DebutMe();
	MessageBox(NULL, _T("ok"), NULL, MB_OK);
	printf("Hello World!\n");
	return 0;
}

