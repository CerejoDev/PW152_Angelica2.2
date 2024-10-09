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
#include "PropsWindow.h"

const _variant_t CMultiValueProp::varMultiFlag = _T("<MultiValue>");
CMultiValueProp::CMultiValueProp(const CString& strName, const _variant_t& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData,
								 LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars)
								 : CBCGPProp(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{
	m_OriginType = varValue.vt;
	m_bIsMultiValue = false;

	if (varValue == varMultiFlag)
		m_bIsMultiValue = true;
}

void CMultiValueProp::SetValue(const _variant_t& varValue)
{
	if (m_bIsMultiValue)
	{
		if (varValue.vt == m_OriginType)
		{
			m_bIsMultiValue = false;
			SetNewValue(varValue);
		}
		return;
	}

	if ((varMultiFlag == varValue) || (varValue.vt != m_OriginType))
	{
		m_bIsMultiValue = true;
		SetNewValue(varMultiFlag);
		return;
	}

	m_bIsMultiValue = false;
	CBCGPProp::SetValue(varValue);
}

void CMultiValueProp::SetNewValue(const _variant_t& val)
{
	ASSERT_VALID(this);
	BOOL bInPlaceEdit = m_bInPlaceEdit;
	if (bInPlaceEdit)
		OnEndEdit();

	m_varValue = val;
	Redraw();

	if (bInPlaceEdit)
	{
		ASSERT_VALID(m_pWndList);
		m_pWndList->EditItem(this);
	}
}

A3DWireCollector* CPhysXObjSelGroup::pWCTopZBuf = 0;
const TCHAR* CPhysXObjSelGroup::szEmpty = _T("");
const _variant_t CPhysXObjSelGroup::varEmpty = _T("");

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

bool CPhysXObjSelGroup::ExtractVector(const CBCGPProp& prop, A3DVECTOR3& outVec)
{
	const int nCount = prop.GetSubItemsCount();
	assert(3 == nCount);
	if (3 != nCount)
		return false;

	outVec.x = prop.GetSubItem(0)->GetValue();
	outVec.y = prop.GetSubItem(1)->GetValue();
	outVec.z = prop.GetSubItem(2)->GetValue();
	return true;
}

CPhysXObjSelGroup::CPhysXObjSelGroup()
{
	m_Status.ClearAllFlags();
	m_pStateReport = 0;
	m_pRHO = 0;
	m_pPropList = 0;
}

bool CPhysXObjSelGroup::EnterRuntime(APhysXScene& aPhysXScene)
{
	m_Status.RaiseFlag(SF_IS_GAME_RUNTIME);
	return true;
}

void CPhysXObjSelGroup::LeaveRuntime()
{
	m_Status.ClearFlag(SF_IS_GAME_RUNTIME);
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

	if (0 != pWCTopZBuf)
		m_focusRefFrame.Draw(*pWCTopZBuf);
	else if (0 != pWC)
		m_focusRefFrame.Draw(*pWC);
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

void CPhysXObjSelGroup::GetAllObjects(std::vector<IObjBase*>& objs) const
{
	objs.clear();
	objs.reserve(m_aObjects.size());
	SelObjHolder::const_iterator it = m_aObjects.begin();
	SelObjHolder::const_iterator itEnd = m_aObjects.end();
	for (; it != itEnd; ++it)
		objs.push_back(it->pObj);
}

bool CPhysXObjSelGroup::IsIncluding(const IObjBase& obj) const
{
	const IObjBase* pObj = &obj;
	SelObjHolder::const_iterator it = m_aObjects.begin();
	SelObjHolder::const_iterator itEnd = m_aObjects.end();
	for (; it != itEnd; ++it)
	{
		if(pObj == it->pObj)
			return true;
	}
	return false;
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
	RefreshFramePose();
	RefreshFrameScale();
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

	if (idx != MaxCount - 1)
	{
		SelObj temp = m_aObjects[idx];
		m_aObjects[idx] = m_aObjects[MaxCount - 1];
		m_aObjects[MaxCount - 1] = temp;
	}
	m_aObjects.pop_back();
	SendToPropsGrid();

	if (0 < Size())
	{
		RefreshFramePose();
		RefreshFrameScale();
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
	const IObjBase* pFocusBackup = GetFocusObject();

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
		RefreshFramePose();
		RefreshFrameScale();
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
	const IObjBase* pFocusBackup = GetFocusObject();

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
	RefreshFramePose();
	RefreshFrameScale();
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
		SetGroupPos(m_focusRefFrame.GetPos());
	}
	else if (CCoordinateDirection::EDIT_TYPE_ROTATE == optType)
	{
		float angRad = 0.0f;
		A3DVECTOR3 axis(0.0f);
		bool rtn = m_focusRefFrame.GetLastRotateInfo(angRad, axis);
		assert(true == rtn);
		SetGroupRot(angRad, axis);
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
		m_aObjects[i]->GetProperties()->OnUpdatePropsGrid(*m_pPropList);

	CommitModified();
}

bool CPhysXObjSelGroup::PropertyChanged(const CBCGPProp& prop)
{
	assert(0 != m_pPropList);
	const DWORD_PTR propID = prop.GetData();
	int idx = GetIndex(PropID(propID));

	m_Props[idx].RaiseFlag(PF_IS_LOCKED);
	size_t nCount = m_aObjects.size();
	for (unsigned int i = 0; i < nCount; ++i)
		m_aObjects[i]->GetProperties()->OnPropertyChanged(prop);
	m_Props[idx].ClearFlag(PF_IS_LOCKED);

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
	m_Status.RaiseFlag(SF_HAS_SOMETHING_DIRTY);
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

void CPhysXObjSelGroup::ExecuteUpdateItem(const PropID& nID, const _variant_t& objVal, bool bIsMerge)
{
	int idx = IncCheckinCount(nID, false);
	if (m_Props[idx].ReadFlag(PF_IS_GROUP))
		return;

	CBCGPProp* pProp = m_Props[idx].pItem;
	if (0 != pProp)
	{
		m_Props[idx].SetNewValue(objVal, bIsMerge);
		return;
	}

	assert(false == bIsMerge);
	CreateCCFlagProp(nID, objVal);
}

void CPhysXObjSelGroup::FillGroupBasic(const IPropObjBase& objProp)
{
	assert(0 != m_pPropList);
	CBCGPProp* pProp = 0;
	CBCGPProp* pGroup = 0;

	const char*  pObjName  = objProp.GetObjName_cstr();
	const TCHAR* pTypeID   = RawObjMgr::GetObjTypeIDText(objProp.GetObjType());
	const TCHAR* pState    = IPropObjBase::GetObjStateText(objProp.GetObjState());
//	const char*  pFileName = objProp.GetFilePathName_cstr(); tp update

	if (!IsMerge(objProp.GetHostObject()))
	{
		pGroup = new CBCGPProp(_T("Basic"), PID_GROUP_Basic);
		SetChiefInfo(PID_GROUP_Basic, pGroup);
		pProp = new CBCGPProp(_T("(Name)"), pObjName, _T("returns the name of the object."), PID_ITEM_Name);
		pProp->Enable(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Name, *pProp, pGroup, false);
		pProp = new CBCGPProp(_T("Type"), pTypeID, _T("returns the type of the object."), PID_ITEM_Type);
		pProp->Enable(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Type, *pProp, pGroup, false);
		pProp = new CBCGPProp(_T("State"), pState, _T("returns the state of the object."), PID_ITEM_State);
		pProp->Enable(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_State, *pProp, pGroup, false);
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
		ExecuteUpdateItem(PID_ITEM_Type, pTypeID, true);
		ExecuteUpdateItem(PID_ITEM_State, pState, true);
//		ExecuteUpdateItem(PID_ITEM_File, pFileName, true);
	}
}

void CPhysXObjSelGroup::FillGroupDrivenBy(const IPropPhysXObjBase& objProp)
{
	assert(0 != m_pPropList);
	CBCGPProp* pProp = 0;
	CBCGPProp* pGroup = 0;

	const TCHAR* pCurrentDM = IPropPhysXObjBase::GetDrivenModeText(objProp.GetDrivenMode(false));
	const TCHAR* pSimPlayDM = IPropPhysXObjBase::GetDrivenModeText(objProp.GetDrivenMode(true));

	if (!IsMerge(objProp.GetHostObject()))
	{
		pGroup = new CBCGPProp(_T("DrivenBy"), PID_GROUP_DrivenBy);
		SetChiefInfo(PID_GROUP_DrivenBy, pGroup);
		pProp = new CBCGPProp(_T("Current"), pCurrentDM, _T("returns current driven type of the object."), PID_ITEM_CurntDVN);
		pProp->Enable(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_CurntDVN, *pProp, pGroup, false);
		pProp = new CBCGPProp(_T("OnSim/Play"), pSimPlayDM, _T("Sets/returns the default driven type while the object enter sim/play mode."), PID_ITEM_OnSimDVN);
		pProp->AddOption(IPropPhysXObjBase::GetDrivenModeText(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX));
		pProp->AddOption(IPropPhysXObjBase::GetDrivenModeText(IPropPhysXObjBase::DRIVEN_BY_ANIMATION));
		pProp->AddOption(IPropPhysXObjBase::GetDrivenModeText(IPropPhysXObjBase::DRIVEN_BY_PART_PHYSX));
		pProp->AllowEdit(FALSE);
		pProp->Enable(m_Status.ReadFlag(SF_IS_GAME_RUNTIME)? FALSE : TRUE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_OnSimDVN, *pProp, pGroup, false);
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
	CBCGPProp* pI = 0;
	CBCGPProp* pGroup = 0;

	A3DVECTOR3 pos = objProp.GetPos();
	A3DVECTOR3 dir = objProp.GetDir();
	A3DVECTOR3 up = objProp.GetUp();
	float scale = objProp.GetScale();

	if (!IsMerge(objProp.GetHostObject()))
	{
		const _variant_t* pAimVal = &CMultiValueProp::varMultiFlag;
		pGroup = new CBCGPProp(_T("Pose&Scale"), PID_GROUP_PoseScale);
		SetChiefInfo(PID_GROUP_PoseScale, pGroup);
		CBCGPProp* pPos = new CBCGPProp(_T("pos"), PID_SUBGROUP_Pos_xyz, TRUE);
		pGroup->AddSubItem(pPos);
		SetItemInfo(PID_SUBGROUP_Pos_xyz, *pPos, pGroup, true, pAimVal);
			pI = new CMultiValueProp(_T("x"), pos.x, _T("Specifies the x-axix value of position."), PID_SGITEM_Pos_x);
			pPos->AddSubItem(pI);
			SetItemInfo(PID_SGITEM_Pos_x, *pI, pPos, false, pAimVal);
			pI = new CMultiValueProp(_T("y"), pos.y, _T("Specifies the y-axix value of position."), PID_SGITEM_Pos_y);
			pPos->AddSubItem(pI);
			SetItemInfo(PID_SGITEM_Pos_y, *pI, pPos, false, pAimVal);
			pI = new CMultiValueProp(_T("z"), pos.z, _T("Specifies the z-axix value of position."), PID_SGITEM_Pos_z);
			pPos->AddSubItem(pI);
			SetItemInfo(PID_SGITEM_Pos_z, *pI, pPos, false, pAimVal);
		pPos->Enable(m_Status.ReadFlag(SF_IS_GAME_RUNTIME)? FALSE : TRUE);
		CBCGPProp* pDir = new CBCGPProp(_T("dir"), PID_SUBGROUP_Dir_xyz, TRUE);
		pGroup->AddSubItem(pDir);
		SetItemInfo(PID_SUBGROUP_Dir_xyz, *pDir, pGroup, true, pAimVal);
			pI = new CMultiValueProp(_T("x"), dir.x, _T("Specifies the x-axix value of face direction."), PID_SGITEM_Dir_x);
			pI->Enable(FALSE);
			pDir->AddSubItem(pI);
			SetItemInfo(PID_SGITEM_Dir_x, *pI, pDir, false, pAimVal);
			pI = new CMultiValueProp(_T("y"), dir.y, _T("Specifies the y-axix value of face direction."), PID_SGITEM_Dir_y);
			pI->Enable(FALSE);
			pDir->AddSubItem(pI);
			SetItemInfo(PID_SGITEM_Dir_y, *pI, pDir, false, pAimVal);
			pI = new CMultiValueProp(_T("z"), dir.z, _T("Specifies the z-axix value of face direction."), PID_SGITEM_Dir_z);
			pI->Enable(FALSE);
			pDir->AddSubItem(pI);
			SetItemInfo(PID_SGITEM_Dir_z, *pI, pDir, false, pAimVal);
		pDir->Enable(FALSE);
		CBCGPProp* pUp = new CBCGPProp(_T("up"), PID_SUBGROUP_Up_xyz, TRUE);
		pGroup->AddSubItem(pUp);
		SetItemInfo(PID_SUBGROUP_Up_xyz, *pUp, pGroup, true, pAimVal);
			pI = new CMultiValueProp(_T("x"), up.x, _T("Specifies the x-axix value of up direction."), PID_SGITEM_Up_x);
			pI->Enable(FALSE);
			pUp->AddSubItem(pI);
			SetItemInfo(PID_SGITEM_Up_x, *pI, pUp, false, pAimVal);
			pI = new CMultiValueProp(_T("y"), up.y, _T("Specifies the y-axix value of up direction."), PID_SGITEM_Up_y);
			pI->Enable(FALSE);
			pUp->AddSubItem(pI);
			SetItemInfo(PID_SGITEM_Up_y, *pI, pUp, false, pAimVal);
			pI = new CMultiValueProp(_T("z"), up.z, _T("Specifies the z-axix value of up direction."), PID_SGITEM_Up_z);
			pI->Enable(FALSE);
			pUp->AddSubItem(pI);
			SetItemInfo(PID_SGITEM_Up_z, *pI, pUp, false, pAimVal);
		pUp->Enable(FALSE);
		pI = new CMultiValueProp(_T("scale"), scale, _T("Specifies the scale of the object."), PID_ITEM_Scale);
		pI->Enable(m_Status.ReadFlag(SF_IS_GAME_RUNTIME)? FALSE : TRUE);
		pGroup->AddSubItem(pI);
		SetItemInfo(PID_ITEM_Scale, *pI, pGroup, false, pAimVal);
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

void CPhysXObjSelGroup::FillGroupRoleBehavior(const CDynProperty& objProp)
{
	assert(0 != m_pPropList);
	CBCGPProp* pProp = 0;
	CBCGPProp* pGroup = 0;

	const TCHAR* pRT = CDynProperty::GetRoleTypeText(objProp.GetRoleType());
	const TCHAR* pAPT = IActionPlayerBase::GetAPTypeText(objProp.GetAPType());
	const bool bHasPB = objProp.GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY);

	if (!IsMerge(objProp.GetHostObject()))
	{
		pGroup = new CBCGPProp(_T("Role&Behavior"), PID_GROUP_RoleBehavior);
		SetChiefInfo(PID_GROUP_RoleBehavior, pGroup);
		pProp = new CBCGPProp(_T("Role"), pRT, _T("Sets/returns the role type of the object."), PID_ITEM_Role);
		pProp->AddOption(CDynProperty::GetRoleTypeText(CDynProperty::ROLE_ACTOR_NONE));
		pProp->AddOption(CDynProperty::GetRoleTypeText(CDynProperty::ROLE_ACTOR_NPC));
		pProp->AllowEdit(FALSE);
		pProp->Enable(m_Status.ReadFlag(SF_IS_GAME_RUNTIME)? FALSE : TRUE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Role, *pProp, pGroup, false);
		pProp = new CMultiValueProp(_T("Behavior"), pAPT, _T("Sets/returns the object's behavior."), PID_ITEM_Behavior);
		pProp->AddOption(IActionPlayerBase::GetAPTypeText(IActionPlayerBase::AP_NULL));
		pProp->AddOption(IActionPlayerBase::GetAPTypeText(IActionPlayerBase::AP_SEQUENCE_ACTION));
		pProp->AddOption(IActionPlayerBase::GetAPTypeText(IActionPlayerBase::AP_WALK_RANDOM));
		pProp->AddOption(IActionPlayerBase::GetAPTypeText(IActionPlayerBase::AP_WALK_AROUND));
		pProp->AddOption(IActionPlayerBase::GetAPTypeText(IActionPlayerBase::AP_WALK_PATROL));
		pProp->AllowEdit(FALSE);
		if (objProp.QueryRoleType(CDynProperty::ROLE_ACTOR_NONE))
			pProp->Enable(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Behavior, *pProp, pGroup, false);
		pProp = new CMultiValueProp(_T("PhysX Body"), bHasPB, _T("Enable/disable the object's PhysX body."), PID_ITEM_PhysX_BodyEnable);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_PhysX_BodyEnable, *pProp, pGroup, false);
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
	CBCGPProp* pProp = 0;
	CBCGPProp* pGroup = 0;

	const int flags = objProp.GetApxRBFlags();
	const bool bPF = (flags & APX_OBJFLAG_CC_ADD_PUSHFORCE)? true : false;
	const bool bSF = (flags & APX_OBJFLAG_CC_ADD_STANDFORCE)? true : false;
	const bool bCA = (flags & APX_OBJFLAG_CC_CAN_ATTACH)? true : false;

	if (!IsMerge(objProp.GetHostObject()))
	{
		pGroup = new CBCGPProp(_T("APhysX RB Flags"), PID_GROUP_Apx_RBFlags);
		SetChiefInfo(PID_GROUP_Apx_RBFlags, pGroup);
		pProp = new CMultiValueProp(_T("APX Push Force"), bPF, _T("Sets/gets APhysXObjectFlag flag: APX_OBJFLAG_CC_ADD_PUSHFORCE."), PID_ITEM_Apx_PushForce);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Apx_PushForce, *pProp, pGroup, false);
		pProp = new CMultiValueProp(_T("APX Stand Force"), bSF, _T("Sets/gets APhysXObjectFlag flag: APX_OBJFLAG_CC_ADD_STANDFORCE."), PID_ITEM_Apx_StandForce);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Apx_StandForce, *pProp, pGroup, false);
		pProp = new CMultiValueProp(_T("APX Attach Enable"), bCA, _T("Sets/gets APhysXObjectFlag flag: APX_OBJFLAG_CC_CAN_ATTACH."), PID_ITEM_Apx_AttachEnable);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_Apx_AttachEnable, *pProp, pGroup, false);
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
	CBCGPProp* pProp = 0;
	CBCGPProp* pGroup = 0;
	const CAPhysXCCMgr& ccMgr = objProp.GetCCMgr();
	const bool bIsMerge = IsMerge(objProp.GetHostObject());

	if (!bIsMerge)
	{
		pGroup = new CBCGPProp(_T("CC Flags"), PID_GROUP_CC_Flags);
		SetChiefInfo(PID_GROUP_CC_Flags, pGroup);
		pProp = new CBCGPProp(_T("CC Type"), ccMgr.GetCCTypeText(), _T("Sets/gets the CC type of the object."), PID_ITEM_CC_Type);
		pProp->AllowEdit(FALSE);
		pGroup->AddSubItem(pProp);
		SetItemInfo(PID_ITEM_CC_Type, *pProp, pGroup, false);
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

void CPhysXObjSelGroup::OnUpdateBehaviorEnable(const CDynProperty& objProp)
{
	CBCGPProp* pProp = m_Props[PID_ITEM_Behavior].pItem;
	assert(0 != pProp);
	if (pProp->IsEnabled())
		return;

	if (CDynProperty::ROLE_ACTOR_NPC == objProp.GetRoleType())
		pProp->Enable(TRUE);
}

void CPhysXObjSelGroup::SetGroupPos(const A3DVECTOR3& newPos_idx0)
{
	const size_t MaxCount = m_aObjects.size();
	assert(0 < MaxCount);
	const A3DVECTOR3 deltaMove = newPos_idx0 - m_aObjects[0]->GetProperties()->GetPos();
	if (NxMath::equals(deltaMove.x, 0.0f, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(deltaMove.y, 0.0f, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(deltaMove.z, 0.0f, APHYSX_FLOAT_EPSILON))
		return;

	m_aObjects[0]->GetProperties()->SetPos(newPos_idx0);
	RefreshFramePose();
	if (1 == MaxCount)
		return;

	A3DVECTOR3 temp;
	for (unsigned int i = 1; i < MaxCount; ++i)
	{
		temp = m_aObjects[i]->GetProperties()->GetPos();
		temp += deltaMove;
		m_aObjects[i]->GetProperties()->SetPos(temp);
	}
}

void CPhysXObjSelGroup::SetGroupRot(const float angleRadians, const A3DVECTOR3& axis)
{
	const size_t MaxCount = m_aObjects.size();
	assert(0 < MaxCount);

	if (NxMath::equals(angleRadians, 0.0f, APHYSX_FLOAT_EPSILON))
		return;
	if (NxMath::equals(axis.x, 0.0f, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(axis.y, 0.0f, APHYSX_FLOAT_EPSILON) &&
		NxMath::equals(axis.z, 0.0f, APHYSX_FLOAT_EPSILON))
		return;

	A3DVECTOR3 dir, up;
	m_focusRefFrame.GetDirAndUp(dir, up);
	m_aObjects[0]->GetProperties()->SetDirAndUp(dir, up);
	RefreshFramePose();
	if (1 == MaxCount)
		return;

	NxQuat quatRot;
	quatRot.fromAngleAxisFast(angleRadians, NxVec3(axis.x, axis.y, axis.z));
	const A3DVECTOR3 anchorPos = m_focusRefFrame.GetPos();
	A3DVECTOR3 oldPos, newPos;
	NxVec3 temp, nd, nu;
	for (unsigned int i = 1; i < MaxCount; ++i)
	{
		oldPos = m_aObjects[i]->GetProperties()->GetPos();
		temp = APhysXConverter::A2N_Vector3(oldPos - anchorPos);
		temp = quatRot.rot(temp);
		newPos = anchorPos + APhysXConverter::N2A_Vector3(temp);
		m_aObjects[i]->GetProperties()->SetPos(newPos);
		m_aObjects[i].distOffset = newPos - anchorPos;
		dir = m_aObjects[i]->GetProperties()->GetDir();
		up = m_aObjects[i]->GetProperties()->GetUp();
		nd = quatRot.rot(APhysXConverter::A2N_Vector3(dir));
		nu = quatRot.rot(APhysXConverter::A2N_Vector3(up));
		m_aObjects[i]->GetProperties()->SetDirAndUp(APhysXConverter::N2A_Vector3(nd), APhysXConverter::N2A_Vector3(nu));
	}
}

void CPhysXObjSelGroup::SetGroupScale()
{
	const size_t MaxCount = m_aObjects.size();
	assert(0 < MaxCount);

	const float sOri = m_aObjects[0].originScale;
	const float newScale = m_focusRefFrame.GetScale();
	m_aObjects[0]->GetProperties()->SetScale(newScale);
	if (1 == MaxCount)
		return;

	const float sRatio = newScale / sOri;
	const A3DVECTOR3 posBase = m_aObjects[0]->GetProperties()->GetPos();
	float ns = 0.0f;
	A3DVECTOR3 tp;
	for (unsigned int i = 1; i < MaxCount; ++i)
	{
		ns = m_aObjects[i].originScale * sRatio;
		m_aObjects[i]->GetProperties()->SetScale(ns);

		tp = m_aObjects[i].distOffset * sRatio;
		tp += posBase;
		m_aObjects[i]->GetProperties()->SetPos(tp);
	}
}

void CPhysXObjSelGroup::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	size_t MaxCount = m_aObjects.size();
	for (unsigned int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->GetProperties()->SetDirAndUp(vDir, vUp);
}

bool CPhysXObjSelGroup::SetScale(const float& scale)
{
	size_t MaxCount = m_aObjects.size();
	for (unsigned int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->GetProperties()->SetScale(scale);
	return true;
}

void CPhysXObjSelGroup::OnAddObject(IObjBase& obj)
{
	SelObj sel;
	sel.pObj = &obj;
	sel.originScale = obj.GetProperties()->GetScale();
	sel.distOffset.Clear();
	if (0 < Size())
		sel.distOffset = obj.GetProperties()->GetPos() - m_aObjects[0]->GetProperties()->GetPos();

	m_aObjects.push_back(sel);
	obj.GetProperties()->SetSelGroup(this);
}

void CPhysXObjSelGroup::RefreshFramePose()
{
	if (m_Status.ReadFlag(SF_REFRESH_FRAME_DISABLE))
		return;

	const int nCount = Size();
	assert(0 < nCount);

	IPropObjBase* pObjProp = m_aObjects[0]->GetProperties();
	m_focusRefFrame.SetPos(pObjProp->GetPos());
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

int CPhysXObjSelGroup::GetIndex(const PropID& nID)
{
	assert(nID < PID_END_BOUND);
	assert(nID > PID_START_BOUND);

	int idx = nID - PID_START_BOUND;
	assert(0 <= idx);
	return idx;
}

void CPhysXObjSelGroup::SendToPropsGrid()
{
	assert(0 != m_pPropList);
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
		m_pPropList->RedrawWindow();
		return;
	}

	assert(0 != m_pPropList);
	for (unsigned int i = 0; i < selCount; ++i)
		m_aObjects[i]->GetProperties()->OnSendToPropsGrid(*m_pPropList);

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

void CPhysXObjSelGroup::SetChiefInfo(const PropID& nID, CBCGPProp* pI)
{
	int idx = nID - PID_START_BOUND;
	m_Props[idx].nCount = 1;
	m_Props[idx].pItem = pI;
	m_Props[idx].RaiseFlag(PF_IS_GROUP);
	m_Props[idx].RaiseFlag(PF_IS_CHIEF);
	m_Props[idx].RaiseFlag(PF_HAS_NEW_VALUE);
}

void CPhysXObjSelGroup::SetItemInfo(const PropID& nID, CBCGPProp& I, CBCGPProp* pL, bool isGroup, const _variant_t* pMultiValueMask)
{
	int idx = nID - PID_START_BOUND;
	m_Props[idx].nCount = 1;
	m_Props[idx].pItem = &I;
	m_Props[idx].pLeader = pL;
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

	CBCGPProp* pI = m_Props[idx].pItem;
	if (0 != pI)
	{
		BOOL rtn = m_pPropList->DeleteProperty(pI, 0, 0);
		if (rtn)
			m_Status.RaiseFlag(SF_PROPLIST_LAYOUT_DIRTY);
		else
			delete m_Props[idx].pItem;
	}
	return ClearItemInfo(idx, SaveUpdateInfo);
}

unsigned int CPhysXObjSelGroup::ClearItemInfo(unsigned int idx, bool SaveUpdateInf)
{
	assert(idx < PID_END_BOUND - PID_START_BOUND);
	
	unsigned int rtn = idx + 1;
	if (m_Props[idx].ReadFlag(PF_IS_GROUP))
	{
		const CBCGPProp* const pTheGroup = m_Props[idx].pItem;
		assert(0 != pTheGroup);
		for (int i = idx + 1; i < PID_END_BOUND - PID_START_BOUND; ++i)
		{
			if (pTheGroup != m_Props[i].pLeader)
				break;
			rtn = ClearItemInfo(i, SaveUpdateInf);
		}
	}

	bool bIsDirty = false;
	if (SaveUpdateInf)
		bIsDirty = m_Props[idx].ReadFlag(PF_IS_DIRTY);
	m_Props[idx].ClearAll();
	if (bIsDirty)
		m_Props[idx].RaiseFlag(PF_IS_DIRTY);

	return rtn;
}

void CPhysXObjSelGroup::CommitModified()
{
	const size_t nCount = m_aObjects.size();

	int idx = 0;
	while (idx < PID_END_BOUND - PID_START_BOUND)
	{
		if ((0 < m_Props[idx].nCount) && (0 < (nCount - m_Props[idx].nCount)))
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
					m_pPropList->AddProperty(m_Props[idx].pItem, 0, 0);
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

	m_Status.ClearFlag(SF_HAS_SOMETHING_DIRTY);
	if (m_Status.ReadFlag(SF_PROPLIST_LAYOUT_DIRTY))
	{
		m_pPropList->AdjustLayout();
		m_Status.ClearFlag(SF_PROPLIST_LAYOUT_DIRTY);
	}
}

void CPhysXObjSelGroup::OnUpdateCCTypeContent(const CDynProperty& objProp)
{
	CBCGPProp* pProp = m_Props[PID_ITEM_CC_Type].pItem;
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
			return;
		case CDynProperty::ROLE_ACTOR_AVATAR:
			pProp->RemoveAllOptions();
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_TOTAL_EMPTY));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_EXTCC));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_DYNCC));
			return;
		default:
			assert(!"Unknown RoleType!");
		}
	}
	else if (2 == oc)
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
			pProp->RemoveAllOptions();
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_TOTAL_EMPTY));
			pProp->AddOption(CAPhysXCCMgr::GetCCTypeText(CAPhysXCCMgr::CC_APHYSX_LWCC));
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

CBCGPProp* CPhysXObjSelGroup::CreateCCFlagProp(const PropID& nID, const _variant_t& objVal)
{
	CBCGPProp* pGroup = m_Props[PID_GROUP_CC_Flags].pItem;
	assert(0 != pGroup);

	CBCGPProp* pProp = 0;
	switch(nID)
	{
	case PID_ITEM_LWCC_RevisePosEOpen:
		pProp = new CMultiValueProp(_T("Enforce RevisePos"), objVal, _T("Sets/gets APhysXLWCCFlag flag: APX_LWCCF_REVISEPOSITION_ENFORCE_OPEN."), nID);
		break;
	case PID_ITEM_CCDCC_Slide:
		pProp = new CMultiValueProp(_T("Slide"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_SLIDE_ENABLE."), nID);
		break;
	case PID_ITEM_CCDCC_AttachEOpen:
		pProp = new CMultiValueProp(_T("Enforce AttachOpen"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_ATTACHMENT_ENFORCE_OPEN."), nID);
		break;
	case PID_ITEM_CCDCC_AttachEClose:
		pProp = new CMultiValueProp(_T("Enforce AttachClose"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_ATTACHMENT_ENFORCE_CLOSE."), nID);
		break;
	case PID_ITEM_CC_PushForce:
		pProp = new CMultiValueProp(_T("Push Force"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_ADD_PUSHFORCE."), nID);
		break;
	case PID_ITEM_CC_StandForce:
		pProp = new CMultiValueProp(_T("Stand Force"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_ADD_STANDFORCE."), nID);
		break;
	case PID_ITEM_DynCC_AutoHangEnd:
		pProp = new CMultiValueProp(_T("Auto HangEnd"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_AUTO_HANG_END_ENABLE."), nID);
		break;
	case PID_ITEM_DynCC_PassiveEffect:
		pProp = new CMultiValueProp(_T("Passive Effect"), objVal, _T("Sets/gets APhysXCCFlag flag: APX_CCF_PASSIVE_EFFECT_ONLY."), nID);
		break;
	default:
		assert(!"Wrong PropID input!");
		break;
	}

	if (0 != pProp)
	{
		pGroup->AddSubItem(pProp);
		SetItemInfo(nID, *pProp, pGroup, false);
		if (!m_Props[PID_GROUP_CC_Flags].ReadFlag(PF_HAS_NEW_VALUE))
			m_Status.RaiseFlag(SF_PROPLIST_LAYOUT_DIRTY);
	}
	return pProp;
}

