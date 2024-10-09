/*
 * FILE: ActorMoveCtrlImp.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/08/02
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_ACTORMOVECTRLIMP_H_
#define _APHYSXLAB_ACTORMOVECTRLIMP_H_

#include "ApxActorBase.h"

class CAPhysXCCMgr;

class ActorGroundMoveCtrlImp : public ApxActorGroundMoveController
{
public:
	static A3DTerrain2* gpTerrain;
	ActorGroundMoveCtrlImp(CAPhysXCCMgr* pCCMgr);
	CAPhysXCCMgr* GetCCMgr() const { return m_pCCMgr; }

	virtual NxVec3 GetVelocity() const;
	virtual bool CheckVolumeValid(const NxVec3& vPos, const NxVec3& vExtents);

	virtual bool IsPushingObj(IPhysXObjBase* pObj) const;
	virtual bool IsInPush() const { return m_PresentIsPushing; }
	virtual bool IsInAir() const;
	virtual void GetGroundPos(NxVec3& vPos) const;

	virtual void PrepareTick();

	virtual bool IsBlocked() const { return m_bBlocked; }
	virtual void SetBlocked(bool bBlocked) { m_bBlocked = bBlocked; }

protected:
	enum Flags
	{
//		ACF_UPDATED_DIR	= (1<<0),
//		ACF_ENABLE_RUN	= (1<<1),
//		ACF_IS_MOVING	= (1<<2),
		ACF_IS_INAIR	= (1<<3),
//		ACF_DIRTY_TARGET_POS = (1<<4),
//		ACF_HAS_TUMBLING	 = (1<<5),
//		ACF_HAS_TEMP_SPEED	 = (1<<6),
//		ACF_HAS_NEW_FACEDIR	 = (1<<7),
		ACF_HAS_NEW_JUMP	 = (1<<8),
//		ACF_ONGROUND_STOP	 = (1<<9),
	};

	virtual void PreTickMove(float dt);
	virtual void TickMove(float dt);
	virtual void PostTickMove(float dt);

	virtual void DoJump(float fUpSpeed);
	virtual float GetDistToGround();

	virtual bool DoStartSwing(const NxVec3& vDir);
	virtual void DoEndSwing();

	virtual void OnSetPos();
	virtual void OnSetExtents();
	virtual void OnSetMoveDir();
	virtual void OnSetHeadDir();
	virtual void OnSetMoveSpeed();
	virtual void OnSetGravity();

	virtual void OnPushObj(IPhysXObjBase* pObj);

private:
	void GetFootPos(NxVec3& posFoot, const NxVec3* pCenterPos = 0) const;

private:
	CAPhysXCCMgr*	m_pCCMgr;
	NxVec3 m_CurVerticalVel;

	bool m_LastFrameIsPushing;
	bool m_PresentIsPushing;
	APtrArray<IPhysXObjBase*> m_PushingObjs;

	bool m_bBlocked;
};

#endif