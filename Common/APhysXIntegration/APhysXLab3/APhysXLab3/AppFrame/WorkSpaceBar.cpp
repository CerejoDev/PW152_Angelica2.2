// workspace.cpp : implementation of the CWorkSpaceBar class
//

#include "stdafx.h"
#include "APhysXLab31.h"
#include "WorkSpaceBar.h"
#include "TreeCtrlItemData.h"
#include "Command.h"
#include "Game.h"

using namespace APhysXCommonDNet;

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

#ifdef ENABLE_MULLITHREAD_TEST
	APhysXMutex CWorkSpaceBar::m_Locker;
#endif
const UINT CWorkSpaceBar::tvID = 0x00123400;

CWorkSpaceBar::CWorkSpaceBar()
{
	// TODO: add one-time construction code here
	m_hRoot = 0;
	m_EnableTree = false;
	m_pGame = 0;
	m_pCmdRecorder = 0;
	m_pObjSelGroup = 0;
	m_MsgDirect = MD_IDLE;
	m_rcDirty.SetRectEmpty();
	m_IsEventActive = false;
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

	SetupTreeContent();
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

void CWorkSpaceBar::SetGame(CEditor& game)
{
	m_pGame = &game;
}

void CWorkSpaceBar::OnNotifyBorn(const IObjBase& obj)
{
	if (!m_EnableTree)
		return;

	HTREEITEM htiParent = GetParentItem(obj);
	if (0 == htiParent)
		return;

	bool bIsFocusObj = false;
	if (0 != m_pObjSelGroup)
	{
		if (&obj == m_pObjSelGroup->GetFocusObject())
			bIsFocusObj = true;
	}
	AddItemItself(obj, htiParent, bIsFocusObj);
}

void CWorkSpaceBar::OnNotifySelChange(const IObjBase* pNewFocus, bool bSelected, bool bFocusOn)
{
	if (!m_EnableTree)
		return;

	HTREEITEM htiSel = 0;
	if (0 != pNewFocus)
	{
		IPropObjBase* pProp = pNewFocus->GetProperties();
		HTREEITEM htiTemp = GetHTIData(*pProp, *this);
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

void CWorkSpaceBar::OnNotifyRename(const IObjBase& obj)
{
	if (!m_EnableTree)
		return;

	IPropObjBase* pProp = obj.GetProperties();
	HTREEITEM htiTemp = GetHTIData(*pProp, *this);
	if (0 == htiTemp)
		return;

	m_wndTree.SetItemText(htiTemp, pProp->GetObjName());
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
				if (!TreeSelChangedProcesser(*pNMTV))
				{
					m_MsgDirect = MD_IDLE;
					return TRUE;
				}
				m_MsgDirect = MD_IDLE;
			}
		}
	}
	return __super::OnNotify(wParam, lParam, pResult);
}

HTREEITEM CWorkSpaceBar::GetHTIData(IPropObjBase& prop, const CWorkSpaceBar& key)
{
	HTIInfo* pInfo = static_cast<HTIInfo*>(prop.GetUserData());
	while (0 != pInfo)
	{
		if (&key == pInfo->pKey)
			return pInfo->hti;

		pInfo = pInfo->pNext;
	}
	return 0;
}

void CWorkSpaceBar::RemoveHTIData(IPropObjBase& prop, const CWorkSpaceBar& key)
{
	HTIInfo* pParent = 0;
	HTIInfo* pInfo = static_cast<HTIInfo*>(prop.GetUserData());
	while (0 != pInfo)
	{
		if (&key == pInfo->pKey)
		{
			if (0 != pParent)
				pParent->pNext = pInfo->pNext;
			else
				prop.SetUserData(pInfo->pNext);
			delete pInfo;
			return;
		}
		pParent = pInfo;
		pInfo = pInfo->pNext;
	}
}

HTREEITEM CWorkSpaceBar::RemoveAllHTIData(const IObjBase& obj, arrHTI* pOutDelHTIs)
{
	IPropObjBase* pProp = obj.GetProperties();
	HTREEITEM htiTemp = GetHTIData(*pProp, *this);
	if (0 != htiTemp)
	{
		RemoveHTIData(*pProp, *this);
		if (0 != pOutDelHTIs)
			pOutDelHTIs->push_back(htiTemp);
	}

	IObjBase* pChild = 0;
	int nChildren = obj.GetChildNum();
	for (int i = 0; i < nChildren; ++i)
	{
		pChild = dynamic_cast<IObjBase*>(obj.GetChild(i));
		RemoveAllHTIData(*pChild, pOutDelHTIs);
	}
	return htiTemp;
}

void CWorkSpaceBar::AddHTIData(IPropObjBase& prop, const CWorkSpaceBar& key, HTREEITEM value)
{
	HTIInfo* pInfo = static_cast<HTIInfo*>(prop.GetUserData());

	HTIInfo* pTheLast = 0;
	while (0 != pInfo)
	{
		if (&key == pInfo->pKey)
		{
			pInfo->hti = value;
			return;
		}
		pTheLast = pInfo;
		pInfo = pInfo->pNext;
	}

	HTIInfo* pNew = new HTIInfo;
	pNew->pKey = &key;
	pNew->hti = value;
	if (0 != pTheLast)
		pTheLast->pNext = pNew;
	else
		prop.SetUserData(pNew);
}

