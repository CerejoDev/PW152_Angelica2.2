/*
* FILE: PhysXObjSelGroup.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2010/12/21
*
* HISTORY: 
*
* Copyright (c) 2010 Perfect World, All Rights Reserved.
*/

#include "stdafx.h"

const TCHAR* CPhysXObjSelGroup::szEmpty = _T("");
const _variant_t CPhysXObjSelGroup::varEmpty = _T("");
const _variant_t PropItem::varMultiFlag = _T("<MultiValue>");

void CPhysXObjSelGroup::ItemInfo::SetNewValue(const _variant_t& nv, bool bIsMerge)
{
	RaiseFlag(PF_HAS_NEW_VALUE);
	if (bIsMerge)
	{
		if (0 != pMultiValueMask)
		{
			if (newValue != *pMultiValueMask)
			{
				if (newValue != nv)
					newValue = *pMultiValueMask;
			}
			return;
		}
	}

	newValue = nv;
}

bool CPhysXObjSelGroup::FCRemove::operator()(const IObjBase& objSample, const SelObj& objX, int* poutCodeSampleToX) const
{
	if (objX.bIsIndependent)
		return false;

	int nCode = objX.pObj->GetRelationshipCode(objSample);
	if (0 != poutCodeSampleToX)
		*poutCodeSampleToX = nCode;
	
	if (0 == nCode)
		return false;
	return true;
}

bool CPhysXObjSelGroup::FCChange::operator()(const IObjBase& objSample, const SelObj& objX, int* poutCodeSampleToX) const
{
	int nCode = objX.pObj->GetRelationshipCode(objSample);
	if (0 != poutCodeSampleToX)
		*poutCodeSampleToX = nCode;
	
	if (0 != nCode)
		return true;
	if (&objSample == objX.pObj)
		return true;
	return false;
}

bool CPhysXObjSelGroup::ExtractVector(const PropItem& prop, A3DVECTOR3& outVec)
{
	const int nCount = prop.GetSubItemsCount();
	assert(3 == nCount);
	if (3 != nCount)
		return false;

	outVec.x = prop.GetSubItemValue(0);
	outVec.y = prop.GetSubItemValue(1);
	outVec.z = prop.GetSubItemValue(2);
	return true;
}

bool CPhysXObjSelGroup::ExtractColor(const PropItem& prop, A3DCOLORVALUE& outVec)
{
	const int nCount = prop.GetSubItemsCount();
	assert(4 == nCount);
	if (4 != nCount)
		return false;

	outVec.r = prop.GetSubItemValue(0);
	outVec.g = prop.GetSubItemValue(1);
	outVec.b = prop.GetSubItemValue(2);
	outVec.a = prop.GetSubItemValue(3);
	return true;
}

bool CPhysXObjSelGroup::ApplyColorValue(PropItem& prop, const A3DCOLORVALUE& value)
{
	const int nCount = prop.GetSubItemsCount();
	assert(4 == nCount);
	if (4 != nCount)
		return false;

	prop.SetSubItemValue(0, value.r);
	prop.SetSubItemValue(1, value.g);
	prop.SetSubItemValue(2, value.b);
	prop.SetSubItemValue(3, value.a);
	return true;
}

CPhysXObjSelGroup::CPhysXObjSelGroup()
{
	m_Status.ClearAllFlags();
	m_pStateReport = 0;
	m_pRHO = 0;
	m_pPropList = 0;
	m_ToUpdateSocketName = false;
}

bool CPhysXObjSelGroup::EnterRuntime(APhysXScene& aPhysXScene)
{
	int nAll = Size();
	for (int i = 0; i < nAll; ++i)
		m_objsBackup.push_back(m_aObjects[i].pObj);
	ReplaceWithObject(0);
	m_Status.RaiseFlag(SF_IS_GAME_RUNTIME);
	return true;
}

void CPhysXObjSelGroup::LeaveRuntime()
{
	m_Status.ClearFlag(SF_IS_GAME_RUNTIME);
	ReplaceWithObjects(m_objsBackup);
	m_objsBackup.clear();
}

bool CPhysXObjSelGroup::IsDrawItemEnable() const
{
	if (m_Status.ReadFlag(SF_IS_GAME_RUNTIME))
		return false;

	if (0 == Size())
		return false;

	return true;
}

void CPhysXObjSelGroup::DrawFrame(A3DWireCollector* pWC)
{
	if (!IsDrawItemEnable())
		return;

	if (0 != pWC)
		m_focusRefFrame.Draw(*pWC);
}

void CPhysXObjSelGroup::EnablePropItem(PropID id, bool bIsEnable)
{
	if (0 != m_Props[id - PID_START_BOUND].pItem)
	{
		if (bIsEnable)
			m_Props[id - PID_START_BOUND].pItem->Enable(TRUE);
		else
			m_Props[id - PID_START_BOUND].pItem->Enable(FALSE);
	}
}

bool CPhysXObjSelGroup::ImportAndExportFFData(bool import, const AString& strFile)
{
	if (!IsOnlyFocusOn(ObjManager::OBJ_TYPEID_FORCEFIELD))
		return false;

	CPhysXObjForceField* pFF = dynamic_cast<CPhysXObjForceField*>(GetFocusObject());
	if (0 == pFF)
		return false;

	bool bRtn = pFF->ImportAndExportFFData(import, strFile);
	if (bRtn)
	{
		if (import)
		{
			ReplaceWithObject(0);
			AppendObject(*pFF);
		}
	}
	return bRtn;
}

int CPhysXObjSelGroup::GetParentIndex(int index) const
{
	IObjBase* pObj = GetObject(index);
	if (0 == pObj)
		return -1;

	IObjBase* pParent = dynamic_cast<IObjBase*>(pObj->GetParent());
	if (0 == pParent)
		return -1;

	const int nCount = Size();
	for (int i = 0; i < nCount; ++i)
	{
		if (pParent == m_aObjects[i].pObj)
			return i;
	}
	return -1;
}

IObjBase* CPhysXObjSelGroup::GetFocusObject() const
{
	if (0 == Size())
		return 0;

	return m_aObjects[0].pObj;
}

IObjBase* CPhysXObjSelGroup::GetObject(int index) const
{
	const int nCount = Size();
	if (0 == nCount)
		return 0;

	if (0 > index)
		return 0;

	if (nCount <= index)
		return 0;

	return m_aObjects[index].pObj;
}

IObjBase* CPhysXObjSelGroup::GetFocusObjectTopAncestor(int* pOutTopAncestorIdx) const
{
	if (0 == GetFocusObject())
	{
		if (0 != pOutTopAncestorIdx)
			*pOutTopAncestorIdx = -1;
		return 0;
	}

	if (m_aObjects[0].bIsIndependent || m_aObjects[0].bIsTopAncestor)
	{
		if (0 != pOutTopAncestorIdx)
			*pOutTopAncestorIdx = 0;
		return m_aObjects[0].pObj;
	}

	IObjBase* pTopAncestor = m_aObjects[0].pObj;
	ApxObjBase* pParent = pTopAncestor->GetParent();
	while (0 != pParent)
	{
		IObjBase* pParentObj = dynamic_cast<IObjBase*>(pParent);
		if (0 == pParentObj->GetProperties()->GetSelGroup())
			break;
		pTopAncestor = pParentObj;
		pParent = pParentObj->GetParent();
	}

	if (0 == pOutTopAncestorIdx)
		return pTopAncestor;

	int nAll = Size();
	for (int i = 0; i < nAll; ++i)
	{
		if (pTopAncestor == m_aObjects[i].pObj)
		{
			assert(true == (m_aObjects[i].bIsIndependent || m_aObjects[i].bIsTopAncestor));
			*pOutTopAncestorIdx = i;
			return pTopAncestor;
		}
	}

	assert(!"Shouldn't be here! Debug it!");
	return pTopAncestor;
}

int CPhysXObjSelGroup::GetTopAncestorObjects(std::vector<IObjBase*>& outObjs) const
{
	int nCount = 0;
	int nAll = Size();
	for (int i = 0; i < nAll; ++i)
	{
		if (IsEnableClone(i))
		{
			outObjs.push_back(m_aObjects[i].pObj);
			++nCount;
		}
	}
	return nCount;
}

void CPhysXObjSelGroup::AppendObject(IObjBase& obj)
{
	const IObjBase* pFocusBackup = GetFocusObject();

	IObjBase* pObj = &obj;
	size_t idx = m_aObjects.size();
	const size_t MaxCount = idx;
	for (unsigned int i = 0; i < MaxCount; ++i)
	{
		if(pObj == m_aObjects[i].pObj)
		{
			idx = i;
			break;
		}
	}

	if (idx == MaxCount)
	{
		OnAddObject(obj);
		SendToPropsGrid();
	}

	if (0 != idx)
	{
		SelObj temp = m_aObjects[0];
		m_aObjects[0] = m_aObjects[idx];
		m_aObjects[idx] = temp;
		RefreshSelObjOffset();
	}
	RefreshFrame();
	if (0 != m_pStateReport)
	{
		const IObjBase* pNewFocus = GetFocusObject();
		if (pFocusBackup != pNewFocus)
		{
			m_pStateReport->OnNotifySelChange(pFocusBackup, true, false);
			m_pStateReport->OnNotifySelChange(pNewFocus, true, true);
			m_pStateReport->OnUpdateStateRedraw();
		}
	}
	return;
}

void CPhysXObjSelGroup::RemoveObject(const IObjBase& obj)
{
	const IObjBase* pFocusBackup = GetFocusObject();

	const IObjBase* pObj = &obj;
	size_t idx = m_aObjects.size();
	const size_t MaxCount = idx;
	for (unsigned int i = 0; i < MaxCount; ++i)
	{
		if(pObj == m_aObjects[i].pObj)
		{
			idx = i;
			break;
		}
	}

	if (idx == MaxCount)
		return;

	IPropObjBase* pProps = obj.GetProperties();
	pProps->SetSelGroup(0);
	if (0 != m_pRHO)
		m_pRHO->OnDecorateObj(obj);

	SelObj SelObjRemoved = m_aObjects[idx];
	if (idx != MaxCount - 1)
	{
		m_aObjects[idx] = m_aObjects[MaxCount - 1];
		m_aObjects[MaxCount - 1] = SelObjRemoved;
	}
	m_aObjects.pop_back();
	UpdateFamilyInfoOnRemove(SelObjRemoved);
	SendToPropsGrid();

	if (0 < Size())
	{
		RefreshFrame();
		if (0 == idx)
			RefreshSelObjOffset();
	}

	if (0 != m_pStateReport)
	{
		m_pStateReport->OnNotifySelChange(&obj, false, false);
		const IObjBase* pNewFocus = GetFocusObject();
		if (pFocusBackup != pNewFocus)
			m_pStateReport->OnNotifySelChange(pNewFocus, true, true);
		m_pStateReport->OnUpdateStateRedraw();
	}
}

