// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "APhysXLab31.h"

#include "MainFrm.h"
#include "APhysXLab31View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CBCGPFrameWnd)

const int  iMaxUserToolbars		= 10;
const UINT uiFirstUserToolBarId	= AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId	= uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, OnToolbarReset)
	ON_WM_ACTIVATEAPP()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CURSTATE,
	ID_INDICATOR_SHOOTTYPE,
	ID_INDICATOR_SHOOTMODE,
	ID_INDICATOR_DRAGOPT,
	ID_INDICATOR_DRVMODE,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

bool CMenuIconMgr::InitImage(UINT uiResID)
{
	m_images.SetImageSize (CSize (16, 16));
	m_images.SetTransparentColor (RGB (255, 0, 255));
	if (!m_images.Load (uiResID))
		return false;

	m_imagesGray.SetImageSize (CSize (16, 16));
	m_imagesGray.SetTransparentColor (RGB (255, 0, 255));
	if (!m_imagesGray.Load (uiResID))
		return false;
	if (!m_imagesGray.GrayImages(100))
		return false;

	m_idToicon.clear();
	return true;
}

bool CMenuIconMgr::AddItemMap(UINT menuID, int imgIndex)
{
	if (0 > imgIndex)
		return false;
	if (imgIndex >= m_images.GetCount())
		return false;

	IconPair ip;
	ip.m_normal = m_images.ExtractIcon(imgIndex);
	ip.m_gray = m_imagesGray.ExtractIcon(imgIndex);
	m_idToicon[menuID] = ip;
	return true;
}

HICON CMenuIconMgr::GetIcon(UINT menuID, bool isGray)
{
	std::map<UINT, IconPair>::const_iterator it = m_idToicon.find(menuID);
	if (it == m_idToicon.end())
		return 0;

	if (isGray)
		return it->second.m_gray;
	return it->second.m_normal;
}

