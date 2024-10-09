/*
 * FILE: Region.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 20011/05/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_REGION_H_
#define _APHYSXLAB_REGION_H_

#include "IObjBase.h"
class IShapeImp;

class Region : public IObjBase, public IPropObjBase
{
public:
	static APhysXU32 GetVersion() { return 0xAA000001; }

	enum ShapeType
	{
		ST_BOX		= 1,
		ST_SPHERE	= 2,		
		ST_CAPSULE	= 3,		
	};
	static const TCHAR* GetShapeTypeText(const ShapeType st);
	static bool GetShapeType(const TCHAR* pStr, ShapeType& outRT);

	static float GetDefRadius() { return gDefRadius; }
	static bool SetDefRadius(float newR);

private:
	static float gDefRadius;

public:
	Region(int UID = -1);
	~Region();
	Region& operator= (const Region& rhs);

	ShapeType GetShapeType() const;
	void SetShapeType(ShapeType st);

	// without scale factor
	A3DVECTOR3 GetVolume() const;
	bool SetVolume(const A3DVECTOR3& newVolume);

	bool TestOverlap(const A3DOBB& obb) const;
	bool TestOverlap(const A3DAABB& aabb) const;
	int  TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec) const;

	virtual void ToDie(const bool IsAsync = false);
	virtual bool Revive(IEngineUtility& eu);

	virtual bool GetOBB(A3DOBB& outOBB) const;
	virtual bool GetAABB(A3DAABB& outAABB) const;

	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo) const;
	virtual void Render(A3DViewport* pViewport, bool bDoVisCheck /*= true*/) const {}
	virtual void RenderExtraData(const IRenderUtility& renderUtil) const;

	virtual bool SaveToFile(NxStream& nxStream);
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion);
	virtual bool OnSendToPropsGrid(CBCGPPropList& lstProp);
	virtual bool OnPropertyChanged(const CBCGPProp& prop);
	virtual void OnUpdatePropsGrid(CBCGPPropList& lstProp);

protected:
	Region(const Region& rhs);
	virtual Region* CreateMyselfInstance(bool snapValues) const;

	virtual void ApplyPose(bool bRuntimePose) {}
	virtual bool OnResetPose() { return true; }
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos) {}
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) {}
	virtual bool OnGetPose(A3DMATRIX4& matOutPose) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose) {}
	virtual bool OnSetScale(const float& scale) { return true; }

private:
	IShapeImp* m_pShapeImp;
};

#endif
