/*
 * FILE: ActionPlayerBase.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/07/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */
#include "stdafx.h"
#include "BrushcharCtrler.h"

const TCHAR* IActionPlayerBase::GetAPTypeText(const APType id)
{
	switch (id)
	{
	case AP_NULL:
		return _T("Null");
	case AP_SEQUENCE_ACTION:
		return _T("Sequence Play");
	case AP_WALK_RANDOM:
		return _T("Random Walk");
	case AP_WALK_AROUND:
		return _T("Walk Around");
	case AP_WALK_PATROL:
		return _T("Walk Patrol");
	case AP_WALK_USERCTRL:
		return _T("User Ctrl");
	}

	assert(!"Unknown APType!");
	return _T("");
}

IActionPlayerBase::IActionPlayerBase(CPhysXObjDynamic& objDyn, const unsigned long sleepTime)
{
	m_pObjDyn = &objDyn;
	m_pModel = objDyn.GetSkinModel();

	FinishCurrentAction();
	m_SleepTime = sleepTime;
}

void IActionPlayerBase::FinishCurrentAction()
{
	m_ElapsedTime = (std::numeric_limits<int>::max)();
	m_ActionTime = 0;
	OnFinishCurrentAction();
}

void APSequence::UpdateMove(float deltaTimeSec, bool bApplyToModel)
{
/*	if (0 == m_pAPhysXCC)
		return;

	static NxVec3 moveDir(0.0f);
	m_pAPhysXCC->SetDir(APhysXConverter::A2N_Vector3(m_pModel->GetDir()));
	m_pAPhysXCC->MoveCC(deltaTimeSec, moveDir);*/
}

A3DSkinModelActionCore* APSequence::GetAction(const bool FirstAction)
{
	A3DSkinModelActionCore* pRtn = 0;
	if (FirstAction)
	{
		pRtn = m_pModel->GetFirstAction();
		return pRtn;
	}
	
	pRtn = m_pModel->GetNextAction();
	if (0 == pRtn)
		pRtn = m_pModel->GetFirstAction();
	return pRtn;
}

void APSequence::PlayAction(const unsigned long deltaTime, const bool FirstAction)
{
	if(m_ElapsedTime > m_ActionTime)
	{
		m_ElapsedTime = 0;
		A3DSkinModelActionCore* pAction = GetAction(FirstAction);
		if(0 == pAction)
		{
			m_ActionTime = 0;
			return;
		}

		m_ActionTime = pAction->GetActionTime() + m_SleepTime;
		m_pModel->PlayActionByNameDC(pAction->GetName());
	}
	m_ElapsedTime += deltaTime;
}

A3DTerrain2* IAPWalk::gpTerrain = 0;
A3DVECTOR3 IAPWalk::GetRandomPos(float radius, const A3DVECTOR3& center, const A3DVECTOR3* pAimPos)
{
	static const float fmin = 1.0f;
	if (0 > radius)
		radius *= -1;
	if (fmin > radius)
		radius = 1;
	
	A3DVECTOR3 dir(0.0f);
	if (0 != pAimPos)
	{
		dir = *pAimPos - center;
		float fAim = dir.Magnitude();
		if (fmin > fAim)
			return *pAimPos;
		if (radius > fAim)
			radius = fAim;
	}
	else
	{
		dir.x = a_Random(-1.0f, 1.0f);
		dir.z = a_Random(-1.0f, 1.0f);
	}
	dir.Normalize();
	
	float dist = a_Random(0.0f, radius);
	A3DVECTOR3 pos = center + dir * dist;

	if (0 != gpTerrain)
	{
		float fHeight = 0.0f;
		if (GetTerrainHeight(gpTerrain, pos, fHeight))
			pos.y = fHeight;
	}
	return pos;
}

