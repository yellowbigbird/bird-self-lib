#pragma once

#include <atlbase.h>
#include <winsvc.h>

class CServiceModule : public CComModule
{
public:
    CServiceModule();

    void Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid = NULL);
	void Init();
    void InitDll();

    void Start();
    void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    void Handler(DWORD dwOpcode);
    void Run();
    BOOL IsInstalled();
    BOOL Install();
    BOOL Uninstall();
    LONG Unlock();
    void LogEvent(LPCTSTR pszFormat, ...);
    void SetServiceStatus(DWORD dwState);
    void SetupAsLocalServer();

    //Implementation
public:
    static void WINAPI _ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI _Handler(DWORD dwOpcode);

    // data members
public:
    TCHAR                  m_szServiceName[256];
    SERVICE_STATUS_HANDLE  m_hServiceStatus;
    SERVICE_STATUS         m_status;

    DWORD       dwThreadID;
    BOOL        m_bService;
    bool        m_fInited;
};

extern CServiceModule _Module;








