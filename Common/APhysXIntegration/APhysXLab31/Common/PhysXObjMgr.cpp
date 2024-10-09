/*
* FILE: PhysXObjMgr.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2009/05/18
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/
#include "stdafx.h"

CPhysXObjMgr::LabPhysXOMD* CPhysXObjMgr::LabPhysXOMD::Clone() const
{
	return new LabPhysXOMD(*this);
}

CPhysXObjMgr::CPhysXObjMgr(int UID)
{
	m_propMgr.InitTypeIDAndHostObject(RawObjMgr::OBJ_TYPEID_MANAGER, *this, UID);
	m_propMgr.SetObjState(IPropObjBase::OBJECT_STATE_ACTIVE);
	SetProperties(m_propMgr);
}

CPhysXObjMgr::~CPhysXObjMgr()
{
	ReleaseObjectToEnd(0);
	ToDie(false);
}

void CPhysXObjMgr::OnObjectDeletionNotify(APhysXObject* pObject)
{
/*	if (0 == pObject)
		return;

	if (!pObject->IsObjectType(APX_OBJTYPE_FORCEFIELD_EXPLOSION))
		return;

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		IPhysXObjBase* pObj = m_aObjects[i];
		if ((0 == pObj) || (CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD != pObj->GetObjType()))
			continue;

		CPhysXObjForceField* pFF = static_cast<CPhysXObjForceField*>(pObj);
		if (pFF->OnDeletionNotify(*pObject))
			break;
	}

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
	{
		IPhysXObjBase* pObj = m_objShotStuffs[j];
		if (CPhysXObjMgr::OBJ_TYPEID_FORCEFIELD != pObj->GetObjType())
			continue;
		
		CPhysXObjForceField* pFF = static_cast<CPhysXObjForceField*>(pObj);
		if (pFF->OnDeletionNotify(*pObject))
			break;
	}*/
}

IPhysXObjBase* CPhysXObjMgr::CreateLabPhysXObject(const ObjTypeID objType, const bool IsShotStuff)
{
	if (!RawObjMgr::IsPhysXObjType(objType))
		return 0;

	LabPhysXOMD lpOMD;
	if (IsShotStuff)
	{
		int idx = m_objShotStuffs.GetSize();
		lpOMD.SetShotMgrIdx(idx);
	}
	else
	{
		int idx = m_aObjects.GetSize();
		lpOMD.SetObjMgrIdx(idx);
	}

	IObjBase* pObj = RawObjMgr::GetInstance()->CreateRawObject(objType, lpOMD);
	if (0 == pObj)
		return 0;

	IPhysXObjBase* pPhysXObj = static_cast<IPhysXObjBase*>(pObj);
	if (IsShotStuff)
		m_objShotStuffs.Add(pPhysXObj);
	else
		m_aObjects.Add(pPhysXObj);
	return pPhysXObj;
}

void CPhysXObjMgr::ReleaseLabPhysXObject(IPhysXObjBase* pObject)
{
	assert(true == m_propMgr.QueryObjState(IPropObjBase::OBJECT_STATE_ACTIVE));
	if (0 == pObject)
		return;

	bool bIsPermanence = false;
	if (pObject->GetProperties()->GetFlags().ReadFlag(OBF_RUNTIME_ONLY))
		bIsPermanence = true;

	pObject->ToDie();
	if (!bIsPermanence)
		return;

	int idx = -1;
	APtrArray<IPhysXObjBase*>* pArray = 0;

	LabPhysXOMD* pLPOMD = static_cast<LabPhysXOMD*>(pObject->GetMgrData());
	if (0 == pLPOMD)
	{
		assert(!"Shouldn't be here! Debug it!");
	}
	else
	{
		idx = pLPOMD->GetObjMgrIdx();
		if (0 <= idx)
		{
			pArray = &m_aObjects;
			assert(-1 == pLPOMD->GetShotMgrIdx());
		}
		else
		{
			idx = pLPOMD->GetShotMgrIdx();
			pArray = &m_objShotStuffs;
			assert(-1 == pLPOMD->GetObjMgrIdx());
		}
		assert(-1 != idx);
		assert(pObject == (*pArray)[idx]);

		pArray->RemoveAtQuickly(idx);
		if (idx < pArray->GetSize())
		{
			LabPhysXOMD* pSwapped = static_cast<LabPhysXOMD*>((*pArray)[idx]->GetMgrData());
			assert(0 != pSwapped);
			if (-1 != pSwapped->GetObjMgrIdx())
			{
				pSwapped->SetObjMgrIdx(idx);
				assert(-1 == pSwapped->GetShotMgrIdx());
			}
			else
			{
				assert(-1 != pLPOMD->GetShotMgrIdx());
				pSwapped->SetShotMgrIdx(idx);
			}
		}
	}
	RawObjMgr::GetInstance()->ReleaseRawObject(pObject);
}

