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

struct CItemData;
#define TVGN_EXT_NEXT_FOLLOWING			0x000F

class IDragDropEvent
{
public:
	virtual bool DragDropBegin(HTREEITEM htiDragItem) = 0;
	virtual bool DragDropMoving(HTREEITEM htiDragItem, HTREEITEM htiDropTarget) = 0;
	virtual void DragDropEnd(HTREEITEM htiDragItem, HTREEITEM htiDropTarget) = 0;
};

class CTreeCtrlExt : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlExt)
public:
	CTreeCtrlExt();

// Attributes
public:
	void SetDragDropEvent(IDragDropEvent* pEvent) { m_pEvent = pEvent; }
	bool HandledRBtnDownByDefault() const { return m_bDefRBtnDown; }
	HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode) const;

private:
	bool m_bDefRBtnDown;

	bool          m_bDragging;
	DWORD		  m_dwLBtnDown;
	CImageList*   m_pDragImage;
	HTREEITEM     m_htiDSource;
	IDragDropEvent* m_pEvent;

	DWORD		  m_dwHoverTimer; // workaround method 
	UINT_PTR      m_nHoverTimerID;
	HTREEITEM     m_htiHoverItem;
	UINT_PTR      m_nScrollTimerID;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeCtrlExt)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeCtrlExt();

private:
	void CustomDrawItemPrepaint(HTREEITEM htiItem, CItemData& itemData, NMTVCUSTOMDRAW& NMTVCD) const;
	void ClearItemOperation(HTREEITEM hti) const;
	void SendSelChangedNotifyToParent(HTREEITEM htiNew, const CPoint& point, bool isFromMouse) const;
	void SetDropTarget(HTREEITEM hItem);

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
	afx_msg void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
};

#endif