void CWorkSpaceBar::AddItemItself(const IObjBase& obj, HTREEITEM htiParent, bool isFocus)
{
	if (!m_EnableTree)
		return;

#ifdef ENABLE_MULLITHREAD_TEST
	APhysXMutexLock autoLocker(m_Locker);
#endif

	IPropObjBase* pProp = obj.GetProperties();
	HTREEITEM htiTemp = m_wndTree.InsertItem(pProp->GetObjName(), htiParent);

	AddHTIData(*pProp, *this, htiTemp);
	CItemData* pID = new CItemData(IT_LEAF);
	pID->m_itemUserData = PtrToUlong(&obj);
	CPhysXObjSelGroup* pSelGroup = pProp->GetSelGroup();
	if (0 != pSelGroup)
		pID->m_itemFlags.RaiseFlag(IF_MULTI_SELECTED);
	if (isFocus)
		pID->m_itemFlags.RaiseFlag(IF_MULTI_SELFOCUS);
	if (ObjManager::OBJ_TYPEID_CAMERA == pProp->GetObjType())
		pID->m_itemFlags.RaiseFlag(IF_FIXED_DISABLESEL);
	m_wndTree.SetItemData(htiTemp, PtrToUlong(pID));
	m_wndTree.Expand(htiParent, TVE_EXPAND);
}

void CWorkSpaceBar::DelTreeItems(HTREEITEM hti)
{
	if (!m_EnableTree)
		return;

	if (0 == hti)
		return;

#ifdef ENABLE_MULLITHREAD_TEST
	APhysXMutexLock autoLocker(m_Locker);
#endif

	HTREEITEM htiChild = m_wndTree.GetChildItem(hti);
	while (0 != htiChild)
	{
		DelTreeItems(htiChild);
		htiChild = m_wndTree.GetChildItem(hti);
	}

	DWORD_PTR pPTR = m_wndTree.GetItemData(hti);
	CItemData* pID = reinterpret_cast<CItemData*>(pPTR);
	delete pID;
	m_wndTree.DeleteItem(hti);
}

bool CWorkSpaceBar::TreeSelChangedProcesser(const NMTREEVIEW& nmtv)
{
	if (MD_TREE_TO_CONTENT != m_MsgDirect)
	{
		assert(MD_TREE_TO_CONTENT == m_MsgDirect);
		return true;
	}
	if (0 == m_pObjSelGroup)
		return true;

	if (m_IsEventActive)
		return true;

	assert(TVIF_HANDLE == (TVIF_HANDLE & nmtv.itemNew.mask));
	HTREEITEM htiNewItem = static_cast<HTREEITEM>(nmtv.itemNew.hItem);
	if (0 == htiNewItem)
	{
		CItemData cid(IT_LEAF);
		cid.m_itemFlags.RaiseFlag(IF_OPERATION_ENABLE);
		cid.m_itemOpt = IO_REPLACE;
		if (!UpdateContent(cid))
			return false;
	}
	else
	{
		assert(TVIF_PARAM == (TVIF_PARAM & nmtv.itemNew.mask));
		CItemData* pNewItemD = reinterpret_cast<CItemData*>(nmtv.itemNew.lParam);
		if (0 != pNewItemD)
		{
			if (!UpdateContent(*pNewItemD))
				return false;
		}
	}

	assert(TVIF_HANDLE == (TVIF_HANDLE & nmtv.itemOld.mask));
	HTREEITEM htiOldItem = static_cast<HTREEITEM>(nmtv.itemOld.hItem);
	if (0 != htiOldItem)
	{
		assert(TVIF_PARAM == (TVIF_PARAM & nmtv.itemOld.mask));
		CItemData* pOldItemD = reinterpret_cast<CItemData*>(nmtv.itemOld.lParam);
		if (0 != pOldItemD)
		{
			if (!UpdateContent(*pOldItemD))
				return false;
		}
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
		HTREEITEM htiTemp = GetHTIData(*pProp, *this);
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
	return true;
}

bool CWorkSpaceBar::UpdateContent(CItemData& itemData)
{
	if (!itemData.m_itemFlags.ReadFlag(IF_OPERATION_ENABLE))
		return true;

	assert(0 != m_pObjSelGroup);
	bool bCmdRecorderEnable = true;
	if ((0 == m_pCmdRecorder) || m_pObjSelGroup->IsRunTime())
		bCmdRecorderEnable = false;

	if (IT_LEAF == itemData.m_itemType)
	{
		IObjBase* pObj = reinterpret_cast<IObjBase*>(itemData.m_itemUserData);
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
			{
				MEditor^ pEdit = dynamic_cast<MEditor^>(MScene::Instance);
				pEdit->RemoveAllSelectedObjects();
				return false;
			}
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
	return true;
}
