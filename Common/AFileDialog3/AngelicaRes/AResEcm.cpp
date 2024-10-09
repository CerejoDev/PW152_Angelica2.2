/*
* FILE: AResEcm.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/1
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResPreRequireHeader.h"
#include "AResEcm.h"
#include "AResManager.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AResEcm
//	
///////////////////////////////////////////////////////////////////////////

AResEcm::AResEcm(AResManager* pResMan)
: base_class(pResMan)
{
}

AResEcm::~AResEcm(void)
{
}

bool AResEcm::Init(const char* szPath)
{
	if (!base_class::Init(szPath))
		return false;

//	ASSERT( FALSE && " Add chf " );

	IResECModel* pResEcm = m_pResMan->GetResInterface()->LoadEcm(szPath);
	if (!pResEcm)
	{
		g_pResLog->Log("%s, Failed to load Ecm %s", __FUNCTION__, szPath);
		return false;
	}

	AResWrapper _ResWrapper(pResEcm, m_pResMan->GetResInterface());

	//	Load smd resource
	AResPtr pRefSmd = m_pResMan->LoadRes(pResEcm->GetSkinModelFileName(), true);
	if (pRefSmd != ARes::NullObject())
		m_aRefRes.push_back(pRefSmd);

	char szMphyFile[1024];
	strcpy_s(szMphyFile, sizeof(szMphyFile), szPath);
	AResChangeFileExt(szMphyFile, sizeof(szMphyFile), ".mphy");
	AResPtr pRefPhys = m_pResMan->LoadRes(szMphyFile, false);
	if (pRefPhys != ARes::NullObject())
	{
		// has mphy file
		m_aRefRes.push_back(pRefPhys);
	}

	char szECSFile[1024];
	strcpy_s(szECSFile, sizeof(szECSFile), szPath);
	AResChangeFileExt(szECSFile, sizeof(szECSFile), ".ecs");
	AResPtr pRefECS = m_pResMan->LoadRes(szECSFile, false);
	if (pRefECS != ARes::NullObject())
	{
		// has ecs file
		m_aRefRes.push_back(pRefECS);
	}

	AResPtr pRefIK = m_pResMan->LoadRes(pResEcm->GetIKSolverFile(), false);
	if (pRefIK != ARes::NullObject())
	{
		// has ik file
		m_aRefRes.push_back(pRefIK);
	}

	//	for child models
	for (int nChildIdx = 0; nChildIdx < pResEcm->GetChildCount(); ++nChildIdx)
	{
		AResPtr pRefChildEcm = m_pResMan->LoadRes(pResEcm->GetChildModelPath(nChildIdx), true);
		if (pRefChildEcm != ARes::NullObject())
			m_aRefRes.push_back(pRefChildEcm);
	}

	//	for additional skins
	for (int iAddiSkiIdx = 0; iAddiSkiIdx < pResEcm->GetAdditionalSkinNum(); ++iAddiSkiIdx)
	{
		char szAdditionalPath[MAX_PATH];
		if (!pResEcm->GetAdditionalSkinPath(iAddiSkiIdx, szAdditionalPath, sizeof(szAdditionalPath)))
			continue;

		AResPtr pRefAddiSki = m_pResMan->LoadRes(szAdditionalPath, false);
		if (pRefAddiSki == ARes::NullObject())
			continue;

		m_aRefRes.push_back(pRefAddiSki);
	}

	//	for gfxs
	for (int nCoGfxIdx = 0; nCoGfxIdx < pResEcm->GetCoGfxPathNum(); ++nCoGfxIdx)
	{
		char szGfxPath[MAX_PATH];
		if (!pResEcm->GetCoGfxPath(nCoGfxIdx, szGfxPath, sizeof(szGfxPath)))
			continue;

		AResPtr pRefGfx = m_pResMan->LoadRes(szGfxPath, true);
		if (pRefGfx == ARes::NullObject())
			continue;

		m_aRefRes.push_back(pRefGfx);
	}

	//	for all comact events
	for (int nComActIdx = 0; nComActIdx < pResEcm->GetComActCount(); ++nComActIdx)
	{
		const IResComAct* pComAct = pResEcm->GetComAct(nComActIdx);
		
		for (int nEventIdx = 0; nEventIdx < pComAct->GetEventCount(); ++nEventIdx)
		{
			IResComActEvent* pEventInfo = pComAct->GetEvent(nEventIdx);
			
			for (int nResPathIdx = 0; nResPathIdx < pEventInfo->GetPathCount(); ++nResPathIdx)
			{
				char szResPath[MAX_PATH];
				AResPtr pRefRes = m_pResMan->LoadRes(pEventInfo->GetPath(nResPathIdx, szResPath, sizeof(szResPath)), true);
				if (pRefRes == ARes::NullObject())
				{
					g_pResLog->Log("%s, Failed to load GFX/SFX/ATT Resource: %s", __FUNCTION__, szResPath);
					continue;
				}

				m_aRefRes.push_back(pRefRes);
			}
		}
	}

	return true;
}

void AResEcm::Release()
{
	m_aRefRes.clear();
}

int AResEcm::GetRefResNum() const
{
	return static_cast<int>(m_aRefRes.size());
}

AResPtr AResEcm::GetRefRes(int nIdx) const
{
	return m_aRefRes[nIdx];
}