void CPhysXObjSelGroup::ReplaceWithObject(IObjBase* pObj)
{
	const size_t MaxCount = m_aObjects.size();
	if (1 == MaxCount)
	{
		if (pObj == m_aObjects[0].pObj)
			return;
	}

	for (unsigned int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->GetProperties()->SetSelGroup(0);
	if (0 < MaxCount)
	{
		if (0 != m_pRHO)
			m_pRHO->OnDecorateObj(*(m_aObjects[0].pObj));
	}
	if (0 != m_pStateReport)
	{
		for (unsigned int i = 0; i < MaxCount; ++i)
			m_pStateReport->OnNotifySelChange(m_aObjects[i].pObj, false, false);
	}

	m_aObjects.clear();
	if (0 != pObj)
	{
		OnAddObject(*pObj);
		RefreshFrame();
	}

	SendToPropsGrid();
	if (0 != m_pStateReport)
	{
		m_pStateReport->OnNotifySelChange(pObj, true, true);
		m_pStateReport->OnUpdateStateRedraw();
	}
}

void CPhysXObjSelGroup::ReplaceWithObjects(const std::vector<IObjBase*>& objs)
{
	const size_t nNewCount = objs.size();
	if (0 == nNewCount)
	{
		ReplaceWithObject(0);
		return;
	}

	if (1 == nNewCount)
	{
		ReplaceWithObject(objs[0]);
		return;
	}

	// new selection is equal to old selection
	IObjBase* pNewObj = 0;
	IObjBase* pOldObj = 0;
	const size_t nOldCount = m_aObjects.size();
	if (nOldCount == nNewCount)
	{
		bool bIsSame = true;
		for (unsigned int i = 0; i < nNewCount; ++i)
		{
			pNewObj = objs[i];
			pOldObj = m_aObjects[i].pObj;
			if (pNewObj != pOldObj)
			{
				bIsSame = false;
				break;
			}
		}
		if (bIsSame)
			return;
	}

	// clear old selection
	for (unsigned int i = 0; i < nOldCount; ++i)
		m_aObjects[i]->GetProperties()->SetSelGroup(0);
	if (0 < nOldCount)
	{
		if (0 != m_pRHO)
			m_pRHO->OnDecorateObj(*(m_aObjects[0].pObj));
	}
	if (0 != m_pStateReport)
	{
		for (unsigned int i = 0; i < nOldCount; ++i)
			m_pStateReport->OnNotifySelChange(m_aObjects[i].pObj, false, false);
	}
	m_aObjects.clear();

	// add new selection
	for (unsigned int i = 0; i < nNewCount; ++i)
	{
		pNewObj = objs[i];
		if (0 != pNewObj)
			OnAddObject(*pNewObj);
	}
	RefreshFrame();
	SendToPropsGrid();
	if (0 != m_pStateReport)
	{
		m_pStateReport->OnNotifySelChange(GetFocusObject(), true, true);
		for (unsigned int i = 1; i < nNewCount; ++i)
			m_pStateReport->OnNotifySelChange(m_aObjects[i].pObj, true, false);
		m_pStateReport->OnUpdateStateRedraw();
	}
}

void CPhysXObjSelGroup::OnNotifySleep(const IObjBase& obj)
{
	RemoveObject(obj);
}

void CPhysXObjSelGroup::RefreshFrame()
{
	RefreshFramePose();
	RefreshFrameScale();
}

bool CPhysXObjSelGroup::UpdateRefFrameOnLButtonDown(const int x, const int y, const unsigned int nFlags)
{
	if (!IsDrawItemEnable())
		return false;

	return m_focusRefFrame.OnLButtonDown(x, y, nFlags);
}

bool CPhysXObjSelGroup::UpdateRefFrameOnMouseMove(const int x, const int y, const unsigned int nFlags, bool OnlyRefFrame)
{
	if (!IsDrawItemEnable())
		return false;

	bool rtn = m_focusRefFrame.OnMouseMove(x, y, nFlags);
	if (OnlyRefFrame)
		return rtn;

	m_Status.RaiseFlag(SF_REFRESH_FRAME_DISABLE);
	const EditType optType = m_focusRefFrame.GetEditType();
	if (CCoordinateDirection::EDIT_TYPE_MOVE == optType)
	{
		SetGroupPos(m_focusRefFrame.GetPos(), true);
	}
	else if (CCoordinateDirection::EDIT_TYPE_ROTATE == optType)
	{
		float angRad = 0.0f;
		A3DVECTOR3 localAxis(0.0f);
		A3DVECTOR3 globalAxis(0.0f);
		bool rtn = m_focusRefFrame.GetLastRotateInfo(angRad, localAxis, globalAxis);
		assert(true == rtn);
		SetGroupRot(angRad, localAxis, globalAxis);
	}
	else if (CCoordinateDirection::EDIT_TYPE_SCALE == optType)
	{
		float scale = 0.0f;
		bool rtn = m_focusRefFrame.GetLastScaleChange(scale);
		assert(true == rtn);
		if (!NxMath::equals(scale, 0.0f, APHYSX_FLOAT_EPSILON))
			SetGroupScale();
	}
	m_Status.ClearFlag(SF_REFRESH_FRAME_DISABLE);
	return rtn;
}

void CPhysXObjSelGroup::UpdatePropItems()
{
	if (0 == m_pPropList)
		return;
	const size_t nCount = m_aObjects.size();
	if (0 == nCount)
		return;
	if (!m_Status.ReadFlag(SF_HAS_SOMETHING_DIRTY))
		return;

	int idx = 0;
	while (idx < PID_END_BOUND - PID_START_BOUND)
	{
		if (m_Props[idx].ReadFlag(PF_TO_DELETE))
		{
			idx = DeleteProps(idx, true);
		}
		else
		{
			assert(false == m_Props[idx].ReadFlag(PF_HAS_NEW_VALUE));
			++idx;
		}
	}

	for (unsigned int i = 0; i < nCount; ++i)
		m_aObjects[i]->GetProperties()->OnUpdatePropsGrid();

	UpdateSocketNames();
	CommitModified();
}

bool CPhysXObjSelGroup::PropertyChanged(PropItem& prop)
{
	if (0 == m_pPropList)
		return false;

	const DWORD_PTR propID = prop.GetData();
	int idx = GetIndex(PropID(propID));

	m_Props[idx].RaiseFlag(PF_IS_LOCKED);
	size_t nCount = m_aObjects.size();
	for (unsigned int i = 0; i < nCount; ++i)
		m_aObjects[i]->GetProperties()->OnPropertyChanged(prop);
	m_Props[idx].ClearFlag(PF_IS_LOCKED);

	const DWORD_PTR propValue = prop.GetData();
	if (PID_SGITEM_RBS_Type == propValue)
	{
		NxForceFieldType fft;
		bool bRtnValue = CPhysXObjForceField::GetFFScale(_bstr_t(prop.GetValue()), fft);
		if (bRtnValue)
		{
			if (NX_FF_TYPE_GRAVITATIONAL == fft)
				EnablePropItem(PID_SGITEM_RBS_Value, false);
			else
				EnablePropItem(PID_SGITEM_RBS_Value, true);
		}
	}
	else if (PID_SGITEM_ClothS_Type == propValue)
	{
		NxForceFieldType fft;
		bool bRtnValue = CPhysXObjForceField::GetFFScale(_bstr_t(prop.GetValue()), fft);
		if (bRtnValue)
		{
			if (NX_FF_TYPE_GRAVITATIONAL == fft)
				EnablePropItem(PID_SGITEM_ClothS_Value, false);
			else
				EnablePropItem(PID_SGITEM_ClothS_Value, true);
		}
	}
	else if (PID_SGITEM_FluidS_Type == propValue)
	{
		NxForceFieldType fft;
		bool bRtnValue = CPhysXObjForceField::GetFFScale(_bstr_t(prop.GetValue()), fft);
		if (bRtnValue)
		{
			if (NX_FF_TYPE_GRAVITATIONAL == fft)
				EnablePropItem(PID_SGITEM_FluidS_Value, false);
			else
				EnablePropItem(PID_SGITEM_FluidS_Value, true);
		}
	}
	else if (PID_SGITEM_SBS_Type == propValue)
	{
		NxForceFieldType fft;
		bool bRtnValue = CPhysXObjForceField::GetFFScale(_bstr_t(prop.GetValue()), fft);
		if (bRtnValue)
		{
			if (NX_FF_TYPE_GRAVITATIONAL == fft)
				EnablePropItem(PID_SGITEM_SBS_Value, false);
			else
				EnablePropItem(PID_SGITEM_SBS_Value, true);
		}
	}

	if (PID_ITEM_LocalName == propValue)
		RaiseUpdateMark(PID_ITEM_LocalName);
	else if (PID_ITEM_Scale == propValue)
		RefreshFrameScale();
	else if ((PID_GROUP_PoseScale < propValue) && (PID_SUBGROUP_Dir_xyz > propValue))
		RefreshFramePose(true);

	if (0 < nCount)
		return true;
	return false;
}

bool CPhysXObjSelGroup::ReadUpdateMark(const PropID& propID)
{
	int idx = GetIndex(propID);
	return m_Props[idx].ReadFlag(PF_IS_DIRTY);
}

bool CPhysXObjSelGroup::RaiseUpdateMark(const PropID& propID, bool bToDel)
{
	int idx = GetIndex(propID);
	if (m_Props[idx].ReadFlag(PF_IS_LOCKED))
		return false;

	m_Props[idx].nCount = 0;
	m_Props[idx].RaiseFlag(PF_IS_DIRTY);
	m_Status.RaiseFlag(SF_HAS_SOMETHING_DIRTY);

	if (bToDel)
		m_Props[idx].RaiseFlag(PF_TO_DELETE);

	size_t nCount = m_Props[idx].pChildren.size();
	for (size_t i = 0; i < nCount; ++i)
		RaiseUpdateMark(PropID(PID_START_BOUND + m_Props[idx].pChildren[i]), bToDel);
	return true;
}

bool CPhysXObjSelGroup::RaiseUpdateMarkToCCFlags()
{
	RaiseUpdateMark(PID_ITEM_LWCC_RevisePosEOpen, true);
	RaiseUpdateMark(PID_ITEM_CCDCC_Slide, true);
	RaiseUpdateMark(PID_ITEM_CC_PushForce, true);
	RaiseUpdateMark(PID_ITEM_CC_StandForce, true);
	RaiseUpdateMark(PID_ITEM_DynCC_AutoHangEnd, true);
	RaiseUpdateMark(PID_ITEM_DynCC_PassiveEffect, true);
	RaiseUpdateMark(PID_ITEM_CCDCC_AttachEOpen, true);
	RaiseUpdateMark(PID_ITEM_CCDCC_AttachEClose, true);
	return true;
}

bool CPhysXObjSelGroup::RaiseUpdateMarkToShapeVolume(CRegion::ShapeType st, bool bToDel)
{
	switch (st)
	{
	case CRegion::ST_BOX:
		RaiseUpdateMark(PID_SUBGROUP_Extent_xyz, bToDel);
		break;
	case CRegion::ST_SPHERE:
		RaiseUpdateMark(PID_ITEM_Radius, bToDel);
		break;
	case CRegion::ST_CAPSULE:
		RaiseUpdateMark(PID_ITEM_Radius, bToDel);
		RaiseUpdateMark(PID_ITEM_Height, bToDel);
		break;
	default:
		assert(!"Unknown ShapeType!");
	}
	return true;
}

bool CPhysXObjSelGroup::AllSelectedHaveTheSameShapeType() const
{
	size_t MaxCount = m_aObjects.size();
	if (0 == MaxCount)
		return false;

	ObjManager::ObjTypeID objType = m_aObjects[0].pObj->GetProperties()->GetObjType();
	for (unsigned int i = 1; i < MaxCount; ++i)
	{
		if (objType != m_aObjects[i].pObj->GetProperties()->GetObjType())
			return false;
	}


	CRegion::ShapeType stTemp = CRegion::ST_BOX;
	CRegion::ShapeType stFirst = CRegion::ST_BOX;
	if (ObjManager::OBJ_TYPEID_FORCEFIELD == objType)
	{
		CPhysXObjForceField* pFFObj = dynamic_cast<CPhysXObjForceField*>(m_aObjects[0].pObj);
		CPhysXObjForceField::GetShapeInfo(*(pFFObj->GetAPhysXFFDesc()), stFirst);
		for (unsigned int i = 1; i < MaxCount; ++i)
		{
			CPhysXObjForceField* pFFT = dynamic_cast<CPhysXObjForceField*>(m_aObjects[i].pObj);
			CPhysXObjForceField::GetShapeInfo(*(pFFT->GetAPhysXFFDesc()), stTemp);
			if (stTemp != stFirst)
				return false;
		}
	}
	else if (ObjManager::OBJ_TYPEID_REGION == objType)
	{
		CRegion* pRgn = dynamic_cast<CRegion*>(m_aObjects[0].pObj);
		stFirst = pRgn->GetShapeType();
		for (unsigned int i = 1; i < MaxCount; ++i)
		{
			CRegion* pRT = dynamic_cast<CRegion*>(m_aObjects[i].pObj);
			stTemp = pRT->GetShapeType();
			if (stTemp != stFirst)
				return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool CPhysXObjSelGroup::IsMerge(const IObjBase* pObj) const
{
	if (0 == pObj)
		return false;
	if (pObj == GetFocusObject())
		return false;
	return true;
}

void CPhysXObjSelGroup::ExecuteItemEnable(const PropID& nID, bool bEnable)
{
	int idx = nID - PID_START_BOUND;
	if (0 != m_Props[idx].pItem)
	{
		if (bEnable)
			m_Props[idx].pItem->Enable(TRUE);
		else
			m_Props[idx].pItem->Enable(FALSE);
	}
}

void CPhysXObjSelGroup::ExecuteUpdateItem(const PropID& nID, const _variant_t& objVal, bool bIsMerge)
{
	int idx = IncCheckinCount(nID, false);
	if (m_Props[idx].ReadFlag(PF_IS_GROUP))
		return;

	PropItem* pProp = m_Props[idx].pItem;
	if (0 != pProp)
	{
		m_Props[idx].SetNewValue(objVal, bIsMerge);
		return;
	}

	if (!bIsMerge)
	{
		if (0 != CreateCCFlagProp(nID, objVal))
			return;
		CreateRegionVolumeProp(nID, objVal);
	}
}

void CPhysXObjSelGroup::ExecuteUpdateItemToShapeVolume(CRegion::ShapeType st, const A3DVECTOR3& vol, bool bIsMerge)
{
	switch (st)
	{
	case CRegion::ST_BOX:
		ExecuteUpdateItem(PID_SUBGROUP_Extent_xyz, varEmpty, bIsMerge);
		ExecuteUpdateItem(PID_SGITEM_Extent_x, vol.x, bIsMerge);
		ExecuteUpdateItem(PID_SGITEM_Extent_y, vol.y, bIsMerge);
		ExecuteUpdateItem(PID_SGITEM_Extent_z, vol.z, bIsMerge);
		break;
	case CRegion::ST_SPHERE:
		ExecuteUpdateItem(PID_ITEM_Radius, vol.x, bIsMerge);
		break;
	case CRegion::ST_CAPSULE:
		ExecuteUpdateItem(PID_ITEM_Radius, vol.x, bIsMerge);
		ExecuteUpdateItem(PID_ITEM_Height, vol.y, bIsMerge);
		break;
	default:
		assert(!"Unknown ShapeType!");
	}
}

void CPhysXObjSelGroup::FillGroupBasic(const IPropObjBase& objProp)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;

	const char*  pObjName  = objProp.GetObjName_cstr();
	const char*  pObjLocalName  = objProp.GetObjLocalName_cstr();
	const TCHAR* pTypeID   = ObjManager::GetObjTypeIDText(objProp.GetObjType());
	const TCHAR* pState    = IPropObjBase::GetObjStateText(objProp.GetObjState());
	AString StrSripts = objProp.GetScriptNames();
 
//	const char*  pFileName = objProp.GetFilePathName_cstr(); tp update

	if (!IsMerge(objProp.GetHostObject()))
	{
		pGroup = m_pPropList->CreatePropItem(_T("Basic"), PID_GROUP_Basic);
		SetChiefInfo(PID_GROUP_Basic, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("(Name)"), pObjName, _T("returns the name of the object."), PID_ITEM_Name);
		pProp->Enable(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Name, *pProp, PID_GROUP_Basic, false);
		pProp = m_pPropList->CreatePropItem(_T("LocalName"), pObjLocalName, _T("returns the local name of the object."), PID_ITEM_LocalName);
		if (0 == objProp.GetHostObject()->GetParent())
			pProp->Enable(FALSE);
		else
			pProp->Enable(TRUE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_LocalName, *pProp, PID_GROUP_Basic, false, &PropItem::varMultiFlag);
		pProp = m_pPropList->CreatePropItem(_T("Type"), pTypeID, _T("returns the type of the object."), PID_ITEM_Type);
		pProp->Enable(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Type, *pProp, PID_GROUP_Basic, false);
		pProp = m_pPropList->CreatePropItem(_T("State"), pState, _T("returns the state of the object."), PID_ITEM_State);
		pProp->Enable(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_State, *pProp, PID_GROUP_Basic, false);
		pProp = m_pPropList->CreatePropItem(_T("Script"), StrSripts.operator const char*(), _T("Binding script."), PID_ITEM_Scripts_Attached, TRUE);
		pProp->Enable(TRUE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Scripts_Attached, *pProp, PID_GROUP_Basic, false);
//		pProp = new CBCGPProp(_T("File"), pFileName, _T("returns the path and file name of the object model."), PID_ITEM_File);
//		pProp->Enable(FALSE);
//		pGroup->AddSubItem(pProp);
//		SetItemInfo(PID_ITEM_File, *pProp, pGroup, false);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_Basic)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_Basic, true);
		ExecuteUpdateItem(PID_ITEM_Name, pObjName, true);
		ExecuteUpdateItem(PID_ITEM_LocalName, pObjLocalName, true);
		m_Props[PID_ITEM_LocalName - PID_START_BOUND].pItem->Enable(FALSE);
		ExecuteUpdateItem(PID_ITEM_Type, pTypeID, true);
		ExecuteUpdateItem(PID_ITEM_State, pState, true);
		ExecuteUpdateItem(PID_ITEM_Scripts_Attached, StrSripts.operator const char*(), true);
//		ExecuteUpdateItem(PID_ITEM_File, pFileName, true);
	}
}

void CPhysXObjSelGroup::FillGroupDrivenBy(const IPropPhysXObjBase& objProp)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;

	const TCHAR* pCurrentDM = IPropPhysXObjBase::GetDrivenModeText(objProp.GetDrivenMode(false));
	const TCHAR* pSimPlayDM = IPropPhysXObjBase::GetDrivenModeText(objProp.GetDrivenMode(true));

	if (!IsMerge(objProp.GetHostObject()))
	{
		pGroup = m_pPropList->CreatePropItem(_T("DrivenBy"), PID_GROUP_DrivenBy);
		SetChiefInfo(PID_GROUP_DrivenBy, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("Current"), pCurrentDM, _T("returns current driven type of the object."), PID_ITEM_CurntDVN);
		pProp->Enable(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_CurntDVN, *pProp, PID_GROUP_DrivenBy, false);
		pProp = m_pPropList->CreatePropItem(_T("OnSim/Play"), pSimPlayDM, _T("Sets/returns the default driven type while the object enter sim/play mode."), PID_ITEM_OnSimDVN);
		pProp->AddOption(IPropPhysXObjBase::GetDrivenModeText(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX));
		pProp->AddOption(IPropPhysXObjBase::GetDrivenModeText(IPropPhysXObjBase::DRIVEN_BY_ANIMATION));
		pProp->AddOption(IPropPhysXObjBase::GetDrivenModeText(IPropPhysXObjBase::DRIVEN_BY_PART_PHYSX));
		pProp->AllowEdit(FALSE);
		pProp->Enable(m_Status.ReadFlag(SF_IS_GAME_RUNTIME)? FALSE : TRUE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_OnSimDVN, *pProp, PID_GROUP_DrivenBy, false);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_DrivenBy)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_DrivenBy, true);
		ExecuteUpdateItem(PID_ITEM_CurntDVN, pCurrentDM, true);
		ExecuteUpdateItem(PID_ITEM_OnSimDVN, pSimPlayDM, true);
	}
}

