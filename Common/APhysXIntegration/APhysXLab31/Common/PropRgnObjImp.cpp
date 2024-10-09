/*
 * FILE: PropRgnObjImp.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include <NxUtilLib.h> 

void* const OLReport::gMyMarkID = (void*)0x01233210;

OLReport::OLReport()
{
	Init();
}
void OLReport::Init()
{
	m_beginIdx = -1;
	m_pOutVec = 0;
	m_UserDataBK.RemoveAll(false);
}

void OLReport::SetReceiveArray(APtrArray<NxActor*>* pArray)
{
	assert(0 > m_beginIdx);
	assert(0 == m_pOutVec);
	assert(0 == m_UserDataBK.GetSize());
	m_pOutVec = pArray;
	if (0 != pArray)
		m_beginIdx = pArray->GetSize();
}

int OLReport::Clear()
{
	assert(0 <= m_beginIdx);

	NxActor* pActor = 0;
	int nSize = m_UserDataBK.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		pActor = (*m_pOutVec)[m_beginIdx + i];
		pActor->userData = m_UserDataBK[i];
	}

	Init();
	return nSize;
}

bool OLReport::onEvent(NxU32 nbEntities, NxShape** entities)
{
	void* pUserData = 0;
	NxActor* pActor = 0;
	NxShape* pShape = 0;
	for (int i = 0; i < nbEntities; ++i)
	{
		pShape = entities[i];
		if (pShape->getFlag(NX_TRIGGER_ENABLE))
			continue;
		if (NX_SHAPE_MESH == pShape->getType())
			continue;
		if (NX_SHAPE_HEIGHTFIELD == pShape->getType())
			continue;
	
		pActor = &(pShape->getActor());
		pUserData = pActor->userData;
		if (gMyMarkID == pUserData)
			continue;
		
		pActor->userData = gMyMarkID;
		m_UserDataBK.Add(pUserData);
		m_pOutVec->Add(pActor);
	}
	return true;
}

const NxVec3 IShapeImp::gZero(0.0f);
bool IShapeImp::GetDefVolumeForTheType(ShapeType stNew, A3DVECTOR3& outVol)
{
	float defR = Region::GetDefRadius();
	switch (stNew)
	{
	case Region::ST_BOX:
		{
			float halfSize = 0.707f * defR;
			outVol.Set(halfSize, halfSize, halfSize);
		}
		break;
	case Region::ST_SPHERE:
		outVol.Set(defR, 0.0f, 0.0f);
		break;
	case Region::ST_CAPSULE:
		outVol.Set(0.5f * defR, defR, 0.0f);
		break;
	default:
		assert(!"Shouldn't be here! Debug it!");
		return false;
	}
	return true;
}

bool IShapeImp::GetProperVolumeForNewType(ShapeType stNew, A3DVECTOR3& outVol) const
{
	if (GetShapeType() == stNew)
	{
		GetVolume(outVol);
		return true;
	}

	return false;
}

bool IShapeImp::TestOverlap(const NxBox& obb, const NxSphere& sphere)
{
	const float R = sphere.radius;
	const float RR = R * R;
	NxBox obbExtR(obb);
	obbExtR.extents.x += R;
	obbExtR.extents.y += R;
	obbExtR.extents.z += R;
	if (!NxBoxContainsPoint(obbExtR, sphere.center))
		return false;

	obbExtR.extents = obb.extents;
	obbExtR.extents.x += R;
	if (NxBoxContainsPoint(obbExtR, sphere.center))
		return true;

	obbExtR.extents = obb.extents;
	obbExtR.extents.y += R;
	if (NxBoxContainsPoint(obbExtR, sphere.center))
		return true;

	obbExtR.extents = obb.extents;
	obbExtR.extents.z += R;
	if (NxBoxContainsPoint(obbExtR, sphere.center))
		return true;

	const NxVec3 xAxis = obb.rot.getColumn(0);
	const NxVec3 yAxis = obb.rot.getColumn(1);
	const NxVec3 zAxis = obb.rot.getColumn(2);

	const NxVec3 bcxp = obb.center + xAxis * obb.extents.x;
	const NxVec3 bcxn = obb.center - xAxis * obb.extents.x;
	const NxVec3 bcyp = obb.center + yAxis * obb.extents.y;
	const NxVec3 bcyn = obb.center - yAxis * obb.extents.y;
	const NxVec3 bczp = obb.center + zAxis * obb.extents.z;
	const NxVec3 bczn = obb.center - zAxis * obb.extents.z;

	const NxPlane plnXP(bcxp, xAxis);
	const NxPlane plnXN(bcxn, xAxis * -1);
	const NxPlane plnYP(bcyp, yAxis);
	const NxPlane plnYN(bcyn, yAxis * -1);
	const NxPlane plnZP(bczp, zAxis);
	const NxPlane plnZN(bczn, zAxis * -1);
	const float toxp = plnXP.distance(sphere.center);
	const float toxn = plnXN.distance(sphere.center);
	const float toyp = plnYP.distance(sphere.center);
	const float toyn = plnYN.distance(sphere.center);
	const float tozp = plnZP.distance(sphere.center);
	const float tozn = plnZN.distance(sphere.center);

	//  	/* X+ plane instruction */
	//
	//           Z-           Z+
	//    ____A________B__________C___
	//	  |      :            :      |
	//	  |   0  :     1      :   1  |
	//	  |      :			  :      |	
	//	  |------+------------+------|  Y+
	//	  |      |0		     1|      |
	//   H|   3  |  real box  |   2  |D
	//	  |      |3          2|      |
	//	  |------+------------+------|  Y-
	//	  |      :            :      |
	//	  |   3  :      2     :   2  |E
	//	  |      :            :      |  extend R box
	//    |--------------------------| 
	//        G         F 
	//     
	//  1. Number 0,1,2,3 means realbox vertexs
	//	2. In the X+ plane, we should test areas A,B,C,D,E,F,G,H
	//  using different test method in every area
	//  area A: calculate the distance(sphereCenter, vertex0)  
	//  area C: calculate the distance(sphereCenter, vertex1)  
	//  area E: calculate the distance(sphereCenter, vertex2)  
	//  area G: calculate the distance(sphereCenter, vertex3)
	//  area B: project sphereCenter to Z+ plane and get the ProjPT, calculate the distance(ProjPT, vertex1)  
	//  area F: project sphereCenter to Z+ plane and get the ProjPT, calculate the distance(ProjPT, vertex2)  
	//  area D: project sphereCenter to Y- plane and get the ProjPT, calculate the distance(ProjPT, vertex2)  
	//  area H: project sphereCenter to Y- plane and get the ProjPT, calculate the distance(ProjPT, vertex3)  

	NxVec3 boxVtx, spherePT;
	//  X+
	if (!(0.0f > toxp))  
	{
		if (0.0f > toyp)
		{
			boxVtx = bcxp - yAxis * obb.extents.y + zAxis * obb.extents.z;
			if (!(0.0f > tozn))
				boxVtx = bcxp - yAxis * obb.extents.y - zAxis * obb.extents.z;
		}
		else
		{
			boxVtx = bcxp + yAxis * obb.extents.y + zAxis * obb.extents.z;
			if (!(0.0f > tozn))
				boxVtx = bcxp + yAxis * obb.extents.y - zAxis * obb.extents.z;
		}

		spherePT = sphere.center;
		if ((0.0f > tozp) && (0.0f > tozn))
			spherePT = sphere.center - plnZP.normal * tozp;
		else if ((0.0f > toyp) && (0.0f > toyn))
			spherePT = sphere.center - plnYN.normal * toyn;
		return PtToPtDistanceIsInRange(boxVtx, spherePT, RR);
	}

	//  X-
	if (!(0.0f > toxn))
	{
		if (0.0f > toyp)
		{
			boxVtx = bcxn - yAxis * obb.extents.y + zAxis * obb.extents.z;
			if (!(0.0f > tozn))
				boxVtx = bcxn - yAxis * obb.extents.y - zAxis * obb.extents.z;
		}
		else
		{
			boxVtx = bcxn + yAxis * obb.extents.y + zAxis * obb.extents.z;
			if (!(0.0f > tozn))
				boxVtx = bcxn + yAxis * obb.extents.y - zAxis * obb.extents.z;
		}

		spherePT = sphere.center;
		if ((0.0f > tozp) && (0.0f > tozn))
			spherePT = sphere.center - plnZP.normal * tozp;
		else if ((0.0f > toyp) && (0.0f > toyn))
			spherePT = sphere.center - plnYN.normal * toyn;
		return PtToPtDistanceIsInRange(boxVtx, spherePT, RR);
	}

	// right now, test the 4 edges of real box: 0-0', 1-1', 2-2' 3-3'
	// project sphereCenter to X+ plane and get the ProjPT, calculate the distance(ProjPT, vertexN)  
	if (0.0f > toyp)
	{
		boxVtx = bcxp - yAxis * obb.extents.y + zAxis * obb.extents.z;
		if (0.0f > tozp)
			boxVtx = bcxp - yAxis * obb.extents.y - zAxis * obb.extents.z;
	}
	else
	{
		boxVtx = bcxp + yAxis * obb.extents.y + zAxis * obb.extents.z;
		if (0.0f > tozp)
			boxVtx = bcxp + yAxis * obb.extents.y - zAxis * obb.extents.z;
	}
	spherePT = sphere.center - plnXP.normal * toxp;
	return PtToPtDistanceIsInRange(boxVtx, spherePT, RR);
}

