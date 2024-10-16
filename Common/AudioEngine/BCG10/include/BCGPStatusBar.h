#if !defined(AFX_BCGPSTATUSBAR_H__714C592C_683E_4DB9_A8E2_FCDBC00A4BCC__INCLUDED_)
#define AFX_BCGPSTATUSBAR_H__714C592C_683E_4DB9_A8E2_FCDBC00A4BCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This is a part of the BCGControlBar Library
// Copyright (C) 1998-2008 BCGSoft Ltd.
// All rights reserved.
//
// This source code can be used, distributed or modified
// only under terms and conditions 
// of the accompanying license agreement.
//*******************************************************************************
// BCGPStatusBar.h : header file
//

#include "BCGCBPro.h"
#include "BCGPControlBar.h"

/////////////////////////////////////////////////////////////////////////////
class BCGCBPRODLLEXPORT CBCGStatusBarPaneInfo
{
public:	

	RECT		rect;			// pane rectangle

	UINT		nID;			// IDC of indicator: 0 => normal text area
	UINT		nStyle;			// style flags (SBPS_*)
	int			cxText;			// width of string area in pixels
								//		on both sides there is a 1 pixel gap and
								//		a one pixel border, making a pane 4 
								//		pixels wider
	COLORREF	clrText;		// text color
	COLORREF	clrBackground;	// background color
	int			cxIcon;			// width of icon area
	int			cyIcon;			// height of icon area
	LPCTSTR		lpszText;		// text in the pane
	LPCTSTR		lpszToolTip;	// pane tooltip
	HIMAGELIST	hImage;			// pane icon or animation

	// Animation parameters
	int			nFrameCount;	// Number of animation frames
	int			nCurrFrame;		// Current frame

	// Progress bar properties
	long		nProgressCurr;	// Current progress value
	long		nProgressTotal;	// Total progress value	(-1 - no progress bar)
	BOOL		bProgressText;	// Display text: "x%"
	COLORREF	clrProgressBar;
	COLORREF	clrProgressBarDest;
	COLORREF	clrProgressText;

	CBCGStatusBarPaneInfo ()
	{
		nID = 0;
		nStyle = 0;
		lpszText = NULL;
		lpszToolTip = NULL;
		clrText = (COLORREF)-1;
		clrBackground = (COLORREF)-1;
		hImage = NULL;
		cxIcon = 0;
		cyIcon = 0;
		rect = CRect (0, 0, 0, 0);
		nFrameCount = 0;
		nCurrFrame = 0;
		nProgressCurr = 0;
		nProgressTotal = -1;
		clrProgressBar = (COLORREF)-1;
		clrProgressBarDest = (COLORREF)-1;
		clrProgressText = (COLORREF)-1;
		bProgressText = FALSE;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CBCGPStatusBar window

class BCGCBPRODLLEXPORT CBCGPStatusBar : public CBCGPControlBar
{
	DECLARE_DYNAMIC(CBCGPStatusBar)

// Construction
public:
	CBCGPStatusBar();
	BOOL Create(CWnd* pParentWnd,
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
			UINT nID = AFX_IDW_STATUS_BAR);
	BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = 0,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
		UINT nID = AFX_IDW_STATUS_BAR);
	BOOL SetIndicators(const UINT* lpIDArray, int nIDCount);

// Attributes
public: 
	virtual BOOL DoesAllowDynInsertBefore () const {return FALSE;}
	// standard control bar things
	int CommandToIndex(UINT nIDFind) const;
	UINT GetItemID(int nIndex) const;
	void GetItemRect(int nIndex, LPRECT lpRect) const;

	// specific to CStatusBar
	void GetPaneText(int nIndex, CString& s) const;
	CString GetPaneText(int nIndex) const;
	
	virtual BOOL SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE);

	void GetPaneInfo(int nIndex, UINT& nID, UINT& nStyle, int& cxWidth) const;
	void SetPaneInfo(int nIndex, UINT nID, UINT nStyle, int cxWidth);
	UINT GetPaneStyle(int nIndex) const;
	void SetPaneStyle(int nIndex, UINT nStyle);
	CString GetTipText(int nIndex) const;
	void SetTipText(int nIndex, LPCTSTR pszTipText);

	int GetCount () const	{	return m_nCount;	}

	// specific to CBCGPStatusBar
	int GetPaneWidth (int nIndex) const;
	void SetPaneWidth (int nIndex, int cx);

	void SetPaneIcon (int nIndex, HICON hIcon, BOOL bUpdate = TRUE, BOOL bAlphaBlend = FALSE);
	void SetPaneIcon (int nIndex, HBITMAP hBmp, 
					COLORREF clrTransparent = RGB (255, 0, 255), BOOL bUpdate = TRUE, BOOL bAlphaBlend = FALSE);
	void SetPaneAnimation (int nIndex, HIMAGELIST hImageList, 
							UINT nFrameRate = 500 /* ms */, BOOL bUpdate = TRUE);
	void SetPaneTextColor (int nIndex, 
		COLORREF clrText = (COLORREF)-1 /* Default */, BOOL bUpdate = TRUE);
	void SetPaneBackgroundColor (int nIndex, 
		COLORREF clrBackground = (COLORREF)-1 /* Default */, BOOL bUpdate = TRUE);

	void EnablePaneProgressBar (int nIndex, long nTotal = 100 /* -1 - disable */,
								BOOL bDisplayText = FALSE, /* display "x%" */
								COLORREF clrBar = -1, COLORREF clrBarDest = -1 /* for gradient fill */,
								COLORREF clrProgressText = -1);
	void SetPaneProgress (int nIndex, long nCurr, BOOL bUpdate = TRUE);
	long GetPaneProgress (int nIndex) const;

	void InvalidatePaneContent (int nIndex);

	void EnablePaneDoubleClick (BOOL bEnable = TRUE);

	virtual BOOL GetExtendedArea (CRect& rect) const;
	void SetDrawExtendedArea (BOOL bSet = TRUE)
	{
		m_bDrawExtendedArea = bSet;
	}

	BOOL GetDrawExtendedArea () const
	{
		return m_bDrawExtendedArea;
	}

// Implementation
public:
	virtual ~CBCGPStatusBar();
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	HFONT			m_hFont;
	int				m_cxSizeBox;        // style size box in corner
	BOOL			m_bPaneDoubleClick;
	CRect			m_rectSizeBox;
	BOOL			m_bDrawExtendedArea;

	inline CBCGStatusBarPaneInfo* _GetPanePtr(int nIndex) const;
	void RecalcLayout ();
	CBCGStatusBarPaneInfo* HitTest (CPoint pt) const;

	HFONT GetCurrentFont () const;

// Overrides:
protected:
	virtual void OnDrawPane (CDC* pDC, CBCGStatusBarPaneInfo* pPane);
	virtual void DoPaint(CDC* pDC);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;

	virtual BOOL AllowShowOnControlMenu () const			{	return FALSE;	}
	virtual BOOL HideInPrintPreviewMode () const			{	return FALSE;	}

	//{{AFX_MSG(CBCGPStatusBar)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnStyleChanged(WPARAM wp, LPARAM lp);
	afx_msg BCGNcHitTestType OnNcHitTest(CPoint point);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BCGPSTATUSBAR_H__714C592C_683E_4DB9_A8E2_FCDBC00A4BCC__INCLUDED_)