void CPhysXObjSelGroup::FillGroupPoseScale(const IPropObjBase& objProp)
{
	assert(0 != m_pPropList);
	PropItem* pI = 0;
	PropItem* pGroup = 0;

	IObjBase* pHostObj = objProp.GetHostObject();
	NxVec3 pos = pHostObj->GetLPos();
	A3DVECTOR3 dir, up;
	pHostObj->GetLDirAndUp(dir, up);
	float scale = pHostObj->GetLScale();

	if (!IsMerge(objProp.GetHostObject()))
	{
		const _variant_t* pAimVal = &PropItem::varMultiFlag;
		pGroup = m_pPropList->CreatePropItem(_T("Pose&Scale"), PID_GROUP_PoseScale);
		SetChiefInfo(PID_GROUP_PoseScale, pGroup);
		PropItem* pPos = m_pPropList->CreatePropItem(_T("pos"), PID_SUBGROUP_Pos_xyz, TRUE);
		pGroup->AddSubItem(pPos);
		SetItemInfo(PID_SUBGROUP_Pos_xyz, *pPos, PID_GROUP_PoseScale, true, pAimVal);
		pI = m_pPropList->CreatePropItem(_T("x"), pos.x, _T("Specifies the x-axix value of position."), PID_SGITEM_Pos_x);
		pPos->AddSubItem(pI);
		pPos->Expand();
		SetItemInfo(PID_SGITEM_Pos_x, *pI, PID_SUBGROUP_Pos_xyz, false, pAimVal);
		pI = m_pPropList->CreatePropItem(_T("y"), pos.y, _T("Specifies the y-axix value of position."), PID_SGITEM_Pos_y);
		pPos->AddSubItem(pI);
		SetItemInfo(PID_SGITEM_Pos_y, *pI, PID_SUBGROUP_Pos_xyz, false, pAimVal);
		pI = m_pPropList->CreatePropItem(_T("z"), pos.z, _T("Specifies the z-axix value of position."), PID_SGITEM_Pos_z);
		pPos->AddSubItem(pI);
		SetItemInfo(PID_SGITEM_Pos_z, *pI, PID_SUBGROUP_Pos_xyz, false, pAimVal);
		pPos->Enable(m_Status.ReadFlag(SF_IS_GAME_RUNTIME)? FALSE : TRUE);
		PropItem* pDir = m_pPropList->CreatePropItem(_T("dir"), PID_SUBGROUP_Dir_xyz, TRUE);
		pGroup->AddSubItem(pDir);
		SetItemInfo(PID_SUBGROUP_Dir_xyz, *pDir, PID_GROUP_PoseScale, true, pAimVal);
		pI = m_pPropList->CreatePropItem(_T("x"), dir.x, _T("Specifies the x-axix value of face direction."), PID_SGITEM_Dir_x);
		pI->Enable(FALSE);
		pDir->AddSubItem(pI);
		SetItemInfo(PID_SGITEM_Dir_x, *pI, PID_SUBGROUP_Dir_xyz, false, pAimVal);
		pI = m_pPropList->CreatePropItem(_T("y"), dir.y, _T("Specifies the y-axix value of face direction."), PID_SGITEM_Dir_y);
		pI->Enable(FALSE);
		pDir->AddSubItem(pI);
		SetItemInfo(PID_SGITEM_Dir_y, *pI, PID_SUBGROUP_Dir_xyz, false, pAimVal);
		pI = m_pPropList->CreatePropItem(_T("z"), dir.z, _T("Specifies the z-axix value of face direction."), PID_SGITEM_Dir_z);
		pI->Enable(FALSE);
		pDir->AddSubItem(pI);
		SetItemInfo(PID_SGITEM_Dir_z, *pI, PID_SUBGROUP_Dir_xyz, false, pAimVal);
		pDir->Enable(FALSE);
		PropItem* pUp = m_pPropList->CreatePropItem(_T("up"), PID_SUBGROUP_Up_xyz, TRUE);
		pGroup->AddSubItem(pUp);
		SetItemInfo(PID_SUBGROUP_Up_xyz, *pUp, PID_GROUP_PoseScale, true, pAimVal);
		pI = m_pPropList->CreatePropItem(_T("x"), up.x, _T("Specifies the x-axix value of up direction."), PID_SGITEM_Up_x);
		pI->Enable(FALSE);
		pUp->AddSubItem(pI);
		SetItemInfo(PID_SGITEM_Up_x, *pI, PID_SUBGROUP_Up_xyz, false, pAimVal);
		pI = m_pPropList->CreatePropItem(_T("y"), up.y, _T("Specifies the y-axix value of up direction."), PID_SGITEM_Up_y);
		pI->Enable(FALSE);
		pUp->AddSubItem(pI);
		SetItemInfo(PID_SGITEM_Up_y, *pI, PID_SUBGROUP_Up_xyz, false, pAimVal);
		pI = m_pPropList->CreatePropItem(_T("z"), up.z, _T("Specifies the z-axix value of up direction."), PID_SGITEM_Up_z);
		pI->Enable(FALSE);
		pUp->AddSubItem(pI);
		SetItemInfo(PID_SGITEM_Up_z, *pI, PID_SUBGROUP_Up_xyz, false, pAimVal);
		pUp->Enable(FALSE);
		pI = m_pPropList->CreatePropItem(_T("scale"), scale, _T("Specifies the scale of the object."), PID_ITEM_Scale);
		pI->Enable(m_Status.ReadFlag(SF_IS_GAME_RUNTIME)? FALSE : TRUE);
		pGroup->AddSubItem(pI);
		SetItemInfo(PID_ITEM_Scale, *pI, PID_GROUP_PoseScale, false, pAimVal);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_PoseScale)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_PoseScale, true);
		ExecuteUpdateItem(PID_SUBGROUP_Pos_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_Pos_x, pos.x, true);
		ExecuteUpdateItem(PID_SGITEM_Pos_y, pos.y, true);
		ExecuteUpdateItem(PID_SGITEM_Pos_z, pos.z, true);
		ExecuteUpdateItem(PID_SUBGROUP_Dir_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_Dir_x, dir.x, true);
		ExecuteUpdateItem(PID_SGITEM_Dir_y, dir.y, true);
		ExecuteUpdateItem(PID_SGITEM_Dir_z, dir.z, true);
		ExecuteUpdateItem(PID_SUBGROUP_Up_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_Up_x, up.x, true);
		ExecuteUpdateItem(PID_SGITEM_Up_y, up.y, true);
		ExecuteUpdateItem(PID_SGITEM_Up_z, up.z, true);
		ExecuteUpdateItem(PID_ITEM_Scale, scale, true);
	}
}

void CPhysXObjSelGroup::FillGroupShapeVolume(const CRegion& rgn)
{
	A3DVECTOR3 vol = rgn.GetVolume(false);
	CRegion::ShapeType st = rgn.GetShapeType();
	FillGroupShapeVolume(st, vol, IsMerge(&rgn));
}

void CPhysXObjSelGroup::FillGroupShapeVolume(const CRegion::ShapeType st, const A3DVECTOR3& vol, bool bIsMerge)
{
	assert(0 != m_pPropList);
	PropItem* pI = 0;
	PropItem* pGroup = 0;
	const TCHAR* pST = CRegion::GetShapeTypeText(st);

	if (!bIsMerge)
	{
		const _variant_t* pAimVal = &PropItem::varMultiFlag;
		pGroup = m_pPropList->CreatePropItem(_T("Shape&Volume"), PID_GROUP_ShapeVolume);
		SetChiefInfo(PID_GROUP_ShapeVolume, pGroup);
		pI = m_pPropList->CreatePropItem(_T("Shape"), pST, _T("Returns the shape type of the object."), PID_ITEM_Shape);
		pI->AddOption(CRegion::GetShapeTypeText(CRegion::ST_BOX));
		pI->AddOption(CRegion::GetShapeTypeText(CRegion::ST_SPHERE));
		pI->AddOption(CRegion::GetShapeTypeText(CRegion::ST_CAPSULE));
		pI->AllowEdit(FALSE);
		pGroup->AddSubItem(pI);
		SetItemInfo(PID_ITEM_Shape, *pI, PID_GROUP_ShapeVolume, false);
		if (CRegion::ST_BOX == st)
		{
			CreateRegionVolumeProp(PID_SUBGROUP_Extent_xyz, varEmpty);
			CreateRegionVolumeProp(PID_SGITEM_Extent_x, vol.x);
			CreateRegionVolumeProp(PID_SGITEM_Extent_y, vol.y);
			CreateRegionVolumeProp(PID_SGITEM_Extent_z, vol.z);
			PropItem* pSubGroup = m_Props[PID_SUBGROUP_Extent_xyz - PID_START_BOUND].pItem;
			if (0 != pSubGroup)
				pSubGroup->Expand();
			return;
		}
		CreateRegionVolumeProp(PID_ITEM_Radius, vol.x);
		if (CRegion::ST_CAPSULE == st)
			CreateRegionVolumeProp(PID_ITEM_Height, vol.y);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_ShapeVolume)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_ShapeVolume, true);
		ExecuteUpdateItem(PID_ITEM_Shape, st, true);
		ExecuteUpdateItemToShapeVolume(st, vol, true);
	}
}

PropItem* CPhysXObjSelGroup::CreateRegionVolumeProp(const PropID& nID, const _variant_t& objVal)
{
	int idxGroup = PID_GROUP_ShapeVolume - PID_START_BOUND;
	PropItem* pGroup = m_Props[idxGroup].pItem;
	if (0 == pGroup)
		return 0;
	int idxSubGroup = PID_SUBGROUP_Extent_xyz - PID_START_BOUND;
	PropItem* pSubGroup = m_Props[idxSubGroup].pItem;

	bool isGroup = false;
	PropItem* pProp = 0;
	PropItem* pParent = 0;
	PropID parentID = PID_START_BOUND;
	const _variant_t* pAimVal = &PropItem::varMultiFlag;
	switch(nID)
	{
	case PID_SUBGROUP_Extent_xyz:
		pProp = m_pPropList->CreatePropItem(_T("extents"), nID, TRUE);
		isGroup = true;
		pParent = pGroup;
		parentID = PID_GROUP_ShapeVolume;
		break;
	case PID_SGITEM_Extent_x:
		pProp = m_pPropList->CreatePropItem(_T("x"), objVal, _T("Specifies the x-axix value of volume."), nID);
		pParent = pSubGroup;
		parentID = PID_SUBGROUP_Extent_xyz;
		break;
	case PID_SGITEM_Extent_y:
		pProp = m_pPropList->CreatePropItem(_T("y"), objVal, _T("Specifies the y-axix value of volume."), nID);
		pParent = pSubGroup;
		parentID = PID_SUBGROUP_Extent_xyz;
		break;
	case PID_SGITEM_Extent_z:
		pProp = m_pPropList->CreatePropItem(_T("z"), objVal, _T("Specifies the z-axix value of volume."), nID);
		pParent = pSubGroup;
		parentID = PID_SUBGROUP_Extent_xyz;
		break;
	case PID_ITEM_Radius:
		pProp = m_pPropList->CreatePropItem(_T("radius"), objVal, _T("Specifies the radius of the object."), nID);
		pParent = pGroup;
		parentID = PID_GROUP_ShapeVolume;
		break;
	case PID_ITEM_Height:
		pProp = m_pPropList->CreatePropItem(_T("height"), objVal, _T("Specifies the cylander height of the object."), nID);
		pParent = pGroup;
		parentID = PID_GROUP_ShapeVolume;
		break;
	default:
		assert(!"Wrong PropID input!");
		break;
	}

	if (0 != pProp)
	{
		assert(0 != pParent);
		pParent->AddSubItem(pProp);
		SetItemInfo(nID, *pProp, parentID, isGroup, pAimVal);
		if (!m_Props[idxGroup].ReadFlag(PF_HAS_NEW_VALUE))
			m_Status.RaiseFlag(SF_PROPLIST_LAYOUT_DIRTY);
	}
	return pProp;
}
void CPhysXObjSelGroup::FillGroupRoleBehavior(const CDynProperty& objProp)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;

	const TCHAR* pRT = CDynProperty::GetRoleTypeText(objProp.GetRoleType());
	const TCHAR* pAPT = IActionPlayerBase::GetAPTypeText(objProp.GetAPType());
	const bool bHasPB = objProp.GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY);

	if (!IsMerge(objProp.GetHostObject()))
	{
		pGroup = m_pPropList->CreatePropItem(_T("Role&Behavior"), PID_GROUP_RoleBehavior);
		SetChiefInfo(PID_GROUP_RoleBehavior, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("Role"), pRT, _T("Sets/returns the role type of the object."), PID_ITEM_Role);
		pProp->AddOption(CDynProperty::GetRoleTypeText(CDynProperty::ROLE_ACTOR_NONE));
		pProp->AddOption(CDynProperty::GetRoleTypeText(CDynProperty::ROLE_ACTOR_NPC));
		pProp->AllowEdit(FALSE);
		pProp->Enable(m_Status.ReadFlag(SF_IS_GAME_RUNTIME)? FALSE : TRUE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Role, *pProp, PID_GROUP_RoleBehavior, false);
		pProp = m_pPropList->CreatePropItem(_T("Behavior"), pAPT, _T("Sets/returns the object's behavior."), PID_ITEM_Behavior);
		pProp->AddOption(IActionPlayerBase::GetAPTypeText(IActionPlayerBase::AP_NULL));
		pProp->AddOption(IActionPlayerBase::GetAPTypeText(IActionPlayerBase::AP_SEQUENCE_ACTION));
		pProp->AddOption(IActionPlayerBase::GetAPTypeText(IActionPlayerBase::AP_WALK_RANDOM));
		pProp->AddOption(IActionPlayerBase::GetAPTypeText(IActionPlayerBase::AP_WALK_AROUND));
		pProp->AddOption(IActionPlayerBase::GetAPTypeText(IActionPlayerBase::AP_WALK_PATROL));
		pProp->AllowEdit(FALSE);
		if (objProp.QueryRoleType(CDynProperty::ROLE_ACTOR_NONE))
			pProp->Enable(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Behavior, *pProp, PID_GROUP_RoleBehavior, false);
		pProp = m_pPropList->CreatePropItem(_T("PhysX Body"), bHasPB, _T("Enable/disable the object's PhysX body."), PID_ITEM_PhysX_BodyEnable);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_PhysX_BodyEnable, *pProp, PID_GROUP_RoleBehavior, false);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_RoleBehavior)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_RoleBehavior, true);
		ExecuteUpdateItem(PID_ITEM_Role, pRT, true);
		ExecuteUpdateItem(PID_ITEM_Behavior, pAPT, true);
		OnUpdateBehaviorEnable(objProp);
		ExecuteUpdateItem(PID_ITEM_PhysX_BodyEnable, bHasPB, true);
	}
}

void CPhysXObjSelGroup::FillGroupApxRBFlags(const CDynProperty& objProp)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;

	const int flags = objProp.GetApxRBFlags();
	const bool bPF = (flags & APX_OBJFLAG_CC_ADD_PUSHFORCE)? true : false;
	const bool bSF = (flags & APX_OBJFLAG_CC_ADD_STANDFORCE)? true : false;
	const bool bCA = (flags & APX_OBJFLAG_CC_CAN_ATTACH)? true : false;

	if (!IsMerge(objProp.GetHostObject()))
	{
		pGroup = m_pPropList->CreatePropItem(_T("APhysX RB Flags"), PID_GROUP_Apx_RBFlags);
		SetChiefInfo(PID_GROUP_Apx_RBFlags, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("APX Push Force"), bPF, _T("Sets/gets APhysXObjectFlag flag: APX_OBJFLAG_CC_ADD_PUSHFORCE."), PID_ITEM_Apx_PushForce);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Apx_PushForce, *pProp, PID_GROUP_Apx_RBFlags, false);
		pProp = m_pPropList->CreatePropItem(_T("APX Stand Force"), bSF, _T("Sets/gets APhysXObjectFlag flag: APX_OBJFLAG_CC_ADD_STANDFORCE."), PID_ITEM_Apx_StandForce);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Apx_StandForce, *pProp, PID_GROUP_Apx_RBFlags, false);
		pProp = m_pPropList->CreatePropItem(_T("APX Attach Enable"), bCA, _T("Sets/gets APhysXObjectFlag flag: APX_OBJFLAG_CC_CAN_ATTACH."), PID_ITEM_Apx_AttachEnable);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Apx_AttachEnable, *pProp, PID_GROUP_Apx_RBFlags, false);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_Apx_RBFlags)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_Apx_RBFlags, true);
		ExecuteUpdateItem(PID_ITEM_Apx_PushForce, bPF, true);
		ExecuteUpdateItem(PID_ITEM_Apx_StandForce, bSF, true);
		ExecuteUpdateItem(PID_ITEM_Apx_AttachEnable, bCA, true);
	}
}

void CPhysXObjSelGroup::FillGroupCCFlags(const CDynProperty& objProp)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;
	const CAPhysXCCMgr& ccMgr = objProp.GetCCMgr();
	const bool bIsMerge = IsMerge(objProp.GetHostObject());

	if (!bIsMerge)
	{
		pGroup = m_pPropList->CreatePropItem(_T("CC Flags"), PID_GROUP_CC_Flags);
		SetChiefInfo(PID_GROUP_CC_Flags, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("CC Type"), ccMgr.GetCCTypeText(), _T("Sets/gets the CC type of the object."), PID_ITEM_CC_Type);
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_CC_Type, *pProp, PID_GROUP_CC_Flags, false);
		OnUpdateCCTypeContent(objProp);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_CC_Flags)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_CC_Flags, true);
		ExecuteUpdateItem(PID_ITEM_CC_Type, varEmpty, true);
		OnUpdateCCTypeContent(objProp);
	}

	switch(ccMgr.GetCCType())
	{
	case CAPhysXCCMgr::CC_TOTAL_EMPTY:
		break;
	case CAPhysXCCMgr::CC_APHYSX_LWCC:
		ExecuteUpdateItem(PID_ITEM_LWCC_RevisePosEOpen, ccMgr.ReadFlag(APhysXLWCharCtrler::APX_LWCCF_REVISEPOSITION_ENFORCE_OPEN), bIsMerge);
		break;
	case CAPhysXCCMgr::CC_APHYSX_EXTCC:
		ExecuteUpdateItem(PID_ITEM_CCDCC_Slide, ccMgr.ReadFlagSlide(), bIsMerge);
		ExecuteUpdateItem(PID_ITEM_CC_PushForce, ccMgr.ReadFlag(APhysXCharacterController::APX_CCF_ADD_PUSHFORCE), bIsMerge);
		ExecuteUpdateItem(PID_ITEM_CC_StandForce, ccMgr.ReadFlag(APhysXCharacterController::APX_CCF_ADD_STANDFORCE), bIsMerge);
		ExecuteUpdateItem(PID_ITEM_CCDCC_AttachEOpen, ccMgr.ReadFlagAttachEnforceOpen(), bIsMerge);
		ExecuteUpdateItem(PID_ITEM_CCDCC_AttachEClose, ccMgr.ReadFlagAttachEnforceClose(), bIsMerge);
		break;
	case CAPhysXCCMgr::CC_APHYSX_DYNCC:
		ExecuteUpdateItem(PID_ITEM_CCDCC_Slide, ccMgr.ReadFlagSlide(), bIsMerge);
		ExecuteUpdateItem(PID_ITEM_DynCC_AutoHangEnd, ccMgr.ReadFlag(APhysXDynCharCtrler::APX_CCF_AUTO_HANG_END_ENABLE), bIsMerge);
		ExecuteUpdateItem(PID_ITEM_DynCC_PassiveEffect, ccMgr.ReadFlag(APhysXDynCharCtrler::APX_CCF_PASSIVE_EFFECT_ONLY), bIsMerge);
		ExecuteUpdateItem(PID_ITEM_CCDCC_AttachEOpen, ccMgr.ReadFlagAttachEnforceOpen(), bIsMerge);
		ExecuteUpdateItem(PID_ITEM_CCDCC_AttachEClose, ccMgr.ReadFlagAttachEnforceClose(), bIsMerge);
		break;
	default:
		break;
	}
}