bool IShapeImp::PtToPtDistanceIsInRange(const NxVec3& p0, const NxVec3& p1, float distSquared)
{
	float dist = p0.distanceSquared(p1);
	if (distSquared > dist)
		return true;
	return false;
}

ShapeBoxImp::ShapeBoxImp()
{
	A3DVECTOR3 vol(0.0f);
	IShapeImp::GetDefVolumeForTheType(Region::ST_BOX, vol);
	SetVolume(vol);
}

bool ShapeBoxImp::GetProperVolumeForNewType(ShapeType stNew, A3DVECTOR3& outVol) const
{
	if (IShapeImp::GetProperVolumeForNewType(stNew, outVol))
		return true;

	float r = (m_STBox.mDimensions.x + m_STBox.mDimensions.y + m_STBox.mDimensions.z) * 0.33333f * 1.414f;
	switch (stNew)
	{
	case Region::ST_SPHERE:
		outVol.Set(r, 0.0f, 0.0f);
		break;
	case Region::ST_CAPSULE:
		outVol.Set(0.5f * r, r, 0.0f);
		break;
	default:
		assert(!"Shouldn't be here! Debug it!");
		return false;
	}
	return true;
}

void ShapeBoxImp::GetVolume(A3DVECTOR3& outVolume) const
{
	outVolume.Set(m_STBox.mDimensions.x, m_STBox.mDimensions.y, m_STBox.mDimensions.z);
}