void CPhysXObjMgr::ReleaseObjectToEnd(const int idxFrom)
{
	if (0 > idxFrom)
	{
		assert(0 <= idxFrom);
		return;
	}

	const int nAllCount = SizeAllObjs();
	if (idxFrom >= nAllCount)
	{
		if (0 == nAllCount)
			return;

		assert(idxFrom < nAllCount);
		return;
	}
	
	const int nMCount = SizeModelObjs();
	if (idxFrom < nMCount)
	{
		for (int i = idxFrom; i < nMCount; ++i)
		{
			m_aObjects[i]->ToDie();
			RawObjMgr::GetInstance()->ReleaseRawObject(m_aObjects[i]);
			m_aObjects[i] = 0;
		}
		m_aObjects.RemoveAt(idxFrom, nMCount - idxFrom);
		ReleaseAllShotObjects();
	}
	else
	{
		const int iStart = idxFrom - nMCount;
		const int iEnd = nAllCount - nMCount;
		for (int i = iStart; i < iEnd; ++i)
		{
			m_objShotStuffs[i]->ToDie();
			RawObjMgr::GetInstance()->ReleaseRawObject(m_objShotStuffs[i]);
			m_objShotStuffs[i] = 0;
		}
		m_objShotStuffs.RemoveAt(idxFrom - nMCount, nAllCount - idxFrom);
	}

	return;
}

void CPhysXObjMgr::ReleaseAllShotObjects()
{
	int MaxCount = m_objShotStuffs.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		m_objShotStuffs[i]->ToDie();
		RawObjMgr::GetInstance()->ReleaseRawObject(m_objShotStuffs[i]);
	}

	m_objShotStuffs.RemoveAll(true);
}

void CPhysXObjMgr::ReleaseAllRuntimeObjects()
{
	const int nMCount = SizeModelObjs();
	int idxEnd = nMCount - 1;
	int i = 0;
	while (i <= idxEnd)
	{
		if (!m_aObjects[i]->GetProperties()->GetFlags().ReadFlag(OBF_RUNTIME_ONLY))
		{
			++i;
			continue;
		}

		IPhysXObjBase* pTemp = m_aObjects[i];
		m_aObjects[i] = m_aObjects[idxEnd];
		m_aObjects[idxEnd] = pTemp;

		LabPhysXOMD* pTempOMD = static_cast<LabPhysXOMD*>(m_aObjects[i]->GetMgrData());
		assert(0 != pTempOMD);
		assert(-1 == pTempOMD->GetShotMgrIdx());
		pTempOMD->SetObjMgrIdx(i);

		LabPhysXOMD* pEndOMD = static_cast<LabPhysXOMD*>(m_aObjects[idxEnd]->GetMgrData());
		assert(0 != pEndOMD);
		assert(-1 == pEndOMD->GetShotMgrIdx());
		pEndOMD->SetObjMgrIdx(idxEnd);

		--idxEnd;
	}

	idxEnd++;
	if (idxEnd < nMCount)
		ReleaseObjectToEnd(idxEnd);

	ReleaseAllShotObjects();
}

bool CPhysXObjMgr::SyncDataPhysXToGraphic()
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->SyncDataPhysXToGraphic();

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->SyncDataPhysXToGraphic();
	return true;
}

bool CPhysXObjMgr::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const ObjTypeID filter) const
{
	const void* backup = hitInfo.UserData;
	const bool hasFilter = (RawObjMgr::OBJ_TYPEID_MANAGER == filter)? false : true;

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		if (hasFilter && filter != m_aObjects[i]->GetProperties()->GetObjType())
			continue;

		m_aObjects[i]->RayTraceObj(ray, hitInfo);
	}

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
	{
		if (hasFilter && filter != m_objShotStuffs[j]->GetProperties()->GetObjType())
			continue;

		m_objShotStuffs[j]->RayTraceObj(ray, hitInfo);
	}

	if (backup == hitInfo.UserData)
		return false;
	return true;
}

bool CPhysXObjMgr::SaveSerialize(NxStream& stream) const
{
	int nMaxLiveCount = SizeModelLiveObjs();
	stream.storeDword(nMaxLiveCount);

	IPropObjBase* pProp = 0;
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		pProp = m_aObjects[i]->GetProperties();
		if (!pProp->IsObjAliveState())
			continue;

		stream.storeDword(pProp->GetObjType());
		m_aObjects[i]->SaveSerialize(stream);
	}

	RawObjMgr::GetInstance()->SaveSerialize(stream);
	return true;
}

