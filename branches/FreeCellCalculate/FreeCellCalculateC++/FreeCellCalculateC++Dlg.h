
// FreeCellCalculateC++Dlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CFreeCellCalculateCDlg dialog
class CFreeCellCalculateCDlg : public CDialog
{
// Construction
public:
	CFreeCellCalculateCDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FREECELLCALCULATEC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:

	// Generated message map functions
	virtual BOOL OnInitDialog();
    void InitList();
    void UpdateList();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedOk();
    afx_msg void OnBtnCalc();
    
protected:
    DWORD       m_dwGameNumber;
    HICON       m_hIcon;
    CListCtrl   m_listResult;
    
    VecStr      m_vecStr;
};
