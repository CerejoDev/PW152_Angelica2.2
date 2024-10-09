/*
 * FILE: PropPhysXObjImp.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

CObjProperty::CObjProperty()
{
	// note: must call OnSetToEmpty() if the virtual function was overloaded. 
	// OnSetToEmpty();
}

CObjProperty::CObjProperty(const CObjProperty& rhs) : IPropPhysXObjBase(rhs) 
{
	// note: must call OnSetToEmpty() if the virtual function was overloaded. 
	// OnSetToEmpty();
	// do the snap prop value action if necessary
}

CObjProperty* CObjProperty::CreateMyselfInstance(bool snapValues) const
{
	CObjProperty* pProp = 0;
	if (snapValues)
		pProp = new CObjProperty(*this);
	else
		pProp = new CObjProperty;
	return pProp;
}

IRoleCallback* CDynProperty::gRoleCallback = 0;

const TCHAR* CDynProperty::GetRoleTypeText(const RoleType rt)
{
	switch (rt)
	{
	case ROLE_ACTOR_NONE:
		return _T("None");
	case ROLE_ACTOR_NPC:
		return _T("NPC");
	case ROLE_ACTOR_AVATAR:
		return _T("Avatar");
	}

	assert(!"Unknown RoleType!");
	return szEmpty;
}

bool CDynProperty::GetRoleType(const TCHAR* pStr, RoleType& outRT)
{
	if (0 == a_strcmp(pStr, szEmpty))
		return false;

	if (0 == a_strcmp(pStr, GetRoleTypeText(ROLE_ACTOR_NONE)))
	{
		outRT = ROLE_ACTOR_NONE;
		return true;
	}
	if (0 == a_strcmp(pStr, GetRoleTypeText(ROLE_ACTOR_NPC)))
	{
		outRT = ROLE_ACTOR_NPC;
		return true;
	}
	if (0 == a_strcmp(pStr, GetRoleTypeText(ROLE_ACTOR_AVATAR)))
	{
		outRT = ROLE_ACTOR_AVATAR;
		return true;
	}

	assert(!"Unknown RoleTypeText!");
	return false;
}

CDynProperty::CDynProperty()
{
	m_pAC = 0;
	OnSetToEmpty();
}

CDynProperty::CDynProperty(const CDynProperty& rhs) : IPropPhysXObjBase(rhs)
{
	m_pAC = 0;
	OnSetToEmpty();
	SetRoleType(rhs.m_roleType);
	SetAPType(rhs.m_apType);
	m_rbFlags = rhs.m_rbFlags;
}

CDynProperty::~CDynProperty()
{
	delete m_pAC;
}

CDynProperty& CDynProperty::operator= (const CDynProperty& rhs)
{
	IPropPhysXObjBase::operator=(rhs);

	SetRoleType(rhs.m_roleType);
	SetAPType(rhs.m_apType);
	m_rbFlags = rhs.m_rbFlags;
	m_CCMgr.SwtichCC(rhs.m_CCMgr.GetCCType());

	if (0 != rhs.m_pAC)
		SetActionPlayer(rhs.m_pAC);
	else
	{
		delete m_pAC;
		m_pAC = 0;
	}

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
	{
		pSelGroup->RaiseUpdateMark(PID_ITEM_Behavior);
		pSelGroup->RaiseUpdateMark(PID_GROUP_Apx_RBFlags);
		pSelGroup->RaiseUpdateMark(PID_GROUP_CC_Flags);
	}
	return *this;
}

void CDynProperty::OnSetToEmpty()
{
	m_rbFlags = 0;
	if (0 != m_pAC)
		delete m_pAC;
	m_pAC = 0;
	m_RoleCallback = 0;
	SetRoleType(ROLE_ACTOR_NONE);
	m_Status.RaiseFlag(OBFI_RESTART_ACTION_PLAY);
}

void CDynProperty::InitHostObjectPost()
{
	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(m_pHostObject);
	assert(0 != pDynObj);
	if (0 != pDynObj)
		m_CCMgr.InitHostObject(*pDynObj);
}

void CDynProperty::InstancePhysXPostNotify()
{
	if (m_Status.ReadFlag(OBFI_INITED_APHYSX_FLAGS))
	{
		ApplyRBFlags(PID_GROUP_Apx_RBFlags);
		return;
	}

	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(m_pHostObject);
	APhysXSkeletonRBObject* pPSRB = pDynObj->GetSkeletonRBObject();
	if (0 == pPSRB)
		return;

	int flags = 0;
	if (pPSRB->GetObjectFlag(APX_OBJFLAG_CC_ADD_PUSHFORCE))
		flags |= APX_OBJFLAG_CC_ADD_PUSHFORCE;
	if (pPSRB->GetObjectFlag(APX_OBJFLAG_CC_ADD_STANDFORCE))
		flags |= APX_OBJFLAG_CC_ADD_STANDFORCE;
	if (pPSRB->GetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH))
		flags |= APX_OBJFLAG_CC_CAN_ATTACH;

	m_rbFlags = flags;
	m_Status.RaiseFlag(OBFI_INITED_APHYSX_FLAGS);
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
	{
		assert(!"Shouldn't be here! To Update Codes");
	/*	if (0 != m_pBackupProps)
		{
			CDynProperty* pDynBKProp = static_cast<CDynProperty*>(m_pBackupProps);
			pDynBKProp->m_rbFlags = flags;
			pDynBKProp->m_Status.RaiseFlag(IObjPropertyBase::OBFI_INITED_APHYSX_FLAGS);
		}
		*/
	}
}

