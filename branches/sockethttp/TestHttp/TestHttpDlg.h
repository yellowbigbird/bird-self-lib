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
#include "httpSocket.h"

class CTestHttpDlg : public CDialog
{
// Construction
	DECLARE_MESSAGE_MAP()
public:
	CTestHttpDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestHttpDlg)
	enum { IDD = IDD_TESTHTTP_DIALOG };
	
	//}}AFX_DATA
    virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    bool WriteFile(CHttpSocket& rHttpSocket, int fileSize);
	virtual void OnOK();

    CString GetStrRequest() const{ return m_strRequest; } 

    int ThreadFunc();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestHttpDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:    
	CEdit	m_edtEdit1;
    CStatic	m_stcSpeed;
	CTextProgressCtrl	m_ctrlProgress;
	CListBox	m_ctrlList;
	CString	m_strResponse;
	CString	m_strRequest;

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestHttpDlg)
	
	//}}AFX_MSG
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTHTTPDLG_H__F3CC5BD9_DEBB_4A40_94F3_4A0514011545__INCLUDED_)
