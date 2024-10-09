/*
* FILE: MainActor.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2009/07/23
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSXLAB_MAINACTOR_H_
#define _APHYSXLAB_MAINACTOR_H_

#include "PhysXObjDynamic.h"
#include "ActionPlayerBase.h"

class CMainActor
{ 
public:
	CMainActor(CPhysXObjDynamic& obj) : m_flags(0), m_objModel(obj)
	{
		assert(true == obj.ReadPeopleInfo(IPhysXObjBase::OBPI_IS_MAIN_ACTOR));
	}

	A3DVECTOR3 GetPos() const { return m_objModel.GetPos(); }
	APhysXCharacterController* GetAPhysXCC() const;
	APhysXDynCharCtrler* GetAPhysXDynCC() const;
	APhysXCCBase* GetAPhysXCCBase() const;
	BrushcharCtrler* GetBrushCC() const;
	APhysXNxCharCtrler* GetAPhysXNxCC() const;

	void SetLButtonDown();
	void SetLButtonUp();
	void ScanControlKey(const A3DVECTOR3& camDir)  const;
	
	void Render(A3DWireCollector& wc, const CMainActor* pCurrentMA = 0);
	void ChangeCCVolume(const bool isIncrease);

	A3DVECTOR3 GetDeltaMove() const;
	void WalkTo(const A3DVECTOR3& pos) const;
	bool IsInAir() const;
	bool IsKeepWalking();
	bool IsRunEnable() const;
	void EnableRun(const bool bEnable) const;
	void Jump(float fJumpSpeed) const;

	void UpdateTargetPT(const int x, const int y);
	bool GetTargetPT(int& xOut, int& yOut);

	void Attack(IPhysXObjBase& objTarget, const A3DVECTOR3& vAttackPos, float fForceMag, NxActor* pHitActor = 0);
	void UpdateAura() const;
	void TickAura() const;

	//«–ªªCC, ccindex = -1,±Ì æÀ≥–Ú«–ªª
	void SwtichCC(int ccindex = -1);
	const A3DAABB& GetModelAABB() const;
	void UpdateDynDes() const; 
	friend class SceneExtraConfig;

private:
	enum Flags
	{
		MAF_LBTN_PRESSED		= (1<<0), 
		MAF_LBTN_CLICK			= (1<<1),
		MAF_TARGET_PT_UPDATED	= (1<<2),
 	};

private:
	bool ReadFlag(const Flags flag) const { return (m_flags & flag)? true : false; }
	void ClearFlag(const Flags flag) { m_flags &= ~flag; }
	void RaiseFlag(const Flags flag) { m_flags |= flag; }

	ACWalk* GetAPWalk() const;

private:
	int		m_flags;
	DWORD	m_dwLBtnDownTime;
	APointI m_ptTarget; 

	CPhysXObjDynamic& m_objModel;
};

#endif