bool CDynProperty::SetRoleType(const RoleType& rt)
{
	if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
		return false;
	if (rt == m_roleType)
		return true;

	m_roleType = rt;
	if (ROLE_ACTOR_NONE == rt)
	{
		m_rbFlags = 0;
		m_RoleCallback = 0;
		APhysXCompoundObjectDesc desc;
		if (desc.mFlags & APX_OBJFLAG_CC_ADD_PUSHFORCE)  m_rbFlags |= APX_OBJFLAG_CC_ADD_PUSHFORCE;
		if (desc.mFlags & APX_OBJFLAG_CC_ADD_STANDFORCE) m_rbFlags |= APX_OBJFLAG_CC_ADD_STANDFORCE;
		if (desc.mFlags & APX_OBJFLAG_CC_CAN_ATTACH)	 m_rbFlags |= APX_OBJFLAG_CC_CAN_ATTACH;
		SetAPType(IActionPlayerBase::AP_NULL);
		SetCCType(CAPhysXCCMgr::CC_TOTAL_EMPTY);
		SetDrivenMode(DRIVEN_BY_PURE_PHYSX, true);
	}
	else if (ROLE_ACTOR_NPC == rt)
	{
		m_RoleCallback = gRoleCallback;
		SetAPType(IActionPlayerBase::AP_NULL);
		SetCCType(CAPhysXCCMgr::CC_BRUSH_CDRCC);
		SetDrivenMode(DRIVEN_BY_ANIMATION, true);
		// SetDrivenMode(DRIVEN_BY_PART_PHYSX, true);
	}
	else if (ROLE_ACTOR_AVATAR == rt)
	{
		m_RoleCallback = gRoleCallback;
		SetAPType(IActionPlayerBase::AP_WALK_USERCTRL);
		SetCCType(CAPhysXCCMgr::CC_BRUSH_CDRCC);
		SetDrivenMode(DRIVEN_BY_ANIMATION, true);
		// SetDrivenMode(DRIVEN_BY_PART_PHYSX, true);
	}
	return true;
}

bool CDynProperty::QueryRoleType(const RoleType& rt) const
{
	return rt == m_roleType;
}

