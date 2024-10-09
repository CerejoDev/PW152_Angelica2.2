/*
 * FILE: MainActor.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WenFeng He, 2011/03/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include "BrushcharCtrler.h"

void IRoleActor::Render(A3DWireCollector& wc, bool bIsDebugRender)
{
	A3DMATRIX4 rot;
	A3DCYLINDER cylinder;
	cylinder.vCenter = m_objModel.GetHeadTopPos(&rot);
	cylinder.vAxisX = rot.GetRow(0);
	cylinder.vAxisY = rot.GetRow(1);
	cylinder.vAxisZ = rot.GetRow(2);
	cylinder.fHalfLen = 0.05f;
	cylinder.fRadius = 0.3f;

	CDynProperty* pProp = m_objModel.GetProperties();
	const CAPhysXCCMgr& ccMgr = pProp->GetCCMgr();
	APhysXCCBase* pCC = ccMgr.GetAPhysXCCBase();
	if (0 != pCC)
	{
		APhysXCCBase::APhysXVolumeType vt;
		const NxVec3 vol = pCC->GetVolume(vt);
		float yOffset = 0;
		switch(vt)
		{
		case APhysXCCBase::APX_VT_BOX:
			yOffset = vol.y;
			break;
		case APhysXCCBase::APX_VT_CAPSULE:
			yOffset = vol.y * 0.5f;
			break;
		default:
			break;
		}
		yOffset *= 1.1f;

		NxVec3 center = pCC->GetCenterPosition();
		cylinder.vCenter.Set(center.x, center.y + yOffset, center.z);
	}

	BrushcharCtrler* pBrushCC = ccMgr.GetBrushHostCC();
	if (bIsDebugRender && 0 != pBrushCC)
		pBrushCC->GetCapsule().RenderWire(&wc);

	APhysXDynCharCtrler* pDynCC = ccMgr.GetAPhysXDynCC();
	if (0 != pDynCC)
	{
		NxVec3 gp0(0.0f);
		if (pDynCC->GetHangGlobalAnchor0(gp0))
		{
			A3DVECTOR3 foot = pProp->GetPos();
			A3DVECTOR3 dir = pProp->GetDir();
			A3DVECTOR3 up = pProp->GetUp();
			float h = m_objModel.GetHeight();
			up *= h;
			A3DVECTOR3 top = foot + up;
			wc.Add3DLine(APhysXConverter::N2A_Vector3(gp0), top, A3DCOLORRGB(64, 192, 64));
		}
	}
}

CMainActor::CMainActor(ApxActorBase& aab) : m_flags(0), m_actorBase(aab), m_objModel(*aab.GetObjDyn())
{
	assert(true == m_objModel.GetProperties()->QueryRoleType(CDynProperty::ROLE_ACTOR_AVATAR));
}

A3DVECTOR3 CMainActor::GetPos() const
{
	return APhysXConverter::N2A_Vector3(m_actorBase.GetPos());
}

const TCHAR* CMainActor::GetCCTypeText() const
{
	const TCHAR* pRtn = CAPhysXCCMgr::GetCCTypeText(m_objModel.GetProperties()->GetCCMgr().GetCCType());
	return pRtn;
}

APhysXDynCharCtrler* CMainActor::GetAPhysXDynCC() const
{
	const CAPhysXCCMgr& ccm = m_objModel.GetProperties()->GetCCMgr();
	return ccm.GetAPhysXDynCC();
}

CPhysXObjDynamic* CMainActor::GetDynObj() const
{
	return &m_objModel;
}

ApxActorBase* CMainActor::GetActorBase() const
{
	return &m_actorBase;
}

IAPWalk* CMainActor::GetAPWalk() const
{
	IActionPlayerBase* pAP = m_objModel.GetProperties()->GetActionPlayer();
	if (0 == pAP)
		return 0;

	if (IActionPlayerBase::AP_WALK_RANDOM > pAP->GetAPType())
		return 0;

	return dynamic_cast<IAPWalk*>(pAP);
}

void CMainActor::SetLButtonDown()
{
	RaiseFlag(MAF_LBTN_PRESSED);
	m_dwLBtnDownTime = GetTickCount(); 
}

void CMainActor::SetLButtonUp()
{
	ClearFlag(MAF_LBTN_PRESSED);
	DWORD dwDeltaTime = GetTickCount() - m_dwLBtnDownTime;
	if ((0 < dwDeltaTime) && (1000 > dwDeltaTime))
	{
		RaiseFlag(MAF_LBTN_CLICK);
		return;
	}
	
	IAPWalk* pAPW = GetAPWalk();
	if (0 != pAPW)
	{
		if (IsInAir())
			pAPW->FallToStand();
		else
			pAPW->Stand();
	}
}

void CMainActor::Render(A3DWireCollector& wc, bool bIsDebugRender, const CMainActor* pCurrentMA) 
{
	A3DMATRIX4 rot;
	A3DCYLINDER cylinder;
	cylinder.vCenter = m_objModel.GetHeadTopPos(&rot);
	cylinder.vAxisX = rot.GetRow(0);
	cylinder.vAxisY = rot.GetRow(1);
	cylinder.vAxisZ = rot.GetRow(2);
	cylinder.fHalfLen = 0.05f;
	cylinder.fRadius = 0.3f;

	CDynProperty* pProp = m_objModel.GetProperties();
	const CAPhysXCCMgr& ccMgr = pProp->GetCCMgr();
	APhysXCCBase* pCC = ccMgr.GetAPhysXCCBase();
	if (0 != pCC)
	{
		APhysXCCBase::APhysXVolumeType vt;
		const NxVec3 vol = pCC->GetVolume(vt);
		float yOffset = 0;
		switch(vt)
		{
		case APhysXCCBase::APX_VT_BOX:
			yOffset = vol.y;
			break;
		case APhysXCCBase::APX_VT_CAPSULE:
			yOffset = vol.y * 0.5f;
			break;
		default:
			break;
		}
		yOffset *= 1.1f;
		
		NxVec3 center = pCC->GetCenterPosition();
		cylinder.vCenter.Set(center.x, center.y + yOffset, center.z);
	}

	BrushcharCtrler* pBrushCC = ccMgr.GetBrushHostCC();
	if (bIsDebugRender && 0 != pBrushCC)
		pBrushCC->GetCapsule().RenderWire(&wc);

	if (pCurrentMA != this)
		wc.AddCylinder(cylinder, 0xffAA3333);

	APhysXDynCharCtrler* pDynCC = ccMgr.GetAPhysXDynCC();
	if (0 != pDynCC)
	{
		NxVec3 gp0(0.0f);
		if (pDynCC->GetHangGlobalAnchor0(gp0))
		{
			A3DVECTOR3 foot = pProp->GetPos();
			A3DVECTOR3 dir = pProp->GetDir();
			A3DVECTOR3 up = pProp->GetUp();
			float h = m_objModel.GetHeight();
			up *= h;
			A3DVECTOR3 top = foot + up;
			wc.Add3DLine(APhysXConverter::N2A_Vector3(gp0), top, A3DCOLORRGB(64, 192, 64));
		}
	}
}

A3DVECTOR3 CMainActor::GetDeltaMove() const
{
	return APhysXConverter::N2A_Vector3(m_actorBase.GetGroundMoveController()->GetDeltaMove());
}

void CMainActor::WalkTo(const A3DVECTOR3& pos) const
{
	m_actorBase.SetGroundTargetPos(APhysXConverter::A2N_Vector3(pos));
}

bool CMainActor::IsInAir() const
{
	return m_actorBase.GetGroundMoveController()->IsInAir();
}

bool CMainActor::IsKeepWalking()
{
	if (!ReadFlag(MAF_LBTN_PRESSED))
		return false;

	if (ReadFlag(MAF_LBTN_CLICK))
	{
		ClearFlag(MAF_LBTN_CLICK);
		return false;
	}

	return !IsInAir();
}

bool CMainActor::IsRunEnable() const
{
	IAPWalk* pAPW = GetAPWalk();
	if (0 == pAPW)
		return false;

	return pAPW->IsRunEnable();
}

void CMainActor::EnableRun(const bool bEnable) const
{
	IAPWalk* pAPW = GetAPWalk();
	if (0 != pAPW)
		pAPW->EnableRun(bEnable);
}

void CMainActor::Jump(float fJumpSpeed) const
{
	m_actorBase.GroundJump(fJumpSpeed);
}

bool CMainActor::HangEnd() const
{
	m_actorBase.GetGroundMoveController()->EndSwing();
	APhysXDynCharCtrler* pDCC = GetAPhysXDynCC();
	if (0 != pDCC)
		return true;
	return false;
}

void CMainActor::UpdateTargetPT(const int x, const int y)
{
	CRecording& rec = m_objModel.GetScene()->GetRecording();
	if (rec.IsReplaying())
	{
		if (rec.IsEventReproducing())
			return;
	}
	if ((x == m_ptTarget.x) && (y == m_ptTarget.y))
		return;

	m_ptTarget.Set(x, y); 
}

bool CMainActor::GetTargetPT(int& xOut, int& yOut)
{
	xOut = m_ptTarget.x;
	yOut = m_ptTarget.y; 
	return true;
}

void CMainActor::SwitchCCType(CAPhysXCCMgr::CCType ct)
{
	if (CAPhysXCCMgr::CC_APHYSX_LWCC == ct)
		return;

	CAPhysXCCMgr& ccm = m_objModel.GetProperties()->GetCCMgr();
	CAPhysXCCMgr::CCType oldCT = ccm.GetCCType();
	CAPhysXCCMgr::CCType newCT = ct;
	if (CAPhysXCCMgr::CC_UNKNOWN_TYPE == ct)
	{
		if (CAPhysXCCMgr::CC_TOTAL_EMPTY == oldCT)
			newCT = CAPhysXCCMgr::CC_APHYSX_NXCC;
		else if (CAPhysXCCMgr::CC_APHYSX_NXCC == oldCT)
			newCT = CAPhysXCCMgr::CC_APHYSX_EXTCC;
		else if (CAPhysXCCMgr::CC_APHYSX_EXTCC == oldCT)
			newCT = CAPhysXCCMgr::CC_APHYSX_DYNCC;
		else if (CAPhysXCCMgr::CC_APHYSX_DYNCC == oldCT)
			newCT = CAPhysXCCMgr::CC_BRUSH_CDRCC;
		else if (CAPhysXCCMgr::CC_BRUSH_CDRCC == oldCT)
			newCT = CAPhysXCCMgr::CC_TOTAL_EMPTY;
		else
			newCT = oldCT;
	}

	if (oldCT == newCT)
		return;

	if (ccm.SwtichCC(newCT, true))
	{
		float speed = m_actorBase.GetGroundMoveController()->GetMoveSpeed();
		APhysXHostCCBase* pHostCC = ccm.GetAPhysXHostCCBase();
		if (0 != pHostCC)
			pHostCC->SetSpeed(speed);
		BrushcharCtrler* pBrushCC = ccm.GetBrushHostCC();
		if (0 != pBrushCC)
			pBrushCC->SetSpeed(speed);
	}
}

/*
void CMainActor::UpdateAura() const
{
	CPhysXObjForceField* pFF = m_objModel.GetAssociateFF();
	if (0 == pFF)
		return;

	APhysXCollisionChannel hostCNL = m_objModel.GetChannel();
	APhysXCCBase* pCCBase = GetAPhysXCCBase();
	if (0 != pCCBase)
		hostCNL = pCCBase->GetHostModelChannel();
	pFF->SetHostChannel(&hostCNL);
}

void CMainActor::TickAura() const
{
	CPhysXObjForceField* pFF = m_objModel.GetAssociateFF();
	if (0 == pFF)
		return;

	ACWalk* pACW = GetAPWalk();
	if (0 != pACW)
	{
		pFF->SetState(!pACW->IsMovingState(), pACW->IsRunEnable());
	}
}*/
void CMainActor::TickMA(float dtSec) const
{
	m_actorBase.Tick(dtSec);
}

