// UDPChatDlg.h : 头文件 
// 
 
#pragma once 
 
typedef struct _stChatinfo  
{ 
	CButton*pButtonSet; 
	CButton*pButtonSend; 
	CEdit*pEditLocalPort; 
	CEdit*pEditPeerPort; 
	CEdit*pEditSend; 
	CEdit*pEditRecv; 
	CIPAddressCtrl *IPAddressCtrl; 
}Chatinfo; 
// CUDPChatDlg 对话框 
class CUDPChatDlg : public CDialog 
{ 
// 构造 
public: 
	CUDPChatDlg(CWnd* pParent = NULL);	// 标准构造函数 
 
// 对话框数据 
	enum { IDD = IDD_UDPCHAT_DIALOG }; 
 
	protected: 
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持 
 
 
// 实现 
protected: 
	HICON m_hIcon; 
    static HANDLE pEvent;//必须是静态的 
	Chatinfo m_Chatinfo; 
	static int m_port; 
	static SOCKET hSocket; 
    static UINT SocketThreadChat(LPVOID pParam); 
	// 生成的消息映射函数 
	virtual BOOL OnInitDialog(); 
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam); 
	afx_msg void OnPaint(); 
	afx_msg HCURSOR OnQueryDragIcon(); 
	DECLARE_MESSAGE_MAP() 
public: 
	afx_msg void OnBnClickedButtonSet(); 
public: 
	afx_msg void OnBnClickedButtonSend(); 
}; 
