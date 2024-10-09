/*
 * FILE: MainActor.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WenFeng He, 2010/01/07
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include "ApxAttack.h"

using namespace _SGC;

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
	
	ACWalk* pACW = GetAPWalk();
	if ((0 != pACW) && !IsInAir())
		pACW->Stand();	
}

void CMainActor::ScanControlKey(const A3DVECTOR3& camDir)  const
{
	ACWalk* pACW = GetAPWalk();
	if (0 == pACW)
		return;

	ACMAWalk* pACMAW = dynamic_cast<ACMAWalk*>(pACW);
	if (0 == pACMAW)
		return;

	int flags = 0;
	if (pACMAW->IsHangMode())
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
	
	pACMAW->SetMoveFlag(flags, camDir);
};

void CMainActor::Render(A3DWireCollector& wc, const CMainActor* pCurrentMA) 
{
	A3DMATRIX4 rot;
	A3DCYLINDER cylinder;
	cylinder.vCenter = m_objModel.GetHeadTopPos(&rot);
	cylinder.vAxisX = rot.GetRow(0);
	cylinder.vAxisY = rot.GetRow(1);
	cylinder.vAxisZ = rot.GetRow(2);
	cylinder.fHalfLen = 0.05f;
	cylinder.fRadius = 0.3f;

	APhysXCCBase* pCC = GetAPhysXCCBase();
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
	BrushcharCtrler* pBrushCC = GetBrushCC();
	if (g_Game.IsPhysXDebugRenderEnabled() && 0 != pBrushCC)
	{
		pBrushCC->GetCapsule().RenderWire(&wc);
	}

	if (pCurrentMA != this)
		wc.AddCylinder(cylinder, 0xffAA3333);
//	else
//		wc.AddCylinder(cylinder, 0xff33ff33);
	
	APhysXDynCharCtrler* pDynCC = GetAPhysXDynCC();
	if (0 != pDynCC)
	{
		NxVec3 gp0(0.0f);
		if (pDynCC->GetHangGlobalAnchor0(gp0))
		{
			A3DVECTOR3 foot = m_objModel.GetPos();
			A3DVECTOR3 dir, up;
			m_objModel.GetDirAndUp(dir, up);
			float h = m_objModel.GetHeight();
			up *= h;
			A3DVECTOR3 top = foot + up;
			wc.Add3DLine(APhysXConverter::N2A_Vector3(gp0), top, A3DCOLORRGB(64, 192, 64));
		}
	}
}

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
}

A3DVECTOR3 CMainActor::GetDeltaMove() const
{
	ACWalk* pACW = GetAPWalk();
	if (0 == pACW)
		return A3DVECTOR3(0);
	
	return pACW->GetDeltaMove(); 
}

void CMainActor::WalkTo(const A3DVECTOR3& pos) const
{
	ACWalk* pACW = GetAPWalk();
	if (0 != pACW)
		pACW->WalkTo(pos);
}

bool CMainActor::IsInAir() const
{
	APhysXCharacterController* pCC = GetAPhysXCC();
	if (0 != pCC)
	{
		int rtn = APhysXCharacterController::APX_CCS_JUMPING_INAIR & pCC->GetStatus();
		return 0 != rtn;
	}
	APhysXDynCharCtrler* pDynCC = GetAPhysXDynCC();
	if (0 != pDynCC)
	{
		int rtn = APhysXDynCharCtrler::APX_CCS_JUMPING_INAIR & pDynCC->GetStatus();
		return 0 != rtn;
	}
	BrushcharCtrler* pBrushCC = GetBrushCC();
	if (0 != pBrushCC)
	{
		return pBrushCC->IsInAir();
	}
	APhysXNxCharCtrler* pNxCC = GetAPhysXNxCC();
	if (0 != pNxCC)
	{
		int rtn = APhysXNxCharCtrler::APX_CCS_JUMPING_INAIR & pNxCC->GetStatus();
		return 0 != rtn;
	}
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

	IActionPlayerBase* pAC = m_objModel.GetPlayAction();
	if (0 == pAC)
		return false;

	return !IsInAir();
}

bool CMainActor::IsRunEnable() const
{
	ACWalk* pACW = GetAPWalk();
	if (0 == pACW)
		return false;

	return pACW->IsRunEnable();
}

void CMainActor::EnableRun(const bool bEnable) const
{
	ACWalk* pACW = GetAPWalk();
	if (0 != pACW)
		pACW->EnableRun(bEnable);
}

void CMainActor::Jump(float fJumpSpeed) const
{
	APhysXCharacterController* pCC = GetAPhysXCC();
	if (0 != pCC)
	{
	//	pCC->JumpCC(4.84974f, true); for xajh
		pCC->JumpCC(fJumpSpeed, true);
		return;
	}

	APhysXDynCharCtrler* pDynCC = GetAPhysXDynCC();
	if (0 != pDynCC)
	{
		pDynCC->JumpCC(fJumpSpeed, false);
		return;
	}

	APhysXNxCharCtrler* pNxCC = GetAPhysXNxCC();
	if (0 != pNxCC)
	{
		pNxCC->JumpCC(fJumpSpeed, false);
		return;
	}
	
	BrushcharCtrler* pBrushCC = GetBrushCC();
	if (0 != pBrushCC)
	{
		pBrushCC->JumpCC(fJumpSpeed, false);
		return;
	}

	ACWalk* pACW = GetAPWalk();
	if (0 != pACW)
		pACW->Jump(fJumpSpeed, false);
}

void CMainActor::UpdateTargetPT(const int x, const int y)
{
	if ((x == m_ptTarget.x) && (y == m_ptTarget.y))
		return;

	m_ptTarget.Set(x, y); 
	RaiseFlag(MAF_TARGET_PT_UPDATED);
}

bool CMainActor::GetTargetPT(int& xOut, int& yOut)
{
	xOut = m_ptTarget.x;
	yOut = m_ptTarget.y; 
	bool rtn = ReadFlag(MAF_TARGET_PT_UPDATED);
	if (rtn)
		ClearFlag(MAF_TARGET_PT_UPDATED); 
	return rtn; 
}

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
}

ACWalk* CMainActor::GetAPWalk() const
{
	IActionPlayerBase* pAP = m_objModel.GetPlayAction();
	if (0 == pAP)
		return 0;
	
	if (IActionPlayerBase::AP_WALK > pAP->GetTypeID())
		return 0;

	return dynamic_cast<ACWalk*>(pAP);
}

APhysXCharacterController* CMainActor::GetAPhysXCC() const
{
	IActionPlayerBase* pAC = m_objModel.GetPlayAction();
	if (0 == pAC)
		return 0;

	return pAC->GetAPhysxCC();
}

APhysXDynCharCtrler* CMainActor::GetAPhysXDynCC() const
{
	IActionPlayerBase* pAC = m_objModel.GetPlayAction();
	if (0 == pAC)
		return 0;
	
	return pAC->GetAPhysxDynCC();
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
	{
		// to update hostCNL
		assert(true == false);
		//hostCNL = pCCBase->GetHostModelChannel();
	}
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
}

const A3DAABB& CMainActor::GetModelAABB() const
{
	return m_objModel.GetSkinModel()->GetModelAABB();
}

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

void CMainActor::UpdateDynDes() const
{
	if (GetBrushCC()) 
	{
		GetBrushCC()->UpdateDynDes(); 
	}
}