bool ShapeBoxImp::SetVolume(const A3DVECTOR3& newVolume)
{
	if (newVolume.x < 0.0f)
		return false;
	if (newVolume.y < 0.0f)
		return false;
	if (newVolume.z < 0.0f)
		return false;

	if (NxMath::equals(newVolume.x, 0.0f, APHYSX_FLOAT_EPSILON))
		return false;
	if (NxMath::equals(newVolume.y, 0.0f, APHYSX_FLOAT_EPSILON))
		return false;
	if (NxMath::equals(newVolume.z, 0.0f, APHYSX_FLOAT_EPSILON))
		return false;

	m_STBox.mDimensions.set(newVolume.x, newVolume.y, newVolume.z); 
	return true;
}

void ShapeBoxImp::Draw(A3DWireCollector& wc, int color, const NxMat34& mat, float scale) const
{
	NxVec3 exts = m_STBox.mDimensions * scale;
	APhysXDrawBasic::DrawBox(wc, NxBox(mat.t, exts, mat.M), color);
}

bool ShapeBoxImp::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const A3DMATRIX4& matPose, float scale) const
{
	A3DVECTOR3 Exts;
	GetVolume(Exts);
	Exts *= scale;
	
	PhysOBB rtOBB;
	rtOBB.Set(matPose.GetRow(0), matPose.GetRow(1), matPose.GetRow(2), matPose.GetRow(3), Exts);
	return RayTraceOBB(ray, rtOBB, hitInfo);
}

