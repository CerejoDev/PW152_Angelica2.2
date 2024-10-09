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

/*
void CCHitReport::onOverlapBegin(const APhysXCCOverlap& ol)
{
//	OutputDebugString("---onOverlapBegin---\n");
	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(ol.controller->userData);
	if (0 != pDynObj)
	{
		A3DModelPhysics* p3DMP = pDynObj->GetA3DModelPhysics();
		if (0 != p3DMP)
		{
			int nSS = p3DMP->GetSkinSlotNum();
			for (int i = 0; i < nSS; ++i)
			{
				A3DSkinPhysSync* p3DSPS = p3DMP->GetSkinPhysSync(i);
				if (0 != p3DSPS)
				{
					APhysXClothAttacher* pCA = p3DSPS->GetClothAttacher();
					if (0 != pCA)
						pCA->EnableCollision(false);
				}
			}
		}
	}
	*/
/*	for (APhysXU32 i = 0; i < nbEntities; ++i)
	{
		A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)entities[i]->mUserData;
		A3DModelPhysics* p3DMP = pSkinModelPhysSync->GetA3DModelPhysics();
		p3DMP->SetModelCollisionChannel(CECModel::COLLISION_CHANNEL_INDEPENDENT, 0);
		m_APhysXObjs.push_back(entities[i]);
	}
*/
/*}

void CCHitReport::onOverlapContinue(const APhysXCCOverlap& ol)
{
//	OutputDebugString("---onOverlapContinue---\n");
}

void CCHitReport::onOverlapEnd(APhysXCharacterController& APhysXCC)
{
//	OutputDebugString("---onOverlapEnd---\n");
	CPhysXObjDynamic* pDynObj = static_cast<CPhysXObjDynamic*>(APhysXCC.userData);
	if (0 != pDynObj)
	{
		A3DModelPhysics* p3DMP = pDynObj->GetA3DModelPhysics();
		if (0 != p3DMP)
		{
			int nSS = p3DMP->GetSkinSlotNum();
			for (int i = 0; i < nSS; ++i)
			{
				A3DSkinPhysSync* p3DSPS = p3DMP->GetSkinPhysSync(i);
				if (0 != p3DSPS)
				{
					APhysXClothAttacher* pCA = p3DSPS->GetClothAttacher();
					if (0 != pCA)
						pCA->EnableCollision(true);
				}
			}
		}
	}
*/
/*	NxU32 nCount = m_APhysXObjs.size(); 
	for (NxU32 i = 0; i < nCount; ++i)
	{
		A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)m_APhysXObjs[i]->mUserData;
		A3DModelPhysics* p3DMP = pSkinModelPhysSync->GetA3DModelPhysics();
		p3DMP->SetModelCollisionChannel(CECModel::COLLISION_CHANNEL_AS_COMMON, 0);
	}
	m_APhysXObjs.clear();
*/
//}

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

	m_bIsRumtime = false;
	m_ccEditType = CC_TOTAL_EMPTY;
	m_ccEditFlags = 0;
	m_ccRumtimeType = CC_TOTAL_EMPTY;
	m_ccRumtimeFlags = 0;

	m_bHasHostObjBKP = false;
	m_hostObjCNLBackup = -1;
	m_hostObjOneWayBKP = -1;
	m_hostObjCGroupBKP = -1;
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
	assert(false == m_bHasHostObjBKP);
	m_ccRumtimeType = CC_TOTAL_EMPTY;
	m_ccRumtimeFlags = 0;
}

bool CAPhysXCCMgr::SwtichCC(const CCType& toType)
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

	return ApplyCCType(toType, newCCFlags);
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

bool CAPhysXCCMgr::IsHangMode() const
{
	if (0 == m_pAPhysXDynCC)
		return false;

	int IsHang = APhysXDynCharCtrler::APX_CCS_HANGING & m_pAPhysXDynCC->GetStatus();
	return (0 != IsHang);
}

