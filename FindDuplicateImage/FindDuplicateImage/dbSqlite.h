#pragma once
#include "asimovlib.h"

#include <list>

#include "new/RecordBase.h"
#include "new/teslaobj.h"  //enum snap

#include "util/utilString.h"
#include "new/CacheHeader.h"

struct sqlite3;
class CSnapBase;

class ASIMOVLIB_API COffsetSnap
{
public:
    COffsetSnap();

    void        SetType(const std::string& str);
    std::string GetTypeStr() const;

    UINT        m_id;       //id in db
    eSnapType   m_type;
    std::string m_name;     //just filename ,no path, no .old
    UINT        m_hash;
    UINT64      m_fileSize;
	//INT32		m_asimovVersion;
    bool        m_fChanged;
    VecRecord   m_vecRec;

	CCacheHeader        m_cacheHeader;
    CCacheHeader        m_cacheHeaderReduce;
};
void InitVecSnapTypeStr();
typedef std::list<COffsetSnap>  ListOffsetSnap  ;

//////////////////////////////////////////////////////////////////////////
class ASIMOVLIB_API COffset
{
public:
	virtual ~COffset();

    static COffset& Get() ;
    static void DetroyInstance();

    void Close();

	bool LoadFile();
	bool SaveFile();

	//template<class T>
	void SetAndSaveSnapshot(const CSnapBase* psnap);

	//template<class T>
	bool CheckNeedLoadSnapshot(CSnapBase* pSnap, bool fNeedLoad, std::string &strReduceId);
	
	ListOffsetSnap& GetListOffsetSnap(){return m_listSnap;};
	
    bool GetIfParsedInDB() const;

    //search sql
    int  FindHotelIDFromRoomTypeID(UINT roomTypeID);
    bool FindRoomTypeFromHotelId(UINT hotelId, VecRecord& vecRec);
    bool FindRateplanFromRoomtypeId(UINT roomtypeId, VecRecord& vecRec);
	
	//get VecRecord
	bool FindVecRecordFromXxxxx(eSnapType esnaptype
		, UINT id
		, const std::string&  field
		, VecRecord& vecRec);   
	bool FindVecRecordFromXxxxx(eSnapType snaptype
		, UINT searchId
		, VecRecord& vecRec
		, const std::string& strTable
		, const std::string& strType        
		);
	bool FindAllVecRecordFromXxxxx(eSnapType snaptype
		, VecRecord& vecRec								 
		);

protected:
    sqlite3*    GetDb();
    sqlite3*    OpenDb();
    void        CloseDb(sqlite3* pdb);
    bool        OptimizeDb(sqlite3* pdb);

    int LoadOrSaveDb(sqlite3 *pInMemeory, const char *zFilename, bool isSave);

	bool LoadSnapshot(sqlite3* pdb, COffsetSnap&, std::string strReduceId = "");
    bool SaveSnapshot(sqlite3* pdb, COffsetSnap& );

    bool LoadRecordBase(CRecordBase& entry
        , int rowIdx
		, char ** dbResult
		, eSnapType snapType);
    bool LoadRecordBase_skugroup(CRecordBase& entry
        , int rowIdx
        , char ** dbResult) const;
    bool LoadRecordBase_roomtype(CRecordBase& entry
        , int rowIdx
        , char ** dbResult) const;
    bool LoadRecordBase_rateplan(CRecordBase& entry
        , int rowIdx
        , char ** dbResult) const;

    bool LoadRecordBase_CancelPenalty(CRecordBase& entry
        , int rowIdx
        , char **dbResult)const;

    bool LoadRecordBase_HotelTaxRate(CRecordBase& entry
        , int rowIdx
        , char ** dbResult) const;

	bool SaveVecRecord(sqlite3* pdb, COffsetSnap& rsnap);
	//bool SaveRoomtype(sqlite3* pdb, int snapIdInDb, const VecRecord& vecdata);
	//bool SaveRateplan(sqlite3* pdb, int snapIdInDb, const VecRecord& vecdata);

	void GetSaveVecRecordStr(std::string& str
		,const CRecordBase& entry
        ,const COffsetSnap& rsnap
		, int idx);

	COffsetSnap* FindSnapshot(const std::string& snapshotName );
	std::string GetAbsFilePathName() const;
	bool        TruncateSnapshot();

	bool		FindAllVecRecordBase(eSnapType snaptype
		,const std::string& strsql
		, VecRecord& vecRec	
		);


	std::string		GetTableNameFromSnapType(eSnapType snaptype);

protected:
    ListOffsetSnap      m_listSnap;

private:
    COffset();

    static COffset*     m_pInstance;
    std::string         m_sqlFilePath;
    bool                m_fParsedInDb; //all info saved in DB
    CRITICAL_SECTION    m_cs;
    sqlite3*            m_pdb;

};
/////////////////



/////////////////////