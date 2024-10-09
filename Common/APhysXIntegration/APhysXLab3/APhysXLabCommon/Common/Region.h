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

class CRegion : public IObjBase, public IPropObjBase
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
	CRegion(int UID = -1);
	virtual ~CRegion();
	CRegion& operator= (const CRegion& rhs);

	ShapeType GetShapeType() const;
	void SetShapeType(ShapeType st);

	A3DVECTOR3 GetVolume(bool withScaled = false) const;
	bool SetVolume(const A3DVECTOR3& newVolume, bool withScaled = false);

	int GetFilterMask() const { return m_FilterMask; }
	// the type of CRegion was filtered out internally.
	void SetFilterMask(int filter);

	bool TestOverlap(const NxBox& obb) const;
	bool TestOverlap(const NxBounds3& aabb) const;
	int  TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec) const;
	bool TestOverlap(const IObjBase& obj) const;

	int GetNbHoldOnObjects() const { return m_HoldOnObjs.GetSize(); }
	IObjBase* GetHoldOnObject(int idx);
	void UpdateHoldOnObjsEnter(const APtrArray<IObjBase*>& arrEnterObjs);
	void UpdateHoldOnObjsLeave(const APtrArray<IObjBase*>& arrLeaveObjs);
	void UpdateHoldOnObjsLeave(const APtrArray<IObjBase*>& arrKeepObjs, APtrArray<IObjBase*>& outLeaveObjs);

	virtual bool GetObjAABB(NxBounds3& outObjAABB) const;
	virtual bool SaveToFile(NxStream& nxStream);
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion);
	virtual bool OnSendToPropsGrid();
	virtual bool OnPropertyChanged(PropItem& prop);
	virtual void OnUpdatePropsGrid();

protected:
	CRegion(const CRegion& rhs);
	virtual CRegion* CreateMyselfInstance(bool snapValues) const;
	virtual void EnterRuntimePreNotify(APhysXScene& aPhysXScene);
	virtual void LeaveRuntimePostNotify();
	virtual void OnTick(float dt);
	virtual bool OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime) { return IObjBase::OnDoWakeUpTask(eu, pPSToRuntime); }

	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene) { return true; }
	virtual void OnLeaveRuntime() {}
	virtual bool OnResetPose() { return true; }
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos) {}
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) {}
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose) {}
	virtual bool OnSetScale(const float& scale);

private:
	virtual CRegion* NewObjAndCloneData() const;

	virtual void OnDoSleepTask() {}
	virtual bool OnRayTraceMyself(const PhysRay& ray, PhysRayTraceHit& hitInfo) const;
	virtual void BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor);

private:
	int m_FilterMask;
	IShapeImp*			 m_pShapeImp;
	APtrArray<IObjBase*> m_HoldOnObjs;
};

#endif