void CPhysXObjSelGroup::FillGroupSocket(const ApxObjSocketBase& objSocket)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;
	RaiseUpdateMarkToSocketName();

	const TCHAR* pDesc = objSocket.GetDescText();
	const TCHAR* pName = objSocket.GetBoneHookName_cstr();
	const TCHAR* pType = ApxObjSocketBase::GetSocketTypeText(objSocket.GetSocketType());
	if (!IsMerge(&objSocket))
	{
		pGroup = m_pPropList->CreatePropItem(_T("Socket"), PID_GROUP_Socket);
		SetChiefInfo(PID_GROUP_Socket, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("DescText"), pDesc, _T("Returns the socket description."), PID_ITEM_SocketDesc);
		pProp->Enable(FALSE);
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_SocketDesc, *pProp, PID_GROUP_Socket, false);
		pProp = m_pPropList->CreatePropItem(_T("LinkName"), pName, _T("Sets/returns the name of bone/hook linked."), PID_ITEM_SocketName);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_SocketName, *pProp, PID_GROUP_Socket, false);
		pProp = m_pPropList->CreatePropItem(_T("SocketType"), pType, _T("Sets/returns the type of socket."), PID_ITEM_SocketType);
		pProp->AddOption(ApxObjSkinModelToSocket::GetSocketTypeText(ApxSM_SocketType_Bone));
		pProp->AddOption(ApxObjSkinModelToSocket::GetSocketTypeText(ApxSM_SocketType_Hook));
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_SocketType, *pProp, PID_GROUP_Socket, false);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_Socket)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_Socket, true);
		ExecuteUpdateItem(PID_ITEM_SocketDesc, pDesc, true);
		ExecuteUpdateItem(PID_ITEM_SocketName, pName, true);
		ExecuteUpdateItem(PID_ITEM_SocketType, pType, true);
	}
}

void CPhysXObjSelGroup::FillGroupSpotLight(const ApxObjSpotLight& objSpotLight)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;

	float fRange = objSpotLight.GetRange();
	float fTheta = objSpotLight.GetInnerAngle();
	float fPhi = objSpotLight.GetOuterAngle();
	A3DVECTOR3 atten = objSpotLight.GetAttenuation();
	A3DCOLORVALUE color = objSpotLight.GetColor();
	if (!IsMerge(&objSpotLight))
	{
		const _variant_t* pAimVal = &PropItem::varMultiFlag;
		pGroup = m_pPropList->CreatePropItem(_T("SpotLight"), PID_GROUP_SpotLight);
		SetChiefInfo(PID_GROUP_SpotLight, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("Range"), fRange, _T("Sets/returns the distance beyond which the light has no effect."), PID_ITEM_Range);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Range, *pProp, PID_GROUP_SpotLight, false);
		pProp = m_pPropList->CreatePropItem(_T("InnerAngle"), fTheta, _T("inner cone angle, in degree.[0, OuterAngle]"), PID_ITEM_InnerTheta);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_InnerTheta, *pProp, PID_GROUP_SpotLight, false);
		pProp = m_pPropList->CreatePropItem(_T("OuterAngle"), fPhi, _T("outer cone angle, in degree.[0, 90)"), PID_ITEM_OuterPhi);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_OuterPhi, *pProp, PID_GROUP_SpotLight, false);
		PropItem* pAttenuation = m_pPropList->CreatePropItem(_T("Attenuation"), PID_SUBGROUP_Attenuation_xyz, TRUE);
		pGroup->AddSubItem(pAttenuation);
		SetItemInfo(PID_SUBGROUP_Attenuation_xyz, *pAttenuation, PID_GROUP_SpotLight, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("a0"), atten.x, _T("Specifies the a0 how the light intensity changes over distance."), PID_SGITEM_Attenuation_x);
		pAttenuation->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Attenuation_x, *pProp, PID_SUBGROUP_Attenuation_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("a1"), atten.y, _T("Specifies the a1 how the light intensity changes over distance."), PID_SGITEM_Attenuation_y);
		pAttenuation->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Attenuation_y, *pProp, PID_SUBGROUP_Attenuation_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("a2"), atten.z, _T("Specifies the a1 how the light intensity changes over distance."), PID_SGITEM_Attenuation_z);
		pAttenuation->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Attenuation_z, *pProp, PID_SUBGROUP_Attenuation_xyz, false, pAimVal);
		pAttenuation->Expand();
		PropItem* pColor = m_pPropList->CreatePropItem(_T("Color"), PID_SUBGROUP_Color_rgba, TRUE);
		pGroup->AddSubItem(pColor);
		SetItemInfo(PID_SUBGROUP_Color_rgba, *pColor, PID_GROUP_SpotLight, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("Red"), color.r, _T("Specifies the red value.[0, 1]"), PID_SGITEM_Color_r);
		pColor->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Color_r, *pProp, PID_SUBGROUP_Color_rgba, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("Green"), color.g, _T("Specifies the green value.[0, 1]"), PID_SGITEM_Color_g);
		pColor->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Color_g, *pProp, PID_SUBGROUP_Color_rgba, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("Blue"), color.b, _T("Specifies the blue value.[0, 1]"), PID_SGITEM_Color_b);
		pColor->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Color_b, *pProp, PID_SUBGROUP_Color_rgba, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("Alpha"), color.a, _T("Specifies the alpha value.[0, 1]"), PID_SGITEM_Color_a);
		pColor->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Color_a, *pProp, PID_SUBGROUP_Color_rgba, false, pAimVal);
		pColor->Expand();
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_SpotLight)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_SpotLight, true);
		ExecuteUpdateItem(PID_ITEM_Range, fRange, true);
		ExecuteUpdateItem(PID_ITEM_InnerTheta, fTheta, true);
		ExecuteUpdateItem(PID_ITEM_OuterPhi, fPhi, true);
		ExecuteUpdateItem(PID_SUBGROUP_Attenuation_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_Attenuation_x, atten.x, true);
		ExecuteUpdateItem(PID_SGITEM_Attenuation_y, atten.y, true);
		ExecuteUpdateItem(PID_SGITEM_Attenuation_z, atten.z, true);
		ExecuteUpdateItem(PID_SUBGROUP_Color_rgba, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_Color_r, color.r, true);
		ExecuteUpdateItem(PID_SGITEM_Color_g, color.g, true);
		ExecuteUpdateItem(PID_SGITEM_Color_b, color.b, true);
		ExecuteUpdateItem(PID_SGITEM_Color_a, color.a, true);
	}
}

void CPhysXObjSelGroup::OnUpdateBehaviorEnable(const CDynProperty& objProp)
{
	PropItem* pProp = m_Props[PID_ITEM_Behavior].pItem;
	assert(0 != pProp);
	if (pProp->IsEnabled())
		return;

	// to update: remove behavior
	//if (CDynProperty::ROLE_ACTOR_NPC == objProp.GetRoleType())
	//	pProp->Enable(TRUE);
}

void CPhysXObjSelGroup::FillAllFFProps(const CPhysXObjForceField& objFF, bool bIncludeCommon)
{
	const APhysXForceFieldObjectDesc* pFFDesc = objFF.GetAPhysXFFDesc();
	if (0 == pFFDesc)
		return;

	bool bIsMerge = IsMerge(&objFF);
	const APhysXForceFieldObjectDesc& ffDesc = *pFFDesc;
	if (bIncludeCommon)
		FillFFCommon(ffDesc, bIsMerge);

	const APhysXObjectType ot = ffDesc.GetObjType();
	switch(ot)
	{
	case APX_OBJTYPE_FORCEFIELD_WIND:
		{
			const APhysXWindForceFieldObjectDesc& ffod = static_cast<const APhysXWindForceFieldObjectDesc&>(ffDesc);
			FillWindFF(ffod, bIsMerge);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_VORTEX:
		{
			const APhysXVortexForceFieldObjectDesc& ffod = static_cast<const APhysXVortexForceFieldObjectDesc&>(ffDesc);
			FillVortexFF(ffod, bIsMerge);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_GRAVITATION:
		{
			const APhysXGravitationForceFieldObjectDesc& ffod = static_cast<const APhysXGravitationForceFieldObjectDesc&>(ffDesc);
			FillGravitationFF(ffod, bIsMerge);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_EXPLOSION:
		{
			const APhysXExplosionForceFieldObjectDesc& ffod = static_cast<const APhysXExplosionForceFieldObjectDesc&>(ffDesc);
			FillExplosionFF(ffod, bIsMerge);
			EnablePropItem(PID_ITEM_Shape, false);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_BUOYANCY:
		{
			const APhysXBuoyancyForceFieldObjectDesc& ffod = static_cast<const APhysXBuoyancyForceFieldObjectDesc&>(ffDesc);
			FillBuoyancyFF(ffod, bIsMerge);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD_VACUUM:
		break;
	case APX_OBJTYPE_FORCEFIELD_BOSS:
		{
			const APhysXBossForceFieldObjectDesc& ffod = static_cast<const APhysXBossForceFieldObjectDesc&>(ffDesc);
			FillBossFF(ffod, bIsMerge);
		}
		break;
	case APX_OBJTYPE_FORCEFIELD:
		FillCustomFF(ffDesc, bIsMerge);
		break;
	}
}

void CPhysXObjSelGroup::FillFFCommon(const APhysXForceFieldObjectDesc& ffDesc, bool bIsMerge)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;
	PropItem* pSubGroup = 0;

	const APhysXObjectType ffType = ffDesc.GetObjType();
	const TCHAR* pFFType = CPhysXObjForceField::GetForceFieldTypeText(ffType);
	const TCHAR* pFFCoordinate = CPhysXObjForceField::GetFFCoordonateText(ffDesc.mFFCoordinates);
	A3DVECTOR3 vol(0);
	CRegion::ShapeType st = CRegion::ST_BOX;
	CPhysXObjForceField::GetShapeInfo(ffDesc, st, &vol);
	if (!bIsMerge)
	{
		pGroup = m_pPropList->CreatePropItem(_T("Common"), PID_GROUP_FF_Common);
		SetChiefInfo(PID_GROUP_FF_Common, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("FFType"), pFFType, _T("Sets/returns the type of force field."), PID_ITEM_FF_Type);
		pProp->AddOption(CPhysXObjForceField::GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_WIND));
		pProp->AddOption(CPhysXObjForceField::GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_VORTEX));
		pProp->AddOption(CPhysXObjForceField::GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_EXPLOSION));
		pProp->AddOption(CPhysXObjForceField::GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_GRAVITATION));
		pProp->AddOption(CPhysXObjForceField::GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_BUOYANCY));
		pProp->AddOption(CPhysXObjForceField::GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_VACUUM));
		pProp->AddOption(CPhysXObjForceField::GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD_BOSS));
		pProp->AddOption(CPhysXObjForceField::GetForceFieldTypeText(APX_OBJTYPE_FORCEFIELD));
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FF_Type, *pProp, PID_GROUP_FF_Common, false);
		pProp = m_pPropList->CreatePropItem(_T("Coordinate"), pFFCoordinate, _T("Sets/returns the coordinate of force field."), PID_ITEM_Coordinate);
		pProp->AddOption(CPhysXObjForceField::GetFFCoordonateText(NX_FFC_CARTESIAN));
		pProp->AddOption(CPhysXObjForceField::GetFFCoordonateText(NX_FFC_SPHERICAL));
		pProp->AddOption(CPhysXObjForceField::GetFFCoordonateText(NX_FFC_CYLINDRICAL));
		pProp->AddOption(CPhysXObjForceField::GetFFCoordonateText(NX_FFC_TOROIDAL));
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Coordinate, *pProp, PID_GROUP_FF_Common, false);
		FillGroupShapeVolume(st, vol, false);
		m_Props[PID_GROUP_ShapeVolume - PID_START_BOUND].pPrevious = &(m_Props[PID_GROUP_FF_Common - PID_START_BOUND]);
		pGroup = m_pPropList->CreatePropItem(_T("FFScale"), PID_GROUP_FF_Scale);
		SetChiefInfo(PID_GROUP_FF_Scale, pGroup);
		CreateFFScaleProp(PID_SUBGROUP_RBScale, varEmpty);
		CreateFFScaleProp(PID_SGITEM_RBS_Type, ffDesc.mRBType);
		pProp = CreateFFScaleProp(PID_SGITEM_RBS_Value, ffDesc.mRBForceScale);
		if (NX_FF_TYPE_OTHER != ffDesc.mRBType)
			pProp->Enable(FALSE);
		CreateFFScaleProp(PID_SUBGROUP_ClothScale, varEmpty);
		CreateFFScaleProp(PID_SGITEM_ClothS_Type, ffDesc.mClothType);
		pProp = CreateFFScaleProp(PID_SGITEM_ClothS_Value, ffDesc.mClothForceScale);
		if (NX_FF_TYPE_OTHER != ffDesc.mClothType)
			pProp->Enable(FALSE);
		CreateFFScaleProp(PID_SUBGROUP_FluidScale, varEmpty);
		CreateFFScaleProp(PID_SGITEM_FluidS_Type, ffDesc.mFluidType);
		pProp = CreateFFScaleProp(PID_SGITEM_FluidS_Value, ffDesc.mFluidForceScale);
		if (NX_FF_TYPE_OTHER != ffDesc.mFluidType)
			pProp->Enable(FALSE);
		CreateFFScaleProp(PID_SUBGROUP_SBScale, varEmpty);
		CreateFFScaleProp(PID_SGITEM_SBS_Type, ffDesc.mSBType);
		pProp = CreateFFScaleProp(PID_SGITEM_SBS_Value, ffDesc.mSBForceScale);
		if (NX_FF_TYPE_OTHER != ffDesc.mSBType)
			pProp->Enable(FALSE);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_FF_Common)].pItem;
		if (0 != pGroup)
		{
			IncCheckinCount(PID_GROUP_FF_Common, true);
			ExecuteUpdateItem(PID_ITEM_FF_Type, pFFType, true);
			ExecuteUpdateItem(PID_ITEM_Coordinate, pFFCoordinate, true);
			FillGroupShapeVolume(st, vol, true);
		}
		pGroup = m_Props[GetIndex(PID_GROUP_FF_Scale)].pItem;
		if (0 != pGroup)
		{
			IncCheckinCount(PID_GROUP_FF_Scale, true);
			ExecuteUpdateItem(PID_SUBGROUP_RBScale, varEmpty, true);
			ExecuteUpdateItem(PID_SGITEM_RBS_Type, CPhysXObjForceField::GetFFScaleText(ffDesc.mRBType), true);
			ExecuteUpdateItem(PID_SGITEM_RBS_Value, ffDesc.mRBForceScale, true);
			ExecuteUpdateItem(PID_SUBGROUP_ClothScale, varEmpty, true);
			ExecuteUpdateItem(PID_SGITEM_ClothS_Type, CPhysXObjForceField::GetFFScaleText(ffDesc.mClothType), true);
			ExecuteUpdateItem(PID_SGITEM_ClothS_Value, ffDesc.mClothForceScale, true);
			ExecuteUpdateItem(PID_SUBGROUP_FluidScale, varEmpty, true);
			ExecuteUpdateItem(PID_SGITEM_FluidS_Type, CPhysXObjForceField::GetFFScaleText(ffDesc.mFluidType), true);
			ExecuteUpdateItem(PID_SGITEM_FluidS_Value, ffDesc.mFluidForceScale, true);
			ExecuteUpdateItem(PID_SUBGROUP_SBScale, varEmpty, true);
			ExecuteUpdateItem(PID_SGITEM_SBS_Type, CPhysXObjForceField::GetFFScaleText(ffDesc.mSBType), true);
			ExecuteUpdateItem(PID_SGITEM_SBS_Value, ffDesc.mSBForceScale, true);
		}
	}
}

