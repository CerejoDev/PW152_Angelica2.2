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

#include <vector.h>
#include "ConvexHullData.h"
#include "ConvexBrush.h"
#include "aabbcd.h"
#include "Capsule.h"
#include "BrushesObj.h"

#ifdef USING_BRUSH_MAN
#include "ELBrushBuilding.h"
#include "EC_BrushMan.h"
#endif

class CPhysXObjMgr;
class IPhysXObjBase;
class CBrushesObj;

using namespace CHBasedCD;


class CBrushesObjMgr  
{
public:
	CBrushesObjMgr();
	virtual ~CBrushesObjMgr();

	void Tick(int deltaTime);

	void Init(CPhysXObjMgr* physxMgr);	

	void Render(A3DFlatCollector* pFC, DWORD dwFColor=0xa0ffff00);

	bool CapsuleTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const CCapsule& capsule, A3DVECTOR3 *pNormal, bool& bStartSolid);

	bool AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const A3DAABB& aabb, A3DVECTOR3 *pNormal, bool& bStartSolid);

	bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3 *pNormal, bool& bStartSolid);

	

	void SetMoving(bool bMove);
	
	bool GetMoving() { return m_isMoving; }

	CBrushesObj* m_CollidedBrush;
	CConvexBrush* m_pSinkedBrush;

	void SkipMovingObjCollision(bool bSkip);

	//获取所有动态物体
	abase::vector<CBrushesObj*> GetDynamicObjs();

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
