// BrushesObj.h: interface for the CBrushesObj class.
//
//////////////////////////////////////////////////////////////////////

/*
 * FILE: BrushesObj.h
 *
 * DESCRIPTION: Brush objects corresponding to a file(.chf) 
 *
 * CREATED BY: YangXiao, 2010/10/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#if !defined(AFX_BRUSHESOBJ_H__BF1BBC6F_5A43_422A_ABB9_C537D1118D9D__INCLUDED_)
#define AFX_BRUSHESOBJ_H__BF1BBC6F_5A43_422A_ABB9_C537D1118D9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define USING_BRUSH_MAN
#ifdef USING_BRUSH_MAN
	#include "ELBrushBuilding.h"
	#include "EC_BrushMan.h"
#endif

#include "ConvexBrush.h"
#include "ChbEntity.h"
#include "ConvexHullDataSet.h"
#include <AString.h>

class IPhysXObjBase;
class CPhysXObjMgr;
class BrushObjAttr;

class CBrushesObj  
{
public:
	CBrushesObj();
	virtual ~CBrushesObj();

	bool Init(IPhysXObjBase* pPhysxObj);

	void Tick(int deltaTime);

	void Render(A3DFlatCollector* pFC, DWORD dwFColor=0xa0ffff00);

	bool CapsuleTrace(CHBasedCD::CapusuleBrushTraceInfo* pInfo);

	bool AABBTrace(CHBasedCD::AABBBrushTraceInfo2* pInfo);

	bool RayTrace(CHBasedCD::RayBrushTraceInfo* pInfo);

	const A3DMATRIX4& GetTrans();//make sure model is loaded before calling

	bool IsLoad() const { return m_bLoad; }

	void SkipCollisionDetection(bool bSkip = true);

	void SetMoving(bool bMove) { if(m_pDynAttr) m_isMoveing = bMove; }
	bool GetMoving() const { return m_isMoveing; }
	
	void SetPos(const A3DVECTOR3& vPos);
	A3DVECTOR3 GetPos() const;

	void AddBrushAttr(BrushObjAttr* pBrushAttr);
	bool IsDynObj() const { return m_pDynAttr != NULL; }

	//get the binding physx object
	IPhysXObjBase* GetPhysXObj() { return m_pPhysxObj; }
	BrushObjAttr* GetBrushObjAttr() { return m_pDynAttr; }

	void UpdateTrans() { DynamicLoad(m_strFile); }

protected:

	bool DynamicLoad(const AString& chf);

	void Release();

	AString m_strFile;//Brush file .chb
	bool m_bLoad;//is it loaded

	BrushObjAttr *m_pDynAttr;

	IPhysXObjBase* m_pPhysxObj;

	CHBasedCD::CChbEntity *m_pChbEntity; //collision entity
	CHBasedCD::CConvexHullDataSet* m_pConvexDataSet;//.chb file

	bool m_isMoveing;

	A3DAABB m_BrushAABBWorld;//世界坐标系下的AABB

	A3DMATRIX4 m_trans; //旋转平移
	
#ifdef USING_BRUSH_MAN
	CELBrushBuilding m_building;
#endif

};

#endif // !defined(AFX_BRUSHESOBJ_H__BF1BBC6F_5A43_422A_ABB9_C537D1118D9D__INCLUDED_)
