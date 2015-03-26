
// LanServerDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CLanServerDlg dialog
class CLanServerDlg : public CDialog
{
// Construction
public:
	CLanServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LANSERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	DECLARE_MESSAGE_MAP()

    void InitList();
    void UpdateList();

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LRESULT OnUpdateDebug(WPARAM wParam, LPARAM lParam); 

	afx_msg void OnBnClickedButTest();
    afx_msg void OnBnClickedButUpdatePath();
    afx_msg void OnBnClickedButSend();

// Implementation
protected:
	HICON m_hIcon;

	CString m_strCurIp;
	CString m_strRootPath;
    CListCtrl m_listLog;
};
