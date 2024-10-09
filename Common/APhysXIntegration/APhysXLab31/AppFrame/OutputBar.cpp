// outputbar.cpp : implementation of the COutputBar class
//

#include "stdafx.h"
#include "APhysXLab31.h"
#include "outputbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 1;
/////////////////////////////////////////////////////////////////////////////
// COutputBar

BEGIN_MESSAGE_MAP(COutputBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(COutputBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputBar construction/destruction

COutputBar::COutputBar()
{
	// TODO: add one-time construction code here
}

COutputBar::~COutputBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// COutputBar message handlers

int COutputBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create list window.
	// TODO: create your own window here:
	const DWORD dwViewStyle =	
		LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY
		| ES_WANTRETURN | ES_AUTOVSCROLL | WS_VSCROLL | ES_NOHIDESEL;

	if (!m_wndEdit.Create (dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create output view\n");
		return -1;      // fail to create
	}

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = -MulDiv(9, GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSY), 72);//9ºÅ×Ö
	_tcscpy_s(lf.lfFaceName, _T("ËÎÌå"));
	VERIFY(m_font.CreateFontIndirect(&lf));
	m_wndEdit.SetFont(&m_font);

	return 0;
}

void COutputBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	m_wndEdit.SetWindowPos (NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectList;
	m_wndEdit.GetWindowRect (rectList);
	ScreenToClient (rectList);

	rectList.InflateRect (1, 1);
	dc.Draw3dRect (rectList,	::GetSysColor (COLOR_3DSHADOW), 
								::GetSysColor (COLOR_3DSHADOW));
}

void COutputBar::OutputText(const char* szMsg)
{
	AddText(CString(szMsg), 0);
}

void COutputBar::AddText(const CString& strInfo, DWORD color)
{
	CHARFORMAT cf = {0};
	cf.cbSize = sizeof(CHARFORMAT);   
	cf.dwMask = CFM_COLOR;   
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	cf.crTextColor = color;

	SYSTEMTIME t;
	GetLocalTime(&t);
	CString strText;
	strText.Format(_T("[%2.2d:%2.2d:%2.2d.%3.3d]  "), t.wHour, t.wMinute, t.wSecond, t.wMilliseconds);
	strText += strInfo;

	static const CString strEnd(_T("text\n"));
	CString strLastChar1 = strText.Right(1);
	CString strLastChar2 = strEnd.Right(1);
	if (strLastChar1 != strLastChar2)
		strText += strLastChar2;

	m_wndEdit.SetSel(-1,-1);
	int nStart = m_wndEdit.GetTextLengthEx(GTL_DEFAULT);
	m_wndEdit.ReplaceSel(strText);
	int nEnd = m_wndEdit.GetTextLengthEx(GTL_DEFAULT);
	m_wndEdit.SetSel(nStart, nEnd);
	m_wndEdit.SetSelectionCharFormat(cf);
	m_wndEdit.HideSelection(TRUE, FALSE);
	m_wndEdit.LineScroll(1);
}

void COutputBar::Clear()
{
	m_wndEdit.SetSel(0, -1);
	m_wndEdit.ReplaceSel(_T(""));
}
