#pragma once
//========================================================================
// Minidump.h : This is a crash trapper - similar to a UNIX style core dump
//========================================================================


#include <Tlhelp32.h>
#include "dbghelp.h"

#include <list>

class MiniDumper
{
public:
	MiniDumper(bool headless);

protected:
	static MiniDumper *gpDumper;
	static LONG WINAPI Handler( struct _EXCEPTION_POINTERS *pExceptionInfo );

	virtual void VSetDumpFileName(void);
	virtual MINIDUMP_USER_STREAM_INFORMATION *VGetUserStreamArray() { return NULL; }
	virtual const TCHAR *VGetUserMessage() { return _T(""); }

	_EXCEPTION_POINTERS *m_pExceptionInfo;
	time_t m_lTime;

	TCHAR m_szDumpPath[_MAX_PATH];
	TCHAR m_szAppPath[_MAX_PATH];
	TCHAR m_szAppBaseName[_MAX_PATH];
	LONG WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo );
	BOOL m_bHeadless;
};

class ModuleEntry
{
public:
	ModuleEntry(const MODULEENTRY32 &me);
	const MODULEENTRY32 *Get() const { return &m_me32; }
	const TCHAR *GetProductVersion() const { return m_pProductVersion; }
	virtual ~ModuleEntry();

protected:
	void ReadFileVersionInfo(void);
	void FindProductVersion(void);

	MODULEENTRY32 m_me32;
	TCHAR *m_pFileVersionInfo;
	TCHAR *m_pProductVersion;

};

#define USER_DATA_BUFFER_SIZE (4096)