IAPWalk::IAPWalk(CPhysXObjDynamic& objDyn, CAPhysXCCMgr& ccMgr) : IActionPlayerBase(objDyn)
{
	m_pProps = objDyn.GetProperties();
	m_pCCMgr = &ccMgr;

	m_Flags = 0;
	m_Speed = 1.5f;
	m_ActState = STATE_LEISURE;
	
	m_TargetPos = m_CurPos = m_LastPos = m_pProps->GetPos();
	m_TargetDir = m_pProps->GetDir();
	m_TargetDir.y = 0.0f;
	m_TargetDir.Normalize();
	m_CurFaceDir = m_TargetDir;
	m_fTurnT = -1.0f;
	m_fTheta = 0.0f;
	m_fTumbleT = 0.0f;

	m_JumpTakeoffSpeed = m_Speed;
	m_JumpTakeoffDir = m_TargetDir;
	m_CurVerticalVel.Clear();

	m_HalfHeight = 0.0f;
	m_TargetDirForHang.zero();
	RaiseFlag(ACF_HAS_NEW_FACEDIR);
}

A3DVECTOR3 IAPWalk::GetDeltaMove() const
{
	return m_CurPos - m_LastPos;
}

A3DVECTOR3 IAPWalk::GetTargetDir() const
{
	if (m_pCCMgr->IsHangMode())
		return APhysXConverter::N2A_Vector3(m_TargetDirForHang);

	return m_TargetDir;
}

NxVec3 IAPWalk::GetTargetDirNx() const
{
	if (m_pCCMgr->IsHangMode())
		return m_TargetDirForHang;

	return APhysXConverter::A2N_Vector3(m_TargetDir);
}

bool IAPWalk::GetNewFaceDir(A3DVECTOR3& outDir) const
{
	if (ReadFlag(ACF_HAS_NEW_FACEDIR))
	{
		outDir = m_CurFaceDir;
		return true;
	}
	return false;
}

bool IAPWalk::WalkTo(const A3DVECTOR3& pos)
{
	if (m_pCCMgr->IsHangMode())
		return false;

	m_TargetPos = pos;
	ClearFlag(ACF_DIRTY_TARGET_POS);

	const A3DVECTOR3 vPosNow = m_pProps->GetPos();
	if (IsToStop(vPosNow))
	{
		RaiseFlag(ACF_DIRTY_TARGET_POS);
		return false;
	}

	A3DVECTOR3 vDir = m_TargetPos - vPosNow;
	UpdateDirForSmoothTurning(vDir);
	RaiseFlag(ACF_UPDATED_DIR);

	if (IsRunEnable())
		EnterState(STATE_RUN);
	else
		EnterState(STATE_WALK);
	return true;
}

void IAPWalk::EnableRun(const bool bEnable)
{
	if (bEnable == IsRunEnable())
		return;

	bEnable? RaiseFlag(ACF_ENABLE_RUN) : ClearFlag(ACF_ENABLE_RUN);
	if (bEnable)
	{
		m_Speed *= 2;
		if (STATE_WALK == m_ActState)
			EnterState(STATE_RUN);
	}
	else
	{
		m_Speed *= 0.5f;
		if (STATE_RUN == m_ActState)
			EnterState(STATE_WALK);
	}

	APhysXHostCCBase* pHostCC = m_pCCMgr->GetAPhysXHostCCBase();
	if (0 != pHostCC)
		pHostCC->SetSpeed(m_Speed);
	BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
	if (0 != pBrushCC)
		pBrushCC->SetSpeed(m_Speed);
}

void IAPWalk::Jump(float fJumpingSpeed, bool withTumbling)
{
	if (ReadFlag(ACF_IS_INAIR))
		return;

	if (0 >= fJumpingSpeed)
		return;

	RaiseFlag(ACF_HAS_NEW_JUMP);
	m_CurVerticalVel += A3DVECTOR3(0, fJumpingSpeed, 0);
	if (withTumbling)
	{
		RaiseFlag(ACF_HAS_TUMBLING);
//		A3DVECTOR3 h = m_pObjDyn->GetHeadTopPos() - m_pObjDyn->GetPos();
//		m_HalfHeight = h.Magnitude() * 0.5f;
		m_fTumbleT = 0.0f;
	}
}