void CPhysXObjSelGroup::FillWindFF(const APhysXWindForceFieldObjectDesc& ffDesc, bool bIsMerge)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;
	PropItem* pSubGroup = 0;

	const APhysXWindForceFieldObjectDesc::APhysXWindForceFieldParameters& para = ffDesc.GetWindFFParameters();
	if (!bIsMerge)
	{
		pGroup = m_pPropList->CreatePropItem(_T("Wind"), PID_GROUP_FF_Wind);
		SetChiefInfo(PID_GROUP_FF_Wind, pGroup);
		pSubGroup = m_pPropList->CreatePropItem(_T("Force&Dir"), PID_SUBGROUP_Force_xyz, TRUE);
		pGroup->AddSubItem(pSubGroup);
		const _variant_t* pAimVal = &PropItem::varMultiFlag;
		SetItemInfo(PID_SUBGROUP_Force_xyz, *pSubGroup, PID_GROUP_FF_Wind, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), para.mWindForce.x, _T("Specifies the x-axix value of force."), PID_SGITEM_Force_x);
		pSubGroup->AddSubItem(pProp);
		pSubGroup->Expand();
		SetItemInfo(PID_SGITEM_Force_x, *pProp, PID_SUBGROUP_Force_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), para.mWindForce.y, _T("Specifies the y-axix value of force."), PID_SGITEM_Force_y);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Force_y, *pProp, PID_SUBGROUP_Force_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), para.mWindForce.z, _T("Specifies the z-axix value of force."), PID_SGITEM_Force_z);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Force_z, *pProp, PID_SUBGROUP_Force_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("Wind Time"), para.mWindTime, _T("Sets/returns the winding time."), PID_ITEM_FFW_WindT);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFW_WindT, *pProp, PID_GROUP_FF_Wind, false);
		pProp = m_pPropList->CreatePropItem(_T("Rest Time"), para.mRestTime, _T("Sets/returns the resting time."), PID_ITEM_FFW_RestT);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFW_RestT, *pProp, PID_GROUP_FF_Wind, false);
		pProp = m_pPropList->CreatePropItem(_T("Cycle Time"), para.mCycleTime, _T("Sets/returns the cycle time."), PID_ITEM_FFW_CycleT);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFW_CycleT, *pProp, PID_GROUP_FF_Wind, false);
		pProp = m_pPropList->CreatePropItem(_T("Cycle Dist"), ffDesc.mCycleDistance, _T("Sets/returns the cycle distance."), PID_ITEM_FFW_CycleDist);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFW_CycleDist, *pProp, PID_GROUP_FF_Wind, false);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_FF_Wind)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_FF_Wind, true);
		ExecuteUpdateItem(PID_SUBGROUP_Force_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_Force_x, para.mWindForce.x, true);
		ExecuteUpdateItem(PID_SGITEM_Force_y, para.mWindForce.y, true);
		ExecuteUpdateItem(PID_SGITEM_Force_z, para.mWindForce.z, true);
		ExecuteUpdateItem(PID_ITEM_FFW_WindT, para.mWindTime, true);
		ExecuteUpdateItem(PID_ITEM_FFW_RestT, para.mRestTime, true);
		ExecuteUpdateItem(PID_ITEM_FFW_CycleT, para.mCycleTime, true);
		ExecuteUpdateItem(PID_ITEM_FFW_CycleDist, ffDesc.mCycleDistance, true);
	}
}

void CPhysXObjSelGroup::FillVortexFF(const APhysXVortexForceFieldObjectDesc& ffDesc, bool bIsMerge)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;
	PropItem* pSubGroup = 0;

	const APhysXVortexForceFieldObjectDesc::APhysXVortexForceFieldParameters& para = ffDesc.GetVortexFFParameters();
	if (!bIsMerge)
	{
		pGroup = m_pPropList->CreatePropItem(_T("Vortex"), PID_GROUP_FF_Vortex);
		SetChiefInfo(PID_GROUP_FF_Vortex, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("Target Velocity"), para.mTargetVelocity, _T("Sets/returns the target velocity in the tangent direction."), PID_ITEM_FFV_TargetVel);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFV_TargetVel, *pProp, PID_GROUP_FF_Vortex, false);
		pProp = m_pPropList->CreatePropItem(_T("TargetVel Scale"), para.mTargetVelocityScaleTerm, _T("Sets/returns the scale term of the compensating force to achieve the target velocity."), PID_ITEM_FFV_TargetVelScale);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFV_TargetVelScale, *pProp, PID_GROUP_FF_Vortex, false);
		pProp = m_pPropList->CreatePropItem(_T("Target Radius"), para.mTargetRadius, _T("Sets/returns the target radius."), PID_ITEM_FFV_TargetR);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFV_TargetR, *pProp, PID_GROUP_FF_Vortex, false);
		pProp = m_pPropList->CreatePropItem(_T("TargetR Scale"), para.mTargetRadiusScaleTerm, _T("Sets/returns the scale term of the compensating force to achieve the target radius."), PID_ITEM_FFV_TargetRScale);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFV_TargetRScale, *pProp, PID_GROUP_FF_Vortex, false);
		pProp = m_pPropList->CreatePropItem(_T("Elevation Force"), para.mConstantElevationForce, _T("Sets/returns the elevating force to objects."), PID_ITEM_FFV_ElevationForce);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFV_ElevationForce, *pProp, PID_GROUP_FF_Vortex, false);
		pProp = m_pPropList->CreatePropItem(_T("Elevation Scale"), para.mElevationExpandScaleTerm, _T("Sets/returns the scale of elevating force."), PID_ITEM_FFV_ElevationScale);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFV_ElevationScale, *pProp, PID_GROUP_FF_Vortex, false);
		pProp = m_pPropList->CreatePropItem(_T("Centripetal Force"), para.mConstantCentripetalForce, _T("Sets/returns the centripetal force to objects."), PID_ITEM_FFV_CentripetalForce);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFV_ElevationScale, *pProp, PID_GROUP_FF_Vortex, false);
		pProp = m_pPropList->CreatePropItem(_T("Noise"), para.mNoise, _T("Sets/returns the noise force."), PID_ITEM_FFV_Noise);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFV_Noise, *pProp, PID_GROUP_FF_Vortex, false);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_FF_Vortex)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_FF_Vortex, true);
		ExecuteUpdateItem(PID_ITEM_FFV_TargetVel, para.mTargetVelocity, true);
		ExecuteUpdateItem(PID_ITEM_FFV_TargetVelScale, para.mTargetVelocityScaleTerm, true);
		ExecuteUpdateItem(PID_ITEM_FFV_TargetR, para.mTargetRadius, true);
		ExecuteUpdateItem(PID_ITEM_FFV_TargetRScale, para.mTargetRadiusScaleTerm, true);
		ExecuteUpdateItem(PID_ITEM_FFV_ElevationForce, para.mConstantElevationForce, true);
		ExecuteUpdateItem(PID_ITEM_FFV_ElevationScale, para.mElevationExpandScaleTerm, true);
		ExecuteUpdateItem(PID_ITEM_FFV_CentripetalForce, para.mConstantCentripetalForce, true);
		ExecuteUpdateItem(PID_ITEM_FFV_Noise, para.mNoise, true);
	}
}

void CPhysXObjSelGroup::FillGravitationFF(const APhysXGravitationForceFieldObjectDesc& ffDesc, bool bIsMerge)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;
	PropItem* pSubGroup = 0;

	NxVec3 g = ffDesc.GetGravity();
	if (!bIsMerge)
	{
		pGroup = m_pPropList->CreatePropItem(_T("Gravitation"), PID_GROUP_FF_Gravitation);
		SetChiefInfo(PID_GROUP_FF_Gravitation, pGroup);
		pSubGroup = m_pPropList->CreatePropItem(_T("Gravity"), PID_SUBGROUP_G_xyz, TRUE);
		pGroup->AddSubItem(pSubGroup);
		const _variant_t* pAimVal = &PropItem::varMultiFlag;
		SetItemInfo(PID_SUBGROUP_G_xyz, *pSubGroup, PID_GROUP_FF_Gravitation, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), g.x, _T("Specifies the x-axix value of gravity."), PID_SGITEM_G_x);
		pSubGroup->AddSubItem(pProp);
		pSubGroup->Expand();
		SetItemInfo(PID_SGITEM_G_x, *pProp, PID_SUBGROUP_G_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), g.y, _T("Specifies the y-axix value of gravity."), PID_SGITEM_G_y);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_G_y, *pProp, PID_SUBGROUP_G_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), g.z, _T("Specifies the z-axix value of gravity."), PID_SGITEM_G_z);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_G_z, *pProp, PID_SUBGROUP_G_xyz, false, pAimVal);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_FF_Gravitation)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_FF_Gravitation, true);
		ExecuteUpdateItem(PID_SUBGROUP_G_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_G_x, g.x, true);
		ExecuteUpdateItem(PID_SGITEM_G_y, g.y, true);
		ExecuteUpdateItem(PID_SGITEM_G_z, g.z, true);
	}
}

void CPhysXObjSelGroup::FillExplosionFF(const APhysXExplosionForceFieldObjectDesc& ffDesc, bool bIsMerge)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;
	PropItem* pSubGroup = 0;

	const APhysXExplosionForceFieldObjectDesc::APhysXExplosionForceFieldParameters& para = ffDesc.GetExplosionFFParameters();
	if (!bIsMerge)
	{
		pGroup = m_pPropList->CreatePropItem(_T("Explosion"), PID_GROUP_FF_Explosion);
		SetChiefInfo(PID_GROUP_FF_Explosion, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("Center Force"), para.mCenterForce, _T("Sets/returns the center force."), PID_ITEM_FFE_CenterF);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFE_CenterF, *pProp, PID_GROUP_FF_Explosion, false);
		pProp = m_pPropList->CreatePropItem(_T("Radius"), para.mRadius, _T("Sets/returns the radius."), PID_ITEM_FFE_Radius);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFE_Radius, *pProp, PID_GROUP_FF_Explosion, false);
		pProp = m_pPropList->CreatePropItem(_T("Life Time"), para.mLifeTime, _T("Sets/returns the life time."), PID_ITEM_FFE_LifeTime);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFE_LifeTime, *pProp, PID_GROUP_FF_Explosion, false);
		pProp = m_pPropList->CreatePropItem(_T("Quadric FallOff"), para.mQuadricFallOff, _T("Sets/returns the quadric falloff."), PID_ITEM_FFE_QuadricFallOff);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFE_QuadricFallOff, *pProp, PID_GROUP_FF_Explosion, false);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_FF_Explosion)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_FF_Explosion, true);
		ExecuteUpdateItem(PID_ITEM_FFE_CenterF, para.mCenterForce, true);
		ExecuteUpdateItem(PID_ITEM_FFE_Radius, para.mRadius, true);
		ExecuteUpdateItem(PID_ITEM_FFE_LifeTime, para.mLifeTime, true);
		ExecuteUpdateItem(PID_ITEM_FFE_QuadricFallOff, para.mQuadricFallOff, true);
	}
}

void CPhysXObjSelGroup::FillBuoyancyFF(const APhysXBuoyancyForceFieldObjectDesc& ffDesc, bool bIsMerge)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;
	PropItem* pSubGroup = 0;

	if (!bIsMerge)
	{
		pGroup = m_pPropList->CreatePropItem(_T("Buoyancy"), PID_GROUP_FF_Buoyancy);
		SetChiefInfo(PID_GROUP_FF_Buoyancy, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("Density"), ffDesc.GetDensity(), _T("Sets/returns the density."), PID_ITEM_FFB_Density);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFB_Density, *pProp, PID_GROUP_FF_Buoyancy, false);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_FF_Buoyancy)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_FF_Buoyancy, true);
		ExecuteUpdateItem(PID_ITEM_FFB_Density, ffDesc.GetDensity(), true);
	}
}

void CPhysXObjSelGroup::FillBossFF(const APhysXBossForceFieldObjectDesc& ffDesc, bool bIsMerge)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;
	PropItem* pSubGroup = 0;

	const APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters& para = ffDesc.GetBossFFParameters();
	if (!bIsMerge)
	{
		pGroup = m_pPropList->CreatePropItem(_T("Boss"), PID_GROUP_FF_Boss);
		SetChiefInfo(PID_GROUP_FF_Boss, pGroup);
		pProp = m_pPropList->CreatePropItem(_T("Force"), para.mForceValue, _T("Sets/returns the force."), PID_ITEM_FFB_Force);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFB_Force, *pProp, PID_GROUP_FF_Boss, false);
		pProp = m_pPropList->CreatePropItem(_T("Radius"), para.mRadius, _T("Sets/returns the radius."), PID_ITEM_FFB_Radius);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFB_Radius, *pProp, PID_GROUP_FF_Boss, false);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_FF_Boss)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_FF_Boss, true);
		ExecuteUpdateItem(PID_ITEM_FFB_Force, para.mForceValue, true);
		ExecuteUpdateItem(PID_ITEM_FFB_Radius, para.mRadius, true);
	}
}

