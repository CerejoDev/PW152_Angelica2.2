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

void CPhysXObjMgr::OnObjectDeletionNotify(APhysXObject* pObject)
{
	if (0 == pObject)
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
	}
}

const CPhysXObjMgr::ObjInfo& CPhysXObjMgr::GetObjInfo(const ObjTypeID id)
{
	static bool bInited = false;
	static ObjInfo theInfo[OBJ_MAX_BOUND];

	assert(id < OBJ_MAX_BOUND);
	if (bInited)
		return (id == OBJ_MAX_BOUND)? theInfo[0] : theInfo[id];

	bInited = true;
	theInfo[OBJ_TYPEID_MANAGER].strWCExt   = _T("");
	theInfo[OBJ_TYPEID_MANAGER].strFilter  = _T("");
	theInfo[OBJ_TYPEID_STATIC].strWCExt    = _T("*.mox");
	theInfo[OBJ_TYPEID_STATIC].strFilter   = _T("MOX 文件(*.mox)|*.mox||");
	theInfo[OBJ_TYPEID_SKINMODEL].strWCExt = _T("*.smd");
	theInfo[OBJ_TYPEID_SKINMODEL].strFilter= _T("SMD 文件(*.smd)|*.smd||");
	theInfo[OBJ_TYPEID_ECMODEL].strWCExt   = _T("*.ecm");
	theInfo[OBJ_TYPEID_ECMODEL].strFilter  = _T("ECM 文件(*.ecm)|*.ecm||");
	theInfo[OBJ_TYPEID_PARTICLE].strWCExt  = _T("*.gfx");
	theInfo[OBJ_TYPEID_PARTICLE].strFilter = _T("GFX 文件(*.gfx)|*.gfx||");
	theInfo[OBJ_TYPEID_FORCEFIELD].strWCExt  = _T("*.xff");
	theInfo[OBJ_TYPEID_FORCEFIELD].strFilter = _T("XFF 文件(*.xff)|*.xff||");
	theInfo[OBJ_TYPEID_CHB].strWCExt  = _T("*.chb");
	theInfo[OBJ_TYPEID_CHB].strFilter = _T("CHB 文件(*.chb)|*.chb||");
	return GetObjInfo(id);
}

IPhysXObjBase* CPhysXObjMgr::CreateShotObject(const ObjTypeID objType)
{
	return CreateObject(objType, false, true);
}

IPhysXObjBase* CPhysXObjMgr::CreateObject(const ObjTypeID objType, const bool isNPC)
{
	return CreateObject(objType, isNPC, false);
}

IPhysXObjBase* CPhysXObjMgr::CreateObject(const ObjTypeID objType, const bool isNPC, const bool IsShotStuff)
{
	IPhysXObjBase* pObj = 0;
	switch(objType)
	{
	case OBJ_TYPEID_STATIC:
		pObj = new CPhysXObjStatic();
		break;
	case OBJ_TYPEID_SKINMODEL:
		pObj = new CPhysXObjSkinModel(isNPC);
		break;
	case OBJ_TYPEID_ECMODEL:
		pObj = new CPhysXObjECModel(isNPC);
		break;
	case OBJ_TYPEID_PARTICLE:
		pObj = new CPhysXObjParticle();
		break;
	case OBJ_TYPEID_FORCEFIELD:
		pObj = new CPhysXObjForceField(objType);
		break;
	case OBJ_TYPEID_CHB:
		pObj = new CPhysXObjChb();
		break;
	default:
		assert(!"Oops! Unknown PhysXObjType!");
		break;
	}
	
	if (0 != pObj)
	{
		m_ObjState = OBJECT_STATE_ACTIVE;

		if (IsShotStuff)
			m_objShotStuffs.Add(pObj);
		else
			m_aObjects.Add(pObj);
	}
	return pObj;
}

void CPhysXObjMgr::ReleaseObject(IPhysXObjBase* pObject)
{
	if (0 == pObject)
		return;

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		if(pObject != m_aObjects[i])
			continue;

		A3DRELEASE(m_aObjects[i]);
		m_aObjects.RemoveAtQuickly(i);
		if (0 == SizeAll())
			m_ObjState = OBJECT_STATE_SLEEP;
		return;
	}

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
	{
		if(pObject != m_objShotStuffs[j])
			continue;
		
		A3DRELEASE(m_objShotStuffs[j]);
		m_objShotStuffs.RemoveAtQuickly(j);
		if (0 == SizeAll())
			m_ObjState = OBJECT_STATE_SLEEP;
		return;
	}
}

