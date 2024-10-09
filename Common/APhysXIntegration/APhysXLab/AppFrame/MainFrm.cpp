// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "APhysXLab.h"

#include "MainFrm.h"
#include "APhysXLabView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_ENTERIDLE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
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

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
const int CMainFrame::RecentMenuIndex = 5;

CMainFrame::CMainFrame() : m_IDBase(60000), m_IDCount(5), m_MenuName("×î½üÎÄµµ"),  m_ConfigName("Configs\\APhysXLabConfig.ini")
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
	CFrameWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	CMenu* pRecent = GetRecentMenu();
	if (0 == pRecent)
		return;
	
	WritePrivateProfileString(_T("Recent File"), 0, 0, m_ConfigName);
	int MaxBound = pRecent->GetMenuItemCount();
	ACString str;
	str.Format(_T("%d"), MaxBound);
	WritePrivateProfileString(_T("Recent File"), _T("Count"), str, m_ConfigName);
	int index = 0;
	for (int i = 0; i < MaxBound; ++i)
	{
		str.Format(_T("Item%d"), i);
		WritePrivateProfileString(_T("Recent File"), str, m_Names[i], m_ConfigName);
	}

	WritePrivateProfileString(_T("BindFF"), _T("Name"), m_FFbindingToMA, m_ConfigName);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
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
	int width = 0;
	UINT nID, nStyle;
	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, width);
	ASSERT(ID_INDICATOR_CURSTATE == nID);
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle, width + 40);

	m_wndStatusBar.GetPaneInfo(2, nID, nStyle, width);
	ASSERT(ID_INDICATOR_SHOOTTYPE == nID);
	m_wndStatusBar.SetPaneInfo(2, nID, nStyle, width + 50);

	m_wndStatusBar.GetPaneInfo(3, nID, nStyle, width);
	ASSERT(ID_INDICATOR_SHOOTMODE == nID);
	m_wndStatusBar.SetPaneInfo(3, nID, nStyle, width + 75);

	m_wndStatusBar.GetPaneInfo(4, nID, nStyle, width);
	ASSERT(ID_INDICATOR_DRAGOPT == nID);
	m_wndStatusBar.SetPaneInfo(4, nID, nStyle, width + 60);

	m_wndStatusBar.GetPaneInfo(5, nID, nStyle, width);
	ASSERT(ID_INDICATOR_DRVMODE == nID);
	m_wndStatusBar.SetPaneInfo(5, nID, nStyle, width + 60);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	
	CMenu* pMenu = GetMenu();
	if (0 != pMenu)
	{
		CMenu* pMFile = pMenu->GetSubMenu(0);
		if (0 != pMFile)
		{
			pMFile->InsertMenu(RecentMenuIndex, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT)pMFile->GetSafeHmenu(), m_MenuName);
			CMenu* pRecent = pMFile->GetSubMenu(RecentMenuIndex);
			
			AIniFile cfgINI;
			if (cfgINI.Open(_TWC2AS(m_ConfigName)))
			{
				int nCount = cfgINI.GetValueAsInt("Recent File", "Count", m_IDCount);
				if (m_IDCount < nCount)
					nCount = m_IDCount;

				int index = 0;
				AString str;
				for (int i = 0; i < nCount; ++i)
				{
					str.Format("Item%d", i);
					m_Names[index] = cfgINI.GetValueAsString("Recent File", str);
					if (!m_Names[index].IsEmpty())
					{
						pRecent->InsertMenu(index, MF_BYPOSITION | MF_STRING, m_IDBase + index, m_Names[index]);
						++index;
					}
				}
				
				m_FFbindingToMA = cfgINI.GetValueAsString("BindFF", "Name");
				cfgINI.Close();
			}
		}
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::Tick()
{
	ASSERT(m_pView);

	static DWORD dwLastFrame = 0;
	DWORD dwTickTime = dwLastFrame ? a_GetTime() - dwLastFrame : 1;
	
	if (dwTickTime)
	{
		dwLastFrame = a_GetTime();
		
		a_ClampRoof(dwTickTime, DWORD(100)); //	Limit time of a logic tick
		if (m_pView->Tick(dwTickTime))
			m_pView->Render();
	}
}

