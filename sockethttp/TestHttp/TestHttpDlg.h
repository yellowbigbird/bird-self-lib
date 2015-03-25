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
#include <httpSocket.h>

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
    //virtual void OnOK();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnReq();
	afx_msg void OnBnClickedBtnSend();

    bool WriteFile(CHttpSocket& rHttpSocket, int fileSize);
	
    //CString GetStrUrl() const{ return m_strUrl; } 

    int ThreadFunc();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestHttpDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:    
	CEdit	m_editRequest;
    CStatic	m_stcSpeed;
	CTextProgressCtrl	m_ctrlProgress;
	CListBox	m_ctrlList;
	CString	m_strResponse;
	CString	m_strUrl;
    CString m_strServer;
    CString m_strObject;
    CString m_strRequest;
    WORD    m_port;

protected:
	HICON m_hIcon;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTHTTPDLG_H__F3CC5BD9_DEBB_4A40_94F3_4A0514011545__INCLUDED_)
