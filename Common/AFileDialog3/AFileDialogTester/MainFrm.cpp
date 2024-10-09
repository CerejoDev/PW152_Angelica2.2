// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AFileDialogDevTester.h"
#include <windowsx.h>

#include "MainFrm.h"
#include "PreviewWnd.h"
#include "AImagePreviewWrapper.h"

//#ifdef _DEBUG
//#ifdef _PROJ_IN_ANGELICA_3
//// In DEBUG Mode, we need the vld's help
//
//__declspec(dllimport) int VLD_LOADER_VER;
//
//struct GOBJ_
//{
//	GOBJ_()
//	{
//		int x = VLD_LOADER_VER;
//	}
//};
//
//static GOBJ_ obj_;
//
//#endif
//#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef _PROJ_IN_ANGELICA_3

CPreviewWnd* g_pPreviewWnd;

class MyFrameRenderListener : public AImagePreviewWrapper::Listener
{
public:
	virtual void OneFrameRendered(DWORD * pdwBuffer, DWORD dwWndSize);
};

void MyFrameRenderListener::OneFrameRendered(DWORD * pdwBuffer, DWORD dwWndSize)
{
	if (!g_pPreviewWnd || !g_pPreviewWnd->GetSafeHwnd() || !IsWindow(g_pPreviewWnd->GetSafeHwnd()))
		return;

	g_pPreviewWnd->m_dwSize = dwWndSize;
	memcpy(g_pPreviewWnd->m_dwBuffer, pdwBuffer, dwWndSize * dwWndSize * 4);
	g_pPreviewWnd->Invalidate();
}

MyFrameRenderListener s_listener;

#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_pPreviewWnd = new CPreviewWnd;	
}

CMainFrame::~CMainFrame()
{
	delete m_pPreviewWnd;
	m_pPreviewWnd = NULL;

}

static BOOL RegisterMyWndClass(HINSTANCE hInstance, const TCHAR* szClassName)
{
	WNDCLASSEX wcx;
	ZeroMemory(&wcx, sizeof(wcx));

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW | 
		CS_VREDRAW;                    // redraw if size changes 
	wcx.lpfnWndProc = DefWindowProc;     // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = hInstance;         // handle to instance 
	wcx.hIcon = LoadIcon(NULL, 
		IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, 
		IDC_ARROW);                    // predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject( 
		WHITE_BRUSH);                  // white background brush 
	wcx.lpszMenuName =  NULL;			// name of menu resource 
	wcx.lpszClassName = szClassName;  // name of window class 
	wcx.hIconSm = NULL;

	return RegisterClassEx(&wcx);
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

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	
	if (!m_pPreviewWnd->CreateEx(0, AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW), _T("Preview wnd"), WS_VISIBLE | WS_CAPTION, CRect(0, 0, 300, 300), NULL, 0))
	{
		TRACE0("Failed to create preview wnd");
		return -1;
	}

	CRect rcRequire(0, 0, 256, 256);
	AdjustWindowRect(&rcRequire, GetWindowStyle(m_pPreviewWnd->GetSafeHwnd()), FALSE);
	m_pPreviewWnd->MoveWindow(&rcRequire);

#ifdef _PROJ_IN_ANGELICA_3

	g_pPreviewWnd = m_pPreviewWnd;
	AImagePreviewWrapper::GetInstance()->SetListener(&s_listener);

#endif

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


BOOL CMainFrame::DestroyWindow()
{
#ifdef _PROJ_IN_ANGELICA_3

	AImagePreviewWrapper::GetInstance()->SetListener(NULL);

#endif
	return CFrameWnd::DestroyWindow();
}