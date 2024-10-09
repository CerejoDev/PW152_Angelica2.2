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

#ifdef _ANGELICA3
	#include "ConvexHullDataSet.h"
#endif

#include <AString.h>

class IPhysXObjBase;
class BrushObjAttr;

class CBrushesObj  
{
public:
	CBrushesObj();
	virtual ~CBrushesObj();

	void Render(A3DFlatCollector* pFC, int dwFColor=0xa0ffff00);
	
	bool CapsuleTrace(CHBasedCD::CapusuleBrushTraceInfo* pInfo);

	bool AABBTrace(CHBasedCD::AABBBrushTraceInfo2* pInfo);

	bool RayTrace(CHBasedCD::RayBrushTraceInfo* pInfo);

	const A3DMATRIX4& GetTrans();

	bool IsLoad() const { return m_bLoad; }

	void SkipCollisionDetection(bool bSkip = true);
	
	void SetPos(const A3DVECTOR3& vPos);
	A3DVECTOR3 GetPos() const;

	bool IsDynObj() const;

	bool Load(const A3DMATRIX4& rotTrans, float fScale);//load brush file(.chb)

	void Release();

	//set rot and trans matrix, without scale
	void SetRotTrans(const A3DMATRIX4& rotTrans);
	void SetPosition(const A3DVECTOR3& vPos);
	void SetScale(float fScale);
	float GetScale() const { return m_fScale; }
	void SetStrFile(const char* strFile);
	AString GetStrFile() const { return m_strFile; } 
	//update brush aabb according to current matrix, do not call it for dynamic object
	void UpdateBrushAABBWorld();

public:
	void* m_UserData;

protected:

	enum Flag
	{
		Dirty_AABB = 1<<0,
		Dynamic_Obj = 1<<1,
	};

	bool ReadFlag(DWORD flag) const { return (flag & m_dwFlag) ? true : false; }
	void RaiseFlag(DWORD flag) { m_dwFlag |= flag; }
	void ClearFlag(DWORD flag) { m_dwFlag &= ~flag; }

	AString m_strFile;//Brush file .chb
	bool m_bLoad;//is it loaded

	CHBasedCD::CChbEntity *m_pChbEntity; //collision entity

#ifdef _ANGELICA3
	CHBasedCD::CConvexHullDataSet* m_pConvexDataSet;//.chb file
#endif	

	A3DAABB m_BrushAABBWorld;//世界坐标系下的AABB

	DWORD   m_dwFlag;//internal flag

	float m_fScale;//scale
	A3DMATRIX4 m_trans; //rot and trans

};

#endif // !defined(AFX_BRUSHESOBJ_H__BF1BBC6F_5A43_422A_ABB9_C537D1118D9D__INCLUDED_)
