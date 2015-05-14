#include "stdafx.h"
#include "OffsetSqlite.h"

#include "config.h"
#include <new/snapbase.h>

#include "../sqlitelib/sqlite3.h"
#include "util/UtilFile.h"

#include <stdlib.h>

#pragma warning(disable: 4996)

using namespace std;
//////////////////////////////////////////////////////////////////////////
#define IF_USE_MEM_SQLITE       1

const char* c_strFileName = "offset.s3db";

const char* c_snapshot = "snapshot";
const char* c_hash = "hash";
const char* c_file = "file";
const char* c_start = "st";
const char* c_len = "len";
const char* c_type = "type";
const char* c_sku = "sku";
const char* c_roomtype = "rt";
const char* c_rateplan = "rp";
const char* c_record = "rec";


COffset* COffset::m_pInstance = NULL;

static vector<string>   g_vecSnapTypeStr;

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

enum eDbSkugroup
{
    eSku_id=0,
	//eSku_snapshotId,
	eSku_start,
	eSku_length,

    eSku_Max,
};

enum eDbRoomtype
{
    eRt_id=0,
	//eRt_snapshotId,
	eRt_start,
	eRt_length,
	eRt_skuId,

    eRt_Max,
};

enum eDbRateplan
{
    eRp_id=0,
	//eRp_snapshotId,
	eRp_start,
	eRp_length,
	eRp_skuId,
	eRp_roomtypeId,

    eRp_Max,
};

enum eDbCancelPenalty
{
    //eCp_index = 0,
    eCp_id,
    //eCp_Snapshopid,
    eCp_start,
    eCp_length,
    eCp_skugroupid,

    eCp_Max,
};


enum eDbHotalTaxRate
{
    eHtr_id = 0,
	//eHtr_snapshotId,
    eHtr_start,
    eHtr_length,
    eHtr_skugroupid,