bool ShapeBoxImp::TestOverlap(const NxBox& obb, const NxMat34& mat, float scale) const
{
	NxVec3 nxExts = m_STBox.mDimensions * scale;
	return NxBoxBoxIntersect(nxExts, mat.t, mat.M, obb.extents, obb.center, obb.rot, true);
}

int ShapeBoxImp::TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec, const NxMat34& mat, float scale) const
{
	NxVec3 exts = m_STBox.mDimensions * scale;
	NxBox obb(mat.t, exts, mat.M);

	m_cbReport.SetReceiveArray(&outVec);
	scene.GetNxScene()->overlapOBBShapes(obb, NX_ALL_SHAPES, 0, 0, &m_cbReport, 0xffffffff, NULL, true);
	return m_cbReport.Clear();
}

ShapeSphereImp::ShapeSphereImp()
{
	A3DVECTOR3 vol(0.0f);
	IShapeImp::GetDefVolumeForTheType(Region::ST_SPHERE, vol);
	SetVolume(vol);
}

bool ShapeSphereImp::GetProperVolumeForNewType(ShapeType stNew, A3DVECTOR3& outVol) const
{
	if (IShapeImp::GetProperVolumeForNewType(stNew, outVol))
		return true;

	switch (stNew)
	{
	case Region::ST_BOX:
		{
			float halfSide = 0.707f * m_STSphere.mRadius;
			outVol.Set(halfSide, halfSide, halfSide);
		}
		break;
	case Region::ST_CAPSULE:
		outVol.Set(0.5f * m_STSphere.mRadius, m_STSphere.mRadius, 0);
		break;
	default:
		assert(!"Shouldn't be here! Debug it!");
		return false;
	}
	return true;
}

void ShapeSphereImp::GetVolume(A3DVECTOR3& outVolume) const
{
	outVolume.Set(m_STSphere.mRadius, 0.0f, 0.0f);
}

bool ShapeSphereImp::SetVolume(const A3DVECTOR3& newVolume)
{
	if (newVolume.x < 0.0f)
		return false;

	if (NxMath::equals(newVolume.x, 0.0f, APHYSX_FLOAT_EPSILON))
		return false;

	m_STSphere.mRadius = newVolume.x; 
	return true;
}

void ShapeSphereImp::Draw(A3DWireCollector& wc, int color, const NxMat34& mat, float scale) const
{
	APhysXDrawBasic::DrawSphere(wc, m_STSphere.mRadius * scale, color, 0xffffffff, &mat);
}

bool ShapeSphereImp::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const A3DMATRIX4& matPose, float scale) const
{
	NxVec3 orig(ray.vStart.x, ray.vStart.y, ray.vStart.z);
	NxVec3 dir(ray.vDelta.x, ray.vDelta.y, ray.vDelta.z);
	float length = dir.magnitude();
	dir.normalize();
	NxVec3 center = APhysXConverter::A2N_Vector3(matPose.GetRow(3));

	NxReal t = 1.0f;
	NxVec3 hitPos(0.0f);
	if (NxRaySphereIntersect(orig, dir, length, center, m_STSphere.mRadius * scale, t, hitPos))
	{
		t /= length;
		if (t < hitInfo.t)
		{
			hitInfo.t = t;
			hitInfo.vHit.Set(hitPos.x, hitPos.y, hitPos.z);
			NxVec3 normal = hitPos - center;
			normal.normalize();
			hitInfo.vNormal.Set(normal.x, normal.y, normal.z);
			return true;
		}
	}
	return false;
}

bool ShapeSphereImp::TestOverlap(const NxBox& obb, const NxMat34& mat, float scale) const
{
	NxSphere sphere(mat.t, m_STSphere.mRadius * scale);
	return IShapeImp::TestOverlap(obb, sphere);
}

int ShapeSphereImp::TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec, const NxMat34& mat, float scale) const
{
	NxSphere sphere(mat.t, m_STSphere.mRadius * scale);
	m_cbReport.SetReceiveArray(&outVec);
	scene.GetNxScene()->overlapSphereShapes(sphere, NX_ALL_SHAPES, 0, 0, &m_cbReport, 0xffffffff, NULL, true);
	return m_cbReport.Clear();
}

