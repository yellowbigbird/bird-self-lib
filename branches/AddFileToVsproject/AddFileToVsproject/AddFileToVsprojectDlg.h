
// AddFileToVsprojectDlg.h : header file
//

#pragma once


// CAddFileToVsprojectDlg dialog
class CAddFileToVsprojectDlg : public CDialog
{
// Construction
    DECLARE_MESSAGE_MAP()
public:
	CAddFileToVsprojectDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ADDFILETOVSPROJECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

    afx_msg void OnEnChangeEdit1();
    afx_msg void OnBtnOpenPrj();
    afx_msg void OnBtnAddPath();
    afx_msg void OnBtnOpenPath();
	
protected:
    HICON       m_hIcon;
    CString     m_strRootPath;
    CString     m_strPrjFilePathName;
};
