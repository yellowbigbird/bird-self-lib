
// AddFileToVsprojectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AddFileToVsproject.h"
#include "AddFileToVsprojectDlg.h"

#include <util/UtilsFile.h>
//#include 
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAddFileToVsprojectDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_EN_CHANGE(IDC_EDIT1, &CAddFileToVsprojectDlg::OnEnChangeEdit1)
    ON_BN_CLICKED(ID_OPEN_PRJ, &CAddFileToVsprojectDlg::OnBtnOpenPrj)
    ON_BN_CLICKED(ID_ADD_PATH, &CAddFileToVsprojectDlg::OnBtnAddPath)
    ON_BN_CLICKED(IDC_BUT_OPEN_PATH, &CAddFileToVsprojectDlg::OnBtnOpenPath)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

CAddFileToVsprojectDlg::CAddFileToVsprojectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddFileToVsprojectDlg::IDD, pParent)
    , m_strRootPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAddFileToVsprojectDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PATH, m_strRootPath);
}


BOOL CAddFileToVsprojectDlg::OnInitDialog()
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAddFileToVsprojectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAddFileToVsprojectDlg::OnPaint()
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
HCURSOR CAddFileToVsprojectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//////////////////////////////////////////////////////////////////////////


void CAddFileToVsprojectDlg::OnEnChangeEdit1()
{
    UpdateData(TRUE); //control to value

    //m_strRootPath;
}

void CAddFileToVsprojectDlg::OnBtnOpenPrj()
{
    CFileDialog dlg(TRUE); //OPEN
    if(IDOK !=dlg.DoModal() )
        return;

    m_strPrjFilePathName = dlg.GetPathName();
    UpdateData(FALSE); //value to control
}

void CAddFileToVsprojectDlg::OnBtnAddPath()
{
    m_strRootPath;
}

void CAddFileToVsprojectDlg::OnBtnOpenPath()
{
    CFileDialog dlg(TRUE); //OPEN
    if(IDOK !=dlg.DoModal() )
        return;

    m_strRootPath = dlg.GetPathName();
    
    //check if it's a path
    wstring wstr = m_strRootPath.GetString();
    bool fExist = UtilFile::DirectoryExists(wstr);
    UpdateData(FALSE); //value to control
}
