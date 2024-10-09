/*
 * FILE: APhysXCCMgr.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/12/02
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */
#include "stdafx.h"
#include "BrushcharCtrler.h"
#include "AppInterface.h"

#ifdef _ANGELICA21
	extern IKInterfaceImp s_IKInterface;
#endif

bool gbIsUsingAsynchronousCC = false;

const TCHAR* CAPhysXCCMgr::GetCCTypeText(const CCType ccType)
{
	switch (ccType)
	{
	case CC_UNKNOWN_TYPE:
		break;
	case CC_TOTAL_EMPTY:
		return _T("NONE");
	case CC_APHYSX_LWCC:
		return _T("LWCC");
	case CC_APHYSX_NXCC:
		return _T("NxCC");
	case CC_APHYSX_EXTCC:
		return _T("ExtCC");
	case CC_APHYSX_DYNCC:
		return _T("DynCC");
	case CC_BRUSH_CDRCC:
		return _T("BrushCC");
	}

	assert(!"Unknown CCType!");
	return IPropObjBase::szEmpty;
}

bool CAPhysXCCMgr::GetCCType(const TCHAR* pStr, CCType& outRT)
{
	outRT = CC_UNKNOWN_TYPE;
	if (0 == a_strcmp(pStr, IPropObjBase::szEmpty))
		return false;

	if (0 == a_strcmp(pStr, GetCCTypeText(CC_TOTAL_EMPTY)))
	{
		outRT = CC_TOTAL_EMPTY;
		return true;
	}
	if (0 == a_strcmp(pStr, GetCCTypeText(CC_APHYSX_LWCC)))
	{
		outRT = CC_APHYSX_LWCC;
		return true;
	}
	if (0 == a_strcmp(pStr, GetCCTypeText(CC_APHYSX_NXCC)))
	{
		outRT = CC_APHYSX_NXCC;
		return true;
	}
	if (0 == a_strcmp(pStr, GetCCTypeText(CC_APHYSX_EXTCC)))
	{
		outRT = CC_APHYSX_EXTCC;
		return true;
	}
	if (0 == a_strcmp(pStr, GetCCTypeText(CC_APHYSX_DYNCC)))
	{
		outRT = CC_APHYSX_DYNCC;
		return true;
	}
	if (0 == a_strcmp(pStr, GetCCTypeText(CC_BRUSH_CDRCC)))
	{
		outRT = CC_BRUSH_CDRCC;
		return true;
	}
	assert(!"Unknown CCTypeText!");
	return false;
}

CAPhysXCCMgr::CAPhysXCCMgr()
{
	m_UserData = 0;
	newStyleTest = false;
	m_pAPhysXCCBase = 0;
	m_pAPhysXHostCC = 0;

	m_pAPhysXLWCC  = 0;
	m_pAPhysXNxCC  = 0;
	m_pAPhysXExtCC = 0;
	m_pAPhysXDynCC = 0;
	m_pBrushCDRCC  = 0;

	m_pHostDynObj = 0;
	m_pAPhysXScene = 0;
	m_pWalk = 0;
	m_defVolume.set(0.3f, 1.6f, -1);

	m_bIsRumtime = false;
	m_ccEditType = CC_TOTAL_EMPTY;
	m_ccEditFlags = 0;
	m_ccRumtimeType = CC_TOTAL_EMPTY;
	m_ccRumtimeFlags = 0;
}

CAPhysXCCMgr::~CAPhysXCCMgr()
{
	OnLeaveRuntime();
}

void CAPhysXCCMgr::OnEnterRuntime(APhysXScene& aPhysXScene, IAPWalk* pWalk)
{
	m_pAPhysXScene = &aPhysXScene;
	m_pWalk = pWalk;
	ApplyCCType(m_ccEditType, m_ccEditFlags);
	m_bIsRumtime = true;
}

void CAPhysXCCMgr::OnLeaveRuntime()
{
	m_bIsRumtime = false;
	ReleaseAPhysXLWCC();
	ReleaseAPhysXNxCC();
	ReleaseAPhysXExtCC();
	ReleaseAPhysXDynCC();
	ReleaseBrushCDRCC();
	m_pAPhysXScene = 0;
	m_pWalk = 0;
	m_ccRumtimeType = CC_TOTAL_EMPTY;
	m_ccRumtimeFlags = 0;
}

bool CAPhysXCCMgr::SwtichCC(const CCType& toType, bool bEnsureSuccess)
{
	bool bIsOK = false;
	if (m_bIsRumtime)
	{
		if (toType == m_ccRumtimeType)
		{
			if (0 != m_pAPhysXCCBase)
			{
				if (&(m_pAPhysXCCBase->GetAPhysXScene()) == m_pAPhysXScene)
					bIsOK = true;
				else
					a_LogOutput(1, "Warning in %s: Recreate CC with the same type but in different scene!", __FUNCTION__);
			}
		}
	}
	else
	{
		if (toType == m_ccEditType)
			bIsOK = true;
	}
	if (bIsOK)
		return true;

	int newCCFlags = 0;
	if (CC_APHYSX_EXTCC == toType)
	{
		APhysXCCCapsuleDesc ccDesc;
		newCCFlags = ccDesc.flags;
		GetRelevantFlagsFromDynCCToCC(newCCFlags);
	}
	else if (CC_APHYSX_DYNCC == toType)
	{
		APhysXCCCapsuleDesc ccDesc;
		ccDesc.SetDefaultFlags(true);
		newCCFlags = ccDesc.flags;
		GetRelevantFlagsFromCCToDynCC(newCCFlags);
	}

	if (!m_bIsRumtime)
	{
		m_ccEditFlags = newCCFlags;
		m_ccEditType = toType;
		return true;
	}

	return ApplyCCType(toType, newCCFlags, bEnsureSuccess);
}

bool CAPhysXCCMgr::QueryCCType(const CCType& cc) const
{
	if (m_bIsRumtime)
		return cc == m_ccRumtimeType;
	return cc == m_ccEditType;
}

CAPhysXCCMgr::CCType CAPhysXCCMgr::GetCCType() const
{
	if (m_bIsRumtime)
		return m_ccRumtimeType;
	return m_ccEditType;
}

NxActor* CAPhysXCCMgr::GetHiddenActor() const
{
	if (0 == m_pAPhysXCCBase)
		return 0;
	return m_pAPhysXCCBase->GetHiddenActor();
}

