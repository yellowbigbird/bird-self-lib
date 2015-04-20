#include "stdafx.h"
#include "DataManager.h"

///////////////////////////
using namespace std;

CDataManager* CDataManager::m_pThis = NULL;
/////////////////////////

CDataManager::CDataManager()
	:m_pHttp(NULL)
{
}

CDataManager::~CDataManager()
{
}

CDataManager&	CDataManager::Get()
{
	if(!m_pThis){
		m_pThis = new CDataManager();
	}
	assert(m_pThis);
	return *m_pThis;
}

bool	CDataManager::SendRequest()
{
	return false;
}

void	Init()
{
}

