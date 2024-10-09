// workspace.cpp : implementation of the CWorkSpaceBar class
//

#include "stdafx.h"
#include "APhysXLab31.h"
#include "WorkSpaceBar.h"
#include "TreeCtrlItemData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 1;
/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar

BEGIN_MESSAGE_MAP(CWorkSpaceBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CWorkSpaceBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar construction/destruction

const UINT CWorkSpaceBar::tvID = 0x00123400;

CWorkSpaceBar::CWorkSpaceBar()
{
	// TODO: add one-time construction code here
	m_EnableTree = false;
	m_pGame = 0;
	m_pCmdRecorder = 0;
	m_pObjSelGroup = 0;
	m_hRoot = 0;
	m_MsgDirect = MD_IDLE;
	m_rcDirty.SetRectEmpty();
}

CWorkSpaceBar::~CWorkSpaceBar()
{
}

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceBar message handlers

int CWorkSpaceBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create tree windows.
	// TODO: create your own tab windows here:
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | 
								TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
	
	if (!m_wndTree.Create (dwViewStyle, rectDummy, this, tvID))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	// Setup trees content:
	m_hRoot = m_wndTree.InsertItem (_T("OBJ_ALL"));
	CItemData* pID = new CItemData(IT_ROOT);
	pID->m_itemTextColor = RGB(0x23,0x8E,0x23);
	pID->m_itemFlags.RaiseFlag(IF_TEXT_COLOR);
	m_wndTree.SetItemData(m_hRoot, PtrToUlong(pID));

	CreateOTTN(RawObjMgr::OBJ_TYPEID_STATIC);
	CreateOTTN(RawObjMgr::OBJ_TYPEID_SKINMODEL);
	CreateOTTN(RawObjMgr::OBJ_TYPEID_ECMODEL);
	CreateOTTN(RawObjMgr::OBJ_TYPEID_PARTICLE);
	CreateOTTN(RawObjMgr::OBJ_TYPEID_FORCEFIELD);
	CreateOTTN(RawObjMgr::OBJ_TYPEID_REGION);
	m_wndTree.Expand(m_hRoot, TVE_EXPAND);
	m_EnableTree = true;
	return 0;
}


void CWorkSpaceBar::OnDestroy()
{
	// TODO: Add your message handler code here
	m_EnableTree = false;
	HTREEITEM htiTemp = m_hRoot;
	while (0 != htiTemp)
	{
		DWORD_PTR pPTR = m_wndTree.GetItemData(htiTemp);
		CItemData* pID = reinterpret_cast<CItemData*>(pPTR);
		delete pID; 
		m_wndTree.SetItemData(m_hRoot, 0);
		htiTemp = m_wndTree.GetNextItem(htiTemp, TVGN_EXT_NEXT_FOLLOWING);
	}
	__super::OnDestroy();
}

void CWorkSpaceBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	// Tab control should cover a whole client area:
	m_wndTree.SetWindowPos (NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void CWorkSpaceBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	m_wndTree.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree,	::GetSysColor (COLOR_3DSHADOW), 
								::GetSysColor (COLOR_3DSHADOW));
}

void CWorkSpaceBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	if (!m_wndTree.HandledRBtnDownByDefault())
		return;

	__super::OnContextMenu(pWnd, point);
}

void CWorkSpaceBar::SetSelGroup(CPhysXObjSelGroup* pSelGroup)
{
	m_pObjSelGroup = pSelGroup;
}

void CWorkSpaceBar::SetCmdQueue(CmdQueue& cmdQueue)
{
	m_pCmdRecorder = &cmdQueue;
}

void CWorkSpaceBar::SetGame(CGame& game)
{
	m_pGame = &game;
}

void CWorkSpaceBar::CreateOTTN(const RawObjMgr::ObjTypeID& nID)
{
	HTREEITEM htiTemp = m_wndTree.InsertItem(RawObjMgr::GetObjTypeIDText(nID), m_hRoot);
	m_ottn.push_back(ObjTypeTreeNode(nID, htiTemp));
	CItemData* pID = new CItemData(IT_NODE);
	pID->m_itemTextColor = RGB(0x23,0x8E,0x23);
	pID->m_itemFlags.RaiseFlag(IF_TEXT_COLOR);
	m_wndTree.SetItemData(htiTemp, PtrToUlong(pID));
}