bool CAPhysXCCMgr::IsHangMode() const
{
	if (0 == m_pAPhysXDynCC)
		return false;

	int IsHang = APhysXDynCharCtrler::APX_CCS_HANGING & m_pAPhysXDynCC->GetStatus();
	return (0 != IsHang);
}

bool CAPhysXCCMgr::IsInAir() const
{
	if (0 != m_pAPhysXCCBase)
	{
		int rtn = APhysXCCBase::APX_CCS_JUMPING_INAIR & m_pAPhysXCCBase->GetStatus();
		return 0 != rtn;
	}

	if (0 != m_pBrushCDRCC)
		return m_pBrushCDRCC->IsInAir();

	return false;
}

int CAPhysXCCMgr::GetCurrentFlags() const
{
	if (m_bIsRumtime)
		return m_ccRumtimeFlags;
	return m_ccEditFlags;
}

void CAPhysXCCMgr::SetCurrentFlags(int flags)
{
	CCType cctype = GetCCType();
	if (CC_TOTAL_EMPTY == cctype)
		return;
	if (CC_APHYSX_NXCC == cctype)
		return;
	if (CC_BRUSH_CDRCC == cctype)
		return;

	if (CC_APHYSX_LWCC == cctype)
	{
		if (APhysXLWCharCtrler::APX_LWCCF_REVISEPOSITION_ENFORCE_OPEN & flags)
			SetFlag(APhysXLWCharCtrler::APX_LWCCF_REVISEPOSITION_ENFORCE_OPEN, true);
		else
			SetFlag(APhysXLWCharCtrler::APX_LWCCF_REVISEPOSITION_ENFORCE_OPEN, false);
		if (APhysXLWCharCtrler::APX_LWCCF_JUMPING_INAIR & flags)
			SetFlag(APhysXLWCharCtrler::APX_LWCCF_JUMPING_INAIR, true);
		else
			SetFlag(APhysXLWCharCtrler::APX_LWCCF_JUMPING_INAIR, false);
	}
	else if (CC_APHYSX_EXTCC == cctype)
	{
		if (APhysXCharacterController::APX_CCF_GRAVITY_ENABLE & flags)
			SetFlag(APhysXCharacterController::APX_CCF_GRAVITY_ENABLE, true);
		else
			SetFlag(APhysXCharacterController::APX_CCF_GRAVITY_ENABLE, false);
		if (APhysXCharacterController::APX_CCF_GRAVITY_READSCENE & flags)
			SetFlag(APhysXCharacterController::APX_CCF_GRAVITY_READSCENE, true);
		else
			SetFlag(APhysXCharacterController::APX_CCF_GRAVITY_READSCENE, false);
		if (APhysXCharacterController::APX_CCF_SLIDE_ENABLE & flags)
			SetFlag(APhysXCharacterController::APX_CCF_SLIDE_ENABLE, true);
		else
			SetFlag(APhysXCharacterController::APX_CCF_SLIDE_ENABLE, false);
		if (APhysXCharacterController::APX_CCF_ADD_PUSHFORCE & flags)
			SetFlag(APhysXCharacterController::APX_CCF_ADD_PUSHFORCE, true);
		else
			SetFlag(APhysXCharacterController::APX_CCF_ADD_PUSHFORCE, false);
		if (APhysXCharacterController::APX_CCF_ADD_STANDFORCE & flags)
			SetFlag(APhysXCharacterController::APX_CCF_ADD_STANDFORCE, true);
		else
			SetFlag(APhysXCharacterController::APX_CCF_ADD_STANDFORCE, false);
		if (APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN & flags)
			SetFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN, true);
		else
			SetFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN, false);
		if (APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE & flags)
			SetFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE, true);
		else
			SetFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE, false);
	}
	else if (CC_APHYSX_DYNCC == cctype)
	{
		if (APhysXDynCharCtrler::APX_CCF_SLIDE_ENABLE & flags)
			SetFlag(APhysXDynCharCtrler::APX_CCF_SLIDE_ENABLE, true);
		else
			SetFlag(APhysXDynCharCtrler::APX_CCF_SLIDE_ENABLE, false);
		if (APhysXDynCharCtrler::APX_CCF_AUTO_HANG_END_ENABLE & flags)
			SetFlag(APhysXDynCharCtrler::APX_CCF_AUTO_HANG_END_ENABLE, true);
		else
			SetFlag(APhysXDynCharCtrler::APX_CCF_AUTO_HANG_END_ENABLE, false);
		if (APhysXDynCharCtrler::APX_CCF_PASSIVE_EFFECT_ONLY & flags)
			SetFlag(APhysXDynCharCtrler::APX_CCF_PASSIVE_EFFECT_ONLY, true);
		else
			SetFlag(APhysXDynCharCtrler::APX_CCF_PASSIVE_EFFECT_ONLY, false);
		if (APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_OPEN & flags)
			SetFlag(APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_OPEN, true);
		else
			SetFlag(APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_OPEN, false);
		if (APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_CLOSE & flags)
			SetFlag(APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_CLOSE, true);
		else
			SetFlag(APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_CLOSE, false);
	}
	else
	{
		assert(!"Unknown CCType!");
	}
}

bool CAPhysXCCMgr::ReadFlag(const APhysXLWCharCtrler::APhysXLWCCFlag& flag) const
{
	bool bRtnValue = false;
	if (m_bIsRumtime)
	{
		if (CC_APHYSX_LWCC != m_ccRumtimeType)
			return false;

		bRtnValue = (m_ccRumtimeFlags & flag)? true : false;
		assert(bRtnValue == m_pAPhysXLWCC->ReadFlag(flag));
	}
	else
	{
		if (CC_APHYSX_LWCC != m_ccEditType)
			return false;

		bRtnValue = (m_ccEditFlags & flag)? true : false;
	}
	return bRtnValue;
}

bool CAPhysXCCMgr::ReadFlag(const APhysXDynCharCtrler::APhysXCCFlag& flag) const
{
	bool bRtnValue = false;
	if (m_bIsRumtime)
	{
		if (CC_APHYSX_DYNCC != m_ccRumtimeType)
			return false;

		bRtnValue = (m_ccRumtimeFlags & flag)? true : false;
		assert(bRtnValue == m_pAPhysXDynCC->ReadFlag(flag));
	}
	else
	{
		if (CC_APHYSX_DYNCC != m_ccEditType)
			return false;

		bRtnValue = (m_ccEditFlags & flag)? true : false;
	}
	return bRtnValue;
}

