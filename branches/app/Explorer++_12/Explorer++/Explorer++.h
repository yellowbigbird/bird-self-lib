#ifndef EXPLORER_INCLUDED
#define EXPLORER_INCLUDED

#include <windows.h>
#include <commctrl.h>
#include "../ShellBrowser/iShellView.h"
#include "Explorer++_internal.h"
#import <msxml3.dll> raw_interfaces_only

void InsertFolderItemsIntoComboBoxInternal(HWND hCreateIn,Bookmark_t *pBookmark);

#define MAX_TABS					100
#define MENU_BOOKMARK_STARTPOS		3
#define MENU_BOOKMARK_STARTID		10000
#define MENU_BOOKMARK_ENDID			11000
#define MENU_HEADER_STARTID			12000
#define MENU_HEADER_ENDID			13000

#define TOOLBAR_START				5000
#define TABTOOLBAR_CLOSE			(TOOLBAR_START + 33)

#define FOLDERS_TOOLBAR_CLOSE		6000

/* wParam contains virtual code of key that was pressed,
lParam not currently used. */
#define CBN_KEYDOWN					WM_APP + 2

/* Sent when a folder size calculation has finished. */
#define WM_APP_FOLDERSIZECOMPLETED	WM_APP + 3

/* Private definitions. */
#define FROM_LISTVIEW				0
#define FROM_TREEVIEW				1
#define FROM_SEARCH					2
#define FROM_DRIVEBAR				3

#define MAX_BOOKMARKTOOLBAR_ITEMS	100

/* Registry keys used to store prgram settings. */
#define REG_MAIN_KEY				_T("Software\\Explorer++")
#define REG_SETTINGS_KEY			_T("Software\\Explorer++\\Settings")
#define REG_WINDOWPOS_KEY			_T("Software\\Explorer++\\Position")
#define REG_BOOKMARKS_KEY			_T("Software\\Explorer++\\Bookmarks")
#define REG_FILTERS_KEY				_T("Software\\Explorer++\\Filters")
#define REG_TABS_KEY				_T("Software\\Explorer++\\Tabs")
#define REG_TOOLBARS_KEY			_T("Software\\Explorer++\\Toolbars")
#define REG_COLUMNS_KEY				_T("Software\\Explorer++\\DefaultColumns")
#define REG_APPLICATIONS_KEY		_T("Software\\Explorer++\\ApplicationToolbar")
#define REG_STATE_KEY				_T("Software\\Explorer++\\State")
#define REG_COLORS_KEY				_T("Software\\Explorer++\\ColorRules")

/* State keys (relative to REG_STATE_KEY). */
#define REG_ADDBOOKMARK_KEY			_T("AddBookmark")
#define REG_COLORRULES_KEY			_T("ColorRules")
#define REG_CUSTOMIZECOLORS_KEY		_T("CustomizeColors")
#define REG_DESTROYFILES_KEY		_T("DestroyFiles")
#define REG_DISPLAYCOLORS_KEY		_T("DisplayColors")
#define REG_FILTER_KEY				_T("Filter")
#define REG_MASSRENAME_KEY			_T("MassRename")
#define REG_MERGEFILES_KEY			_T("MergeFiles")
#define REG_ORGANIZEBOOKMARKS_KEY	_T("OrganizeBookmarks")
#define REG_DRIVEPROPERTIES_KEY		_T("DriveProperties")
#define REG_SEARCH_KEY				_T("Search")
#define REG_SELECTCOLUMNS_KEY		_T("SelectColumns")
#define REG_SELECTDEFAULTCOLUMNS_KEY	_T("SelectDefaultColumns")
#define REG_SETFILEATTRIBUTES_KEY	_T("SetFileAttributes")
#define REG_SPLITFILE_KEY			_T("SplitFile")
#define REG_WILDCARD_KEY			_T("WildcardSelect")

#define TAB_WINDOW_HEIGHT			24
#define DEFAULT_TREEVIEW_WIDTH		208

typedef struct
{
	TCHAR szFileName[MAX_PATH];
} CutFile_t;

typedef struct
{
	UINT SortById;
	UINT GroupById;
} ArrangeMenuItem_t;

typedef struct
{
	TCHAR *pszFilterString;
} Filter_t;

typedef struct
{
	TCHAR szText[512];
} StatusBarPart_t;

/* Used to hold information on all tabs. */
typedef struct
{
	BOOL	bLocked;
	BOOL	bAddressLocked;
	BOOL	bUseCustomName;
	TCHAR	szName[MAX_PATH];

	/* Although each tab manages its
	own columns, it does not know
	about any column defaults.
	Therefore, it makes more sense
	for this setting to remain here. */
	//BOOL	bUsingDefaultColumns;
} TabInfo_t;

typedef struct
{
	int iItemID;
} ToolbarButton_t;

typedef struct
{
	/* Description of this rule. */
	TCHAR		szDescription[512];

	/* Filename filtering. */
	TCHAR		szFilterPattern[512];

	/* Attribute filtering. */
	DWORD		dwFilterAttributes;

	/* Text color. */
	COLORREF	rgbColour;
} ListViewColouring_t;

typedef struct
{
	TCHAR szFullFileName[MAX_PATH];
	WIN32_FIND_DATA wfd;
} SetFileAttributesInfo_t;

/* Describes the view modes and their order
(as they differ on Windows XP and Vista/7). */
typedef struct
{
	UINT uViewMode;
} ViewMode_t;

typedef struct
{
	TCHAR szType[32];
} DWFileType_t;

typedef struct
{
	TCHAR szCommand[256];
	TCHAR szDescription[256];
} DWCommand_t;

typedef struct
{
	TCHAR szDescription[256];
	TCHAR szText[256];
} DWLine_t;

typedef struct
{
	TCHAR szDescription[256];
	
	list<DWFileType_t> FileTypes;
	list<DWLine_t> Lines;
} DWRule_t;

typedef struct
{
	ATOM	atomClass;
	HWND	hProxy;
	int		iTabId;
} TabProxyInfo_t;

typedef struct
{
	int		iTabId;
	HBITMAP	hbm;
	POINT	ptOrigin;
} TabPreviewInfo_t;

typedef enum
{
	STARTUP_PREVIOUSTABS	= 1,
	STARTUP_DEFAULTFOLDER	= 2
} STARTUP_MODES;

typedef enum
{
	INFOTIP_SYSTEM	= 0,
	INFOTIP_CUSTOM	= 1
} INFOTIP_TYPE;

static const int ToolbarButtons[] =
{TOOLBAR_BACK,TOOLBAR_FORWARD,TOOLBAR_UP,
TOOLBAR_FOLDERS,TOOLBAR_COPYTO,TOOLBAR_MOVETO,
TOOLBAR_NEWFOLDER,TOOLBAR_COPY,TOOLBAR_CUT,
TOOLBAR_PASTE,TOOLBAR_DELETE,TOOLBAR_VIEWS,
TOOLBAR_SEARCH,TOOLBAR_PROPERTIES,TOOLBAR_REFRESH};

class CContainer : public IDropTarget, public IServiceProvider, public IShellView2, public INewMenuClient, public IDropFilesCallback
{
public:

	CContainer(HWND);
	~CContainer();