bool CDynProperty::SetAPType(const APType& ap)
{
	if (ap == m_apType)
		return true;

	if (IActionPlayerBase::AP_NULL == ap)
	{
		const bool res = QueryRoleType(ROLE_ACTOR_NONE);
//	toupdate	assert(true == res);
//		if (!res)
//			return false;
	}
	else if (IActionPlayerBase::AP_WALK_USERCTRL == ap)
	{
		const bool res = QueryRoleType(ROLE_ACTOR_AVATAR);
		assert(true == res);
		if (!res)
			return false;
	}
	else
	{
		const bool res = QueryRoleType(ROLE_ACTOR_NPC);
		assert(true == res);
		if (!res)
			return false;
	}
	m_apType = ap;
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMark(PID_ITEM_Behavior);
	return true;
}

bool CDynProperty::QueryAPType(const APType& ap) const
{
	return ap == m_apType;
}

bool CDynProperty::SetCCType(const CCType& cc)
{
	if (cc == m_CCMgr.GetCCType())
		return true;

	if (CAPhysXCCMgr::CC_APHYSX_LWCC == cc)
	{
		const bool res = QueryRoleType(ROLE_ACTOR_NPC);
		assert(true == res);
		if (!res)
			return false;
	}
	else if ((CAPhysXCCMgr::CC_APHYSX_EXTCC == cc) || (CAPhysXCCMgr::CC_APHYSX_DYNCC == cc))
	{
		const bool res = QueryRoleType(ROLE_ACTOR_AVATAR);
		assert(true == res);
		if (!res)
			return false;
	}

	m_CCMgr.SwtichCC(cc);
	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 != pSelGroup)
		pSelGroup->RaiseUpdateMarkToCCFlags();
	return true;
}

void CDynProperty::EnterRuntimePreNotify(APhysXScene& aPhysXScene)
{
	IPropPhysXObjBase::EnterRuntimePreNotify(aPhysXScene); 
	CDynProperty* pBK = dynamic_cast<CDynProperty*>(m_pBackupProps);
	if (0 != pBK)
		*pBK= *this;
}

void CDynProperty::LeaveRuntimePostNotify()
{
	IPropPhysXObjBase::LeaveRuntimePostNotify();
	CDynProperty* pBK = dynamic_cast<CDynProperty*>(m_pBackupProps);
	if (0 != pBK)
		*this = *pBK;
}

void CDynProperty::EnterRuntimePostNotify(APhysXScene& aPhysXScene)
{
	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(m_pHostObject);
	assert(0 != pDynObj);
	if (0 != pDynObj)
	{
		switch (m_apType)
		{
		case IActionPlayerBase::AP_NULL:
			SetActionPlayer(0);
			break;
		case IActionPlayerBase::AP_SEQUENCE_ACTION:
			SetActionPlayer(&APSequence(*pDynObj));
			break;
		case IActionPlayerBase::AP_WALK_RANDOM:
		case IActionPlayerBase::AP_WALK_AROUND:
			SetActionPlayer(&ACNPCWalk(*pDynObj, m_CCMgr, true));
			break;
		case IActionPlayerBase::AP_WALK_PATROL:
			SetActionPlayer(&ACNPCWalk(*pDynObj, m_CCMgr, false));
			break;
		case IActionPlayerBase::AP_WALK_USERCTRL:
			SetActionPlayer(&ACMAWalk(*pDynObj, m_CCMgr));
			break;
		}

		if (0 != m_RoleCallback)
		{
			if (QueryRoleType(ROLE_ACTOR_NPC))
				m_RoleCallback->CreateNPCRole(*pDynObj);
		}
	}
	if (!QueryRoleType(ROLE_ACTOR_NONE))
	{
		IAPWalk* pWalk = 0;
		if (IActionPlayerBase::AP_SEQUENCE_ACTION < GetAPType())
			pWalk = static_cast<IAPWalk*>(m_pAC);
		m_CCMgr.OnEnterRuntime(aPhysXScene, pWalk);
		NxVec3 vDir, vUp;
		m_pHostObject->GetGDirAndUp(vDir, vUp);
		APhysXHostCCBase* pHost = m_CCMgr.GetAPhysXHostCCBase();
		if (0 != pHost)
			pHost->SetDir(vDir);
		BrushcharCtrler* pBrush = m_CCMgr.GetBrushHostCC();
		if (0 != pBrush)
			pBrush->SetDir(APhysXConverter::N2A_Vector3(vDir));
	}
	m_Status.RaiseFlag(OBFI_ENABLE_ACTION_PLAY);
}