    eHtr_Max,
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
COffsetSnap::COffsetSnap()
:m_hash(0)
,m_fileSize(0)
//,m_asimovVersion(0)
,m_id(0)
,m_fChanged(false)
{
}

void InitVecSnapTypeStr()
{
    g_vecSnapTypeStr.clear();
    g_vecSnapTypeStr.resize(eSnapUnkown+1);

    g_vecSnapTypeStr[eSkuGroup] = ("skugroup");
    g_vecSnapTypeStr[eRoomType] = ("roomtype");
    g_vecSnapTypeStr[eRatePlan] = ("rateplan");
    g_vecSnapTypeStr[eSnapCancelPenalty] = ("cancelpenalty");
    g_vecSnapTypeStr[eSnapHotelTaxRate] = ("hoteltaxrate");
    
	g_vecSnapTypeStr[eSnapUnkown] = ("unknown");
}

void COffsetSnap::SetType(const string& strSrc)
{
	string str = strSrc;
	UtilString::ChangeStringLetter(str, false);
	size_t pos = 0;
    for(UINT idx=0; idx< g_vecSnapTypeStr.size(); idx++)
	{
		pos = str.find(g_vecSnapTypeStr[idx]);
		if(str == g_vecSnapTypeStr[idx]
		|| pos < str.length()
			){
           m_type = static_cast<eSnapType>(idx);
           return;
        }
    }
	m_type = eSnapUnkown;
}

std::string COffsetSnap::GetTypeStr() const
{
    if((size_t)m_type >= g_vecSnapTypeStr.size() ){
        assert(false);
        return "max";
    }
    return g_vecSnapTypeStr[m_type];
}
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
        InitVecSnapTypeStr();

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
    const wstring wstrPath = CConfig::GetInstance().GetDataPath(); //UtilFile::GetFilePath()
    string strFile = UtilString::ConvertWideCharToMultiByte(wstrPath);
    string strver = CConfig::GetInstance().GetCurSnapVersionNo();
    strFile = strFile + ("\\") + strver + "_" + c_strFileName;
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
    stringstream strs;
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
int COffset::FindHotelIDFromRoomTypeID(UINT roomTypeID)
{
	sqlite3 *pdb = NULL;
    char *zErrMsg = 0;
    int rc = 0;
    bool fok = true;
    string strSql;
	char ID[32] = {0};
	_snprintf(ID, sizeof(ID)-1,"%d",roomTypeID);

	int skugroup_id = 0;

    try
    {
        //printf("start find hotelid.\n");

        pdb = GetDb();

        int nRow, nColumn;
        char ** dbResult; 
        strSql = "select * from roomtype where id=";
		strSql += ID;
		strSql += ";";
        rc = sqlite3_get_table( pdb, strSql.c_str(), &dbResult, &nRow, &nColumn, &zErrMsg );
        //rc = sqlite3_exec(pdb, strSql.c_str(), callback, 0, &zErrMsg);
        if( SQLITE_OK != rc ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            //sqlite3_close(pdb);
            return 0;
        }      	
		nRow++;

        int idx = 0;
        INT64 i64Temp = 0;
        string str;
        //1st row is value name, row need to +1
        for(int  rowIdx = 1; rowIdx < nRow ; rowIdx++ )
        {
            //name, type
            idx = rowIdx*eRt_Max + eRt_skuId;
            skugroup_id = atoi(dbResult[idx ]);            
        }
		//release  dbResult
        sqlite3_free_table( dbResult );

    }
    catch (...)
    {
        assert("FindHotelIDFromRoomTypeID" && false);
        fok = false;
    }
    //CloseDb(pdb);
	return skugroup_id;
}

////////////////////////
bool COffset::FindRoomTypeFromHotelId(UINT hotelId, VecRecord& vecRec)
{    
    const string strTable = "roomtype";
    const string strType = "skugroup_id";
    bool fok = FindVecRecordFromXxxxx(eRoomType, hotelId, vecRec, strTable, strType );
    return fok;
}

bool COffset::FindRateplanFromRoomtypeId(UINT roomtypeId, VecRecord& vecRec)
{
    const string strTable = "rateplan";
    const string strType = "roomtype_id";
    bool fok = FindVecRecordFromXxxxx(eRatePlan, roomtypeId, vecRec, strTable, strType );
    return fok;
}

///////////////////////
string		COffset::GetTableNameFromSnapType(eSnapType snaptype)
{
	string strTable ;
	switch(snaptype){
		case eRatePlan:
			strTable = "rateplan";
			break;
		case eSkuGroup:
			strTable = "skugroup";
			break;
		case eRoomType:
			strTable = "roomtype";
			break;
		case eSnapCancelPenalty:
			strTable = "cancelpenalty";
			break;
		case eSnapHotelTaxRate:
			strTable = "hoteltaxrate";
			break;
		default:
			AddDebug("COffset::GetTableNameFromSnapType snaptype error!!!");
			assert(false);
	}
	return strTable;
}

bool COffset::FindVecRecordFromXxxxx(eSnapType snaptype
		, UINT id
		, const std::string&  strType
		, VecRecord& vecRec)
{
	string strTable = GetTableNameFromSnapType(snaptype);
	
	bool fok = FindVecRecordFromXxxxx(snaptype, id, vecRec, strTable, strType);
    return fok;
}
/////////////////////////////////////////////////////////////////////////////////////////

bool COffset::FindVecRecordFromXxxxx(eSnapType snaptype
									 , UINT searchId
									 , VecRecord& vecRec
									 , const std::string& strTable
									 , const std::string& strType									 
									 )
{
	bool fok = true;

	string strSql;
	const int c_id_size = 32;
	char ID[c_id_size] = {0};
	_snprintf(ID, c_id_size -1,"%d", searchId);

	strSql = "select * from ";
	strSql += strTable;
	strSql += " where ";
	strSql +=  strType;   
	strSql += " =";
	strSql += ID;
	strSql += ";";

	fok = FindAllVecRecordBase(snaptype,strSql, vecRec);
	return fok;
}

bool COffset::FindAllVecRecordFromXxxxx(eSnapType snaptype
									 , VecRecord& vecRec								 
									 )
{
	const string strTable = GetTableNameFromSnapType(snaptype);
	if(strTable.empty() )
		return false;

    bool fok = true;

	string strSql;
	strSql = "select  * from ";
	strSql += "[";
	strSql += strTable;
	strSql += "]";
	strSql += " limit 20"; //20 should be a parameter
	strSql += ";";

	fok = FindAllVecRecordBase(snaptype,strSql, vecRec);
    return fok;
}

bool	COffset::FindAllVecRecordBase(eSnapType snaptype
		,const std::string& strSql
		, VecRecord& vecRec	
		)
{
    char *zErrMsg = 0;
    bool fok = true;

    try
    {
        sqlite3 *pdb = GetDb();

        int nRow =0, nColumn = 0;
        char ** dbResult; 
        
        int rc = sqlite3_get_table( pdb, strSql.c_str(), &dbResult, &nRow, &nColumn, &zErrMsg );
        //rc = sqlite3_exec(pdb, strSql.c_str(), callback, 0, &zErrMsg);
        if( SQLITE_OK != rc ){
            assert(false);
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
            //CloseDb(pdb);
            return 0;
        }      	
        nRow++;

        CRecordBase entry;
        vecRec.clear();

        //1st row is value name, row need to +1
        for(int  rowIdx = 1; rowIdx < nRow ; rowIdx++ )
        {
            LoadRecordBase(entry, rowIdx, dbResult, snaptype ) ;
            vecRec.push_back(entry);
        }
        //release  dbResult
        sqlite3_free_table( dbResult );

        if(vecRec.empty() )
            fok =false;
    }
    catch (...)
    {
        assert("FindRoomTypeFromHotelId" && false);
        fok = false;
    }
    //CloseDb(pdb);
    return fok;
}
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