bool CAPhysXCCMgr::ReadFlag(const APhysXCharacterController::APhysXCCFlag& flag) const
{
	bool bRtnValue = false;
	if (m_bIsRumtime)
	{
		if (CC_APHYSX_EXTCC != m_ccRumtimeType)
			return false;

		bRtnValue = (m_ccRumtimeFlags & flag)? true : false;
		assert(bRtnValue == m_pAPhysXExtCC->ReadFlag(flag));
	}
	else
	{
		if (CC_APHYSX_EXTCC != m_ccEditType)
			return false;

		bRtnValue = (m_ccEditFlags & flag)? true : false;
	}
	return bRtnValue;
}

bool CAPhysXCCMgr::ReadFlagSlide() const
{
	switch (GetCCType())
	{
	case CC_TOTAL_EMPTY:
	case CC_APHYSX_LWCC:
	case CC_APHYSX_NXCC:
		return false;
	case CC_APHYSX_EXTCC:
		return ReadFlag(APhysXCharacterController::APX_CCF_SLIDE_ENABLE);
	case CC_APHYSX_DYNCC:
		return ReadFlag(APhysXDynCharCtrler::APX_CCF_SLIDE_ENABLE);
	case CC_BRUSH_CDRCC:
		return true;
	default:
		assert(!"Unknown CCType!");
	}
	return false;
}

bool CAPhysXCCMgr::ReadFlagAttachEnforceOpen() const
{
	switch (GetCCType())
	{
	case CC_TOTAL_EMPTY:
	case CC_APHYSX_LWCC:
	case CC_APHYSX_NXCC:
		return false;
	case CC_APHYSX_EXTCC:
		return ReadFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN); 
	case CC_APHYSX_DYNCC:
		return ReadFlag(APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_OPEN);
	case CC_BRUSH_CDRCC:
		return true;
	default:
		assert(!"Unknown CCType!");
	}
	return false;
}

bool CAPhysXCCMgr::ReadFlagAttachEnforceClose() const
{
	switch (GetCCType())
	{
	case CC_TOTAL_EMPTY:
	case CC_APHYSX_LWCC:
	case CC_APHYSX_NXCC:
		return false;
	case CC_APHYSX_EXTCC:
		return ReadFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE); 
	case CC_APHYSX_DYNCC:
		return ReadFlag(APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_CLOSE);
	case CC_BRUSH_CDRCC:
		return true;
	default:
		assert(!"Unknown CCType!");
	}
	return false;
}

void CAPhysXCCMgr::SetFlag(const APhysXLWCharCtrler::APhysXLWCCFlag& flag, bool enable)
{
	if (m_bIsRumtime)
	{
		if (CC_APHYSX_LWCC != m_ccRumtimeType)
			return;

		assert(0 != m_pAPhysXLWCC);
		if (0 != m_pAPhysXLWCC)
		{
			if (enable)
				m_pAPhysXLWCC->RaiseFlag(flag);
			else
				m_pAPhysXLWCC->ClearFlag(flag);
		}
		if (enable)
			m_ccRumtimeFlags |= flag;
		else
			m_ccRumtimeFlags &= ~flag;
	}
	else
	{
		if (CC_APHYSX_LWCC != m_ccEditType)
			return;

		if (enable)
			m_ccEditFlags |= flag;
		else
			m_ccEditFlags &= ~flag;
	}
}

void CAPhysXCCMgr::SetFlag(const APhysXDynCharCtrler::APhysXCCFlag& flag, bool enable)
{
	if (m_bIsRumtime)
	{
		if (CC_APHYSX_DYNCC != m_ccRumtimeType)
			return;

		assert(0 != m_pAPhysXDynCC);
		if (0 != m_pAPhysXDynCC)
		{
			if (enable)
				m_pAPhysXDynCC->RaiseFlag(flag);
			else
				m_pAPhysXDynCC->ClearFlag(flag);
		}
		if (enable)
			m_ccRumtimeFlags |= flag;
		else
			m_ccRumtimeFlags &= ~flag;
	}
	else
	{
		if (CC_APHYSX_DYNCC != m_ccEditType)
			return;

		if (enable)
			m_ccEditFlags |= flag;
		else
			m_ccEditFlags &= ~flag;
	}
}

void CAPhysXCCMgr::SetFlag(const APhysXCharacterController::APhysXCCFlag& flag, bool enable)
{
	if (m_bIsRumtime)
	{
		if (CC_APHYSX_EXTCC != m_ccRumtimeType)
			return;

		assert(0 != m_pAPhysXExtCC);
		if (0 != m_pAPhysXExtCC)
		{
			if (enable)
				m_pAPhysXExtCC->RaiseFlag(flag);
			else
				m_pAPhysXExtCC->ClearFlag(flag);
		}
		if (enable)
			m_ccRumtimeFlags |= flag;
		else
			m_ccRumtimeFlags &= ~flag;
	}
	else
	{
		if (CC_APHYSX_EXTCC != m_ccEditType)
			return;

		if (enable)
			m_ccEditFlags |= flag;
		else
			m_ccEditFlags &= ~flag;
	}
}

void CAPhysXCCMgr::SetFlagSlide(bool enable)
{
	switch (GetCCType())
	{
	case CC_TOTAL_EMPTY:
	case CC_APHYSX_LWCC:
	case CC_APHYSX_NXCC:
	case CC_BRUSH_CDRCC:
		break;
	case CC_APHYSX_EXTCC:
		SetFlag(APhysXCharacterController::APX_CCF_SLIDE_ENABLE, enable); 
	case CC_APHYSX_DYNCC:
		SetFlag(APhysXDynCharCtrler::APX_CCF_SLIDE_ENABLE, enable);
	default:
		assert(!"Unknown CCType!");
	}
	return;
}

void CAPhysXCCMgr::SetFlagAttachEnforceOpen(bool enable)
{
	switch (GetCCType())
	{
	case CC_TOTAL_EMPTY:
	case CC_APHYSX_LWCC:
	case CC_APHYSX_NXCC:
	case CC_BRUSH_CDRCC:
		break;
	case CC_APHYSX_EXTCC:
		SetFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN, enable); 
	case CC_APHYSX_DYNCC:
		SetFlag(APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_OPEN, enable);
		break;
	default:
		assert(!"Unknown CCType!");
	}
	return;
}

