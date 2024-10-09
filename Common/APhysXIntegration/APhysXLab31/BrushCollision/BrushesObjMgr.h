// ConvexBrushMgr.h: interface for the CConvexBrushMgr class.
//class for manage the ConvexBrush 
//////////////////////////////////////////////////////////////////////

/*
 * FILE: ConvexBrushMgr.h
 *
 * DESCRIPTION: Manager of CBrushesObj 
 *
 * CREATED BY: YangXiao, 2010/10/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#if !defined(AFX_CONVEXBRUSHMGR_H__8AEDA2C2_977B_4822_97F7_98389109E8B5__INCLUDED_)
#define AFX_CONVEXBRUSHMGR_H__8AEDA2C2_977B_4822_97F7_98389109E8B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Capsule.h"
#include "ConvexBrush.h"
#include "ReportInterface.h"

#ifdef USING_BRUSH_MAN
	#include "ELBrushBuilding.h"
	#include "EC_BrushMan.h"
#endif

class CPhysXObjMgr;
class IPhysXObjBase;
class CBrushesObj;

class CBrushesObjMgr : public IObjStateReport
{
public:
	typedef CHBasedCD::CCapsule		 CCapsule;
	typedef CHBasedCD::CConvexBrush  CConvexBrush;

	CBrushesObjMgr();
	virtual ~CBrushesObjMgr();

public:
	virtual void OnNotifyBorn(const IPhysXObjBase& obj);
	virtual void OnNotifyDie(const IPhysXObjBase& obj);
	virtual void OnNotifyWakeUp(const IPhysXObjBase& obj);
	virtual void OnNotifySleep(const IPhysXObjBase& obj);

public:
	void Tick(int deltaTime);
	void Render(A3DFlatCollector* pFC, DWORD dwFColor=0xa0ffff00);

	bool CapsuleTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const CCapsule& capsule, A3DVECTOR3 *pNormal, bool& bStartSolid, CConvexBrush** ppSinkedBrush = NULL);
	bool AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const A3DAABB& aabb, A3DVECTOR3 *pNormal, bool& bStartSolid, CConvexBrush** ppSinkedBrush = NULL);
	bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3 *pNormal, bool& bStartSolid, CConvexBrush** ppSinkedBrush = NULL);

	void SetMoving(bool bMove);
	bool GetMoving() const { return m_isMoving; }

	void SkipMovingObjCollision(bool bSkip);

	bool HasDynamicObjs() const;
	void GetAllDynamicObjs(abase::vector<CBrushesObj*>& outObjs) const;

	int GetSize() const { return (int)m_pBrushObjs.size(); }
	CBrushesObj* GetBrushesObj(int i) 
	{
		if (i < (int)m_pBrushObjs.size())
			return m_pBrushObjs[i]; 
		return NULL;
	}

	bool AddBrushesObj(IPhysXObjBase* pPhysxObj);
	bool ReleaseBrushesObj(IPhysXObjBase* pPhysxObj);
	void Release();

	//开始进入play模式前，更新Object
	void PrepareStart();

protected:
	
	abase::vector<CBrushesObj*> m_pBrushObjs; 

	bool m_isMoving;
};

#ifdef USING_BRUSH_MAN
	extern CECBrushMan g_brushMan;
#endif


#endif // !defined(AFX_CONVEXBRUSHMGR_H__8AEDA2C2_977B_4822_97F7_98389109E8B5__INCLUDED_)
