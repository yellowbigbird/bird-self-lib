
// StockMonitorDlg.h : header file
//

#pragma once
#include "afxvslistbox.h"


// CStockMonitorDlg dialog
class CStockMonitorDlg : public CDialogEx
{
	DECLARE_MESSAGE_MAP()
// Construction
public:
	CStockMonitorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_STOCKMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
// Implementation
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	void	InitList();
	void	UpdateList();
	

protected:
	CVSListBox m_listBox;
	HICON m_hIcon;
};
