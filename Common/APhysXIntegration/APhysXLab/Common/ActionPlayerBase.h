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

class A3DSkinModel; 
class A3DSkinModelActionCore;
class A3DTerrain2;
class CPhysXObjDynamic;
class APhysXCharacterController;
class APhysXCharacterControllerManager;
class APhysXScene;
class A3DModelPhysics;
class CCameraController;

class BrushcharCtrler;

class CCHitReport : public APhysXCCHitReport
{
public:
	virtual void onShapeHit(const APhysXCCShapeHit& hit) { }//OutputDebugString("---onShapeHit---\n"); }
	virtual void onControllerHit(const APhysXCCsHit& hit) { }//OutputDebugString("---onControllerHit---\n"); }
	
	virtual void onOverlapBegin(const APhysXCCOverlap& ol);
	virtual void onOverlapContinue(const APhysXCCOverlap& ol);
	virtual void onOverlapEnd(APhysXCharacterController& controller);
	
private:
	NxArray<APhysXObject*> m_APhysXObjs;
};

class IActionPlayerBase
{
public:
	enum ID
	{
		AP_DANCE		= 0,
		AP_WALK			= 1,
		AP_WALK_AROUND	= 2,
		AP_WALK_XAXIS	= 3,
		AP_MAIN_ACTOR	= 4,
	};

public:
	IActionPlayerBase(CPhysXObjDynamic& objDyn, const unsigned long sleepTime = 100);
	virtual ~IActionPlayerBase();

	virtual ID GetTypeID() const = 0;
	virtual IActionPlayerBase* Clone() const = 0;
	virtual void UpdateMove(float deltaTimeSec) = 0;

	void PlayAction(const unsigned long deltaTime, const bool FirstAction = false);
	void FinishCurrentAction();
	void OnAfterPhysXSim();

	bool CreateAPhysxCC(APhysXScene& scene, const float yOffset = 0);
	void ReleaseAPhysxCC();
	APhysXCharacterController* GetAPhysxCC() const { return m_pAPhysXCC; }

	bool CreateAPhysxLWCC(APhysXScene& scene, const float yOffset = 0);
	void ReleaseAPhysxLWCC();
	APhysXLWCharCtrler* GetAPhysxLWCC() const { return m_pAPhysXLWCC; }
	
	bool CreateAPhysxDynCC(APhysXScene& scene, const float yOffset = 0);
	void ReleaseAPhysxDynCC();
	APhysXDynCharCtrler* GetAPhysxDynCC() const { return m_pAPhysXDynCC; }

	bool CreateBrushCC();
	void ReleaseBrushCC();
	BrushcharCtrler* GetBrushCC() const { return m_pBrushCC; }
	
	bool CreateAPhysxNxCC(APhysXScene& scene, const float yOffset = 0);
	void ReleaseAPhysxNxCC();
	APhysXNxCharCtrler* GetAPhysxNxCC() const { return m_pAPhysXNxCC; }
	
	APhysXCCBase* GetAPhysXCCBase() const { return m_pAPhysXCCBase; }
	void ReleaseAllCC();

	//切换到ccType的CC下位置是否可用
	bool CCPosAvailable(APhysXScene& scene, int ccType);

protected:
	virtual void UpdateInfoFromCC() {};

private:
	void RegisterHostModels();
	void UnRegisterHostModels(APhysXCCBase& baseCC);
	virtual void CreateAPhysxCCPost() {};
	virtual void OnFinishCurrentAction() {};
	virtual void OnPlayAction(const unsigned long deltaTime, const bool FirstAction) = 0;

protected:
	A3DSkinModel*		m_pModel;
	CPhysXObjDynamic*	m_pObjDyn;

	int				m_ElapsedTime;
	int				m_ActionTime;
	unsigned long	m_SleepTime;

	CCHitReport		m_CCHitReceiver;
	APhysXCharacterController* m_pAPhysXCC;
	APhysXLWCharCtrler*		   m_pAPhysXLWCC;
	APhysXDynCharCtrler*	   m_pAPhysXDynCC;
	APhysXCCBase*			   m_pAPhysXCCBase;

	BrushcharCtrler*           m_pBrushCC;
	APhysXNxCharCtrler*        m_pAPhysXNxCC;
};

class ACNPCDance : public IActionPlayerBase
{
public:
	ACNPCDance(CPhysXObjDynamic& objDyn, const unsigned long sleepTime = 100) : IActionPlayerBase(objDyn, sleepTime) {}

	virtual ID GetTypeID() const { return AP_DANCE; }
	virtual IActionPlayerBase* Clone() const { return new ACNPCDance(*this); }
	virtual void UpdateMove(float deltaTimeSec);

private:
	virtual void OnPlayAction(const unsigned long deltaTime, const bool FirstAction);