bool CAPhysXCCMgr::IsInAir() const
{
	if (0 != m_pAPhysXHostCC)
	{
		int rtn = APhysXCCBase::APX_CCS_JUMPING_INAIR & m_pAPhysXHostCC->GetStatus();
		return 0 != rtn;
	}

	if (0 != m_pAPhysXLWCC)
		return m_pAPhysXLWCC->ReadFlag(APhysXLWCharCtrler::APX_LWCCF_JUMPING_INAIR);

	if (0 != m_pBrushCDRCC)
		return m_pBrushCDRCC->IsInAir();

	return false;
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

bool CAPhysXCCMgr::SetHostModelChannel(const APhysXCollisionChannel channel) const
{
	if (0 == m_pAPhysXCCBase)
		return false;

	m_pAPhysXCCBase->SetHostModelChannel(channel);
	return true;
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
	outVolume.x = 0.3f;
	outVolume.y = 1.6f;
	outVolume.z = -1;
	outVT = APhysXCCBase::APX_VT_CAPSULE;
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
		if (!m_bHasHostObjBKP)
			PrepareHostModelSetting();

		assert(0 != m_pAPhysXScene);
		NxVec3 posTry(posFoot);
		if (!APhysXCCBase::CheckOverlap(*m_pAPhysXScene, posTry, volume, APhysXCCBase::APX_VT_CAPSULE))
			bRtnValue = true;
		if (!bRtnValue)
		{
			posTry.y += yOffset;
			if (!APhysXCCBase::CheckOverlap(*m_pAPhysXScene, posTry, volume, APhysXCCBase::APX_VT_CAPSULE))
				bRtnValue = true;
		}
		if (bRtnValue)
			outGoodPos = posTry;

		if (m_bHasHostObjBKP)
			RecoverHostModelSetting();
		return bRtnValue;
	}
	return false;
}

APhysXCollisionChannel CAPhysXCCMgr::PrepareHostModelSetting()
{
	assert(0 != m_pHostDynObj);
	if (m_bHasHostObjBKP)
	{
		APhysXCollisionChannel rtnCNL = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
		m_pHostDynObj->GetCollisionChannel(rtnCNL);
		return rtnCNL;
	}

	m_bHasHostObjBKP = true;
	m_hostObjCNLBackup = -1;
	m_hostObjOneWayBKP = -1;
	m_hostObjCGroupBKP = -1;

	// make sure the one-way setting is correct
	APhysXSkeletonRBObject* pSRB = m_pHostDynObj->GetSkeletonRBObject();
	if(0 != pSRB)
	{
		bool bIsOneWay = pSRB->GetObjectFlag(APX_OBJFLAG_ONEWAY_COLLISION);
		if (!bIsOneWay)
		{
			m_hostObjOneWayBKP = 0;
			pSRB->SetObjectFlag(APX_OBJFLAG_ONEWAY_COLLISION, true);
		}
	}

	// make sure the group is correct
	if(0 != pSRB)
	{
		APhysXCollisionGroup oldGroup = pSRB->GetCollisionGroup();
		if (APX_COLLISION_GROUP_CC_HOST_RB != oldGroup)
		{
			m_hostObjCGroupBKP = oldGroup;
			pSRB->SetCollisionGroup(APX_COLLISION_GROUP_CC_HOST_RB);
		}
	}

	// make sure the channel is correct
	APhysXCollisionChannel rtnCNL = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
	if (m_pHostDynObj->GetCollisionChannel(rtnCNL))
	{
		if ((rtnCNL > APX_COLLISION_CHANNEL_INVALID) && (rtnCNL < APX_COLLISION_CHANNEL_CC_COMMON_HOST))
		{
			m_hostObjCNLBackup = rtnCNL;
			rtnCNL = APX_COLLISION_CHANNEL_CC_COMMON_HOST;
			m_pHostDynObj->SetCollisionChannel(CPhysXObjDynamic::CNL_INDEPENDENT_CCHOST);
		}
	}
	return rtnCNL;
}

