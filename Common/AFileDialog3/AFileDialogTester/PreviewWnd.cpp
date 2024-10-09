// PreviewWnd.cpp : implementation file
//

#include "stdafx.h"
#include "PreviewWnd.h"
#include <A3DMacros.h>


// CPreviewWnd

IMPLEMENT_DYNAMIC(CPreviewWnd, CWnd)

CPreviewWnd::CPreviewWnd()
{
	m_dwSize = 256;
	memset(m_dwBuffer, 0, m_dwSize * m_dwSize *4);
}

CPreviewWnd::~CPreviewWnd()
{
}


BEGIN_MESSAGE_MAP(CPreviewWnd, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CPreviewWnd message handlers



void CPreviewWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages

	CDC memDc;
	memDc.CreateCompatibleDC(&dc);

	CBitmap bm;
	bm.CreateCompatibleBitmap(&dc, m_dwSize, m_dwSize);
	
	memDc.SelectObject(&bm);

	bm.SetBitmapBits(m_dwSize * m_dwSize * 4, m_dwBuffer);

	CRect rcClient;
	GetClientRect(&rcClient);
	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDc, 0, 0, SRCCOPY);
}