HTREEITEM CWorkSpaceBar::GetOTTN(const RawObjMgr::ObjTypeID& nID) const
{
	if (RawObjMgr::OBJ_TYPEID_MANAGER == nID)
		return 0;

	OTTN::const_iterator it = m_ottn.begin();
	OTTN::const_iterator itEnd = m_ottn.end();
	for (; it != itEnd; ++it)
	{
		if (nID == it->tid)
			return it->hti;
	}

	assert(!"Shouldn't be here! Debug it!");
	return m_hRoot;
}

void CWorkSpaceBar::OnNotifyBorn(const IObjBase& obj)
{
	if (!m_EnableTree)
		return;

	IPropObjBase* pProp = obj.GetProperties();
	HTREEITEM htiParent = GetOTTN(pProp->GetObjType());
	if (0 == htiParent)
		return;

	HTREEITEM htiTemp = m_wndTree.InsertItem(_TAS2WC(pProp->GetObjName()), htiParent);
	pProp->SetUserData(htiTemp);
	CItemData* pID = new CItemData(IT_LEAF);
	pID->m_itemUserData = PtrToUlong(&obj);
	m_wndTree.SetItemData(htiTemp, PtrToUlong(pID));
	m_wndTree.Expand(htiParent, TVE_EXPAND);
}

void CWorkSpaceBar::OnNotifyDie(const IObjBase& obj)
{
	if (!m_EnableTree)
		return;

	IPropObjBase* pProp = obj.GetProperties();
	HTREEITEM htiTemp = static_cast<HTREEITEM>(pProp->GetUserData());
	if (0 == htiTemp)
		return;

	pProp->SetUserData(0);
	DWORD_PTR pPTR = m_wndTree.GetItemData(htiTemp);
	CItemData* pID = reinterpret_cast<CItemData*>(pPTR);
	delete pID; 
	m_wndTree.DeleteItem(htiTemp);
}

void CWorkSpaceBar::OnNotifySelChange(const IObjBase* pNewFocus, bool bSelected, bool bFocusOn)
{
	if (!m_EnableTree)
		return;

	HTREEITEM htiSel = 0;
	if (0 != pNewFocus)
	{
		IPropObjBase* pProp = pNewFocus->GetProperties();
		HTREEITEM htiTemp = static_cast<HTREEITEM>(pProp->GetUserData());
		if (0 != htiTemp)
		{
			RECT rc;
			m_wndTree.GetItemRect(htiTemp, &rc, false);
			m_rcDirty |= rc;

			htiSel = htiTemp;
			DWORD_PTR pPTR = m_wndTree.GetItemData(htiTemp);
			CItemData* pID = reinterpret_cast<CItemData*>(pPTR);
			if (bSelected)
			{
				pID->m_itemFlags.RaiseFlag(IF_MULTI_SELECTED);
				if (bFocusOn)
					pID->m_itemFlags.RaiseFlag(IF_MULTI_SELFOCUS);
				else
					pID->m_itemFlags.ClearFlag(IF_MULTI_SELFOCUS);
			}
			else
			{
				pID->m_itemFlags.ClearFlag(IF_MULTI_SELECTED);
				pID->m_itemFlags.ClearFlag(IF_MULTI_SELFOCUS);
			}
		}
	}

	if (MD_TREE_TO_CONTENT == m_MsgDirect)
		return;
	if (bFocusOn)
	{
		m_MsgDirect = MD_CONTENT_TO_TREE;
		m_wndTree.SelectItem(htiSel);
		m_MsgDirect = MD_IDLE;
	}
}

void CWorkSpaceBar::OnUpdateStateRedraw()
{
	if (!m_rcDirty.IsRectEmpty())
	{
		m_wndTree.InvalidateRect(&m_rcDirty);
		m_rcDirty.SetRectEmpty();
	}
}

BOOL CWorkSpaceBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	if (tvID == wParam)
	{
		LPNMTREEVIEW pNMTV = reinterpret_cast<LPNMTREEVIEW>(lParam);
		if (TVN_SELCHANGED == pNMTV->hdr.code)
		{
			if (MD_IDLE == m_MsgDirect)
			{
				m_MsgDirect = MD_TREE_TO_CONTENT;
				TreeSelChangedProcesser(*pNMTV);
				m_MsgDirect = MD_IDLE;
			}
		}
	}
	return __super::OnNotify(wParam, lParam, pResult);
}

