// WSBTreeParentChild.cpp : implementation of the CWSBTreeParentChild class
//

#include "stdafx.h"
#include "resource.h" 
#include "WSBTreeParentChild.h"
#include "TreeCtrlItemData.h"

using namespace APhysXCommonDNet;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWSBTreeParentChild

/////////////////////////////////////////////////////////////////////////////
// CWSBTreeParentChild construction/destruction
CWSBTreeParentChild::CWSBTreeParentChild()
{
	m_bEnableApxOptEvent = true;
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_TREECTRL);
	m_imgList.Create(16, 16, ILC_COLORDDB | ILC_MASK, 1, 1);
	m_imgList.Add(&bitmap, RGB(192, 192, 192));
}

void CWSBTreeParentChild::OnNotifyDie(const IObjBase& obj)
{
	HTREEITEM hti = RemoveAllHTIData(obj);
	DelTreeItems(hti);
}

/////////////////////////////////////////////////////////////////////////////
// CWSBTreeParentChild message handlers

void CWSBTreeParentChild::SetupTreeContent()
{
	m_wndTree.SetDragDropEvent(this);
	m_wndTree.SetImageList(&m_imgList, TVSIL_NORMAL);
	APtrArray<IObjBase*> Objs;
	int nCount = ObjManager::GetInstance()->GetRootLiveObjects(Objs, ObjManager::OBJ_TYPEID_ALL);
	for (int i = 0; i < nCount; ++i)
		AddItems(*Objs[i]);
}

HTREEITEM CWSBTreeParentChild::GetParentItem(const IObjBase& obj)
{
	IObjBase* pParent = dynamic_cast<IObjBase*>(obj.GetParent());
	if (0 == pParent)
		return m_hRoot;

	IPropObjBase* pProp = pParent->GetProperties();
	HTREEITEM htiParent = GetHTIData(*pProp, *this);
	assert(0 != htiParent);
	return htiParent;
}

void CWSBTreeParentChild::AddItems(const IObjBase& obj)
{
	IPropObjBase::ObjState os = obj.GetProperties()->GetObjState();
	if (IPropObjBase::OBJECT_STATE_NULL == os)
		return;
	if (IPropObjBase::OBJECT_STATE_CORPSE == os)
		return;

	bool bIsFocusObj = false;
	if (0 != m_pObjSelGroup)
	{
		if (&obj == m_pObjSelGroup->GetFocusObject())
			bIsFocusObj = true;
	}

	HTREEITEM htiParent = GetParentItem(obj);
	AddItemItself(obj, htiParent, bIsFocusObj);

	IObjBase* pChild = 0;
	int nChildren = obj.GetChildNum();
	for (int i = 0; i < nChildren; ++i)
	{
		pChild = dynamic_cast<IObjBase*>(obj.GetChild(i));
		AddItems(*pChild);
	}
}

IObjBase* CWSBTreeParentChild::GetObject(HTREEITEM hti)
{
	IObjBase* pObj = 0;
	DWORD_PTR pPTR = m_wndTree.GetItemData(hti);
	CItemData* pID = reinterpret_cast<CItemData*>(pPTR);
	assert(0 != pID);
	if (0 != pID)
		pObj = reinterpret_cast<IObjBase*>(pID->m_itemUserData);
	return pObj;
}

bool CWSBTreeParentChild::DragDropBegin(HTREEITEM htiDragItem)
{
	if (m_hRoot == htiDragItem)
		return false;
	return true;
}

bool CWSBTreeParentChild::DragDropMoving(HTREEITEM htiDragItem, HTREEITEM htiDropTarget)
{
	if (htiDropTarget == htiDragItem)
		return false;

	HTREEITEM htiParent = m_wndTree.GetNextItem(htiDropTarget, TVGN_PARENT);
	while (0 != htiParent)
	{
		if (htiParent == htiDragItem)
			return false;
		htiParent = m_wndTree.GetNextItem(htiParent, TVGN_PARENT);
	}
	return true;
}

void CWSBTreeParentChild::DragDropEnd(HTREEITEM htiDragItem, HTREEITEM htiDropTarget)
{
	if (0 == htiDropTarget)
		return;
	if (htiDragItem == htiDropTarget)
		return;

	IObjBase* pObjSource = GetObject(htiDragItem);
	assert(0 != pObjSource);
	if (0 != pObjSource)
	{
		IObjBase* pParentNow = dynamic_cast<IObjBase*>(pObjSource->GetParent());
		IObjBase* pParentTarget = GetObject(htiDropTarget);
		if (pParentTarget == pParentNow)
			return;

		m_IsEventActive = true;
		m_bEnableApxOptEvent = false;
		DelTreeItems(htiDragItem);
		// ObjManager should receive the add/remove child event.
		// we must add/remove child object via MScene due to the "global-object" issue.
		if (0 != m_pCmdRecorder)
			m_pCmdRecorder->SubmitBeforeExecution(CmdParentChange(*pObjSource));
		if (0 == pParentTarget)
			MScene::Instance->RemoveChild(pParentNow, pObjSource);
		else
			MScene::Instance->AddChild(pParentTarget, pObjSource);
		AddItems(*pObjSource);
		m_bEnableApxOptEvent = true;
		m_IsEventActive = false;
	}
}

void CWSBTreeParentChild::OnCloneNotify(unsigned int nbEntities, CPair* entities)
{
	for(unsigned int i = 0; i < nbEntities; ++i)
	{
		if (0 != entities->pClone)
		{
			OnNotifyDie(*(entities->pClone));
			AddItems(*(entities->pClone));
		}
		++entities;
	}
}

void CWSBTreeParentChild::OnAddChildNotify(ApxObjBase* pParent, ApxObjBase* pChild)
{
	if (!m_bEnableApxOptEvent)
		return;

	IObjBase* pObjChild = dynamic_cast<IObjBase*>(pChild);
	IPropObjBase* pProp = pObjChild->GetProperties();
	HTREEITEM htiTemp = GetHTIData(*pProp, *this);
	DelTreeItems(htiTemp);
	AddItems(*pObjChild);
}

void CWSBTreeParentChild::OnRemoveChildNotify(ApxObjBase* pParent, ApxObjBase* pChild)
{
	if (!m_bEnableApxOptEvent)
		return;

	IObjBase* pObjChild = dynamic_cast<IObjBase*>(pChild);
	IPropObjBase* pProp = pObjChild->GetProperties();
	HTREEITEM htiTemp = GetHTIData(*pProp, *this);
	DelTreeItems(htiTemp);
	AddItems(*pObjChild);
}
