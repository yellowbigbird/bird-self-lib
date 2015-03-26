
// LanTxtEditorDlg.h : header file
//

#pragma once


// CLanTxtEditorDlg dialog
class CLanTxtEditorDlg : public CDialog
{
	DECLARE_MESSAGE_MAP()
// Construction
public:
	CLanTxtEditorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_LANTXTEDITOR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//afx_msg void OnButStartServer();
    afx_msg void OnButSend();
    afx_msg void OnBtnGetFile();
    afx_msg void OnBtnRestart();

    afx_msg LRESULT OnUpdateDebug(WPARAM wParam, LPARAM lParam); 
    afx_msg LRESULT OnClose(WPARAM wParam, LPARAM lParam);

    virtual void OnCancel();
    virtual void OnOK();

    void    InitList();
    void    UpdateList();

	//void	InitServer();
    
	//void	SendGetFileReg();
	//void	SendGetFileListReq();

// Implementation
protected:
	HICON m_hIcon;
	
	CString		m_strSendIp;
	UINT		m_portSend;
	UINT		m_portListen;
    CListCtrl   m_listLog;
    CString     m_strFile;
public:
};
