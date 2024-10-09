/*
 * FILE: TConvexMesh.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma warning(disable:4786)
#include "stdafx.h"

const int TestConvexMesh::m_nGroupSize = 10;

TestConvexMesh::TestConvexMesh(CRender& render, APhysXScene& scene, CPhysXObjMgr& objMgr, A3DTerrain2* pTerrain, const int NumLoadThread)
 : m_Render(render), m_Scene(scene), m_objMgr(objMgr), m_pTerrain(pTerrain)
{
	m_idxStart = m_objMgr.SizeModels();
	m_nAllocatedCount = 0;

	int nCount = (1 < NumLoadThread)? NumLoadThread : 1;
	for (int i = 0; i < nCount; ++i)
	{
		Loader* pLd = new Loader(*this);
		if (0 != pLd)
			m_pvecLoader.Add(pLd);
	}
}

TestConvexMesh::~TestConvexMesh()
{
	for (int i = 0; i < m_pvecLoader.GetSize(); ++i)
		delete m_pvecLoader.GetAt(i);
	m_pvecLoader.RemoveAll();

	ClearAll();
}

void TestConvexMesh::ClearAll()
{
	const int nSize = m_ptrObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_objMgr.ReleaseObject(m_ptrObjs[i]);
	
	m_ptrObjs.RemoveAll();
	m_nAllocatedCount = 0;
}

A3DVECTOR3 TestConvexMesh::GetGroupBasePos(A3DVECTOR3& zDir, const bool bIsNewPos)
{
	static A3DVECTOR3 basePos;
	static A3DVECTOR3 zDelta;
	static A3DVECTOR3 xDelta;

	if (bIsNewPos)
	{
		A3DCamera* pCam = m_Render.GetCamera();
		zDelta = pCam->GetDirH();
		xDelta = pCam->GetRight();
		basePos = pCam->GetPos() + zDelta * 5;
	}
	else
	{
		basePos += xDelta * 10;
	}

	if (0 != m_pTerrain)
		basePos.y = m_pTerrain->GetPosHeight(basePos);

	zDir = zDelta;
	return basePos; 
}

void TestConvexMesh::CreateObjectGroup(const char* szFile)
{
	static int iThread = -1;
	iThread = (++iThread) % m_pvecLoader.GetSize();

	assert(0 != m_pvecLoader[iThread]);
	if (0 == m_pvecLoader[iThread])
		return;

	A3DVECTOR3 zDir;
	bool bIsNewPos = false;
	if (0 >= m_nAllocatedCount)
	{
		bIsNewPos = true;
		for (int i = 0; i < m_pvecLoader.GetSize(); ++i)
		{
			Loader* pL = m_pvecLoader.GetAt(i);
			if (0 != m_pvecLoader[i]->GetTaskCount())
			{
				bIsNewPos = false;
				break;
			}
		}
	}

	ObjDesc desc;
	desc.interval = 400;
	desc.pos = GetGroupBasePos(zDir, bIsNewPos);
	desc.strModelFile = szFile;

	for (int i = 0; i < m_nGroupSize; ++i)
	{
		if (0 != m_pTerrain)
			desc.pos.y = m_pTerrain->GetPosHeight(desc.pos);

		desc.fScale = 0.5 + 0.1f * i;
		m_pvecLoader[iThread]->AddTask(desc);

		desc.pos += zDir * 12;
	}
}

void TestConvexMesh::Loader::DoTask(const ElementType& it)
{
	m_TCM.CreateObject(it);
	//OutputDebugString("---DoTask---\n");
}

void TestConvexMesh::CreateObject(const ObjDesc& desc)
{
	m_AtomLocker.Lock();
//	IPhysXObjBase* pObj = ApplyAnObject(desc.strModelFile.c_str());
	IPhysXObjBase* pObj = ApplyAnObject(desc.strModelFile);
	if (0 != pObj)
	{
		pObj->SetPos(desc.pos);
		pObj->SetScale(desc.fScale);
		pObj->EnterRuntime(m_Scene);
		m_nAllocatedCount++;
	}	
	m_AtomLocker.Unlock();

	if ((0 != pObj) && (0 < desc.interval))
		Sleep(desc.interval);
}

void TestConvexMesh::ReleaseObject()
{
	APhysXMutexLock autoLocker(m_AtomLocker);

	if (0 < m_nAllocatedCount)
	{
		IPhysXObjBase* pObj = m_ptrObjs.GetAt(m_nAllocatedCount - 1);
		pObj->Release(true);
		m_nAllocatedCount--;
	}
}

IPhysXObjBase* TestConvexMesh::ApplyAnObject(const char* szFile)
{
	if (0 == szFile)
		return 0;

	assert(m_nAllocatedCount >= 0);
	if (m_nAllocatedCount < 0)
		return 0;

	assert(m_nAllocatedCount <= m_ptrObjs.GetSize());
	if (m_nAllocatedCount > m_ptrObjs.GetSize())
		return 0;

	IPhysXObjBase* pObj = 0;
	if (m_nAllocatedCount < m_ptrObjs.GetSize())
		pObj = m_ptrObjs.GetAt(m_nAllocatedCount);
	else
	{
		pObj = m_objMgr.CreateObject(CPhysXObjMgr::OBJ_TYPEID_STATIC);
		if (0 != pObj)
			m_ptrObjs.Add(pObj);
	}

	if (0 == pObj)
		return 0;

	if (!pObj->LoadModel(m_Render, szFile))
		return 0;

	pObj->ClearFlag(IPhysXObjBase::OBF_DRAW_BINDING_BOX);
	pObj->ClearFlag(IPhysXObjBase::OBF_DRAW_REF_FRAME);
	return pObj;
}