void CWorkSpaceBar::TreeSelChangedProcesser(const NMTREEVIEW& nmtv)
{
	if (MD_TREE_TO_CONTENT != m_MsgDirect)
	{
		assert(MD_TREE_TO_CONTENT == m_MsgDirect);
		return;
	}
	if (0 == m_pObjSelGroup)
		return;

	assert(TVIF_HANDLE == (TVIF_HANDLE & nmtv.itemNew.mask));
	HTREEITEM htiNewItem = static_cast<HTREEITEM>(nmtv.itemNew.hItem);
	if (0 == htiNewItem)
	{
		CItemData cid(IT_LEAF);
		cid.m_itemFlags.RaiseFlag(IF_OPERATION_ENABLE);
		cid.m_itemOpt = IO_REPLACE;
		UpdateContent(cid);
	}
	else
	{
		assert(TVIF_PARAM == (TVIF_PARAM & nmtv.itemNew.mask));
		CItemData* pNewItemD = reinterpret_cast<CItemData*>(nmtv.itemNew.lParam);
		if (0 != pNewItemD)
			UpdateContent(*pNewItemD);
	}

	assert(TVIF_HANDLE == (TVIF_HANDLE & nmtv.itemOld.mask));
	HTREEITEM htiOldItem = static_cast<HTREEITEM>(nmtv.itemOld.hItem);
	if (0 != htiOldItem)
	{
		assert(TVIF_PARAM == (TVIF_PARAM & nmtv.itemOld.mask));
		CItemData* pOldItemD = reinterpret_cast<CItemData*>(nmtv.itemOld.lParam);
		if (0 != pOldItemD)
			UpdateContent(*pOldItemD);
	}

	bool bUpdateSel = false;
	HTREEITEM htiNewSel = 0;
	IObjBase* pObj = m_pObjSelGroup->GetFocusObject();
	if (0 == pObj)
	{
		bUpdateSel = true;
	}
	else
	{
		IPropObjBase* pProp = pObj->GetProperties();
		HTREEITEM htiTemp = static_cast<HTREEITEM>(pProp->GetUserData());
		if (0 != htiTemp)
		{
			htiNewSel = htiTemp;
			bUpdateSel = true;
		}
	}

	if (bUpdateSel)
	{
		HTREEITEM hCurSel = m_wndTree.GetSelectedItem();
		if (htiNewSel != hCurSel)
			m_wndTree.SelectItem(htiNewSel);
	}
}

void CWorkSpaceBar::UpdateContent(CItemData& itemData)
{
	if (!itemData.m_itemFlags.ReadFlag(IF_OPERATION_ENABLE))
		return;

	assert(0 != m_pObjSelGroup);
	bool bCmdRecorderEnable = true;
	if ((0 == m_pCmdRecorder) || m_pObjSelGroup->IsRunTime())
		bCmdRecorderEnable = false;

	if (IT_LEAF == itemData.m_itemType)
	{
		IPhysXObjBase* pObj = reinterpret_cast<IPhysXObjBase*>(itemData.m_itemUserData);
		switch (itemData.m_itemOpt)
		{ 
		case IO_APPEND:
			assert(0 != pObj); 
			if (bCmdRecorderEnable)
				m_pCmdRecorder->SubmitBeforeExecution(CmdSelAppend());
			m_pObjSelGroup->AppendObject(*pObj);
			break;
		case IO_REMOVE:
			assert(0 != pObj); 
			if (bCmdRecorderEnable)
				m_pCmdRecorder->SubmitBeforeExecution(CmdSelRemove());
			m_pObjSelGroup->RemoveObject(*pObj);
			break;
		case IO_DELETE:
			assert(0 != m_pGame);
			m_pGame->ReleaseAllSelectedObjects();
			break;
		case IO_REPLACE:
			if (bCmdRecorderEnable)
				m_pCmdRecorder->SubmitBeforeExecution(CmdSelReplace());
			m_pObjSelGroup->ReplaceWithObject(pObj);
			break;
		default:
			break;
		}
	}

	itemData.m_itemFlags.ClearFlag(IF_OPERATION_ENABLE);
	itemData.m_itemOpt = IO_NONE;
}

