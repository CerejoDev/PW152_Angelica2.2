/*
* FILE: AResGfx.cpp
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
#include "AResGfx.h"
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
//	Implement AResGfx
//	
///////////////////////////////////////////////////////////////////////////

AResGfx::AResGfx(AResManager* pResMan)
: base_class(pResMan)
{
}

AResGfx::~AResGfx(void)
{
}

bool AResGfx::Init(const char* szPath)
{
	if (!base_class::Init(szPath))
		return false;

	IResGfx* pResGfx = m_pResMan->GetResInterface()->LoadGfx(szPath);
	if (!pResGfx)
	{
		g_pResLog->Log("%s, Failed load gfx file %s", __FUNCTION__, szPath);
		return false;
	}

	AResWrapper _ResWrapper(pResGfx, m_pResMan->GetResInterface());

	//	physics file exist?
	char szGphyPath[MAX_PATH];
	strcpy_s(szGphyPath, szPath);
	AResChangeFileExt(szGphyPath, sizeof(szGphyPath), ".gphy");
	if (AResIsFileExist(szGphyPath))
	{
		AResPtr pRefGphy = m_pResMan->LoadRes(szGphyPath, false);
		if (pRefGphy != ARes::NullObject())
			m_aRefRes.push_back(pRefGphy);
		else
		{
			g_pResLog->Log("%s, GPHY file %s is exist but not able to load.", __FUNCTION__, szGphyPath);
		}
	}

	char szGfsPath[MAX_PATH];
	strcpy_s(szGfsPath, szPath);
	AResChangeFileExt(szGfsPath, sizeof(szGfsPath), ".gfs");
	if (AResIsFileExist(szGfsPath))
	{
		AResPtr pRefGfs = m_pResMan->LoadRes(szGfsPath, false);
		if (pRefGfs != ARes::NullObject())
			m_aRefRes.push_back(pRefGfs);
		else
		{
			g_pResLog->Log("%s, GFS file %s is exist but not able to load.", __FUNCTION__, szGfsPath);
		}
	}	

	for (int iEleIdx = 0; iEleIdx < pResGfx->GetElementCount(); ++iEleIdx)
	{
		const IResGfxElement* pEle = pResGfx->GetElement(iEleIdx);
		switch (pEle->GetType())
		{
		case IResGfxElement::TYPE_GFXCONTAINER:
			{
				AResPtr pRefGfx = m_pResMan->LoadRes(pEle->GetSubGfxPath(), true);
				if (pRefGfx != ARes::NullObject())
					m_aRefRes.push_back(pRefGfx);
				else
				{
					g_pResLog->Log("%s, Failed to load res %s", __FUNCTION__, pEle->GetSubGfxPath());
				}
			}
			break;
		case IResGfxElement::TYPE_MODEL:
			{
				AResPtr pRefModel = m_pResMan->LoadRes(pEle->GetSmdPath(), true);
				if (pRefModel != ARes::NullObject())
					m_aRefRes.push_back(pRefModel);
				else
				{
					g_pResLog->Log("%s, Failed to load res %s", __FUNCTION__, pEle->GetSmdPath());
				}
			}
			break;
		case IResGfxElement::TYPE_ECMODEL:
			{
				AResPtr pRefModel = m_pResMan->LoadRes(pEle->GetEcmPath(), true);
				if (pRefModel != ARes::NullObject())
					m_aRefRes.push_back(pRefModel);
				else
				{
					g_pResLog->Log("%s, Failed to load res %s", __FUNCTION__, pEle->GetEcmPath());
				}
			}
			break;
		case IResGfxElement::TYPE_SOUND:
			for (int nIdx = 0; nIdx < pEle->GetRandSoundNum(); ++nIdx)
			{
				const char* szSfxPath = pEle->GetRandSoundPath(nIdx);
				AResPtr pRefSfx = m_pResMan->LoadRes(szSfxPath, true);
				if (pRefSfx != ARes::NullObject())
					m_aRefRes.push_back(pRefSfx);
				else
				{
					g_pResLog->Log("%s, Failed to load res %s", __FUNCTION__, szSfxPath);
				}
			}
			break;
		case IResGfxElement::TYPE_NORMAL:
			for (int nIdx = 0; nIdx < pEle->GetTexNum(); ++nIdx)
			{
				const char* szTexPath = pEle->GetTexPath(nIdx);
				AResPtr pRefTex = m_pResMan->LoadRes(szTexPath, true);
				if (pRefTex != ARes::NullObject())
					m_aRefRes.push_back(pRefTex);
				else
				{
					g_pResLog->Log("%s, Faild to load res %s", __FUNCTION__, szTexPath);
				}
			}
			break;
		default:
			g_pResLog->Log("%s, Unexpected error occured when initializing a gfx resource %s", __FUNCTION__, szPath);
			exit(0);
			break;
		}
	}


	return true;
}

void AResGfx::Release()
{
	base_class::Release();

	m_aRefRes.clear();
}

int AResGfx::GetRefResNum() const
{
	return static_cast<int>(m_aRefRes.size());
}

AResPtr AResGfx::GetRefRes(int nIdx) const
{
	return m_aRefRes[nIdx];
}