        COffsetSnap offsnap;
        int idx = 0;
        UINT64 i64Temp = 0;
        string str;
        //1st row is value name, row need to +1
        for(int  rowIdx = 1; rowIdx < nRow ; rowIdx++ )
        {
            //name, type
            idx = rowIdx*eDbSnap_max + eDbSnap_name;
            offsnap.m_name = dbResult[idx ];            
            offsnap.SetType(offsnap.m_name);
            if(eSnapUnkown == offsnap.m_type)
                continue;

            //id
            idx = rowIdx*eDbSnap_max + eDbSnap_id;
            offsnap.m_id = atoi(dbResult[idx ]);

            //hash
            idx = rowIdx*eDbSnap_max + eDbSnap_hash;
            i64Temp = strtoul(dbResult[idx ], NULL, 0 );
            offsnap.m_hash = i64Temp & 0xffffffff;

            //file size
            idx = rowIdx*eDbSnap_max + eDbSnap_fileSize;
            i64Temp = _strtoui64(dbResult[idx ], NULL, 0 );  //strtoul  
            offsnap.m_fileSize = i64Temp ;  //UINT64

			//asimovVersion
			idx = rowIdx*eDbSnap_max + eDbSnap_asimovVersion;
			offsnap.m_cacheHeader.m_version = atoi(dbResult[idx ]);  //int32 m_asimovVersion
			
            m_listSnap.push_back(offsnap);
        }
		//release  dbResult
        sqlite3_free_table( dbResult );

		//check load vecdata
        //load when check hash	
        if(m_listSnap.size() >= eSnapUnkown){
            m_fParsedInDb = true;
        }

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

    //save each snapshot
    UINT idx=0;
    for(ListOffsetSnap::iterator it = m_listSnap.begin();
        it != m_listSnap.end();
        it++)
    {
        COffsetSnap& offSnap = *it;
        if(eSnapUnkown == offSnap.m_type
            //|| !fSaveAllSnap && (snapType != offSnap.m_type)
            )
            continue;

        SaveSnapshot(pdb, offSnap); 

        //if(!fSaveAllSnap )
        //    break; //just save 1 snapshot        
    }

    //CloseDb(pdb);

    return fok;
}

