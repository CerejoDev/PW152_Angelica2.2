/*
 * FILE: OverlapBounceDamping.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/08/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

bool CMOBDamping::SceneNode::AddActor(NxActor& ac)
{
	NxScene& pScene = ac.getScene();
	if (!IsTheSameScene(pScene))
	{
		assert(!"Oops! Different NxScene!");
		return false;
	}

	bool bVal = ac.readActorFlag(NX_AF_CONTACT_MODIFICATION);
	m_mapActInfo[&ac] = bVal;
	if (!bVal)
		ac.raiseActorFlag(NX_AF_CONTACT_MODIFICATION);
	return true;
}

bool CMOBDamping::SceneNode::SetContactModify(CMOBDamping* pCM)
{
	if (0 == m_pScene)
		return false;

	if (!m_bIsInstalledCM)
	{
		m_pCMBack = m_pScene->getUserContactModify();
		m_pScene->setUserContactModify(pCM);
		m_bIsInstalledCM = true;
	}
	return true;
}

bool CMOBDamping::SceneNode::UpdateTick()
{
	if (0 == m_pScene)
		return false;
	
	std::map<NxActor*, bool>::iterator it = m_mapActInfo.begin();
	std::map<NxActor*, bool>::iterator itEnd = m_mapActInfo.end();	
	for (; it != itEnd;)
	{
		if (!it->first->isSleeping())
		{
			++it;
		}
		else
		{
			if (!it->second)
				it->first->clearActorFlag(NX_AF_CONTACT_MODIFICATION);
			it = m_mapActInfo.erase(it);
		}
	}
	if (0 == m_mapActInfo.size())
		RestoreAll();
	return true;
}

void CMOBDamping::SceneNode::RestoreAll()
{
	if (0 != m_pScene)
	{
#if defined(_DEBUG)
		static count = 0;
		ACString str;
		str.Format(_T("CMOBDamping Debug Info:\n    Scene: %d, RestoreAll----%d--\n"), m_pScene, count);
		OutputDebugString(str);
		++count;
#endif

		m_pScene->setUserContactModify(m_pCMBack);
		m_pScene = 0;
		m_pCMBack = 0;
		m_bIsInstalledCM = false;
	}
	
	std::map<NxActor*, bool>::const_iterator it = m_mapActInfo.begin();
	std::map<NxActor*, bool>::const_iterator itEnd = m_mapActInfo.end();	
	for (; it != itEnd; ++it)
	{
		if (!it->second)
			it->first->clearActorFlag(NX_AF_CONTACT_MODIFICATION);
	}
	m_mapActInfo.clear();
}

bool CMOBDamping::SceneNode::IsTheSameScene(NxScene& sc)
{
	if (0 == m_pScene)
	{
		m_pScene = &sc;
		return true;
	}

	return (m_pScene == &sc);
}

bool CMOBDamping::onContactConstraint(NxU32& changeFlags, const NxShape* shape0, const NxShape* shape1,  
						 const NxU32 featureIndex0, const NxU32 featureIndex1, NxContactCallbackData& data)
{
	assert(true == m_bIsInitedCS);
	if (!m_bIsInitedCS)
		return true;

	EnterCriticalSection(&m_csCM);
	
	data.restitution = 0;
	changeFlags = NX_CCC_RESTITUTION;
	while (data.error.magnitudeSquared() > 0.001f)
	{
		data.error *= 0.2f;
		changeFlags |= NX_CCC_ERROR;
	}
	
	LeaveCriticalSection(&m_csCM);
	return true;
}

void CMOBDamping::OpenCM()
{
	CloseCM();
	if (!m_bIsInitedCS)
	{
		InitializeCriticalSection(&m_csCM);
		m_bIsInitedCS = true;
	}
}

bool CMOBDamping::OpenCM(APhysXSkeletonRBObject& sklRB)
{
	OpenCM();
	return AppendModel(sklRB);
}

bool CMOBDamping::AppendModel(APhysXSkeletonRBObject& sklRB)
{
	const int nCount = sklRB.GetNxActorNum();
	if (0 == nCount)
		return true;

	for (int i = 0; i < nCount; ++i)
	{
		NxScene& rScene = sklRB.GetNxActor(i)->getScene();
		if (!m_Scene.IsTheSameScene(rScene))
			return false;
	}

	NxActor* pActor = 0;
	for (int j = 0; j < nCount; ++j)
	{
		pActor = sklRB.GetNxActor(j);
		m_Scene.AddActor(*pActor);
	}
	m_Scene.SetContactModify(this);
	return true;
}
