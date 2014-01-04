
// FreeCellCalculateC++Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "FreeCellCalculateC++.h"
#include "FreeCellCalculateC++Dlg.h"

#include "card.h"
#include "Calculate.h"
#include "UtilString.h"

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace UtilString;
//////////////////////////////////////////////////////////////////////////
// CFreeCellCalculateCDlg dialog

CFreeCellCalculateCDlg::CFreeCellCalculateCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFreeCellCalculateCDlg::IDD, pParent)
	, m_dwGameNumber(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFreeCellCalculateCDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_dwGameNumber);
    DDX_Control(pDX, IDC_LIST1, m_listResult);
}

BEGIN_MESSAGE_MAP(CFreeCellCalculateCDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, &CFreeCellCalculateCDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON1, &CFreeCellCalculateCDlg::OnBtnCalc)
END_MESSAGE_MAP()


// CFreeCellCalculateCDlg message handlers

BOOL CFreeCellCalculateCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    InitList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFreeCellCalculateCDlg::OnPaint()
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
HCURSOR CFreeCellCalculateCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFreeCellCalculateCDlg::OnBnClickedOk()
{
    OnOK();
}

void CFreeCellCalculateCDlg::OnBtnCalc()
{
	UpdateData();	//control to value

    CCalculate cal;
    cal.Run(m_dwGameNumber);
    m_vecStr = cal.m_vecStrStep;

    UpdateList();
}

void CFreeCellCalculateCDlg::InitList()
{
    m_listResult.InsertColumn( 0, _T("step"), LVCFMT_LEFT, 10 );
    m_listResult.InsertColumn( 1, _T("text"), LVCFMT_LEFT, 350 );
}

void CFreeCellCalculateCDlg::UpdateList()
{
    m_listResult.SetRedraw(FALSE);
    wstring wstr ;

    //update
    {
        m_listResult.DeleteAllItems();

        int nRow = 0;
        for(UINT idx=0; idx< m_vecStr.size(); idx++) {
            const string& str = m_vecStr[idx];
            wstr = ConvertMultiByteToWideChar(str);
            nRow = m_listResult.InsertItem(idx, _T("") );
            m_listResult.SetItemText(nRow, 1, wstr.c_str());
        }
        m_listResult.EnsureVisible(1, FALSE);
    }
    m_listResult.SetRedraw(TRUE);
    m_listResult.Invalidate();
    m_listResult.UpdateWindow();
}
