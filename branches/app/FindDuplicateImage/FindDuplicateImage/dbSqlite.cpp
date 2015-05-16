#include "stdafx.h"
#include "dbSqlite.h"

#include "config.h"

#include "../../../lib/libsqlite/sqlite3.h"
#include "util/UtilFile.h"

#include <iostream>
//#include <iosfwd>
#include <stdlib.h>

#pragma warning(disable: 4996)

using namespace std;
//////////////////////////////////////////////////////////////////////////
#define IF_USE_MEM_SQLITE       1

const char* c_strFileName = "image.s3db";

COffset* COffset::m_pInstance = NULL;

#define CHECK_DB	if(!pdb){\
		assert(false);\
		return false;\
	}\

enum eDbSnapshot
{
    eDbSnap_id =0,
    eDbSnap_hash,
    eDbSnap_name,
    eDbSnap_fileSize,
    eDbSnap_date,
	eDbSnap_asimovVersion,

    eDbSnap_max,
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
COffset::COffset()
:m_fParsedInDb(false)
,m_pdb(NULL)
{
    ::InitializeCriticalSection(&m_cs);
	LoadFile();
}

COffset::~COffset()
{
    m_pdb = NULL;
    ::DeleteCriticalSection(&m_cs);
}

void COffset::Close()
{
    if(m_pdb)
        CloseDb(m_pdb);
    m_pdb = NULL;
}


COffset& COffset::Get()
{
    if(!m_pInstance){
        m_pInstance = new COffset();
        atexit(COffset::DetroyInstance );

    }
    return *m_pInstance;
}

void COffset::DetroyInstance()
{
    SAFE_DELETE(m_pInstance);
}
//////////////////////////////////////////////////////////////////////////
std::string COffset::GetAbsFilePathName() const
{
    //get absolute file path 
    const wstring wstrPath = UtilFile::GetFilePath();
        //CConfig::Get().GetDataPath(); 
    
    string strFile = UtilString::ConvertWideCharToMultiByte(wstrPath);
    //string strver = CConfig::Get().GetCurSnapVersionNo();
    strFile = strFile + ("\\") + c_strFileName;
	return strFile;
}

bool COffset::GetIfParsedInDB() const
{
    return m_fParsedInDb;
}

//////////////////////////////////////////////////////////////////////////
sqlite3* COffset::GetDb()
{
    if(m_pdb)
        return m_pdb;
    m_pdb = OpenDb();
    return m_pdb;
}

sqlite3* COffset::OpenDb()
{
    if(m_pdb)
        return m_pdb;

    sqlite3 *pdb = NULL;
    const string strFile = GetAbsFilePathName();
    const wstring wstrFile = UtilString::ConvertMultiByteToWideChar(strFile);

	string strdbg = "COffset::OpenDb path=";
	strdbg += strFile;
	AddDebug(strdbg);

    int rc =0;

    try
    {
        const bool fFileExist = UtilFile::FileExists(wstrFile);
        if(!fFileExist){
            //copy file
            wstring wstr = UtilString::ConvertMultiByteToWideChar(c_strFileName);
            wstring wstrPath = UtilFile::GetFilePath();
            wstrPath += _T("\\");
            wstrPath += wstr;
            UtilFile::CopyFile(wstrPath, wstrFile);
        }
        m_sqlFilePath = strFile;

#if IF_USE_MEM_SQLITE 
        rc = sqlite3_open(":memory:", &pdb); 
        if(rc){
            assert("sqlite3_open memory" && false);
        }
        rc = LoadOrSaveDb(pdb, strFile.c_str(), false); //文件数据库导入到内存数据库 
#else
        rc = sqlite3_open(strFile.c_str(), &pdb);
#endif //IF_USE_MEM_SQLITE

        if( rc ){
            assert("sqlite3_open error" && false);
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(pdb) );
            sqlite3_close(pdb);
        } 

        OptimizeDb(pdb);
    }
    catch(...){
        assert(" COffset::OpenDb" && false);
        AddDebug("COffset::OpenDb error.");
    }
    return pdb;
}