void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
	
	// TODO: Add your message handler code here
	m_bActive = bActive? true : false;

	m_pView = static_cast<CAPhysXLabView*>(GetActiveView());
	ASSERT(m_pView);
}

void CMainFrame::OnEnterIdle(UINT nWhy, CWnd* pWho) 
{
	CFrameWnd::OnEnterIdle(nWhy, pWho);
	
	// TODO: Add your message handler code here
	Tick();
}

CMenu* CMainFrame::GetRecentMenu()
{
	CMenu* pRtn = 0;

	CMenu* pMenu = GetMenu();
	if (0 != pMenu)
	{
		CMenu* pMFile = pMenu->GetSubMenu(0);
		if (0 != pMFile)
		{
			pRtn = pMFile->GetSubMenu(RecentMenuIndex);
			if (0 != pRtn)
			{
				CString text;
				pMFile->GetMenuString(RecentMenuIndex, text, MF_BYPOSITION);
				assert(m_MenuName == text);
				if (m_MenuName != text)
					pRtn = 0;
			}
		}
	}

	return pRtn;
}

void CMainFrame::AddToRecent(const CString& strName)
{
	CMenu* pRecent = GetRecentMenu();
	if (0 == pRecent)
		return;

	int MaxBound = pRecent->GetMenuItemCount();
	if (MaxBound > m_IDCount)
	{
		for (int j = m_IDCount; j < MaxBound; ++j)
			pRecent->DeleteMenu(m_IDCount, MF_BYPOSITION);

		MaxBound = pRecent->GetMenuItemCount();
	}
	assert(MaxBound <= m_IDCount);

	if (0 == MaxBound)
	{
		m_Names[0] = strName;
		pRecent->InsertMenu(0, MF_BYPOSITION | MF_STRING, m_IDBase, m_Names[0]);
		return;
	}

	int i = 0;
	for (; i < MaxBound; ++i)
	{
		if (strName == m_Names[i])
			break;			
	}

	if (0 == i)
		return;

	if (m_IDCount == i)
		pRecent->DeleteMenu(m_IDCount - 1, MF_BYPOSITION);
	else
		pRecent->DeleteMenu(i, MF_BYPOSITION);

	MaxBound = pRecent->GetMenuItemCount();
	for (int k = 0; k < MaxBound; ++k)
	{
		pRecent->GetMenuString(k, m_Names[k + 1], MF_BYPOSITION);
		pRecent->ModifyMenu(k, MF_BYPOSITION, m_IDBase + k + 1, m_Names[k + 1]);
	}

	m_Names[0] = strName;
	pRecent->InsertMenu(0, MF_BYPOSITION | MF_STRING, m_IDBase, m_Names[0]);
}

void CMainFrame::DelFromRecent(const CString& strName)
{
	CMenu* pRecent = GetRecentMenu();
	if (0 == pRecent)
		return;

	for (int i = 0; i < m_IDCount; ++i)
	{
		if (strName == m_Names[i])
			break;			
	}

	if (m_IDCount > i)
	{
		pRecent->DeleteMenu(i, MF_BYPOSITION);

		int MaxBound = pRecent->GetMenuItemCount();
		for (int k = i; k < MaxBound; ++k)
		{
			m_Names[k] = m_Names[k + 1];
			pRecent->ModifyMenu(k, MF_BYPOSITION, m_IDBase + k, m_Names[k]);
		}
		m_Names[m_IDCount - 1].Empty(); 
	}
}

bool CMainFrame::GetRecentString(const UINT nID, CString& outPath, CString* poutTitle) const
{
	int index = nID - m_IDBase;
	if (0 > index)
		return false;
	if (m_IDCount <= index)
		return false;

	outPath = m_Names[index];

	if (0 != poutTitle)
	{
		int left1 = outPath.ReverseFind('/');
		int left2 = outPath.ReverseFind('\\');
		int left = (left1 > left2)? left1 : left2;
		
		*poutTitle = outPath.Right(outPath.GetLength() - left);
		int right = poutTitle->Find(_T("."));
		*poutTitle = poutTitle->Left(right);
	}
	return true;
}
