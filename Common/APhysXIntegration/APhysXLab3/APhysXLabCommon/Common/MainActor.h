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

#include "ActionPlayerBase.h"

class IRoleActor
{
public:
	IRoleActor(ApxActorBase& aab) : m_actorBase(aab), m_objModel(*aab.GetObjDyn()){}

	CPhysXObjDynamic* GetDynObj() const { return &m_objModel;}
	ApxActorBase* GetActorBase() const { return &m_actorBase; }
	void TickMA(float dtSec) const{ m_actorBase.Tick(dtSec); }
	void Render(A3DWireCollector& wc, bool bIsDebugRender);

protected:
	ApxActorBase&		m_actorBase;
	CPhysXObjDynamic&	m_objModel;
};

class CMainActor
{ 
public:
	CMainActor(ApxActorBase& aab);

	A3DVECTOR3 GetPos() const;
	const TCHAR* GetCCTypeText() const;
	CPhysXObjDynamic* GetDynObj() const;
	ApxActorBase* GetActorBase() const;

	void SetLButtonDown();
	void SetLButtonUp();
	void Render(A3DWireCollector& wc, bool bIsDebugRender, const CMainActor* pCurrentMA = 0);
	void TickMA(float dtSec) const;  

	A3DVECTOR3 GetDeltaMove() const;
	void WalkTo(const A3DVECTOR3& pos) const;
	bool IsInAir() const;
	bool IsKeepWalking();
	bool IsRunEnable() const;
	void EnableRun(const bool bEnable) const;
	
	void Jump(float fJumpSpeed) const;
	bool HangEnd() const;

	void UpdateTargetPT(const int x, const int y);
	bool GetTargetPT(int& xOut, int& yOut);

//	void UpdateAura() const;
//	void TickAura() const;

	void SwitchCCType(CAPhysXCCMgr::CCType ct);
//	friend class SceneExtraConfig;

private:
	enum Flags
	{
		MAF_LBTN_PRESSED		= (1<<0), 
		MAF_LBTN_CLICK			= (1<<1),
 	};

private:
	APhysXDynCharCtrler* GetAPhysXDynCC() const;
	IAPWalk* GetAPWalk() const;
	bool ReadFlag(const Flags flag) const { return (m_flags & flag)? true : false; }
	void ClearFlag(const Flags flag) { m_flags &= ~flag; }
	void RaiseFlag(const Flags flag) { m_flags |= flag; }

private:
	int		m_flags;
	DWORD	m_dwLBtnDownTime;
	APointI m_ptTarget; 

	ApxActorBase&		m_actorBase;
	CPhysXObjDynamic&	m_objModel;
};

#endif