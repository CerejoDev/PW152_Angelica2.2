/*
* FILE: Recover.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2012/04/28
*
* HISTORY: 
*
* Copyright (c) 2012 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSXLAB_RECOVER_H_
#define _APHYSXLAB_RECOVER_H_

class ModelBreakReport: public APhysXUserBreakReport
{
public:
	ModelBreakReport();
	void TickBroken(float dtSec);
	void ClearAllBroken();

	void EnableRecover() { m_bEnable = true; }
	void DisableRecover() { m_bEnable = false; }

	void SetRecoverInterval(float t, bool bIsFirstBreak);
	float GetRecoverInterval(bool bIsFirstBreak) const;

	virtual void OnBreakNotify(APhysXBreakable* pBreakable) {}

	virtual void OnFirstBreakNotify(APhysXBreakable* pBreakable);
	virtual void OnAllBreakNotify(APhysXBreakable* pBreakable);

private:
	struct BrokenInfo;
	void OnRecover(const BrokenInfo& bi);

private:
	struct BrokenInfo
	{
		BrokenInfo();
		CPhysXObjDynamic* m_pDynObj;
		A3DMATRIX4 m_pose;
		float m_CountFirstB;
		float m_CountAllB;
	};

	typedef std::vector<BrokenInfo>		arrBrokens;

	bool m_bEnable;
	float m_maxFirstBreak;
	float m_maxAllBreak;
	arrBrokens m_objBroken;
};

#endif