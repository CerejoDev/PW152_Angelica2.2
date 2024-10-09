/*
* FILE: ObjTask.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/06/15
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#include "stdafx.h"

bool ITask::IsRightType(IObjBase* pObj) const
{
	if (pObj->GetProperties()->GetObjType() & m_TypeRequirement)
		return true;
	return false;
}

void TaskSleepToDie::Execute(IObjBase* pObj) const
{
	if (m_IsSleep)
		pObj->Sleep(m_IsAsync);
	else
		pObj->ToDie(m_IsAsync);
}

void TaskWakeUp::Execute(IObjBase* pObj) const
{
	pObj->WakeUp(m_eu, m_pPSToR);
}

void TaskRevive::Execute(IObjBase* pObj) const
{
	pObj->Revive(m_eu);
}

void TaskEnterLeaveRuntime::Execute(IObjBase* pObj) const
{
	if (m_IsEn)
		pObj->EnterRuntime(*m_pScene);
	else
		pObj->LeaveRuntime();
}

void TaskRayTrace::Execute(IObjBase* pObj) const
{
	pObj->RayTraceObj(*m_pRay, *m_pHitInfo, m_getRootObj, m_objTypeFilter);
}

void TaskCheckDefPropUpdate::Execute(IObjBase* pObj) const
{
	IPhysXObjBase* pPO = dynamic_cast<IPhysXObjBase*>(pObj);
	bool bRtn = pPO->CheckDefPropsUpdate(m_eu, m_scene);
	m_Result.CheckResultValue(*pObj, bRtn);
}

void TaskPhysXObjOnOff::Execute(IObjBase* pObj) const
{
	IPhysXObjBase* pPO = dynamic_cast<IPhysXObjBase*>(pObj);
	if (m_IsOn)
		pPO->InstancePhysXObj(*m_pScene);
	else
		pPO->ReleasePhysXObj();
}

void TaskControlClothes::Execute(IObjBase* pObj) const
{
	IPhysXObjBase* pPO = dynamic_cast<IPhysXObjBase*>(pObj);
	pPO->ControlClothes(m_IsClose);
}

void TaskSetDrivenMode::Execute(IObjBase* pObj) const
{
	IPhysXObjBase* pPO = dynamic_cast<IPhysXObjBase*>(pObj);
	pPO->GetProperties()->SetDrivenMode(m_dm, m_IsOnPlay);
}

void TaskSyncData::Execute(IObjBase* pObj) const
{
	IPhysXObjBase* pPO = dynamic_cast<IPhysXObjBase*>(pObj);
	pPO->SyncDataPhysXToGraphic();
}