// CMainFrame construction/destruction
CMainFrame::CMainFrame() : m_PopMenuIndex(5), m_PopMenuName("最近文档"), m_ConfigName("Configs\\APhysXLab3Config.ini")
{
	// TODO: add member initialization code here
	m_bActive = false;
	m_pView = 0;
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::OnDestroy()
{
	APhysXCommonDNet::MLogWrapper::Instance->SetOutputWnd(nullptr);
	CBCGPFrameWnd::OnDestroy();

	// TODO: Add your message handler code here
	CMenu* pRecent = GetRecentMenu();
	if (0 == pRecent)
		return;

	WritePrivateProfileString(_T("Recent File"), 0, 0, m_ConfigName);
	int nMenuItems = pRecent->GetMenuItemCount();
	assert(nMenuItems <= RECENT_FILE_ID_COUNT);
	if (nMenuItems > RECENT_FILE_ID_COUNT)
		nMenuItems = RECENT_FILE_ID_COUNT;

	ACString str;
	str.Format(_T("%d"), nMenuItems);
	WritePrivateProfileString(_T("Recent File"), _T("Count"), str, m_ConfigName);
	for (int i = 0; i < nMenuItems; ++i)
	{
		str.Format(_T("Item%d"), i);
		WritePrivateProfileString(_T("Recent File"), str, m_Names[i], m_ConfigName);
	}

//	WritePrivateProfileString(_T("BindFF"), _T("Name"), m_FFbindingToMA, m_ConfigName);
}


BOOL CMainFrame::OnDrawMenuImage (	CDC* pDC, 
							  const CBCGPToolbarMenuButton* pMenuButton, 
							  const CRect& rectImage)
{
	CRect rect(rectImage);
	rect.DeflateRect(2, 2);
	CPoint pt(rect.left, rect.top);
	if (pMenuButton->m_nID == ID_EDIT_ADDSTATICOBJ)
	{
		HICON hIcon = m_miiMgr.GetIcon(ID_EDIT_ADDSTATICOBJ, !m_pView->EnableAddObject());
		if (0 != hIcon)
		{
			pDC->DrawState(pt, rect.Size(), hIcon, DSS_NORMAL, HBRUSH(0));
			return TRUE;
		}
	}
	return FALSE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// enable VS 2005 look:
	CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2005));
	CBCGPTabbedControlBar::m_StyleTabWnd = CBCGPTabWnd::STYLE_3D_ROUNDED;
	CBCGPVisualManager::GetInstance ();
	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);

	// VISUAL_MANAGER
	
	if (CBCGPToolBar::GetUserImages () == NULL)
	{
		// Load toolbar user images:
		if (!m_UserImages.Load (_T(".\\UserImages.bmp")))
		{
			TRACE(_T("Failed to load user images\n"));
		}
		else
		{
			CBCGPToolBar::SetUserImages (&m_UserImages);
		}
	}

	CBCGPToolBar::EnableQuickCustomization ();

	CMenu* pMenu = GetMenu();
	if (0 != pMenu)
	{
		CMenu* pMFile = pMenu->GetSubMenu(0);
		if (0 != pMFile)
		{
			int nCount = 0;
			pMFile->InsertMenu(m_PopMenuIndex, MF_BYPOSITION | MF_POPUP | MF_STRING, PtrToUint(pMFile->GetSafeHmenu()), m_PopMenuName);
			CMenu* pRecent = pMFile->GetSubMenu(m_PopMenuIndex);

			AIniFile cfgINI;
			if (cfgINI.Open(_TWC2AS(m_ConfigName)))
			{
				nCount = cfgINI.GetValueAsInt("Recent File", "Count", RECENT_FILE_ID_COUNT);
				if (RECENT_FILE_ID_COUNT < nCount)
					nCount = RECENT_FILE_ID_COUNT;

				AString str;
				int index = 0;
				for (int i = 0; i < nCount; ++i)
				{
					str.Format("Item%d", i);
					m_Names[index] = cfgINI.GetValueAsString("Recent File", str);
					if (!m_Names[index].IsEmpty())
					{
						pRecent->InsertMenu(index, MF_BYPOSITION | MF_STRING, RECENT_FILE_ID_BEGIN + index, GetRecentlyFileText(index));
						++index;
					}
				}

		//		m_FFbindingToMA = cfgINI.GetValueAsString("BindFF", "Name");
				cfgINI.Close();
			}

			if (0 == nCount)
				pMFile->EnableMenuItem(m_PopMenuIndex, MF_DISABLED | MF_GRAYED | MF_BYPOSITION);
		}
	}

	// TODO: Define your own basic commands. Be sure, that each pulldown 
	// menu have at least one basic command.

	CList<UINT, UINT>	lstBasicCommands;

	lstBasicCommands.AddTail (ID_VIEW_TOOLBARS);
	lstBasicCommands.AddTail (ID_FILE_NEW);
	lstBasicCommands.AddTail (ID_FILE_OPEN);
	lstBasicCommands.AddTail (ID_FILE_SAVE);
	lstBasicCommands.AddTail (ID_FILE_PRINT);
	lstBasicCommands.AddTail (ID_APP_EXIT);
	lstBasicCommands.AddTail (ID_EDIT_CUT);
	lstBasicCommands.AddTail (ID_EDIT_PASTE);
	lstBasicCommands.AddTail (ID_EDIT_UNDO);
	lstBasicCommands.AddTail (ID_RECORD_NEXT);
	lstBasicCommands.AddTail (ID_RECORD_LAST);
	lstBasicCommands.AddTail (ID_APP_ABOUT);
	lstBasicCommands.AddTail (ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail (ID_VIEW_CUSTOMIZE);

	CBCGPToolBar::SetBasicCommands (lstBasicCommands);

	if (!m_wndMenuBar.Create (this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}
	CBCGPMenuBar::SetShowAllCommands(TRUE);
	CBCGPMenuBar::SetRecentlyUsedMenus(FALSE);
	DWORD barStyle =  m_wndMenuBar.GetBarStyle();
	barStyle &= ~CBRS_GRIPPER;
	barStyle |= CBRS_SIZE_DYNAMIC;
	m_wndMenuBar.SetBarStyle(barStyle);

	// Detect color depth. 256 color toolbars can be used in the
	// high or true color modes only (bits per pixel is > 8):
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;

	UINT uiToolbarHotID = bIsHighColor ? IDB_TOOLBAR256 : 0;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, 0, 0, FALSE, 0, 0, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = -MulDiv(9, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);//9号字
	_tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("宋体"));
	VERIFY(m_font.CreateFontIndirect(&lf));
	m_wndStatusBar.SetFont(&m_font);

	int width = 0;
	UINT nID, nStyle;
	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, width);
	ASSERT(ID_INDICATOR_CURSTATE == nID);
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, width + 110);

	m_wndStatusBar.GetPaneInfo(2, nID, nStyle, width);
	ASSERT(ID_INDICATOR_SHOOTTYPE == nID);
	m_wndStatusBar.SetPaneInfo(2, nID, nStyle, width + 120);

	m_wndStatusBar.GetPaneInfo(3, nID, nStyle, width);
	ASSERT(ID_INDICATOR_SHOOTMODE == nID);
	m_wndStatusBar.SetPaneInfo(3, nID, nStyle, width + 140);

	m_wndStatusBar.GetPaneInfo(4, nID, nStyle, width);
	ASSERT(ID_INDICATOR_DRAGOPT == nID);
	m_wndStatusBar.SetPaneInfo(4, nID, nStyle, width + 120);

	m_wndStatusBar.GetPaneInfo(5, nID, nStyle, width);
	ASSERT(ID_INDICATOR_DRVMODE == nID);
	m_wndStatusBar.SetPaneInfo(5, nID, nStyle, width + 120);

	m_miiMgr.InitImage(IDB_WORKSPACE);
	m_miiMgr.AddItemMap(ID_EDIT_ADDSTATICOBJ, 1);

	// Load control bar icons:
	CBCGPToolBarImages imagesWorkspace;
	imagesWorkspace.SetImageSize (CSize (16, 16));
	imagesWorkspace.SetTransparentColor (RGB (255, 0, 255));
	imagesWorkspace.Load (IDB_WORKSPACE);
	
	if (!m_wndWSBCL.Create (_T("分类显示"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_WSB_CLASSIFIED,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar\n");
		return -1;      // fail to create
	}
	m_wndWSBCL.SetIcon (imagesWorkspace.ExtractIcon (2), FALSE);

	if (!m_wndWSBPC.Create (_T("父子显示"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_WSB_PARENTCHILD,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		a_LogOutput(1,"Failed to create Workspace bar\n");
		return -1;      // fail to create
	}
	m_wndWSBPC.SetIcon (imagesWorkspace.ExtractIcon (0), FALSE);


	if (!m_wndPropsWnd.Create (_T("属性"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_PROPERTIES,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar 2\n");
		return -1;      // fail to create
	}

	m_wndPropsWnd.SetIcon (imagesWorkspace.ExtractIcon (2), FALSE);


	if (!m_wndOutput.Create (_T("输出信息"), this, CSize (150, 150),
		TRUE /* Has gripper */, ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM))
	{
		TRACE0("Failed to create output bar\n");
		return -1;      // fail to create
	}

	CString strMainToolbarTitle;
	strMainToolbarTitle.LoadString (IDS_MAIN_TOOLBAR);
	m_wndToolBar.SetWindowText (strMainToolbarTitle);

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_TOP);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndWSBCL.EnableDocking(CBRS_ALIGN_ANY);
	m_wndWSBPC.EnableDocking(CBRS_ALIGN_ANY);
	m_wndPropsWnd.EnableDocking(CBRS_ALIGN_ANY);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMenuBar);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndWSBCL);
	DockControlBar(&m_wndPropsWnd, AFX_IDW_DOCKBAR_RIGHT);
	DockControlBar(&m_wndOutput);
	m_wndWSBPC.AttachToTabWnd(&m_wndWSBCL, BCGP_DM_STANDARD, FALSE, NULL);

	m_wndToolBar.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));

	// Allow user-defined toolbars operations:
	InitUserToobars (NULL,
					uiFirstUserToolBarId,
					uiLastUserToolBarId);

	// Enable control bar context menu (list of bars + customize command):
	EnableControlBarMenu (	
		TRUE,				// Enable
		ID_VIEW_CUSTOMIZE, 	// Customize command ID
		_T("Customize..."),	// Customize command text
		ID_VIEW_TOOLBARS);	// Menu items with this ID will be replaced by
							// toolbars menu

	APhysXCommonDNet::MLogWrapper::Instance->SetOutputWnd(LogOutputWnd::Instance);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CBCGPFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGPFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers




void CMainFrame::OnViewCustomize()
{
	//------------------------------------
	// Create a customize toolbars dialog:
	//------------------------------------
	CBCGPToolbarCustomize* pDlgCust = new CBCGPToolbarCustomize (this,
		TRUE /* Automatic menus scaning */);

	pDlgCust->EnableUserDefinedToolbars ();
	pDlgCust->Create ();
}

afx_msg LRESULT CMainFrame::OnToolbarReset(WPARAM /*wp*/,LPARAM)
{
	// TODO: reset toolbar with id = (UINT) wp to its initial state:
	//
	// UINT uiToolBarId = (UINT) wp;
	// if (uiToolBarId == IDR_MAINFRAME)
	// {
	//		do something with m_wndToolBar
	// }

	return 0;
}
 // RIBBON_APP

void CMainFrame::Tick()
{
	if (0 == m_pView)
	{
		m_pView = static_cast<CAPhysXLab31View*>(GetActiveView());
		if (0 == m_pView)
			return;
	}

	if (m_bActive)
	{
		if (m_pView->Tick())
		{
			m_pView->Render();
			m_wndPropsWnd.UpdateProp();
		}
	}
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CBCGPFrameWnd::OnActivateApp(bActive, dwThreadID);

	// TODO: Add your message handler code here
	m_bActive = bActive? true : false;
	m_pView = static_cast<CAPhysXLab31View*>(GetActiveView());
}

CMenu* CMainFrame::GetFileMenuGroup()
{
	CMenu* pRtn = 0;
	CMenu* pMenu = GetMenu();
	if (0 == pMenu)
		pMenu = CMenu::FromHandle(m_wndMenuBar.GetHMenu());

	if (0 != pMenu)
	{
		pRtn = pMenu->GetSubMenu(0);
		if (0 != pRtn)
		{
			CString text;
			pRtn->GetMenuString(m_PopMenuIndex, text, MF_BYPOSITION);
			assert(m_PopMenuName == text);
			if (m_PopMenuName != text)
				pRtn = 0;
		}
	}

	return pRtn;
}

CMenu* CMainFrame::GetRecentMenu()
{
	CMenu* pMFile = GetFileMenuGroup();
	if (0 == pMFile)
		return 0;

	return pMFile->GetSubMenu(m_PopMenuIndex);
}

CString CMainFrame::GetRecentlyFileText(int index)
{
	assert(0 <= index); 
	assert(index < RECENT_FILE_ID_COUNT); 

	CString strTemp;
	// double up any '&' characters so they are not underlined
	LPCTSTR lpszSrc = m_Names[index];
	LPTSTR lpszDest = strTemp.GetBuffer(m_Names[index].GetLength()*2);
	while (*lpszSrc != 0)
	{
		if (*lpszSrc == '&')
			*lpszDest++ = '&';
		if (_istlead(*lpszSrc))
			*lpszDest++ = *lpszSrc++;
		*lpszDest++ = *lpszSrc++;
	}
	*lpszDest = 0;
	strTemp.ReleaseBuffer();

	CString text;
	if (index > 9)
		text.Format(_T("%d %s"), index + 1, strTemp);
	else if (index == 9)
		text.Format(_T("1&0 %s"), strTemp);
	else
		text.Format(_T("&%d %s"), index + 1, strTemp);

	return text;
}

void CMainFrame::AddToRecent(const CString& strName)
{
 	CMenu* pRecent = GetRecentMenu();
	if (0 == pRecent)
		return;

	int nMenuItems = pRecent->GetMenuItemCount();
	if (nMenuItems > RECENT_FILE_ID_COUNT)
	{
		for (int n = RECENT_FILE_ID_COUNT; n < nMenuItems; ++n)
			pRecent->DeleteMenu(n, MF_BYPOSITION);

		nMenuItems = pRecent->GetMenuItemCount();
	}
	assert(nMenuItems <= RECENT_FILE_ID_COUNT);

	if (0 == nMenuItems)
	{
		CMenu* pMFile = GetFileMenuGroup();
		pMFile->EnableMenuItem(m_PopMenuIndex, MF_ENABLED | MF_BYPOSITION);
		m_Names[0] = strName;
		pRecent->InsertMenu(0, MF_BYPOSITION | MF_STRING, RECENT_FILE_ID_BEGIN, GetRecentlyFileText(0));
		m_wndMenuBar.CreateFromMenu(m_wndMenuBar.GetHMenu(), TRUE, TRUE);
		return;
	}

	int i = 0;
	for (; i < nMenuItems; ++i)
	{
		if (strName == m_Names[i])
			break;			
	}
	if (0 == i)
		return;

	if (i < nMenuItems)
		pRecent->DeleteMenu(i, MF_BYPOSITION);
	else
	{
		if (nMenuItems == RECENT_FILE_ID_COUNT)
		{
			pRecent->DeleteMenu(nMenuItems - 1, MF_BYPOSITION);
			i = nMenuItems - 1;
		}
	}
	nMenuItems = pRecent->GetMenuItemCount();
	while(i >= 1)
	{
		m_Names[i] = m_Names[i - 1];
		--i;
	}
	for (int k = 0; k < nMenuItems; ++k)
		pRecent->ModifyMenu(k, MF_BYPOSITION, RECENT_FILE_ID_BEGIN + k + 1, GetRecentlyFileText(k + 1));

	m_Names[0] = strName;
	pRecent->InsertMenu(0, MF_BYPOSITION | MF_STRING, RECENT_FILE_ID_BEGIN, GetRecentlyFileText(0));
	m_wndMenuBar.CreateFromMenu(m_wndMenuBar.GetHMenu(), TRUE, TRUE);
}

void CMainFrame::DelFromRecent(const CString& strName)
{
	CMenu* pRecent = GetRecentMenu();
	if (0 == pRecent)
		return;

	int i = 0;
	for (; i < RECENT_FILE_ID_COUNT; ++i)
	{
		if (strName == m_Names[i])
			break;			
	}

	if (RECENT_FILE_ID_COUNT > i)
	{
		int nMenuItems = pRecent->GetMenuItemCount();
		for (int k = i; k < nMenuItems - 1; ++k)
		{
			m_Names[k] = m_Names[k + 1];
			pRecent->ModifyMenu(k, MF_BYPOSITION, RECENT_FILE_ID_BEGIN + k, GetRecentlyFileText(k));
		}

		m_Names[nMenuItems - 1].Empty(); 
		pRecent->DeleteMenu(nMenuItems - 1, MF_BYPOSITION);
		m_wndMenuBar.CreateFromMenu(m_wndMenuBar.GetHMenu(), TRUE, TRUE);
	}
}

bool CMainFrame::GetRecentString(const UINT nID, CString& outPath, CString* poutTitle) const
{
	int index = nID - RECENT_FILE_ID_BEGIN;
	if (0 > index)
		return false;
	if (RECENT_FILE_ID_COUNT <= index)
		return false;

	outPath = m_Names[index];

	if (0 != poutTitle)
	{
		int left1 = outPath.ReverseFind('/');
		int left2 = outPath.ReverseFind('\\');
		int left = (left1 > left2)? left1 : left2;

		*poutTitle = outPath.Right(outPath.GetLength() - left - 1);
		int right = poutTitle->Find(_T("."));
		*poutTitle = poutTitle->Left(right);
	}
	return true;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle , CWnd* pParentWnd , CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL bRtn = CBCGPFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
	if (bRtn)
	{
		// Recent Menu items maybe different from the data read from config file. Because BCG load all menu state from OS Regedit.
		// So, we have to refresh it from config file data.
		CMenu* pRecent = GetRecentMenu();
		if (0 != pRecent)
		{
			CBCGPToolbarMenuButton* pRecentItem = 0;
			CBCGPToolbarMenuButton* pFileMenu = DYNAMIC_DOWNCAST(CBCGPToolbarMenuButton, m_wndMenuBar.GetButton(0));

			const CObList& lstCmds = pFileMenu->GetCommands();
			POSITION pos = lstCmds.GetHeadPosition();
			for (int i = 0; i < m_PopMenuIndex; ++i)
				pRecentItem = (CBCGPToolbarMenuButton*)lstCmds.GetNext(pos);
			pRecentItem = (CBCGPToolbarMenuButton*)lstCmds.GetNext(pos);
			pRecentItem->CreateFromMenu(pRecent->GetSafeHmenu());
		}
	}
	return bRtn;
}
