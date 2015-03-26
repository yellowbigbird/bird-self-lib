#include "StdAfx.h"
#include "ProcessWatch.h"

ProcessWatch::ProcessWatch(void)
{
	m_file.open(FILEPATH, ios::in|ios::out|ios::trunc);
	if(!m_file.good())
	{
		WriteToLog(TEXT("Open the log file failed"));
	}
	m_file << "Open log file successed" << endl;
}

ProcessWatch::~ProcessWatch(void)
{
	m_file.close();
}

wstring ProcessWatch::ANSIToUnicode( const string& str )
{
	int  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar( CP_ACP,
			0,
			str.c_str(),
			-1,
			NULL,
			0 ); 
	wchar_t *  pUnicode; 
	pUnicode = new  wchar_t[unicodeLen+1]; 
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t)); 
	::MultiByteToWideChar( CP_ACP,
			0,
			str.c_str(),
			-1,
			(LPWSTR)pUnicode,
			unicodeLen ); 
	wstring  rt; 
	rt = ( wchar_t* )pUnicode;
	delete  pUnicode;
 
	return  rt; 
}

string ProcessWatch::UnicodeToANSI( const wstring& str )
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte( CP_ACP,
			0,
			str.c_str(),
			-1,
			NULL,
			0,
	NULL,
			NULL );
	pElementText = new char[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
	::WideCharToMultiByte( CP_ACP,
			0,
			str.c_str(),
			-1,
			pElementText,
			iTextLen,
			NULL,
			NULL );
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

void ProcessWatch::GetTime(char *str, int size)
{
	time_t t = time(NULL);
	struct tm ptm;
	localtime_s(&ptm, &t);
	sprintf_s(str, size, "%d/%d/%d %d:%d:%d", 
			ptm.tm_year+1900,
			ptm.tm_mon,
			ptm.tm_mday,
			ptm.tm_hour,
			ptm.tm_min,
			ptm.tm_sec);
}

void ProcessWatch::WriteToLog(TCHAR *strToWrite, int id, BOOL isError)
{
	string strWrite = UnicodeToANSI(strToWrite);
	char time[128] = {};
	if(isError)
	{
		HLOCAL hlocal = NULL;
		DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
		BOOL fOk = FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL,
			id,
			systemLocale,
			(PTSTR)&hlocal,
			0,
			NULL);
		if(fOk && (hlocal != NULL))
		{
			GetTime(time, sizeof(time));
			m_file << "[" << time << "]" << " ERROR: " << (PCTSTR)LocalLock(hlocal) << endl;
			LocalFree(hlocal);
		}
	}
	else
	{
		GetTime(time, sizeof(time));
		m_file <<  "[" << time << "]" << strWrite.c_str() << endl;
	}

}

__int64 ProcessWatch::FileTimeToInt64(const FILETIME& time)
{
	ULARGE_INTEGER tt;
	tt.LowPart = time.dwLowDateTime;
	tt.HighPart = time.dwHighDateTime;
	return(tt.QuadPart);
}

int ProcessWatch::GetProcessorNumber()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (int)info.dwNumberOfProcessors;
}

int ProcessWatch::GetProcessCpuUsage(int pid)
{  
	static int processor_count_ = -1;
	static __int64 last_time_ = 0;
	static __int64 last_system_time_ = 0;

	FILETIME now;
	FILETIME creation_time;
	FILETIME exit_time;
	FILETIME kernel_time;
	FILETIME user_time;
	__int64 system_time;
	__int64 time;
	// 	__int64 system_time_delta;
	// 	__int64 time_delta;

	double cpu = -1;

	if(processor_count_ == -1)
	{
		processor_count_ = GetProcessorNumber();
	}

	GetSystemTimeAsFileTime(&now);

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION/*PROCESS_ALL_ACCESS*/, false, pid);
	if (!hProcess)
	{
		return -1;
	}
	if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
	{
		return -1;
	}
	system_time = (FileTimeToInt64(kernel_time) + FileTimeToInt64(user_time)) / processor_count_;  
	time = FileTimeToInt64(now);		

	last_system_time_ = system_time;
	last_time_ = time;
	CloseHandle( hProcess );

	Sleep(30);

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION/*PROCESS_ALL_ACCESS*/, false, pid);
	if (!hProcess)
	{
		return -1;
	}
	if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
	{
		return -1;
	}
	GetSystemTimeAsFileTime(&now);
	system_time = (FileTimeToInt64(kernel_time) + FileTimeToInt64(user_time)) / processor_count_; 
	time = FileTimeToInt64(now);		

	CloseHandle( hProcess );

	cpu = ((double)(system_time - last_system_time_) / (double)(time - last_time_)) * 100;
	return (int)cpu;
}

