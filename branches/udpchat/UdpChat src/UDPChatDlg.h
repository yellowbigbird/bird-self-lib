// UDPChatDlg.h : ͷ�ļ� 
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
// CUDPChatDlg �Ի��� 
class CUDPChatDlg : public CDialog 
{ 
// ���� 
public: 
	CUDPChatDlg(CWnd* pParent = NULL);	// ��׼���캯�� 
 
// �Ի������� 
	enum { IDD = IDD_UDPCHAT_DIALOG }; 
 
	protected: 
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧�� 
 
 
// ʵ�� 
protected: 
	HICON m_hIcon; 
    static HANDLE pEvent;//�����Ǿ�̬�� 
	Chatinfo m_Chatinfo; 
	static int m_port; 
	static SOCKET hSocket; 
    static UINT SocketThreadChat(LPVOID pParam); 
	// ���ɵ���Ϣӳ�亯�� 
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
