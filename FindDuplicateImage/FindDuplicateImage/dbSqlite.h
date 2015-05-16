#pragma once

#include <list>


#include "util/utilString.h"

struct sqlite3;


//////////////////////////////////////////////////////////////////////////
class COffset
{
public:
	virtual ~COffset();

    static COffset& Get() ;
    static void DetroyInstance();

    void Close();

	bool LoadFile();
	bool SaveFile();

	//template<class T>
	//void SetAndSaveSnapshot();
    	
    bool GetIfParsedInDB() const;

protected:
    sqlite3*    GetDb();
    sqlite3*    OpenDb();
    void        CloseDb(sqlite3* pdb);
    bool        OptimizeDb(sqlite3* pdb);

    int         LoadOrSaveDb(sqlite3 *pInMemeory, const char *zFilename, bool isSave);
    std::string GetAbsFilePathName() const;

    bool        TruncateSnapshot();

private:
    COffset();

    static COffset*     m_pInstance;
    std::string         m_sqlFilePath;
    bool                m_fParsedInDb; //all info saved in DB
    CRITICAL_SECTION    m_cs;
    sqlite3*            m_pdb;

};



/////////////////////