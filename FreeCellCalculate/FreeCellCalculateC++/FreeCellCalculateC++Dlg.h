
// FreeCellCalculateC++Dlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CCalculate;
class CThreadPool;

// CFreeCellCalculateCDlg dialog
class CFreeCellCalculateCDlg : public CDialog
{
// Construction
public:
	CFreeCellCalculateCDlg(CWnd* pParent = NULL);	// standard constructor
    virtual ~CFreeCellCalculateCDlg();

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
    void OnStop();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedOk();
    afx_msg void OnBtnCalc();
    afx_msg void OnBtnStop();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    
protected:
    DWORD       m_dwGameNumber;
    UINT        m_tickStart;
    HICON       m_hIcon;
    CListCtrl   m_listResult;
    
    VecStr      m_vecStr;

    double      m_timePassed;  //in sec
    //CCalculate* m_pCalc;
    CThreadPool*    m_pool;
    CButton     m_btnCalc;

};
