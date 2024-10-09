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

#ifdef USING_BRUSH_MAN
	#include "ELBrushBuilding.h"
	#include "EC_BrushMan.h"
#endif

class CBrushesObj;

class IDynObjStateReceiver
{
public:
	virtual void OnRegisterDynBrushObj(CBrushesObj* pObj) = 0;
	virtual void OnUnRegisterDynBrushObj(CBrushesObj* pObj) = 0;
};

class CBrushesObjMgr
{
public:
	typedef CHBasedCD::CCapsule		 CCapsule;
	typedef CHBasedCD::CConvexBrush  CConvexBrush;

	CBrushesObjMgr();
	virtual ~CBrushesObjMgr();

public:
	static CBrushesObjMgr* GetInstance()
	{
		static CBrushesObjMgr Mgr;
		return &Mgr;
	}
	void AddBrushesObj(CBrushesObj* pBrushObj);
	bool RemoveBrushesObj(CBrushesObj* pBrushObj);

	void Render(A3DFlatCollector* pFC, DWORD dwFColor=0xa0ffff00);

	bool CapsuleTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const CCapsule& capsule, A3DVECTOR3 *pNormal, bool& bStartSolid, CConvexBrush** ppSinkedBrush = NULL);
	bool AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, const A3DAABB& aabb, A3DVECTOR3 *pNormal, bool& bStartSolid, CConvexBrush** ppSinkedBrush = NULL);
	bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float& fFraction, A3DVECTOR3 *pNormal, bool& bStartSolid, CConvexBrush** ppSinkedBrush = NULL);

	void SkipMovingObjCollision(bool bSkip);

	bool HasDynamicObjs() const{ return (m_pDynObjs.size() > 0) ? true : false;}
	const abase::vector<CBrushesObj*>& GetAllDynamicObjs() const { return m_pDynObjs; }

	int GetSize() const { return (int)m_pBrushObjs.size(); }
	CBrushesObj* GetBrushesObj(int i) 
	{
		if (i < (int)m_pBrushObjs.size())
			return m_pBrushObjs[i]; 
		return NULL;
	}

	void Release();

	void RegisterDynObjReceiver(IDynObjStateReceiver* pDynObjReceiver);
	void UnRegisterDynObjReceiver(IDynObjStateReceiver* pDynObjReceiver);

protected:
	friend class CBrushesObj;

	void RegisterDynBrushObj(CBrushesObj* pObj);
	bool UnRegisterDynBrushObj(CBrushesObj* pObj);
	
	abase::vector<CBrushesObj*> m_pBrushObjs; //brush objects (static & dynamic objects)

	abase::vector<CBrushesObj*> m_pDynObjs;//dynamic object
	abase::vector<IDynObjStateReceiver*> m_DynObjReceiver;

};



#endif // !defined(AFX_CONVEXBRUSHMGR_H__8AEDA2C2_977B_4822_97F7_98389109E8B5__INCLUDED_)