void CAPhysXCCMgr::RecoverHostModelSetting()
{
	assert(0 != m_pHostDynObj);
	if (!m_bHasHostObjBKP)
		return;

	APhysXSkeletonRBObject* pSRB = m_pHostDynObj->GetSkeletonRBObject();
	// recover one-way setting if it's necessary
	if (-1 != m_hostObjOneWayBKP)
	{
		if(0 != pSRB)
			pSRB->SetObjectFlag(APX_OBJFLAG_ONEWAY_COLLISION, false);
		m_hostObjOneWayBKP = -1;
	}

	// recover the group setting if it's necessary
	if (-1 != m_hostObjCGroupBKP)
	{
		if(0 != pSRB)
			pSRB->SetCollisionGroup(APhysXCollisionGroup(m_hostObjCGroupBKP));
		m_hostObjCGroupBKP = -1;
	}

	// recover the channel setting if it's necessary
	if (-1 != m_hostObjCNLBackup)
	{
		if (APX_COLLISION_CHANNEL_COMMON == m_hostObjCNLBackup)
		{
			m_pHostDynObj->SetCollisionChannel(CPhysXObjDynamic::CNL_COMMON);
		}
		else
		{
			assert(!"Please check it!");
			// currently, it is impossible to set this channel by GfxCommon interface.
			// So, we have to visit the APhysX stuff directly.  
			APhysXSkeletonRBObject* pPSRB = m_pHostDynObj->GetSkeletonRBObject();
			if (0 != pPSRB)
				pPSRB->SetCollisionChannel(APX_COLLISION_CHANNEL_COLLIDE_ALL);
			A3DModelPhysics* p3DMP = m_pHostDynObj->GetA3DModelPhysics();
			if (0 != p3DMP)
			{
				int nCount = p3DMP->GetSkinSlotNum();
				for (int i = 0; i < nCount; ++i)
				{
					A3DSkinPhysSync* pPhysSkin = p3DMP->GetSkinSlot(i)->pPhysSync;
					if ((0 != pPhysSkin) && pPhysSkin->IsSkinBound())
						pPhysSkin->GetClothAttacher()->SetCollisionChannel(APX_COLLISION_CHANNEL_COLLIDE_ALL);
				}
			}
		}
		m_hostObjCNLBackup = -1;
	}

	m_bHasHostObjBKP = false;
}

bool CAPhysXCCMgr::ApplyCCType(CCType ccType, int ccFlags)
{
	NxVec3 posFoot(0.0f); 
	NxVec3 volume(0.0f);
	VolumeType vt;
	GetCreateInfo(posFoot, volume, vt);
	if (!CheckCCCreatePosition(ccType, posFoot, volume, vt, posFoot))
		return false;

	ReleaseAPhysXLWCC();
	ReleaseAPhysXNxCC();
	ReleaseAPhysXExtCC();
	ReleaseAPhysXDynCC();
	ReleaseBrushCDRCC();
	assert(false == m_bHasHostObjBKP);

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
	pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);

	APhysXLWCCDesc desc;
	desc.flags  = ccFlags;
	desc.height = volume.y;
	desc.radius = volume.x;
	desc.hostModelHasPhysX = pProp->GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY);
	desc.posFoot = posFoot;
	desc.hostModelChannel = PrepareHostModelSetting();

	m_pAPhysXCCBase = m_pAPhysXLWCC = pCCM->CreateLWCC(*m_pAPhysXScene, desc);
	if (0 == m_pAPhysXLWCC)
	{
		RecoverHostModelSetting();
		return false;
	}

	m_pAPhysXLWCC->userData = m_pHostDynObj;
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
	pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);

	APhysXCCDesc* pDesc = 0;
	APhysXCCCapsuleDesc descC;
	descC.height = volume.y;
	descC.radius = volume.x;
	pDesc = &descC;
//	pDesc->callback = &m_CCHitReceiver;
	pDesc->stepOffset = 0.3f;
	pDesc->hostModelHasPhysX = pProp->GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY);
	pDesc->posFoot = posFoot;
	pDesc->hostModelChannel = PrepareHostModelSetting();
	if (0 != m_pWalk)
		pDesc->speed = m_pWalk->GetSpeed();

	m_pAPhysXCCBase = m_pAPhysXHostCC = m_pAPhysXNxCC = pCCM->CreateNxCC(*m_pAPhysXScene, *pDesc);
	if (0 == m_pAPhysXNxCC)
	{
		RecoverHostModelSetting();
		return false;
	}

	m_pAPhysXNxCC->userData = m_pHostDynObj;
	m_pAPhysXNxCC->SetDir(APhysXConverter::A2N_Vector3(pProp->GetDir()));
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
	pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);

	APhysXCCDesc* pDesc = 0;
	APhysXCCCapsuleDesc descC;
	descC.flags  = ccFlags;
	descC.height = volume.y;
	descC.radius = volume.x;
	pDesc = &descC;