ShapeCapsuleImp::ShapeCapsuleImp()
{
	A3DVECTOR3 vol(0.0f);
	IShapeImp::GetDefVolumeForTheType(Region::ST_CAPSULE, vol);
	SetVolume(vol);
}

bool ShapeCapsuleImp::GetProperVolumeForNewType(ShapeType stNew, A3DVECTOR3& outVol) const
{
	if (IShapeImp::GetProperVolumeForNewType(stNew, outVol))
		return true;

	float maxVal = m_STCapsule.mRadius;
	if (maxVal < m_STCapsule.mHeight)
		maxVal = m_STCapsule.mHeight;

	switch (stNew)
	{
	case Region::ST_SPHERE:
		outVol.Set(maxVal, 0.0f, 0.0f);
		break;
	case Region::ST_BOX:
		{
			float halfSide = 0.707f * maxVal;
			outVol.Set(halfSide, halfSide, halfSide);
		}
		break;
	default:
		assert(!"Shouldn't be here! Debug it!");
		return false;
	}
	return true;
}

void ShapeCapsuleImp::GetVolume(A3DVECTOR3& outVolume) const
{
	outVolume.Set(m_STCapsule.mRadius, m_STCapsule.mHeight, 0.0f);
}

bool ShapeCapsuleImp::SetVolume(const A3DVECTOR3& newVolume)
{
	if (newVolume.x < 0.0f)
		return false;
	if (newVolume.y < 0.0f)
		return false;

	if (NxMath::equals(newVolume.x, 0.0f, APHYSX_FLOAT_EPSILON))
		return false;
	if (NxMath::equals(newVolume.y, 0.0f, APHYSX_FLOAT_EPSILON))
		return false;

	m_STCapsule.mRadius = newVolume.x;
	m_STCapsule.mHeight = newVolume.y;
	return true;
}

NxBox gobb;
void ShapeCapsuleImp::Draw(A3DWireCollector& wc, int color, const NxMat34& mat, float scale) const
{
	APhysXDrawBasic::DrawCapsule(wc, m_STCapsule.mRadius * scale, m_STCapsule.mHeight * scale, color, 0xffffffff, &mat);
	APhysXDrawBasic::DrawBox(wc,gobb, color);
}

bool ShapeCapsuleImp::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const A3DMATRIX4& matPose, float scale) const
{
	NxVec3 orig(ray.vStart.x, ray.vStart.y, ray.vStart.z);
	NxVec3 dir(ray.vDelta.x, ray.vDelta.y, ray.vDelta.z);
	float length = dir.magnitude();
	dir.normalize();

	NxVec3 yAxis = APhysXConverter::A2N_Vector3(matPose.GetRow(1));
	NxVec3 center = APhysXConverter::A2N_Vector3(matPose.GetRow(3));
	NxCapsule capsule;
	capsule.p0 = center + yAxis * m_STCapsule.mHeight * scale * 0.5f;
	capsule.p1 = center - yAxis * m_STCapsule.mHeight * scale * 0.5f;
	capsule.radius = m_STCapsule.mRadius * scale;

	bool bOnSurface = false;
	if (IsCapsuleContainsPoint(capsule, orig, bOnSurface))
	{
		if (bOnSurface)
		{
			hitInfo.t = 0.0f;
			hitInfo.vHit.Set(orig.x, orig.y, orig.z);
			NxVec3 normal;
			ComputeNormalFromPoint(capsule, orig, normal);
			hitInfo.vNormal.Set(normal.x, normal.y, normal.z);
			return true;
		}
		return false;
	}

	NxReal t[2] = { -1.0f, -1.0f };
	NxU32 nHitPT = NxRayCapsuleIntersect(orig, dir, capsule, t);
	if (0 == nHitPT)
		return false;

	float theT = -1.0f;
	if (!(t[0] < 0.0f))   // don't use "t[x] >= 0" directly
		theT = t[0];
	if (!(t[1] < 0.0f))
	{
		if (t[1] < theT)
			theT = t[1];
	}

	if (!(theT < 0.0f))
	{
		float hitT = theT / length;
		if (hitT < hitInfo.t)
		{
			hitInfo.t = hitT;
			NxVec3 hitPos = orig + theT* dir;
			hitInfo.vHit.Set(hitPos.x, hitPos.y, hitPos.z);
			NxVec3 normal;
			ComputeNormalFromPoint(capsule, orig, normal);
			hitInfo.vNormal.Set(normal.x, normal.y, normal.z);
			return true;
		}
	}
	return false;
}

