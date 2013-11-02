
// DcTestDlg.h : header file
//

#pragma once

//#include "dcdata.h"
#include "afxwin.h"
#include "afxcmn.h"

class CDcData;

// CDcTestDlg dialog
class CDcTestDlg : public CDialog
{
	DECLARE_MESSAGE_MAP()
// Construction
public:
	CDcTestDlg(CWnd* pParent = NULL);	// standard constructor
    virtual ~CDcTestDlg();

// Dialog Data
	enum { IDD = IDD_DCTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();


	afx_msg void OnBnClickedButGo();
	afx_msg void OnBnClickedButOpen();
	afx_msg void OnBnClickedButReload();

protected:	
	HICON		m_hIcon;
	CDcData*	m_pData;
	CString		m_strFileRequest;
	//CString		m_strRequestFile;
	CComboBox	m_comboUrl;
	CString		m_strUrl;

	CRichEditCtrl	m_richEditRequst;
	CRichEditCtrl	m_richEditResponse;
public:
    afx_msg void OnEnChangeEditUrl();
};