COffsetSnap* COffset::FindSnapshot(const std::string& snapshotName)
{
	COffsetSnap* pSnap = NULL;
	UINT idx=0;
	for(ListOffsetSnap::iterator it = m_listSnap.begin();
		it != m_listSnap.end();
		it++, idx++)
	{
		COffsetSnap& rsnap = *it;
		if(snapshotName == rsnap.m_name){
			pSnap = &rsnap;
			break;
		}
	}
	return pSnap;
}

//////////////////////////////////////////////////////////////////////////
bool COffset::LoadSnapshot(sqlite3* pdb, COffsetSnap& rOffSnap, std::string strReduceId)
{
	CHECK_DB;

	int result = 0;
	string strsql;
	char* strErr = NULL;
    char ** dbResult = NULL; 
	stringstream strs;
	bool fok = true;
	
	try{
		strs.str("");
		//select * from skugroup where snapshot_id = 5
		if (strReduceId.size() == 0)
		{
			strs<<"select * from " <<rOffSnap.GetTypeStr()<< ";"; 
		}
		else
		{
			if (rOffSnap.m_type == eSkuGroup)
			{
				strs<<"select * from " <<rOffSnap.GetTypeStr()
                    << " where id in(" << strReduceId << ")" << " order by start asc;";
			}
			else
			{
				strs<<"select * from " <<rOffSnap.GetTypeStr() << " where skugroup_id in(" << strReduceId << ") order by start asc;";
			}
		}
		strsql = strs.str();

		int nRow = 0, nColumn=0;
		result = sqlite3_get_table( pdb, strsql.c_str(), &dbResult, &nRow, &nColumn, &strErr );
		if(SQLITE_OK != result
			|| !dbResult){
				return false;
		}
		nRow++;
		CRecordBase entry;
		bool fok = true;
		int idx = 0;
		rOffSnap.m_vecRec.clear();
		for(int  rowIdx = 1; rowIdx < nRow ; rowIdx++ )
        {
			entry.Clear();

			fok = LoadRecordBase(entry, rowIdx, dbResult, rOffSnap.m_type );

			rOffSnap.m_vecRec.push_back(entry);
		}
		sqlite3_free_table( dbResult );		
	}
	catch(...){
		assert(false);
		fok =false;
	}
	return fok;
}

bool COffset::SaveSnapshot(sqlite3* pdb, COffsetSnap& rOffSnap)
{
	CHECK_DB;

	int result = 0;
	string strsql;
	char* strErr;
	stringbuf strbuf;
    char ** dbResult; 
	bool fok = true;

	try{
		//todo check if snap table exist
		//int snapIdInDb = 0;
        stringstream strs;

        //todo uint64 lltostr,  sprintf("%lld")

        strs<<"insert into snapshot(hash, name, file_size, asimov_version) values(" 
            << rOffSnap.m_hash 
			<< ",'" <<rOffSnap.m_name << "'," 
			<< rOffSnap.m_fileSize 
			<<	"," << rOffSnap.m_cacheHeader.m_version 
            << ");" ;
        strsql = strs.str();		
		result = sqlite3_exec(pdb, strsql.c_str(), NULL, NULL, &strErr );
        if( SQLITE_OK == result ){
            //get snap id
            //select id from snapshot where name="test.snap"
            strs.str(""); 
            strs<<"select id from snapshot where name='"
                <<rOffSnap.m_name 
                << "';";
            strsql = strs.str();
            int nRow = 0, nColumn=0;
            result = sqlite3_get_table( pdb, strsql.c_str(), &dbResult, &nRow, &nColumn, &strErr );
            if(SQLITE_OK == result
				&& dbResult){					
					nRow++;
					rOffSnap.m_id = atoi(dbResult[1] ) ;
					sqlite3_free_table( dbResult );
            }
        }
        else{
            //id don't change
            //update 
            strs.str("");  
            strs<<"update snapshot "
                <<"set hash=" << rOffSnap.m_hash << ", "
                << "file_size=" << rOffSnap.m_fileSize << " " 
                << "where name='" << rOffSnap.m_name << "'"
                << ";" ;
            strsql = strs.str();
            result = sqlite3_exec(pdb, strsql.c_str(), NULL, NULL, &strErr );
        }
        
        if(rOffSnap.m_vecRec.empty()
            || !rOffSnap.m_fChanged)
            return true;

		SaveVecRecord(pdb, rOffSnap);		
	}
	catch(...){
        assert(false);
		fok = false;
	}
    return fok;
}
///////////////////////////////

