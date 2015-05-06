#include "stdafx.h"
#include "config.h"

#include "util/UtilsFile.h"
#include <util/UtilString.h>

#include <CIni/Ini.h>
//////////////////////////////////////////////////////////////////////////
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CConfig*     CConfig::g_pThis = NULL;

#define CONFIG_FILE_NAME  _T("config.ini")
//////////////////////////////////////////////////////////////////////////
CConfig::CConfig()
    :m_port(0)
    ,m_fServer(false)
    ,m_f7z(false)
    ,m_portServerSend(30000)
    ,m_portServerListen(30001)
    , m_portClientSend(30001)
    ,m_portClientListen(30000)
{
}

CConfig::~CConfig()
{
}

CConfig& CConfig::Get()
{
    if(!g_pThis)
    {
        g_pThis = new CConfig();
        atexit(DetroyInstance);
        if(g_pThis)
        {
			wstring wstr = UtilFile::GetFilePath();
			wstr +=  _T("\\");
			wstr += CONFIG_FILE_NAME;
			g_pThis->LoadFile(wstr);
        }
		else{
			assert(false);
		}
    }
    return *g_pThis;
}

void CConfig::DetroyInstance()
{
    SAFE_DELETE(g_pThis);
}
//////////////////////////////////////////////////////////////////////////

void    CConfig::SetFileName(const wstring& strName)
{
    m_strFileName = strName;
}

const TCHAR* c_str_server = _T("server");
const TCHAR* c_str_client = _T("client");
const TCHAR* c_str_path = _T("path");
const TCHAR* c_str_7z = _T("7z");
const TCHAR* c_str_portSend = _T("portSend");
const TCHAR* c_str_portListen = _T("portListen");
const TCHAR* c_str_ip = _T("ip");
const TCHAR* c_str_common = _T("common");
//const TCHAR* c_str_ = _T("");

bool    CConfig::LoadFile(const wstring& strNameInput)
{
    wstring strName = strNameInput;
    if(strName.length()< 1)
        strName = m_strFileName;
    if(strName.length() < 1)
        return false;
    m_strFileName = strName;

    const bool ifFile = UtilFile::FileExists(m_strFileName);
	if(!ifFile){
		assert(false);
		AddDebug("can't find config file.");
		return false;
	}
    CString cstr;

    CIni ini(m_strFileName.c_str() );

	//common
	if(ini.IsSectionExist(c_str_common) )	{
		//path
        cstr = ini.GetString(c_str_common,c_str_path );
        m_path = cstr.GetString();
	}

    if(ini.IsSectionExist(c_str_server) ) 
    {        
		//7z
        int i7z = ini.GetInt(c_str_server,c_str_7z, 0 );
        m_f7z = (i7z != 0);

        //ip
        cstr = ini.GetString(c_str_server, c_str_ip);
        m_strDestIpServer = UtilString::ConvertWideCharToMultiByte(cstr.GetString() );

        //port
        m_portServerListen = ini.GetUInt(c_str_server, c_str_portListen, 30000 );
        m_portServerSend = ini.GetUInt(c_str_server, c_str_portSend, 30001 );
        
    }

    if(ini.IsSectionExist(c_str_client) ) 
    {		
        //ip
        cstr = ini.GetString(c_str_client, c_str_ip);
        m_strDestIpClient = UtilString::ConvertWideCharToMultiByte(cstr.GetString() );

        //port
        m_portClientListen = ini.GetUInt(c_str_client, c_str_portListen, 30001 );
        m_portClientSend = ini.GetUInt(c_str_client, c_str_portSend, 30000 );
    }

    return true;
}

 bool    CConfig::SaveFile()
 {
     CIni ini(m_strFileName.c_str() );

     BOOL FOK  = TRUE;
     wstring wstr;

	 //common
	 {
		 //path
         FOK = ini.WriteString(c_str_common, c_str_path, m_path.c_str() );
	 }

     //section server
     {
         //ip
         wstr = UtilString::ConvertMultiByteToWideChar(m_strDestIpServer);
         FOK = ini.WriteString(c_str_server, c_str_ip, wstr.c_str() );

         //port send, listen
         FOK = ini.WriteUInt(c_str_server, c_str_portSend, m_portServerSend );
         FOK = ini.WriteUInt(c_str_server, c_str_portListen, m_portServerListen );
         
         //7z
         FOK = ini.WriteUInt(c_str_server, c_str_7z, m_f7z );
     }

     //section client
     {
         //ip
         wstr = UtilString::ConvertMultiByteToWideChar(m_strDestIpClient);
         FOK = ini.WriteString(c_str_client, c_str_ip, wstr.c_str() );

         //port send ,listen
         FOK = ini.WriteUInt(c_str_client, c_str_portSend, m_portClientSend );
         FOK = ini.WriteUInt(c_str_client, c_str_portListen, m_portClientListen );
     }

     return true;
 }

//////////////////////////////////////////////////////////////////////////
bool    CConfig:: GetIsServer() const
{
    return m_fServer;
}

void     CConfig::SetIsServer(bool fServer)
{
    m_fServer = fServer;
}
//////////////////////////////////////////////////////////////////////////
bool   CConfig:: GetIs7z() const
{
    return m_f7z;
}
    
const std::wstring&   CConfig::GetRootPath() const
{
    return m_path;
}

void   CConfig::SetRootPath(const std::wstring& wstrPath)
{
    if(wstrPath.length()< 1)
        return;
    m_path = wstrPath;
}
//////////////////////////////////////////////////////////////////////////