void CAPhysXCCMgr::SetFlagAttachEnforceClose(bool enable)
{
	switch (GetCCType())
	{
	case CC_TOTAL_EMPTY:
	case CC_APHYSX_LWCC:
	case CC_APHYSX_NXCC:
	case CC_BRUSH_CDRCC:
		break;
	case CC_APHYSX_EXTCC:
		SetFlag(APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE, enable); 
	case CC_APHYSX_DYNCC:
		SetFlag(APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_CLOSE, enable);
	default:
		assert(!"Unknown CCType!");
	}
	return;
}

void CAPhysXCCMgr::RegisterHostModels(IObjBase* pObj)
{
	if (0 == m_pAPhysXCCBase)
		return;
	if (0 == pObj)
		return;

	CPhysXObjDynamic* pHostDynObj = 0;
	int objTypes = ObjManager::OBJ_TYPEID_SKINMODEL | ObjManager::OBJ_TYPEID_ECMODEL;
	if (pObj->GetProperties()->GetObjType() & objTypes)
	{
		pHostDynObj = dynamic_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pHostDynObj)
		{
			bool bHasNewCnl = false;
			APhysXCollisionChannel oldCNL, newCNL;
			APhysXSkeletonRBObject* pPSRB = pHostDynObj->GetSkeletonRBObject();
			if (0 != pPSRB)
			{
				oldCNL = pPSRB->GetCollisionChannel();
				if (m_pAPhysXCCBase->RegisterHostModel(pPSRB, newCNL))
					bHasNewCnl = true;
			}

			if (bHasNewCnl)
			{
				if (oldCNL != newCNL)
				{
					assert(APX_COLLISION_CHANNEL_CC_COMMON_HOST == newCNL);
					// note here: EC model keep the old channel value because RegisterHostModel() update the channel value around ECM 
				}
			}
		}
	}

	IObjBase* pChild = 0;
	int nChildren = pObj->GetChildNum();
	for (int i = 0; i < nChildren; ++i)
	{
		pChild = dynamic_cast<IObjBase*>(pObj->GetChild(i));
		RegisterHostModels(pChild);
	}

	CECModel* pECM = pHostDynObj->GetECModel();
	if (0 != pECM)
	{
		int nChildren = pECM->GetChildCount();
		for (int i = 0; i < nChildren; ++i)
		{
			CPhysXObjECModel* pObj = ObjManager::GetInstance()->GetECModelObject(pECM->GetChildModel(i));
			if (0 != pObj)
			RegisterHostModels(pObj);
		}
	}
}

void CAPhysXCCMgr::UnRegisterHostModels(IObjBase* pObj)
{
	if (0 == m_pAPhysXCCBase)
		return;
	if (0 == pObj)
		return;

	CPhysXObjDynamic* pHostDynObj = 0;
	int objTypes = ObjManager::OBJ_TYPEID_SKINMODEL | ObjManager::OBJ_TYPEID_ECMODEL;
	if (pObj->GetProperties()->GetObjType() & objTypes)
	{
		pHostDynObj = dynamic_cast<CPhysXObjDynamic*>(pObj);
		if (0 != pHostDynObj)
		{
			APhysXSkeletonRBObject* pPSRB = pHostDynObj->GetSkeletonRBObject();
			if (0 != pPSRB)
			{
				APhysXCollisionChannel oldCNL, origCNL;
				oldCNL = pPSRB->GetCollisionChannel();
				if (m_pAPhysXCCBase->UnRegisterHostModel(pPSRB, origCNL))
				{
					if (oldCNL != origCNL)
					{
						// should the same with ECM value
						//if (APX_COLLISION_CHANNEL_COMMON == origCNL);
						//	pHostDynObj->SetCollisionChannel(CPhysXObjDynamic::CNL_COMMON);
					}
				}
			}
		}
	}

	IObjBase* pChild = 0;
	int nChildren = pObj->GetChildNum();
	for (int i = 0; i < nChildren; ++i)
	{
		pChild = dynamic_cast<IObjBase*>(pObj->GetChild(i));
		UnRegisterHostModels(pChild);
	}

	CECModel* pECM = pHostDynObj->GetECModel();
	if (0 != pECM)
	{
		int nChildren = pECM->GetChildCount();
		for (int i = 0; i < nChildren; ++i)
		{
			CPhysXObjECModel* pObj = ObjManager::GetInstance()->GetECModelObject(pECM->GetChildModel(i));
			if (0 != pObj)
				UnRegisterHostModels(pObj);
		}
	}
}

void CAPhysXCCMgr::GetRelevantFlagsFromCCToDynCC(int& inoutFlags)
{
	int theFlags = 0; 
	if (m_bIsRumtime)
	{
		if (CC_APHYSX_EXTCC != m_ccRumtimeType)
			return;
		theFlags = m_ccRumtimeFlags;
	}
	else
	{
		if (CC_APHYSX_EXTCC != m_ccEditType)
			return;
		theFlags = m_ccEditFlags;
	}

	if (theFlags & APhysXCharacterController::APX_CCF_SLIDE_ENABLE)
		inoutFlags |= APhysXDynCharCtrler::APX_CCF_SLIDE_ENABLE;
	else
		inoutFlags &= ~APhysXDynCharCtrler::APX_CCF_SLIDE_ENABLE;

	if (theFlags & APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN)
		inoutFlags |= APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_OPEN;
	else
		inoutFlags &= ~APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_OPEN;
	
	if (theFlags & APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE)
		inoutFlags |= APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_CLOSE;
	else
		inoutFlags &= ~APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_CLOSE;
}

void CAPhysXCCMgr::GetRelevantFlagsFromDynCCToCC(int& inoutFlags)
{
	int theFlags = 0; 
	if (m_bIsRumtime)
	{
		if (CC_APHYSX_DYNCC != m_ccRumtimeType)
			return;
		theFlags = m_ccRumtimeFlags;
	}
	else
	{
		if (CC_APHYSX_DYNCC != m_ccEditType)
			return;
		theFlags = m_ccEditFlags;
	}

	if (theFlags & APhysXDynCharCtrler::APX_CCF_SLIDE_ENABLE)
		inoutFlags |= APhysXCharacterController::APX_CCF_SLIDE_ENABLE;
	else
		inoutFlags &= ~APhysXCharacterController::APX_CCF_SLIDE_ENABLE;

	if (theFlags & APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_OPEN)
		inoutFlags |= APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN;
	else
		inoutFlags &= ~APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_OPEN;

	if (theFlags & APhysXDynCharCtrler::APX_CCF_ATTACHMENT_ENFORCE_CLOSE)
		inoutFlags |= APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE;
	else
		inoutFlags &= ~APhysXCharacterController::APX_CCF_ATTACHMENT_ENFORCE_CLOSE;
}