void IAPWalk::EnterState(const ActState newState)
{
	if (newState == m_ActState)
		return;

	static char* szStandActs[] = 
	{
		"人族站立",
		"人族飞吻",
		"人族休闲",
		"人族_猎人_捡东西",
		"人族挖矿1",
		"人族挖矿2",
		"羽族_牧师_仙守阵",
		"羽族_牧师_祝福术",
		"羽族_牧师_专注",
		"人族法师易燃",
		"人族法师刷新",
		"人族_猎人_同伴召唤",
		"人族_猎人_战斗站立",
		"人族_猎人_战域",
	};
	static const int nCount = sizeof(szStandActs) / sizeof(char*);

	switch (newState)
	{
	case STATE_STAND:
		m_pModel->PlayActionByNameDC("人族站立", -1);
		ClearFlag(ACF_IS_MOVING);
		RaiseFlag(ACF_DIRTY_TARGET_POS);
		break;
	case STATE_LEISURE:
		m_pModel->PlayActionByNameDC(szStandActs[a_Random(0, nCount - 1)], -1);
		ClearFlag(ACF_IS_MOVING);
		RaiseFlag(ACF_DIRTY_TARGET_POS);
		break;
	case STATE_WALK:
		m_pModel->PlayActionByNameDC("人族走", -1);
		RaiseFlag(ACF_IS_MOVING);
		break;
	case STATE_RUN:
		m_pModel->PlayActionByNameDC("人族奔跑", -1);
		RaiseFlag(ACF_IS_MOVING);
		break;
	default:
		return;
	}

	m_ActState = newState;
	return;
}

void IAPWalk:: OnFinishCurrentAction()
{
	EnterState(STATE_STAND);
}

bool IAPWalk::Tick_StateStand(const unsigned long deltaTime)
{
	bool rtn = false;
	if(m_ElapsedTime > m_ActionTime)
	{
		rtn = true;
		m_ElapsedTime = 0;
		m_ActionTime = 2000 + a_Random(100, 2000);
		if (STATE_STAND == m_ActState)
			EnterState(STATE_LEISURE);
		else
		{
			m_ActionTime *= 2;
			EnterState(STATE_STAND);
		}
	}
	m_ElapsedTime += deltaTime;
	return rtn;
}

void IAPWalk::UpdateDirForSmoothTurning(const A3DVECTOR3& dir)
{
	m_TargetDir = dir;
	m_TargetDir.y = 0.0f;
	m_TargetDir.Normalize();
	if (m_TargetDir.IsZero())
		return;

	float dotVal = DotProduct(m_TargetDir, m_CurFaceDir);
	if (1.0f > dotVal)
	{
		m_fTurnT = 0.0f;
		m_fTheta = NxMath::acos(dotVal);
	}
}

bool IAPWalk::IsToStop(const A3DVECTOR3& posCurrent)
{
	if (ReadFlag(ACF_IS_INAIR))
		return false;

	if (ReadFlag(ACF_ONGROUND_STOP))
	{
		EnterState(STATE_STAND);
		ClearFlag(ACF_ONGROUND_STOP);
		return true;
	}

	if (ReadFlag(ACF_DIRTY_TARGET_POS))
		return true;

	A3DVECTOR3 vDist = m_TargetPos - posCurrent;
	vDist.y = 0;
	if (0.01 > vDist.Magnitude())
	{
		EnterState(STATE_STAND);
		return true;
	}
	return false;
}

