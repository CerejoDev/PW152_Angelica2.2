/*
* FILE: MainActor.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/03/30
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSXLAB_MAINACTOR_H_
#define _APHYSXLAB_MAINACTOR_H_

#include "PhysXObjDynamic.h"
#include "ActionPlayerBase.h"

class CMainActor
{ 
public:
	CMainActor(CPhysXObjDynamic& obj);

	A3DVECTOR3 GetPos() const;
	const TCHAR* GetCCTypeText() const;
	APhysXDynCharCtrler* GetAPhysXDynCC() const;
/*	APhysXCharacterController* GetAPhysXCC() const;
	APhysXCCBase* GetAPhysXCCBase() const;
	BrushcharCtrler* GetBrushCC() const;
	APhysXNxCharCtrler* GetAPhysXNxCC() const;*/

	void SetLButtonDown();
	void SetLButtonUp();
	void ScanControlKey(const A3DVECTOR3& camDir)  const;
	void Render(A3DWireCollector& wc, bool bIsDebugRender, const CMainActor* pCurrentMA = 0);

	A3DVECTOR3 GetDeltaMove() const;
	void WalkTo(const A3DVECTOR3& pos) const;
	bool IsInAir() const;
	bool IsKeepWalking();
	bool IsRunEnable() const;
	void EnableRun(const bool bEnable) const;
	void Jump(float fJumpSpeed) const;

	void UpdateTargetPT(const int x, const int y);
	bool GetTargetPT(int& xOut, int& yOut);

//	void Attack(IPhysXObjBase& objTarget, const A3DVECTOR3& vAttackPos, float fForceMag, NxActor* pHitActor = 0);
//	void UpdateAura() const;
//	void TickAura() const;

	void SwitchCCType(CAPhysXCCMgr::CCType ct);
	void UpdateDynDes() const; 
//	friend class SceneExtraConfig;

private:
	enum Flags
	{
		MAF_LBTN_PRESSED		= (1<<0), 
		MAF_LBTN_CLICK			= (1<<1),
 	};

private:
	IAPWalk* GetAPWalk() const;
	bool ReadFlag(const Flags flag) const { return (m_flags & flag)? true : false; }
	void ClearFlag(const Flags flag) { m_flags &= ~flag; }
	void RaiseFlag(const Flags flag) { m_flags |= flag; }

private:
	int		m_flags;
	DWORD	m_dwLBtnDownTime;
	APointI m_ptTarget; 

	CPhysXObjDynamic& m_objModel;
};

#endif