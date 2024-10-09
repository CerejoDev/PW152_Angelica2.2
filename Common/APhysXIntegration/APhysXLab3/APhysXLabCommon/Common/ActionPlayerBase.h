/*
 * FILE: ActionPlayerBase.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/07/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_ACTIONPLAYERBASE_H_
#define _APHYSXLAB_ACTIONPLAYERBASE_H_

#include <A3DVector.h>
#include <APhysX.h>

class CPhysXObjDynamic;
class A3DSkinModel; 
class A3DSkinModelActionCore;
class A3DTerrain2;
class CDynProperty;
class CAPhysXCCMgr;

class IActionPlayerBase
{
public:
	enum APType
	{
		AP_NULL				= 0,
		AP_SEQUENCE_ACTION	= 1,
		AP_WALK_RANDOM		= 2,
		AP_WALK_AROUND		= 3,
		AP_WALK_PATROL		= 4,
		AP_WALK_USERCTRL	= 5,
	};
	static const TCHAR* GetAPTypeText(const APType id);

public:
	IActionPlayerBase(CPhysXObjDynamic& objDyn, const unsigned long sleepTime = 100);
	virtual ~IActionPlayerBase() {}

	virtual APType GetAPType() const = 0;
	virtual IActionPlayerBase* Clone() const = 0;
	virtual void UpdateMove(float deltaTimeSec, bool bApplyToModel) = 0;
	virtual void PlayAction(const unsigned long deltaTime, const bool FirstAction = false) = 0;

	void FinishCurrentAction();

private:
	virtual void OnFinishCurrentAction() {};

protected:
	A3DSkinModel*		m_pModel;
	CPhysXObjDynamic*	m_pObjDyn;

	int				m_ElapsedTime;
	int				m_ActionTime;
	unsigned long	m_SleepTime;
};

class APSequence : public IActionPlayerBase
{
public:
	APSequence(CPhysXObjDynamic& objDyn, const unsigned long sleepTime = 100) : IActionPlayerBase(objDyn, sleepTime) {}

	virtual APType GetAPType() const { return AP_SEQUENCE_ACTION; }
	virtual IActionPlayerBase* Clone() const { return new APSequence(*this); }
	virtual void UpdateMove(float deltaTimeSec, bool bApplyToModel);
	virtual void PlayAction(const unsigned long deltaTime, const bool FirstAction = false);

private:
	A3DSkinModelActionCore* GetAction(const bool FirstAction);
};

class IAPWalk : public IActionPlayerBase
{
public:
	static A3DTerrain2* gpTerrain;
	static A3DVECTOR3 GetRandomPos(float radius, const A3DVECTOR3& center, const A3DVECTOR3* pAimPos = 0);

public:
	IAPWalk(CPhysXObjDynamic& objDyn, CAPhysXCCMgr& ccMgr);
	virtual ~IAPWalk() = 0 {}
	
	bool IsRunEnable() const { return ReadFlag(ACF_ENABLE_RUN); }
	bool IsMovingState() const { return ReadFlag(ACF_IS_MOVING); }
	bool IsInAirState() const { return ReadFlag(ACF_IS_INAIR); }

	A3DVECTOR3 GetDeltaMove() const;
	A3DVECTOR3 GetCurPos() const { return m_CurPos; }
	A3DVECTOR3 GetTargetDir() const;
	NxVec3	   GetTargetDirNx() const;
	bool GetNewFaceDir(A3DVECTOR3& outDir) const;
	float GetSpeed() const { return m_Speed; }

	void Stand() { EnterState(STATE_STAND); }
	void FallToStand() { RaiseFlag(ACF_ONGROUND_STOP); }
	bool WalkTo(const A3DVECTOR3& pos);
	void EnableRun(const bool bEnable);
	void Jump(float fJumpingSpeed, bool withTumbling = true);

	void ReceivePosFromCC(const A3DVECTOR3& pos);
	void ReceiveFaceDirFromCC(const A3DVECTOR3& faceDir, const A3DVECTOR3& upDir);
	virtual void UpdateMove(float deltaTimeSec, bool bApplyToModel);

protected:
	enum Flags
	{
		ACF_UPDATED_DIR	= (1<<0),
		ACF_ENABLE_RUN	= (1<<1),
		ACF_IS_MOVING	= (1<<2),
		ACF_IS_INAIR	= (1<<3),
		ACF_DIRTY_TARGET_POS = (1<<4),
		ACF_HAS_TUMBLING	 = (1<<5),
		ACF_HAS_TEMP_SPEED	 = (1<<6),
		ACF_HAS_NEW_FACEDIR	 = (1<<7),
		ACF_HAS_NEW_JUMP	 = (1<<8),
		ACF_ONGROUND_STOP	 = (1<<9),
	};

	enum ActState
	{
		STATE_STAND   = 0,
		STATE_LEISURE = 1,
		STATE_WALK    = 2,
		STATE_RUN     = 3,
	};

protected:
	bool ReadFlag(const Flags flag) const { return (m_Flags & flag)? true : false; }
	void ClearFlag(const Flags flag) { m_Flags &= ~flag; }
	void RaiseFlag(const Flags flag) { m_Flags |= flag; }

	void EnterState(const ActState newState);
	bool Tick_StateStand(const unsigned long deltaTime);

	void UpdateDirForSmoothTurning(const A3DVECTOR3& dir);
	void UpdateDirForHang(const NxVec3& dir) { m_TargetDirForHang = dir; }

private:
	virtual void OnFinishCurrentAction();

	bool IsToStop(const A3DVECTOR3& posCurrent);
	A3DVECTOR3 CalculatStep(float deltaTimeSec);

protected:
	CDynProperty* m_pProps;
	CAPhysXCCMgr* m_pCCMgr;

private:
	int		 m_Flags;
	float	 m_Speed;
	ActState m_ActState;

	A3DVECTOR3 m_LastPos;
	A3DVECTOR3 m_CurPos;
	A3DVECTOR3 m_TargetPos;

	A3DVECTOR3 m_TargetDir;
	A3DVECTOR3 m_CurFaceDir;
	float m_fTurnT;
	float m_fTheta;
	float m_fTumbleT;

	float	   m_JumpTakeoffSpeed;
	A3DVECTOR3 m_JumpTakeoffDir;
	A3DVECTOR3 m_CurVerticalVel;

	float m_HalfHeight;
	NxVec3 m_TargetDirForHang;
};

class ACNPCWalk : public IAPWalk
{
public:
	ACNPCWalk(CPhysXObjDynamic& objDyn, CAPhysXCCMgr& ccMgr, const bool bIsAround = true)
		: IAPWalk(objDyn, ccMgr), m_bIsAround(bIsAround), nStandCount(0) {}

	virtual APType GetAPType() const;
	virtual IActionPlayerBase* Clone() const { return new ACNPCWalk(*this); }
	virtual void PlayAction(const unsigned long deltaTime, const bool FirstAction = false);

private:
	bool m_bIsAround;
	int nStandCount;
};

class ACMAWalk : public IAPWalk
{
public:
	ACMAWalk(CPhysXObjDynamic& objDyn, CAPhysXCCMgr& ccMgr) : IAPWalk(objDyn, ccMgr) {}
	void SetMoveFlags(const int flags, const A3DVECTOR3& camDir);

	virtual APType GetAPType() const { return AP_WALK_USERCTRL; }
	virtual IActionPlayerBase* Clone() const { return new ACMAWalk(*this); }
	virtual void PlayAction(const unsigned long deltaTime, const bool FirstAction = false) {}

public:
	enum MoveFlag
	{
		MOVE_FORWARD     = (1<<0),
		MOVE_BACKWARD    = (1<<1),
		MOVE_LEFTSTRAFE	 = (1<<2),
		MOVE_RIGHTSTRAFE = (1<<3),
		MOVE_UP			 = MOVE_FORWARD,
		MOVE_DOWN		 = MOVE_BACKWARD,
		MOVE_KEY_LEFT	 = MOVE_LEFTSTRAFE,
		MOVE_KEY_RIGHT	 = MOVE_RIGHTSTRAFE,
	};

private:
	void UpdateMoveFlags(const int flags, const A3DVECTOR3& camDir);
	void UpdateHangFlags(const int flags, const A3DVECTOR3& camDir);

private:
	int		m_moveFlag;
};

#endif
