// TestHttpDlg.h : header file
//

#if !defined(AFX_TESTHTTPDLG_H__F3CC5BD9_DEBB_4A40_94F3_4A0514011545__INCLUDED_)
#define AFX_TESTHTTPDLG_H__F3CC5BD9_DEBB_4A40_94F3_4A0514011545__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestHttpDlg dialog
#include "TextProgressCtrl.h"
class CTestHttpDlg : public CDialog
{
// Construction
public:
	CTestHttpDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestHttpDlg)
	enum { IDD = IDD_TESTHTTP_DIALOG };
	CStatic	m_stcSpeed;
	CEdit	m_edtEdit1;
	CTextProgressCtrl	m_ctrlProgress;
	CListBox	m_ctrlList;
	CString	m_strResponse;
	CString	m_strRequest;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestHttpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestHttpDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTHTTPDLG_H__F3CC5BD9_DEBB_4A40_94F3_4A0514011545__INCLUDED_)