void CPhysXObjSelGroup::FillCustomFF(const APhysXForceFieldObjectDesc& ffDesc, bool bIsMerge)
{
	assert(0 != m_pPropList);
	PropItem* pProp = 0;
	PropItem* pGroup = 0;
	PropItem* pSubGroup = 0;
	PropItem* pSub2Group = 0;

	if (!bIsMerge)
	{
		pGroup = m_pPropList->CreatePropItem(_T("Custom"), PID_GROUP_FF_Custom);
		SetChiefInfo(PID_GROUP_FF_Custom, pGroup);
		pSubGroup = m_pPropList->CreatePropItem(_T("ConstForce"), PID_SUBGROUP_Const_xyz, TRUE);
		pGroup->AddSubItem(pSubGroup);
		const _variant_t* pAimVal = &PropItem::varMultiFlag;
		SetItemInfo(PID_SUBGROUP_Const_xyz, *pSubGroup, PID_GROUP_FF_Custom, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mConstant.x, _T("Specifies the x value of constant part of kernel."), PID_SGITEM_Const_x);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Const_x, *pProp, PID_SUBGROUP_Const_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mConstant.y, _T("Specifies the y value of constant part of kernel."), PID_SGITEM_Const_y);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Const_y, *pProp, PID_SUBGROUP_Const_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mConstant.z, _T("Specifies the z value of constant part of kernel."), PID_SGITEM_Const_z);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Const_z, *pProp, PID_SUBGROUP_Const_xyz, false, pAimVal);
		// --------------PosMultiplier-----------------------------
		pSubGroup = m_pPropList->CreatePropItem(_T("PosMultiplier"), PID_SUBGROUP_PosMultiplier);
		pGroup->AddSubItem(pSubGroup);
		pSub2Group = m_pPropList->CreatePropItem(_T("PMRow0"), PID_SUBGROUP_PMRow0_xyz, true);
		pSubGroup->AddSubItem(pSub2Group);
		SetItemInfo(PID_SUBGROUP_PMRow0_xyz, *pSub2Group, PID_SUBGROUP_PosMultiplier, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mPositionMultiplier(0, 0), _T("Specifies the [0,0] value of position coefficient matrix."), PID_SGITEM_PMR0_x);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_PMR0_x, *pProp, PID_SUBGROUP_PMRow0_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mPositionMultiplier(0, 1), _T("Specifies the [0,1] value of position coefficient matrix."), PID_SGITEM_PMR0_y);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_PMR0_y, *pProp, PID_SUBGROUP_PMRow0_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mPositionMultiplier(0, 2), _T("Specifies the [0,2] value of position coefficient matrix."), PID_SGITEM_PMR0_z);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_PMR0_z, *pProp, PID_SUBGROUP_PMRow0_xyz, false, pAimVal);
		pSub2Group = m_pPropList->CreatePropItem(_T("PMRow1"), PID_SUBGROUP_PMRow1_xyz, true);
		pSubGroup->AddSubItem(pSub2Group);
		SetItemInfo(PID_SUBGROUP_PMRow1_xyz, *pSub2Group, PID_SUBGROUP_PosMultiplier, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mPositionMultiplier(1, 0), _T("Specifies the [1,0] value of position coefficient matrix."), PID_SGITEM_PMR1_x);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_PMR1_x, *pProp, PID_SUBGROUP_PMRow1_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mPositionMultiplier(1, 1), _T("Specifies the [1,1] value of position coefficient matrix."), PID_SGITEM_PMR1_y);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_PMR1_y, *pProp, PID_SUBGROUP_PMRow1_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mPositionMultiplier(1, 2), _T("Specifies the [1,2] value of position coefficient matrix."), PID_SGITEM_PMR1_z);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_PMR1_z, *pProp, PID_SUBGROUP_PMRow1_xyz, false, pAimVal);
		pSub2Group = m_pPropList->CreatePropItem(_T("PMRow2"), PID_SUBGROUP_PMRow2_xyz, true);
		pSubGroup->AddSubItem(pSub2Group);
		SetItemInfo(PID_SUBGROUP_PMRow2_xyz, *pSub2Group, PID_SUBGROUP_PosMultiplier, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mPositionMultiplier(2, 0), _T("Specifies the [2,0] value of position coefficient matrix."), PID_SGITEM_PMR2_x);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_PMR2_x, *pProp, PID_SUBGROUP_PMRow2_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mPositionMultiplier(2, 1), _T("Specifies the [2,1] value of position coefficient matrix."), PID_SGITEM_PMR2_y);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_PMR2_y, *pProp, PID_SUBGROUP_PMRow2_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mPositionMultiplier(2, 2), _T("Specifies the [2,2] value of position coefficient matrix."), PID_SGITEM_PMR2_z);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_PMR2_z, *pProp, PID_SUBGROUP_PMRow2_xyz, false, pAimVal);
		// --------------TargetPos-----------------------------
		pSubGroup = m_pPropList->CreatePropItem(_T("TargetPos"), PID_SUBGROUP_TargetPos_xyz, TRUE);
		pGroup->AddSubItem(pSubGroup);
		SetItemInfo(PID_SUBGROUP_TargetPos_xyz, *pSubGroup, PID_GROUP_FF_Custom, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mPositionTarget.x, _T("Specifies the x value of target position."), PID_SGITEM_TP_x);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_TP_x, *pProp, PID_SUBGROUP_TargetPos_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mPositionTarget.y, _T("Specifies the y value of target position."), PID_SGITEM_TP_y);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_TP_y, *pProp, PID_SUBGROUP_TargetPos_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mPositionTarget.z, _T("Specifies the z value of target position."), PID_SGITEM_TP_z);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_TP_z, *pProp, PID_SUBGROUP_TargetPos_xyz, false, pAimVal);
		// --------------VelMultiplier-----------------------------
		pSubGroup = m_pPropList->CreatePropItem(_T("VelMultiplier"), PID_SUBGROUP_VelMultiplier);
		pGroup->AddSubItem(pSubGroup);
		pSub2Group = m_pPropList->CreatePropItem(_T("VMRow0"), PID_SUBGROUP_VMRow0_xyz, true);
		pSubGroup->AddSubItem(pSub2Group);
		SetItemInfo(PID_SUBGROUP_VMRow0_xyz, *pSub2Group, PID_SUBGROUP_VelMultiplier, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mVelocityMultiplier(0, 0), _T("Specifies the [0,0] value of velocity coefficient matrix."), PID_SGITEM_VMR0_x);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_VMR0_x, *pProp, PID_SUBGROUP_VMRow0_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mVelocityMultiplier(0, 1), _T("Specifies the [0,1] value of velocity coefficient matrix."), PID_SGITEM_VMR0_y);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_VMR0_y, *pProp, PID_SUBGROUP_VMRow0_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mVelocityMultiplier(0, 2), _T("Specifies the [0,2] value of velocity coefficient matrix."), PID_SGITEM_VMR0_z);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_VMR0_z, *pProp, PID_SUBGROUP_VMRow0_xyz, false, pAimVal);
		pSub2Group = m_pPropList->CreatePropItem(_T("VMRow1"), PID_SUBGROUP_VMRow1_xyz, true);
		pSubGroup->AddSubItem(pSub2Group);
		SetItemInfo(PID_SUBGROUP_VMRow1_xyz, *pSub2Group, PID_SUBGROUP_VelMultiplier, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mVelocityMultiplier(1, 0), _T("Specifies the [1,0] value of velocity coefficient matrix."), PID_SGITEM_VMR1_x);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_VMR1_x, *pProp, PID_SUBGROUP_VMRow1_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mVelocityMultiplier(1, 1), _T("Specifies the [1,1] value of velocity coefficient matrix."), PID_SGITEM_VMR1_y);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_VMR1_y, *pProp, PID_SUBGROUP_VMRow1_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mVelocityMultiplier(1, 2), _T("Specifies the [1,2] value of velocity coefficient matrix."), PID_SGITEM_VMR1_z);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_VMR1_z, *pProp, PID_SUBGROUP_VMRow1_xyz, false, pAimVal);
		pSub2Group = m_pPropList->CreatePropItem(_T("VMRow2"), PID_SUBGROUP_VMRow2_xyz, true);
		pSubGroup->AddSubItem(pSub2Group);
		SetItemInfo(PID_SUBGROUP_VMRow2_xyz, *pSub2Group, PID_SUBGROUP_VelMultiplier, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mVelocityMultiplier(2, 0), _T("Specifies the [2,0] value of velocity coefficient matrix."), PID_SGITEM_VMR2_x);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_VMR2_x, *pProp, PID_SUBGROUP_VMRow2_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mVelocityMultiplier(2, 1), _T("Specifies the [2,1] value of velocity coefficient matrix."), PID_SGITEM_VMR2_y);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_VMR2_y, *pProp, PID_SUBGROUP_VMRow2_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mVelocityMultiplier(2, 2), _T("Specifies the [2,2] value of velocity coefficient matrix."), PID_SGITEM_VMR2_z);
		pSub2Group->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_VMR2_z, *pProp, PID_SUBGROUP_VMRow2_xyz, false, pAimVal);
		// --------------TargetVel-----------------------------
		pSubGroup = m_pPropList->CreatePropItem(_T("TargetVel"), PID_SUBGROUP_TargetVel_xyz, TRUE);
		pGroup->AddSubItem(pSubGroup);
		SetItemInfo(PID_SUBGROUP_TargetVel_xyz, *pSubGroup, PID_GROUP_FF_Custom, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mVelocityTarget.x, _T("Specifies the x value of target velocity."), PID_SGITEM_TV_x);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_TV_x, *pProp, PID_SUBGROUP_TargetVel_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mVelocityTarget.y, _T("Specifies the y value of target velocity."), PID_SGITEM_TV_y);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_TV_y, *pProp, PID_SUBGROUP_TargetVel_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mVelocityTarget.z, _T("Specifies the z value of target velocity."), PID_SGITEM_TV_z);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_TV_z, *pProp, PID_SUBGROUP_TargetVel_xyz, false, pAimVal);
		// --------------TorusRadius-----------------------------
		pProp = m_pPropList->CreatePropItem(_T("TorusRadius"), ffDesc.mFFLKernelParameters.mTorusRadius, _T("Sets/returns radius for NX_FFC_TOROIDAL type coordinates only."), PID_ITEM_FFC_TorusR);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_FFC_TorusR, *pProp, PID_GROUP_FF_Custom, false);
		// --------------Noise-----------------------------
		pSubGroup = m_pPropList->CreatePropItem(_T("Noise"), PID_SUBGROUP_Noise_xyz, TRUE);
		pGroup->AddSubItem(pSubGroup);
		SetItemInfo(PID_SUBGROUP_Noise_xyz, *pSubGroup, PID_GROUP_FF_Custom, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mNoise.x, _T("Specifies the x value of noise."), PID_SGITEM_Noise_x);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Noise_x, *pProp, PID_SUBGROUP_Noise_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mNoise.y, _T("Specifies the y value of noise."), PID_SGITEM_Noise_y);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Noise_y, *pProp, PID_SUBGROUP_Noise_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mNoise.z, _T("Specifies the z value of noise."), PID_SGITEM_Noise_z);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_Noise_z, *pProp, PID_SUBGROUP_Noise_xyz, false, pAimVal);
		// --------------FalloffLinear-----------------------------
		pSubGroup = m_pPropList->CreatePropItem(_T("FalloffLinear"), PID_SUBGROUP_FalloffLN_xyz, TRUE);
		pGroup->AddSubItem(pSubGroup);
		SetItemInfo(PID_SUBGROUP_FalloffLN_xyz, *pSubGroup, PID_GROUP_FF_Custom, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mFalloffLinear.x, _T("Specifies the x value of linear falloff. Range: [0, inf)"), PID_SGITEM_FOLN_x);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_FOLN_x, *pProp, PID_SUBGROUP_FalloffLN_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mFalloffLinear.y, _T("Specifies the y value of linear falloff. Range: [0, inf)"), PID_SGITEM_FOLN_y);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_FOLN_y, *pProp, PID_SUBGROUP_FalloffLN_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mFalloffLinear.z, _T("Specifies the z value of linear falloff. Range: [0, inf)"), PID_SGITEM_FOLN_z);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_FOLN_z, *pProp, PID_SUBGROUP_FalloffLN_xyz, false, pAimVal);
		// --------------FalloffQuadratic-----------------------------
		pSubGroup = m_pPropList->CreatePropItem(_T("FalloffQuad"), PID_SUBGROUP_FalloffQuad_xyz, TRUE);
		pGroup->AddSubItem(pSubGroup);
		SetItemInfo(PID_SUBGROUP_FalloffQuad_xyz, *pSubGroup, PID_GROUP_FF_Custom, true, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("x"), ffDesc.mFFLKernelParameters.mFalloffQuadratic.x, _T("Specifies the x value of quadratic falloff. Range: [0, inf)"), PID_SGITEM_FOQD_x);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_FOQD_x, *pProp, PID_SUBGROUP_FalloffQuad_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("y"), ffDesc.mFFLKernelParameters.mFalloffQuadratic.y, _T("Specifies the y value of quadratic falloff. Range: [0, inf)"), PID_SGITEM_FOQD_y);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_FOQD_y, *pProp, PID_SUBGROUP_FalloffQuad_xyz, false, pAimVal);
		pProp = m_pPropList->CreatePropItem(_T("z"), ffDesc.mFFLKernelParameters.mFalloffQuadratic.z, _T("Specifies the z value of quadratic falloff. Range: [0, inf)"), PID_SGITEM_FOQD_z);
		pSubGroup->AddSubItem(pProp);
		SetItemInfo(PID_SGITEM_FOQD_z, *pProp, PID_SUBGROUP_FalloffQuad_xyz, false, pAimVal);
	}
	else
	{
		pGroup = m_Props[GetIndex(PID_GROUP_FF_Custom)].pItem;
		if (0 == pGroup)
			return;
		IncCheckinCount(PID_GROUP_FF_Custom, true);
		ExecuteUpdateItem(PID_SUBGROUP_Const_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_Const_x, ffDesc.mFFLKernelParameters.mConstant.x, true);
		ExecuteUpdateItem(PID_SGITEM_Const_y, ffDesc.mFFLKernelParameters.mConstant.y, true);
		ExecuteUpdateItem(PID_SGITEM_Const_z, ffDesc.mFFLKernelParameters.mConstant.z, true);
		ExecuteUpdateItem(PID_SUBGROUP_PosMultiplier, varEmpty, true);
		ExecuteUpdateItem(PID_SUBGROUP_PMRow0_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_PMR0_x, ffDesc.mFFLKernelParameters.mPositionMultiplier(0, 0), true);
		ExecuteUpdateItem(PID_SGITEM_PMR0_y, ffDesc.mFFLKernelParameters.mPositionMultiplier(0, 1), true);
		ExecuteUpdateItem(PID_SGITEM_PMR0_z, ffDesc.mFFLKernelParameters.mPositionMultiplier(0, 2), true);
		ExecuteUpdateItem(PID_SUBGROUP_PMRow1_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_PMR1_x, ffDesc.mFFLKernelParameters.mPositionMultiplier(1, 0), true);
		ExecuteUpdateItem(PID_SGITEM_PMR1_y, ffDesc.mFFLKernelParameters.mPositionMultiplier(1, 1), true);
		ExecuteUpdateItem(PID_SGITEM_PMR1_z, ffDesc.mFFLKernelParameters.mPositionMultiplier(1, 2), true);
		ExecuteUpdateItem(PID_SUBGROUP_PMRow2_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_PMR2_x, ffDesc.mFFLKernelParameters.mPositionMultiplier(2, 0), true);
		ExecuteUpdateItem(PID_SGITEM_PMR2_y, ffDesc.mFFLKernelParameters.mPositionMultiplier(2, 1), true);
		ExecuteUpdateItem(PID_SGITEM_PMR2_z, ffDesc.mFFLKernelParameters.mPositionMultiplier(2, 2), true);
		ExecuteUpdateItem(PID_SUBGROUP_TargetPos_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_TP_x, ffDesc.mFFLKernelParameters.mPositionTarget.x, true);
		ExecuteUpdateItem(PID_SGITEM_TP_y, ffDesc.mFFLKernelParameters.mPositionTarget.y, true);
		ExecuteUpdateItem(PID_SGITEM_TP_z, ffDesc.mFFLKernelParameters.mPositionTarget.z, true);
		ExecuteUpdateItem(PID_SUBGROUP_VelMultiplier, varEmpty, true);
		ExecuteUpdateItem(PID_SUBGROUP_VMRow0_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_VMR0_x, ffDesc.mFFLKernelParameters.mVelocityMultiplier(0, 0), true);
		ExecuteUpdateItem(PID_SGITEM_VMR0_y, ffDesc.mFFLKernelParameters.mVelocityMultiplier(0, 1), true);
		ExecuteUpdateItem(PID_SGITEM_VMR0_z, ffDesc.mFFLKernelParameters.mVelocityMultiplier(0, 2), true);
		ExecuteUpdateItem(PID_SUBGROUP_VMRow1_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_VMR1_x, ffDesc.mFFLKernelParameters.mVelocityMultiplier(1, 0), true);
		ExecuteUpdateItem(PID_SGITEM_VMR1_y, ffDesc.mFFLKernelParameters.mVelocityMultiplier(1, 1), true);
		ExecuteUpdateItem(PID_SGITEM_VMR1_z, ffDesc.mFFLKernelParameters.mVelocityMultiplier(1, 2), true);
		ExecuteUpdateItem(PID_SUBGROUP_VMRow2_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_VMR2_x, ffDesc.mFFLKernelParameters.mVelocityMultiplier(2, 0), true);
		ExecuteUpdateItem(PID_SGITEM_VMR2_y, ffDesc.mFFLKernelParameters.mVelocityMultiplier(2, 1), true);
		ExecuteUpdateItem(PID_SGITEM_VMR2_z, ffDesc.mFFLKernelParameters.mVelocityMultiplier(2, 2), true);
		ExecuteUpdateItem(PID_SUBGROUP_TargetVel_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_TV_x, ffDesc.mFFLKernelParameters.mVelocityTarget.x, true);
		ExecuteUpdateItem(PID_SGITEM_TV_y, ffDesc.mFFLKernelParameters.mVelocityTarget.y, true);
		ExecuteUpdateItem(PID_SGITEM_TV_z, ffDesc.mFFLKernelParameters.mVelocityTarget.z, true);
		ExecuteUpdateItem(PID_ITEM_FFC_TorusR, ffDesc.mFFLKernelParameters.mTorusRadius, true);
		ExecuteUpdateItem(PID_SUBGROUP_Noise_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_Noise_x, ffDesc.mFFLKernelParameters.mNoise.x, true);
		ExecuteUpdateItem(PID_SGITEM_Noise_y, ffDesc.mFFLKernelParameters.mNoise.y, true);
		ExecuteUpdateItem(PID_SGITEM_Noise_z, ffDesc.mFFLKernelParameters.mNoise.z, true);
		ExecuteUpdateItem(PID_SUBGROUP_FalloffLN_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_FOLN_x, ffDesc.mFFLKernelParameters.mFalloffLinear.x, true);
		ExecuteUpdateItem(PID_SGITEM_FOLN_y, ffDesc.mFFLKernelParameters.mFalloffLinear.y, true);
		ExecuteUpdateItem(PID_SGITEM_FOLN_z, ffDesc.mFFLKernelParameters.mFalloffLinear.z, true);
		ExecuteUpdateItem(PID_SUBGROUP_FalloffQuad_xyz, varEmpty, true);
		ExecuteUpdateItem(PID_SGITEM_FOQD_x, ffDesc.mFFLKernelParameters.mFalloffQuadratic.x, true);
		ExecuteUpdateItem(PID_SGITEM_FOQD_y, ffDesc.mFFLKernelParameters.mFalloffQuadratic.y, true);
		ExecuteUpdateItem(PID_SGITEM_FOQD_z, ffDesc.mFFLKernelParameters.mFalloffQuadratic.z, true);
	}
}

