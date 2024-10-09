/*
* FILE: TreeCtrlExt.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/03/22
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#include "stdafx.h"
#include "TreeCtrlExt.h"
#include "TreeCtrlItemData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTreeCtrlExt, CTreeCtrl)
	//{{AFX_MSG_MAP(CTreeCtrlExt)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CTreeCtrlExt::OnNMCustomdraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CTreeCtrlExt::OnBeginDrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CTreeCtrlExt, CTreeCtrl)

CTreeCtrlExt::CTreeCtrlExt()
{
	m_bDefRBtnDown = true;
	m_bDragging  = false;
	m_pDragImage = 0;
	m_htiDSource = 0;
	m_pEvent = 0;

	m_nHoverTimerID = 0;
	m_htiHoverItem = 0;
	m_nScrollTimerID = 0;
}

CTreeCtrlExt::~CTreeCtrlExt()
{
	if(0 != m_nHoverTimerID)
	{
		KillTimer(m_nHoverTimerID);
		m_nHoverTimerID = 0;
	}
	if(0 != m_nScrollTimerID)
	{
		KillTimer(m_nScrollTimerID);
		m_nScrollTimerID = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrlEx message handlers

HTREEITEM CTreeCtrlExt::GetNextItem(HTREEITEM hItem, UINT nCode) const
{
	if (0 == hItem)
		return hItem;

	if (TVGN_EXT_NEXT_FOLLOWING != nCode)
		return CTreeCtrl::GetNextItem(hItem, nCode);

	// This special code lets us iterate through ALL tree items regardless 
	// of their parent/child relationship (very handy)
	HTREEITEM htiNextItem = 0;
	htiNextItem = GetChildItem(hItem);
	if (0 != htiNextItem)
		return htiNextItem;

	htiNextItem = GetNextSiblingItem(hItem);
	if (0 != htiNextItem)
		return htiNextItem;

	HTREEITEM htiParent = hItem;
	while (0 == htiNextItem)
	{
		htiParent = GetParentItem(htiParent);
		if (0 == htiParent)
			break;
		htiNextItem = GetNextSiblingItem(htiParent);
	}
	return htiNextItem;
}

void CTreeCtrlExt::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	LPNMTVCUSTOMDRAW pNMTVCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	switch (pNMTVCD->nmcd.dwDrawStage)
	{ 
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		{
			HTREEITEM htiTemp = reinterpret_cast<HTREEITEM>(pNMTVCD->nmcd.dwItemSpec);
			DWORD_PTR pPTR = GetItemData(htiTemp);
			CItemData* pID = reinterpret_cast<CItemData*>(pPTR);
			if (0 != pID)
				CustomDrawItemPrepaint(htiTemp, *pID, *pNMTVCD);
		}
		*pResult = CDRF_NOTIFYPOSTPAINT;
		break;
	case CDDS_ITEMPOSTPAINT:
		{
			HTREEITEM htiTemp = reinterpret_cast<HTREEITEM>(pNMTVCD->nmcd.dwItemSpec);
			DWORD_PTR pPTR = GetItemData(htiTemp);
			CItemData* pID = reinterpret_cast<CItemData*>(pPTR);
			if (0 != pID)
			{
				if (pID->m_itemFlags.ReadFlag(IF_MULTI_SELFOCUS))
				{
					static const TCHAR* pText = _T("*");
					static size_t len = _tcsclen(pText);
					SIZE rgn;
					::GetTextExtentPoint32(pNMTVCD->nmcd.hdc, pText, int(len), &rgn);

					RECT rc;
					TreeView_GetItemRect(pNMTVCD->nmcd.hdr.hwndFrom, (HTREEITEM)pNMTVCD->nmcd.dwItemSpec, &rc, 1);
					rc.left = rc.right + 5;
					rc.right = rc.left + rgn.cx;
					DrawText(pNMTVCD->nmcd.hdc, pText, -1, &rc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
				}
			}
		}
		break;
	}
}

void CTreeCtrlExt::CustomDrawItemPrepaint(HTREEITEM htiItem, CItemData& itemData, NMTVCUSTOMDRAW& NMTVCD) const
{
	bool bIsBTNColor = false;
	if (itemData.m_itemFlags.ReadFlag(IF_MULTI_SELECTED))
	{
		if (m_bDragging)
		{
			if (htiItem != GetDropHilightItem())
			{
				NMTVCD.clrTextBk = GetSysColor(COLOR_BTNFACE);
				bIsBTNColor = true;
			}
		}
		else 
		{
			if (CDIS_FOCUS != (NMTVCD.nmcd.uItemState & CDIS_FOCUS))
			{
				NMTVCD.clrTextBk = GetSysColor(COLOR_BTNFACE);
				bIsBTNColor = true;
			}
		}
	}
	else
	{
		if (NMTVCD.clrTextBk == GetSysColor(COLOR_BTNFACE))
			NMTVCD.clrTextBk = GetSysColor(COLOR_WINDOW);
	}

	if (itemData.m_itemFlags.ReadFlag(IF_TEXT_COLOR))
		NMTVCD.clrText = itemData.m_itemTextColor;
	else if (bIsBTNColor)
	{
		if (NMTVCD.clrText == GetSysColor(COLOR_HIGHLIGHTTEXT))
			NMTVCD.clrText = GetSysColor(COLOR_MENUTEXT);
	}
}

void CTreeCtrlExt::ClearItemOperation(HTREEITEM hti) const
{
	if (0 == hti)
		return;

	DWORD_PTR ptr = GetItemData(hti);
	CItemData* pID = reinterpret_cast<CItemData*>(ptr);
	if (0 == pID)
		return;

	if (pID->m_itemFlags.ReadFlag(IF_OPERATION_ENABLE))
		pID->m_itemFlags.ClearFlag(IF_OPERATION_ENABLE);
	if (IO_NONE != pID->m_itemOpt)
		pID->m_itemOpt = IO_NONE;
}

void CTreeCtrlExt::SendSelChangedNotifyToParent(HTREEITEM htiNew, const CPoint& point, bool isFromMouse) const
{
	CWnd* pParentWnd = GetParent();
	if (0 == pParentWnd)
		return;

	NM_TREEVIEW tv;
	ZeroMemory(&tv, sizeof(tv));

	tv.hdr.hwndFrom = GetSafeHwnd();
	tv.hdr.idFrom = GetWindowLong(tv.hdr.hwndFrom, GWL_ID);
	tv.hdr.code = TVN_SELCHANGED;
	if (isFromMouse)
		tv.action = TVC_BYMOUSE;
	else
		tv.action = TVC_BYKEYBOARD;

	tv.itemNew.mask = TVIF_HANDLE | TVIF_STATE | TVIF_PARAM;
	tv.itemNew.hItem = htiNew;
	if (0 != htiNew)
	{
		tv.itemNew.state = GetItemState(htiNew, 0xffffffff);
		tv.itemNew.lParam = GetItemData(htiNew);
	}
	
	tv.itemOld.mask = TVIF_HANDLE | TVIF_STATE | TVIF_PARAM;
	HTREEITEM hCurSel = GetSelectedItem(); 
	tv.itemOld.hItem = hCurSel;
	if (0 != hCurSel)
	{
		tv.itemOld.state = GetItemState(hCurSel, 0xffffffff);
		tv.itemOld.lParam = GetItemData(hCurSel);
	}

	tv.ptDrag.x = point.x;
	tv.ptDrag.y = point.y;
	pParentWnd->SendMessage(WM_NOTIFY, tv.hdr.idFrom, reinterpret_cast<LPARAM>(&tv));
}

void CTreeCtrlExt::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	m_dwLBtnDown = GetTickCount();
	HTREEITEM hCurSel = GetSelectedItem(); 
	ClearItemOperation(hCurSel);

	UINT nHitFlags = 0;
	HTREEITEM hClickedItem = HitTest(point, &nHitFlags);
	if (hClickedItem != hCurSel)
		ClearItemOperation(hClickedItem);
	if (nHitFlags & TVHT_NOWHERE)
	{
		SelectItem(0);
		return;
	}

	if (nHitFlags & TVHT_ONITEM)
	{
		if (hCurSel != hClickedItem)
		{
			CItemData* pID = reinterpret_cast<CItemData*>(GetItemData(hClickedItem));
			if (0 != pID)
			{
				if (pID->m_itemFlags.ReadFlag(IF_FIXED_DISABLESEL))
					return;
				if (!pID->m_itemFlags.ReadFlag(IF_MULTI_SELFOCUS))
				{
					pID->m_itemFlags.RaiseFlag(IF_OPERATION_ENABLE);
					pID->m_itemOpt = IO_REPLACE;
					if (MK_CONTROL & nFlags)
					{
						pID->m_itemOpt = IO_APPEND;
						SelectItem(hClickedItem);
						return;
					}
				}
			}
		}
	}

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CTreeCtrlExt::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bDefRBtnDown = true;
	HTREEITEM hCurSel = GetSelectedItem(); 
	ClearItemOperation(hCurSel);

	UINT nHitFlags = 0;
	HTREEITEM hClickedItem = HitTest(point, &nHitFlags);
	if (hClickedItem != hCurSel)
		ClearItemOperation(hClickedItem);
	if (nHitFlags & TVHT_ONITEM)
	{
		if (MK_CONTROL & nFlags)
		{
			CItemData* pID = reinterpret_cast<CItemData*>(GetItemData(hClickedItem));
			if (0 != pID)
			{
				pID->m_itemFlags.RaiseFlag(IF_OPERATION_ENABLE);
				pID->m_itemOpt = IO_REMOVE;
				m_bDefRBtnDown = false;
				SendSelChangedNotifyToParent(hClickedItem, point, true);
				return;
			}
		}
	}

	CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CTreeCtrlExt::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	HTREEITEM hCurSel = GetSelectedItem();
	ClearItemOperation(hCurSel);
	if (VK_DELETE == nChar)
	{
		CItemData* pID = reinterpret_cast<CItemData*>(GetItemData(hCurSel));
		if (0 != pID)
		{
			pID->m_itemFlags.RaiseFlag(IF_OPERATION_ENABLE);
			pID->m_itemOpt = IO_DELETE;

			RECT rc;
			TreeView_GetItemRect(GetSafeHwnd(), hCurSel, &rc, 1);
			CPoint pt;
			pt.x = (rc.left + rc.right) / 2;
			pt.y = (rc.top + rc.bottom) / 2;
			SendSelChangedNotifyToParent(hCurSel, pt, false);
			return;
		}
	}

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTreeCtrlExt::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (60 > (GetTickCount() - m_dwLBtnDown))
		return;

	LPNMTREEVIEW pNMTV = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	m_htiDSource = pNMTV->itemNew.hItem;
	if (0 != m_pEvent)
	{
		if (!m_pEvent->DragDropBegin(m_htiDSource))
			return;
	}
	m_pDragImage = CreateDragImage(m_htiDSource);
	if(0 == m_pDragImage)
		return;

	m_bDragging = true;
	m_pDragImage->BeginDrag(0, CPoint(0, 0));
	CImageList::DragEnter(this, pNMTV->ptDrag);

	ShowCursor(FALSE);
	SetCapture();
	m_nScrollTimerID = SetTimer(2, 300, 0);
}

void CTreeCtrlExt::SetDropTarget(HTREEITEM hItem)
{
	if (0 == m_pEvent)
		return;

	if (m_pEvent->DragDropMoving(m_htiDSource, hItem))
		SelectDropTarget(hItem);
	else
		SelectDropTarget(m_htiDSource);
}

void CTreeCtrlExt::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bDragging)
	{
		CImageList::DragMove(point);
		CImageList::DragShowNolock(false);

		// lead to bcg memory leak!! very strange!! 
//		m_nHoverTimerID = SetTimer(1, 500, 0);

		HTREEITEM htiTemp = HitTest(point);
//		m_htiHoverItem = htiTemp;
		// we have to workaround the memory leak 
		if (0 != htiTemp)
		{
			if (!(TVIS_EXPANDED & GetItemState(htiTemp, TVIF_STATE)))
			{
				if (m_htiHoverItem != htiTemp)
				{
					m_dwHoverTimer = GetTickCount();
					m_htiHoverItem = htiTemp;
				}
				else
				{
					DWORD n = GetTickCount() - m_dwHoverTimer;
					if ((500 < n) || (0 > n))
						Expand(m_htiHoverItem, TVE_EXPAND);
				}
			}
		}

		SetDropTarget(htiTemp);
		CImageList::DragShowNolock(true);
	}
	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CTreeCtrlExt::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bDragging)
	{
		m_bDragging = false;
		HTREEITEM htiTemp = GetDropHilightItem();
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		SelectDropTarget(0);
		ReleaseCapture();
 		while (0 > ShowCursor(TRUE))
		{
		}
		delete m_pDragImage;
		m_pDragImage = 0;
		if (0 != m_pEvent)
			m_pEvent->DragDropEnd(m_htiDSource, htiTemp);
		KillTimer(m_nScrollTimerID);
		m_nScrollTimerID = 0;
	}
	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CTreeCtrlExt::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nHoverTimerID)
	{
		if (0 != m_htiHoverItem)
		{
			if (m_bDragging)
				Expand(m_htiHoverItem, TVE_EXPAND);
		}
	}
	else if (nIDEvent == m_nScrollTimerID)
	{
		CPoint pt;
		GetCursorPos(&pt);
		CRect rect;
		GetClientRect(&rect);
		ClientToScreen(&rect);

		HTREEITEM hItem = GetFirstVisibleItem();
		if (pt.y < rect.top + 10)
		{
			CImageList::DragShowNolock(false);
			SendMessage(WM_VSCROLL, SB_LINEUP);
			SetDropTarget(hItem);
			CImageList::DragShowNolock(true);
		}
		else if (pt.y > rect.bottom - 10)
		{
			CImageList::DragShowNolock(false);
			SendMessage(WM_VSCROLL, SB_LINEDOWN);
			HTREEITEM htiBTM = hItem;
			int nCount = GetVisibleCount();
			for(int i = 0; i < nCount; ++i)
			{
				hItem = GetNextVisibleItem(hItem);
				if (0 == hItem)
					break;
				htiBTM = hItem;
			}
			SetDropTarget(htiBTM);
			CImageList::DragShowNolock(true);
		}
	}
	CTreeCtrl::OnTimer(nIDEvent);
}