	/* Window/dialog procedures.
	Window procedures use LRESULT
	Dialog procedures use INT_PTR */
	LRESULT CALLBACK	WindowProcedure(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	ListViewSubclassProc(HWND ListView,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	ListViewEditProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	EditSubclass(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	RebarSubclass(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	BookmarksToolbarSubclass(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	DrivesToolbarSubclass(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	TabBackingProc(HWND hTabCtrl,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	TreeViewHolderProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	ShellMenuHookProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	TabSubclassProc(HWND hTab,UINT msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	TreeViewSubclass(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	StaticColorProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK	TabProxyWndProc(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam,int iTabId);
	INT_PTR CALLBACK	GeneralSettingsProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	FilesFoldersProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	WindowProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	DefaultSettingsProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	TabSettingsProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	BookmarkTabDlgProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	NewBookmarkFolderProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	OrganizeBookmarks(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	BookmarkPropertiesProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	BookmarkFolderPropertiesProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	MassRenameProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	RenameTabProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	ChangeDisplayColours(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	SetFileAttributesProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	WildcardSelectProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	FilterProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	SplitFileProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	MergeFilesProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	DestroyFilesProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	SelectColumnsProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	SetDefaultColumnsProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	ApplicationButtonPropertiesProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	ApplicationToolbarNewButtonProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	ColorFilteringProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	ColorRuleProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	SearchProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	DWChangeDetailsProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam);
	INT_PTR CALLBACK	DWLinePropertiesProc(HWND hDlg,UINT Msg,WPARAM wParam,LPARAM lParam);

	void				FolderSizeCallback(FolderSizeExtraInfo_t *pfsei,int nFolders,int nFiles,PULARGE_INTEGER lTotalFolderSize);

	/* Directory modification. */
	static void			DirectoryAlteredCallback(TCHAR *szFileName,DWORD dwAction,void *pData);

	/* IUnknown methods. */
	HRESULT __stdcall	QueryInterface(REFIID iid,void **ppvObject);
	ULONG __stdcall		AddRef(void);
	ULONG __stdcall		Release(void);

	/* Drop target handler. */
	HRESULT _stdcall	DragEnter(IDataObject *pDataObject,DWORD grfKeyState,POINTL pt,DWORD *pdwEffect);
	HRESULT _stdcall	DragOver(DWORD grfKeyState,POINTL pt,DWORD *pdwEffect);
	HRESULT _stdcall	DragLeave(void);
	HRESULT _stdcall	Drop(IDataObject *pDataObject,DWORD grfKeyState,POINTL pt,DWORD *pdwEffect);

	/* IServiceProvider methods. */
	HRESULT	_stdcall	QueryService(REFGUID guidService,REFIID riid,void **ppv);

	/* IShellView2 methods. */
	HRESULT _stdcall	CreateViewWindow2(LPSV2CVW2_PARAMS lpParams);
	HRESULT _stdcall	GetView(SHELLVIEWID *pvid,ULONG uView);
	HRESULT _stdcall	HandleRename(LPCITEMIDLIST pidlNew);
	HRESULT _stdcall	SelectAndPositionItem(LPCITEMIDLIST pidlItem,UINT uFlags,POINT *ppt);

	HRESULT _stdcall	GetWindow(HWND *);
	HRESULT _stdcall	ContextSensitiveHelp(BOOL bHelp);
	HRESULT _stdcall	TranslateAccelerator(MSG *msg);
	HRESULT _stdcall	EnableModeless(BOOL fEnable);
	HRESULT _stdcall	UIActivate(UINT uActivate);
	HRESULT _stdcall	Refresh(void);
	HRESULT _stdcall	CreateViewWindow(IShellView *psvPrevious,LPCFOLDERSETTINGS pfs,IShellBrowser *psb,RECT *prcView,HWND *phWnd);
	HRESULT _stdcall	DestroyViewWindow(void);
	HRESULT _stdcall	GetCurrentInfo(LPFOLDERSETTINGS pfs);
	HRESULT _stdcall	AddPropertySheetPages(DWORD dwReserved,LPFNSVADDPROPSHEETPAGE pfn,LPARAM lparam);
	HRESULT _stdcall	SaveViewState(void);
	HRESULT _stdcall	SelectItem(LPCITEMIDLIST pidlItem,SVSIF uFlags);
	HRESULT _stdcall	GetItemObject(UINT uItem,REFIID riid,void **ppv);

	/* INewMenuClient - Windows Vista only. */
	HRESULT _stdcall	SelectAndEditItem(PCIDLIST_ABSOLUTE pidlItem,NMCSAEI_FLAGS flags);
	HRESULT _stdcall	IncludeItems(NMCII_FLAGS *pFlags);

	void				OnDropFile(list<PastedFile_t> *ppfl,POINT *ppt);

	void				PasteFilesCallbackInternal(list<PastedFile_t> *pPastedFileList);


private:

	class CLoadSaveRegistry : public ILoadSave
	{
	public:

		CLoadSaveRegistry(CContainer *pContainer);
		~CLoadSaveRegistry();

		/* IUnknown methods. */
		HRESULT __stdcall	QueryInterface(REFIID iid,void **ppvObject);
		ULONG __stdcall		AddRef(void);
		ULONG __stdcall		Release(void);

		/* Loading functions. */
		void	LoadGenericSettings(void);
		LONG	LoadWindowPosition(InitialWindowPos_t *piwp);
		void	LoadFilters(void);
		void	LoadBookmarks(void);
		int		LoadPreviousTabs(void);
		void	LoadDefaultColumns(void);
		void	LoadApplicationToolbar(void);
		void	LoadToolbarInformation(void);
		void	LoadColorRules(void);
		void	LoadState(void);

		/* Saving functions. */
		void	SaveGenericSettings(void);
		LONG	SaveWindowPosition(void);
		void	SaveFilters(void);
		void	SaveBookmarks(void);
		void	SaveTabs(void);
		void	SaveDefaultColumns(void);
		void	SaveApplicationToolbar(void);
		void	SaveToolbarInformation(void);
		void	SaveColorRules(void);
		void	SaveState(void);

	private:

		int	m_iRefCount;

		CContainer *m_pContainer;
	};

	class CLoadSaveXML : public ILoadSave
	{
	public:

		CLoadSaveXML(CContainer *pContainer,BOOL bLoad);
		~CLoadSaveXML();

		/* IUnknown methods. */
		HRESULT __stdcall	QueryInterface(REFIID iid,void **ppvObject);
		ULONG __stdcall		AddRef(void);
		ULONG __stdcall		Release(void);

		void	InitializeLoadEnvironment(void);
		void	ReleaseLoadEnvironment(void);
		void	InitializeSaveEnvironment(void);
		void	ReleaseSaveEnvironment(void);

		/* Loading functions. */
		void	LoadGenericSettings(void);
		LONG	LoadWindowPosition(InitialWindowPos_t *piwp);
		void	LoadFilters(void);
		void	LoadBookmarks(void);
		int		LoadPreviousTabs(void);
		void	LoadDefaultColumns(void);
		void	LoadApplicationToolbar(void);
		void	LoadToolbarInformation(void);
		void	LoadColorRules(void);
		void	LoadState(void);

		/* Saving functions. */
		void	SaveGenericSettings(void);
		LONG	SaveWindowPosition(void);
		void	SaveFilters(void);
		void	SaveBookmarks(void);
		void	SaveTabs(void);
		void	SaveDefaultColumns(void);
		void	SaveApplicationToolbar(void);
		void	SaveToolbarInformation(void);
		void	SaveColorRules(void);
		void	SaveState(void);

	private:

		int	m_iRefCount;

		CContainer				*m_pContainer;
		BOOL					m_bLoad;

		/* These are used for saving + loading. */
		MSXML2::IXMLDOMDocument	*m_pXMLDom;

		/* Used exclusively for loading. */
		BOOL					m_bLoadedCorrectly;

		/* Used exclusively for saving. */
		MSXML2::IXMLDOMElement	*m_pRoot;
	};

	friend CLoadSaveXML;

	class CBookmarkToolbarDrop : public IDropTarget
	{
	public:
		CBookmarkToolbarDrop(CContainer *pContainer);
		~CBookmarkToolbarDrop();

		/* IUnknown methods. */
		HRESULT __stdcall	QueryInterface(REFIID iid,void **ppvObject);
		ULONG __stdcall		AddRef(void);
		ULONG __stdcall		Release(void);

		/* Drag and drop. */
		HRESULT _stdcall	DragEnter(IDataObject *pDataObject,DWORD grfKeyStat,POINTL pt,DWORD *pdwEffect);
		HRESULT _stdcall	DragOver(DWORD grfKeyState,POINTL pt,DWORD *pdwEffect);
		HRESULT _stdcall	DragLeave(void);
		HRESULT _stdcall	Drop(IDataObject *pDataObject,DWORD grfKeyState,POINTL pt,DWORD *pdwEffect);

	private:
		int m_iRefCount;

		CContainer *m_pContainer;

		/* Drag and drop. */
		IDragSourceHelper *	m_pDragSourceHelper;
		IDropTargetHelper *	m_pDropTargetHelper;
		BOOL m_bAcceptData;

		HRESULT	InitializeDragDropHelpers(void);
	};

	friend CBookmarkToolbarDrop;

	class CApplicationToolbarDrop : public IDropTarget
	{
	public:
		CApplicationToolbarDrop(CContainer *pContainer);
		~CApplicationToolbarDrop();

		/* IUnknown methods. */
		HRESULT __stdcall	QueryInterface(REFIID iid,void **ppvObject);
		ULONG __stdcall		AddRef(void);
		ULONG __stdcall		Release(void);

		/* Drag and drop. */
		HRESULT _stdcall	DragEnter(IDataObject *pDataObject,DWORD grfKeyStat,POINTL pt,DWORD *pdwEffect);
		HRESULT _stdcall	DragOver(DWORD grfKeyState,POINTL pt,DWORD *pdwEffect);
		HRESULT _stdcall	DragLeave(void);
		HRESULT _stdcall	Drop(IDataObject *pDataObject,DWORD grfKeyState,POINTL ptl,DWORD *pdwEffect);

	private:
		int m_iRefCount;
		CContainer *m_pContainer;

		/* Drag and drop. */
		IDragSourceHelper *	m_pDragSourceHelper;
		IDropTargetHelper *	m_pDropTargetHelper;
		BOOL m_bAcceptData;

		HRESULT	InitializeDragDropHelpers(void);
	};

	friend CApplicationToolbarDrop;

	int						m_iRefCount;

	/* Internal private functions. */
	void					OnTabChangeInternal(BOOL bSetFocus);
	void					UpdateArrangeMenuItems(void);
	void					VerifyAndSetWindowPosition(InitialWindowPos_t *piwp);
	void					SetDefaultWindowPosition(void);

	/* <----Private message handlers.----> */

	/* Main window private message handlers. */
	LRESULT CALLBACK		CommandHandler(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);
	LRESULT CALLBACK		NotifyHandler(HWND hwnd,UINT Msg,WPARAM wParam,LPARAM lParam);
	void					OnMenuCommand(WPARAM wParam,LPARAM lParam);
	BOOL					OnSize(int MainWindowWidth,int MainWindowHeight);
	int						OnClose(void);
	int						OnDestroy(void);
	void					OnSaveDirectoryListing(void);
	void					OnCopyItemPath(void);
	void					OnCopyUniversalPaths(void);
	void					OnCopy(BOOL bCopy);
	void					OnRightClick(NMHDR *nmhdr);
	void					OnTabSelectionChange(void);
	void					OnSetFileAttributes(void);
	void					OnShowFileProperties(void);
	void					OnWindowCreate(void);
	void					OnComboBoxKeyDown(WPARAM wParam);
	void					OnNewTab(void);
	void					OnFileRename(void);
	void					OnFileDelete(BOOL bPermanent);
	void					OnWildcardSelect(BOOL bSelect);
	void					OnSaveFileSlack(void);
	void					OnDirChanged(int iTabId);
	void					OnResolveLink(void);
	void					OnTabCtrlGetDispInfo(LPARAM lParam);
	void					OnDrawClipboard(void);
	void					OnChangeCBChain(WPARAM wParam,LPARAM lParam);
	void					OnSetFocus(void);
	void					ItemChangedHandler(TCHAR *ItemName,BOOL Selected);
	int						SelectAllFolders(HWND ListView);
	int						HighlightSimilarFiles(HWND ListView);
	void					ShowHiddenFiles(void);
	HRESULT					OnDeviceChange(WPARAM wParam,LPARAM lParam);
	LRESULT					StatusBarMenuSelect(WPARAM wParam,LPARAM lParam);
	void					HandleDirectoryMonitoring(int iTabId);
	void					OnTbnDropDown(LPARAM lParam);
	void					OnTabMClick(WPARAM wParam,LPARAM lParam);
	void					OnDisplayWindowResized(WPARAM wParam);
	void					OnStartedBrowsing(int iTabId,TCHAR *szPath);
	void					OnDuplicateTab(int iTab);
	void					OnLockTab(int iTab);
	void					OnLockTabInternal(int iTab,int iTabId);
	void					OnLockTabAndAddress(int iTab);
	void					HandleTabToolbarItemStates(void);
	void					OnAutoSizeColumns(void);
	BOOL					OnMeasureItem(WPARAM wParam,LPARAM lParam);
	BOOL					OnDrawItem(WPARAM wParam,LPARAM lParam);
	void					OnToolbarViews(void);
	void					ShowToolbarViewsDropdown(void);
	void					OnMainToolbarRClick(void);
	void					OnApplicationToolbarRClick(void);
	void					OnBookmarksToolbarRClick(int iItem);
	void					OnAddressBarGo(void);
	void					OnSortByAscending(BOOL bSortAscending);
	void					OnPreviousWindow(void);
	void					OnNextWindow(void);
	HRESULT					BrowseFolder(TCHAR *szPath,UINT wFlags);
	HRESULT					BrowseFolder(TCHAR *szPath,UINT wFlags,BOOL bOpenInNewTab,BOOL bSwitchToNewTab,BOOL bOpenInNewWindow);
	HRESULT					BrowseFolder(LPITEMIDLIST pidlDirectory,UINT wFlags);
	HRESULT					BrowseFolder(LPITEMIDLIST pidlDirectory,UINT wFlags,BOOL bOpenInNewTab,BOOL bSwitchToNewTab,BOOL bOpenInNewWindow);
	int						DetermineListViewObjectIndex(HWND hListView);
	void					OnUndo(void);
	void					OnLockToolbars(void);
	void					LoadAllSettings(ILoadSave **pLoadSave);
	void					OnShellNewItemCreated(LPARAM lParam);
	void					OnCreateNewFolder(void);
	void					OnPaste(void);
	void					OnAppCommand(WPARAM wParam,LPARAM lParam);
	void					OnBrowseBack(void);
	void					OnBrowseForward(void);
	void					OnRefresh(void);
	void					OnDirectoryModified(int iTabId);
	void					OnIdaRClick(void);
	void					OnAssocChanged(void);
	void					OnNdwRClick(WPARAM wParam,LPARAM lParam);
	void					OnNdwIconRClick(WPARAM wParam,LPARAM lParam);
	LRESULT					OnCustomDraw(LPARAM lParam);
	void					OnSortBy(UINT uSortMode);
	void					OnGroupBy(UINT uSortMode);
	void					OnHome(void);
	void					OnNavigateUp(void);
	void					OnSelectTab(int iTab);
	void					OnSelectTab(int iTab,BOOL bSetFocus);

	/* ListView private message handlers. */
	void					OnListViewMButtonDown(WPARAM wParam,LPARAM lParam);
	void					OnListViewMButtonUp(WPARAM wParam,LPARAM lParam);
	LRESULT					OnListViewLButtonDown(WPARAM wParam,LPARAM lParam);
	void					OnListViewDoubleClick(NMHDR *nmhdr);
	void					OnListViewFileRename(void);
	void					OnListViewColumnClick(LPARAM lParam);
	void					OnListViewGetInfoTip(LPARAM lParam);
	LRESULT					OnListViewKeyDown(LPARAM lParam);
	void					OnListViewItemChanged(LPARAM lParam);
	HRESULT					OnListViewBeginDrag(LPARAM lParam,DragTypes_t DragType);
	BOOL					OnListViewBeginLabelEdit(LPARAM lParam);
	BOOL					OnListViewEndLabelEdit(LPARAM lParam);
	void					OnListViewGetDisplayInfo(LPARAM lParam);
	HRESULT					OnListViewFileDelete(BOOL bPermanent);
	void					OnListViewRClick(HWND hParent,POINT *pCursorPos);
	void					OnListViewHeaderRClick(POINT *pCursorPos);
	int						GetColumnHeaderMenuList(unsigned int **pHeaderList);
	void					OnListViewShowFileProperties(void);
	void					OnListViewCopyItemPath(void);
	void					OnListViewCopyUniversalPaths(void);
	void					OnListViewSetFileAttributes(void);
	void					OnListViewPaste(void);

	/* TreeView private message handlers. */
	void					OnTreeViewFileRename(void);
	void					OnTreeViewFileDelete(BOOL bPermanent);
	void					OnTreeViewFileDeletePermanent(void);
	void					OnTreeViewRightClick(WPARAM wParam,LPARAM lParam);
	void					OnTreeViewShowFileProperties(void);
	BOOL					OnTreeViewItemExpanding(LPARAM lParam);
	void					OnTreeViewSelChanged(LPARAM lParam);
	int						OnTreeViewBeginLabelEdit(LPARAM lParam);
	int						OnTreeViewEndLabelEdit(LPARAM lParam);
	LRESULT					OnTreeViewKeyDown(LPARAM lParam);
	void					OnTreeViewCopyItemPath(void);
	void					OnTreeViewCopy(BOOL bCopy);
	void					OnTreeViewSetFileAttributes(void);
	void					OnTreeViewCopyUniversalPaths(void);
	void					OnTreeViewPaste(void);

	/* Main toolbar private message handlers. */
	BOOL					OnTBQueryInsert(LPARAM lParam);
	BOOL					OnTBQueryDelete(LPARAM lParam);
	BOOL					OnTBGetButtonInfo(LPARAM lParam);
	BOOL					OnTBRestore(LPARAM lParam);
	void					OnTBSave(LPARAM lParam);
	void					OnTBReset(void);
	void					OnTBGetInfoTip(LPARAM lParam);

	/* Tab control private message handlers. */
	void					OnInitTabMenu(WPARAM wParam);
	void					OnTabCtrlLButtonDown(WPARAM wParam,LPARAM lParam);
	void					OnTabCtrlLButtonUp(void);
	void					OnTabCtrlMouseMove(WPARAM wParam,LPARAM lParam);
	void					OnTabCtrlRButtonUp(WPARAM wParam,LPARAM lParam);
	void					ProcessTabCommand(UINT uMenuID,int iTabHit);

	/* Address bar private message handlers. */
	void					OnAddressBarBeginDrag(void);

	/* Holder window private message handlers. */
	LRESULT CALLBACK		TreeViewHolderWindowCommandHandler(WPARAM wParam);
	LRESULT CALLBACK		TreeViewHolderWindowNotifyHandler(LPARAM lParam);
	void					OnTreeViewHolderWindowTimer(void);

	/* Tabs. */
	void					SelectAdjacentTab(BOOL bNextTab);
	HRESULT					CreateNewTab(TCHAR *TabDirectory,InitialSettings_t *pSettings,TabInfo_t *pTabInfo,BOOL bSwitchToNewTab,int *pTabObjectIndex);
	HRESULT					CreateNewTab(LPITEMIDLIST pidlDirectory,InitialSettings_t *pSettings,TabInfo_t *pTabInfo,BOOL bSwitchToNewTab,int *pTabObjectIndex);
	void					InsertNewTab(LPITEMIDLIST pidlDirectory,int iNewTabIndex,int iTabId);
	HRESULT					CloseTab(int TabIndex);
	HRESULT					OnCloseTab(void);
	HRESULT					RestoreTabs(ILoadSave *pLoadSave);
	void					RefreshTab(int iTabId);
	void					RefreshAllTabs(void);
	void					CloseOtherTabs(int iTab);

	/* Clone Window. */
	void					OnCloneWindow(void);

	/* Menus. */
	void					SetProgramMenuItemStates(HMENU hProgramMenu);
	void					SetArrangeMenuItemStates(HMENU);

	/* Columns. */
	void					SetAllDefaultColumns(void);
	void					SetDefaultRealFolderColumns(list<Column_t> *pColumns);
	void					SetDefaultControlPanelColumns(list<Column_t> *pColumns);
	void					SetDefaultMyComputerColumns(list<Column_t> *pColumns);
	void					SetDefaultRecycleBinColumns(list<Column_t> *pColumns);
	void					SetDefaultPrintersColumns(list<Column_t> *pColumns);
	void					SetDefaultNetworkConnectionsColumns(list<Column_t> *pColumns);
	void					SetDefaultMyNetworkPlacesColumns(list<Column_t> *pColumns);

	/* Status bar. */
	void					HandleStatusBarMenuOpen(void);
	void					HandleStatusBarMenuClose(void);

	/* Application toolbar. */
	void					InitializeApplicationToolbar(void);
	void					ApplicationToolbarNewButton(void);
	ApplicationButton_t		*ApplicationToolbarAddItem(TCHAR *szName,TCHAR *szCommand,BOOL bShowNameOnToolbar);
	void					ApplicationToolbarAddButtonsToToolbar(void);
	void					ApplicationToolbarAddButtonToToolbar(ApplicationButton_t *pab);
	void					ApplicationToolbarRefreshButton(int iItem);
	void					ApplicationToolbarOpenItem(int iItem,TCHAR *szParameters);
	void					ApplicationToolbarDeleteItem(int iItem);
	void					ApplicationToolbarShowItemProperties(int iItem);

	/* Application button properties dialog. */
	void					OnApplicationButtonPropertiesInit(HWND hDlg);
	void					OnApplicationButtonPropertiesOk(HWND hDlg);

	/* New application button dialog. */
	void					OnApplicationToolbarNewButtonInit(HWND hDlg);
	void					OnApplicationToolbarNewButtonOk(HWND hDlg);

	/* Shared between application dialogs. */
	void					OnApplicationToolbarCommandButton(HWND hDlg);

	/* Drives toolbar. */
	void					InsertDrivesIntoDrivesToolbar(void);
	LRESULT					InsertDriveIntoDrivesToolbar(TCHAR *szDrive);
	LRESULT					RemoveDriveFromDrivesToolbar(TCHAR *szDrive);
	void					UpdateDrivesToolbarIcon(TCHAR *szDrive);
	void					DrivesToolbarRefreshAllIcons(void);

	/* File infotips. */
	void					CreateFileInfoTip(int iItem,TCHAR *szInfoTip,UINT cchMax);

	/* Control creation. */
	HWND					CreateAndSubclassListView(HWND hParent,DWORD Style);
	void					CreateMainControls(void);
	void					CreateFolderControls(void);
	void					CreateMainToolbar(void);
	void					CreateAddressToolbar(void);
	void					CreateAddressBar(void);
	void					CreateBookmarksToolbar(void);
	void					CreateDrivesToolbar(void);
	void					CreateApplicationToolbar(void);
	HWND					CreateTabToolbar(HWND hParent,int idCommand,TCHAR *szTip);
	void					CreateTabBacking(void);

	/* Main toolbars. */
	void					InitializeMainToolbars(void);
	void					AdjustMainToolbarSize(void);

	/* Directory specific settings. */
	void					SaveDirectorySpecificSettings(int iTab);
	void					SetDirectorySpecificSettings(int iTab,LPITEMIDLIST pidlDirectory);

	/* Settings. */
	void					SaveAllSettings(void);
	LONG					SaveSettings();
	LONG					LoadSettings(LPCTSTR);
	void					ValidateLoadedSettings(void);
	void					ValidateToolbarSettings(void);
	void					ValidateColumns(void);
	void					ValidateSingleColumnSet(int iColumnSet,list<Column_t> *pColumnList);
	void					ApplyLoadedSettings(void);
	void					ApplyToolbarSettings(void);
	void					AddStyleToToolbar(UINT *fStyle,UINT fStyleToAdd);
	void					SetDefaultValues(void);
	void					TestConfigFile(void);
	void					SaveTabSettingsToRegistry(void);
	int						LoadTabSettingsFromRegistry(void);
	void					LoadColumnFromRegistry(HKEY hColumnsKey,TCHAR *szKeyName,list<Column_t> *pColumns);
	void					SaveColumnToRegistry(HKEY hColumnsKey,TCHAR *szKeyName,list<Column_t> *pColumns);
	void					LoadColumnWidthsFromRegistry(HKEY hColumnsKey,TCHAR *szKeyName,list<Column_t> *pColumns);
	void					SaveColumnWidthsToRegistry(HKEY hColumnsKey,TCHAR *szKeyName,list<Column_t> *pColumns);
	void					LoadDefaultColumnsFromRegistry(void);
	void					SaveDefaultColumnsToRegistry(void);
	void					InitializeBookmarks(void);
	void					SaveBookmarksToRegistry(void);
	void					SaveBookmarksToRegistryInternal(HKEY hKey,Bookmark_t *pBookmark,int count);
	void					LoadBookmarksFromRegistry(void);
	void					LoadBookmarksFromRegistryInternal(HKEY hBookmarks,void *ParentFolder);
	void					LoadApplicationToolbarFromRegistry(void);
	void					LoadApplicationToolbarFromRegistryInternal(HKEY hKey);
	void					SaveApplicationToolbarToRegistry(void);
	void					LoadColorRulesFromRegistry(void);
	void					LoadColorRulesFromRegistryInternal(HKEY hKey);
	void					SaveColorRulesToRegistry(void);
	void					SaveColorRulesToRegistryInternal(HKEY hKey,ListViewColouring_t *plvc,int iCount);
	void					SaveApplicationToolbarToRegistryInternal(HKEY hKey,ApplicationButton_t	*pab,int count);
	void					SaveToolbarInformationToRegistry(void);
	void					LoadToolbarInformationFromRegistry(void);
	void					SaveStateToRegistry(void);
	void					LoadStateFromRegistry(void);
	void					SaveWildcardStateToRegistry(HKEY hParentKey);
	void					LoadWildcardStateFromRegistry(HKEY hParentKey);
	void					SaveAddBookmarkStateToRegistry(HKEY hParentKey);
	void					LoadAddBookmarkStateFromRegistry(HKEY hParentKey);
	void					SaveColorRulesStateToRegistry(HKEY hParentKey);
	void					LoadColorRulesStateFromRegistry(HKEY hParentKey);
	void					SaveCustomizeColorsStateToRegistry(HKEY hParentKey);
	void					LoadCustomizeColorsStateFromRegistry(HKEY hParentKey);
	void					SaveDestroyFilesStateToRegistry(HKEY hParentKey);
	void					LoadDestroyFilesStateFromRegistry(HKEY hParentKey);
	void					SaveDisplayColorsStateToRegistry(HKEY hParentKey);
	void					LoadDisplayColorsStateFromRegistry(HKEY hParentKey);
	void					SaveFilterStateToRegistry(HKEY hParentKey);
	void					LoadFilterStateFromRegistry(HKEY hParentKey);
	void					SaveMassRenameStateToRegistry(HKEY hParentKey);
	void					LoadMassRenameStateFromRegistry(HKEY hParentKey);
	void					SaveMergeFilesStateToRegistry(HKEY hParentKey);
	void					LoadMergeFilesStateFromRegistry(HKEY hParentKey);
	void					SaveOrganizeBookmarksStateToRegistry(HKEY hParentKey);
	void					LoadOrganizeBookmarksStateFromRegistry(HKEY hParentKey);
	void					SaveSearchStateToRegistry(HKEY hParentKey);
	void					LoadSearchStateFromRegistry(HKEY hParentKey);
	void					SaveSelectColumnsStateToRegistry(HKEY hParentKey);
	void					LoadSelectColumnsStateFromRegistry(HKEY hParentKey);
	void					SaveSelectDefaultColumnsStateToRegistry(HKEY hParentKey);
	void					LoadSelectDefaultColumnsStateFromRegistry(HKEY hParentKey);
	void					SaveSetFileAttributesStateToRegistry(HKEY hParentKey);
	void					LoadSetFileAttributesStateFromRegistry(HKEY hParentKey);
	void					SaveSplitFileColumnsStateToRegistry(HKEY hParentKey);
	void					LoadSplitFileStateFromRegistry(HKEY hParentKey);

	/* Window state update. */
	void					UpdateWindowStates(void);
	void					HandleMainWindowText(void);
	void					HandleComboBoxText(void);
	void					HandleTabText(void);
	void					HandleTabText(int iTabId);
	void					HandleTabText(int iTab,int iTabId);
	void					SetTabIcon(void);
	void					SetTabIcon(int iTabId);
	void					SetTabIcon(int iIndex,int iTabId);
	void					SetTabIcon(int iIndex,int iTabId,LPITEMIDLIST pidlDirectory);
	void					HandleTreeViewSelection(void);
	void					SetStatusBarParts(int width);
	void					ResizeWindows(void);
	void					SetListViewInitialPosition(HWND hListView);
	void					AdjustFolderPanePosition(void);
	void					SetComboBoxExTitleString(HWND CbEx,LPITEMIDLIST pidl,TCHAR *szDisplayText);
	void					HandleToolbarItemStates(void);
	HRESULT					HandleStatusText(void);
	void					ToggleFolders(void);

	/* Windows 7 taskbar thumbnail previews. */
	ATOM					RegisterTabProxyClass(TCHAR *szClassName,LPITEMIDLIST pidlDirectory);
	void					CreateTabProxy(LPITEMIDLIST pidlDirectory,int iTabId,BOOL bSwitchToNewTab);
	void					RegisterTab(HWND hTabProxy,TCHAR *szDisplayName,BOOL bTabActive);
	HBITMAP					CaptureTabScreenshot(int iTabId);
	void					GetTabLivePreviewBitmap(int iTabId,TabPreviewInfo_t *ptpi);

	/* Windows 7 jumplist tasks. */
	void					SetupJumplistTasks();

	/* Search. */
	void					OnSearch(HWND hDlg);
	void					SearchSaveState(HWND hDlg);

	/* Tab ID's. */
	void					InitializeTabMap(void);
	void					ReleaseTabId(int iTabId);
	int						GenerateUniqueTabId(void);
	BOOL					CheckTabIdStatus(int iTabId);

	/* Languages. */
	void					SetLanguageModule(void);

	/* Undo. */
	int						RenameFileWithUndo(TCHAR *szNewFileName,TCHAR *szOldFileName);

	/* Default settings. */
	void					SetDefaultTabSettings(TabInfo_t *pTabInfo);

	/* Filters. */
	LONG					LoadFilters(void);
	LONG					SaveFilters(void);

	/* Color rules. */
	void					InitializeColorRules(void);

	/* Arrange menu. */
	void					InitializeArrangeMenuItems(void);
	void					SetActiveArrangeMenuItems(void);
	int						InsertArrangeMenuItems(HMENU hMenu);
	UINT					GetArrangeMenuItemStringIndex(UINT uItemId);
	void					DeletePreviousArrangeMenuItems(void);
	int						DetermineSortModeMenuId(int SortMode);
	int						DetermineGroupModeMenuId(int SortMode);

	/* File operations. */
	void					CopyToFolder(BOOL bMove);
	void					GotoFolder(int FolderCSIDL);
	void					OpenAllSelectedItems(BOOL bOpenInNewTab);
	void					OpenListViewItem(int iItem,BOOL bOpenInNewTab,BOOL bOpenInNewWindow);
	void					OpenItem(TCHAR *szItem,BOOL bOpenInNewTab,BOOL bOpenInNewWindow);
	void					OpenItem(LPITEMIDLIST pidlItem,BOOL bOpenInNewTab,BOOL bOpenInNewWindow);
	void					OpenFolderItem(LPITEMIDLIST pidlItem,BOOL bOpenInNewTab,BOOL bOpenInNewWindow);
	void					OpenFileItem(LPITEMIDLIST pidlItem,TCHAR *szParameters);
	HRESULT					OnListViewCopy(BOOL bCopy);
	HRESULT					ProcessShellMenuCommand(IContextMenu *pContextMenu,UINT CmdIDOffset);
	HRESULT					CreateFileContextMenu(HWND hwnd,LPITEMIDLIST pidlParent,POINT MousePos,UINT uFrom,LPCITEMIDLIST *ppidl,int nFiles,BOOL bRename,BOOL bExtended);
	HRESULT					ShowMultipleFileProperties(LPITEMIDLIST pidlDirectory,LPCITEMIDLIST *ppidl,int nFiles);
	HRESULT					ExecuteActionFromContextMenu(LPITEMIDLIST pidlDirectory,LPCITEMIDLIST *ppidl,int nFiles,TCHAR *szAction,DWORD fMask);

	/* Listview selection file tests. */
	void					BuildListViewFileSelectionList(HWND hListView,std::list<std::wstring> *pFileSelectionList);
	HRESULT					TestListViewSelectionAttributes(SFGAOF *pItemAttributes);
	HRESULT					TestTreeViewSelectionAttributes(SFGAOF *pItemAttributes);	
	HRESULT					GetSelectionAttributes(SFGAOF *pItemAttributes);
	BOOL					IsRenamePossible(void);
	BOOL					IsDeletionPossible(void);
	BOOL					CanShowFileProperties(void);
	BOOL					CanCutOrCopySelection(void);
	BOOL					AreAllSelectedFilesReal(void);
	BOOL					CanPaste(void);

	/* Tabs. */
	void					DuplicateTab(int iTabInternal);

	/* Tab proxys. */
	void					SetTabProxyIcon(int iTabId,HICON hIcon);

	/* Tab drag and drop. */
	void					GetSourceFileName(IDataObject *pDataObject);
	BOOL					CheckItemLocations(int iTabId);

	/* Display window file information. */
	void					HandleFileSelectionDisplay(void);
	void					HandleFileSelectionDisplayZero(void);
	void					HandleFileSelectionDisplayOne(void);
	void					HandleFileSelectionDisplayMore(void);
	void					OutputInformationOnDisplayWindow(TCHAR *szFullFileName,DWInfoType_t InfoType);
	void					FormatDisplayString(TCHAR *szDisplayRaw,int iSelected,TCHAR *szFullFileName,TCHAR *szDisplayFinal,UINT cchMax);
	void					TranslateDisplayWindowBuffer(TCHAR *szSymbol,UINT cchMax,int iSelected,TCHAR *szFullFileName);

	/* Set file attributes dialog. */
	void					OnInitializeFileAttributesDlg(HWND hDlg,LPARAM lParam);
	void					OnFileAttributesDlgOk(HWND hDlg);
	void					OnSetFileAttributesNotify(HWND hDlg,WPARAM wParam,LPARAM lParam);
	void					OnDateReset(HWND hDlg,int FieldToReset);
	void					InitializeAttributesStructure(void);
	void					InitializeDateFields(HWND hDlg);
	void					SetFileAttributesSaveState(HWND hDlg);

	/* Display window colours dialog. */
	void					OnInitializeDisplayColorsDlg(HWND hDlg);
	void					OnDisplayColorsDlgOk(HWND hDlg);
	void					OnDisplayColorsChooseFont(HWND hDlg);
	void					DisplayColorsSaveState(HWND hDlg);
	void					OnDisplayColorsHScroll(HWND hDlg);
	void					OnDisplayColorsEnChange(HWND hDlg,LPARAM lParam);

	/* Delete securely dialog. */
	void					OnDeleteSecurelyOk(HWND hDlg);

	/* Mass rename dialog. */
	void					OnMassRenameOk(HWND hDlg);
	void					MassRenameSaveState(HWND hDlg);

	/* Add bookmark dialog. */
	void					OnAddBookmarkInit(HWND hDlg,LPARAM lParam);
	void					OnAddBookmarkOk(HWND hDlg);
	void					OnAddBookmarkNewFolder(HWND hDlg);
	void					OnBookmarkDetails(HWND hDlg);
	int						LocateBookmarkInComboBox(HWND hComboBox,void *pBookmarkHandle);
	void					InsertFolderItemsIntoComboBox(HWND hCreateIn,Bookmark_t *pBookmark);
	void					InsertFolderItemsIntoComboBoxInternal(HWND hCreateIn,Bookmark_t *pBookmark,int iIndent,int iBookmarkFolderItem);
	void					AddBookmarkSaveState(HWND hDlg);

	/* New bookmark folder dialog. */
	void					OnNewBookmarkFolderInit(HWND hDlg);
	void					OnNewBookmarkFolderOk(HWND hDlg);

	/* Bookmarks organization dialog. */
	void					OnOrganizeBookmarksInit(HWND hDlg);
	void					MoveColumnItem(HWND hDlg,BOOL bUp);
	void					OrganizeBookmarksMove(HWND hDlg,BOOL bUp);
	void					OnOrganizeBookmarksDelete(HWND hDlg);
	void					OnOrganizeBookmarksOk(HWND hDlg);
	void					OnOrganizeBookmarksProperties(HWND hDlg);
	void					OnOrganizeBookmarksDoubleClick(HWND hDlg,LPARAM lParam);
	void					OnOrganizeBookmarksRightClick(HWND hDlg,LPARAM lParam);
	void					OrganizeBookmarksRefreshItem(HWND hDlg,int iItem);
	void					OnOrganizeBookmarksTvnSelChanged(HWND hDlg,LPARAM lParam);
	void					OnOrganizeBookmarksInitMenu(HWND hDlg,WPARAM wParam);
	void					OnOrganizeBookmarksShowOnToolbar(HWND hDlg);
	void					OnOrganizeBookmarksOpen(HWND hDlg,BOOL bOpenInNewTab);
	void					ShowBookmarkProperties(HWND hDlg,HWND hListView,int iItem);
	void					InsertBookmarksIntoTreeView(HWND hTreeView,HTREEITEM hParent,Bookmark_t *pBookmark);
	void					InsertBookmarksIntoTreeViewInternal(HWND hTreeView,HTREEITEM hParent,Bookmark_t *pBookmark);
	void					InsertBookmarkFolderItemsIntoTreeView(HWND hFolders,HTREEITEM hParent,Bookmark_t *pBookmark);
	void					InsertBookmarksIntoListView(HWND hBookmarks,Bookmark_t *pBookmark);
	void					InitializeBookmarkToolbarMap(void);
	int						GenerateUniqueBookmarkToolbarId(void);
	void					GetBookmarkMenuItemDirectory(HMENU hMenu,int iBookmarkId,TCHAR *szDirectory,UINT uBufSize);
	void					OrganizeBookmarksTrackInTreeView(HWND hDlg,void *pBookmarkHandle);
	void					OrganizeBookmarksSaveState(HWND hDlg);

	/* Bookmark properties dialog. */
	void					OnBookmarkPropertiesInit(HWND hDlg,LPARAM lParam);
	void					OnBookmarkPropertiesOk(HWND hDlg);

	/* Bookmark folder properties dialog. */
	void					OnBookmarkFolderPropertiesInit(HWND hDlg,LPARAM lParam);
	void					OnBookmarkFolderPropertiesOk(HWND hDlg);

	/* Helper functions for the bookmark toolbar. */
	void					BookmarkToolbarOpenItem(int iItem,BOOL bOpenInNewTab);
	void					BookmarkToolbarDeleteItem(int iItem);
	void					BookmarkToolbarShowItemProperties(int iItem);
	void					BookmarkToolbarNewBookmark(int iItem);
	void					BookmarkToolbarNewFolder(int iItem);
	void					RemoveItemFromBookmarksToolbar(void *pBookmarkHandle);

	/* Select column dialog. */
	void					OnInitColumnDlg(HWND hDlg);
	void					MoveColumnItem2(HWND hDlg,BOOL bUp);
	void					EnableColumnItem(HWND hDlg,BOOL bEnable);
	BOOL					OnColumnDlgLvnItemChanging(HWND hDlg,LPARAM lParam);
	void					OnColumnDlgOk(HWND hDlg);
	void					SelectColumnsSaveState(HWND hDlg);

	/* Columns. */
	int						LookupColumnNameStringIndex(int iColumnId);
	int						LookupColumnDescriptionStringIndex(int iColumnId);
	void					CopyColumnInfoToClipboard(void);

	/* Filter dialog. */
	void					FilterSaveState(HWND hDlg);

	/* Bookmarks. */
	void					InsertBookmarkToolbarButtons(void);
	void					InsertToolbarButtonsInternal(Bookmark_t *pBookmark);
	void					InsertBookmarkIntoToolbar(Bookmark_t *pBookmark,int id);
	void					UpdateToolbarButton(Bookmark_t *pBookmark);

	/* Bookmark handling. */
	HRESULT					ExpandAndBrowsePath(TCHAR *szPath);
	HRESULT					ExpandAndBrowsePath(TCHAR *szPath,BOOL bOpenInNewTab,BOOL bSwitchToNewTab);
	void					InsertBookmarksIntoMenu(void);
	void					InsertBookmarksIntoMenuInternal(HMENU hMenu,Bookmark_t *pBookmark,int iStartPos,int iStartId);
	void					InsertBookmarksIntoMenuInternal(HMENU hMenu,Bookmark_t *pBookmark,int iStartPos);
	BOOL					DeleteBookmarkSafe(HWND hwnd,void *pBookmarkHandle);

	/* Filtering. */
	void					SetFilterStatus(void);

	/* Default settings dialog. */
	void					OnDefaultSettingsNewTabDir(HWND hDlg);
	void					DefaultSettingsSetNewTabDir(HWND hEdit,LPITEMIDLIST pidl);
	void					DefaultSettingsSetNewTabDir(HWND hEdit,TCHAR *szPath);

	/* Files and folders dialog. */
	void					SetInfoTipWindowStates(HWND hDlg);
	void					SetFolderSizeWindowState(HWND hDlg);

	/* Split file dialog. */
	void					OnSplitFileInit(HWND hDlg);
	void					OnSplitFileOk(HWND hDlg);
	void					OnSplitFileCancel(HWND hDlg);
	void					OnSplitFileChangeOutputDirectory(HWND hDlg);
	void					OnSplitFinished(HWND hDlg);
	void					SplitFileSaveState(HWND hDlg);

	/* Merge files dialog. */
	void					OnMergeFilesInit(HWND hDlg);
	void					OnMergeFilesRemove(HWND hDlg);
	void					OnMergeFilesMove(HWND hDlg,BOOL bUp);
	void					OnMergeFilesOk(HWND hDlg);
	void					OnMergeFilesCancel(HWND hDlg);
	void					OnMergeFilesChangeOutputDirectory(HWND hDlg);
	void					OnMergingFinished(HWND hDlg);
	void					MergeFilesSaveState(HWND hDlg);

	/* Destroy files dialog. */
	void					OnDestroyFilesInit(HWND hDlg);
	void					OnDestroyFilesRemove(HWND hDlg);
	void					OnDestroyFilesOk(HWND hDlg);
	void					OnDestroyFilesConfirmDelete(HWND hDlg);
	void					DestroyFilesSaveState(HWND hDlg);

	/* Wildcard select dialog. */
	void					OnWildcardSelectInit(HWND hDlg);
	void					OnWildcardSelectOk(HWND hDlg);
	void					WildcardSelectSaveState(HWND hDlg);

	/* Rename tab dialog. */
	void					OnRenameTabInit(HWND hDlg);
	void					OnRenameTabOk(HWND hDlg);

	/* Set default columns dialog. */
	void					OnSetDefaultColumnsInit(HWND hDlg);
	void					OnSetDefaultColumnsCBChange(HWND hDlg);
	BOOL					OnSetDefaultColumnsLvnItemChanging(HWND hDlg,LPARAM lParam);
	void					OnSetDefaultColumnsOk(HWND hDlg);
	void					OnSetDefaultColumnsInitialzeTempColumns(void);
	void					OnSetDefaultColumnsExportTempColumns(void);
	void					GetCurrentDefaultColumnState(HWND hDlg);
	void					SetDefaultColumnsSaveState(HWND hDlg);

	/* Customize colors dialog. */
	void					CustomizeColorsMove(HWND hDlg,BOOL bUp);
	void					CustomizeColorsDelete(HWND hDlg);
	void					CustomizeColorsSaveState(HWND hDlg);
	void					OnEditColorRule(HWND hDlg,int iSelected);

	/* New color rule dialog. */
	void					ColorRuleChooseNewColor(HWND hDlg);

	/* XML Settings. */
	void					LoadGenericSettingsFromXML(MSXML2::IXMLDOMDocument *pXMLDom);
	void					SaveGenericSettingsToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pRoot);
	int						LoadTabSettingsFromXML(MSXML2::IXMLDOMDocument *pXMLDom);
	void					SaveTabSettingsToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pRoot);
	void					SaveTabSettingsToXMLnternal(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe);
	int						LoadColumnFromXML(MSXML2::IXMLDOMNode *pNode,list<Column_t> *pColumns);
	void					SaveColumnToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pColumnsNode,list<Column_t> *pColumns,TCHAR *szColumnSet,int iIndent);
	int						LoadFiltersFromXML(MSXML2::IXMLDOMDocument *pXMLDom);
	void					SaveFiltersToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pRoot);
	void					SaveFiltersToXMLInternal(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe);
	int						LoadBookmarksFromXML(MSXML2::IXMLDOMDocument *pXMLDom);
	void					LoadBookmarksFromXMLInternal(MSXML2::IXMLDOMNode *pNode,void *pParentFolder);
	void					SaveBookmarksToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pRoot);
	void					SaveBookmarksToXMLInternal(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe,Bookmark_t *pBookmark);
	int						LoadDefaultColumnsFromXML(MSXML2::IXMLDOMDocument *pXMLDom);
	void					SaveDefaultColumnsToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pRoot);
	void					SaveDefaultColumnsToXMLInternal(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pColumnsNode);
	int						LoadWindowPositionFromXML(MSXML2::IXMLDOMDocument *pXMLDom,InitialWindowPos_t *piwp);
	void					SaveWindowPositionToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pRoot);
	void					SaveWindowPositionToXMLInternal(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pWndPosNode);
	void					LoadApplicationToolbarFromXML(MSXML2::IXMLDOMDocument *pXMLDom);
	void					LoadApplicationToolbarFromXMLInternal(MSXML2::IXMLDOMNode *pNode);
	void					SaveApplicationToolbarToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pRoot);
	void					SaveApplicationToolbarToXMLInternal(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe,ApplicationButton_t *pab);
	void					LoadColorRulesFromXML(MSXML2::IXMLDOMDocument *pXMLDom);
	void					LoadColorRulesFromXMLInternal(MSXML2::IXMLDOMNode *pNode);
	void					SaveColorRulesToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pRoot);
	void					SaveColorRulesToXMLInternal(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe,ListViewColouring_t *plvc);
	void					LoadToolbarInformationFromXML(MSXML2::IXMLDOMDocument *pXMLDom);
	void					SaveToolbarInformationToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pRoot);
	void					SaveToolbarInformationToXMLnternal(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe);
	void					LoadStateFromXML(MSXML2::IXMLDOMDocument *pXMLDom);
	void					LoadColorRulesStateFromXML(MSXML2::IXMLDOMNamedNodeMap *pam,long lChildNodes);
	void					LoadCustomizeColorsStateFromSML(MSXML2::IXMLDOMNamedNodeMap *pam,long lChildNodes);
	void					LoadSearchStateFromXML(MSXML2::IXMLDOMNamedNodeMap *pam,long lChildNodes);
	void					LoadWildcardStateFromXML(MSXML2::IXMLDOMNamedNodeMap *pam,long lChildNodes);
	void					SaveStateToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pRoot);
	void					SaveColorRulesStateToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe);
	void					SaveCustomizeColorsStateToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe);
	void					SaveSearchStateToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe);
	void					SaveWildcardStateToXML(MSXML2::IXMLDOMDocument *pXMLDom,MSXML2::IXMLDOMElement *pe);
	void					MapAttributeToValue(MSXML2::IXMLDOMNode *pNode,WCHAR *wszName,WCHAR *wszValue);
	void					MapTabAttributeValue(WCHAR *wszName,WCHAR *wszValue,InitialSettings_t *pSettings,TabInfo_t *pTabInfo);

	/* Miscellaneous. */
	int						CreateListViewFileList(TCHAR *FileNameList,unsigned int BufferSize);
	BOOL					CompareVirtualFolders(UINT uFolderCSIDL);
	void					OnShowOptions(void);
	void					AddLanguages(HWND hDlg);
	WORD					AddLanguageToComboBox(HWND hComboBox,TCHAR *szImageDirectory,TCHAR *szFileName);
	int						GetLanguageIDFromIndex(HWND hDlg,int iIndex);
	void					PushGlobalSettingsToTab(int iTabId);
	void					PushGlobalSettingsToAllTabs(void);
	void					CreateViewsMenu(POINT *ptOrigin);
	void					SetMenuItemBitmap(HMENU hMenu,UINT ItemID,int iBitmap);
	void					SetMenuOwnerDraw(HMENU hMenu);
	void					SetMenuOwnerDrawInternal(HMENU hMenu,int nMenus);
	void					SetMenuItemOwnerDrawn(HMENU hMenu,int iItem);
	void					SetInitialToolbarButtons(void);
	int						LookupToolbarButtonTextID(int iButtonID);
	int						LookupToolbarButtonImage(int iButtonID);
	BYTE					LookupToolbarButtonExtraStyles(int iButtonID);
	void					InsertToolbarButtons(void);
	void					InsertToolbarButton(ToolbarButton_t *ptb,int iPos);
	void					DeleteToolbarButton(int iButton);
	void					AddStringsToMainToolbar(void);
	void					CreateStatusBar(void);
	void					InitializeDisplayWindow(void);
	void					InitializeTabs(void);
	void					AddDefaultTabIcons(HIMAGELIST himlTab);
	void					InitializeMenus(void);
	void					SetGoMenuName(HMENU hMenu,UINT uMenuID,UINT csidl);
	int						CreateDriveFreeSpaceString(TCHAR *szPath,TCHAR *szBuffer,int nBuffer);
	BOOL					CheckItemSelection(void);
	HWND					DecodeWindowConstant(UINT uWindow);
	HWND					MyGetNextWindow(HWND hwndCurrent);
	BOOL					IsNextWindowVisible(HWND hNext);
	void					ShowMainRebarBand(HWND hwnd,BOOL bShow);
	BOOL					OnMouseWheel(WPARAM wParam,LPARAM lParam);
	void					CycleViewState(BOOL bCycleForward);
	int						GetViewModeMenuId(UINT uViewMode);
	int						GetViewModeMenuStringId(UINT uViewMode);
	BOOL					VerifyLanguageVersion(TCHAR *szLanguageModule);
	HMENU					CreateRebarHistoryMenu(BOOL bBack);




	/* ------ Internal state. ------ */

	HWND					m_hContainer;
	HWND					m_hStatusBar;
	HWND					m_hMainRebar;
	HWND					m_hDisplayWindow;
	HWND					m_hTabCtrl;
	HWND					m_hTabWindowToolbar;
	HWND					m_hTreeView;
	HWND					m_hHolder;
	HWND					m_hAddressBar;
	HWND					m_hMainToolbar;
	HWND					m_hListView[MAX_TABS];
	HWND					m_hFoldersToolbar;
	HWND					m_hTabBacking;
	HWND					m_hAddressToolbar;
	HWND					m_hBookmarksToolbar;
	HWND					m_hDrivesToolbar;
	HWND					m_hApplicationToolbar;

	MyIFolderView2 *		m_pFolderView[MAX_TABS];
	IShellBrowser2 *		m_pShellBrowser[MAX_TABS];
	IDisplayWindowMain *	m_pDisplayMain;
	IShellBrowser2 *		m_pActiveShellBrowser;
	IDirectoryMonitor *		m_pDirMon;
	CMyTreeView *			m_pMyTreeView;
	CBookmark				m_Bookmark;
	CCustomMenu *			m_pCustomMenu;
	HANDLE					m_hIconThread;
	HANDLE					m_hTreeViewIconThread;
	HANDLE					m_hFolderSizeThread;

	/** Internal state. **/
	HWND					m_hActiveListView;
	HWND					m_hLastActiveWindow;
	HWND					m_hNextClipboardViewer;
	LPITEMIDLIST			m_pidlDirectory;
	HMENU					m_hArrangeSubMenu;
	HMENU					m_hGroupBySubMenu;
	HMENU					m_hRightClickMenu;
	HMENU					m_hBookmarksMenu;
	HMENU					m_hTabRightClickMenu;
	HMENU					m_hToolbarRightClickMenu;
	HMENU					m_hBookmarksRightClickMenu;
	HMENU					m_hApplicationRightClickMenu;
	HMENU					m_hDisplayWindowRightClickMenu;
	HMENU					m_hViewsMenu;
	TCHAR					m_CurrentDirectory[MAX_PATH];
	TCHAR					m_DefaultTabDirectoryStatic[MAX_PATH];
	TCHAR					m_DefaultTabDirectory[MAX_PATH];
	TCHAR					m_OldTreeViewFileName[MAX_PATH];
	DWORD					m_Language;
	DWORD					m_dwMajorVersion;
	DWORD					m_dwMinorVersion;
	LONG					m_DisplayWindowHeight;
	BOOL					m_bTreeViewRightClick;
	BOOL					m_bSelectingTreeViewDirectory;
	BOOL					m_bInVirtualFolder;
	BOOL					m_bStickySelection;
	BOOL					m_bAttemptToolbarRestore;
	BOOL					m_bLanguageLoaded;
	BOOL					m_bTreeViewOpenInNewTab;
	unsigned int			m_TreeViewWidth;
	int						m_iObjectIndex;
	int						m_iMaxArrangeMenuItem;
	int						m_iLastSelectedTab;
	int						m_iTabSelectedItem;
	int						m_ListViewEditingStage;

	list<ViewMode_t>		m_ViewModes;

	/* Initialization. */
	BOOL					m_bLoadSettingsFromXML;
	Color					m_DisplayWindowCentreColor;
	Color					m_DisplayWindowSurroundColor;
	COLORREF				m_DisplayWindowTextColor;
	HFONT					m_DisplayWindowFont;

	/* Internal tab state. */
	TabInfo_t				m_TabInfo[MAX_TABS];
	UINT					m_uTabMap[MAX_TABS];

	/* Directory-speecific settings. */
	std::list<DirectorySettings_t>	m_DirectorySettingsList;

	/* User options variables. */
	BOOL					m_bShowStatusBar;
	BOOL					m_bShowFolders;
	BOOL					m_bShowAddressBar;
	BOOL					m_bShowDisplayWindow;
	BOOL					m_bShowMainToolbar;
	BOOL					m_bShowBookmarksToolbar;
	BOOL					m_bShowDrivesToolbar;
	BOOL					m_bShowApplicationToolbar;
	BOOL					m_bAlwaysShowTabBar;
	BOOL					m_bShowTabBar;
	BOOL					m_bShowFullTitlePath;
	BOOL					m_bAlwaysOpenNewTab;
	BOOL					m_bShowFolderSizes;
	BOOL					m_bDisableFolderSizesNetworkRemovable;
	BOOL					m_bUnlockFolders;
	BOOL					m_bOpenNewTabNextToCurrent;
	BOOL					m_bConfirmCloseTabs;
	BOOL					m_bTreeViewDelayEnabled;
	BOOL					m_bSavePreferencesToXMLFile;
	BOOL					m_bLockToolbars;
	BOOL					m_bExtendTabControl;
	BOOL					m_bUseFullRowSelect;
	BOOL					m_bShowUserNameInTitleBar;
	BOOL					m_bShowPrivilegeLevelInTitleBar;
	BOOL					m_bShowFilePreviews;
	BOOL					m_bOneClickActivate;
	BOOL					m_bAllowMultipleInstances;
	BOOL					m_bForceSameTabWidth;
	BOOL					m_bDoubleClickTabClose;
	BOOL					m_bHandleZipFiles;
	BOOL					m_bInsertSorted;
	BOOL					m_bOverwriteExistingFilesConfirmation;
	BOOL					m_bCheckBoxSelection;
	BOOL					m_bForceSize;
	BOOL					m_bCloseMainWindowOnTabClose;
	BOOL					m_bShowTabBarAtBottom;
	BOOL					m_bShowTaskbarThumbnails;
	BOOL					m_bShowTaskbarThumbnailsProvisional;
	BOOL					m_bSynchronizeTreeview;
	BOOL					m_bTVAutoExpandSelected;
	BOOL					m_bLargeToolbarIcons;
	SizeDisplayFormat_t		m_SizeDisplayFormat;
	UINT					m_StartupMode;
	UINT					m_ReplaceExplorerMode;

	/* Infotips (user options). */
	BOOL					m_bShowInfoTips;
	UINT					m_InfoTipType;

	/* Global options. */
	DWORD					m_ViewModeGlobal;
	BOOL					m_bShowHiddenGlobal;
	BOOL					m_bShowExtensionsGlobal;
	BOOL					m_bShowInGroupsGlobal;
	BOOL					m_bAutoArrangeGlobal;
	BOOL					m_bSortAscendingGlobal;
	BOOL					m_bShowFriendlyDatesGlobal;
	BOOL					m_bShowGridlinesGlobal;
	BOOL					m_bHideSystemFilesGlobal;
	BOOL					m_bHideLinkExtensionGlobal;

	/* Windows 7 taskbar thumbnail previews. */
	DwmInvalidateIconicBitmapsProc	DwmInvalidateIconicBitmaps;
	HMODULE					m_hDwmapi;
	ITaskbarList4			*m_pTaskbarList3;
	list<TabProxyInfo_t>	m_TabProxyList;
	UINT					m_uTaskbarButtonCreatedMessage;
	BOOL					m_bTaskbarInitialised;

	/* Win+E keyboard hook DLL. */
	HHOOK					m_hKeyboardHook;

	/* Undo state. */
	BOOL					m_bCanUndo;
	UndoItem_t				m_UndoItem;

	/* Main toolbars. */
	REBARBANDINFO			m_ToolbarInformation[NUM_MAIN_TOOLBARS];

	/* Main toolbar. */
	HIMAGELIST				m_himlToolbarSmall;
	HIMAGELIST				m_himlToolbarLarge;

	/* Bookmark toolbar. */
	UINT					m_uBookmarkToolbarMap[MAX_BOOKMARKTOOLBAR_ITEMS];
	int						m_iSelectedRClick;

	/* Toolbar buttons. */
	list<ToolbarButton_t>	m_tbInitial;

	/* Drives toolbar. */
	int						m_nDrivesInToolbar;

	/* Application toolbar. */
	ApplicationButton_t		*m_pAppButtons;
	ApplicationButton_t		*m_pAppButtonSelected;
	int						m_nAppButtons;
	int						m_iAppIdOffset;

	/* Display window details. */
	list<DWRule_t>			m_DWRules;
	list<DWLine_t>			m_DWLines;
	list<DWCommand_t>		m_DWCommands;

	/* Display window folder sizes. */
	list<DWFolderSize_t>	m_DWFolderSizes;
	int						m_iDWFolderSizeUniqueId;

	/* Default columns. */
	list<Column_t>			m_RealFolderColumnList;
	list<Column_t>			m_MyComputerColumnList;
	list<Column_t>			m_ControlPanelColumnList;
	list<Column_t>			m_RecycleBinColumnList;
	list<Column_t>			m_PrintersColumnList;
	list<Column_t>			m_NetworkConnectionsColumnList;
	list<Column_t>			m_MyNetworkPlacesColumnList;

	/* ListView selection. */
	BOOL					m_bCountingUp;
	BOOL					m_bCountingDown;
	BOOL					m_bInverted;
	int						m_nSelected;
	int						m_nSelectedOnInvert;
	int						m_ListViewMButtonItem;

	/* Listview item colouring. */
	list<ListViewColouring_t>	m_ColourFilter;
	COLORREF				m_rgbCompressed;
	COLORREF				m_rgbEncrypted;

	/* Copy/cut. */
	IDataObject				*m_pClipboardDataObject;
	HTREEITEM				m_hCutTreeViewItem;

	/* Drag and drop. */
	BOOL					m_bDragging;
	BOOL					m_bDragCancelled;
	BOOL					m_bDragAllowed;

	/* Rename support. */
	BOOL					m_bListViewBeginRename;
	BOOL					m_bListViewRenaming;
	int						m_iItemEditing;

	/* Tab handler data. */
	BOOL					m_bTabBeenDragged;
	BOOL					m_bTCMouseCaptured;
	RECT					m_rcDraggedTab;
	int						m_iTabMenuItem;

	/* Shell context menu. */
	IContextMenu3			*m_pShellContext3;
	IContextMenu2			*m_pShellContext2;
	IContextMenu			*m_pShellContext;
	HMENU					m_hMenu;
	BOOL					m_bMixedMenu;
	int						m_MenuPos;
	LRESULT					(CALLBACK *DefaultMainWndProc)(HWND,UINT,WPARAM,LPARAM);

	/* Statusbar. */
	list<StatusBarPart_t>	m_StatusBarParts;
	BOOL					m_bAlteredStatusBarParts;

	/* Tab drop target. */
	IDragSourceHelper *		m_pDragSourceHelper;
	IDropTargetHelper *		m_pDropTargetHelper;
	TCHAR					m_pszSource[MAX_PATH];
	DragTypes_t				m_DragType;
	BOOL					m_bDataAccept;
	int						m_iTabDragTab;

	/* Set file attribute dialog internal state. */
	BOOL					m_bModificationDateEnabled;
	BOOL					m_bCreationDateEnabled;
	BOOL					m_bAccessDateEnabled;

	/* Wildcard selection dialog. */
	BOOL					m_bWildcardSelect;
	list<WildcardSelectInfo_t>	m_wsiList;
	TCHAR					m_szwsiText[256];
	BOOL					m_bWildcardDlgStateSaved;
	POINT					m_ptWildcardSelect;

	/* Add bookmark dialog. */
	BOOL					m_bAddBookmarkDlgStateSaved;
	POINT					m_ptAddBookmark;

	/* Destroy files dialog. */
	BOOL					m_bDestroyFilesDlgStateSaved;
	POINT					m_ptDestroyFiles;

	/* Filter dialog. */
	BOOL					m_bFilterDlgStateSaved;
	POINT					m_ptFilter;

	/* Mass rename dialog. */
	BOOL					m_bMassRenameDlgStateSaved;
	POINT					m_ptMassRename;

	/* Merge files dialog. */
	BOOL					m_bMergeFilesDlgStateSaved;
	POINT					m_ptMergeFiles;

	/* Organize bookmarks dialog. */
	BOOL					m_bOrganizeBookmarksDlgStateSaved;
	POINT					m_ptOrganizeBookmarks;

	/* Drive properties dialog. */
	BOOL					m_bDrivePropertiesDlgStateSaved;
	POINT					m_ptDriveProperties;

	/* Split file dialog data. */
	TCHAR					m_SplitFileName[MAX_PATH];
	BOOL					m_bSplittingFile;

	/* Select columns dialog. */
	BOOL					m_bSelectColumnsDlgStateSaved;
	POINT					m_ptSelectColumns;

	/* Set default columns dialog. */
	BOOL					m_bSetDefaultColumnsDlgStateSaved;
	POINT					m_ptSetDefaultColumns;

	/* Display colors dialog. */
	BOOL					m_bDisplayColorsDlgStateSaved;
	POINT					m_ptDisplayColors;

	/* Set file attributes diaog. */
	list<SetFileAttributesInfo_t>	m_sfaiList;
	BOOL					m_bSetFileAttributesDlgStateSaved;
	POINT					m_ptSetFileAttributes;

	/* Split file dialog. */
	BOOL					m_bSplitFileDlgStateSaved;
	POINT					m_ptSplitFile;

	/* Customize colors dialog. */
	BOOL					m_bCustomizeColorsDlgStateSaved;
	POINT					m_ptCustomizeColors;
	COLORREF				m_crInitialColor;

	/* New color rule dialog. */
	ListViewColouring_t		*m_pColoringItem;
	COLORREF				m_ccCustomColors[16];

	/* Search dialog. */
	BOOL					m_bSearchDlgStateSaved;
	BOOL					m_bSearchSubFolders;
	BOOL					m_bSetSearchTimer;
	POINT					m_ptSearch;
	int						m_iSearchWidth;
	int						m_iSearchHeight;
	int						m_iColumnWidth1;
	int						m_iColumnWidth2;
	TCHAR					m_SearchPatternText[MAX_PATH];
	list<SearchDirectoryInfo_t>	m_SearchDirectories;
	list<SearchPatternInfo_t>	m_SearchPatterns;
	list<LPITEMIDLIST>		m_SearchItems;
	
	/* Cut items data. */
	list<CutFile_t>			m_CutFileNameList;
	BOOL					m_bCuttingItems;
	int						m_iCutTabInternal;

	/* Arrange menu related data. */
	list<ArrangeMenuItem_t>	m_ArrangeList;
	list<ArrangeMenuItem_t>	*m_pActiveArrangeMenuItems;
	list<ArrangeMenuItem_t>	m_ArrangeMenuRealFolder;
	list<ArrangeMenuItem_t>	m_ArrangeMenuMyComputer;
	list<ArrangeMenuItem_t>	m_ArrangeMenuControlPanel;
	list<ArrangeMenuItem_t>	m_ArrangeMenuRecycleBin;

	/* Filtering related data. */
	list<Filter_t>			m_FilterList;

	/* TreeView middle click. */
	HTREEITEM				m_hTVMButtonItem;

	BOOL					m_bBlockNext;
};

typedef struct
{
	CContainer	*pContainer;
	int			iTabId;
} TabProxy_t;

#endif