bool CPhysXObjMgr::LoadDeserialize(NxStream& stream, bool& outIsLowVersion)
{
	assert(0 == SizeAllLiveObjs());

	const int iObjNum = stream.readDword();
	for (int i = 0; i < iObjNum; ++i)
	{
		ObjTypeID iType = ObjTypeID(stream.readDword());
		IPhysXObjBase* pObject = CreateLabPhysXObject(iType);
		if(0 != pObject)
		{
			if(pObject->LoadDeserialize(stream, outIsLowVersion))
			{
				pObject->GetProperties()->SetObjState(IPropObjBase::OBJECT_STATE_SLEEP);
				continue;
			}
			
			a_LogOutput(1, "%s: load object fail! idx: %d", __FUNCTION__, i);
			ReleaseLabPhysXObject(pObject);
		}
	}

	RawObjMgr::GetInstance()->LoadDeserialize(stream, outIsLowVersion);
	return true;
}

bool CPhysXObjMgr::CheckDefPropsUpdate(IEngineUtility& eu, APhysXScene& scene)
{
	bool bRtn = false;
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		if (m_aObjects[i]->CheckDefPropsUpdate(eu, scene))
			bRtn = true;
	}
	return bRtn;
}

int CPhysXObjMgr::SizeAllLiveObjs() const
{
	int nLiveObjs = SizeModelLiveObjs();
	IPropObjBase* pProp = 0;

	int MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
	{
		pProp = m_objShotStuffs[j]->GetProperties();
		if (pProp->IsObjAliveState())
			++nLiveObjs;
	}
	return nLiveObjs;
}

int CPhysXObjMgr::SizeModelLiveObjs() const
{
	int nLiveObjs = 0;
	IPropObjBase* pProp = 0;

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		pProp = m_aObjects[i]->GetProperties();
		if (pProp->IsObjAliveState())
			++nLiveObjs;
	}

	return nLiveObjs;
}

IPhysXObjBase* CPhysXObjMgr::GetObject(const int index) const
{
	int MaxCount = m_aObjects.GetSize();
	if (index < MaxCount)
		return m_aObjects[index];

	int idxShot = index - MaxCount;
	if (idxShot >= m_objShotStuffs.GetSize())
		return 0;

	return m_objShotStuffs[idxShot];
}

IPhysXObjBase* CPhysXObjMgr::GetObject(NxActor& actor)
{
	// note here: we doesn't remap NxActor when PhysXObj is to die or releasing in runtime.
	// So there are some NxActor* and IPhysXObjBase* that are not available in the map.
	std::map<NxActor*, IPhysXObjBase*>::const_iterator it = m_ActorToModel.find(&actor);
	if (it == m_ActorToModel.end())
	{
		ReMapNxActor(&actor);
		it = m_ActorToModel.find(&actor);
	}
	return (it == m_ActorToModel.end())? 0 : it->second;
}

void CPhysXObjMgr::OnReleaseModel()
{
	ReleaseAllRuntimeObjects();

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->ToDie();
}

bool CPhysXObjMgr::OnEnterRuntime(APhysXScene& aPhysXScene)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->EnterRuntime(aPhysXScene);

	ReMapNxActor();
	assert(0 == m_objShotStuffs.GetSize());
	m_propMgr.m_dmCurrent = m_propMgr.m_dmDefOnSim;
	return true;
}

void CPhysXObjMgr::OnLeaveRuntime()
{
	ReleaseAllRuntimeObjects();

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->LeaveRuntime();
}

bool CPhysXObjMgr::OnInstancePhysXObj(APhysXScene& aPhysXScene)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->InstancePhysXObj(aPhysXScene);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->InstancePhysXObj(aPhysXScene);

	ReMapNxActor();
	m_propMgr.m_Status.RaiseFlag(OBFI_DISABLE_DEF_DMONSIM);
	return true;
}

void CPhysXObjMgr::OnReleasePhysXObj()
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->ReleasePhysXObj();

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->ReleasePhysXObj();
}

void CPhysXObjMgr::ReMapNxActor(NxActor* pTarget)
{
	m_ActorToModel.clear();
	APtrArray<NxActor*> temp;

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		temp.RemoveAll(false);
		int nCount = m_aObjects[i]->GetNxActors(temp, pTarget);
		for (int j = 0; j < nCount; ++j)
			m_ActorToModel[temp[j]] = m_aObjects[i];
	}

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
	{
		temp.RemoveAll(false);
		int nCount = m_objShotStuffs[j]->GetNxActors(temp, pTarget);
		for (int k = 0; k < nCount; ++k)
			m_ActorToModel[temp[k]] = m_objShotStuffs[j];
	}
}