bool COffset::LoadRecordBase(CRecordBase& entry
							 , int rowIdx
							 , char **  dbResult
							 , eSnapType snapType
							 )
{
	if(!dbResult)
		return false;

	int idx = 0;
	entry.Clear();

	switch(snapType){
case eSkuGroup:
	LoadRecordBase_skugroup(entry,rowIdx, dbResult);
	break;
case eRoomType:
	LoadRecordBase_roomtype(entry,rowIdx, dbResult);
	break;
case eRatePlan:
	LoadRecordBase_rateplan(entry,rowIdx, dbResult);
	break;
case eSnapCancelPenalty:
    LoadRecordBase_CancelPenalty(entry,rowIdx, dbResult);
    break;
case eSnapHotelTaxRate:
    LoadRecordBase_HotelTaxRate(entry, rowIdx, dbResult);
    break;
default:
	assert(false);
	return false;
	break;
	}
	return true;
}

bool COffset::LoadRecordBase_skugroup(CRecordBase& entry
                                           , int rowIdx
                                           , char ** dbResult                                           
                                           ) const
{
    int idx = 0;

    //sku id
    idx = rowIdx*eSku_Max + eSku_id;
    entry.m_skuGroupID = atoi(dbResult[idx ]);

    //start  64 bit UINT64
    idx = rowIdx*eSku_Max + eSku_start;
    entry.m_fileStart = atol(dbResult[idx ]);

    //length
    idx = rowIdx*eSku_Max + eSku_length;
    entry.m_fileDataLen = atoi(dbResult[idx ]);
    return true;
}

bool COffset::LoadRecordBase_roomtype(CRecordBase& entry
                                      , int rowIdx
                                      , char ** dbResult                                           
                                      ) const
{
    int idx =0 ;
    //sku id
    idx = rowIdx*eRt_Max + eRt_id;
    entry.m_roomTypeID = atoi(dbResult[idx ]);

    //start
    idx = rowIdx*eRt_Max + eRt_start;
    entry.m_fileStart = atol(dbResult[idx ]);

    //length
    idx = rowIdx*eRt_Max + eRt_length;
    entry.m_fileDataLen = atoi(dbResult[idx ]);

    //sku id
    idx = rowIdx*eRt_Max + eRt_skuId;
    entry.m_skuGroupID = atoi(dbResult[idx ]);
    return true;
}

bool COffset::LoadRecordBase_rateplan(CRecordBase& entry
                                      , int rowIdx
                                      , char ** dbResult                                           
                                      ) const
{
    int idx=0;
    //sku id
    idx = rowIdx*eRp_Max + eRp_id;
    entry.m_ratePlanID = atoi(dbResult[idx ]);

    //start
    idx = rowIdx*eRp_Max + eRp_start;
    entry.m_fileStart = _strtoui64(dbResult[idx ],  NULL, 0 );   //todo , UInt64 strtoll

    //length
    idx = rowIdx*eRp_Max + eRp_length;
    entry.m_fileDataLen = atoi(dbResult[idx ]);

    //sku id
    idx = rowIdx*eRp_Max + eRp_skuId;
    entry.m_skuGroupID = atoi(dbResult[idx ]);

    //roomtype id
    idx = rowIdx*eRp_Max + eRp_roomtypeId;
    entry.m_roomTypeID = atoi(dbResult[idx ]);

    return true;
}

bool COffset::LoadRecordBase_CancelPenalty(CRecordBase& entry, int rowIdx, char **dbResult)const
{
    int idx = 0;

    idx = rowIdx * eCp_Max + eCp_start;
    entry.m_fileStart = atoi(dbResult[idx]);

    idx = rowIdx * eCp_Max + eCp_length;
    entry.m_fileDataLen = atoi(dbResult[idx]);

    idx = rowIdx * eCp_Max + eCp_skugroupid;
    entry.m_skuGroupID = atoi(dbResult[idx]);
    return true;
}

