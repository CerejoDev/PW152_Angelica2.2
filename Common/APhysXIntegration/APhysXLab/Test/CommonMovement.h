/*
 * FILE: CommonMovement.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/12/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_COMMON_MOVEMENT_H_
#define _APHYSXLAB_COMMON_MOVEMENT_H_

#include <NxArray.h>
#include <NxVec3.h>
#include <NxBounds3.h>
#include <NxMat34.h>

class NxActor;
class IPhysXObjBase;
class CGame;

class PlayTask
{
public:
	enum PTStatusFlag
	{
		SF_CHANGED_LINEAR	= (1<<0),
		SF_CHANGED_ANGULAR	= (1<<1),
		SF_PT_LINEAR		= (1<<2),
		SF_PT_ANGULAR		= (1<<3),
		SF_PT_RESET_POSE	= (1<<4),
	};
	
public:
	PlayTask();

	bool IsDisable() const { return (0 == flags); }
	void SetDisable() { flags = 0; }

	bool ReadFlag(const PTStatusFlag flag) const { return (flags & flag)? true : false; }
	void ClearFlag(const PTStatusFlag flag) { flags &= ~flag; }
	void RaiseFlag(const PTStatusFlag flag) { flags |= flag; }
	
	NxVec3 GetVelocity(const bool IsLinear, float& coefficient) const;
	NxVec3 GetMovement(float dwDeltaTimeSec, float& coefficient) const;
	NxQuat GetRotation(float dwDeltaTimeSec, float& coefficient) const;
	
public:
	mutable NxVec3 MoveDir;
	NxVec3 AngleVelAxis;
	
private:
	int	flags;
	float valSpeed;
	float valRot;
};

class ActorObj
{
public:
	enum AOStatusFlag
	{
		SF_HAS_BOUND	= (1<<0),
		SF_ISL_VARIABLE	= (1<<1),
		SF_ISA_VARIABLE	= (1<<2),
	};
	
public:
	ActorObj(NxActor& actor);
	ActorObj(IPhysXObjBase& obj);
	void Release(CGame* pGame); 
	
	bool ReadFlag(const AOStatusFlag flag) const { return (flags & flag)? true : false; }
	void ClearFlag(const AOStatusFlag flag) { flags &= ~flag; }
	void RaiseFlag(const AOStatusFlag flag) { flags |= flag; }
	
	void OnTickActor(float dwDeltaTimeSec, const PlayTask& PT);
	bool GetPos(NxVec3& outPos);

private:
	void Init();
	bool CheckBound(const NxVec3& pos, NxVec3& val);
	bool GetVariableCoefficient(const bool IsLinear, float& outV);
	
public:
	NxBounds3 bound;
	
private:
	int	flags;
	NxActor* pActor;
	IPhysXObjBase* pObj;

	NxMat34 pose;
};

class MMPolicy
{
public:
	MMPolicy() { InIt(); }

	void TickScenario(float dwDeltaTimeSec);
	bool ControlScenario(const unsigned int nChar);
	bool GetFirstActorPosInGeneralGroup(NxVec3& outPos);

	bool Add(IPhysXObjBase& obj);
	bool Add(NxActor& obj, bool isDisableMove = false);
	void RemoveAll();
	void ReleaseAll(CGame* pGame);

private:
	void InIt();

private:
	bool m_Enable;

	PlayTask m_rot;
	PlayTask m_vertical;
	PlayTask m_horizontal;
	PlayTask m_skewed;
	PlayTask m_general;

	NxArray<ActorObj> m_pActorsRot;
	NxArray<ActorObj> m_pActorsVtl;
	NxArray<ActorObj> m_pActorsHzn;
	NxArray<ActorObj> m_pActorsSkw;
	NxArray<ActorObj> m_pActorsGnr;
	NxArray<ActorObj> m_pActorsDis;
};

#endif