void CAPhysXCCMgr::GetCreateInfo(NxVec3& outFootPos, NxVec3& outVolume, VolumeType& outVT)
{
	if (0 != m_pAPhysXCCBase)
	{
		outFootPos = m_pAPhysXCCBase->GetFootPosition();
		outVolume  = m_pAPhysXCCBase->GetVolume(outVT);
		return;
	}

	if (0 != m_pBrushCDRCC)
	{
		outFootPos = APhysXConverter::A2N_Vector3(m_pBrushCDRCC->GetPos());
		outVolume.x = m_pBrushCDRCC->GetCapsule().GetRadius();
		outVolume.y = m_pBrushCDRCC->GetCapsule().GetHeight() + 2 * outVolume.x;
		outVolume.z = -1;
		outVT = APhysXCCBase::APX_VT_CAPSULE;
		return;
	}

	assert(0 != m_pHostDynObj);
	CDynProperty* pProp = m_pHostDynObj->GetProperties();
	outFootPos = APhysXConverter::A2N_Vector3(pProp->GetPos());
	outFootPos.y += m_pHostDynObj->GetYOffsetForRevive();
	outVolume = m_defVolume;
	outVolume.z = -1;
	outVT = APhysXCCBase::APX_VT_CAPSULE;
}

bool CAPhysXCCMgr::CheckVolumeValid(const NxVec3& posFoot, const NxVec3& volume, const APhysXCCBase::APhysXVolumeType type)
{
	CCType cctype = GetCCType();
	if (CC_UNKNOWN_TYPE == cctype)
		return true;
	if (CC_TOTAL_EMPTY == cctype)
		return true;

	if (CC_BRUSH_CDRCC  == cctype)
	{
		assert(APhysXCCBase::APX_VT_CAPSULE == type);
		A3DVECTOR3 posCenter = APhysXConverter::N2A_Vector3(posFoot);
		posCenter.y += volume.y * 0.5f;
		const float CyliderHeight = volume.y - 2 * volume.x;
		return BrushcharCtrler::IsPosAvailable(posCenter, volume.x, CyliderHeight);
	}
	else
	{
		if (0 == m_pAPhysXScene)
			return true;

		APhysXCollisionChannel cnlBK = 0;
		bool localBKFlag = PrepareHostModelCNL(cnlBK);
		bool bRtnValue = !APhysXCCBase::CheckOverlap(*m_pAPhysXScene, posFoot, volume, type);
		if (localBKFlag)
			RecoverHostModelCNL(cnlBK);
		return bRtnValue;
	}
	return false;
}

bool CAPhysXCCMgr::CheckCCCreatePosition(const CCType& toCreateType, const NxVec3& posFoot, const NxVec3& volume, const VolumeType type, NxVec3& outGoodPos)
{
	if (CC_UNKNOWN_TYPE == toCreateType)
		return true;
	if (CC_TOTAL_EMPTY == toCreateType)
		return true;

	const float yOffset = 0.03f;

	if (CC_BRUSH_CDRCC  == toCreateType)
	{
		assert(APhysXCCBase::APX_VT_CAPSULE == type);
		A3DVECTOR3 posCenter = APhysXConverter::N2A_Vector3(posFoot);
		posCenter.y += volume.y * 0.5f;
		const float CyliderHeight = volume.y - 2 * volume.x;
		if (BrushcharCtrler::IsPosAvailable(posCenter, volume.x, CyliderHeight))
		{
			outGoodPos = posFoot;
			return true;
		}

		posCenter.y += yOffset; 
		if (BrushcharCtrler::IsPosAvailable(posCenter, volume.x, CyliderHeight))
		{
			outGoodPos = posFoot;
			outGoodPos.y += yOffset;
			return true;
		}
	}
	else
	{
		bool bRtnValue = false;
		APhysXCollisionChannel cnlBK = 0;
		bool localBKFlag = PrepareHostModelCNL(cnlBK);

		assert(0 != m_pAPhysXScene);
		NxVec3 posTry(posFoot);
		if (!APhysXCCBase::CheckOverlap(*m_pAPhysXScene, posTry, volume, type))
			bRtnValue = true;
		if (!bRtnValue)
		{
			posTry.y += yOffset;
			if (!APhysXCCBase::CheckOverlap(*m_pAPhysXScene, posTry, volume, type))
				bRtnValue = true;
		}
		if (bRtnValue)
			outGoodPos = posTry;

		if (localBKFlag)
			RecoverHostModelCNL(cnlBK);
		return bRtnValue;
	}
	return false;
}

bool CAPhysXCCMgr::PrepareHostModelCNL(APhysXCollisionChannel& cnlBK)
{
	APhysXSkeletonRBObject* pPSRB = m_pHostDynObj->GetSkeletonRBObject();
	if (0 == pPSRB)
		return false;

	cnlBK = pPSRB->GetCollisionChannel();
	if (APX_COLLISION_CHANNEL_CC_COMMON_HOST == cnlBK)
		return false;

	pPSRB->SetCollisionChannel(APX_COLLISION_CHANNEL_CC_COMMON_HOST);
	return true;
}

void CAPhysXCCMgr::RecoverHostModelCNL(APhysXCollisionChannel cnlBK)
{
	APhysXSkeletonRBObject* pPSRB = m_pHostDynObj->GetSkeletonRBObject();
	if (0 != pPSRB)
		pPSRB->SetCollisionChannel(cnlBK);
}

