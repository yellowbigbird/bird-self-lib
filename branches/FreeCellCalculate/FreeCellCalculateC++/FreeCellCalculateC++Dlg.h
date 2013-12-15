
// FreeCellCalculateC++Dlg.h : header file
//

#pragma once


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
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedButton1();
};