void COffset::CloseDb(sqlite3* pdb)
{
#if IF_USE_MEM_SQLITE 
    int rc = LoadOrSaveDb(pdb, m_sqlFilePath.c_str(), true); //文件数据库导入到内存数据库 
#endif
    sqlite3_close(pdb);

	const string strFile = GetAbsFilePathName();
	if ( m_sqlFilePath != strFile)
	{
		wstring wstrSrcFile = UtilString::ConvertMultiByteToWideChar(m_sqlFilePath);
		wstring wstrDestFile = UtilString::ConvertMultiByteToWideChar(strFile);
		UtilFile::MoveFile(wstrSrcFile, wstrDestFile);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////
//参数说明:
//pInMemory: 指向内存数据库指针
//zFilename: 指向文件数据库目录的字符串指针
//isSave  0: 从文件数据库载入到内存数据库 1：从内存数据库备份到文件数据库
////////////////////////////////////////////////////////////////////////////////////////////
int COffset::LoadOrSaveDb(sqlite3 *pInMemeory, const char *zFilename, bool isSave)
{
    int rc;
    sqlite3 *pFile;
    sqlite3_backup *pBackup;
    sqlite3 *pTo;
    sqlite3 *pFrom;

    rc = sqlite3_open(zFilename, &pFile);
    if(rc == SQLITE_OK)
    {
        pFrom = (isSave?pInMemeory:pFile);
        pTo = (isSave?pFile:pInMemeory);

        pBackup = sqlite3_backup_init(pTo,"main",pFrom,"main");

        if(pBackup)
        {
            (void)sqlite3_backup_step(pBackup,-1);
            (void)sqlite3_backup_finish(pBackup);
        }
        rc = sqlite3_errcode(pTo);
    }
    (void)sqlite3_close(pFile);
    return rc;
}

bool COffset::OptimizeDb(sqlite3* pdb)
{
    CHECK_DB;

    int rc = 0;
    string strsql;
    //stringstream strs;
    char *strErr = 0;

    try
    {
        //auto_vacuum
        strsql = "PRAGMA auto_vacuum = 0";
        rc = sqlite3_exec(pdb, strsql.c_str(), NULL,NULL, &strErr );

        //cache_size
        strsql = "PRAGMA cache_size = 8000";
        rc = sqlite3_exec(pdb, strsql.c_str(), NULL,NULL, &strErr );

        //count_changes
        strsql = "PRAGMA count_changes = 1";
        rc = sqlite3_exec(pdb, strsql.c_str(), NULL,NULL, &strErr );

        //synchronous
        strsql = "PRAGMA synchronous = OFF";
        rc = sqlite3_exec(pdb, strsql.c_str(), NULL,NULL, &strErr );

        //mem mode
        strsql = "PRAGMA temp_store = MEMORY;";
        rc = sqlite3_exec(pdb, strsql.c_str(), NULL,NULL, &strErr );
    }
    catch(...){
		assert(false);
	}
    return true;
}

////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
bool COffset::TruncateSnapshot()
{
	sqlite3 *pdb = NULL;
    char *zErrMsg = 0;
    bool fok = true;
	int rc = 0;

	string strSql;

	try
    {
        pdb = GetDb();   

		//truncate snapshot
		rc = sqlite3_exec( pdb, "delete from snapshot;", 0, 0, &zErrMsg );
        //if(rc){
        //    assert(false);
        //}
    }
    catch (...)
    {
        fok = false;
        assert(false);
    }
    return fok;
}

/////////////////////////////////////////////////////////////////////////////
//load snapshot cacheheader
bool COffset::LoadFile()
{
    sqlite3 *pdb = NULL;
    char *zErrMsg = 0;
    int rc = 0;
    bool fok = true;
    string strSql;

    try
    {
        AddDebug("start load offset.");

        pdb = GetDb();
        OptimizeDb(pdb);

        int nRow, nColumn;
        char ** dbResult; 
        strSql = "select * from snapshot;";
        rc = sqlite3_get_table( pdb, strSql.c_str(), &dbResult, &nRow, &nColumn, &zErrMsg );
        //rc = sqlite3_exec(pdb, strSql.c_str(), callback, 0, &zErrMsg);
        if( SQLITE_OK != rc ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            //sqlite3_close(pdb);
            return false;
        }      	
		nRow++;
        
        {
        }
		//release  dbResult
        sqlite3_free_table( dbResult );

		//check load vecdata
        //load when check hash	
        m_fParsedInDb = true;        

    }
    catch (...)
    {
        fok = false;
        assert(false);
    }
    //sqlite3_close(pdb);
	return true;
}

//bool fSaveAllSnap, eSnapType snapType
bool COffset::SaveFile()
{
	sqlite3 *pdb = GetDb();
    bool fok = true;
       
    //CloseDb(pdb);

    return fok;
}



