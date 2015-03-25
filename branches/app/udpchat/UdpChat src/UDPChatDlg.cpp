// UDPChatDlg.cpp : 实现文件   
//   

#include "stdafx.h"   
#include "UDPChat.h"   
#include "UDPChatDlg.h"   

#ifdef _DEBUG   
#define new DEBUG_NEW   
#endif   


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框   

// CUDPChatDlg 对话框   



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


// CUDPChatDlg 消息处理程序   

BOOL CUDPChatDlg::OnInitDialog()   
{   
	CDialog::OnInitDialog();   

	// 将“关于...”菜单项添加到系统菜单中。   

	// IDM_ABOUTBOX 必须在系统命令范围内。   
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动   
	//  执行此操作   
	SetIcon(m_hIcon, TRUE);         // 设置大图标   
	SetIcon(m_hIcon, FALSE);        // 设置小图标   

	// TODO: 在此添加额外的初始化代码   
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
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE   
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

// 如果向对话框添加最小化按钮，则需要下面的代码   
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，   
//  这将由框架自动完成。   

void CUDPChatDlg::OnPaint()   
{   
	if (IsIconic())   
	{   
		CPaintDC dc(this); // 用于绘制的设备上下文   

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);   

		// 使图标在工作矩形中居中   
		int cxIcon = GetSystemMetrics(SM_CXICON);   
		int cyIcon = GetSystemMetrics(SM_CYICON);   
		CRect rect;   
		GetClientRect(&rect);   
		int x = (rect.Width() - cxIcon + 1) / 2;   
		int y = (rect.Height() - cyIcon + 1) / 2;   

		// 绘制图标   
		dc.DrawIcon(x, y, m_hIcon);   
	}   
	else   
	{   
		CDialog::OnPaint();   
	}   
}   

//当用户拖动最小化窗口时系统调用此函数取得光标显示。   
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
	//绑定   
	if (bind(hSocket,(sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR)   
	{   
		return -2;   
	}   
	//3\接收数据   
	for (;;)   
	{   
		TCHAR sBuf[255]={0};   
		sockaddr_in peeraddr;   
		int nLen =sizeof(sockaddr_in);   
		int nRecvLen=recvfrom(hSocket,(char*)sBuf,255,0,(sockaddr*)&peeraddr,&nLen);   
		_tcscat(sBuf,_T("\r\n"));   
		pEdit->ReplaceSel(sBuf);//追加在最后一位后面   
	}   
	closesocket(hSocket);   
	return 0;   
}   

void CUDPChatDlg::OnBnClickedButtonSet()   
{   
	m_port =GetDlgItemInt(IDC_EDIT_LOCAL_PORT);   
	SetEvent(pEvent);   
	// TODO: 在此添加控件通知处理程序代码   
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

	// TODO: 在此添加控件通知处理程序代码   
}   