PropID CPhysXObjSelGroup::GetForceFieldSpecialGroupPropID(const APhysXObjectType ffType)
{
	switch(ffType)
	{
	case APX_OBJTYPE_FORCEFIELD_WIND:
		return PID_GROUP_FF_Wind;
	case APX_OBJTYPE_FORCEFIELD_VORTEX:
		return PID_GROUP_FF_Vortex;
	case APX_OBJTYPE_FORCEFIELD_EXPLOSION:
		return PID_GROUP_FF_Explosion;
	case APX_OBJTYPE_FORCEFIELD_GRAVITATION:
		return PID_GROUP_FF_Gravitation;
	case APX_OBJTYPE_FORCEFIELD_BUOYANCY:
		return PID_GROUP_FF_Buoyancy;
	case APX_OBJTYPE_FORCEFIELD_VACUUM:
		return PID_START_BOUND;
	case APX_OBJTYPE_FORCEFIELD_BOSS:
		return PID_GROUP_FF_Boss;
	case APX_OBJTYPE_FORCEFIELD:
		return PID_GROUP_FF_Custom;
	}
	return PID_START_BOUND;
}

void CPhysXObjSelGroup::SetGroupPos(const A3DVECTOR3& newPos_idx0, bool fromRefFrame)
{
	const size_t MaxCount = m_aObjects.size();
	assert(0 < MaxCount);
	const A3DVECTOR3 deltaMove = newPos_idx0 - m_aObjects[0]->GetProperties()->GetPos();
	if (NxMath::equals(deltaMove.x, 0.0f, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(deltaMove.y, 0.0f, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(deltaMove.z, 0.0f, APHYSX_FLOAT_EPSILON))
		return;

	m_aObjects[0]->SetGPos(newPos_idx0);
	if (!fromRefFrame)
		RefreshFramePose(true);
	if (1 == MaxCount)
		return;

	A3DVECTOR3 temp;
	for (unsigned int i = 1; i < MaxCount; ++i)
	{
		if (IsEnableGroupOperation(i))
		{
			temp = m_aObjects[i]->GetProperties()->GetPos();
			temp += deltaMove;
			m_aObjects[i]->SetGPos(temp);
		}
	}
}

bool CPhysXObjSelGroup::IsEnableGroupOperation(int idx) const
{
	assert(0 <= idx);
	assert(idx < Size());
	if (m_aObjects[idx].bIsIndependent)
		return true;

	IObjBase* pObjFocus = GetFocusObject();
	IObjBase* pObjTest = m_aObjects[idx].pObj;
	if (pObjFocus == pObjTest)
		return true;

	int nCode = pObjFocus->GetRelationshipCode(*pObjTest);
	if (0 == nCode)
	{
		// the same clan detection
		ApxObjBase* pTestParent = 0;
		ApxObjBase* pFocusParent = pObjFocus->GetParent();
		while (0 != pFocusParent)
		{
			pTestParent = pObjTest->GetParent();
			while (0 != pTestParent)
			{
				if (pTestParent == pFocusParent)
					return true;
				pTestParent = pTestParent->GetParent();
			}
			pFocusParent = pFocusParent->GetParent();
		}

		if (m_aObjects[idx].bIsTopAncestor)
			return true;
	}
	return false;
}

bool CPhysXObjSelGroup::IsEnableClone(int idx) const
{
	assert(0 <= idx);
	assert(idx < Size());
	if (m_aObjects[idx].bIsIndependent)
		return true;
	if (m_aObjects[idx].bIsTopAncestor)
		return true;
	return false;
}

void CPhysXObjSelGroup::SetGroupRot(const float angleRadians, const A3DVECTOR3& localAxis, const A3DVECTOR3& globalAxis)
{
	const size_t MaxCount = m_aObjects.size();
	assert(0 < MaxCount);

	if (NxMath::equals(angleRadians, 0.0f, APHYSX_FLOAT_EPSILON))
		return;
	if (NxMath::equals(localAxis.x, 0.0f, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(localAxis.y, 0.0f, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(localAxis.z, 0.0f, APHYSX_FLOAT_EPSILON))
		return;

	NxQuat quatRotL;
	quatRotL.fromAngleAxisFast(angleRadians, NxVec3(localAxis.x, localAxis.y, localAxis.z));

	NxQuat oldQuat = m_aObjects[0]->GetGRot();
	NxQuat newQuat;
	newQuat.multiply(oldQuat, quatRotL);
	m_aObjects[0]->SetGRot(newQuat);
	if (1 == MaxCount)
		return;

	NxVec3 temp;
	A3DVECTOR3 oldPos, newPos;
	const A3DVECTOR3 anchorPos = m_focusRefFrame.GetPos();
	NxQuat quatRotG;
	quatRotG.fromAngleAxisFast(angleRadians, NxVec3(globalAxis.x, globalAxis.y, globalAxis.z));
	for (unsigned int i = 1; i < MaxCount; ++i)
	{
		if (IsEnableGroupOperation(i))
		{
			oldPos = m_aObjects[i]->GetProperties()->GetPos();
			temp = APhysXConverter::A2N_Vector3(oldPos - anchorPos);
			temp = quatRotG.rot(temp);
			newPos = anchorPos + APhysXConverter::N2A_Vector3(temp);
			m_aObjects[i]->SetGPos(newPos);
			m_aObjects[i].distOffset = newPos - anchorPos;
			oldQuat = m_aObjects[i]->GetGRot();
			newQuat.multiply(quatRotG, oldQuat);
			m_aObjects[i]->SetGRot(newQuat);
		}
	}
}

void CPhysXObjSelGroup::SetGroupScale()
{
	const size_t MaxCount = m_aObjects.size();
	assert(0 < MaxCount);

	const float sOri = m_aObjects[0].originScale;
	const float newScale = m_focusRefFrame.GetScale();
	m_aObjects[0]->SetGScale(newScale);
	if (1 == MaxCount)
		return;

	const float sRatio = newScale / sOri;
	const A3DVECTOR3 posBase = m_aObjects[0]->GetProperties()->GetPos();
	float ns = 0.0f;
	A3DVECTOR3 tp;
	for (unsigned int i = 1; i < MaxCount; ++i)
	{
		if (IsEnableGroupOperation(i))
		{
			ns = m_aObjects[i].originScale * sRatio;
			m_aObjects[i]->SetGScale(ns);

			tp = m_aObjects[i].distOffset * sRatio;
			tp += posBase;
			m_aObjects[i]->SetGPos(tp);
		}
	}
}

void CPhysXObjSelGroup::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	size_t MaxCount = m_aObjects.size();
	for (unsigned int i = 0; i < MaxCount; ++i)
	{
		if (IsEnableGroupOperation(i))
			m_aObjects[i]->SetLDirAndUp(vDir, vUp);
	}
}

bool CPhysXObjSelGroup::SetScale(const float& scale)
{
	size_t MaxCount = m_aObjects.size();
	for (unsigned int i = 0; i < MaxCount; ++i)
	{
		if (IsEnableGroupOperation(i))
			m_aObjects[i]->SetLScale(scale);
	}
	return true;
}

bool CPhysXObjSelGroup::IsOnlyFocusOn(const ObjManager::ObjTypeID& objType) const
{
	if (1 == m_aObjects.size())
	{
		if (objType == GetFocusObject()->GetProperties()->GetObjType())
			return true;
	}
	return false;
}

bool CPhysXObjSelGroup::MakeCmdsTitle(std::vector<TString>& outTitles) const
{
	if (IsOnlyFocusOn(ObjManager::OBJ_TYPEID_FORCEFIELD))
	{
		outTitles.push_back(_T("导入力场文件..."));
		outTitles.push_back(_T("导出力场文件..."));
		return true;
	}
	else if (IsOnlyFocusOn(ObjManager::OBJ_TYPEID_REGION))
	{
		outTitles.push_back(_T("添加力场子对象"));
		return true;
	}
	return false;
}

void CPhysXObjSelGroup::OnAddObject(IObjBase& obj)
{
	SelObj sel;
	sel.pObj = &obj;
	sel.originScale = obj.GetProperties()->GetScale();
	sel.distOffset.Clear();
	if (0 < Size())
		sel.distOffset = obj.GetProperties()->GetPos() - m_aObjects[0]->GetProperties()->GetPos();
	UpdateFamilyInfoOnAdd(sel);

	m_aObjects.push_back(sel);
	obj.GetProperties()->SetSelGroup(this);
}

void CPhysXObjSelGroup::RefreshFramePose(bool posOnly)
{
	if (m_Status.ReadFlag(SF_REFRESH_FRAME_DISABLE))
		return;

	const int nCount = Size();
	assert(0 < nCount);

	IPropObjBase* pObjProp = m_aObjects[0]->GetProperties();
	m_focusRefFrame.SetPos(pObjProp->GetPos());
	if (posOnly)
		return;

	m_focusRefFrame.SetDirAndUp(pObjProp->GetDir(), pObjProp->GetUp());
}

void CPhysXObjSelGroup::RefreshFrameScale()
{
	if (m_Status.ReadFlag(SF_REFRESH_FRAME_DISABLE))
		return;

	const int nCount = Size();
	if (0 < nCount)
		m_focusRefFrame.SetScale(m_aObjects[0]->GetProperties()->GetScale());

	for (int i = 0; i < nCount; ++i)
	{
		IPropObjBase* pObjProp = m_aObjects[i]->GetProperties();
		m_aObjects[i].originScale = pObjProp->GetScale();
	}
}

void CPhysXObjSelGroup::RefreshSelObjOffset()
{
	const int nCount = Size();
	if (0 == nCount)
		return;

	const A3DVECTOR3 pos = m_aObjects[0]->GetProperties()->GetPos();
	m_aObjects[0].distOffset.Clear();

	for (int i = 1; i < nCount; ++i)
		m_aObjects[i].distOffset = m_aObjects[i]->GetProperties()->GetPos() - pos;
}

void CPhysXObjSelGroup::UpdateFamilyInfoOnAdd(SelObj& objAdded)
{
	assert(0 != objAdded.pObj);
	objAdded.bIsIndependent = true;
	objAdded.bIsTopAncestor = false;
	int nCode = 0;

	bool bHasParent = false;
	int nCount = Size();
	for (int i = 0; i < nCount; ++i)
	{
		nCode = m_aObjects[i]->GetRelationshipCode(*objAdded.pObj);
		if (0 != nCode)
		{
			objAdded.bIsIndependent = false;
			if (m_aObjects[i].bIsIndependent)
			{
				m_aObjects[i].bIsIndependent = false;
				m_aObjects[i].bIsTopAncestor = true;
			}
			if (0 < nCode)
			{
				if (m_aObjects[i].bIsTopAncestor)
					m_aObjects[i].bIsTopAncestor = false;
			}
			else
			{
				bHasParent = true;
			}
		}
	}

	if (objAdded.bIsIndependent)
		return;
	if (bHasParent)
		return;
	objAdded.bIsTopAncestor = true;
}

void CPhysXObjSelGroup::UpdateFamilyInfoOnRemove(SelObj& objRemoved)
{
	if (objRemoved.bIsIndependent)
		return;
	UpdateFamilyInfo(*objRemoved.pObj, FCRemove());
}

void CPhysXObjSelGroup::UpdateFamilyInfo(const IObjBase& objSample, const FamilyCondition& pred)
{
	std::vector<int> allIdx;
	int nCodeMax = std::numeric_limits<int>::min();
	int nCodeMaxIdx = -1;

	int nCode = 0;
	int nCount = Size();
	for (int i = 0; i < nCount; ++i)
	{
		if (!pred(objSample, m_aObjects[i], &nCode))
			continue;

		allIdx.push_back(i);
		if (nCodeMax < nCode)
		{
			nCodeMax = nCode;
			nCodeMaxIdx = i;
		}
	}

	size_t nAllIdx = allIdx.size();
	for (size_t j = 0; j < nAllIdx; ++j)
	{
		int idx = allIdx[j];
		m_aObjects[idx].bIsIndependent = false;
		m_aObjects[idx].bIsTopAncestor = false;
	}
	if (1 == nAllIdx)
	{
		int idx = allIdx[0];
		m_aObjects[idx].bIsIndependent = true;
		return;
	}
	if (0 <= nCodeMaxIdx)
		m_aObjects[nCodeMaxIdx].bIsTopAncestor = true;
}

void CPhysXObjSelGroup::OnAddChildNotify(ApxObjBase* pParent, ApxObjBase* pChild)
{
	IObjBase* pObjParent = dynamic_cast<IObjBase*>(pParent);
	assert(0 != pObjParent);
	UpdateFamilyInfo(*pObjParent, FCChange());
}

void CPhysXObjSelGroup::OnRemoveChildNotify(ApxObjBase* pParent, ApxObjBase* pChild)
{
	FCChange pred;
	IObjBase* pObjParent = dynamic_cast<IObjBase*>(pParent);
	assert(0 != pObjParent);
	UpdateFamilyInfo(*pObjParent, pred);
	IObjBase* pObjChild = dynamic_cast<IObjBase*>(pChild);
	assert(0 != pObjChild);
	UpdateFamilyInfo(*pObjChild, pred);
}

int CPhysXObjSelGroup::GetIndex(const PropID& nID)
{
	assert(nID < PID_END_BOUND);
	assert(nID >= PID_START_BOUND);

	int idx = nID - PID_START_BOUND;
	assert(0 <= idx);
	return idx;
}

void CPhysXObjSelGroup::SendToPropsGrid()
{
	if(0 == m_pPropList)
		return;

	const int propCount = m_pPropList->GetPropertyCount();
	if (0 < propCount)
	{
		m_pPropList->RemoveAll();
		int i = 0;
		while (i < PID_END_BOUND - PID_START_BOUND)
			i = ClearItemInfo(i, false);
	}

	const size_t selCount = m_aObjects.size();
	if (0 == selCount)
	{
		m_pPropList->DisableCommands();
		m_pPropList->RedrawWindow();
		return;
	}

	if (!IsRunTime())
	{
		std::vector<TString> cmds; 
		bool bHasTitle = MakeCmdsTitle(cmds);
		if (bHasTitle)
			m_pPropList->EnableCommands(cmds);
		else
			m_pPropList->DisableCommands();
	}

	assert(0 != m_pPropList);
	for (unsigned int i = 0; i < selCount; ++i)
		m_aObjects[i]->GetProperties()->OnSendToPropsGrid();

	UpdateSocketNames();
	CommitModified();
}

int CPhysXObjSelGroup::IncCheckinCount(const PropID& nID, bool bIncludeChiefItem)
{
	int idx = nID - PID_START_BOUND;
	assert(0 <= idx);
	if (!bIncludeChiefItem)
	{
		const bool bIsChief = m_Props[idx].ReadFlag(PF_IS_CHIEF);
		assert(false == bIsChief);
		if (bIsChief)
			return idx;
	}

	m_Props[idx].nCount++;
	return idx;
}

void CPhysXObjSelGroup::SetChiefInfo(const PropID& nID, PropItem* pI)
{
	int idx = nID - PID_START_BOUND;
	m_Props[idx].nCount = 1;
	m_Props[idx].pItem = pI;
	m_Props[idx].RaiseFlag(PF_IS_GROUP);
	m_Props[idx].RaiseFlag(PF_IS_CHIEF);
	m_Props[idx].RaiseFlag(PF_HAS_NEW_VALUE);
}

void CPhysXObjSelGroup::SetItemInfo(const PropID& nID, PropItem& I, const PropID& nParentID, bool isGroup, const _variant_t* pMultiValueMask)
{
	PropItem* pParent = 0;
	int idxParent = nParentID - PID_START_BOUND;
	if (PID_Unknown < idxParent)
		pParent = m_Props[idxParent].pItem;

	int idx = nID - PID_START_BOUND;
	m_Props[idx].nCount = 1;
	m_Props[idx].pItem = &I;
	m_Props[idx].pLeader = pParent;
	if (0 != pParent)
		m_Props[idxParent].pChildren.push_back(idx);
	m_Props[idx].newValue = I.GetValue();
	if (isGroup)
		m_Props[idx].RaiseFlag(PF_IS_GROUP);
	if (0 != pMultiValueMask)
		m_Props[idx].pMultiValueMask = pMultiValueMask;
}

unsigned int CPhysXObjSelGroup::DeleteProps(unsigned int idx, bool SaveUpdateInfo)
{
	if (idx	>= PID_END_BOUND - PID_START_BOUND)
		return PID_END_BOUND - PID_START_BOUND;

	PropItem* pI = m_Props[idx].pItem;
	if (0 != pI)
	{
		bool bInList = false;
		m_pPropList->ReleasePropItem(m_Props[idx].pItem, &bInList);
		if (bInList)
			m_Status.RaiseFlag(SF_PROPLIST_LAYOUT_DIRTY);
	}
	return ClearItemInfo(idx, SaveUpdateInfo);
}

unsigned int CPhysXObjSelGroup::ClearItemInfo(unsigned int idx, bool SaveUpdateInf)
{
	assert(idx < PID_END_BOUND - PID_START_BOUND);
	unsigned int rtn = idx + 1;

	size_t nCount = m_Props[idx].pChildren.size();
	for (size_t i = 0; i < nCount; ++i)
		rtn = ClearItemInfo(m_Props[idx].pChildren[i], SaveUpdateInf);

	bool bIsDirty = false;
	if (SaveUpdateInf)
		bIsDirty = m_Props[idx].ReadFlag(PF_IS_DIRTY);
	m_Props[idx].ClearAll();
	if (bIsDirty)
		m_Props[idx].RaiseFlag(PF_IS_DIRTY);

	return rtn;
}

void CPhysXObjSelGroup::AddGroupItem(PropItem* pItem, ItemInfo* pPrev)
{
	if (0 == pItem)
		return;

	PropItem* pTarget = 0;
	if (0 != pPrev)
		pTarget = pPrev->pItem;

	lstPropItems::iterator it = m_pGroupItems.begin();
	lstPropItems::iterator itEnd = m_pGroupItems.end();
	for (; it != itEnd; ++it)
	{
		if (*it == pItem)
			return;

		if (*it == pTarget)
			break;
	}

	if (it != itEnd)
	{
		++it;
		m_pGroupItems.insert(it, pItem);
	}
	else
	{
		ItemInfo* pPP = 0;
		if (0 != pPrev)
			pPP = pPrev->pPrevious;
		AddGroupItem(pTarget, pPP);
		m_pGroupItems.push_back(pItem);
	}
}

void CPhysXObjSelGroup::CommitModified()
{
//	bool bNoSelFFORRgn = AllSelectedWithoutFForRgn();
	bool bHasSameShapeType = AllSelectedHaveTheSameShapeType();
	const size_t nCount = m_aObjects.size();

	int idx = 0;
	while (idx < PID_END_BOUND - PID_START_BOUND)
	{
		bool bIsToDel = false;
		if (0 < m_Props[idx].nCount)
		{
			int n = nCount - m_Props[idx].nCount;
			if (0 != n)
			{
				if (0 > n)
				{
					assert(!"count error, debug it!");
				}
				bIsToDel = true;
			}
			else if ((PID_GROUP_ShapeVolume - PID_START_BOUND) == idx)
			{
				if (!bHasSameShapeType)
					bIsToDel = true;
			}
		}
		if (bIsToDel)
		{
			idx = DeleteProps(idx, false);
		}
		else
		{
			assert((0 == m_Props[idx].nCount) || (nCount == m_Props[idx].nCount));
			if (m_Props[idx].ReadFlag(PF_HAS_NEW_VALUE))
			{
				m_Props[idx].ClearFlag(PF_HAS_NEW_VALUE);
				assert(0 != m_Props[idx].pItem);
				if (m_Props[idx].ReadFlag(PF_IS_CHIEF))
				{
					AddGroupItem(m_Props[idx].pItem, m_Props[idx].pPrevious);
					m_Status.RaiseFlag(SF_PROPLIST_LAYOUT_DIRTY);
				}
				else
				{
					m_Props[idx].pItem->SetValue(m_Props[idx].newValue);
				}
			}
			if (m_Props[idx].ReadFlag(PF_IS_DIRTY))
				m_Props[idx].ClearFlag(PF_IS_DIRTY);
			++idx;
		}
	}

	lstPropItems::iterator it = m_pGroupItems.begin();
	lstPropItems::iterator itEnd = m_pGroupItems.end();
	for (; it != itEnd; ++it)
		m_pPropList->AddProperty(*it, 0, 0);
	m_pGroupItems.clear();

	m_Status.ClearFlag(SF_HAS_SOMETHING_DIRTY);
	if (m_Status.ReadFlag(SF_PROPLIST_LAYOUT_DIRTY))
	{
		m_pPropList->AdjustLayout();
		m_Status.ClearFlag(SF_PROPLIST_LAYOUT_DIRTY);
	}
}

void CPhysXObjSelGroup::OnUpdateCCTypeContent(const CDynProperty& objProp)
{
	PropItem* pProp = m_Props[PID_ITEM_CC_Type].pItem;
	assert(0 != pProp);

	int oc = pProp->GetOptionCount();
	if (0 == oc)
	{
		switch (objProp.GetRoleType())
		{
		case CDynProperty::ROLE_ACTOR_NONE:
			return;
		case CDynProperty::ROLE_ACTOR_NPC:
			pProp->RemoveAllOptions();
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_TOTAL_EMPTY));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_LWCC));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_BRUSH_CDRCC));
			return;
		case CDynProperty::ROLE_ACTOR_AVATAR:
			pProp->RemoveAllOptions();
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_TOTAL_EMPTY));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_BRUSH_CDRCC));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_EXTCC));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_DYNCC));
			return;
		default:
			assert(!"Unknown RoleType!");
		}
	}
	else if (3 == oc)
	{
		switch (objProp.GetRoleType())
		{
		case CDynProperty::ROLE_ACTOR_NONE:
			return;
		case CDynProperty::ROLE_ACTOR_NPC:
			return;
		case CDynProperty::ROLE_ACTOR_AVATAR:
			pProp->RemoveAllOptions();
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_TOTAL_EMPTY));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_LWCC));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_BRUSH_CDRCC));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_EXTCC));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_DYNCC));
			return;
		default:
			assert(!"Unknown RoleType!");
		}
	}
	else if (4 == oc)
	{
		switch (objProp.GetRoleType())
		{
		case CDynProperty::ROLE_ACTOR_NONE:
			return;
		case CDynProperty::ROLE_ACTOR_NPC:
			pProp->RemoveAllOptions();
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_TOTAL_EMPTY));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_LWCC));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_BRUSH_CDRCC));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_EXTCC));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_DYNCC));
			return;
		case CDynProperty::ROLE_ACTOR_AVATAR:
			return;
		default:
			assert(!"Unknown RoleType!");
		}
	}
}