bool ShapeCapsuleImp::IsCapsuleContainsPoint(const NxCapsule& capsule, const NxVec3 &p, bool& outOnSurface) const
{
	outOnSurface = false;
	const float rSqr = capsule.radius * capsule.radius;

	// test top semi-sphere
	NxVec3 ptDir = p - capsule.p0;
	float ptDistSqr = ptDir.magnitudeSquared();
	if (rSqr > ptDistSqr)
		return true;

	NxVec3 upDir = capsule.p0 - capsule.p1;
	upDir.normalize();
	if (NxMath::equals(rSqr, ptDistSqr, APHYSX_FLOAT_EPSILON))
	{
		ptDir.normalize();
		float dotVal = upDir.dot(ptDir);
		if (0.0f > dotVal)
			return true;

		outOnSurface = true;
		return true;
	}

	// test bottom semi-sphere
	ptDir = p - capsule.p1;
	ptDistSqr = ptDir.magnitudeSquared();
	if (rSqr > ptDistSqr)
		return true;

	if (NxMath::equals(rSqr, ptDistSqr, APHYSX_FLOAT_EPSILON))
	{
		ptDir.normalize();
		float dotVal = upDir.dot(ptDir);
		if (0.0f < dotVal)
			return true;

		outOnSurface = true;
		return true;
	}

	// test cylinder
	float p1PlaneD = capsule.p1.dot(upDir) * -1.0f;
	float distToPlane = p.dot(upDir) + p1PlaneD;
	if ((0.0f < distToPlane) && (distToPlane < capsule.computeLength()))
	{
		NxVec3 pProj = p + upDir * -1.0f * distToPlane;
		NxVec3 d = pProj - capsule.p1;
		ptDistSqr = d.magnitudeSquared();
		if (rSqr > ptDistSqr)
			return true;

		if (NxMath::equals(rSqr, ptDistSqr, APHYSX_FLOAT_EPSILON))
		{
			outOnSurface = true;
			return true;
		}
	}
	return false;
}

void ShapeCapsuleImp::ComputeNormalFromPoint(const NxCapsule& capsule, const NxVec3& p, NxVec3& outNormal) const
{
	const float height = capsule.computeLength();
	NxVec3 upDir = capsule.p0 - capsule.p1;
	upDir.normalize();

	float p0PlaneD = capsule.p0.dot(upDir) * -1.0f;
	float distToPlane = p.dot(upDir) + p0PlaneD;
	if (0.0f < distToPlane)
	{
		outNormal = p - capsule.p0;
	}
	else if ((height * -1.0f) > distToPlane) 
	{
		outNormal = p - capsule.p1;
	}
	else
	{
		NxVec3 pProj = p + upDir * -1.0f * distToPlane;
		outNormal = pProj - capsule.p0;
	}
	outNormal.normalize();
}