bool COffset::LoadRecordBase_HotelTaxRate(CRecordBase& entry, int rowIdx, char ** dbResult) const
{
    int idx = 0;
    idx = rowIdx * eHtr_Max + eHtr_id;
    entry.m_taxRateID = atoi(dbResult[idx]);

    idx = rowIdx * eHtr_Max + eHtr_start;
    entry.m_fileStart = atoi(dbResult[idx]);

    idx = rowIdx * eHtr_Max + eHtr_length;
    entry.m_fileDataLen = atoi(dbResult[idx]);

    idx = rowIdx * eHtr_Max + eHtr_skugroupid;
    entry.m_skuGroupID = atoi(dbResult[idx]);
    return true;
}

////////////////////
void COffset::GetSaveVecRecordStr(string& str
								  , const CRecordBase& entry
								  , const COffsetSnap& rOffSnap
                                  , int idx)
{
    stringstream strs;
	strs.str(""); 
	switch(rOffSnap.m_type){
case eSkuGroup:
	strs<<"insert into " << rOffSnap.GetTypeStr()<<"(id , start, length) values(" 
		<< entry.m_skuGroupID << "," 
		<< entry.m_fileStart <<"," << entry.m_fileDataLen
		<< ");" ;
	break;
case eRoomType:
	strs<<"insert into "/*roomtype*/<< rOffSnap.GetTypeStr()<<"(id, start, length, skugroup_id) values(" 
		<< entry.m_roomTypeID << "," 
		<< entry.m_fileStart <<"," << entry.m_fileDataLen <<"," <<entry.m_skuGroupID
		<< ");" ;
	break;
case eRatePlan:
	strs<<"insert into "/*rateplan*/<< rOffSnap.GetTypeStr()<<"(id, start, length, skugroup_id, roomtype_id) values(" 
		<< entry.m_ratePlanID << "," 
		<< entry.m_fileStart <<"," << entry.m_fileDataLen 
		<<"," <<entry.m_skuGroupID <<"," << entry.m_roomTypeID
		<< ");" ;
	break;
case eSnapCancelPenalty:
    strs << "insert into " << rOffSnap.GetTypeStr()//cancelpenalty 
        <<"(idx, start, length, skugroup_id) values("
        << idx << ","
        << entry.m_fileStart << "," << entry.m_fileDataLen 
        <<"," <<entry.m_skuGroupID
        << ");";

    break;
case eSnapHotelTaxRate:
    strs << "insert into " << rOffSnap.GetTypeStr()//HotelTaxRate
        <<"(id, start, length, skugroup_id) values("
        << entry.m_taxRateID << ","
        << entry.m_fileStart << "," << entry.m_fileDataLen 
        <<"," <<entry.m_skuGroupID
        << ");";
    break;
default:
	assert(false);
    return;
	break;
	}
    str = strs.str();
}

bool COffset::SaveVecRecord(sqlite3* pdb
							,COffsetSnap& rOffSnap
						   )
{
	CHECK_DB;

	char* strErr = 0;
	bool fok = true;
    int result = 0;
    stringstream strs;
    string strsql;

	try{
        //clear old data 1st
        //"delete from skugroup where snapshot_id= 2" 
        strs.str(""); 
        strs<<"delete from " << rOffSnap.GetTypeStr()
            <<" where snapshot_id=" << rOffSnap.m_id
            ;
        strsql = strs.str();	
        result = sqlite3_exec( pdb, strsql.c_str(), 0, 0, &strErr );

        const VecRecord& vecdata = rOffSnap.m_vecRec;
		//add snap table
		result = sqlite3_exec( pdb, "begin transaction", 0, 0, &strErr );

        UINT idx=0;
        for(VecRecord::const_iterator it = vecdata.begin();
            it != vecdata.end();
            it++, idx++)
        {
            const CRecordBase& entry = *it;

			GetSaveVecRecordStr(strsql, entry, rOffSnap, idx);
            
            //strsql = strs.str();		
            result = sqlite3_exec(pdb, strsql.c_str(), NULL, NULL, &strErr );
        }
		result = sqlite3_exec( pdb, "commit transaction", 0, 0, &strErr );
	}
	catch(...){
		assert(false);
		fok = false;
	}
	return fok;
}

