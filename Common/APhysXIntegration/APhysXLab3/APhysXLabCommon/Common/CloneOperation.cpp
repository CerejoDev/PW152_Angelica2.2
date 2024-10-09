/*
* FILE: CloneOperation.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/07/07
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#include "stdafx.h"

CloneOpt::CloneOpt()
{
	m_EnableCRs = true;
	m_nbCRs = 0;
}

void CloneOpt::Register(ICloneReport* pCR)
{
	if (0 == pCR)
		return;
	m_CRs.UniquelyAdd(pCR);
	m_nbCRs = m_CRs.GetSize();
}

void CloneOpt::UnRegister(ICloneReport* pCR)
{
	int idx = m_CRs.Find(pCR);
	if(0 > idx)
		return;
	m_CRs.RemoveAtQuickly(idx);
	m_nbCRs = m_CRs.GetSize();
}

IObjBase* CloneOpt::CloneObject(LoadPackage& lp, IObjBase* pObj)
{
	IObjBase* pClone = 0;
	if (0 == pObj)
		return pClone;

	pClone = dynamic_cast<IObjBase*>(pObj->GetCloned());
	if (0 == pClone)
	{
		a_LogOutput(1, "Warning in %s: Clone object fail!", __FUNCTION__);
		return pClone;
	}

	IObjBase* pParent = dynamic_cast<IObjBase*>(pObj->GetParent());
	if (0 != pParent)
		pParent->AddChild(pClone);

	if (!pClone->WakeUp(lp))
	{
		a_LogOutput(1, "Warning in %s: Wake up the cloned object fail!", __FUNCTION__);
		pParent->RemoveChild(pClone);
		ObjManager::GetInstance()->ReleaseObject(pClone);
		pClone = 0;
	}

	if (m_EnableCRs)
	{
		ICloneReport::CPair cp;
		cp.pSource = pObj;
		cp.pClone = pClone;
		for (int i = 0; i < m_nbCRs; ++i)
			m_CRs[i]->OnCloneNotify(1, &cp);
	}
	return pClone;
}

IObjBase* CloneOpt::CloneObjects(LoadPackage& lp, const CPhysXObjSelGroup& selGroup, std::vector<IObjBase*>& outCloneObjs)
{
	std::vector<IObjBase*> pSampleObjs;
	selGroup.GetTopAncestorObjects(pSampleObjs);

	m_EnableCRs = false;
	IObjBase* pClone = 0;
	IObjBase* pParent = 0;
	std::vector<IObjBase*> pCloneObjs;
	const size_t nSampleCount = pSampleObjs.size();
	for (size_t i = 0; i < nSampleCount; ++i)
	{
		pClone = CloneObject(lp, pSampleObjs[i]);
		pCloneObjs.push_back(pClone);
	}
	m_EnableCRs = true;

	int nCount = selGroup.Size();
	std::vector<ICloneReport::CPair> cps;
	cps.reserve(nCount);
	ICloneReport::CPair cpTemp;

	std::vector<int> idxOrder;
	for (int k = 0; k < nCount; ++k)
	{
		idxOrder.clear();
		cpTemp.pSource = selGroup.GetObject(k);
		GetPathIndex(*cpTemp.pSource, pSampleObjs, idxOrder);
		cpTemp.pClone = SetPathIndex(idxOrder, pCloneObjs);
		cps.push_back(cpTemp);
		outCloneObjs.push_back(cpTemp.pClone);
	}

	for (int i = 0; i < m_nbCRs; ++i)
		m_CRs[i]->OnCloneNotify(cps.size(), &cps[0]);

	int idxFocusAncestor = -1;
	selGroup.GetFocusObjectTopAncestor(&idxFocusAncestor);
	assert(-1 != idxFocusAncestor);
	if (0 < idxFocusAncestor)
	{
		assert(idxFocusAncestor < int(outCloneObjs.size()));
		IObjBase* pTemp = outCloneObjs[0];
		outCloneObjs[0] = outCloneObjs[idxFocusAncestor];
		outCloneObjs[idxFocusAncestor] = pTemp;
	}
	return outCloneObjs[0];
}

void CloneOpt::GetPathIndex(const IObjBase& obj, const std::vector<IObjBase*>& samples, std::vector<int>& outIdxOrder)
{
	const size_t nSampleCount = samples.size();
	for (size_t i = 0; i < nSampleCount; ++i)
	{
		outIdxOrder.push_back(i);
		if (GetPathIndex(obj, samples[i], outIdxOrder))
			return;
		outIdxOrder.pop_back();
	}
}

bool CloneOpt::GetPathIndex(const IObjBase& obj, const IObjBase* pSample, std::vector<int>& outIdxOrder)
{
	if (0 == pSample)
		return false;
	if (&obj == pSample)
		return true;

	IObjBase* pChild = 0;
	int nChildren = pSample->GetChildNum();
	for (int i = 0; i < nChildren; ++i)
	{
		outIdxOrder.push_back(i);
		pChild = dynamic_cast<IObjBase*>(pSample->GetChild(i));
		if (GetPathIndex(obj, pChild, outIdxOrder))
			return true;
		outIdxOrder.pop_back();
	}
	return false;
}

IObjBase* CloneOpt::SetPathIndex(const std::vector<int>& idxOrder, const std::vector<IObjBase*>& targets)
{
	const size_t nCount = idxOrder.size();
	assert(0 < nCount);

	int idx = idxOrder[0];
	assert(idx < int(targets.size()));

	IObjBase* pObj = targets[idx];
	for (size_t i = 1; i < nCount; ++i)
	{
		pObj = SetPathIndex(idxOrder[i], pObj);
		if (0 == pObj)
			break;
	}
	return pObj;
}

IObjBase* CloneOpt::SetPathIndex(int idxChild, IObjBase* pTarget)
{
	assert(0 != pTarget);
	int nChild = pTarget->GetChildNum();
	assert(idxChild < nChild);
	ApxObjBase* pApx = pTarget->GetChild(idxChild);
	return dynamic_cast<IObjBase*>(pApx);
}

