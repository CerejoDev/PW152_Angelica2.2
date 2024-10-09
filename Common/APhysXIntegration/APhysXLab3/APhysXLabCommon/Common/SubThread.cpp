/*
 * FILE: SubThread.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"
#include "SubThread.h"

QTLoader::QTLoader(IEngineUtility& eu, Scene& scene) : m_eu(eu), m_Scene(scene)
{
	m_pCollection = 0;
}

void QTLoader::DoTask(const ElementType& it)
{
	IPhysXObjBase* pObj = m_Scene.CreateObjectRuntime(ObjManager::OBJ_TYPEID_ECMODEL, "Models\\NPCS\\ÆÆËé\\ÆÆËéÄ¾Í°\\ÆÆËéÄ¾Í°.ecm", it, true, true);
	if (0 != pObj && 0 != m_pCollection)
		m_pCollection->RegisterObj(*pObj);

	CPhysXObjECModel* pECM = (CPhysXObjECModel*)pObj;
	pECM->GetECModel()->GetModelPhysics()->GetModelSync()->GetSkeletonRBObject()->AddForce(NxVec3(1200, 0, 0), NX_FORCE);

	int nTasks = GetTaskCount();
	if (nTasks < 20)
	{
		A3DVECTOR3 pos = it;
		for (int i = 0; i < 10; ++i)
		{
			pos.x += 1.0f;
			pos.y += 0.5f;
			if (100.0f < pos.x)
				pos.x -= 100;
			if (15.0f < pos.y)
				pos.y -= 15;
			AddTask(pos);
		}
	}
}

void QTUnLoader::DoTask(const ElementType& it)
{
	m_Scene.ReleaseObject(it);
}

BackgroundTask::BackgroundTask(IEngineUtility& eu, Scene& scene) : m_qtUnLoader(scene)
{
	for (int i = 0; i < nLoaderNum; ++i)
	{
		QTLoader* pL = new QTLoader(eu, scene);
		if (0 != pL)
		{
			pL->SetCollection(*this);
			m_qtLoader.push_back(pL);
		}
	}
}

BackgroundTask::~BackgroundTask()
{
	for (int i = 0; i < nLoaderNum; ++i)
		delete m_qtLoader[i];
}

void BackgroundTask::Tick(float dtSec)
{
	APhysXMutexLock autoLocker(m_Locker);

	int i = 0;
	int nAll = m_Objs.size();
	int nDelCount = 0;
	for (; i < nAll;)
	{
		m_Objs[i].fAge += dtSec;
		if (m_Objs[i].fAge > 1.0)
		{
			int nLastOK = nAll - 1;
			ConvexOBJInfo tmp = m_Objs[i];
			m_Objs[i] = m_Objs[nLastOK];
			m_Objs[nLastOK] = tmp;
			--nAll;
			++nDelCount;

			m_qtUnLoader.AddTask(tmp.pObj);
		}
		else
		{
			++i;
		}
	}
	while (nDelCount > 0)
	{
		m_Objs.pop_back();
		--nDelCount;
	}
}

void BackgroundTask::AddTask(const A3DVECTOR3& pos)
{
	A3DVECTOR3 p = pos;
	for (int i = 0; i < nLoaderNum; ++i)
	{
		m_qtLoader[i]->AddTask(p);
		p.y += 2;
	}
}

void BackgroundTask::RegisterObj(IPhysXObjBase& obj)
{
	APhysXMutexLock autoLocker(m_Locker);

	ConvexOBJInfo ci;
	ci.pObj = &obj;
	m_Objs.push_back(ci);
}
