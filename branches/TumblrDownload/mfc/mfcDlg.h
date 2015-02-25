
// mfcDlg.h : header file
//

#pragma once


// CmfcDlg dialog
class CmfcDlg : public CDialog
{
// Construction
public:
	CmfcDlg(CWnd* pParent = NULL);	// standard constructor
	DECLARE_MESSAGE_MAP()

// Dialog Data
	enum { IDD = IDD_MFC_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	bool Download(const CString& strFileURLInServer, //待下载文件的URL 
			  const CString & strFileLocalFullPath);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButDownload();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	
protected:
	CString		m_strPath;
	CString		m_strUrl;
};
