#include "stdafx.h"
#include "Recover.h"

ModelBreakReport::BrokenInfo::BrokenInfo()
{
	m_pDynObj = 0;
	m_pose.Identity();
	m_CountFirstB = -1;
	m_CountAllB = -1;
}

ModelBreakReport::ModelBreakReport()
{
	m_bEnable = true;
	m_maxFirstBreak = 200;
	m_maxAllBreak = 20;
}

void ModelBreakReport::OnFirstBreakNotify(APhysXBreakable* pBreakable)
{
	if (!m_bEnable)
		return;

	if (0 == pBreakable->mBreakUserData)
		return;

	CPhysXObjDynamic* pObj = static_cast<CPhysXObjDynamic*>(pBreakable->mBreakUserData);
	int nCount = m_objBroken.size();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_objBroken[i].m_pDynObj == pObj)
			return;
	}

	BrokenInfo bi;
	bi.m_pDynObj = pObj;
	pObj->GetGPose(bi.m_pose);
	bi.m_CountFirstB = 0;
	m_objBroken.push_back(bi);
}

void ModelBreakReport::OnAllBreakNotify(APhysXBreakable* pBreakable)
{
	if (0 == pBreakable->mBreakUserData)
		return;

	CPhysXObjDynamic* pObj = static_cast<CPhysXObjDynamic*>(pBreakable->mBreakUserData);
	int nCount = m_objBroken.size();
	for (int i = 0; i < nCount; ++i)
	{
		if (m_objBroken[i].m_pDynObj == pObj)
		{
			m_objBroken[i].m_CountAllB = 0;
			return;
		}
	}
}

void ModelBreakReport::TickBroken(float dtSec)
{
	int idx = 0;
	int nCount = m_objBroken.size();
	while (idx < nCount)
	{
		if (!(0 > m_objBroken[idx].m_CountFirstB))
			m_objBroken[idx].m_CountFirstB += dtSec;
		if (!(0 > m_objBroken[idx].m_CountAllB))
			m_objBroken[idx].m_CountAllB += dtSec;
		if ((m_objBroken[idx].m_CountFirstB > m_maxFirstBreak) || (m_objBroken[idx].m_CountAllB > m_maxAllBreak))
		{
			OnRecover(m_objBroken[idx]);
			if (idx != nCount - 1)
				m_objBroken[idx] = m_objBroken[nCount - 1];
			m_objBroken.pop_back();
			nCount = m_objBroken.size();
		}
		else
		{
			++idx;
		}
	}
}

void ModelBreakReport::OnRecover(const BrokenInfo& bi)
{
	CPhysXObjDynamic* pDynObj = bi.m_pDynObj;
	APhysXScene* pScene = pDynObj->GetScene()->GetAPhysXScene();
	IPropPhysXObjBase* pProp = pDynObj->GetProperties();
	IPropPhysXObjBase::DrivenMode dmback = pProp->GetDrivenMode(false);
	pDynObj->ReleasePhysXObj();
	if (!pProp->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION))
		pProp->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION);
	pDynObj->SetGPose(bi.m_pose);
	pProp->SetDrivenMode(dmback);
	pDynObj->Tick(0.0);
	if (pDynObj->GetScene()->QueryRPTStateEnable(pDynObj->GetGPos()))
		pDynObj->InstancePhysXObj(*pScene);
}

void ModelBreakReport::ClearAllBroken()
{
	m_objBroken.clear();
}

void ModelBreakReport::SetRecoverInterval(float t, bool bIsFirstBreak)
{
	if (bIsFirstBreak)
		m_maxFirstBreak = t;
	else
		m_maxAllBreak = t;
}

float ModelBreakReport::GetRecoverInterval(bool bIsFirstBreak) const
{
	if (bIsFirstBreak)
		return m_maxFirstBreak;
	return m_maxAllBreak;
}