bool ShapeCapsuleImp::TestOverlap(const NxBox& obb, const NxMat34& mat, float scale) const
{
	gobb = obb;
	NxVec3 nxExts(m_STCapsule.mRadius, m_STCapsule.mHeight, m_STCapsule.mRadius);
	nxExts.y += nxExts.x * 2;
	nxExts *= scale;
	if (!NxBoxBoxIntersect(nxExts, mat.t, mat.M, obb.extents, obb.center, obb.rot, true))
	{
		NxBox capsuleOBB(mat.t, nxExts, mat.M);
		if (!NxBoxContainsPoint(capsuleOBB, obb.center))
			return false;
	}

	if (NxBoxContainsPoint(obb, mat.t))
		return true;

	const float R = m_STCapsule.mRadius * scale;
	const float H = m_STCapsule.mHeight * scale;

	nxExts.y -= nxExts.x * 2;
	bool bBoxBoxIntersect = false;
	bBoxBoxIntersect = NxBoxBoxIntersect(nxExts, mat.t, mat.M, obb.extents, obb.center, obb.rot, true);
	if (!bBoxBoxIntersect)
	{
		NxBox cylinderOBB(mat.t, nxExts, mat.M);
		if (NxBoxContainsPoint(cylinderOBB, obb.center))
			bBoxBoxIntersect = true;
	}
	if (bBoxBoxIntersect)
	{
		if (BoxCylinderIntersect(obb, mat, R, H))
			return true;
	}

	const NxVec3 yAxis = mat.M.getColumn(1);
	const NxVec3 p0 = mat.t + yAxis * H * 0.5f;
	NxSphere sphere0(p0, R);
	if (IShapeImp::TestOverlap(obb, sphere0))
		return true;

	const NxVec3 p1 = mat.t - yAxis * H * 0.5f;
	NxSphere sphere1(p1, R);
	if (IShapeImp::TestOverlap(obb, sphere1))
		return true;

	return false;
}

int ShapeCapsuleImp::TestOverlap(APhysXScene& scene, APtrArray<NxActor*>& outVec, const NxMat34& mat, float scale) const
{
	NxVec3 yAxis = mat.M.getColumn(1);
	NxCapsule capsule;
	capsule.p0 = mat.t + yAxis * m_STCapsule.mHeight * scale * 0.5f;
	capsule.p1 = mat.t - yAxis * m_STCapsule.mHeight * scale * 0.5f;
	capsule.radius = m_STCapsule.mRadius * scale;

	m_cbReport.SetReceiveArray(&outVec);
	scene.GetNxScene()->overlapCapsuleShapes(capsule, NX_ALL_SHAPES, 0, 0, &m_cbReport, 0xffffffff, NULL, true);
	return m_cbReport.Clear();
}