//	pDesc->callback = &m_CCHitReceiver;
	pDesc->stepOffset = 0.3f;
	pDesc->hostModelHasPhysX = pProp->GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY);
	pDesc->posFoot = posFoot;
	pDesc->hostModelChannel = PrepareHostModelSetting();
	if (0 != m_pWalk)
		pDesc->speed = m_pWalk->GetSpeed();

	m_pAPhysXCCBase = m_pAPhysXHostCC = m_pAPhysXExtCC = pCCM->CreateCC(*m_pAPhysXScene, *pDesc);
	if (0 == m_pAPhysXExtCC)
	{
		RecoverHostModelSetting();
		return false;
	}

	m_pAPhysXExtCC->userData = m_pHostDynObj;
	m_pAPhysXExtCC->SetDir(APhysXConverter::A2N_Vector3(pProp->GetDir()));
	m_pAPhysXExtCC->SetPositionForecastThreshold(0.2f * m_pAPhysXExtCC->GetMass() / 1.2f);
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
	pCCM->SetFlushMode(APX_CCFM_ASYNCHRONOUS);

	APhysXCCCapsuleDesc desc;
	desc.SetDefaultFlags(true);
	desc.flags  = ccFlags;
	desc.height = volume.y;
	desc.radius = volume.x;
	desc.stepOffset = 0.3f;
	desc.hostModelHasPhysX = pProp->GetFlags().ReadFlag(OBF_ENABLE_PHYSX_BODY);
	desc.posFoot = posFoot;
	desc.hostModelChannel = PrepareHostModelSetting();
	if (0 != m_pWalk)
		desc.speed = m_pWalk->GetSpeed();

	m_pAPhysXCCBase = m_pAPhysXHostCC = m_pAPhysXDynCC = pCCM->CreateDynCC(*m_pAPhysXScene, desc);
	if (0 == m_pAPhysXDynCC)
	{
		RecoverHostModelSetting();
		return false;
	}

	m_pAPhysXDynCC->userData = m_pHostDynObj;
	m_pAPhysXDynCC->SetDir(APhysXConverter::A2N_Vector3(pProp->GetDir()));
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
	return true;
}

void CAPhysXCCMgr::ReleaseAPhysXLWCC()
{
	if (0 == m_pAPhysXLWCC)
		return;

	APhysXCharacterControllerManager& ccm = m_pAPhysXLWCC->GetCCManager();
	if (ccm.userData == m_pAPhysXLWCC)
		ccm.userData = 0;
	ccm.ReleaseLWCC(m_pAPhysXLWCC);
	if (m_pAPhysXCCBase == m_pAPhysXLWCC)
		m_pAPhysXCCBase = 0;
	m_pAPhysXLWCC = 0;

	RecoverHostModelSetting();
}

void CAPhysXCCMgr::ReleaseAPhysXNxCC()
{
	if (0 == m_pAPhysXNxCC)
		return;

	APhysXCharacterControllerManager& ccm = m_pAPhysXNxCC->GetCCManager();
	if (ccm.userData == m_pAPhysXNxCC)
		ccm.userData = 0;
	ccm.ReleaseNxCC(m_pAPhysXNxCC);
	if (m_pAPhysXCCBase == m_pAPhysXNxCC)
		m_pAPhysXCCBase = 0;
	if (m_pAPhysXHostCC == m_pAPhysXExtCC)
		m_pAPhysXHostCC = 0;
	m_pAPhysXNxCC = 0;

	RecoverHostModelSetting();
}

void CAPhysXCCMgr::ReleaseAPhysXExtCC()
{
	if (0 == m_pAPhysXExtCC)
		return;

	APhysXCharacterControllerManager& ccm = m_pAPhysXExtCC->GetCCManager();
	if (ccm.userData == m_pAPhysXExtCC)
		ccm.userData = 0;
	ccm.ReleaseCC(m_pAPhysXExtCC);
	if (m_pAPhysXCCBase == m_pAPhysXExtCC)
		m_pAPhysXCCBase = 0;
	if (m_pAPhysXHostCC == m_pAPhysXExtCC)
		m_pAPhysXHostCC = 0;
	m_pAPhysXExtCC = 0;

	RecoverHostModelSetting();
}

void CAPhysXCCMgr::ReleaseAPhysXDynCC()
{
	if (0 == m_pAPhysXDynCC)
		return;
	
	APhysXCharacterControllerManager& ccm = m_pAPhysXDynCC->GetCCManager();
	if (ccm.userData == m_pAPhysXDynCC)
		ccm.userData = 0;
	ccm.ReleaseDynCC(m_pAPhysXDynCC);
	if (m_pAPhysXCCBase == m_pAPhysXDynCC)
		m_pAPhysXCCBase = 0;
	if (m_pAPhysXHostCC == m_pAPhysXDynCC)
		m_pAPhysXHostCC = 0;
	m_pAPhysXDynCC = 0;

	RecoverHostModelSetting();
}

void CAPhysXCCMgr::ReleaseBrushCDRCC()
{
	delete m_pBrushCDRCC;
	m_pBrushCDRCC = 0;
}

void CAPhysXCCMgr::SyncNonDynCCPoseToModel() const
{
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
