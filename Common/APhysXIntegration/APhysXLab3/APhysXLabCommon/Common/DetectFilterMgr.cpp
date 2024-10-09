/*
 * FILE: DetectFilterMgr.cpp
 *
 * DESCRIPTION: 
 *		manager the temporary filter for PhysX raycast/sweep
 *
 * CREATED BY: Yang Liu, 2011/12/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */
#include "stdafx.h"
#include "DetectFilterMgr.h"

DetectFilterMgr::DetectFilterMgr()
{
	m_pAPhysXScene = 0;
	m_pCCM = 0;
	m_TempCNL = APX_COLLISION_CHANNEL_INVALID;
	m_ActiveGroup = 0xffffffff;
	m_GroupMask.bits0 = 0;
	m_GroupMask.bits1 = 0;
	m_GroupMask.bits2 = 0;
	m_GroupMask.bits3 = 0;
}

DetectFilterMgr::~DetectFilterMgr()
{
	FilterEnd();
}

bool DetectFilterMgr::FilterBegin(APhysXScene& scene)
{
	if (APX_COLLISION_CHANNEL_INVALID != m_TempCNL)
		return false;

	APhysXCollisionChannelManager* pCCM = scene.GetCollisionChannelManager();
	if (0 == pCCM)
		return false;

	APhysXCollisionChannel tempCNL = APX_COLLISION_CHANNEL_INVALID;
	tempCNL = pCCM->GetRelativeIsolatedChannel();
	if (APX_COLLISION_CHANNEL_INVALID == tempCNL)
		return false;

	m_pAPhysXScene = &scene;
	m_pCCM = pCCM;
	m_TempCNL = tempCNL;
	m_ActiveGroup = 0xffffffff;
	m_BackupInfo.clear();

	m_GroupMask = m_pCCM->GetChannelGroupMask(m_TempCNL);
	m_GroupMask.bits0 = ~m_GroupMask.bits0;
	m_GroupMask.bits1 = ~m_GroupMask.bits1;
	m_GroupMask.bits2 = ~m_GroupMask.bits2;
	m_GroupMask.bits3 = ~m_GroupMask.bits3;
	return true;
}

void DetectFilterMgr::FilterEnd()
{
	if (APX_COLLISION_CHANNEL_INVALID == m_TempCNL)
		return;

	if (0 == m_BackupInfo.size())
		return;

	std::vector<CNLInfo>::iterator it = m_BackupInfo.begin();
	std::vector<CNLInfo>::iterator itEnd = m_BackupInfo.end();
	m_pAPhysXScene->LockWritingScene();
	for (; it != itEnd; ++it)
	{
		it->pSRBO->SetCollisionChannel(it->backupCNL);
		if (0 != it->pActor)
		{
			NxU32 nShapes = it->pActor->getNbShapes();
			NxShape* const* ppShape = it->pActor->getShapes();
			for (NxU32 i = 0; i < nShapes; ++i)
				ppShape[i]->setGroupsMask(it->backupGM);
		}
	}
	m_pAPhysXScene->UnlockWritingScene();
	m_BackupInfo.clear();
	m_TempCNL = APX_COLLISION_CHANNEL_INVALID;
}

NxU32 DetectFilterMgr::GetActiveGroup() const
{
	if (APX_COLLISION_CHANNEL_INVALID == m_TempCNL)
		return 0xffffffff;
	return m_ActiveGroup;
}

const NxGroupsMask* DetectFilterMgr::GetGroupsMask() const
{
	if (APX_COLLISION_CHANNEL_INVALID == m_TempCNL)
		return 0;
	return &m_GroupMask;
}

void DetectFilterMgr::FilterOutDynObject(CPhysXObjDynamic* pObj)
{
	if (0 == pObj)
		return;

	assert(APX_COLLISION_CHANNEL_INVALID != m_TempCNL);
	APhysXSkeletonRBObject* pSRBO = pObj->GetSkeletonRBObject();
	if (0 == pSRBO)
		return;

	APhysXCollisionChannel backupCNL = pSRBO->GetCollisionChannel();
	CNLInfo backupInfo(pSRBO, backupCNL);
	backupInfo.pActor = pObj->GetProperties()->GetCCMgr().GetHiddenActor();
	NxU32 nShapes = 0;
	NxShape* const* ppShape = 0;
	if (0 != backupInfo.pActor)
	{
		nShapes = backupInfo.pActor->getNbShapes();
		ppShape = backupInfo.pActor->getShapes();
		if (0 == nShapes)
			backupInfo.pActor = 0;
		else
			backupInfo.backupGM = ppShape[0]->getGroupsMask();
	}
	m_BackupInfo.push_back(backupInfo);

	m_pAPhysXScene->LockWritingScene();
	pSRBO->SetCollisionChannel(m_TempCNL);
	if (0 != backupInfo.pActor)
	{
		NxGroupsMask groupMask = m_pCCM->GetChannelGroupMask(m_TempCNL);
		for (NxU32 i = 0; i < nShapes; ++i)
			ppShape[i]->setGroupsMask(groupMask);
	}
	m_pAPhysXScene->UnlockWritingScene();
}
