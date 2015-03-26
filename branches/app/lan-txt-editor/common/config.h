#pragma once
#include "comDll.h"

class COM_DLL CConfig
{
public:
    CConfig();
    virtual ~CConfig();

    static CConfig& GetInstance();
    static void DetroyInstance();

    void    SetFileName(const std::wstring& strName);

    bool    LoadFile(const std::wstring& strName = _T(""));
    bool    SaveFile();

    bool    GetIsServer() const;
    void    SetIsServer(bool fServer);

    bool    GetIs7z() const;

    const std::wstring&     GetRootPath() const;
    void                    SetRootPath(const std::wstring& wstrPath);

public:
    
    UINT32      m_port;
    

    UINT        m_portServerSend;
    UINT        m_portServerListen;

    UINT        m_portClientSend;
    UINT        m_portClientListen;

    std::string      m_strDestIpServer;
    std::string      m_strDestIpClient;

private:
    static CConfig*     g_pThis;
    std::wstring        m_strFileName;
    std::wstring         m_path;

    bool        m_fServer;
    bool        m_f7z;

};
