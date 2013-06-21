// TestHttpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestHttp.h"
#include "TestHttpDlg.h"

#include <string>
#include <vector>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString c_strStart = 
//_T("http://gaia.uat.karmalab.net:8100");
_T("http://gaia.uat.karmalab.net:8100/hotels/2/features?within=0km&type=region&verbose=3&cid=demo&apk=demo");
//_T("http://www.baidu.com");
const CString c_strLog = _T("d:\\1.log");
//_T("");
//_T("http://ambm.ku.net");
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

/////////////////////////////////////////////////////////////////////////////
// CTestHttpDlg dialog

CTestHttpDlg::CTestHttpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestHttpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestHttpDlg)
	m_strResponse = _T("");
	m_strRequest = c_strStart;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestHttpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestHttpDlg)
	DDX_Control(pDX, IDC_STATIC_SPEED, m_stcSpeed);
	DDX_Control(pDX, IDC_EDIT1, m_edtEdit1);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
	DDX_Control(pDX, IDC_LIST1, m_ctrlList);
	DDX_Text(pDX, IDC_EDIT1, m_strResponse);
	DDX_Text(pDX, IDC_EDIT2, m_strRequest);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestHttpDlg, CDialog)
	//{{AFX_MSG_MAP(CTestHttpDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestHttpDlg message handlers

BOOL CTestHttpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_ctrlProgress.ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestHttpDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		//CAboutDlg dlgAbout;
		//dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestHttpDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestHttpDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;

}

UINT DownloadThread(void *pArg)
{
	CTestHttpDlg *pMainWnd = (CTestHttpDlg *)pArg;
    if(!pMainWnd)
        return 0;

    pMainWnd->ThreadFunc();
    return 0;

}
int CTestHttpDlg::ThreadFunc()
{
	//CFileDialog FileDlg(FALSE);
    

	CHttpSocket HttpSocket;
	CString strServer,strObject;
	unsigned short nPort;
	DWORD dwServiceType;
	long nLength;
	const char *pRequestHeader = NULL;
	AfxParseURL(GetStrRequest(), dwServiceType, strServer, strObject, nPort);
	
    BOOL IFOK = FALSE;

	pRequestHeader = HttpSocket.FormatRequestHeader((LPTSTR)(LPCTSTR)strServer,(LPTSTR)(LPCTSTR)strObject,nLength);	
	IFOK = HttpSocket.Socket();
	IFOK = HttpSocket.Connect((LPTSTR)(LPCTSTR)strServer, nPort);

    //set reqestheader
    string strsend;
    if(1)
    {
        strsend = "GET http://gaia.uat.karmalab.net:8100/hotels/1/features?within=0km&type=region&verbose=3&apk=demo&cid=demo HTTP/1.1";   strsend+="\r\n";
        strsend+="Host: gaia.uat.karmalab.net:8100";   strsend+="\r\n";
        strsend+="Connection: keep-alive";   strsend+="\r\n";
        //strsend+="Cache-Control: max-age=0";   strsend+="\r\n";
        strsend+="Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";   strsend+="\r\n";
        strsend+="User-Agent: Mozilla/5.0 (Windows NT 5.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/27.0.1453.110 Safari/537.36";   strsend+="\r\n";
        strsend+="Accept-Encoding: gzip,deflate,sdch";   strsend+="\r\n";
        strsend+="Accept-Language: zh-CN,zh;q=0.8";   strsend+="\r\n";
    }
    else{
        strsend = "GET /hotels/1/features?within=0km&type=region&verbose=3&apk=demo&cid=demo HTTP/1.1";   strsend+="\r\n";
        strsend+="Host: gaia.uat.karmalab.net:8100";   strsend+="\r\n";
        strsend+="Connection: keep-alive";   strsend+="\r\n";
        //strsend+="Cache-Control: max-age=0";   strsend+="\r\n";
        strsend+="Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8";   strsend+="\r\n";
        strsend+="User-Agent: Mozilla/5.0 (Windows NT 5.2; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/27.0.1453.110 Safari/537.36";   strsend+="\r\n";
        strsend+="Accept-Encoding: gzip,deflate,sdch";   strsend+="\r\n";
        strsend+="Accept-Language: zh-CN,zh;q=0.8";   strsend+="\r\n";
    }

    //HttpSocket.SendRequest(strsend.c_str(), strsend.length() );
	HttpSocket.SetTimeout(10,0);
    HttpSocket.SendRequest( );
/*
	int nSize;
	HttpSocket.GetResponseHeader(nSize);*/

	m_edtEdit1.SetWindowText(pRequestHeader);
	int nLineSize = 0;
	char szLine[256];
	while(nLineSize != -1)
	{
		nLineSize = HttpSocket.GetResponseLine(szLine,256);
		if(nLineSize > -1)
		{
			szLine[nLineSize] = '\0';
			m_ctrlList.AddString(szLine);
		}
	}
	char szValue[30];
	HttpSocket.GetField("Content-Length",szValue,30);
	int nSvrState = HttpSocket.GetServerState();
	int nFileSize = atoi(szValue);
	m_ctrlProgress.ShowWindow(SW_SHOW);
	m_ctrlProgress.SetRange(0,nFileSize / 1024);
	
	
    bool ifwriteok = this->WriteFile(HttpSocket, nFileSize);
	
	m_ctrlProgress.ShowWindow(SW_HIDE);
	m_ctrlProgress.SetPos(0);

    string sdf;
    sdf.push_back('d');

	return 0;
}

bool CTestHttpDlg::WriteFile(CHttpSocket& rHttpSocket, int fileSize)
{
    int nCompletedSize = 0;

    //get log file name
	CString strFileName;	
    strFileName = c_strLog;

    CFile DownloadFile;
	DownloadFile.Open(strFileName,CFile::modeCreate | CFile::modeWrite);
	char pData[1024];
	int nReceSize = 0;
	DWORD dwStartTime,dwEndTime;

	while(nCompletedSize < fileSize)
	{
		dwStartTime = GetTickCount();
		nReceSize = rHttpSocket.Receive(pData,1024);
		if(nReceSize == 0)
		{
			AfxMessageBox("Server has been shut down..");
			break;
		}
		if(nReceSize == -1)
		{
			AfxMessageBox("receive timeout.");
			break;
		}
		dwEndTime = GetTickCount();
		DownloadFile.Write(pData,nReceSize);
		nCompletedSize += nReceSize;
		m_ctrlProgress.SetPos(nCompletedSize / 1024);
		
		//Speed
		CString strSpeed;
		//strSpeed.Format("%d",dwStartTime -dwEndTime);
		strSpeed.Format("%d",nReceSize);
		m_stcSpeed.SetWindowText(strSpeed);
	}
	DownloadFile.Close();
    return true;
}

void CTestHttpDlg::OnOK() 
{
	UpdateData();
	AfxBeginThread(DownloadThread,(void *)this);
}
