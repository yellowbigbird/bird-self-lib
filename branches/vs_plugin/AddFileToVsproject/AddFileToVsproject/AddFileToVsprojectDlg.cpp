
// AddFileToVsprojectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AddFileToVsproject.h"
#include "AddFileToVsprojectDlg.h"

#include "FileManager.h"
#include "FileInfo.h"

#include <util/UtilsFile.h>
#include <util/UtilString.h>

#include <tinyXml/tinyxml.h>

//#include 
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CAddFileToVsprojectDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_EN_CHANGE(IDC_EDIT1, &CAddFileToVsprojectDlg::OnEnChangeEdit1)
    ON_BN_CLICKED(ID_OPEN_PRJ, &CAddFileToVsprojectDlg::OnBtnOpenPrj)
    ON_BN_CLICKED(ID_ADD_PATH, &CAddFileToVsprojectDlg::OnBtnAddPath)
    ON_BN_CLICKED(IDC_BUT_OPEN_PATH, &CAddFileToVsprojectDlg::OnBtnOpenPath)
    ON_BN_CLICKED(IDC_BUT_SEARCH, &CAddFileToVsprojectDlg::OnBtnSearch)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

CAddFileToVsprojectDlg::CAddFileToVsprojectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddFileToVsprojectDlg::IDD, pParent)
    , m_strRootPath(_T("X:\\lan-txt-editor\\explorer++_1.2_src"))
    ,m_strPrjFilePathName(_T("X:\\lan-txt-editor\\testprj\\testprj.vcproj") )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAddFileToVsprojectDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PATH, m_strRootPath);
}


BOOL CAddFileToVsprojectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    CFileManager::GetInstance().StartSearchTread();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAddFileToVsprojectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAddFileToVsprojectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAddFileToVsprojectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//////////////////////////////////////////////////////////////////////////


void CAddFileToVsprojectDlg::OnEnChangeEdit1()
{
    UpdateData(TRUE); //control to value

    //m_strRootPath;
}

void CAddFileToVsprojectDlg::OnBtnOpenPrj()
{
    CFileDialog dlg(TRUE); //OPEN
    if(IDOK !=dlg.DoModal() )
        return;

    CString cstr = dlg.GetPathName();

    wstring wstr = cstr.GetString();
    bool fExist = UtilFile::FileExists(wstr);
    if(!fExist)
        return;

    m_strPrjFilePathName = cstr;
    UpdateData(FALSE); //value to control
}

void CAddFileToVsprojectDlg::OnBtnAddPath()
{
    if(m_strRootPath.GetLength() < 1
        ||  m_strPrjFilePathName.GetLength() < 1)
        return;

    //wait search the path
    while(CFileManager::GetInstance().GetIsSearching() )
    {
        ::Sleep(1000);
    }
    
    ChangeProjectFile();
}

void CAddFileToVsprojectDlg::OnBtnOpenPath()
{
    CFileDialog dlg(TRUE); //OPEN
    if(IDOK !=dlg.DoModal() )
        return;

    CString cstr;
    cstr = dlg.GetPathName();
    
    //check if it's a path
    wstring wstr = cstr.GetString();
    bool fExist = UtilFile::DirectoryExists(wstr);
    if(!fExist)
        return;
    m_strRootPath = cstr;
    UpdateData(FALSE); //value to control
}

void CAddFileToVsprojectDlg::OnBtnSearch()
{
    if(m_strRootPath.GetLength() < 1)
        return ; //false;

    wstring wstr = m_strRootPath.GetString();
    CFileManager::GetInstance().SetPath(wstr);
    CFileManager::GetInstance().StartSearchPath();

    //return true;
}
//////////////////////////////////////////////////////////////////////////
//bool CAddFileToVsprojectDlg::SearchFiles()
//{
//    if(m_strRootPath.GetLength() < 1)
//        return false;
//
//    wstring wstr = m_strRootPath.GetString();
//    CFileManager::GetInstance().SetPath(wstr);
//    CFileManager::GetInstance().StartSearchPath();
//
//    return true;
//}

const char* c_str_files = "Files";
const char* c_str_file = "File";
const char* c_str_filter = "Filter";
const char* c_str_name = "Name";
const char* c_str_relaPath = "RelativePath";


bool CAddFileToVsprojectDlg::ChangeProjectFile()
{
    if(m_strPrjFilePathName.GetLength() < 1)
        return false;

    TiXmlDocument xmldoc;
    string strfile = UtilString::ConvertWideCharToMultiByte(m_strPrjFilePathName.GetString() );
    bool fok = xmldoc.LoadFile(strfile);
    if(!fok){
        ASSERT(false);
        return false;
    }

    TiXmlElement* pRoot = xmldoc.RootElement();
    if(!pRoot)
        return false;

    TiXmlElement* pFiles = pRoot->FirstChildElement(c_str_files);
    if(!pFiles)
        return false;

    //find if the root path exist
    //TiXmlElement* pRootPathFilter = ;
    bool fRootFilterExist = false;
    if(fRootFilterExist){
        //delete
    }

    //create root filter    
    {
        TiXmlElement rootFilterElem(c_str_filter);        

        //recursive create
        CFileInfo rootFileInfo;
        CFileManager::GetInstance().GetRootFile(rootFileInfo);
        CreateXmlElement(&rootFileInfo, &rootFilterElem);

        pFiles->InsertEndChild(rootFilterElem);
    }

    //save
    strfile += ".xml";
    fok = xmldoc.SaveFile(strfile);
    if(!fok)
        return false;
    xmldoc.Clear();
    
    MessageBox(_T("done") );
    return true;
}

bool   CAddFileToVsprojectDlg::CreateXmlElement(const CFileInfo* pFileInfo, TiXmlElement* pXmlElem) const
{
    if(!pFileInfo
        || !pXmlElem)
        return false;
    TiXmlElement& elemFather = *pXmlElem;
    const CFileInfo& infoFather = *pFileInfo;

    string str;
    wstring wstr;

    //dir name ;
    str = UtilString::ConvertWideCharToMultiByte(infoFather.m_strName);
    elemFather.SetAttribute(c_str_name, str);
    
    TiXmlElement elemFile(c_str_file);

    for(list<CFileInfo>::const_iterator it = infoFather.m_files.begin() ;
        it !=  infoFather.m_files.end();
        it++)    {
            const CFileInfo& infoSon = *it;
            if(infoSon.m_fDir){
                 TiXmlElement sonElem(c_str_filter); 
                 CreateXmlElement(&infoSon, &sonElem);

                 elemFather.InsertEndChild(sonElem);
            }
            else{
                str = UtilString::ConvertWideCharToMultiByte(infoSon.m_strFullPathName);
                elemFile.SetAttribute(c_str_relaPath, str);

                elemFather.InsertEndChild(elemFile);
            }
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////


