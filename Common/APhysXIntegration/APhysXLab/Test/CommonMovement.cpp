/*
 * FILE: CommonMovement.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/12/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

PlayTask::PlayTask()
{
	valSpeed = 5;  // Unit: meters per second
	valRot = 60;  // Unit: degree per second
	MoveDir.set(0, 1, 0);
	AngleVelAxis.set(0, 1, 0);
	flags = 0;
}

NxVec3 PlayTask::GetVelocity(const bool IsLinear, float& coefficient) const
{
	if (0 > coefficient)
		coefficient = 0;
	
	if (IsLinear)
		return valSpeed * coefficient * MoveDir;
	
	return valRot * coefficient * AngleVelAxis;
}

NxVec3 PlayTask::GetMovement(float dwDeltaTimeSec, float& coefficient) const
{
	return GetVelocity(true, coefficient) * dwDeltaTimeSec;
}

NxQuat PlayTask::GetRotation(float dwDeltaTimeSec, float& coefficient) const
{
	return NxQuat(valRot * coefficient * dwDeltaTimeSec, AngleVelAxis);
}

ActorObj::ActorObj(NxActor& actor)
{
	Init();
	pActor = &actor;
	pose = pActor->getGlobalPose();
}

ActorObj::ActorObj(IPhysXObjBase& obj)
{
	Init();
	pObj = &obj;
}

void ActorObj::Init()
{
	flags = 0;
	pActor = 0;
	pObj = 0;
	bound.setEmpty();
}

void ActorObj::Release(CGame* pGame)
{
	if (0 == pObj)
	{
		if (0 != pActor)
			pActor->getScene().releaseActor(*pActor);
	}
	else
	{
		assert(0 != pGame);
		if (0 != pGame)
			pGame->ReleaseObject(pObj);
	}
	pActor = 0;
	pObj = 0;
}

bool ActorObj::CheckBound(const NxVec3& pos, NxVec3& val)
{
	if (!ReadFlag(SF_HAS_BOUND))
		return true;
	
	if (bound.contain(pos))
		return true;
	
	NxVec3 center;
	bound.getCenter(center);
	
	NxVec3 V0 = pos - center;
	NxVec3 V1(val);
	V0.normalize();
	V1.normalize();
	
	if (0 < V0.dot(V1))
		return false;
	
	return true;
}

bool ActorObj::GetVariableCoefficient(const bool IsLinear, float& outV)
{
	outV = 1.0f;
	if (IsLinear)
	{
		if (!ReadFlag(SF_ISL_VARIABLE))
			return false;
	}
	else
	{
		if (!ReadFlag(SF_ISA_VARIABLE))
			return false;
	}
	
	static float ValMin = 0.8f;
	static float ValMax = 1.2f;
	static float step = (ValMax - ValMin) / 100;
	static float ValCurrent = 1.0f;
	static bool IsIns = true;
	
	if (IsIns)
	{
		ValCurrent += step;
		if (ValCurrent > ValMax)
		{
			IsIns = false;
			ValCurrent = ValMax;
		}
	}
	else
	{
		ValCurrent -= step;
		if (ValCurrent < ValMin)
		{
			IsIns = true;
			ValCurrent = ValMin;
		}
	}
	
	outV = ValCurrent;
	return true;
}

void ActorObj::OnTickActor(float dwDeltaTimeSec, const PlayTask& PT)
{
	static const NxVec3 gZero(0, 0, 0);
	
	if (0 == pActor)
	{
		pActor = pObj->GetFirstNxActor();
		if (0 == pActor)
			return;
		pose = pActor->getGlobalPose();
	}
	
	if (!pActor->isDynamic())
		return;
	
	bool IsK = pActor->readBodyFlag(NX_BF_KINEMATIC);

	if (!PT.ReadFlag(PlayTask::SF_PT_LINEAR))
	{
		if (PT.ReadFlag(PlayTask::SF_CHANGED_LINEAR))
			pActor->setLinearVelocity(gZero);
	}
	else
	{
		float vCoef = 1.0f;
		bool IsV = GetVariableCoefficient(true, vCoef);
		if (IsK)
		{
			NxVec3 move = PT.GetMovement(dwDeltaTimeSec, vCoef);
			if (!CheckBound(pActor->getGlobalPosition(), move))
			{
				PT.MoveDir *= -1;
				move = PT.GetMovement(dwDeltaTimeSec, vCoef);
			}
			pActor->moveGlobalPosition(pActor->getGlobalPosition() + move);
		}
		else
		{
			if (IsV || PT.ReadFlag(PlayTask::SF_CHANGED_LINEAR))
			{
				NxVec3 vel = PT.GetVelocity(true, vCoef);
				if (!CheckBound(pActor->getGlobalPosition(), vel))
				{
					PT.MoveDir *= -1;
					vel = PT.GetVelocity(true, vCoef);
				}
				pActor->setLinearVelocity(vel);
			}
		}
	}
		
	if (!PT.ReadFlag(PlayTask::SF_PT_ANGULAR))
	{
		if (PT.ReadFlag(PlayTask::SF_CHANGED_ANGULAR))
			pActor->setAngularVelocity(gZero);
	}
	else
	{	
		float vCoef = 1.0f;
		bool IsV = GetVariableCoefficient(false, vCoef);
		if (IsK)
		{
			const NxQuat rot = PT.GetRotation(dwDeltaTimeSec, vCoef);
			pActor->moveGlobalOrientationQuat(rot * pActor->getGlobalOrientationQuat());

		}
		else
		{
			if (IsV || PT.ReadFlag(PlayTask::SF_CHANGED_ANGULAR))
				pActor->setAngularVelocity(PT.GetVelocity(false, vCoef));
		}
	}

	if (PT.ReadFlag(PlayTask::SF_PT_RESET_POSE))
	{
		if (IsK)
			pActor->moveGlobalPose(pose);
		else
			pActor->setGlobalPose(pose);
	}
}

bool ActorObj::GetPos(NxVec3& outPos)
{
	if (0 != pObj)
	{
		A3DVECTOR3 pos = pObj->GetPos();
		outPos.x = pos.x;
		outPos.y = pos.y;
		outPos.z = pos.z;
		return true;
	}
	
	if (0 != pActor)
	{
		outPos = pActor->getGlobalPosition();
		return true;
	}
	
	return false;
}

void MMPolicy::InIt()
{
	m_Enable = false;

	m_rot.AngleVelAxis.set(0, 1, 0);
	m_vertical.MoveDir.set(0, 1, 0);
	m_horizontal.MoveDir.set(1, 0, 0);
	m_skewed.MoveDir.set(1, 1, 0);
}

void MMPolicy::TickScenario(float dwDeltaTimeSec)
{
	if (!m_Enable)
		return;

	NxU32 nSize = 0;
	if (!m_rot.IsDisable())
	{
		nSize = m_pActorsRot.size();
		for (NxU32 i = 0; i < nSize; ++i)
			m_pActorsRot[i].OnTickActor(dwDeltaTimeSec, m_rot);
		m_rot.ClearFlag(PlayTask::SF_CHANGED_LINEAR);
		m_rot.ClearFlag(PlayTask::SF_CHANGED_ANGULAR);
		m_rot.ClearFlag(PlayTask::SF_PT_RESET_POSE);
	}

	if (!m_vertical.IsDisable())
	{
		nSize = m_pActorsVtl.size();
		for (NxU32 i = 0; i < nSize; ++i)
			m_pActorsVtl[i].OnTickActor(dwDeltaTimeSec, m_vertical);
		m_vertical.ClearFlag(PlayTask::SF_CHANGED_LINEAR);
		m_vertical.ClearFlag(PlayTask::SF_CHANGED_ANGULAR);
		m_vertical.ClearFlag(PlayTask::SF_PT_RESET_POSE);
	}

	if (!m_horizontal.IsDisable())
	{
		nSize = m_pActorsHzn.size();
		for (NxU32 i = 0; i < nSize; ++i)
			m_pActorsHzn[i].OnTickActor(dwDeltaTimeSec, m_horizontal);
		m_horizontal.ClearFlag(PlayTask::SF_CHANGED_LINEAR);
		m_horizontal.ClearFlag(PlayTask::SF_CHANGED_ANGULAR);
		m_horizontal.ClearFlag(PlayTask::SF_PT_RESET_POSE);
	}

	if (!m_skewed.IsDisable())
	{
		nSize = m_pActorsSkw.size();
		for (NxU32 i = 0; i < nSize; ++i)
			m_pActorsSkw[i].OnTickActor(dwDeltaTimeSec, m_skewed);
		m_skewed.ClearFlag(PlayTask::SF_CHANGED_LINEAR);
		m_skewed.ClearFlag(PlayTask::SF_CHANGED_ANGULAR);
		m_skewed.ClearFlag(PlayTask::SF_PT_RESET_POSE);
	}

	if (!m_general.IsDisable())
	{
		nSize = m_pActorsGnr.size();
		for (NxU32 i = 0; i < nSize; ++i)
			m_pActorsGnr[i].OnTickActor(dwDeltaTimeSec, m_general);
		m_general.ClearFlag(PlayTask::SF_CHANGED_LINEAR);
		m_general.ClearFlag(PlayTask::SF_CHANGED_ANGULAR);
		m_general.ClearFlag(PlayTask::SF_PT_RESET_POSE);
	}
}

bool MMPolicy::ControlScenario(const unsigned int nChar)
{
	const float valForce = 500;
	const float valAngVel = 3;
	switch (nChar)
	{
	case VK_UP:
		m_general.MoveDir.set(0, 1, 0);
		m_general.RaiseFlag(PlayTask::SF_PT_LINEAR);
		m_general.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		if (m_general.ReadFlag(PlayTask::SF_PT_ANGULAR))
		{
			m_general.ClearFlag(PlayTask::SF_PT_ANGULAR);
			m_general.RaiseFlag(PlayTask::SF_CHANGED_ANGULAR);
		}
		break;
	case VK_DOWN:
		m_general.MoveDir.set(0, -1, 0);
		m_general.RaiseFlag(PlayTask::SF_PT_LINEAR);
		m_general.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		if (m_general.ReadFlag(PlayTask::SF_PT_ANGULAR))
		{
			m_general.ClearFlag(PlayTask::SF_PT_ANGULAR);
			m_general.RaiseFlag(PlayTask::SF_CHANGED_ANGULAR);
		}
		break;
	case VK_LEFT:
		m_general.MoveDir.set(-1, 0, 0);
		m_general.RaiseFlag(PlayTask::SF_PT_LINEAR);
		m_general.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		if (m_general.ReadFlag(PlayTask::SF_PT_ANGULAR))
		{
			m_general.ClearFlag(PlayTask::SF_PT_ANGULAR);
			m_general.RaiseFlag(PlayTask::SF_CHANGED_ANGULAR);
		}
		break;
	case VK_RIGHT:
		m_general.MoveDir.set(1, 0, 0);
		m_general.RaiseFlag(PlayTask::SF_PT_LINEAR);
		m_general.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		if (m_general.ReadFlag(PlayTask::SF_PT_ANGULAR))
		{
			m_general.ClearFlag(PlayTask::SF_PT_ANGULAR);
			m_general.RaiseFlag(PlayTask::SF_CHANGED_ANGULAR);
		}
		break;
	case VK_NUMPAD0:  // small pad district: 0(Ins)
		m_general.AngleVelAxis.set(0, 1, 0);
		m_rot.RaiseFlag(PlayTask::SF_CHANGED_ANGULAR);
		if (m_rot.ReadFlag(PlayTask::SF_PT_ANGULAR))
			m_rot.ClearFlag(PlayTask::SF_PT_ANGULAR);
		else
			m_rot.RaiseFlag(PlayTask::SF_PT_ANGULAR);
		m_general.RaiseFlag(PlayTask::SF_CHANGED_ANGULAR);
		if (m_general.ReadFlag(PlayTask::SF_PT_ANGULAR))
			m_general.ClearFlag(PlayTask::SF_PT_ANGULAR);
		else
			m_general.RaiseFlag(PlayTask::SF_PT_ANGULAR);
		break;
	case VK_DECIMAL:  // small pad district: .(Del)
		m_vertical.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		if (m_vertical.ReadFlag(PlayTask::SF_PT_LINEAR))
			m_vertical.ClearFlag(PlayTask::SF_PT_LINEAR);
		else
			m_vertical.RaiseFlag(PlayTask::SF_PT_LINEAR);
		m_horizontal.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		if (m_horizontal.ReadFlag(PlayTask::SF_PT_LINEAR))
			m_horizontal.ClearFlag(PlayTask::SF_PT_LINEAR);
		else
			m_horizontal.RaiseFlag(PlayTask::SF_PT_LINEAR);
		m_skewed.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		if (m_skewed.ReadFlag(PlayTask::SF_PT_LINEAR))
			m_skewed.ClearFlag(PlayTask::SF_PT_LINEAR);
		else
			m_skewed.RaiseFlag(PlayTask::SF_PT_LINEAR);
		m_general.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		if (m_general.ReadFlag(PlayTask::SF_PT_LINEAR))
			m_general.ClearFlag(PlayTask::SF_PT_LINEAR);
		else
			m_general.RaiseFlag(PlayTask::SF_PT_LINEAR);
		break;
	case VK_NUMPAD3: // small pad district: 3(PgDn)
		m_vertical.ClearFlag(PlayTask::SF_PT_LINEAR);
		m_vertical.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		m_vertical.RaiseFlag(PlayTask::SF_PT_RESET_POSE);
		m_horizontal.ClearFlag(PlayTask::SF_PT_LINEAR);
		m_horizontal.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		m_horizontal.RaiseFlag(PlayTask::SF_PT_RESET_POSE);
		m_skewed.ClearFlag(PlayTask::SF_PT_LINEAR);
		m_skewed.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		m_skewed.RaiseFlag(PlayTask::SF_PT_RESET_POSE);
		m_general.ClearFlag(PlayTask::SF_PT_LINEAR);
		m_general.RaiseFlag(PlayTask::SF_CHANGED_LINEAR);
		m_general.RaiseFlag(PlayTask::SF_PT_RESET_POSE);
		m_rot.ClearFlag(PlayTask::SF_PT_ANGULAR);
		m_rot.RaiseFlag(PlayTask::SF_CHANGED_ANGULAR);
		m_rot.RaiseFlag(PlayTask::SF_PT_RESET_POSE);
	default:
		return false;
	}

	return true;
}

bool MMPolicy::Add(NxActor& obj, bool isDisableMove)
{
	assert(0 == obj.userData);
	if (0 != obj.userData)
		return false;

	ActorObj objAct(obj);
	if (isDisableMove)
		m_pActorsDis.push_back(objAct);
	else
	{
		m_pActorsGnr.push_back(objAct);
		m_Enable = true;
	}
	return true;
}

bool MMPolicy::Add(IPhysXObjBase& obj)
{
	PlayTask* pPT = 0;
	NxArray<ActorObj>* pArray = 0;

	switch (obj.GetControlGroup())
	{
	case 0:
		pPT = &m_general;
		pArray = &m_pActorsGnr;
		break;
	case 1:
		pPT = &m_rot;
		pArray = &m_pActorsRot;
		break;
	case 2:
		pPT = &m_vertical;
		pArray = &m_pActorsVtl;
		break;
	case 3:
		pPT = &m_skewed;
		pArray = &m_pActorsSkw;
		break;
	case 4:
		pPT = &m_horizontal;
		pArray = &m_pActorsHzn;
		break;
	default:
		ActorObj objAct(obj);
		m_pActorsDis.push_back(objAct);
		return true;
	}

	NxActor* pActor = obj.GetFirstNxActor();
	if (0 == pActor)
		return false;

	CPhysXObjDynamic* pDynObj = dynamic_cast<CPhysXObjDynamic*>(&obj);
	if (0 == pDynObj)
		return false;

	ActorObj objAct(obj);
	if (0 < obj.GetMotionDistance() && pDynObj->GetPhysXRigidBodyAABB(objAct.bound))
	{
		NxVec3 temp = APhysXConverter::A2N_Vector3(obj.GetPos());
		temp += pPT->MoveDir * obj.GetMotionDistance();
		objAct.bound.include(temp);
		objAct.bound.fatten(0.1f);
		objAct.RaiseFlag(ActorObj::SF_HAS_BOUND);
	}
	pArray->push_back(objAct);
	obj.RaiseFlag(IPhysXObjBase::OBF_FORCE_LIVING);
	
	m_Enable = true;
	return true;
}

void MMPolicy::RemoveAll()
{
	InIt();

	m_pActorsRot.clear();
	m_pActorsVtl.clear();
	m_pActorsHzn.clear();
	m_pActorsSkw.clear();
	m_pActorsGnr.clear();
	m_pActorsDis.clear();

	m_rot.SetDisable();
	m_vertical.SetDisable();
	m_horizontal.SetDisable();
	m_skewed.SetDisable();
	m_general.SetDisable();
}

void MMPolicy::ReleaseAll(CGame* pGame)
{
	NxArray<ActorObj>::iterator it, itEnd;
	it = m_pActorsRot.begin();
	itEnd = m_pActorsRot.end();
	for (; it != itEnd; ++it)
		it->Release(pGame);

	it = m_pActorsVtl.begin();
	itEnd = m_pActorsVtl.end();
	for (; it != itEnd; ++it)
		it->Release(pGame);

	it = m_pActorsHzn.begin();
	itEnd = m_pActorsHzn.end();
	for (; it != itEnd; ++it)
		it->Release(pGame);

	it = m_pActorsSkw.begin();
	itEnd = m_pActorsSkw.end();
	for (; it != itEnd; ++it)
		it->Release(pGame);

	it = m_pActorsGnr.begin();
	itEnd = m_pActorsGnr.end();
	for (; it != itEnd; ++it)
		it->Release(pGame);

	it = m_pActorsDis.begin();
	itEnd = m_pActorsDis.end();
	for (; it != itEnd; ++it)
		it->Release(pGame);

	RemoveAll();
}

bool MMPolicy::GetFirstActorPosInGeneralGroup(NxVec3& outPos)
{
	if (0 < m_pActorsGnr.size())
		return m_pActorsGnr[0].GetPos(outPos);

	return false;
}