bool ShapeCapsuleImp::BoxCylinderIntersect(const NxBox& obb, const NxMat34& cylinderPose, float r, float h) const
{
	const NxVec3 cylinderYAxis = cylinderPose.M.getColumn(1);
	const NxVec3 p0 = cylinderPose.t + cylinderYAxis * h * 0.5f;
	const NxVec3 p1 = cylinderPose.t - cylinderYAxis * h * 0.5f;

	const NxPlane plnP1(p1, cylinderYAxis);
	const float dist = plnP1.distance(obb.center);
	if ((0 < dist) && (dist < h))
	{
		NxVec3 proj = obb.center - plnP1.normal * dist;
		if (PtToPtDistanceIsInRange(proj, p1, r * r))
			return true;
	}

	NxVec3 obbVtxs[8];
	NxComputeBoxPoints(obb, obbVtxs);

	// test box vs cylinder plane
	float dToCenter[8];
	NxVec3 projVtx[8];
	NxVec3 vTemp;
	for(int i = 0; i < 8; ++i)
	{
		float dist = plnP1.distance(obbVtxs[i]);
		projVtx[i] = obbVtxs[i] - plnP1.normal * dist;
		vTemp = projVtx[i] - p1;
		dToCenter[i] = vTemp.magnitudeSquared();
	}

	int minIdx = 0;
	float minDist = dToCenter[0];
	for(int i = 1; i < 8; ++i)
	{
		if (dToCenter[i] < minDist)
		{
			minDist = dToCenter[i];
			minIdx = i;
		}
	}

	NxVec3 cc;
	NxVec3 normal;
	NxPlane plnTangent;
	if (!(r * r > minDist))
	{
		normal = projVtx[minIdx] - p1;
		normal.normalize();
		cc = p1 + normal * r;
		plnTangent.set(cc, normal);
		if (!PlaneBoxIntersect(plnTangent, obbVtxs, 8))
			return false;
	}

	const NxVec3 xAxis = obb.rot.getColumn(0);
	const NxVec3 yAxis = obb.rot.getColumn(1);
	const NxVec3 zAxis = obb.rot.getColumn(2);

	NxVec3 zTemp = xAxis.cross(cylinderYAxis);
	if (!zTemp.equals(gZero, APHYSX_FLOAT_EPSILON))
	{
		normal = cylinderYAxis.cross(zTemp);
		cc = cylinderPose.t + normal * r;
		plnTangent.set(cc, normal);
		if (!PlaneBoxIntersect(plnTangent, obbVtxs, 8))
			return false;
	}

	zTemp = yAxis.cross(cylinderYAxis);
	if (!zTemp.equals(gZero, APHYSX_FLOAT_EPSILON))
	{
		normal = cylinderYAxis.cross(zTemp);
		cc = cylinderPose.t + normal * r;
		plnTangent.set(cc, normal);
		if (!PlaneBoxIntersect(plnTangent, obbVtxs, 8))
			return false;
	}

	zTemp = zAxis.cross(cylinderYAxis);
	if (!zTemp.equals(gZero, APHYSX_FLOAT_EPSILON))
	{
		normal = cylinderYAxis.cross(zTemp);
		cc = cylinderPose.t + normal * r;
		plnTangent.set(cc, normal);
		if (!PlaneBoxIntersect(plnTangent, obbVtxs, 8))
			return false;
	}

	plnTangent.set(p0, cylinderYAxis);
	if (!PlaneBoxIntersect(plnTangent, obbVtxs, 8))
		return false;

	plnTangent.set(p1, cylinderYAxis * -1);
	if (!PlaneBoxIntersect(plnTangent, obbVtxs, 8))
		return false;

	// test cylinder vs box plane  
	const NxVec3 bcxp = obb.center + xAxis * obb.extents.x;
	const NxPlane plnXP(bcxp, xAxis);
	if (!PlaneCylinderIntersect(plnXP, p0, p1, r))
		return false;

	const NxVec3 bcxn = obb.center - xAxis * obb.extents.x;
	const NxPlane plnXN(bcxn, xAxis * -1);
	if (!PlaneCylinderIntersect(plnXN, p0, p1, r))
		return false;

	const NxVec3 bcyp = obb.center + yAxis * obb.extents.y;
	const NxPlane plnYP(bcyp, yAxis);
	if (!PlaneCylinderIntersect(plnYP, p0, p1, r))
		return false;

	const NxVec3 bcyn = obb.center - yAxis * obb.extents.y;
	const NxPlane plnYN(bcyn, yAxis * -1);
	if (!PlaneCylinderIntersect(plnYN, p0, p1, r))
		return false;

	const NxVec3 bczp = obb.center + zAxis * obb.extents.z;
	const NxPlane plnZP(bczp, zAxis);
	if (!PlaneCylinderIntersect(plnZP, p0, p1, r))
		return false;

	const NxVec3 bczn = obb.center - zAxis * obb.extents.z;
	const NxPlane plnZN(bczn, zAxis * -1);
	if (!PlaneCylinderIntersect(plnZN, p0, p1, r))
		return false;
	return true;
}

bool ShapeCapsuleImp::PlaneCylinderIntersect(const NxPlane& plane, const NxVec3& p0, const NxVec3& p1, float r) const
{
	const float d0 = plane.distance(p0);
	const float d1 = plane.distance(p1);
	if (0 > d0)
		return true;
	if (0 > d1)
		return true;

	NxVec3 pj0 = p0 - plane.normal * d0;
	NxVec3 pj1 = p1 - plane.normal * d1;

	float t = 0.0f;
	NxVec3 vPJ = pj1 - pj0;
	if (!vPJ.equals(gZero, APHYSX_FLOAT_EPSILON))
	{
		vPJ.normalize();
		NxVec3 vP = p1 - p0;
		vP.normalize();
		float cosValue = vP.dot(vPJ);
		t = NxMath::abs(cosValue * r);
	}

	float minD = d0;
	if (d1 < minD)
		minD = d1;
	if (minD < t)
		return true;
	return false;
}

bool ShapeCapsuleImp::PlaneBoxIntersect(const NxPlane& plane, const NxVec3* pBOXVtxs, int nPT) const
{
	for(int i = 0; i < nPT; ++i)
	{
		float dist = plane.distance(pBOXVtxs[i]);
		if (0 > dist)
			return true;
	}
	return false;
}
