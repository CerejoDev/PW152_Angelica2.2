/*
* FILE: CloneOperation.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/07/07
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSXLAB_CLONEOPERATION_H_
#define _APHYSXLAB_CLONEOPERATION_H_

class IObjBase;

class ICloneReport
{
public:
	struct CPair
	{
		IObjBase* pSource;
		IObjBase* pClone;
	};
	virtual void OnCloneNotify(unsigned int nbEntities, CPair* entities) = 0;
};

class CloneOpt
{
public:
	CloneOpt();
	void Register(ICloneReport* pCR);
	void UnRegister(ICloneReport* pCR);

	IObjBase* CloneObject(LoadPackage& lp, IObjBase* pObj);
	IObjBase* CloneObjects(LoadPackage& lp, const CPhysXObjSelGroup& selGroup, std::vector<IObjBase*>& outCloneObjs);

private:
	void GetPathIndex(const IObjBase& obj, const std::vector<IObjBase*>& samples, std::vector<int>& outIdxOrder);
	bool GetPathIndex(const IObjBase& obj, const IObjBase* pSample, std::vector<int>& outIdxOrder);
	IObjBase* SetPathIndex(const std::vector<int>& idxOrder, const std::vector<IObjBase*>& targets);
	IObjBase* SetPathIndex(int idxChild, IObjBase* pTarget);
	
private:
	bool m_EnableCRs;
	int	 m_nbCRs;
	APtrArray<ICloneReport*> m_CRs;
};

#endif