A3DVECTOR3 IAPWalk::CalculatStep(float deltaTimeSec)
{
	static const A3DVECTOR3 g_G(0, -9.81f, 0);

	A3DVECTOR3 dHor(0.0f);
	A3DVECTOR3 dVer(0.0f);
	if (ReadFlag(ACF_IS_INAIR))
	{
		dHor = m_JumpTakeoffDir * deltaTimeSec * m_JumpTakeoffSpeed;
		dVer = m_CurVerticalVel * deltaTimeSec + 0.5f * g_G * deltaTimeSec* deltaTimeSec;
	}
	else
	{
		dHor = GetTargetDir() * deltaTimeSec * m_Speed;
		dVer = m_CurVerticalVel * deltaTimeSec;
		if (!m_CurVerticalVel.IsZero())
		{
			RaiseFlag(ACF_IS_INAIR);
			m_JumpTakeoffSpeed = m_Speed;
			m_JumpTakeoffDir = GetTargetDir();
		}
		else
		{
			if (!ReadFlag(ACF_DIRTY_TARGET_POS))
			{
				if (!NxMath::equals(0.0f, deltaTimeSec, APHYSX_FLOAT_EPSILON))
				{
					A3DVECTOR3 dEnd = m_TargetPos - (m_CurPos + dHor);
					float dotVal = DotProduct(dEnd, dHor);
					if (0.0f > dotVal)
					{
						dHor = m_TargetPos - m_CurPos;
						float mag = dHor.Magnitude();
						if (mag < deltaTimeSec * m_Speed)
						{
							float fTempSpeed = mag / deltaTimeSec;
							APhysXHostCCBase* pHostCC = m_pCCMgr->GetAPhysXHostCCBase();
							if (0 != pHostCC)
							{
								pHostCC->SetSpeed(fTempSpeed);
								RaiseFlag(ACF_HAS_TEMP_SPEED);
							}
							BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
							if (0 != pBrushCC)
							{
								pBrushCC->SetSpeed(fTempSpeed);
								RaiseFlag(ACF_HAS_TEMP_SPEED);
							}
						}
					}
				}
			}
		}
	}

	A3DVECTOR3 step = dHor + dVer;
	A3DVECTOR3 vNewPos = m_CurPos + step;
	if (0 != gpTerrain)
	{
		float yTerrain;
		if (GetTerrainHeight(gpTerrain, vNewPos, yTerrain))
		{
			if (ReadFlag(ACF_IS_INAIR))
			{
				if (vNewPos.y < yTerrain)
				{
					ClearFlag(ACF_IS_INAIR);
					ClearFlag(ACF_HAS_TUMBLING);
					vNewPos.y = yTerrain;
				}
			}
			else
			{
				vNewPos.y = yTerrain;
			}
		}
		step = vNewPos - m_CurPos;
	}

	if (ReadFlag(ACF_IS_INAIR))
		m_CurVerticalVel += g_G * deltaTimeSec; 
	else
		m_CurVerticalVel.Clear();
	return step;
}

void IAPWalk::ReceivePosFromCC(const A3DVECTOR3& pos)
{
	m_CurPos = pos;
	m_pObjDyn->SetLPos(pos);
}

void IAPWalk::ReceiveFaceDirFromCC(const A3DVECTOR3& faceDir, const A3DVECTOR3& upDir)
{
	m_CurFaceDir = faceDir;
	m_pObjDyn->SetLDirAndUp(faceDir, upDir);
}

