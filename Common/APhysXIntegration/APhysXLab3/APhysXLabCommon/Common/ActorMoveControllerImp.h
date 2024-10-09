/*
 * FILE: ActorMoveControllerImp.h
 *
 * DESCRIPTION: 
 *
 * YX 2011.7.21
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

//   ------ Deprecated! "ActorMoveCtrlImp.h" instead of this ------
#pragma once

#include "ApxActorBase.h"
#include "BrushcharCtrler.h"

class  GroundMoveControllerBrush: public ApxActorGroundMoveController
{
public:
	GroundMoveControllerBrush();
	virtual ~GroundMoveControllerBrush();

public:
	virtual bool IsInAir() const;
	virtual bool CheckVolumeValid(const NxVec3& vPos, const NxVec3& vExtents);
	virtual void GetGroundPos(NxVec3& vPos) const;

	virtual NxVec3 GetVelocity() const;
	
protected:
	
	virtual void PreTickMove(float dt);
	virtual void TickMove(float dt);
	virtual void DoJump(float fUpSpeed);
	virtual float GetDistToGround();

	virtual void OnSetPos();
	virtual void OnSetExtents();
	virtual void OnSetMoveDir();
	virtual void OnSetHeadDir();
	virtual void OnSetMoveSpeed();
	virtual void OnSetGravity();

public:
	BrushcharCtrler* m_pBrushCC;
};