DWORD ProcessWatch::GetProcessID()
{
	DWORD bytesReturned;
	DWORD processID[MAX_BUF_PATH];
	memset(processID, 0, MAX_BUF_PATH);
	EnumProcesses(processID, MAX_BUF_PATH, &bytesReturned);
	int processCount = bytesReturned/sizeof(DWORD);
	ProcessInfo processInfo;
	    
	for (int i=0; i<processCount; i++)
	{
		processInfo.processID = processID[i];
		HANDLE hProcess;
		HMODULE hModule[MAX_BUF_PATH];
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,false,processID[i]);
		if(hProcess)
		{
			//enum process module info
			EnumProcessModules(hProcess, hModule, sizeof(hModule), &bytesReturned);
			// get the module path
			GetModuleFileNameEx(hProcess, hModule[0], processInfo.processPath, MAX_BUF_PATH); 
			//_tprintf(TEXT("%s --- %d\n"), processInfo.processPath, processID[i]);
		}	
		m_vecProcessInfo.push_back(processInfo);
		
	}
	
	return processCount;
}

int ProcessWatch::FindHeavyProcess()
{
	
	int processCount = GetProcessID();
	for(int i=1; i<processCount; i++)
	{
		int cpu = GetProcessCpuUsage(m_vecProcessInfo[i].processID);
		
		//_tprintf(TEXT("%d, %d\n"), m_vecProcessInfo[i].processID, cpu);
		if(cpu>15)
		{
			char str[MAX_BUF_PATH] = {};
			TCHAR tstr[MAX_BUF_PATH] = {};
			sprintf_s(str, sizeof(str), "PID[%d]  cpu usage more than limited value: %d %%, the path is %s \n", 
							m_vecProcessInfo[i].processID, cpu, UnicodeToANSI(m_vecProcessInfo[i].processPath).c_str());
			_tcscpy_s(tstr, ANSIToUnicode(string(str)).c_str());
			WriteToLog(tstr);
			return m_vecProcessInfo[i].processID;
		}
	}
	return 0;
}

DWORD ProcessWatch::ThreadProc(void *param)
{
	ProcessWatch *pThis = (ProcessWatch*)param;
	//pThis->
	pThis->WatchThread();
	return 0;
}

int ProcessWatch::WatchThread()
{
	int times(0);
	int sumCpu(0);
	int aveCpu(0);
	while(times < 60)
	{
		int cpu = GetProcessCpuUsage(m_pid);
		sumCpu += cpu;
		Sleep(1000);
		times++;
	}
	aveCpu = sumCpu / times;
	HANDLE hProcess = OpenProcess(/*PROCESS_ALL_ACCESS,FALSE*/PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE, false, m_pid);
	if(aveCpu >= 30)
	{
		if(!TerminateProcess(hProcess, 0))
		{
			WriteToLog(TEXT("Terminate the process failed"), GetLastError(), TRUE);
			return FALSE;
		}
		char str[MAX_BUF_PATH] = {};
		TCHAR tstr[MAX_BUF_PATH] = {};
		sprintf_s(str, sizeof(str), "process[%d] was kill\n", m_pid);
		_tcscpy_s(tstr, ANSIToUnicode(string(str)).c_str());
		WriteToLog(tstr);
	}
	return 0;
}

UINT ProcessWatch::KillHeavyProgram()
{
	while(1)
	{
		m_vecProcessInfo.clear();
		m_pid = FindHeavyProcess();
		if(m_pid)
		{
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this, 0, NULL);
			WaitForSingleObject(hThread, INFINITE);
			if(hThread == NULL)
			{
				DWORD err = GetLastError();
				return FALSE;
			}
			m_pid = 0;
		}
		
		
	}
}