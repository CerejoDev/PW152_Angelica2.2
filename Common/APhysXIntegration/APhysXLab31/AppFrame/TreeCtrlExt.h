///////////////////////////////////////////////////////////////////////////////
/*
* FILE: TreeCtrlExt.h
*
* DESCRIPTION: 
*	CTreeCtrlExt - Multiple selection tree control for MFC
*	operation:
*		select an item: left button down
*		append an item: CTRL + left button down
*		remove an item: CTRL + right button down
*		delete selected items: 'delete' key down
*
* CREATED BY: Yang Liu, 2011/03/22
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _TREECTRLEXT_H_
#define _TREECTRLEXT_H_

#define TVGN_EXT_NEXT_FOLLOWING			0x000F

class CTreeCtrlExt : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlExt)
public:
	CTreeCtrlExt() {}

// Attributes
public:
	bool HandledRBtnDownByDefault() const { return m_bDefRBtnDown; }
	HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode) const;

private:
	bool m_bDefRBtnDown;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlExt)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeCtrlExt() {}

private:
	void ClearItemOperation(HTREEITEM hti) const;
	void SendSelChangedNotifyToParent(HTREEITEM htiNew, const CPoint& point, bool isFromMouse) const;

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeCtrlExt)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#endif
