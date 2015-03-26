
// LanTxtEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LanTxtEditor.h"
#include "LanTxtEditorDlg.h"

#include <common/UdpServer.h>
#include <common/config.h>
#include <common/HandleMsg.h>
#include <common/FileManager.h>

#include <util/UtilString.h>

//#pragma warning(disable:4244 4267)
//#include <common/proto/MessageProto.pb.h>

#include <common/MsgCmd.h>

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

//////////////////////////////////////////////////////////////////////////
// CLanTxtEditorDlg dialog

BEGIN_MESSAGE_MAP(CLanTxtEditorDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //ON_WM_CLOSE(OnClose)
    //}}AFX_MSG_MAP
    //ON_BN_CLICKED(IDC_BUT_TEST, &CLanTxtEditorDlg::OnButStartServer)
    ON_BN_CLICKED(IDC_BUT_SEND, &CLanTxtEditorDlg::OnButSend)

    ON_MESSAGE(WM_UPDATE_DEBUG, OnUpdateDebug) 
    ON_MESSAGE(WM_CLOSE, OnClose) 
    ON_BN_CLICKED(IDC_BUT_GET_FILE, &CLanTxtEditorDlg::OnBtnGetFile)
    ON_BN_CLICKED(IDC_BUT_RESTART, &CLanTxtEditorDlg::OnBtnRestart)
END_MESSAGE_MAP()

CLanTxtEditorDlg::CLanTxtEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLanTxtEditorDlg::IDD, pParent)
    , m_strFile(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_strSendIp = CConfig::GetInstance().m_strDestIpClient.c_str();
        //UtilString::ConvertMultiByteToWideChar(CConfig::GetInstance().m_strDestIpClient);
    m_portSend = CConfig::GetInstance().m_portClientSend;
    m_portListen = CConfig::GetInstance().m_portClientListen;
}

//////////////////////////////////////////////////////////////////////////
void CLanTxtEditorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SERVER_IP, m_strSendIp);
    DDX_Text(pDX, IDC_EDIT_PORT, m_portSend);
    DDX_Text(pDX, IDC_EDIT_PORT_LISTEN, m_portListen);
    DDX_Control(pDX, IDC_LIST1, m_listLog);
    DDX_Text(pDX, IDC_EDIT_FILE_NAME, m_strFile);
}

BOOL CLanTxtEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


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

	//InitServer();
    CFileManager::GetInstance().StartSearchPath();

    InitList();
    UpdateList();
    CDebug::GetInstance().SetUpdateWindow(GetSafeHwnd() );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLanTxtEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLanTxtEditorDlg::OnPaint()
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
HCURSOR CLanTxtEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//////////////////////////////////////////////////////////////////////////
void CLanTxtEditorDlg::InitList()
{
    m_listLog.InsertColumn( 0, _T("time"), LVCFMT_LEFT, 10 );
    m_listLog.InsertColumn( 1, _T("text"), LVCFMT_LEFT, 350 );
}

void CLanTxtEditorDlg::UpdateList()
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

LRESULT CLanTxtEditorDlg::OnUpdateDebug(WPARAM wParam, LPARAM lParam)
{
    UpdateList();
    return 0;
}
//////////////////////////////////////////////////////////////////////////

void CLanTxtEditorDlg::OnButSend()
{
}


void CLanTxtEditorDlg::OnBtnGetFile()
{
    UpdateData(TRUE); //control to value

    //save config
    wstring wstr = m_strSendIp.GetString();
    string str = UtilString::ConvertWideCharToMultiByte(wstr);
    CConfig::GetInstance().m_strDestIpClient = (str);
    CConfig::GetInstance().SaveFile();

    wstr = m_strFile.GetString();
    CHandleMsg::GetInstance().SendGetFileRequest(wstr);
    
    {
        CString cstr = _T("get file =");
        cstr += m_strFile;
        wstring wstr = cstr.GetString();
        AddDebug(wstr);

    }
}


void CLanTxtEditorDlg::OnBtnRestart()
{
    AddDebug("OnBtnRestart");
    CHandleMsg::GetInstance().SendRequstRestart();
}

//////////////////////////////////////////////////////////////////////////
void CLanTxtEditorDlg::OnCancel()
{
    //do nothing
}

void CLanTxtEditorDlg:: OnOK()
{
    //do nothing
}

LRESULT CLanTxtEditorDlg::OnClose(WPARAM wParam, LPARAM lParam)
{
    EndDialog(IDCANCEL);     
    CDialog::OnClose(); 
    return 0;
}
//////////////////////////////////////////////////////////////////////////

