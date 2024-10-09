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
//#include "ApxAttack.h"

//using namespace _SGC;

CMainActor::CMainActor(CPhysXObjDynamic& obj) : m_flags(0), m_objModel(obj)
{
	assert(true == obj.GetProperties()->QueryRoleType(CDynProperty::ROLE_ACTOR_AVATAR));
}

A3DVECTOR3 CMainActor::GetPos() const
{
	return m_objModel.GetProperties()->GetPos();
}

const TCHAR* CMainActor::GetCCTypeText() const
{
	const TCHAR* pRtn = CAPhysXCCMgr::GetCCTypeText(m_objModel.GetProperties()->GetCCMgr().GetCCType());
	return pRtn;
}

APhysXDynCharCtrler* CMainActor::GetAPhysXDynCC() const
{
	const CAPhysXCCMgr& ccm = m_objModel.GetProperties()->GetCCMgr();
	return ccm.GetAPhysxDynCC();
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

void CMainActor::ScanControlKey(const A3DVECTOR3& camDir)  const
{
	IAPWalk* pACW = GetAPWalk();
	if (0 == pACW)
		return;

	ACMAWalk* pACMAW = dynamic_cast<ACMAWalk*>(pACW);
	if (0 == pACMAW)
		return;

	int flags = 0;
	if (m_objModel.GetProperties()->GetCCMgr().IsHangMode())
	{
		if (GetAsyncKeyState('W') & 0x8000)
			flags |= ACMAWalk::MOVE_UP;
		else if (GetAsyncKeyState('S') & 0x8000)
			flags |= ACMAWalk::MOVE_DOWN;
		if (GetAsyncKeyState('A') & 0x8000)
			flags |= ACMAWalk::MOVE_KEY_LEFT;
		else if (GetAsyncKeyState('D') & 0x8000)
			flags |= ACMAWalk::MOVE_KEY_RIGHT;
	}
	else
	{
		if (GetAsyncKeyState('W') & 0x8000)
			flags |= ACMAWalk::MOVE_FORWARD;
		else if (GetAsyncKeyState('S') & 0x8000)
			flags |= ACMAWalk::MOVE_BACKWARD;	
		if (GetAsyncKeyState('A') & 0x8000)
			flags |= ACMAWalk::MOVE_LEFTSTRAFE;
		else if (GetAsyncKeyState('D') & 0x8000)
			flags |= ACMAWalk::MOVE_RIGHTSTRAFE;
	}
	
	pACMAW->SetMoveFlags(flags, camDir);
};

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

	const CAPhysXCCMgr& ccMgr = m_objModel.GetProperties()->GetCCMgr();
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
	else
		wc.AddCylinder(cylinder, 0xff33ff33);
}
/*
void CMainActor::ChangeCCVolume(const bool isIncrease)
{
	APhysXCCBase* pCC = GetAPhysXCCBase();
	if (0 == pCC)
		return;
	
	APhysXCCBase::APhysXVolumeType vt;
	NxVec3 volume = pCC->GetVolume(vt);

	if (isIncrease)
		volume *= 1.1f;
	else
		volume /= 1.1f;
	pCC->ChangeVolume(volume);
}*/

A3DVECTOR3 CMainActor::GetDeltaMove() const
{
	IAPWalk* pACW = GetAPWalk();
	if (0 == pACW)
		return A3DVECTOR3(0);
	
	return pACW->GetDeltaMove(); 
}

void CMainActor::WalkTo(const A3DVECTOR3& pos) const
{
	IAPWalk* pACW = GetAPWalk();
	if (0 != pACW)
		pACW->WalkTo(pos);
}

bool CMainActor::IsInAir() const
{
	const CAPhysXCCMgr& ccMgr = m_objModel.GetProperties()->GetCCMgr();
	if (!ccMgr.QueryCCType(CAPhysXCCMgr::CC_TOTAL_EMPTY))
		return ccMgr.IsInAir();

	IAPWalk* pAPW = GetAPWalk();
	if (0 != pAPW)
		return pAPW->IsInAirState();

	return false;
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

	IActionPlayerBase* pAP = m_objModel.GetProperties()->GetActionPlayer();
	if (0 == pAP)
		return false;

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
	const CAPhysXCCMgr& ccMgr = m_objModel.GetProperties()->GetCCMgr();
	APhysXHostCCBase* pCC = ccMgr.GetAPhysXHostCCBase();
	if (0 != pCC)
	{
		pCC->JumpCC(fJumpSpeed, false);
		return;
	}

	BrushcharCtrler* pBrushCC = ccMgr.GetBrushHostCC();
	if (0 != pBrushCC)
	{
		pBrushCC->JumpCC(fJumpSpeed, false);
		return;
	}

	IAPWalk* pAPW = GetAPWalk();
	if (0 != pAPW)
		pAPW->Jump(fJumpSpeed, false);
}

void CMainActor::UpdateTargetPT(const int x, const int y)
{
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
	ccm.SwtichCC(newCT);
}

