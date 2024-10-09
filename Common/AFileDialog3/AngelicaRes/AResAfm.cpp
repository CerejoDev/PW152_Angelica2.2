/*
* FILE: AResAfm.cpp
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
#include "AResAfm.h"
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
//	Implement AResAfm
//	
///////////////////////////////////////////////////////////////////////////

AResAfm::AResAfm(AResManager* pResMan)
: base_class(pResMan)
{
}

AResAfm::~AResAfm(void)
{
}

bool AResAfm::Init(const char* szPath)
{
	if (!base_class::Init(szPath))
		return false;

	IResFxMaterial* pFxMtl = m_pResMan->GetResInterface()->LoadAfm(szPath);
	if (!pFxMtl)
	{
		g_pResLog->Log("%s, Failed to load FxMaterial file %s", __FUNCTION__, szPath);
		return false;
	}

	const char* szFxFile = pFxMtl->GetFXFileName();
	if (AResIsFileExist(szFxFile))
	{
		AResPtr pRefFxFile = m_pResMan->LoadRes(szFxFile, false);
		g_pResLog->Log("%s, Failed to load .fx res %s", __FUNCTION__, szFxFile);
		return false;
	}
	else
	{
		char szCompFile[MAX_PATH];
		strcpy_s(szCompFile, szFxFile);
		strcat_s(szCompFile, "_x");
		AResPtr pRefFxFile = m_pResMan->LoadRes(szCompFile, false);
		if (pRefFxFile == ARes::NullObject())
		{
			g_pResLog->Log("%s, Failed to load .fx_x res %s", __FUNCTION__, szCompFile);
			return false;
		}

		m_aRefRes.push_back(pRefFxFile);
	}

	for (int nIdx = 0; nIdx < pFxMtl->GetUIParamNum(); ++nIdx)
	{
		IResFxUIParam* pParam = pFxMtl->GetUIParam(nIdx);
		assert( pParam );

		if (pParam->IsParamTexture())
		{
			const char* szTexPath = pParam->GetTexPath();
			AResPtr pRefTexFile = m_pResMan->LoadRes(szTexPath, true);
			if (pRefTexFile != ARes::NullObject())
				m_aRefRes.push_back(pRefTexFile);
			else
			{
				g_pResLog->Log("%s, Failed to load tex res %s", __FUNCTION__, szTexPath);
			}
		}
	}

	return true;
}

void AResAfm::Release()
{
	m_aRefRes.clear();
}

int AResAfm::GetRefResNum() const
{
	return static_cast<int>(m_aRefRes.size());
}

AResPtr AResAfm::GetRefRes(int nIdx) const
{
	return m_aRefRes[nIdx];
}