bool CAPhysXCCMgr::ApplyCCType(CCType ccType, int ccFlags, bool bEnsureSuccess)
{
	NxVec3 posFoot(0.0f); 
	NxVec3 volume(0.0f);
	VolumeType vt;
	GetCreateInfo(posFoot, volume, vt);
	if (!CheckCCCreatePosition(ccType, posFoot, volume, vt, posFoot))
	{
		if (!bEnsureSuccess)
			return false;

		bool bIsOK = false; 
		while (bIsOK == false)
		{
			posFoot.y += 0.05f;
			bIsOK = CheckCCCreatePosition(ccType, posFoot, volume, vt, posFoot);
		}
	}

	ReleaseAPhysXLWCC();
	ReleaseAPhysXNxCC();
	ReleaseAPhysXExtCC();
	ReleaseAPhysXDynCC();
	ReleaseBrushCDRCC();
#ifdef _ANGELICA21
	s_IKInterface.HasPhysXCCNotify(false);
#endif

	switch (ccType)
	{
	case CC_TOTAL_EMPTY:
		break;
	case CC_APHYSX_LWCC:
		CreateAPhysXLWCC(posFoot, volume, vt, ccFlags);
		break;
	case CC_APHYSX_NXCC:
		CreateAPhysXNxCC(posFoot, volume, vt, ccFlags);
		break;
	case CC_APHYSX_EXTCC:
		CreateAPhysXExtCC(posFoot, volume, vt, ccFlags);
		break;
	case CC_APHYSX_DYNCC:
		CreateAPhysXDynCC(posFoot, volume, vt, ccFlags);
		break;
	case CC_BRUSH_CDRCC:
		CreateBrushCDRCC(posFoot, volume, vt, ccFlags);
		break;
	default:
		assert(!"Unknown CCType!");
	}

	m_ccRumtimeType = ccType;
	m_ccRumtimeFlags = ccFlags;
	if (CC_TOTAL_EMPTY != ccType)
	{
		if ((0 == m_pAPhysXCCBase) && (0 == m_pBrushCDRCC))
		{
			m_ccRumtimeType = CC_TOTAL_EMPTY;
			m_ccRumtimeFlags = 0;
			a_LogOutput(1, "Warning in %s: new CC can't be created!", __FUNCTION__);
		}

#ifdef _ANGELICA21
		if (0 != m_pAPhysXCCBase)
			s_IKInterface.HasPhysXCCNotify(true);
#endif
	}
	return true;
}

bool CAPhysXCCMgr::CreateAPhysXLWCC(const NxVec3& posFoot, const NxVec3& volume, const VolumeType& vt, int ccFlags)
{
	assert(0 != m_pHostDynObj);
	assert(0 != m_pAPhysXScene);
	assert(APhysXCCBase::APX_VT_CAPSULE == vt);
	CDynProperty* pProp = m_pHostDynObj->GetProperties();

	APhysXCharacterControllerManager* pCCM = m_pAPhysXScene->GetAPhysXEngine()->GetAPhysXCCManager();
	if (0 == pCCM)
		return false;
	if (gbIsUsingAsynchronousCC)
		pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);

	APhysXLWCCDesc desc;
	desc.flags  = ccFlags;
	desc.height = volume.y;
	desc.radius = volume.x;
	desc.posFoot = posFoot;

	m_pAPhysXCCBase = m_pAPhysXLWCC = pCCM->CreateLWCC(*m_pAPhysXScene, desc);
	if (0 == m_pAPhysXLWCC)
		return false;

	m_pAPhysXLWCC->userData = m_UserData;
	RegisterHostModels(m_pHostDynObj);
	return true;
}

bool CAPhysXCCMgr::CreateAPhysXNxCC(const NxVec3& posFoot, const NxVec3& volume, const VolumeType& vt, int ccFlags)
{
	assert(0 != m_pHostDynObj);
	assert(0 != m_pAPhysXScene);
	assert(APhysXCCBase::APX_VT_CAPSULE == vt);
	CDynProperty* pProp = m_pHostDynObj->GetProperties();

	APhysXCharacterControllerManager* pCCM = m_pAPhysXScene->GetAPhysXEngine()->GetAPhysXCCManager();
	if (0 == pCCM)
		return false;
	if (gbIsUsingAsynchronousCC)
		pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);

	APhysXCCDesc* pDesc = 0;
	APhysXCCCapsuleDesc descC;
	descC.height = volume.y;
	descC.radius = volume.x;
	pDesc = &descC;
	pDesc->stepOffset = 0.3f;
	pDesc->posFoot = posFoot;
	if (0 != m_pWalk)
		pDesc->speed = m_pWalk->GetSpeed();

	m_pAPhysXCCBase = m_pAPhysXHostCC = m_pAPhysXNxCC = pCCM->CreateNxCC(*m_pAPhysXScene, *pDesc);
	if (0 == m_pAPhysXNxCC)
		return false;

	m_pAPhysXNxCC->userData = m_UserData;
	m_pAPhysXNxCC->SetDir(APhysXConverter::A2N_Vector3(pProp->GetDir()));
	RegisterHostModels(m_pHostDynObj);
	return true;
}

bool CAPhysXCCMgr::CreateAPhysXExtCC(const NxVec3& posFoot, const NxVec3& volume, const VolumeType& vt, int ccFlags)
{
	assert(0 != m_pHostDynObj);
	assert(0 != m_pAPhysXScene);
	assert(APhysXCCBase::APX_VT_CAPSULE == vt);
	CDynProperty* pProp = m_pHostDynObj->GetProperties();

	APhysXCharacterControllerManager* pCCM = m_pAPhysXScene->GetAPhysXEngine()->GetAPhysXCCManager();
	if (0 == pCCM)
		return false;
	if (gbIsUsingAsynchronousCC)
		pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);

	APhysXCCDesc* pDesc = 0;
	APhysXCCCapsuleDesc descC;
	descC.flags  = ccFlags;
	descC.height = volume.y;
	descC.radius = volume.x;
	pDesc = &descC;
	pDesc->stepOffset = 0.3f;
	pDesc->posFoot = posFoot;
	if (0 != m_pWalk)
		pDesc->speed = m_pWalk->GetSpeed();

	bool SettingForDebugXAJH = false;
#ifdef _ANGELICA3
	SettingForDebugXAJH = false;
#endif
	if (SettingForDebugXAJH)
	{
		descC.height = 1.8f;
		descC.radius = 0.5f;
		descC.speed = 3.5;
		pDesc->mass = 3;//5;
		pDesc->flags |= APhysXCharacterController::APX_CCF_ATTACHMENT_ONLY_VERTICAL;
		pDesc->flags |= APhysXCharacterController::APX_CCF_CONTROL_JUMPINGDIR_ENABLE;
		pDesc->flags &= ~APhysXCharacterController::APX_CCF_GRAVITY_READSCENE;
	}


	m_pAPhysXCCBase = m_pAPhysXHostCC = m_pAPhysXExtCC = pCCM->CreateCC(*m_pAPhysXScene, *pDesc);
	if (0 == m_pAPhysXExtCC)
		return false;

	m_pAPhysXExtCC->userData = m_UserData;
	m_pAPhysXExtCC->SetDir(APhysXConverter::A2N_Vector3(pProp->GetDir()));
	m_pAPhysXExtCC->SetStepOffset(0.5f);
	m_pAPhysXExtCC->SetAttachingPTLinearVelThreshold(0.2f);
	m_pAPhysXExtCC->SetUserGravity(NxVec3(0, -9.8f, 0));