int CPhysXObjMgr::GetNxActors(APtrArray<NxActor*>& outVec, NxActor* pTestActor) const
{
	int rtn = 0;

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		rtn += m_aObjects[i]->GetNxActors(outVec, pTestActor);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		rtn += m_objShotStuffs[j]->GetNxActors(outVec, pTestActor);
	return rtn;
}

bool CPhysXObjMgr::OnTickMove(float deltaTimeSec)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->TickMove(deltaTimeSec);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->TickMove(deltaTimeSec);
	return true;
}

bool CPhysXObjMgr::OnTickAnimation(const unsigned long deltaTime)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->TickAnimation(deltaTime);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->TickAnimation(deltaTime);
	return true;
}

void CPhysXObjMgr::OnSetPos(const A3DVECTOR3& vPos)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->GetProperties()->SetPos(vPos);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->GetProperties()->SetPos(vPos);
}

void CPhysXObjMgr::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->GetProperties()->SetDirAndUp(vDir, vUp);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->GetProperties()->SetDirAndUp(vDir, vUp);
}

void CPhysXObjMgr::OnSetPose(const A3DMATRIX4& matPose)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->GetProperties()->SetPose(matPose);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->GetProperties()->SetPose(matPose);
}

bool CPhysXObjMgr::OnSetScale(const float& scale)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->GetProperties()->SetScale(scale);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->GetProperties()->SetScale(scale);

	return true;
}

void CPhysXObjMgr::OnChangeDrivenMode(const DrivenMode& dmNew)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < m_aObjects.GetSize(); ++i)
		m_aObjects[i]->GetProperties()->SetDrivenMode(dmNew);
	// do not change driven mode in m_objShotStuffs
}

bool CPhysXObjMgr::OnWakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->WakeUp(eu, pPSToRuntime);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->WakeUp(eu, pPSToRuntime);
	return true;
}

bool CPhysXObjMgr::OnResetPose()
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->ResetPose();

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->ResetPose();

	return false;
}
/*
void CPhysXObjMgr::OnControlClothes(const bool IsClose)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->ControlClothes(IsClose);
	
	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->ControlClothes(IsClose);
}

void CPhysXObjMgr::UpdateNeighborObjects(const A3DVECTOR3& vCenter, const float radius, CRender& render, APhysXScene& aPhysXScene, const bool bForceAll, BackgroundTask* pBKTask)
{
	static A3DVECTOR3 LastCenter;
	if (!bForceAll)
	{
		if (vCenter == LastCenter)
			return;
	}

	A3DVECTOR3 vDist;
	const float SquareR = radius * radius;

	IPhysXObjBase* pObj = 0;
	if (0 == pBKTask)
	{
		int MaxCount = m_aObjects.GetSize();
		for (int i = 0; i < MaxCount; ++i)
			m_aObjects[i]->UpdateStateSync(vCenter, SquareR, render, aPhysXScene);
		MaxCount = m_objShotStuffs.GetSize();
		for (int j = 0; j < MaxCount; ++j)
			m_objShotStuffs[j]->UpdateStateSync(vCenter, SquareR, render, aPhysXScene);
	}
	else
	{
		int MaxCount = m_aObjects.GetSize();
		for (int i = 0; i < MaxCount; ++i)
			m_aObjects[i]->UpdateStateAsync(vCenter, SquareR, *pBKTask);
		MaxCount = m_objShotStuffs.GetSize();
		for (int j = 0; j < MaxCount; ++j)
			m_objShotStuffs[j]->UpdateStateAsync(vCenter, SquareR, *pBKTask);
	}
	LastCenter = vCenter;
}
*/
//void CPhysXObjMgr::AddCMM(MMPolicy& mmp)
//{
//	IPhysXObjBase* pObj = 0;
//	int MaxCount = m_aObjects.GetSize();
//	for (int i = 0; i < MaxCount; ++i)
//	{
		// Create CC demo stuff
/*
		pObj = m_aObjects[i];
		A3DVECTOR3 pos = pObj->GetPos();
		if (-340 > pos.x && 260 < pos.z)
			pObj->SetControlGroup(1);
		else if (-330 > pos.x && 260 < pos.z)
			pObj->SetControlGroup(2);
		else if (-320 > pos.x && 260 < pos.z)
			pObj->SetControlGroup(3);
		else if (-310 > pos.x && 260 < pos.z)
			pObj->SetControlGroup(4);
		else
			continue;
		pObj->SetMotionDistance(100);
*/
//		mmp.Add(*m_aObjects[i]);
//	}
//}
/*(
void CPhysXObjMgr::ControlWeather(const bool Enable)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		if (OBJ_TYPEID_PARTICLE != m_aObjects[i]->GetObjType())
			continue;
		
		CPhysXObjParticle* pObj = static_cast<CPhysXObjParticle*>(m_aObjects[i]);
		pObj->ControlState(Enable);
	}
}
*/
