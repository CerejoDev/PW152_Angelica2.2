/*
 * FILE: TConvexMesh.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_TCONVEXMESH_H_
#define _APHYSXLAB_TCONVEXMESH_H_

#include "SubThreadInterface.h"

#include <AArray.h>
#include <AString.h>
#include <A3DVector.h>

class CRender;
class A3DTerrain2;
class CPhysXObjMgr;
class IPhysXObjBase;
class APhysXScene;

class TestConvexMesh
{
private:
	struct ObjDesc
	{
		ObjDesc() { interval = -1; fScale = 1;}
		
		int interval;
		float fScale;
		A3DVECTOR3 pos;

		// NOTE: We have to use "char*" to instead of string(AString or std::string).
		// String will cause a weird memory leak. So strange thing! Debug it in the future.
		const char*  strModelFile;
		//AString strModelFile;   
	};

public:
	TestConvexMesh(CRender& render, APhysXScene& scene, CPhysXObjMgr& objMgr, A3DTerrain2* pTerrain = 0, const int NumLoadThread = 1);
	~TestConvexMesh();

	void CreateObjectGroup(const char* szFile);
	
	void CreateObject(const ObjDesc& desc);
	void ReleaseObject();

private:
	void ClearAll();

	A3DVECTOR3 GetGroupBasePos(A3DVECTOR3& zDir, const bool bIsNewPos);
	IPhysXObjBase* ApplyAnObject(const char* szFile);

private:
	class Loader : public IQueueThread<ObjDesc>
	{
	public:
		Loader(TestConvexMesh& TCM) : m_TCM(TCM) {}

	private:
		virtual void DoTask(const ElementType& it);

	private:
		TestConvexMesh& m_TCM;
	};

private:
	static const int m_nGroupSize;
	
	int m_idxStart;
	CRender&	  m_Render;
	APhysXScene&  m_Scene;
	CPhysXObjMgr& m_objMgr;
	A3DTerrain2*  m_pTerrain;

	APtrArray<Loader*> m_pvecLoader;
	APhysXMutex m_AtomLocker;

	int m_nAllocatedCount;
	APtrArray<IPhysXObjBase*> m_ptrObjs;
};

#endif