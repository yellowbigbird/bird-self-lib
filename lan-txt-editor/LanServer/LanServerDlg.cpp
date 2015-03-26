
// LanServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LanServer.h"
#include "LanServerDlg.h"

#include "common\FileManager.h"
#include "common\message.h"
#include "common\UdpServer.h"
#include "common\config.h"
#include <common\DebugFile.h>

#include <util/UtilString.h>

//#include <lzmaWrapper.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////////

// CLanServerDlg dialog
const wstring c_strRootPathStart = _T("X:\\lan-txt-editor\\bird-self-lib\\src");

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLanServerDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUT_TEST, &CLanServerDlg::OnBnClickedButTest)
    ON_BN_CLICKED(IDC_BUT_UPDATE_PATH, &CLanServerDlg::OnBnClickedButUpdatePath)
    ON_BN_CLICKED(IDC_BUTTON2, &CLanServerDlg::OnBnClickedButSend)
    ON_MESSAGE(WM_UPDATE_DEBUG, OnUpdateDebug) 
END_MESSAGE_MAP()

CLanServerDlg::CLanServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLanServerDlg::IDD, pParent)
	, m_strCurIp(_T(""))
	, m_strRootPath(c_strRootPathStart.c_str() )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_strRootPath = CConfig::GetInstance().GetRootPath().c_str();
    m_strCurIp = UtilString::ConvertMultiByteToWideChar(CConfig::GetInstance().m_strDestIpClient).c_str();
}

void CLanServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_IP, m_strCurIp);
    DDX_Text(pDX, IDC_EDIT_ROOT_PATH, m_strRootPath);
    DDX_Control(pDX, IDC_LIST1, m_listLog);
}



// CLanServerDlg message handlers

BOOL CLanServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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
	CFileManager::GetInstance().StartSearchPath();

    InitList();
    UpdateList();
    CDebug::GetInstance().SetUpdateWindow(GetSafeHwnd() );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLanServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLanServerDlg::OnPaint()
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
HCURSOR CLanServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//////////////////////////////////////////////////////////////////////////
void CLanServerDlg::InitList()
{
    m_listLog.InsertColumn( 0, _T("time"), LVCFMT_LEFT, 10 );
    m_listLog.InsertColumn( 1, _T("text"), LVCFMT_LEFT, 350 );
}

void CLanServerDlg::UpdateList()
{
    m_listLog.SetRedraw(FALSE);
    //update
    {
        m_listLog.DeleteAllItems();
        vector<wstring> vecString;
        CDebug::GetInstance().GetVecString(vecString);
        int nRow = 0;
        for(UINT idx=0; idx< vecString.size(); idx++) {
            const wstring& wstr = vecString[idx];
            nRow = m_listLog.InsertItem(0, _T("time") );
            m_listLog.SetItemText(nRow, 1, wstr.c_str());
        }
        m_listLog.EnsureVisible(1, FALSE);
    }
    m_listLog.SetRedraw(TRUE);
    m_listLog.Invalidate();
    m_listLog.UpdateWindow();

}

LRESULT CLanServerDlg::OnUpdateDebug(WPARAM wParam, LPARAM lParam)
{
    UpdateList();
    return 0;
}
//////////////////////////////////////////////////////////////////////////

void CLanServerDlg::OnBnClickedButTest()
{
}


void CLanServerDlg::OnBnClickedButUpdatePath()
{
	UpdateData(TRUE); //control to value

    const wstring wstr = m_strRootPath.GetString();

    CConfig::GetInstance().SetRootPath(wstr);
    //CFileManager::GetInstance().SetPath(wstr);
	CFileManager::GetInstance().StartSearchPath();   

    CConfig::GetInstance().SaveFile();
    //save path to config
}

void CLanServerDlg::OnBnClickedButSend()
{
	CMessage msg;
	msg.m_data.push_back('a');
	//string strip = "192.168.1.3";
	//CUdpServer::GetInstance().SetSendIp(strip);
	CUdpServer::GetInstance().AddMsg(msg);    
}
//////////////////////////////////////////////////////////////////////////