void IAPWalk::UpdateMove(float deltaTimeSec, bool bApplyToModel)
{
	static const A3DVECTOR3 g_vUp(0, 1, 0);

	m_LastPos = m_pModel->GetPos();
	APhysXHostCCBase* pHostCC = m_pCCMgr->GetAPhysXHostCCBase();
	if (0 != pHostCC)
	{
		m_pCCMgr->SyncDynCCPoseToModel();
		if (ReadFlag(ACF_HAS_TEMP_SPEED))
		{
			ClearFlag(ACF_HAS_TEMP_SPEED);
			pHostCC->SetSpeed(m_Speed);
		}
	}
	BrushcharCtrler* pBrushCC = m_pCCMgr->GetBrushHostCC();
	if (0 != pBrushCC)
	{
		if (ReadFlag(ACF_HAS_TEMP_SPEED))
		{
			ClearFlag(ACF_HAS_TEMP_SPEED);
			pBrushCC->SetSpeed(m_Speed);
		}
	}

	m_CurPos = m_pModel->GetPos();
	bool bHorizonMoveEnable = false;
	if (ReadFlag(ACF_IS_MOVING))
		bHorizonMoveEnable = !IsToStop(m_CurPos);

	if (ReadFlag(ACF_UPDATED_DIR))
	{
		bHorizonMoveEnable = true;
		ClearFlag(ACF_UPDATED_DIR);
	}
	else if (ReadFlag(ACF_HAS_NEW_JUMP))
	{
		bHorizonMoveEnable = true;
		ClearFlag(ACF_HAS_NEW_JUMP);
	}
	else
	{
		if (ReadFlag(ACF_IS_INAIR))
		{
			bHorizonMoveEnable = true;
		}
		else
		{
			m_TargetDir.Clear();
			if (!ReadFlag(ACF_DIRTY_TARGET_POS))
			{
				A3DVECTOR3 vDir = m_TargetPos - m_CurPos;
				UpdateDirForSmoothTurning(vDir);
			}
		}
	}

	ClearFlag(ACF_HAS_NEW_FACEDIR);
	if (!bHorizonMoveEnable)
	{
		if (ReadFlag(ACF_IS_MOVING))
			Stand();
	}
	else
	{
		A3DVECTOR3 step = CalculatStep(deltaTimeSec);
		A3DVECTOR3 NextPos = m_CurPos + step;
		if (!ReadFlag(ACF_IS_MOVING))
		{
			A3DVECTOR3 stepN(step);
			stepN.Normalize();
			float dotVal = DotProduct(stepN, g_vUp);
			if (!NxMath::equals(dotVal, 1.0f, APHYSX_FLOAT_EPSILON))
			{
				if (!NxMath::equals(dotVal, -1.0f, APHYSX_FLOAT_EPSILON))
				{
					if (IsRunEnable())
						EnterState(STATE_RUN);
					else
						EnterState(STATE_WALK);
				}
			}
		}
		
		if ((0.0f <= m_fTurnT) && (!m_TargetDir.IsZero()))
		{
			m_fTurnT += deltaTimeSec * APHYSX_PI;
			
			float t = 0;
			if (!NxMath::equals(m_fTheta, 0.0f, APHYSX_FLOAT_EPSILON))
			{
				t = m_fTurnT / m_fTheta;
				if (m_fTurnT >= m_fTheta)
				{
					t = 1.0f;
					m_fTurnT = -1.0f;
				}
			}
			
			NxVec3 vNewDir = APhysXUtilLib::Slerp(APhysXConverter::A2N_Vector3(m_CurFaceDir), APhysXConverter::A2N_Vector3(m_TargetDir), t);
			m_CurFaceDir = APhysXConverter::N2A_Vector3(vNewDir);
			m_pObjDyn->SetLDirAndUp(m_CurFaceDir, g_vUp);
			RaiseFlag(ACF_HAS_NEW_FACEDIR);
		}

	/*	if (ReadFlag(ACF_HAS_TUMBLING))
		{
			m_fTumbleT += deltaTimeSec * 360;
			if (360 < m_fTumbleT)
			{
				ClearFlag(ACF_HAS_TUMBLING);
				m_fTumbleT = 0.0f;
				m_pObjDyn->SetDirAndUp(m_CurFaceDir, g_vUp);
			}
			else
			{
				NxVec3 axis = APhysXConverter::A2N_Vector3(CrossProduct(g_vUp, m_CurFaceDir));
				NxQuat quat(m_fTumbleT, axis);
				A3DVECTOR3 v = APhysXConverter::N2A_Vector3(quat.rot(NxVec3(0, -m_HalfHeight, 0))); 

				A3DVECTOR3 center = vNewPos;
				center.y += m_HalfHeight;
				vNewPos = center + v;
				m_pObjDyn->RaiseFlag(IPhysXObjBase::OBF_ASSOCIATE_ALL_ISLOCKED);
				m_pObjDyn->SetPos(vNewPos);
				m_pObjDyn->ClearFlag(IPhysXObjBase::OBF_ASSOCIATE_ALL_ISLOCKED);
				if (0 != m_pAPhysXLWCC)
					m_pAPhysXLWCC->SetFootPosition(APhysXConverter::A2N_Vector3(vNewPos));

				v *= -1;
				A3DVECTOR3 newdir = CrossProduct(APhysXConverter::N2A_Vector3(axis), v);
				m_pObjDyn->SetDirAndUp(newdir, v);
			}
		}*/

		if (bApplyToModel)
		{
			m_pObjDyn->SetLPos(NextPos);
			m_CurPos = NextPos;
		}
	}
}

IActionPlayerBase::APType ACNPCWalk::GetAPType() const
{
	if (m_bIsAround)
		return AP_WALK_AROUND;

	return AP_WALK_PATROL;
}

