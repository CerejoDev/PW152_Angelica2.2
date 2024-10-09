/*
* FILE: AResFuncs.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/2
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResPreRequireHeader.h"
#include "AResFuncs.h"
#include <iterator>
#include <set>
#include <vector>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

struct AResHandle
{
	std::vector<AResString> m_aFiles;
};

extern AResManager* g_pResMan;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static void 
_AResRetriveAllFiles(AResPtr& pRes, std::set<AResString>& aFiles)
{
	aFiles.insert(pRes->GetFile());
	for (int nRefIdx = 0; nRefIdx < pRes->GetRefResNum(); ++nRefIdx)
	{
		AResPtr pRefRes = pRes->GetRefRes(nRefIdx);
		_AResRetriveAllFiles(pRefRes, aFiles);
	}
}

bool AResInitializeModule(IResInterface* pResEngineProxy)
{
	if (!pResEngineProxy)
		return false;

	AResManager::CreateResMan(pResEngineProxy);
	if (!g_pResMan)
		return false;

	return true;
}

void AResFinalizeModule()
{
	AResManager::ReleaseResMan();
}

bool AResCreateHandle(const char* szPath, AResHandle** pResHandle)
{
	if (!pResHandle || !g_pResMan)
		return false;

	*pResHandle = new AResHandle();
	AResPtr pRes = g_pResMan->LoadRes(szPath, true);
	if (pRes == ARes::NullObject())
		return false;
	
	std::set<AResString> aFiles;
	_AResRetriveAllFiles(pRes, aFiles);
	(*pResHandle)->m_aFiles.reserve(aFiles.size());
	
	std::copy(aFiles.begin(), aFiles.end(), std::back_inserter((*pResHandle)->m_aFiles));
	return true;
}

int AResGetFileNum(AResHandle* pResHandle)
{
	return static_cast<int>(pResHandle->m_aFiles.size());
}

bool AResGetFilePath(AResHandle* pResHandle, int nIdx, char* szPath, int iBufLen)
{
	const int nStrLen = len(pResHandle->m_aFiles[nIdx]);
	if (nStrLen > iBufLen - 1)
	{
		g_pResLog->Log("%s, Not enough buffer.", __FUNCTION__);
		return false;
	}

	strcpy_s(szPath, iBufLen, str(pResHandle->m_aFiles[nIdx]));
	return true;
}

void AResCloseHandle(AResHandle* pResHandle)
{
	delete pResHandle;
}