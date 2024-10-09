/*
 * FILE: PropRgnObjImp.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_PROPRGNOBJIMP_H_
#define _APHYSXLAB_PROPRGNOBJIMP_H_

#include "Region.h"

class OLReport : public NxUserEntityReport<NxShape*>
{
public:
	virtual bool onEvent(NxU32 nbEntities, NxShape** entities);

	OLReport();
	void SetReceiveArray(APtrArray<NxActor*>* pArray);
	int Clear();

private:
	void Init();

private:
	static void* const gMyMarkID;

	int m_beginIdx;
	APtrArray<NxActor*>* m_pOutVec;
	APtrArray<void*> m_UserDataBK;
};

class IShapeImp
{
public:
	typedef Region::ShapeType		ShapeType;

	static bool GetDefVolumeForTheType(ShapeType stNew, A3DVECTOR3& outVol);
	static bool TestOverlap(const NxBox& obb, const NxSphere& sphere);
	static bool PtToPtDistanceIsInRange(const NxVec3& p0, const NxVec3& p1, float distSquared);

	virtual ~IShapeImp() {};
	virtual ShapeType GetShapeType() const = 0;
	virtual bool GetProperVolumeForNewType(ShapeType stNew, A3DVECTOR3& outVol) const = 0;

	// Note: without scale factor
	virtual void GetVolume(A3DVECTOR3& outVolume) const = 0;
	virtual bool SetVolume(const A3DVECTOR3& newVolume) = 0;

	virtual void Draw(A3DWireCollector& wc, int color, const NxMat34& mat, float scale) const = 0;
	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const A3DMATRIX4& matPose, float scale) const = 0;

	virtual bool TestOverlap(const NxBox& obb, const NxMat34& mat, float scale) const = 0;
	virtual int  TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec, const NxMat34& mat, float scale) const = 0;

protected:
	static const NxVec3 gZero;
	mutable OLReport m_cbReport;
};

class ShapeBoxImp : public IShapeImp
{
public:
	ShapeBoxImp();
	virtual ShapeType GetShapeType() const { return Region::ST_BOX; }
	virtual bool GetProperVolumeForNewType(ShapeType stNew, A3DVECTOR3& outVol) const;
	
	virtual void GetVolume(A3DVECTOR3& outVolume) const;
	virtual bool SetVolume(const A3DVECTOR3& newVolume);
	
	virtual void Draw(A3DWireCollector& wc, int color, const NxMat34& mat, float scale) const;
	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const A3DMATRIX4& matPose, float scale) const;
	
	virtual bool TestOverlap(const NxBox& obb, const NxMat34& mat, float scale) const;
	virtual int  TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec, const NxMat34& mat, float scale) const;

private:
	APhysXBoxShapeDesc m_STBox;
};

class ShapeSphereImp : public IShapeImp
{
public:
	ShapeSphereImp();
	virtual ShapeType GetShapeType() const { return Region::ST_SPHERE; }
	virtual bool GetProperVolumeForNewType(ShapeType stNew, A3DVECTOR3& outVol) const;
	
	virtual void GetVolume(A3DVECTOR3& outVolume) const;
	virtual bool SetVolume(const A3DVECTOR3& newVolume);
	
	virtual void Draw(A3DWireCollector& wc, int color, const NxMat34& mat, float scale) const;
	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const A3DMATRIX4& matPose, float scale) const;

	virtual bool TestOverlap(const NxBox& obb, const NxMat34& mat, float scale) const;
	virtual int  TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec, const NxMat34& mat, float scale) const;

private:
	APhysXSphereShapeDesc m_STSphere;
};

class ShapeCapsuleImp : public IShapeImp
{
public:
	ShapeCapsuleImp();
	virtual ShapeType GetShapeType() const { return Region::ST_CAPSULE; }
	virtual bool GetProperVolumeForNewType(ShapeType stNew, A3DVECTOR3& outVol) const;
	
	virtual void GetVolume(A3DVECTOR3& outVolume) const;
	virtual bool SetVolume(const A3DVECTOR3& newVolume);
	
	virtual void Draw(A3DWireCollector& wc, int color, const NxMat34& mat, float scale) const;
	virtual bool RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const A3DMATRIX4& matPose, float scale) const;

	virtual bool TestOverlap(const NxBox& obb, const NxMat34& mat, float scale) const;
	virtual int  TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec, const NxMat34& mat, float scale) const;

private:
	// return true if the point is in the capsule body(or on surface).
	bool IsCapsuleContainsPoint(const NxCapsule& capsule, const NxVec3& p, bool& outOnSurface) const;
	void ComputeNormalFromPoint(const NxCapsule& capsule, const NxVec3& p, NxVec3& outNormal) const;
	bool BoxCylinderIntersect(const NxBox& obb, const NxMat34& cylinderPose, float r, float h) const;
	bool PlaneCylinderIntersect(const NxPlane& plane, const NxVec3& p0, const NxVec3& p1, float r) const;
	bool PlaneBoxIntersect(const NxPlane& plane, const NxVec3* pBOXVtxs, int nPT) const;

private:
	APhysXCapsuleShapeDesc m_STCapsule;
};

#endif