//	m_pAPhysXExtCC->SetAttachingPTLinearVelThreshold(0.2f * m_pAPhysXExtCC->GetMass() / 1.2f);
	RegisterHostModels(m_pHostDynObj);
	return true;
}

bool CAPhysXCCMgr::CreateAPhysXDynCC(const NxVec3& posFoot, const NxVec3& volume, const VolumeType& vt, int ccFlags)
{
	assert(0 != m_pHostDynObj);
	assert(0 != m_pAPhysXScene);
	assert(APhysXCCBase::APX_VT_CAPSULE == vt);
	CDynProperty* pProp = m_pHostDynObj->GetProperties();

	APhysXCharacterControllerManager* pCCM = m_pAPhysXScene->GetAPhysXEngine()->GetAPhysXCCManager();
	if (0 == pCCM)
		return false;
	if (gbIsUsingAsynchronousCC)
		pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);

	APhysXCCCapsuleDesc desc;
	desc.SetDefaultFlags(true);
	desc.flags  = ccFlags;
	desc.height = volume.y;
	desc.radius = volume.x;
	desc.stepOffset = 0.3f;
	desc.posFoot = posFoot;
	if (0 != m_pWalk)
		desc.speed = m_pWalk->GetSpeed();

	m_pAPhysXCCBase = m_pAPhysXHostCC = m_pAPhysXDynCC = pCCM->CreateDynCC(*m_pAPhysXScene, desc);
	if (0 == m_pAPhysXDynCC)
		return false;

	m_pAPhysXDynCC->userData = m_UserData;
	m_pAPhysXDynCC->SetDir(APhysXConverter::A2N_Vector3(pProp->GetDir()));
	RegisterHostModels(m_pHostDynObj);
	return true;
}

bool CAPhysXCCMgr::CreateBrushCDRCC(const NxVec3& posFoot, const NxVec3& volume, const VolumeType& vt, int ccFlags)
{
	assert(APhysXCCBase::APX_VT_CAPSULE == vt);
	const float radius = volume.x;
	const float height = volume.y - 2 * volume.x;
	A3DVECTOR3 pos = APhysXConverter::N2A_Vector3(posFoot);
	pos.y += volume.y * 0.5f;

	BrushcharCtrlerDes desc;
	desc.fCapsuleRadius = radius;
	desc.fCylinerHei = height;
	if (0 != m_pWalk)
		desc.fSpeed = m_pWalk->GetSpeed();

	m_pBrushCDRCC = new BrushcharCtrler(desc);
	if (0 == m_pBrushCDRCC)
		return false;

	m_pBrushCDRCC->SetPos(pos);
	CDynProperty* pProp = m_pHostDynObj->GetProperties();
	m_pBrushCDRCC->SetDir(pProp->GetDir());
	return true;
}

void CAPhysXCCMgr::ReleaseAPhysXLWCC()
{
	if (0 == m_pAPhysXLWCC)
		return;

	UnRegisterHostModels(m_pHostDynObj);
	APhysXCharacterControllerManager& ccm = m_pAPhysXLWCC->GetCCManager();
	if (ccm.userData == m_pAPhysXLWCC)
		ccm.userData = 0;
	ccm.ReleaseLWCC(m_pAPhysXLWCC);
	if (m_pAPhysXCCBase == m_pAPhysXLWCC)
		m_pAPhysXCCBase = 0;
	m_pAPhysXLWCC = 0;
}

void CAPhysXCCMgr::ReleaseAPhysXNxCC()
{
	if (0 == m_pAPhysXNxCC)
		return;

	UnRegisterHostModels(m_pHostDynObj);
	APhysXCharacterControllerManager& ccm = m_pAPhysXNxCC->GetCCManager();
	if (ccm.userData == m_pAPhysXNxCC)
		ccm.userData = 0;
	ccm.ReleaseNxCC(m_pAPhysXNxCC);
	if (m_pAPhysXCCBase == m_pAPhysXNxCC)
		m_pAPhysXCCBase = 0;
	if (m_pAPhysXHostCC == m_pAPhysXExtCC)
		m_pAPhysXHostCC = 0;
	m_pAPhysXNxCC = 0;
}

void CAPhysXCCMgr::ReleaseAPhysXExtCC()
{
	if (0 == m_pAPhysXExtCC)
		return;

	UnRegisterHostModels(m_pHostDynObj);
	APhysXCharacterControllerManager& ccm = m_pAPhysXExtCC->GetCCManager();
	if (ccm.userData == m_pAPhysXExtCC)
		ccm.userData = 0;
	ccm.ReleaseCC(m_pAPhysXExtCC);
	if (m_pAPhysXCCBase == m_pAPhysXExtCC)
		m_pAPhysXCCBase = 0;
	if (m_pAPhysXHostCC == m_pAPhysXExtCC)
		m_pAPhysXHostCC = 0;
	m_pAPhysXExtCC = 0;

#ifdef _ANGELICA21
	s_IKInterface.HasPhysXCCNotify(false);
#endif
}

void CAPhysXCCMgr::ReleaseAPhysXDynCC()
{
	if (0 == m_pAPhysXDynCC)
		return;
	
	UnRegisterHostModels(m_pHostDynObj);
	APhysXCharacterControllerManager& ccm = m_pAPhysXDynCC->GetCCManager();
	if (ccm.userData == m_pAPhysXDynCC)
		ccm.userData = 0;
	ccm.ReleaseDynCC(m_pAPhysXDynCC);
	if (m_pAPhysXCCBase == m_pAPhysXDynCC)
		m_pAPhysXCCBase = 0;
	if (m_pAPhysXHostCC == m_pAPhysXDynCC)
		m_pAPhysXHostCC = 0;
	m_pAPhysXDynCC = 0;
}

void CAPhysXCCMgr::ReleaseBrushCDRCC()
{
	delete m_pBrushCDRCC;
	m_pBrushCDRCC = 0;
}