void CDynProperty::LeaveRuntimePreNotify()
{
	m_Status.ClearFlag(OBFI_ENABLE_ACTION_PLAY);
	m_CCMgr.OnLeaveRuntime();
	delete m_pAC;
	m_pAC = 0;
}

bool CDynProperty::SaveToFile(NxStream& nxStream)
{
	if (!IPropPhysXObjBase::SaveToFile(nxStream))
		return false;

	SaveVersion(&nxStream);
	nxStream.storeDword(m_roleType);
	nxStream.storeDword(m_apType);
	nxStream.storeDword(m_rbFlags);
	nxStream.storeDword(m_CCMgr.GetCCType());
	return true;
}

bool CDynProperty::LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
{
	if (!IPropPhysXObjBase::LoadFromFile(nxStream, outIsLowVersion))
		return false;

	LoadVersion(&nxStream);
	if(m_StreamObjVer < GetVersion())
		outIsLowVersion = true;

	if(m_StreamObjVer >= 0xAA000001)
	{
		SetRoleType(RoleType(nxStream.readDword()));
		SetAPType(APType(nxStream.readDword()));
		m_rbFlags = nxStream.readDword();
		int ccType = nxStream.readDword();
		if ((ccType > CAPhysXCCMgr::CC_BRUSH_CDRCC) || (ccType < CAPhysXCCMgr::CC_TOTAL_EMPTY))
		{
			ccType = CAPhysXCCMgr::CC_TOTAL_EMPTY;
			outIsLowVersion = true;
		}
		m_CCMgr.SwtichCC(CCType(ccType));
	}

	return true;
}

bool CDynProperty::OnSendToPropsGrid()
{
	if (!IPropPhysXObjBase::OnSendToPropsGrid())
		return false;

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	assert(0 != pSelGroup);
	pSelGroup->FillGroupRoleBehavior(*this);
	if (QueryRoleType(CDynProperty::ROLE_ACTOR_NONE))
		pSelGroup->FillGroupApxRBFlags(*this);
	else
		pSelGroup->FillGroupCCFlags(*this);
	return true;
}

