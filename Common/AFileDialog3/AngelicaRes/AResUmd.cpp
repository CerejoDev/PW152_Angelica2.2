/*
* FILE: AResUmd.cpp
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
#include "AResUmd.h"

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
//	Implement AResUmd
//	
///////////////////////////////////////////////////////////////////////////

AResUmd::AResUmd(AResManager* pResMan)
: base_class(pResMan)
{
}

AResUmd::~AResUmd(void)
{
}

bool AResUmd::Init(const char* szPath)
{
	if (!base_class::Init(szPath))
		return false;

	//	.bsp file exist ?
	char szBspPath[MAX_PATH];
	strcpy_s(szBspPath, sizeof(szBspPath), szPath);
	AResChangeFileExt(szBspPath, sizeof(szBspPath), ".bsp");
	AResPtr pRefBsp = m_pResMan->LoadRes(szBspPath, false);
	if (pRefBsp != ARes::NullObject())
	{
		m_aRefRes.push_back(pRefBsp);
	}
	else
	{
		g_pResLog->Log("%s, %s lack bsp file!", __FUNCTION__, szPath);
	}

	//	.chf file exist?
	char szChfPath[MAX_PATH];
	strcpy_s(szChfPath, sizeof(szChfPath), szPath);
	AResChangeFileExt(szChfPath, sizeof(szChfPath), ".chf");
	AResPtr pRefChf = m_pResMan->LoadRes(szChfPath, false);
	if (pRefChf != ARes::NullObject())
	{
		m_aRefRes.push_back(pRefChf);
	}
	else
	{
		g_pResLog->Log("%s, %s lack chf file!", __FUNCTION__, szPath);
	}

	if (!ProcessFile(szPath))
	{
		g_pResLog->Log("%s, Failed to process umd/umds file %s's textures", __FUNCTION__, szPath);
	}
	 
	return true;
}

bool AResUmd::ProcessFile(const char* szPath)
{
	IResUnlitModel* pUmd = m_pResMan->GetResInterface()->LoadUmd(szPath);
	if (!pUmd)
	{
		g_pResLog->Log("%s, Failed to load umd res %s", __FUNCTION__, szPath);
		return false;
	}

	AResWrapper _ResWrapper(pUmd, m_pResMan->GetResInterface());

	ProcessUmdTex(pUmd);
	return true;
}

//	Parse umd file and get all texture out
void AResUmd::ProcessUmdTex(IResUnlitModel* pUmd)
{
	//	texture files
	for (int nMeshIdx = 0; nMeshIdx < pUmd->GetNumMtls(); ++nMeshIdx)
	{
		IResFxMaterial* pMeshMtl = pUmd->GetMeshMaterial(nMeshIdx);
		assert( pMeshMtl );

		for (int nUIParamIdx = 0; nUIParamIdx < pMeshMtl->GetUIParamNum(); ++nUIParamIdx)
		{
			IResFxUIParam* pUIParam = pMeshMtl->GetUIParam(nUIParamIdx);
			assert( pUIParam );

			if (pUIParam->IsParamTexture())
			{
				const char* szTexPath = pUIParam->GetTexPath();
				AResPtr pRefTexFile = m_pResMan->LoadRes(szTexPath, true);
				if (pRefTexFile != ARes::NullObject())
					m_aRefRes.push_back(pRefTexFile);
				else
				{
					g_pResLog->Log("%s, Failed to load tex res %s", __FUNCTION__, szTexPath);
				}
			}
		}
	}
}

void AResUmd::Release()
{
	base_class::Release();
	m_aRefRes.clear();
}

int AResUmd::GetRefResNum() const
{
	return static_cast<int>(m_aRefRes.size());
}

AResPtr AResUmd::GetRefRes(int nIdx) const
{
	return m_aRefRes[nIdx];
}