PropItem* CPhysXObjSelGroup::CreateCCFlagProp(const PropID& nID, const _variant_t& objVal)
{
	int idxParent = PID_GROUP_CC_Flags - PID_START_BOUND;
	PropItem* pGroup = m_Props[idxParent].pItem;
	if (0 == pGroup)
		return 0;

	PropItem* pProp = 0;
	switch(nID)
	{
	case PID_ITEM_LWCC_RevisePosEOpen:
		pProp = m_pPropList->CreatePropItem(_T("Enforce RevisePos"), objVal, _T("Sets/gets APhysXLWCCFlag flag: APX_LWCCF_REVISEPOSITION_ENFORCE_OPEN."), nID);
		break;
	case PID_ITEM_CCDCC_Slide:
		pProp = m_pPropList->CreatePropItem(_T("Slide"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_SLIDE_ENABLE."), nID);
		break;
	case PID_ITEM_CCDCC_AttachEOpen:
		pProp = m_pPropList->CreatePropItem(_T("Enforce AttachOpen"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_ATTACHMENT_ENFORCE_OPEN."), nID);
		break;
	case PID_ITEM_CCDCC_AttachEClose:
		pProp = m_pPropList->CreatePropItem(_T("Enforce AttachClose"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_ATTACHMENT_ENFORCE_CLOSE."), nID);
		break;
	case PID_ITEM_CC_PushForce:
		pProp = m_pPropList->CreatePropItem(_T("Push Force"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_ADD_PUSHFORCE."), nID);
		break;
	case PID_ITEM_CC_StandForce:
		pProp = m_pPropList->CreatePropItem(_T("Stand Force"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_ADD_STANDFORCE."), nID);
		break;
	case PID_ITEM_DynCC_AutoHangEnd:
		pProp = m_pPropList->CreatePropItem(_T("Auto HangEnd"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_AUTO_HANG_END_ENABLE."), nID);
		break;
	case PID_ITEM_DynCC_PassiveEffect:
		pProp = m_pPropList->CreatePropItem(_T("Passive Effect"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_PASSIVE_EFFECT_ONLY."), nID);
		break;
	default:
		assert(!"Wrong PropID input!");
		break;
	}

	if (0 != pProp)
	{
		pGroup->AddSubItem(pProp);
		SetItemInfo(nID, *pProp, PID_GROUP_CC_Flags, false);
		if (!m_Props[idxParent].ReadFlag(PF_HAS_NEW_VALUE))
			m_Status.RaiseFlag(SF_PROPLIST_LAYOUT_DIRTY);
	}
	return pProp;
}

void CPhysXObjSelGroup::UpdateSocketNames()
{
	if (!m_ToUpdateSocketName)
		return;
	m_ToUpdateSocketName = false;

	int idx = PID_ITEM_SocketName - PID_START_BOUND;
	assert(0 <= idx);
	PropItem* pProp = m_Props[idx].pItem;
	if (0 == pProp)
		return;

	pProp->RemoveAllOptions();
	const size_t nCount = m_aObjects.size();
	for (unsigned int i = 0; i < nCount; ++i)
	{
		ObjManager::ObjTypeID ot = m_aObjects[i]->GetProperties()->GetObjType();
		if ((ObjManager::OBJ_TYPEID_SMSOCKET == ot) || (ObjManager::OBJ_TYPEID_SOCKETSM == ot))
		{
			ApxObjSocketBase* pSocket = dynamic_cast<ApxObjSocketBase*>(m_aObjects[i].pObj);
			if (0 != pSocket)
			{
				CPhysXObjDynamic* pDynObj = pSocket->GetBindingObject();
				if (0 != pDynObj)
					OnUpdateBoneHookName(*pProp, pDynObj->GetSkinModel(), pSocket->GetSocketType());
			}
		}
	}

	ObjManager::ObjTypeID ot = m_aObjects[0]->GetProperties()->GetObjType();
	if ((ObjManager::OBJ_TYPEID_SMSOCKET == ot) || (ObjManager::OBJ_TYPEID_SMSOCKET == ot))
		RefreshFrame();
}

void CPhysXObjSelGroup::OnUpdateBoneHookName(PropItem& prop, A3DSkinModel* pSkinModel, ApxSkinModelSocketType smST)
{
	if (0 == pSkinModel)
		return;

	A3DSkeleton* pSK = pSkinModel->GetSkeleton();
	if (0 == pSK)
		return;

	if (ApxSM_SocketType_Bone == smST)
	{
		int nBones = pSK->GetBoneNum();
		for (int i = 0; i < nBones; ++i)
		{
			A3DBone* pBone = pSK->GetBone(i);
			const char* pName = pBone->GetName();
			if (0 != pName)
				prop.AddOption(_TAS2WC(pName));
		}
	}
	else if (ApxSM_SocketType_Hook == smST)
	{
		int nHooks = pSK->GetHookNum();
		for (int i = 0; i < nHooks; ++i)
		{
			A3DSkeletonHook* pHook = pSK->GetHook(i);
			const char* pName = pHook->GetName();
			if (0 != pName)
				prop.AddOption(_TAS2WC(pName));
		}
	}
}

PropItem* CPhysXObjSelGroup::CreateFFScaleProp(const PropID& nID, const _variant_t& objVal)
{
	int idxGroup = PID_GROUP_FF_Scale - PID_START_BOUND;
	PropItem* pGroup = m_Props[idxGroup].pItem;
	if (0 == pGroup)
		return 0;

	bool isGroup = false;
	PropItem* pProp = 0;
	PropItem* pParent = 0;
	PropID parentID = PID_START_BOUND;
	const _variant_t* pAimVal = &PropItem::varMultiFlag;
	switch(nID)
	{
	case PID_SUBGROUP_RBScale:
		pProp = m_pPropList->CreatePropItem(_T("RB Scale"), nID);
		isGroup = true;
		pParent = pGroup;
		parentID = PID_GROUP_FF_Scale;
		break;
	case PID_SGITEM_RBS_Type:
		pProp = m_pPropList->CreatePropItem(_T("Type"), CPhysXObjForceField::GetFFScaleText(NxForceFieldType(int(objVal))), _T("Specifies the RB scale type."), nID);
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_OTHER));
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_GRAVITATIONAL));
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_NO_INTERACTION));
		pProp->AllowEdit(FALSE);
		pParent = m_Props[PID_SUBGROUP_RBScale - PID_START_BOUND].pItem;
		parentID = PID_SUBGROUP_RBScale;
		break;
	case PID_SGITEM_RBS_Value:
		pProp = m_pPropList->CreatePropItem(_T("Value"), objVal, _T("Specifies the RB scale value."), nID);
		pParent = m_Props[PID_SUBGROUP_RBScale - PID_START_BOUND].pItem;
		parentID = PID_SUBGROUP_RBScale;
		break;
	case PID_SUBGROUP_ClothScale:
		pProp = m_pPropList->CreatePropItem(_T("Cloth Scale"), nID);
		isGroup = true;
		pParent = pGroup;
		parentID = PID_GROUP_FF_Scale;
		break;
	case PID_SGITEM_ClothS_Type:
		pProp = m_pPropList->CreatePropItem(_T("Type"), CPhysXObjForceField::GetFFScaleText(NxForceFieldType(int(objVal))), _T("Specifies the Cloth scale type."), nID);
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_OTHER));
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_GRAVITATIONAL));
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_NO_INTERACTION));
		pProp->AllowEdit(FALSE);
		pParent = m_Props[PID_SUBGROUP_ClothScale - PID_START_BOUND].pItem;
		parentID = PID_SUBGROUP_ClothScale;
		break;
	case PID_SGITEM_ClothS_Value:
		pProp = m_pPropList->CreatePropItem(_T("Value"), objVal, _T("Specifies the Cloth scale value."), nID);
		pParent = m_Props[PID_SUBGROUP_ClothScale - PID_START_BOUND].pItem;
		parentID = PID_SUBGROUP_ClothScale;
		break;
	case PID_SUBGROUP_FluidScale:
		pProp = m_pPropList->CreatePropItem(_T("Fluid Scale"), nID);
		isGroup = true;
		pParent = pGroup;
		parentID = PID_GROUP_FF_Scale;
		break;
	case PID_SGITEM_FluidS_Type:
		pProp = m_pPropList->CreatePropItem(_T("Type"), CPhysXObjForceField::GetFFScaleText(NxForceFieldType(int(objVal))), _T("Specifies the Fluid scale type."), nID);
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_OTHER));
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_GRAVITATIONAL));
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_NO_INTERACTION));
		pProp->AllowEdit(FALSE);
		pParent = m_Props[PID_SUBGROUP_FluidScale - PID_START_BOUND].pItem;
		parentID = PID_SUBGROUP_FluidScale;
		break;
	case PID_SGITEM_FluidS_Value:
		pProp = m_pPropList->CreatePropItem(_T("Value"), objVal, _T("Specifies the Fluid scale value."), nID);
		pParent = m_Props[PID_SUBGROUP_FluidScale - PID_START_BOUND].pItem;
		parentID = PID_SUBGROUP_FluidScale;
		break;
	case PID_SUBGROUP_SBScale:
		pProp = m_pPropList->CreatePropItem(_T("SoftBody Scale"), nID);
		isGroup = true;
		pParent = pGroup;
		parentID = PID_GROUP_FF_Scale;
		break;
	case PID_SGITEM_SBS_Type:
		pProp = m_pPropList->CreatePropItem(_T("Type"), CPhysXObjForceField::GetFFScaleText(NxForceFieldType(int(objVal))), _T("Specifies the SB scale type."), nID);
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_OTHER));
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_GRAVITATIONAL));
		pProp->AddOption(CPhysXObjForceField::GetFFScaleText(NX_FF_TYPE_NO_INTERACTION));
		pProp->AllowEdit(FALSE);
		pParent = m_Props[PID_SUBGROUP_SBScale - PID_START_BOUND].pItem;
		parentID = PID_SUBGROUP_SBScale;
		break;
	case PID_SGITEM_SBS_Value:
		pProp = m_pPropList->CreatePropItem(_T("Value"), objVal, _T("Specifies the SB scale value."), nID);
		pParent = m_Props[PID_SUBGROUP_SBScale - PID_START_BOUND].pItem;
		parentID = PID_SUBGROUP_SBScale;
		break;
	default:
		assert(!"Wrong PropID input!");
		break;
	}

	if (0 != pProp)
	{
		assert(0 != pParent);
		pParent->AddSubItem(pProp);
		SetItemInfo(nID, *pProp, parentID, isGroup, pAimVal);
		if (!m_Props[idxGroup].ReadFlag(PF_HAS_NEW_VALUE))
			m_Status.RaiseFlag(SF_PROPLIST_LAYOUT_DIRTY);
	}
	return pProp;
}
