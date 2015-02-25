
// mfcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfc.h"
#include "mfcDlg.h"
#include "httpsdata.h"

#include "util/utilString.h"
#include "util/utilFile.h"

#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace UtilFile;
using namespace UtilString;

//////////////////////////

BEGIN_MESSAGE_MAP(CmfcDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUT_DOWNLOAD, &CmfcDlg::OnBnClickedButDownload)
END_MESSAGE_MAP()

CmfcDlg::CmfcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CmfcDlg::IDD, pParent)
	, m_strPath(_T("d:\\") )
	, m_strUrl(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_URL, m_strUrl);
}

// CmfcDlg message handlers

BOOL CmfcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CmfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CmfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CmfcDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnDropFiles(hDropInfo);
}

void CmfcDlg::OnBnClickedButDownload()
{
	UpdateData(TRUE); //control to value

	if(m_strUrl.IsEmpty() )
		return;

	if(m_strPath.IsEmpty() ){
		m_strPath = _T("c:\\");
	}

	//replace string
	const CString cstr1280 = _T("_1280.");
	const CString cstr540 = _T("_540.");
	const CString cstr500 = _T("_500.");
	CString cstrUrlNew = m_strUrl;
	int pos = cstrUrlNew.Find(cstr540 );
	if(pos >=0 && pos < cstrUrlNew.GetLength() ){
		cstrUrlNew.Replace(cstr540 , cstr1280 );
	}
	else{
		pos = cstrUrlNew.Find(cstr500 );
		if(pos >=0 && pos < cstrUrlNew.GetLength() ){
			cstrUrlNew.Replace(cstr500 , cstr1280 );
		}
	}

	//get name
	wstring wstrFileName = ExtractFileName(cstrUrlNew.GetString() );

	CString cstrFileFullPathName = m_strPath + wstrFileName.c_str();

	CHttpsData https;

	//download
	bool fok = https.GetData(cstrUrlNew, cstrFileFullPathName );
	if(!fok){
		//use  old name
		wstrFileName = ExtractFileName(m_strUrl.GetString() );
		cstrFileFullPathName = m_strPath + wstrFileName.c_str();

		fok = https.GetData(m_strUrl, cstrFileFullPathName );
	}
}

bool CmfcDlg::Download(const CString& strFileURLInServer, //待下载文件的URL 
			  const CString & strFileLocalFullPath)//存放到本地的路径 
{ 
	ASSERT(strFileURLInServer != ""); 
	ASSERT(strFileLocalFullPath != ""); 
	CInternetSession session; 
	CHttpConnection* pHttpConnection = NULL; 
	CHttpFile* pHttpFile = NULL; 
	CString strServer, strObject; 
	INTERNET_PORT wPort; 
	bool bReturn = false; 

	DWORD dwType; 
	const int nTimeOut = 2000; 
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeOut); //重试之间的等待延时 
	session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1); //重试次数 
	char* pszBuffer = NULL; 

	try 
	{ 
		AfxParseURL(strFileURLInServer, dwType, strServer, strObject, wPort); 
		pHttpConnection = session.GetHttpConnection(strServer, wPort); 
		pHttpFile = pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject); 
		if(pHttpFile->SendRequest() == FALSE) 
			return false; 
		DWORD dwStateCode; 

		pHttpFile->QueryInfoStatusCode(dwStateCode); 
		if(dwStateCode == HTTP_STATUS_OK) 
		{ 
			HANDLE hFile = CreateFile(strFileLocalFullPath, GENERIC_WRITE, 
				FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
				NULL); //创建本地文件 
			if(hFile == INVALID_HANDLE_VALUE) 
			{ 
				pHttpFile->Close(); 
				pHttpConnection->Close(); 
				session.Close(); 
				return false; 
			} 

			char szInfoBuffer[1000]; //返回消息 
			DWORD dwFileSize = 0; //文件长度 
			DWORD dwInfoBufferSize = sizeof(szInfoBuffer); 
			BOOL bResult = FALSE; 
			bResult = pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, 
				(void*)szInfoBuffer,&dwInfoBufferSize,NULL); 

			dwFileSize = atoi(szInfoBuffer); 
			const int BUFFER_LENGTH = 1024 * 10; 
			pszBuffer = new char[BUFFER_LENGTH]; //读取文件的缓冲 
			DWORD dwWrite, dwTotalWrite; 
			dwWrite = dwTotalWrite = 0; 
			UINT nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH); //读取服务器上数据 

			while(nRead > 0) 
			{ 
				WriteFile(hFile, pszBuffer, nRead, &dwWrite, NULL); //写到本地文件 
				dwTotalWrite += dwWrite; 
				nRead = pHttpFile->Read(pszBuffer, BUFFER_LENGTH); 
			} 

			delete[]pszBuffer; 
			pszBuffer = NULL; 
			CloseHandle(hFile); 
			bReturn = true; 
		} 
	} 
	catch(CInternetException* e) 
	{ 
		TCHAR tszErrString[256]={0}; 
		//e->GetErrorMessage(tszErrString, ArraySize(tszErrString)); 
		TRACE(_T("Download XSL error! URL: %s,Error: %s"), strFileURLInServer, tszErrString); 
		e->Delete(); 
	} 
	catch(...) 
	{ 
	} 

	if(pszBuffer != NULL) 
	{ 
		delete[]pszBuffer; 
	} 
	if(pHttpFile != NULL) 
	{ 
		pHttpFile->Close(); 
		delete pHttpFile; 
	} 
	if(pHttpConnection != NULL) 
	{ 
		pHttpConnection->Close(); 
		delete pHttpConnection; 
	} 
	session.Close(); 
	return bReturn; 
} 