bool CDynProperty::OnPropertyChanged(PropItem& prop)
{
	if (IPropPhysXObjBase::OnPropertyChanged(prop))
		return true;

	const DWORD_PTR propValue = prop.GetData();
	if (PID_ITEM_Role == propValue)
	{
		RoleType rt;
		bool rtn = GetRoleType(_bstr_t(prop.GetValue()), rt);
		if (rtn)
		{
			SetRoleType(rt);
			CPhysXObjSelGroup* pSelGroup = GetSelGroup();
			if (0 != pSelGroup)
			{
				pSelGroup->RaiseUpdateMark(PID_GROUP_Apx_RBFlags, true);
				pSelGroup->RaiseUpdateMark(PID_GROUP_CC_Flags, true);
			}
		}
		return rtn;
	}
	else if (PID_ITEM_PhysX_BodyEnable == propValue)
	{
		CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(m_pHostObject);
		pDynObj->ApplyPhysXBody(prop.GetValue(), m_CCMgr.GetRuntimeScene());
		return true;
	}
	else if (PID_ITEM_Apx_PushForce == propValue)
	{
		const bool bPF = prop.GetValue();
		if (bPF)
			m_rbFlags |= APX_OBJFLAG_CC_ADD_PUSHFORCE;
		else
			m_rbFlags &= ~APX_OBJFLAG_CC_ADD_PUSHFORCE;
		if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
			ApplyRBFlags(PID_ITEM_Apx_PushForce);
		return true;
	}
	else if (PID_ITEM_Apx_StandForce == propValue)
	{
		const bool bSF = prop.GetValue();
		if (bSF)
			m_rbFlags |= APX_OBJFLAG_CC_ADD_STANDFORCE;
		else
			m_rbFlags &= ~APX_OBJFLAG_CC_ADD_STANDFORCE;
		if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
			ApplyRBFlags(PID_ITEM_Apx_StandForce);
		return true;
	}
	else if (PID_ITEM_Apx_AttachEnable == propValue)
	{
		const bool bCA = prop.GetValue();
		if (bCA)
			m_rbFlags |= APX_OBJFLAG_CC_CAN_ATTACH;
		else
			m_rbFlags &= ~APX_OBJFLAG_CC_CAN_ATTACH;
		if (m_Status.ReadFlag(OBFI_IS_IN_RUNTIME))
			ApplyRBFlags(PID_ITEM_Apx_AttachEnable);
		return true;
	}
	else if (PID_ITEM_CC_Type == propValue)
	{
		CCType cc;
		bool rtn = CAPhysXCCMgr::GetCCType(_bstr_t(prop.GetValue()), cc);
		if (rtn)
		{
			SetCCType(cc);
			return true;
		}
	}
	else if (PID_ITEM_LWCC_RevisePosEOpen == propValue)
	{
		m_CCMgr.SetFlag(APhysXLWCharCtrler::APX_LWCCF_REVISEPOSITION_ENFORCE_OPEN, prop.GetValue());
		return true;
	}
	else if (PID_ITEM_CCDCC_Slide == propValue)
	{
		m_CCMgr.SetFlagSlide(prop.GetValue());
		return true;	
	}
	else if (PID_ITEM_CC_PushForce == propValue)
	{
		m_CCMgr.SetFlag(APhysXCharacterController::APX_CCF_ADD_PUSHFORCE, prop.GetValue());
		return true;	
	}
	else if (PID_ITEM_CC_StandForce == propValue)
	{
		m_CCMgr.SetFlag(APhysXCharacterController::APX_CCF_ADD_STANDFORCE, prop.GetValue());
		return true;	

	}
	else if (PID_ITEM_DynCC_AutoHangEnd == propValue)
	{
		m_CCMgr.SetFlag(APhysXDynCharCtrler::APX_CCF_AUTO_HANG_END_ENABLE, prop.GetValue());
		return true;	
	}
	else if (PID_ITEM_DynCC_PassiveEffect == propValue)
	{
		m_CCMgr.SetFlag(APhysXDynCharCtrler::APX_CCF_PASSIVE_EFFECT_ONLY, prop.GetValue());
		return true;	
	}
	else if (PID_ITEM_CCDCC_AttachEOpen == propValue)
	{
		m_CCMgr.SetFlagAttachEnforceOpen(prop.GetValue());
		return true;	
	}
	else if (PID_ITEM_CCDCC_AttachEClose == propValue)
	{
		m_CCMgr.SetFlagAttachEnforceClose(prop.GetValue());
		return true;	
	}
	return false;
}