void CPhysXObjMgr::ReleaseObjectToEnd(const int idxFrom)
{
	assert(0 <= idxFrom);
	if (0 >  idxFrom)
		return;

	const int nAllCount = SizeAll();
	assert(idxFrom < nAllCount);
	if (idxFrom >= nAllCount)
		return;
	
	const int nMCount = SizeModels();
	if (idxFrom < nMCount)
	{
		for (int i = idxFrom; i < nMCount; ++i)
			A3DRELEASE(m_aObjects[i]);
		m_aObjects.RemoveAt(idxFrom, nMCount - idxFrom);
		ReleaseAllShotObjects();
	}
	else
	{
		const int iStart = idxFrom - nMCount;
		const int iEnd = nAllCount - nMCount;
		for (int i = iStart; i < iEnd; ++i)
			A3DRELEASE(m_objShotStuffs[i]);
		m_objShotStuffs.RemoveAt(idxFrom - nMCount, nAllCount - idxFrom);
	}

	if (0 == SizeAll())
		m_ObjState = OBJECT_STATE_SLEEP;
	return;
}

void CPhysXObjMgr::ReleaseAllShotObjects()
{
	int MaxCount = m_objShotStuffs.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		A3DRELEASE(m_objShotStuffs[i]);

	m_objShotStuffs.RemoveAll(true);
	if (0 == SizeAll())
		m_ObjState = OBJECT_STATE_SLEEP;
}

void CPhysXObjMgr::ReleaseAllRuntimeObjects()
{
	const int nMCount = SizeModels();
	int idxEnd = nMCount - 1;
	int i = 0;
	while (i <= idxEnd)
	{
		if (!m_aObjects[i]->ReadFlag(OBF_RUNTIME_ONLY))
		{
			++i;
			continue;
		}

		IPhysXObjBase* pTemp = m_aObjects[i];
		m_aObjects[i] = m_aObjects[idxEnd];
		m_aObjects[idxEnd] = pTemp;
		--idxEnd;
	}

	idxEnd++;
	if (idxEnd < nMCount)
		ReleaseObjectToEnd(idxEnd);

	ReleaseAllShotObjects();
}

bool CPhysXObjMgr::LoadDeserialize(CRender& render, NxStream& stream, const unsigned int nVersion, APhysXScene& aPhysXScene)
{
	Release();
	
	int iObjNum = stream.readDword();
	for (int i = 0; i < iObjNum; ++i)
	{
		ObjTypeID iType = ObjTypeID(stream.readDword());
		IPhysXObjBase* pObject = CreateObject(iType);
		if(0 != pObject)
		{
			if(!pObject->LoadDeserialize(render, stream, nVersion, aPhysXScene))
				ReleaseObject(pObject);
		}
	}
	return true;
}

bool CPhysXObjMgr::SaveSerialize(NxStream& stream) const
{
	int MaxCount = m_aObjects.GetSize();
	stream.storeDword(MaxCount);
	for (int i = 0; i < MaxCount; ++i)
	{
		stream.storeDword(m_aObjects[i]->GetObjType());
		m_aObjects[i]->SaveSerialize(stream);
	}

	// currently, do not save the objects in m_objShotStuffs
	return true;
}

void CPhysXObjMgr::OnReleaseModel()
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		A3DRELEASE(m_aObjects[i]);
	m_aObjects.RemoveAll(true);

	ReleaseAllShotObjects();
	m_ObjState = OBJECT_STATE_SLEEP;
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

void CPhysXObjMgr::OnRender(A3DViewport& viewport, A3DWireCollector* pWC, bool isDebug) const
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->Render(viewport, pWC, isDebug);
	
	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->Render(viewport, pWC, isDebug);
}

void CPhysXObjMgr::OnRenderShadow(A3DViewport& viewport) const
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->RenderShadow(viewport);
	
	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->RenderShadow(viewport);
}

void CPhysXObjMgr::OnSetPos(const A3DVECTOR3& vPos)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->SetPos(vPos);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->SetPos(vPos);
}

void CPhysXObjMgr::OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->SetDirAndUp(vDir, vUp);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->SetDirAndUp(vDir, vUp);
}

bool CPhysXObjMgr::OnSetScale(const float& scale)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->SetScale(scale);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->SetScale(scale);

	return true;
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
	RaiseFlag(OBF_CLOSE_DEFAULT_DM);
	return true;
}

void CPhysXObjMgr::OnReleasePhysXObj(const bool RunTimeIsEnd)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->ReleasePhysXObj(RunTimeIsEnd);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->ReleasePhysXObj(RunTimeIsEnd);
}

void CPhysXObjMgr::OnControlClothes(const bool IsClose)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->ControlClothes(IsClose);
	
	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->ControlClothes(IsClose);
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