//////////////////////////////////////////////////////////////////////////
//if need load , return true
bool COffset::CheckNeedLoadSnapshot(CSnapBase* pSnapSrc, bool fNeedLoad, std::string &strReduceId )
{
	if(! pSnapSrc){
		assert(false);
		return false;
	}
	CSnapBase& snapSrc = *pSnapSrc;

	const std::string strOriginal = UtilString::ConvertWideCharToMultiByte(snapSrc.m_wstrFileNameOriginal);
	if(strOriginal.empty() )
		return false;
	COffsetSnap* pSnap = FindSnapshot(strOriginal);
	if(!pSnap)
		return true;

	COffsetSnap& rOffSnap = *pSnap;

    std::string str = snapSrc.GetFileName();
    AddDebug(str );

	bool ifReduce = CConfig::GetInstance().GetIfReduce();
	if(!fNeedLoad && !ifReduce){
		//need to set sth
		snapSrc.m_cacheHeader.m_version = rOffSnap.m_cacheHeader.m_version;
		snapSrc.m_cacheHeaderReduce.m_version = rOffSnap.m_cacheHeader.m_version;
        return true;
	}

	//compare filedata, don't check hash , only size && version
	if(rOffSnap.m_fileSize == snapSrc.m_fileSize){
        AddDebug(str + " load from db.");
        //start load offset
        sqlite3* pdb= GetDb();
		{
            LoadSnapshot(pdb, rOffSnap, strReduceId);
        }
        //CloseDb(pdb);

		//copy data
		snapSrc.m_fLoadListDone = true;
		snapSrc.SetListBuf(rOffSnap.m_vecRec);
		snapSrc.m_cacheHeader.m_version = rOffSnap.m_cacheHeader.m_version;
		snapSrc.m_cacheHeaderReduce.m_version = rOffSnap.m_cacheHeader.m_version;
		return false;
	}
    else{
		TruncateSnapshot();
		m_listSnap.clear();
        AddDebug("TruncateSnapshot, load from Asimov file.");
    }

	return false;
}

void COffset::SetAndSaveSnapshot(const CSnapBase* pSnapSrc)
{
	if(!pSnapSrc){
		assert(false);
		return;
	}
	const CSnapBase& snapSrc = *pSnapSrc;

	//find if snap exist
	const std::string strOriginal = UtilString::ConvertWideCharToMultiByte(snapSrc.m_wstrFileNameOriginal);
	if(strOriginal.empty() )
		return;
	COffsetSnap* pSnap = FindSnapshot(strOriginal);
	if(!pSnap){
		m_listSnap.push_back(COffsetSnap() );
		ListOffsetSnap::reverse_iterator it = m_listSnap.rbegin();
		COffsetSnap& rSnapTemp = *it;
		pSnap = &rSnapTemp;
	}
	COffsetSnap& rSnap = *pSnap;

    //check if changed
    if(rSnap.m_fileSize == snapSrc.m_fileSize){
		rSnap.m_cacheHeader = snapSrc.m_cacheHeader;
        rSnap.m_fChanged = false;
        return;
    }
    else{
        rSnap.m_fChanged = true;
    }

	//set attrib
	rSnap.m_name = strOriginal; //no path ,no .old
	rSnap.m_hash = 0; //snapSrc.m_hash;
	rSnap.m_type = snapSrc.m_type;
    rSnap.m_fileSize = snapSrc.m_fileSize;

	rSnap.m_cacheHeader = snapSrc.m_cacheHeader;
	rSnap.m_cacheHeaderReduce = snapSrc.m_cacheHeaderReduce;
	//rSnap.m_vecRec = snapSrc.m_list.m_dataBuf;
	snapSrc.GetListBuf(rSnap.m_vecRec);

    //save 
    {
        sqlite3 *pdb = GetDb();
        SaveSnapshot(pdb, rSnap); 
        //CloseDb(pdb);
    }
}