void CDynProperty::OnUpdatePropsGrid()
{
	IPropPhysXObjBase::OnUpdatePropsGrid();

	CPhysXObjSelGroup* pSelGroup = GetSelGroup();
	if (0 == pSelGroup)
		return;

	const bool bIsMerge = pSelGroup->IsMerge(m_pHostObject);

	const bool u1 = pSelGroup->ReadUpdateMark(PID_GROUP_RoleBehavior);
	const bool u2 = pSelGroup->ReadUpdateMark(PID_ITEM_Role);
	const bool u3 = pSelGroup->ReadUpdateMark(PID_ITEM_PhysX_BodyEnable);
//	const bool u4 = pSelGroup->ReadUpdateMark(PID_ITEM_CC_Type);
	if (u1 || u2 || u3)// || u4)  to update: how to deal with this???
	{
		assert(!"Shouldn't be changed or not supported yet!");
	}

	if (pSelGroup->ReadUpdateMark(PID_ITEM_Behavior))
	{
		pSelGroup->ExecuteUpdateItem(PID_ITEM_Behavior, IActionPlayerBase::GetAPTypeText(GetAPType()), bIsMerge);
		pSelGroup->OnUpdateBehaviorEnable(*this);
	}

	if (pSelGroup->ReadUpdateMark(PID_GROUP_Apx_RBFlags))
	{
		if (QueryRoleType(CDynProperty::ROLE_ACTOR_NONE))
			pSelGroup->FillGroupApxRBFlags(*this);
	}
	else
	{
		if (pSelGroup->ReadUpdateMark(PID_ITEM_Apx_PushForce))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_Apx_PushForce, (m_rbFlags & APX_OBJFLAG_CC_ADD_PUSHFORCE)? true:false, bIsMerge);

		if (pSelGroup->ReadUpdateMark(PID_ITEM_Apx_StandForce))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_Apx_StandForce, (m_rbFlags & APX_OBJFLAG_CC_ADD_STANDFORCE)? true:false, bIsMerge);

		if (pSelGroup->ReadUpdateMark(PID_ITEM_Apx_AttachEnable))
			pSelGroup->ExecuteUpdateItem(PID_ITEM_Apx_StandForce, (m_rbFlags & APX_OBJFLAG_CC_CAN_ATTACH)? true:false, bIsMerge);
	}

	if (pSelGroup->ReadUpdateMark(PID_GROUP_CC_Flags))
	{
		if (!QueryRoleType(CDynProperty::ROLE_ACTOR_NONE))
			pSelGroup->FillGroupCCFlags(*this);
	}
	else
	{
		switch(m_CCMgr.GetCCType())
		{
		case CAPhysXCCMgr::CC_TOTAL_EMPTY:
			break;
		case CAPhysXCCMgr::CC_APHYSX_LWCC:
			if (pSelGroup->ReadUpdateMark(PID_ITEM_LWCC_RevisePosEOpen))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_LWCC_RevisePosEOpen, m_CCMgr.ReadFlag(APhysXLWCharCtrler::APX_LWCCF_REVISEPOSITION_ENFORCE_OPEN), bIsMerge);
			break;
		case CAPhysXCCMgr::CC_APHYSX_EXTCC:
			if (pSelGroup->ReadUpdateMark(PID_ITEM_CCDCC_Slide))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_CCDCC_Slide, m_CCMgr.ReadFlagSlide(), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_ITEM_CC_PushForce))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_CC_PushForce, m_CCMgr.ReadFlag(APhysXCharacterController::APX_CCF_ADD_PUSHFORCE), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_ITEM_CC_StandForce))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_CC_StandForce, m_CCMgr.ReadFlag(APhysXCharacterController::APX_CCF_ADD_STANDFORCE), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_ITEM_CCDCC_AttachEOpen))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_CCDCC_AttachEOpen, m_CCMgr.ReadFlagAttachEnforceOpen(), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_ITEM_CCDCC_AttachEClose))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_CCDCC_AttachEClose, m_CCMgr.ReadFlagAttachEnforceClose(), bIsMerge);
			break;
		case CAPhysXCCMgr::CC_APHYSX_DYNCC:
			if (pSelGroup->ReadUpdateMark(PID_ITEM_CCDCC_Slide))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_CCDCC_Slide, m_CCMgr.ReadFlagSlide(), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_ITEM_DynCC_AutoHangEnd))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_DynCC_AutoHangEnd, m_CCMgr.ReadFlag(APhysXDynCharCtrler::APX_CCF_AUTO_HANG_END_ENABLE), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_ITEM_DynCC_PassiveEffect))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_DynCC_PassiveEffect, m_CCMgr.ReadFlag(APhysXDynCharCtrler::APX_CCF_PASSIVE_EFFECT_ONLY), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_ITEM_CCDCC_AttachEOpen))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_CCDCC_AttachEOpen, m_CCMgr.ReadFlagAttachEnforceOpen(), bIsMerge);
			if (pSelGroup->ReadUpdateMark(PID_ITEM_CCDCC_AttachEClose))
				pSelGroup->ExecuteUpdateItem(PID_ITEM_CCDCC_AttachEClose, m_CCMgr.ReadFlagAttachEnforceClose(), bIsMerge);
			break;
		default:
			break;
		}
	}
}