bool CPhysXObjMgr::OnEnterRuntime(APhysXScene& aPhysXScene)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->EnterRuntime(aPhysXScene);
	
	ReMapNxActor();

	assert(0 == m_objShotStuffs.GetSize());
	return true;
}

void CPhysXObjMgr::OnLeaveRuntime()
{
	ReleaseAllRuntimeObjects();

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->LeaveRuntime();
}

void CPhysXObjMgr::OnSetRefFrameEditMode(const EditType et)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->SetRefFrameEditMode(et);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->SetRefFrameEditMode(et);
}

void CPhysXObjMgr::OnChangeDrivenMode(const DrivenMode dmNew)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < m_aObjects.GetSize(); ++i)
		m_aObjects[i]->SetDrivenMode(dmNew);
	// do not change driven mode in m_objShotStuffs
}

void CPhysXObjMgr::SetDrawModelAABB(const bool bEnable)
{
	if (bEnable)
	{
		int MaxCount = m_aObjects.GetSize();
		for (int i = 0; i < MaxCount; ++i)
			m_aObjects[i]->RaiseFlag(OBF_DRAW_MODEL_AABB);
		
		MaxCount = m_objShotStuffs.GetSize();
		for (int j = 0; j < MaxCount; ++j)
			m_objShotStuffs[j]->RaiseFlag(OBF_DRAW_MODEL_AABB);
	}
	else
	{
		int MaxCount = m_aObjects.GetSize();
		for (int i = 0; i < MaxCount; ++i)
			m_aObjects[i]->ClearFlag(OBF_DRAW_MODEL_AABB);
		
		MaxCount = m_objShotStuffs.GetSize();
		for (int j = 0; j < MaxCount; ++j)
			m_objShotStuffs[j]->ClearFlag(OBF_DRAW_MODEL_AABB);
	}
}

bool CPhysXObjMgr::OnWakeUp(CRender& render, APhysXScene* pPhysXScene)
{
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
		m_aObjects[i]->WakeUp(render, pPhysXScene);

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
		m_objShotStuffs[j]->WakeUp(render, pPhysXScene);
	return true;
}

bool CPhysXObjMgr::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, const ObjTypeID filter) const
{
	hitInfo.UserData = 0;
	const bool hasFilter = (OBJ_TYPEID_MANAGER == filter)? false : true;

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		if (hasFilter && filter != m_aObjects[i]->GetObjType())
			continue;

		m_aObjects[i]->RayTraceObj(ray, hitInfo);
	}

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
	{
		if (hasFilter && filter != m_objShotStuffs[j]->GetObjType())
			continue;

		m_objShotStuffs[j]->RayTraceObj(ray, hitInfo);
	}
	return (0 != hitInfo.UserData);
}

int CPhysXObjMgr::GetNxActors(abase::vector<NxActor*>& outVec, NxActor* pTestActor) const
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

void CPhysXObjMgr::ReMapNxActor(NxActor* pTarget)
{
	m_ActorToModel.clear();
	abase::vector<NxActor*> temp;

	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
		temp.clear();
		int nCount = m_aObjects[i]->GetNxActors(temp, pTarget);
		for (int j = 0; j < nCount; ++j)
			m_ActorToModel[temp[j]] = m_aObjects[i];
	}

	MaxCount = m_objShotStuffs.GetSize();
	for (int j = 0; j < MaxCount; ++j)
	{
		temp.clear();
		int nCount = m_objShotStuffs[j]->GetNxActors(temp, pTarget);
		for (int k = 0; k < nCount; ++k)
			m_ActorToModel[temp[k]] = m_objShotStuffs[j];
	}
}

IPhysXObjBase* CPhysXObjMgr::GetObject(const int index) const
{
	int MaxCount = m_aObjects.GetSize();
	if (index < MaxCount)
		return m_aObjects[index];


	if (index - MaxCount >= m_objShotStuffs.GetSize())
		return 0;

	return m_objShotStuffs[index - MaxCount];
}

IPhysXObjBase* CPhysXObjMgr::GetObject(NxActor& actor)
{
	std::map<NxActor*, IPhysXObjBase*>::const_iterator it = m_ActorToModel.find(&actor);
	if (it == m_ActorToModel.end())
	{
		ReMapNxActor(&actor);
		it = m_ActorToModel.find(&actor);
	}
	return (it == m_ActorToModel.end())? 0 : it->second;
}

void CPhysXObjMgr::AddCMM(MMPolicy& mmp)
{
	IPhysXObjBase* pObj = 0;
	int MaxCount = m_aObjects.GetSize();
	for (int i = 0; i < MaxCount; ++i)
	{
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
		mmp.Add(*m_aObjects[i]);
	}
}

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