/*
void CMainActor::Attack(IPhysXObjBase& objTarget, const A3DVECTOR3& vAttackPos, float fForceMag, NxActor* pHitActor)
{
	ApxAttackSkillGfxMan* pMan = (ApxAttackSkillGfxMan*)AfxGetSkillGfxEventMan();
	pMan->SetHitForceMag(fForceMag);
	pMan->SetHitInfo(vAttackPos, &objTarget);

	A3DVECTOR3 posCaster = m_objModel.GetPos();
	if (!m_objModel.PlayAttackAction(objTarget))
	{
		ApxHitForceInfo apxHitForceInfo;
		apxHitForceInfo.mHitDir = vAttackPos - posCaster;
		apxHitForceInfo.mHitDir.y -= 1;
		apxHitForceInfo.mHitDir.Normalize();
		apxHitForceInfo.mHitForceMagnitude = fForceMag;
		apxHitForceInfo.mHitPos = vAttackPos;
		objTarget.OnHitbyForce(apxHitForceInfo, pHitActor);
	}

	A3DVECTOR3 posT = objTarget.GetPos();
	A3DVECTOR3 dir = posT - posCaster;
	m_objModel.SetDirAndUp(dir, A3DVECTOR3(0, 1, 0));

	APhysXCharacterController* pCC = GetAPhysXCC();
	if (0 != pCC)
		pCC->SetDir(APhysXConverter::A2N_Vector3(dir));
	APhysXDynCharCtrler* pDynCC = GetAPhysXDynCC();
	if (0 != pDynCC)
		pDynCC->SetDir(APhysXConverter::A2N_Vector3(dir));
}*/
/*
APhysXCharacterController* CMainActor::GetAPhysXCC() const
{
	IActionPlayerBase* pAC = m_objModel.GetPlayAction();
	if (0 == pAC)
		return 0;

	return pAC->GetAPhysxCC();
}

APhysXCCBase* CMainActor::GetAPhysXCCBase() const
{
	IActionPlayerBase* pAC = m_objModel.GetPlayAction();
	if (0 == pAC)
		return 0;
	
	return pAC->GetAPhysXCCBase();
}

BrushcharCtrler* CMainActor::GetBrushCC() const
{
	IActionPlayerBase* pAC = m_objModel.GetPlayAction();
	if (0 == pAC)
		return 0;
	
	return pAC->GetBrushCC();
}

APhysXNxCharCtrler* CMainActor::GetAPhysXNxCC() const
{
	IActionPlayerBase* pAC = m_objModel.GetPlayAction();
	if (0 == pAC)
		return 0;
	
	return pAC->GetAPhysxNxCC();
}

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
/*
void CMainActor::SwtichCC(int ccindex)
{
	IActionPlayerBase* pAC = m_objModel.GetPlayAction();
	if (0 == pAC)
		return;
	
	int info = m_objModel.GetPeopleInfo();
	
	info &= ~(IPhysXObjBase::OBPI_HAS_BRUSH_CC | IPhysXObjBase::OBPI_HAS_APHYSX_DYNCC 
		| IPhysXObjBase::OBPI_HAS_APHYSX_CC | IPhysXObjBase::OBPI_HAS_APHYSX_NXCC);
	
	switch(ccindex)
	{
	case 0:
		info |= IPhysXObjBase::OBPI_HAS_APHYSX_CC;
		break;
	case 1:
		info |= IPhysXObjBase::OBPI_HAS_APHYSX_DYNCC;
		break;
	case 2:
		info |= IPhysXObjBase::OBPI_HAS_BRUSH_CC;
		break;
	case 3:
		info |= IPhysXObjBase::OBPI_HAS_APHYSX_NXCC;
		break;
	case -1://Ë³ÐòÇÐ»»
		{
			if (0 != GetAPhysXDynCC())
			{
				info |= IPhysXObjBase::OBPI_HAS_BRUSH_CC;
				//info |= IPhysXObjBase::OBPI_HAS_APHYSX_CC;
			}
			else if ( 0 != GetAPhysXCC())
			{
				info |= IPhysXObjBase::OBPI_HAS_APHYSX_DYNCC;
			}
			else if (0 != GetBrushCC())
			{
				info |= IPhysXObjBase::OBPI_HAS_APHYSX_NXCC;
			}
			else if (0 != GetAPhysXNxCC())
			{
				info |= IPhysXObjBase::OBPI_HAS_APHYSX_CC;
			}
			break;
		}
	default:
		return;
	}
	
	if (pAC->CCPosAvailable(*g_Game.GetAPhysXScene(), info))
		m_objModel.SetPeopleFlags(info, *g_Game.GetAPhysXScene());
	
}
*/
void CMainActor::UpdateDynDes() const
{
	const CAPhysXCCMgr& ccMgr = m_objModel.GetProperties()->GetCCMgr();
	BrushcharCtrler* pBrushCC = ccMgr.GetBrushHostCC();
	if (0!= pBrushCC) 
		pBrushCC->UpdateDynDes(); 
}

