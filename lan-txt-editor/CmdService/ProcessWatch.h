#pragma once
#include <vector>
//#include <stdio.h>  
#include <Windows.h>
#include <vector>
#include <iostream>
#include <fstream>
#include<ctime>
#include <process.h>
#include "PSAPI.H"
#pragma comment( lib, "PSAPI.LIB" )
#define MAX_BUF_PATH 1024
#define FILEPATH "../ServiceLog.txt"
using namespace std;

class ProcessWatch
{
public:
	struct ProcessInfo
	{
		DWORD processID;
		TCHAR processPath[MAX_BUF_PATH];
	};
	ProcessWatch(void);
	~ProcessWatch(void);

	UINT KillHeavyProgram();
	

	
private:
	wstring ANSIToUnicode( const string& str );
	string UnicodeToANSI( const wstring& str );
	__int64 FileTimeToInt64(const FILETIME& time);
	int GetProcessorNumber();
	int GetProcessCpuUsage(int pid);
	DWORD GetProcessID();
	int FindHeavyProcess();
	static DWORD ThreadProc(void *param);
	int WatchThread(void);
	void GetTime(char *str, int size);
	void WriteToLog(TCHAR *strToWrite, int id = 0, BOOL isError = FALSE);

	std::vector<ProcessInfo> m_vecProcessInfo;
	int m_pid;
	fstream m_file;
};
