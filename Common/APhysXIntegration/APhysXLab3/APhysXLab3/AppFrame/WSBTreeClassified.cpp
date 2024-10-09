// WSBTreeClassified.cpp : implementation of the CWSBTreeClassified class
//

#include "stdafx.h"
#include "WSBTreeClassified.h"
#include "TreeCtrlItemData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWSBTreeClassified

/////////////////////////////////////////////////////////////////////////////
// CWSBTreeClassified construction/destruction

/////////////////////////////////////////////////////////////////////////////
// CWSBTreeClassified message handlers

void CWSBTreeClassified::OnNotifyDie(const IObjBase& obj)
{
	arrHTI dirty;
	RemoveAllHTIData(obj, &dirty);
	size_t nCount = dirty.size();
	for (size_t i = 0; i < nCount; ++i)
		DelTreeItems(dirty[i]);
}

void CWSBTreeClassified::SetupTreeContent()
{
	CreateOTTN(ObjManager::OBJ_TYPEID_CAMERA);
	CreateOTTN(ObjManager::OBJ_TYPEID_STATIC);
	CreateOTTN(ObjManager::OBJ_TYPEID_SKINMODEL);
	CreateOTTN(ObjManager::OBJ_TYPEID_ECMODEL);
	CreateOTTN(ObjManager::OBJ_TYPEID_PARTICLE);
	CreateOTTN(ObjManager::OBJ_TYPEID_FORCEFIELD);
	CreateOTTN(ObjManager::OBJ_TYPEID_REGION);
	CreateOTTN(ObjManager::OBJ_TYPEID_DUMMY);
	CreateOTTN(ObjManager::OBJ_TYPEID_SMSOCKET);
	CreateOTTN(ObjManager::OBJ_TYPEID_SOCKETSM);
	CreateOTTN(ObjManager::OBJ_TYPEID_SPOTLIGHT);
}

void CWSBTreeClassified::CreateOTTN(const ObjManager::ObjTypeID& nID)
{
	assert(0 != m_hRoot);
	HTREEITEM htiTemp = m_wndTree.InsertItem(ObjManager::GetObjTypeIDText(nID), m_hRoot);
	m_ottn.push_back(ObjTypeTreeNode(nID, htiTemp));
	CItemData* pID = new CItemData(IT_NODE);
	pID->m_itemTextColor = RGB(0x23,0x8E,0x23);
	pID->m_itemFlags.RaiseFlag(IF_TEXT_COLOR);
	m_wndTree.SetItemData(htiTemp, PtrToUlong(pID));
}

HTREEITEM CWSBTreeClassified::GetOTTN(const ObjManager::ObjTypeID& nID) const
{
	if (ObjManager::OBJ_TYPEID_EMPTY == nID)
		return 0;
	if (ObjManager::OBJ_TYPEID_PHYSX == nID)
		return 0;
	if (ObjManager::OBJ_TYPEID_ALL == nID)
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

HTREEITEM CWSBTreeClassified::GetParentItem(const IObjBase& obj)
{
	IPropObjBase* pProp = obj.GetProperties();
	HTREEITEM htiParent = GetOTTN(pProp->GetObjType());
	return htiParent;
}