CDynProperty* CDynProperty::CreateMyselfInstance(bool snapValues) const
{
	CDynProperty* pProp = 0;
	if (snapValues)
		pProp = new CDynProperty(*this);
	else
		pProp = new CDynProperty;
	return pProp;
}

void CDynProperty::SetActionPlayer(const IActionPlayerBase* pAC)
{
	if (0 == pAC)
	{
		delete m_pAC;
		m_pAC = 0;
	}
	else
	{
		if (0 != m_pAC)
		{
			if (pAC->GetAPType() == m_pAC->GetAPType())
				return;

			delete m_pAC;
		}
		m_pAC = pAC->Clone();
	}
}

bool CDynProperty::ApplyRBFlags(const PropID& pid)
{
	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(m_pHostObject);
	APhysXSkeletonRBObject* pPSRB = pDynObj->GetSkeletonRBObject();
	if (0 == pPSRB)
		return false;

	switch (pid)
	{
	case PID_ITEM_Apx_PushForce:
		pPSRB->SetObjectFlag(APX_OBJFLAG_CC_ADD_PUSHFORCE, (m_rbFlags & APX_OBJFLAG_CC_ADD_PUSHFORCE)? true:false);
		break;
	case PID_ITEM_Apx_StandForce:
		pPSRB->SetObjectFlag(APX_OBJFLAG_CC_ADD_STANDFORCE, (m_rbFlags & APX_OBJFLAG_CC_ADD_STANDFORCE)? true:false);
		break;
	case PID_ITEM_Apx_AttachEnable:
		pPSRB->SetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH,		(m_rbFlags & APX_OBJFLAG_CC_CAN_ATTACH)? true:false);
		break;
	case PID_GROUP_Apx_RBFlags:
		pPSRB->SetObjectFlag(APX_OBJFLAG_CC_ADD_PUSHFORCE,	(m_rbFlags & APX_OBJFLAG_CC_ADD_PUSHFORCE)? true:false);
		pPSRB->SetObjectFlag(APX_OBJFLAG_CC_ADD_STANDFORCE, (m_rbFlags & APX_OBJFLAG_CC_ADD_STANDFORCE)? true:false);
		pPSRB->SetObjectFlag(APX_OBJFLAG_CC_CAN_ATTACH,		(m_rbFlags & APX_OBJFLAG_CC_CAN_ATTACH)? true:false);
		break;
	default:
		assert(!"Wrong PropID input!");
		return false;
	}
	return true;
}

void CDynProperty::UpdateMove(float deltaTimeSec)
{
	if (m_CCMgr.newStyleTest)
		return;

	APhysXHostCCBase* pHostCC = m_CCMgr.GetAPhysXHostCCBase();
	if (0 != m_pAC)
		m_pAC->UpdateMove(deltaTimeSec, (0 == pHostCC)? true : false);
}

void CDynProperty::DoMoveCC(float deltaTimeSec)
{
	m_CCMgr.DoMoveCC(deltaTimeSec);
}

bool CDynProperty::PlayAction(const unsigned long deltaTime)
{
	m_CCMgr.SyncNonDynCCPoseToModel();
	if (0 == m_pAC) 
		return false;

	if (m_CCMgr.newStyleTest)
		return false;

	if (QueryDrivenMode(DRIVEN_BY_PURE_PHYSX))
		return false;

	if (!m_Status.ReadFlag(OBFI_ENABLE_ACTION_PLAY))
		return false;

	if (m_Status.ReadFlag(OBFI_RESTART_ACTION_PLAY))
	{
		FinishCurrentAction();
		m_pAC->PlayAction(deltaTime, true);
		m_Status.ClearFlag(OBFI_RESTART_ACTION_PLAY);
	}
	else
	{
		m_pAC->PlayAction(deltaTime, false);
	}
	return true;
}

void CDynProperty::FinishCurrentAction()
{
	if (0 != m_pAC)
		m_pAC->FinishCurrentAction(); 
}
