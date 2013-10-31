#include "stdafx.h"
#include "dcdata.h"

#include "httpSocket.h"

#include "util/UtilString.h"
#include "util/UtilsFile.h"

using namespace std;
////////////////////
CDcData::CDcData()
{
}

CDcData::~CDcData()
{
}

///////////////////////
bool CDcData::LoadRequest(const CString& filePathName)
{
	if(filePathName.GetLength() < 1)
		return false;
	
	string strData;
	wstring wstrFile = filePathName.GetString();
	bool fok = UtilFile::ReadFileAsString(wstrFile, strData);
	if(fok){
		m_strRequest = strData;
	}
	return true;
}

void	CDcData::SetUrl(const std::wstring& strUrl)
{
}

bool CDcData::SendRequest()
{
}