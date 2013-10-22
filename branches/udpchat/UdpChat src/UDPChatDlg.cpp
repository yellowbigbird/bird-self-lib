// UDPChatDlg.cpp : ʵ���ļ�   
//   

#include "stdafx.h"   
#include "UDPChat.h"   
#include "UDPChatDlg.h"   

#ifdef _DEBUG   
#define new DEBUG_NEW   
#endif   


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���   

// CUDPChatDlg �Ի���   



int CUDPChatDlg::m_port=0;   
SOCKET CUDPChatDlg::hSocket=NULL;   
HANDLE CUDPChatDlg::pEvent=NULL;   

CUDPChatDlg::CUDPChatDlg(CWnd* pParent /*=NULL*/)   
: CDialog(CUDPChatDlg::IDD, pParent)   
{   
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);   
}   

void CUDPChatDlg::DoDataExchange(CDataExchange* pDX)   
{   
	CDialog::DoDataExchange(pDX);   
}   

BEGIN_MESSAGE_MAP(CUDPChatDlg, CDialog)   
	ON_WM_SYSCOMMAND()   
	ON_WM_PAINT()   
	ON_WM_QUERYDRAGICON()   
	//}}AFX_MSG_MAP   
	ON_BN_CLICKED(IDC_BUTTON_SET, &CUDPChatDlg::OnBnClickedButtonSet)   
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CUDPChatDlg::OnBnClickedButtonSend)   
END_MESSAGE_MAP()   


// CUDPChatDlg ��Ϣ�������   

BOOL CUDPChatDlg::OnInitDialog()   
{   
	CDialog::OnInitDialog();   

	// ��������...���˵�����ӵ�ϵͳ�˵��С�   

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�   
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);   
	ASSERT(IDM_ABOUTBOX < 0xF000);   

	CMenu* pSysMenu = GetSystemMenu(FALSE);   
	if (pSysMenu != NULL)   
	{   
		CString strAboutMenu;   
		strAboutMenu.LoadString(IDS_ABOUTBOX);   
		if (!strAboutMenu.IsEmpty())   
		{   
			pSysMenu->AppendMenu(MF_SEPARATOR);   
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);   
		}   
	}   

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�   
	//  ִ�д˲���   
	SetIcon(m_hIcon, TRUE);         // ���ô�ͼ��   
	SetIcon(m_hIcon, FALSE);        // ����Сͼ��   

	// TODO: �ڴ���Ӷ���ĳ�ʼ������   
	m_Chatinfo.pEditLocalPort=(CEdit*)GetDlgItem(IDC_EDIT_LOCAL_PORT);   
	m_Chatinfo.pEditPeerPort=(CEdit*)GetDlgItem(IDC_EDIT_PEER_PORT);   
	m_Chatinfo.pEditRecv=(CEdit*)GetDlgItem(IDC_EDIT_REVC);   
	m_Chatinfo.pEditSend=(CEdit*)GetDlgItem(IDC_EDIT_SEND);   
	m_Chatinfo.pButtonSet=(CButton*)GetDlgItem(IDC_BUTTON_SET);   
	m_Chatinfo.pButtonSend=(CButton*)GetDlgItem(IDC_BUTTON_SEND);   
	m_Chatinfo.IPAddressCtrl=(CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS);   
	hSocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);   
	pEvent=CreateEvent(NULL,FALSE,FALSE,NULL);   
	AfxBeginThread(SocketThreadChat,&m_Chatinfo);   
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE   
}   

void CUDPChatDlg::OnSysCommand(UINT nID, LPARAM lParam)   
{   
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)   
	{   
		CAboutDlg dlgAbout;   
		dlgAbout.DoModal();   
	}   
	else   
	{   
		CDialog::OnSysCommand(nID, lParam);   
	}   
}   

// �����Ի��������С����ť������Ҫ����Ĵ���   
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���   
//  �⽫�ɿ���Զ���ɡ�   

void CUDPChatDlg::OnPaint()   
{   
	if (IsIconic())   
	{   
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������   

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);   

		// ʹͼ���ڹ��������о���   
		int cxIcon = GetSystemMetrics(SM_CXICON);   
		int cyIcon = GetSystemMetrics(SM_CYICON);   
		CRect rect;   
		GetClientRect(&rect);   
		int x = (rect.Width() - cxIcon + 1) / 2;   
		int y = (rect.Height() - cyIcon + 1) / 2;   

		// ����ͼ��   
		dc.DrawIcon(x, y, m_hIcon);   
	}   
	else   
	{   
		CDialog::OnPaint();   
	}   
}   

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��   
//   
HCURSOR CUDPChatDlg::OnQueryDragIcon()   
{   
	return static_cast<HCURSOR>(m_hIcon);   
}   
UINT CUDPChatDlg::SocketThreadChat(LPVOID pParam)   
{   
	Chatinfo*info=(Chatinfo*)pParam;   
	CEdit*pEdit=info->pEditRecv;   
	WaitForSingleObject(pEvent,INFINITE);   
	if (hSocket==INVALID_SOCKET)   
	{   
		return -1;   
	}   
	sockaddr_in addr;   
	addr.sin_family=AF_INET;   
	addr.sin_port = htons(m_port);   
	addr.sin_addr.S_un.S_addr=ADDR_ANY;   
	//��   
	if (bind(hSocket,(sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR)   
	{   
		return -2;   
	}   
	//3\��������   
	for (;;)   
	{   
		TCHAR sBuf[255]={0};   
		sockaddr_in peeraddr;   
		int nLen =sizeof(sockaddr_in);   
		int nRecvLen=recvfrom(hSocket,(char*)sBuf,255,0,(sockaddr*)&peeraddr,&nLen);   
		_tcscat(sBuf,_T("\r\n"));   
		pEdit->ReplaceSel(sBuf);//׷�������һλ����   
	}   
	closesocket(hSocket);   
	return 0;   
}   

void CUDPChatDlg::OnBnClickedButtonSet()   
{   
	m_port =GetDlgItemInt(IDC_EDIT_LOCAL_PORT);   
	SetEvent(pEvent);   
	// TODO: �ڴ���ӿؼ�֪ͨ����������   
}   

void CUDPChatDlg::OnBnClickedButtonSend()   
{   
	int portsend=GetDlgItemInt(IDC_EDIT_PEER_PORT);   
	if (hSocket==INVALID_SOCKET)   
	{   
		return ;   
	}   
	sockaddr_in addr;   
	addr.sin_family=AF_INET;   
	addr.sin_port = htons(portsend);   
	DWORD address ;   
	m_Chatinfo.IPAddressCtrl->GetAddress(address);   
	addr.sin_addr.S_un.S_addr=htonl(address);   
	TCHAR sBuf[255]={0};   
	GetDlgItem(IDC_EDIT_SEND)->GetWindowText(sBuf,254);   
	int nLen =sizeof(sockaddr_in);   
	int nRecvLen=sendto(hSocket,(char*)sBuf,_tcslen(sBuf)*sizeof(TCHAR),0,(sockaddr*)&addr,nLen);   
	if (nRecvLen==SOCKET_ERROR)   
	{   
		return;   
	}   

	// TODO: �ڴ���ӿؼ�֪ͨ����������   
}   