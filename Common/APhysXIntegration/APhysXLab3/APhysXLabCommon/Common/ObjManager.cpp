/*
 * FILE: ObjManager.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/06/01
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

int ObjMgrData::GetRegionHoldOnIdx(const CRegion& rgn) const
{
	size_t nSize = m_RgnHold.size();
	for (size_t i = 0; i < nSize; ++i)
	{
		if (&rgn == m_RgnHold[i].m_pRgn)
			return m_RgnHold[i].m_idxHoldOn;
	}
	return -1;
}

void ObjMgrData::SetRegionHoldOnIdx(const CRegion& rgn, int idx)
{
	bool toDel = false;
	if (-1 == idx)
		toDel = true;

	size_t nSize = m_RgnHold.size();
	for (size_t i = 0; i < nSize; ++i)
	{
		if (&rgn == m_RgnHold[i].m_pRgn)
		{
			if (toDel)
			{
				m_RgnHold[i] = m_RgnHold[nSize - 1];
				m_RgnHold.pop_back();
			}
			else
			{
				m_RgnHold[i].m_idxHoldOn = idx;
			}
			return;
		}
	}
	m_RgnHold.push_back(RegionHoldOn(rgn, idx));
}

const TCHAR* ObjManager::GetObjTypeIDText(const ObjTypeID id)
{
	switch (id)
	{
	case OBJ_TYPEID_EMPTY:
		return _T("OBJ_EMPTY");
	case OBJ_TYPEID_STATIC:
		return _T("OBJ_STATIC");
	case OBJ_TYPEID_SKINMODEL:
		return _T("OBJ_SKINMODEL");
	case OBJ_TYPEID_ECMODEL:
		return _T("OBJ_ECMODEL");
	case OBJ_TYPEID_PARTICLE:
		return _T("OBJ_PARTICLE");
	case OBJ_TYPEID_FORCEFIELD:
		return _T("OBJ_FORCEFIELD");
	case OBJ_TYPEID_REGION:
		return _T("OBJ_REGION");
	case OBJ_TYPEID_DUMMY:
		return _T("OBJ_DUMMY");
	case OBJ_TYPEID_SMSOCKET:
		return _T("OBJ_SKINMODELSOCKET");
	case OBJ_TYPEID_SOCKETSM:
		return _T("OBJ_SOCKETSKINMODEL");
	case OBJ_TYPEID_SPOTLIGHT:
		return _T("OBJ_SPOTLIGHT");
	case OBJ_TYPEID_CAMERA:
		return _T("OBJ_CAMERA");
	}
	assert(!"Unknown ObjTypeID!");
	return IPropObjBase::szEmpty;
}

bool ObjManager::GetObjTypeID(const TCHAR* pStr, ObjTypeID& outTypeID)
{
	if (0 == a_strcmp(pStr, IPropObjBase::szEmpty))
		return false;

	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_EMPTY)))
	{
		outTypeID = OBJ_TYPEID_EMPTY;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_STATIC)))
	{
		outTypeID = OBJ_TYPEID_STATIC;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_SKINMODEL)))
	{
		outTypeID = OBJ_TYPEID_SKINMODEL;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_ECMODEL)))
	{
		outTypeID = OBJ_TYPEID_ECMODEL;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_PARTICLE)))
	{
		outTypeID = OBJ_TYPEID_PARTICLE;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_FORCEFIELD)))
	{
		outTypeID = OBJ_TYPEID_FORCEFIELD;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_REGION)))
	{
		outTypeID = OBJ_TYPEID_REGION;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_DUMMY)))
	{
		outTypeID = OBJ_TYPEID_DUMMY;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_SMSOCKET)))
	{
		outTypeID = OBJ_TYPEID_SMSOCKET;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_SOCKETSM)))
	{
		outTypeID = OBJ_TYPEID_SOCKETSM;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_SPOTLIGHT)))
	{
		outTypeID = OBJ_TYPEID_SPOTLIGHT;
		return true;
	}
	if (0 == a_strcmp(pStr, GetObjTypeIDText(OBJ_TYPEID_CAMERA)))
	{
		outTypeID = OBJ_TYPEID_CAMERA;
		return true;
	}

	assert(!"Unknown ObjTypeIDText!");
	return false;
}

bool ObjManager::IsPhysXObjType(const ObjTypeID& id)
{
	if (OBJ_TYPEID_PHYSX & id)
		return true;
	return false;
}

void ObjManager::IterateChildren(const IObjBase& theObj, const ITask& task)
{
	IObjBase* pChild = 0;
	int nChildren = theObj.GetChildNum();
	for (int i = 0; i < nChildren; ++i)
	{
		pChild = dynamic_cast<IObjBase*>(theObj.GetChild(i));
		if (task.IsRightType(pChild))
			task.Execute(pChild);
		else
			IterateChildren(*pChild, task);
	}
}

const ObjManager::ObjTypeInfo& ObjManager::GetObjTypeInfo(const ObjTypeID id)
{
	static bool bInited = false;
	static ObjTypeInfo theInfo[6];

	if (bInited)
	{
		int idx = 0;
		switch (id)
		{
		case OBJ_TYPEID_STATIC:
			idx = 1;
			break;
		case OBJ_TYPEID_SKINMODEL:
			idx = 2;
			break;
		case OBJ_TYPEID_ECMODEL:
			idx = 3;
			break;
		case OBJ_TYPEID_PARTICLE:
			idx = 4;
			break;
		case OBJ_TYPEID_FORCEFIELD:
			idx = 5;
			break;
		}
		return theInfo[idx];
	}

	bInited = true;
	theInfo[0].strWCExt	 = IPropObjBase::szEmpty;
	theInfo[0].strFilter = IPropObjBase::szEmpty;
#ifdef _ANGELICA3
	theInfo[1].strWCExt  = _T("*.umd");
	theInfo[1].strFilter = _T("UMD 文件(*.umd)|*.umd||");
	theInfo[2].strWCExt	 = _T("*.smd");
	theInfo[2].strFilter = _T("SMD 文件(*.smd)|*.smd||");
	theInfo[3].strWCExt  = _T("*.ecm");
	theInfo[3].strFilter = _T("ECM3 文件(*.ecm3)|*.ecm3||");
	theInfo[4].strWCExt  = _T("*.gfx3");
	theInfo[4].strFilter = _T("GFX3 文件(*.gfx3)|*.gfx3||");
	theInfo[5].strWCExt  = _T("*.xff");
	theInfo[5].strFilter = _T("XFF 文件(*.xff)|*.xff||");
#else
	theInfo[1].strWCExt  = _T("*.mox");
	theInfo[1].strFilter = _T("MOX 文件(*.mox)|*.mox||");
	theInfo[2].strWCExt  = _T("*.smd");
	theInfo[2].strFilter = _T("SMD 文件(*.smd)|*.smd||");
	theInfo[3].strWCExt  = _T("*.ecm");
	theInfo[3].strFilter = _T("ECM 文件(*.ecm)|*.ecm||");
	theInfo[4].strWCExt  = _T("*.gfx");
	theInfo[4].strFilter = _T("GFX 文件(*.gfx)|*.gfx||");
	theInfo[5].strWCExt  = _T("*.xff");
	theInfo[5].strFilter = _T("XFF 文件(*.xff)|*.xff||");
#endif
	return GetObjTypeInfo(id);
}

bool ObjManager::Condition::CheckCondition(IObjBase* pObj) const
{
	assert(0 != pObj);
	if (m_bIsRTOnly)
	{
		if (pObj->GetProperties()->GetFlags().ReadFlag(OBF_RUNTIME_ONLY))
			return true;
	}
	if (m_bIsShotStuff)
	{
		if (pObj->GetProperties()->GetFlags().ReadFlag(OBF_IS_SHOT_STUFF))
			return true;
	}
	return false;
}

ObjManager* ObjManager::GetInstance()
{
	static ObjManager gobjMgr;
	return  &gobjMgr;
}

ObjManager::ObjManager()
{
	m_CountChecker = 0;
	ClearUniqueSequenceID();
	SetArchiveStyle(true);
}

ObjManager::~ObjManager()
{
	assert(0 == m_CountChecker);
}

void ObjManager::ClearUniqueSequenceID()
{
	m_SequenceID = 0;
}

int ObjManager::GetUniqueSequenceID()
{
	int sid = m_SequenceID;
	++m_SequenceID;
	return sid;
}

bool ObjManager::SaveObject(NxStream& stream, IObjBase* pObj)
{
	if (0 == pObj)
		return false;
	stream.storeDword(pObj->GetProperties()->GetObjType());
	pObj->SaveSerialize(stream);

	IObjBase* pChild = 0;
	int nChildren = pObj->GetChildNum();
	stream.storeDword(nChildren);
	for (int i = 0; i < nChildren; ++i)
	{
		pChild = dynamic_cast<IObjBase*>(pObj->GetChild(i));
		SaveObject(stream, pChild);
	}
	return true;
}

IObjBase* ObjManager::LoadObject(Scene& scene, NxStream& stream, bool& outIsLowVersion, IObjBase* pParent, bool bIsOldArchiveStyle, bool bIsKeepIdx)
{
	bool bIsRoot = true;
	if (0 != pParent)
		bIsRoot = false;

	int iType = stream.readDword();
	if (bIsOldArchiveStyle)
		iType = 1 << (iType - 1);
	ObjTypeID objType = ObjTypeID(iType);

	ObjManager* pObjMgr = GetInstance();
	IObjBase* pObject = pObjMgr->CreateObject(objType, bIsRoot, scene);
	if(0 == pObject)
		return 0;

	if(!pObject->LoadDeserialize(stream, outIsLowVersion, bIsKeepIdx))
	{
		a_LogOutput(1, "%s: load %s type object fail!", __FUNCTION__, GetObjTypeIDText(objType));
		pObjMgr->ReleaseObject(pObject);
		return 0;
	}

	if (0 != pParent)
		pParent->AddChild(pObject);
	pObject->GetProperties()->SetObjState(IPropObjBase::OBJECT_STATE_SLEEP);

	if(pObjMgr->m_StreamObjVer >= 0xAA000002)
	{
		int nChildren = stream.readDword();
		for (int i = 0; i < nChildren; ++i)
			LoadObject(scene, stream, outIsLowVersion, pObject, bIsOldArchiveStyle, bIsKeepIdx);
	}
	return pObject;
}

bool ObjManager::SaveSerialize(NxStream& stream) const
{
	SaveVersion(&stream);

	Vessel allObjects;
	int nLiveCount = GetRootLiveObjs(OBJ_TYPEID_ALL, &allObjects);
	stream.storeDword(nLiveCount);
	int nSize = allObjects.GetSize();
	for (int i = 0; i < nSize; ++i)
		SaveObject(stream, allObjects[i]);

	stream.storeDword(m_SequenceID);
	return true;
}

bool ObjManager::LoadDeserialize(Scene& scene, NxStream& stream, bool& outIsLowVersion)
{
	if (m_ArchiveOldStyle)
	{
		m_StreamObjVer = 0;
		outIsLowVersion = true;
	}
	else
	{
		LoadVersion(&stream);
		if(m_StreamObjVer < GetVersion())
			outIsLowVersion = true;
	}

	assert(0 == GetRootLiveObjs(OBJ_TYPEID_ALL));
	int iObjNum = stream.readDword();
	for (int i = 0; i < iObjNum; ++i)
		LoadObject(scene, stream, outIsLowVersion, 0, m_ArchiveOldStyle, false);

	m_SequenceID = stream.readDword();
	return true;
}

bool ObjManager::CheckDefPropsUpdate(IEngineUtility& eu, APhysXScene& scene)
{
	TaskCheckDefPropUpdate task(eu, scene, true);
	ExecuteTask(task);
	if (0 < task.GetExpectResult().GetNbObjectsEqualExpect())
		return true;
	return false;
}

IObjBase* ObjManager::CreateObject(const ObjTypeID objType)
{
	IObjBase* pObj = 0;
	switch(objType)
	{
	case OBJ_TYPEID_STATIC:
		pObj = new CPhysXObjStatic(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_SKINMODEL:
		pObj = new CPhysXObjSkinModel(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_ECMODEL:
		pObj = new CPhysXObjECModel(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_PARTICLE:
		pObj = new CPhysXObjParticle(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_FORCEFIELD:
		pObj = new CPhysXObjForceField(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_REGION:
		pObj = new CRegion(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_DUMMY:
		pObj = new CDummy(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_SMSOCKET:
		pObj = new ApxObjSkinModelToSocket(GetUniqueSequenceID());
		break;
	case OBJ_TYPEID_SOCKETSM:
		pObj = new ApxObjSocketToSkinModel(GetUniqueSequenceID());
		break;

	case OBJ_TYPEID_SPOTLIGHT:
		pObj = new ApxObjSpotLight(GetUniqueSequenceID());
		break;

	default:
		assert(!"Oops! Unknown ObjType!");
		break;
	}

	if (0 != pObj)
	{
		if (objType != pObj->GetProperties()->GetObjType())
		{
			delete pObj;
			return 0;
		}
		++m_CountChecker;
	}
	return pObj;
}

bool ObjManager::BuildManagerData(IObjBase& object, bool bIsRootObj)
{
	ObjMgrData* pMgrData = object.GetMgrData();
	if (0 != pMgrData)
		return true;

	pMgrData = new ObjMgrData;
	if (0 == pMgrData)
		return false;

	object.SetMgrData(pMgrData);
	if (bIsRootObj)
		AddRootManagerData(object, *pMgrData);
	int idxAll = m_AllObjs.GetSize();
	pMgrData->SetAllIndex(idxAll);
	m_AllObjs.Add(&object);
	return true;
}

void ObjManager::EraseManagerData(IObjBase& object)
{
#ifdef ENABLE_MULLITHREAD_TEST
	APhysXMutexLock autoLocker(m_Locker);
#endif

	ObjMgrData* pMgrData = object.GetMgrData();
	if (0 == pMgrData)
		return;
	--m_CountChecker;

	RemoveRootManagerData(object, *pMgrData);
	int idxAll = pMgrData->GetAllIndex();
	pMgrData->SetAllIndex(-1);
	assert(idxAll < m_AllObjs.GetSize());
	m_AllObjs.RemoveAtQuickly(idxAll);
	if (idxAll < m_AllObjs.GetSize())
	{
		ObjMgrData* pSwapped = m_AllObjs[idxAll]->GetMgrData();
		assert(0 != pSwapped);
		pSwapped->SetAllIndex(idxAll);
	}

	delete pMgrData;
	object.SetMgrData(0);
}

bool ObjManager::AddRootManagerData(IObjBase& object, ObjMgrData& OMD)
{
#ifdef ENABLE_MULLITHREAD_TEST
	APhysXMutexLock autoLocker(m_Locker);
#endif

	int idxRoot = m_RootObjs.GetSize();
	OMD.SetRootIndex(idxRoot);
	m_RootObjs.Add(&object);
	return true;
}

bool ObjManager::RemoveRootManagerData(IObjBase& object, ObjMgrData& OMD)
{
#ifdef ENABLE_MULLITHREAD_TEST
	APhysXMutexLock autoLocker(m_Locker);
#endif

	int idxRoot = OMD.GetRootIndex();
	if (0 > idxRoot)
		return true;

	OMD.SetRootIndex(-1);
	assert(idxRoot < m_RootObjs.GetSize());
	m_RootObjs.RemoveAtQuickly(idxRoot);

	if (idxRoot < m_RootObjs.GetSize())
	{
		ObjMgrData* pSwapped = m_RootObjs[idxRoot]->GetMgrData();
		assert(0 != pSwapped);
		pSwapped->SetRootIndex(idxRoot);
	}
	return true;
}

IObjBase* ObjManager::CreateObject(const ObjTypeID objType, bool bIsRootObj, Scene& scene)
{
	IObjBase* pObj = CreateObject(objType);
	if (0 != pObj)
	{
		pObj->SetScene(&scene);
		if (!BuildManagerData(*pObj, bIsRootObj))
		{
			delete pObj;
			return 0;
		}
	}
	return pObj;
}

void ObjManager::ReleaseObject(IObjBase* pObject)
{
	if (0 == pObject)
		return;

	pObject->Release();
	delete pObject;
}

void ObjManager::ReleaseAllRootObjects()
{
	IObjBase* pObject = 0;
	int nSize = m_RootObjs.GetSize();
	while (0 < nSize)
	{
		ReleaseObject(m_RootObjs[0]);
		--nSize;
	}
	m_ActorToModel.clear();
}

void ObjManager::ReleaseAllShotObjects()
{
	Condition cond;
	cond.SetShotStuffCondition();
	ReleaseConditionObjs(cond);
}

IObjBase* ObjManager::FindObj(const char* objName)
{
	int nSize = m_RootObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		const char* strname = m_RootObjs[i]->GetProperties()->GetObjName_cstr();
		if (strcmp(objName, strname) == 0)
			return m_RootObjs[i];
	}
	return 0;
}

IPhysXObjBase* ObjManager::GetPhysXObject(NxActor& actor) const
{
	NxShape* const* ppShape = actor.getShapes();
	int nShapes = actor.getNbShapes();
	for (int i = 0; i < nShapes; ++i)
	{
		NxShapeType st = ppShape[i]->getType();
		if (NX_SHAPE_HEIGHTFIELD == st)
			return 0;
		if (NX_SHAPE_PLANE == st)
			return 0;
	}

	// note here: we doesn't remap NxActor when PhysXObj is to die or releasing in runtime.
	// So there are some NxActor* and IPhysXObjBase* that are not available in the map.
	std::map<NxActor*, IPhysXObjBase*>::const_iterator it = m_ActorToModel.find(&actor);
	if (it == m_ActorToModel.end() && m_RemapEnable)
	{
		ReMapNxActor(&actor);
		it = m_ActorToModel.find(&actor);
	}
	return (it == m_ActorToModel.end())? 0 : it->second;
}

void ObjManager::ReMapNxActor(NxActor* pTarget) const
{
	ObjManager* pObjsMgr = ObjManager::GetInstance();
	IPhysXObjBase* pPhysXObj = 0;
	m_ActorToModel.clear();
	APtrArray<NxActor*> temp;

	int nAllCount = m_AllObjs.GetSize();
	for (int i = 0; i < nAllCount; ++i)
	{
		if (!pObjsMgr->IsPhysXObjType(m_AllObjs[i]->GetProperties()->GetObjType()))
			continue;

		pPhysXObj = dynamic_cast<IPhysXObjBase*>(m_AllObjs[i]);
		int nCount = pPhysXObj->GetNxActors(temp, pTarget);
		for (int j = 0; j < nCount; ++j)
			m_ActorToModel[temp[j]] = pPhysXObj;
		temp.RemoveAll(false);
	}
	m_RemapEnable = false;
}

CPhysXObjECModel* ObjManager::GetECModelObject(CECModel* pECM) const
{
	if (0 == pECM)
		return 0;

	IPropObjBase* pProp = 0;
	CPhysXObjECModel* pRtn = 0;
	int nSize = m_AllObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		pProp = m_AllObjs[i]->GetProperties();
		if (OBJ_TYPEID_ECMODEL & pProp->GetObjType())
		{
			pRtn = dynamic_cast<CPhysXObjECModel*>(m_AllObjs[i]);
			if (pECM == pRtn->GetECModel())
				return pRtn;
		}
	}
	return 0;
}

int ObjManager::GetRootLiveObjs(int objTypeFilter, Vessel* pOut) const
{
	if (OBJ_TYPEID_EMPTY == objTypeFilter)
		return 0;

	int nRtnCount = 0;
	IPropObjBase* pProp = 0;
	int nSize = m_RootObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		pProp = m_RootObjs[i]->GetProperties();
		if (pProp->IsObjAliveState())
		{
			if (pProp->GetObjType() & objTypeFilter)
			{
				if (0 != pOut)
					pOut->Add(m_RootObjs[i]);
				++nRtnCount;
			}
		}
	}
	return nRtnCount;
}

int ObjManager::GetLiveObjects(APtrArray<IObjBase*>& outObjs, int objTypeFilter) const
{
	if (OBJ_TYPEID_EMPTY == objTypeFilter)
		return 0;

	int nRtnCount = 0;
	IPropObjBase* pProp = 0;
	int nSize = m_AllObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		pProp = m_AllObjs[i]->GetProperties();
		if (pProp->IsObjAliveState())
		{
			if (pProp->GetObjType() & objTypeFilter)
			{
				outObjs.Add(m_AllObjs[i]);
				++nRtnCount;
			}
		}
	}
	return nRtnCount;
}

int ObjManager::GetRootLiveObjects(APtrArray<IObjBase*>& outObjs, int objTypeFilter) const
{
	return GetRootLiveObjs(objTypeFilter, &outObjs);
}

bool ObjManager::EnterRuntime(APhysXScene& aPhysXScene)
{
	int nSize = m_RootObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_RootObjs[i]->EnterRuntime(aPhysXScene);
	ReMapNxActor();
	return true;
}

void ObjManager::LeaveRuntime()
{
	int nSize = m_RootObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_RootObjs[i]->LeaveRuntime();

	Condition cond;
	cond.SetRuntimeCondition();
	ReleaseConditionObjs(cond);
}

void ObjManager::Tick(float dt)
{
#ifdef ENABLE_MULLITHREAD_TEST
	APhysXMutexLock autoLocker(m_Locker);
#endif

	m_RemapEnable = true;
	int nSize = m_RootObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_RootObjs[i]->Tick(dt);
}

void ObjManager::EnableAllPhysX(APhysXScene& aScene)
{
	TaskPhysXObjOnOff task(aScene);
	ExecuteTask(task);
}

void ObjManager::DisableAllPhysX()
{
	TaskPhysXObjOnOff task;
	ExecuteTask(task);
}

void ObjManager::ControlModelClothes(bool bEnable)
{
	TaskControlClothes task(bEnable);
	ExecuteTask(task);
}

void ObjManager::SetDrivenMode(int dm)
{
	IPropPhysXObjBase::DrivenMode dmVal = static_cast<IPropPhysXObjBase::DrivenMode>(dm);
	TaskSetDrivenMode task(dmVal, false);
	ExecuteTask(task);
}

bool ObjManager::SyncDataPhysXToGraphic()
{
	TaskSyncData task;
	ExecuteTask(task);
	return true;
}

bool ObjManager::WakeUp(IEngineUtility& eu, APhysXScene* pPSToRuntime)
{
	//toupdate add filter
	int nSize = m_RootObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_RootObjs[i]->WakeUp(eu, pPSToRuntime);
	return true;
}

bool ObjManager::RayTraceObj(const PhysRay& ray, PhysRayTraceHit& hitInfo, bool getRootObj, int objTypeFilter) const
{
	//toupdate RayTraceBrush???
	void* pBackup = hitInfo.UserData;
	int nSize = m_RootObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
		m_RootObjs[i]->RayTraceObj(ray, hitInfo, getRootObj, objTypeFilter);
	if (pBackup != hitInfo.UserData)
		return true;
	return false;
}

void ObjManager::ReleaseConditionObjs(const Condition& cond)
{
	int idx = 0;
	int nCount = m_AllObjs.GetSize();
	while (idx < nCount)
	{
		if (cond.CheckCondition(m_AllObjs[idx]))
		{
			if (0 < m_AllObjs[idx]->GetChildNum())
				m_AllObjs[idx]->RemoveChildren();
			ReleaseObject(m_AllObjs[idx]);
			nCount = m_AllObjs.GetSize();
			continue;
		}
		++idx;
	}
}

void ObjManager::ExecuteTask(const ITask& task)
{
	IObjBase* pObj = 0;
	int nSize = m_RootObjs.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		pObj = m_RootObjs[i];
		if (task.IsRightType(pObj))
			task.Execute(pObj);
		else
			IterateChildren(*pObj, task);
	}
}

void ObjManager::OnDeletionNotify(APhysXObject& object)
{
	CPhysXObjForceField* pFF = 0;
	int nAllCount = m_AllObjs.GetSize();
	for (int i = 0; i < nAllCount; ++i)
	{
		if (OBJ_TYPEID_FORCEFIELD != m_AllObjs[i]->GetProperties()->GetObjType())
			continue;

		pFF = dynamic_cast<CPhysXObjForceField*>(m_AllObjs[i]);
		if (pFF->OnDeletionNotify(object))
			break;
	}
}

void ObjManager::OnAddChildNotify(ApxObjBase* pParent, ApxObjBase* pChild)
{
	if (0 == pChild)
		return;

	IObjBase* pObj = dynamic_cast<IObjBase*>(pChild);
	if (0 != pObj)
	{
		ObjMgrData* pOMD = pObj->GetMgrData();
		if (0 != pOMD)
			RemoveRootManagerData(*pObj, *pOMD);
	}
}

void ObjManager::OnRemoveChildNotify(ApxObjBase* pParent, ApxObjBase* pChild)
{
	if (0 == pChild)
		return;

	IObjBase* pObj = dynamic_cast<IObjBase*>(pChild);
	if (0 != pObj)
	{
		ObjMgrData* pOMD = pObj->GetMgrData();
		if (0 != pOMD)
			AddRootManagerData(*pObj, *pOMD);
	}
}
