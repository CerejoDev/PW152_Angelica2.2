/*
* FILE: AResSmd.cpp
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
#include "AResManager.h"
#include "AResSmd.h"

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
//	Implement AResSmd
//	
///////////////////////////////////////////////////////////////////////////

AResSmd::AResSmd(AResManager* pResMan)
: base_class(pResMan)
{
}

AResSmd::~AResSmd(void)
{

}

bool AResSmd::Init(const char* szPath)
{
	if (!base_class::Init(szPath))
		return false;

	IResSkinModel* pSmd = m_pResMan->GetResInterface()->LoadSmd(szPath);
	if (!pSmd)
	{
		g_pResLog->Log("%s, Failed to load smd res: %s", __FUNCTION__, szPath);
		return false;
	}

	AResWrapper _ResWrapper(pSmd, m_pResMan->GetResInterface());

	//	.bon file
	const char* szBonFile = pSmd->GetSkeletonFileName();
	AResPtr pRefBone = m_pResMan->LoadRes(szBonFile, true);
	if (pRefBone != ARes::NullObject())
	{
		m_aRefRes.push_back(pRefBone);
	}
	else
	{
		g_pResLog->Log("%s, Failed to load res %s", __FUNCTION__, szBonFile);
		return false;
	}

	//	.ski files 
	for (int nSkiIdx = 0; nSkiIdx < pSmd->GetSkinNum(); ++nSkiIdx)
	{
		const IResSkin* pSki = pSmd->GetSkin(nSkiIdx);
		AResPtr pRefSki = m_pResMan->LoadRes(pSki->GetFileName(), true);
		if (pRefSki != ARes::NullObject())
		{
			m_aRefRes.push_back(pRefSki);
		}
		else
		{
			g_pResLog->Log("%s, Failed to load res %s", __FUNCTION__, pSki->GetFileName());
		}
	}

	//	.stck files
	for (int nStckIdx = 0; nStckIdx < pSmd->GetTrackSetFileNum(); ++nStckIdx)
	{
		AResPtr pRefStck = m_pResMan->LoadRes(pSmd->GetTrackSetFile(nStckIdx), true);
		if (pRefStck == ARes::NullObject())
		{
			g_pResLog->Log("%s, Failed to load res %s", __FUNCTION__, pSmd->GetTrackSetFile(nStckIdx));
			continue;
		}

		m_aRefRes.push_back(pRefStck);
	}

	return true;
}

void AResSmd::Release()
{
	base_class::Release();
	m_aRefRes.clear();
}

int AResSmd::GetRefResNum() const
{
	return static_cast<int>(m_aRefRes.size());
}

AResPtr AResSmd::GetRefRes(int nIdx) const
{
	return m_aRefRes[nIdx];
}