void ACNPCWalk::PlayAction(const unsigned long deltaTime, const bool FirstAction)
{
	if (ReadFlag(ACF_IS_MOVING))
		return;

	bool StandAgain = Tick_StateStand(deltaTime);
	if (StandAgain)
		++nStandCount;

	bool StartWalk = false;
	if (2 < nStandCount)
		StartWalk = true;
	if (1 < nStandCount)
	{
		float sign = a_Random(-1.0f, 1.0f);
		if (0 < sign)
			StartWalk = true;
	}

	if (StartWalk)
	{
		while (0 != nStandCount)
		{
			A3DVECTOR3* pAimPos = 0;
			if (!m_bIsAround)
			{
				// get next patrol aim pos
			}
			A3DVECTOR3 pos = GetRandomPos(5, m_pProps->GetPos(false), pAimPos);
			if (WalkTo(pos))
				nStandCount = 0;
		}
	}
}

void ACMAWalk::SetMoveFlags(const int flags, const A3DVECTOR3& camDir)
{
	if (m_pCCMgr->IsHangMode())
		UpdateHangFlags(flags, camDir);
	else
		UpdateMoveFlags(flags, camDir);
}

void ACMAWalk::UpdateMoveFlags(const int flags, const A3DVECTOR3& camDir)
{
	if (0 == flags)
	{
		if (0 != m_moveFlag)
		{
			m_moveFlag = 0;
			RaiseFlag(ACF_DIRTY_TARGET_POS);
		}
		return;
	}

	RaiseFlag(ACF_DIRTY_TARGET_POS);
	m_moveFlag = flags;
	if (!ReadFlag(ACF_IS_MOVING))
	{
		if (IsRunEnable())
			EnterState(STATE_RUN);
		else
			EnterState(STATE_WALK);
	}

	static const A3DVECTOR3 g_vUp(0, 1, 0);
	A3DVECTOR3 vDir(0, 0, 0);
	if (MOVE_RIGHTSTRAFE & m_moveFlag)
		vDir = CrossProduct(g_vUp, camDir);
	if (MOVE_LEFTSTRAFE & m_moveFlag)
	{
		A3DVECTOR3 vTemp = CrossProduct(g_vUp, camDir);
		vTemp *= -1;
		vDir += vTemp;
	}
	if (MOVE_FORWARD & m_moveFlag)
		vDir += camDir;
	if (MOVE_BACKWARD & m_moveFlag)
		vDir += camDir * -1;

	if (!vDir.IsZero())
	{
		RaiseFlag(ACF_UPDATED_DIR);
		UpdateDirForSmoothTurning(vDir);
	}
}

void ACMAWalk::UpdateHangFlags(const int flags, const A3DVECTOR3& camDir)
{
	NxVec3 vDir(0.0f);
	if (0 == flags)
	{
		if (0 != m_moveFlag)
		{
			m_moveFlag = 0;
			UpdateDirForHang(vDir);
		}
		return;
	}

	m_moveFlag = flags;
	APhysXHostCCBase* pCC = m_pCCMgr->GetAPhysXHostCCBase();
	NxVec3 g;
	pCC->GetNxScene().getGravity(g);
	g.normalize();

	NxVec3 up = pCC->GetUpDir();
	NxVec3 ccdir = pCC->GetDir();
	NxVec3 axis = up.cross(ccdir);
	float d = DotProduct(camDir, APhysXConverter::N2A_Vector3(axis));

	if (MOVE_UP & m_moveFlag)
		vDir = g * -1;
	if (MOVE_DOWN & m_moveFlag)
		vDir = g;
	if (0 > d)
	{
		if (MOVE_KEY_LEFT & m_moveFlag)
			vDir += pCC->GetHorizonDir() * -1;
		if (MOVE_KEY_RIGHT & m_moveFlag)
			vDir += pCC->GetHorizonDir();
	}
	else if ( 0 < d)
	{
		if (MOVE_KEY_LEFT & m_moveFlag)
			vDir += pCC->GetHorizonDir();
		if (MOVE_KEY_RIGHT & m_moveFlag)
			vDir += pCC->GetHorizonDir() * -1;
	}
	UpdateDirForHang(vDir);
}
