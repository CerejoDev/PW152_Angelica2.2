// PropsWindow.cpp : implementation of the CPropsWindow class
//

#include "stdafx.h"
#include "APhysXLab31.h"
#include "PropsWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 1;

/////////////////////////////////////////////////////////////////////////////
// CPropsWindow

BEGIN_MESSAGE_MAP(CPropsWindow, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CPropsWindow)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropsWindow construction/destruction

CPropsWindow::CPropsWindow()
{
	// TODO: add one-time construction code here
	m_pCmdRecorder = 0;
	m_pObjSelGroup = 0;
}

CPropsWindow::~CPropsWindow()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPropsWindow message handlers

int CPropsWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();
	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, rectDummy, this, 0xffffffff))
	{
		TRACE0("Failed to create property list\n");
		return -1;      // fail to create
	}

	m_wndPropList.EnableHeaderCtrl (FALSE);
	m_wndPropList.EnableDesciptionArea ();
	m_wndPropList.SetVSDotNetLook ();
	m_wndPropList.SetScrollBarsStyle (CBCGPScrollBar::BCGP_SBSTYLE_VISUAL_MANAGER);
	m_wndPropList.SetDescriptionRows(4);
	return 0;
}

void CPropsWindow::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	// Tab control should cover a whole client area:
	m_wndPropList.SetWindowPos (NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropsWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndPropList.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree,	::GetSysColor (COLOR_3DSHADOW), 
		::GetSysColor (COLOR_3DSHADOW));
}

void CPropsWindow::SetCmdQueue(CmdQueue& cmdQueue)
{
	m_pCmdRecorder = &cmdQueue;
}

void CPropsWindow::SetSelGroup(CPhysXObjSelGroup& selGroup)
{
	m_pObjSelGroup = &selGroup;
	m_pObjSelGroup->SetPropList(m_wndPropList);
}

void CPropsWindow::UpdateProp()
{
	assert(0 != m_pObjSelGroup);
	m_pObjSelGroup->UpdatePropItems();
}

afx_msg LRESULT CPropsWindow::OnPropertyChanged(WPARAM wParam, LPARAM lParam) 
{ 
	CBCGPProp* pProp = reinterpret_cast<CBCGPProp*>(lParam);
	if (0 == pProp)
		return 0;

	assert(0 != m_pObjSelGroup);
	if ((0 != m_pCmdRecorder) && !m_pObjSelGroup->IsRunTime())
		m_pCmdRecorder->SubmitBeforeExecution(CmdPropChange());
	m_pObjSelGroup->PropertyChanged(*pProp);
	return 0;
} 
