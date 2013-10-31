
// DcTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DcTest.h"
#include "DcTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
////////////////////////
//////////////////////////////
// CDcTestDlg dialog




CDcTestDlg::CDcTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDcTestDlg::IDD, pParent)
	, m_strUrl(_T("localhost::8080"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDcTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_URL, m_comboUrl);
	DDX_Text(pDX, IDC_EDIT_URL, m_strUrl);
}

BEGIN_MESSAGE_MAP(CDcTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUT_GO, &CDcTestDlg::OnBnClickedButGo)
	ON_BN_CLICKED(IDC_BUT_OPEN, &CDcTestDlg::OnBnClickedButOpen)
	ON_BN_CLICKED(IDC_BUT_RELOAD, &CDcTestDlg::OnBnClickedButReload)
END_MESSAGE_MAP()


// CDcTestDlg message handlers

BOOL CDcTestDlg::OnInitDialog()
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

void CDcTestDlg::OnPaint()
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
HCURSOR CDcTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

///////////////////////////

void CDcTestDlg::OnBnClickedButGo()
{
	UpdateData(TRUE);  //control to value
	if(m_strUrl.IsEmpty() )
		return;

	const wstring wstrUrl = m_strUrl;
	m_data.SetUrl(wstrUrl);
	bool fok = m_data.SendRequest();
}

void CDcTestDlg::OnBnClickedButOpen()
{
	CFileDialog dlg(TRUE, _T("*.xml") );
	if(IDOK == dlg.DoModal() )	{		
		m_strFileRequest = dlg.GetPathName();
		bool fok = m_data.LoadRequest(m_strFileRequest);
	}

}

void CDcTestDlg::OnBnClickedButReload()
{
}