void CAPhysXCCMgr::SyncNonDynCCPoseToModel() const
{
	if (newStyleTest)
		return;

	static const A3DVECTOR3 g_vUp(0, 1, 0);

	assert(0 != m_pHostDynObj);
	CDynProperty* pProp = m_pHostDynObj->GetProperties();

	if (0 != m_pAPhysXHostCC)
	{
		if (0 == m_pAPhysXDynCC)
		{
			assert(0 != m_pWalk);
			A3DVECTOR3 pos = APhysXConverter::N2A_Vector3(m_pAPhysXHostCC->GetFootPosition());
			A3DVECTOR3 dir = APhysXConverter::N2A_Vector3(m_pAPhysXHostCC->GetHorizonDir());
			m_pWalk->ReceivePosFromCC(pos);
			m_pWalk->ReceiveFaceDirFromCC(dir, g_vUp);
		}
	}

	if (0 != m_pAPhysXLWCC)
	{
		assert(0 != m_pWalk);
		A3DVECTOR3 pos = APhysXConverter::N2A_Vector3(m_pAPhysXLWCC->GetFootPosition());
		m_pWalk->ReceivePosFromCC(pos);
	}

	if (0 != m_pBrushCDRCC)
	{
		A3DVECTOR3 pos = m_pBrushCDRCC->GetFootPosition();
		m_pWalk->ReceivePosFromCC(pos);
		A3DVECTOR3 dir = m_pBrushCDRCC->GetDir();
		dir.y = 0;
		dir.Normalize();
		if (!dir.IsZero())
			m_pWalk->ReceiveFaceDirFromCC(dir, g_vUp);
	}
}

void CAPhysXCCMgr::SyncDynCCPoseToModel() const
{
	if (newStyleTest)
		return;

	if (0 == m_pAPhysXDynCC)
		return;

	m_pAPhysXDynCC->RefeshCCBySimResult();

	assert(0 != m_pHostDynObj);
	CDynProperty* pProp = m_pHostDynObj->GetProperties();
	A3DVECTOR3 up = pProp->GetUp();
	A3DVECTOR3 pos = APhysXConverter::N2A_Vector3(m_pAPhysXDynCC->GetFootPosition());
	A3DVECTOR3 dir = APhysXConverter::N2A_Vector3(m_pAPhysXDynCC->GetHorizonDir());

	if (APhysXDynCharCtrler::APX_CCS_HANGING & m_pAPhysXDynCC->GetStatus())
	{
		dir = APhysXConverter::N2A_Vector3(m_pAPhysXDynCC->GetDir());
		up = APhysXConverter::N2A_Vector3(m_pAPhysXDynCC->GetUpDir());
	}
	assert(0 != m_pWalk);
	m_pWalk->ReceivePosFromCC(pos);
	m_pWalk->ReceiveFaceDirFromCC(dir, up);
}

bool CAPhysXCCMgr::DoMoveCC(float deltaTimeSec)
{
	if (newStyleTest)
		return false;

	if (0 != m_pAPhysXLWCC)
	{
		if (0 != m_pWalk)
			m_pAPhysXLWCC->SetFootPosition(APhysXConverter::A2N_Vector3(m_pWalk->GetCurPos()));
		return false;
	}

	if (0 != m_pAPhysXHostCC)
	{
		assert(0 != m_pWalk);
		if (!IsHangMode())
		{
			A3DVECTOR3 dir;
			if (m_pWalk->GetNewFaceDir(dir))
				m_pAPhysXHostCC->SetDir(APhysXConverter::A2N_Vector3(dir));
		}
		NxVec3 moveDir(m_pWalk->GetTargetDirNx());
		m_pAPhysXHostCC->MoveCC(deltaTimeSec, moveDir);
		return true;
	}

	if (0 != m_pBrushCDRCC)
	{
		assert(0 != m_pWalk);
		A3DVECTOR3 dir;
		if (m_pWalk->GetNewFaceDir(dir))
			m_pBrushCDRCC->SetDir(dir);

		A3DVECTOR3 moveDir(m_pWalk->GetTargetDir()); 
		m_pBrushCDRCC->MoveCC(deltaTimeSec, moveDir);
		return true;
	}
	return false;
}

void CAPhysXCCMgr::PreMoveCC(float deltaTimeSec, const NxVec3* pFootPos)
{
	if (0 != m_pAPhysXLWCC)
	{
		assert(0 != pFootPos);
		m_pAPhysXLWCC->SetFootPosition(*pFootPos);
		return;
	}
	if (0 != m_pBrushCDRCC)
	{
		m_pBrushCDRCC->UpdateDynDes(); 
		return;
	}
}

bool CAPhysXCCMgr::MoveCC(float deltaTimeSec, const NxVec3& moveDir)
{
	bool bRtn = true;
	if (0 != m_pAPhysXHostCC)
		m_pAPhysXHostCC->MoveCC(deltaTimeSec, moveDir);
	else if (0 != m_pBrushCDRCC)
		m_pBrushCDRCC->MoveCC(deltaTimeSec, APhysXConverter::N2A_Vector3(moveDir));
	else if (0 != m_pAPhysXLWCC)
	{
		if (APX_CCFM_SYNCHRONOUS == m_pAPhysXLWCC->GetCCManager().GetFlushMode())
			m_pAPhysXLWCC->TickReviseVerticalPos();
	}
	else
		bRtn = false;
	return bRtn;
}

bool CAPhysXCCMgr::SyncNonDynCCPose(NxVec3& footPos, NxVec3& faceDir, NxVec3& up) const
{
	if (0 != m_pAPhysXHostCC)
	{
		if (0 != m_pAPhysXDynCC)
			return false;

		footPos = m_pAPhysXHostCC->GetFootPosition();
		faceDir = m_pAPhysXHostCC->GetHorizonDir();
		up = m_pAPhysXHostCC->GetUpDir();
		return true;
	}

	if (0 != m_pAPhysXLWCC)
	{
		footPos = m_pAPhysXLWCC->GetFootPosition();
		return true;
	}

	if (0 != m_pBrushCDRCC)
	{
		footPos = APhysXConverter::A2N_Vector3(m_pBrushCDRCC->GetFootPosition());
		faceDir = APhysXConverter::A2N_Vector3(m_pBrushCDRCC->GetDir());
		return true; 
	}
	return false;
}

bool CAPhysXCCMgr::SyncDynCCPose(NxVec3& footPos, NxVec3& faceDir, NxVec3& up) const
{
	if (0 != m_pAPhysXHostCC)
	{
		if (0 != m_pAPhysXDynCC)
		{
			m_pAPhysXDynCC->RefeshCCBySimResult();

			footPos = m_pAPhysXDynCC->GetFootPosition();
			faceDir = m_pAPhysXHostCC->GetHorizonDir();
			up = m_pAPhysXHostCC->GetUpDir();
			if (APhysXCCBase::APX_CCS_HANGING & m_pAPhysXDynCC->GetStatus())
				faceDir = m_pAPhysXDynCC->GetDir();
			return true;
		}
	}
	return false;
}