	A3DSkinModelActionCore* GetAction(const bool FirstAction);
};

class ACWalk : public IActionPlayerBase
{
public:
	static A3DTerrain2* gpTerrain;
	static CCameraController* gpCameraCtrl;
	static A3DVECTOR3 GetRandomPos(float radius, const A3DVECTOR3* pCurPos = 0, A3DTerrain2* pTerrain = 0, CCameraController* pCameraCtrl = 0, bool isXAxis = false);

public:
	ACWalk(CPhysXObjDynamic& objDyn);
	
	virtual void UpdateMove(float deltaTimeSec);

	void Stand() { EnterState(STATE_STAND); }
	A3DVECTOR3 GetDeltaMove() const;
	bool WalkTo(const A3DVECTOR3& pos);
	bool IsRunEnable() const { return ReadFlag(ACF_ENABLE_RUN); }
	void EnableRun(const bool bEnable);

	bool IsMovingState() const { return ReadFlag(ACF_IS_MOVING); }
	bool IsHangMode() const;
	void Jump(float fJumpingSpeed, bool withTumbling = true);

protected:
	enum Flags
	{
		ACF_UPDATED_DIR	= (1<<0),
		ACF_ENABLE_RUN	= (1<<1),
		ACF_IS_MOVING	= (1<<2),
		ACF_IS_INAIR	= (1<<3),
		ACF_DIRTY_TARGET_POS = (1<<4),
		ACF_HAS_TUMBLING = (1<<5),
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
	A3DVECTOR3 GetTargetDir() const { return IsHangMode()? APhysXConverter::N2A_Vector3(m_TargetDirForHang) : m_TargetDir; }
	NxVec3	   GetTargetDirNx() const { return IsHangMode()? m_TargetDirForHang : APhysXConverter::A2N_Vector3(m_TargetDir); }
	A3DVECTOR3 GetCurrentPos() const { return m_CurPos; }
	float GetSpeed() const { return m_Speed; }
	void UpdateDirForHang(const NxVec3& dir) { m_TargetDirForHang = dir; }

	virtual void UpdateInfoFromCC();

private:
	virtual void OnPlayAction(const unsigned long deltaTime, const bool FirstAction);
	virtual void CreateAPhysxCCPost();
	virtual void OnFinishCurrentAction();
	bool IsNearEnoughToStop(const A3DVECTOR3& posCurrent, A3DVECTOR3* pStep = 0);
	A3DVECTOR3 CalculatStep(float deltaTimeSec);

private:
	int		 m_Flags;
	float	 m_Speed;
	ActState m_ActState;

	A3DVECTOR3 m_LastPos;
	A3DVECTOR3 m_CurPos;
	A3DVECTOR3 m_TargetPos;
	A3DVECTOR3 m_CurVerVel;

	float		m_JumpTakeoffSpeed;
	A3DVECTOR3	m_JumpTakeoffDir;
	
	A3DVECTOR3 m_CurFaceDir;
	A3DVECTOR3 m_TargetDir;
	float m_fTurnT;
	float m_fTheta;

	float m_HalfHeight;
	float m_fTumbleT;
	NxVec3 m_TargetDirForHang;
};

class ACNPCWalk : public ACWalk
{
public:
	ACNPCWalk(CPhysXObjDynamic& objDyn, const bool isFixRange = true, const bool isXAxisRange = false)
		: ACWalk(objDyn), bFixRange(isFixRange), bXAxisRange(isXAxisRange), nStandCount(0) {}

	virtual ID GetTypeID() const;
	virtual IActionPlayerBase* Clone() const { return new ACNPCWalk(*this); }

private:
	virtual void OnPlayAction(const unsigned long deltaTime, const bool FirstAction);

private:
	bool bFixRange;
	bool bXAxisRange;
	int nStandCount;
};

class ACMAWalk : public ACWalk
{
public:
	ACMAWalk(CPhysXObjDynamic& objDyn) : ACWalk(objDyn) {}
	void SetMoveFlag(const int flags, const A3DVECTOR3& camDir);

	virtual ID GetTypeID() const { return AP_MAIN_ACTOR; }
	virtual IActionPlayerBase* Clone() const { return new ACMAWalk(*this); }

public:
	enum MoveFlag
	{
		MOVE_FORWARD     = (1<<0),
		MOVE_BACKWARD    = (1<<1),
		MOVE_LEFTSTRAFE	 = (1<<2),
		MOVE_RIGHTSTRAFE = (1<<3),
		MOVE_UP			 = (1<<4),
		MOVE_DOWN		 = (1<<5),
		MOVE_KEY_LEFT	 = (1<<6),
		MOVE_KEY_RIGHT	 = (1<<7),
	};

private:
	int		m_moveFlag;
};

#endif
