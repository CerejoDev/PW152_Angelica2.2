/*
* FILE: AResSki.cpp
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
#include "AResSki.h"
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
//	Implement AResSki
//	
///////////////////////////////////////////////////////////////////////////

AResSki::AResSki(AResManager* pResMan)
: base_class(pResMan)
{
}

AResSki::~AResSki(void)
{

}

bool AResSki::Init(const char* szPath)
{
	if (!base_class::Init(szPath))
		return false;

	IResSkin* pResSkin = m_pResMan->GetResInterface()->LoadSki(szPath);
	if (!pResSkin)
	{
		g_pResLog->Log("%s, Failed to load ski file %s", __FUNCTION__, szPath);
		return false;
	}

	AResWrapper _ResWrapper(pResSkin, m_pResMan->GetResInterface());

	//	.sphy file exist?
	char szSPHYPath[MAX_PATH];
	strcpy_s(szSPHYPath, szPath);
	AResChangeFileExt(szSPHYPath, sizeof(szSPHYPath), ".sphy");
	if (AResIsFileExist(szSPHYPath))
	{
		AResPtr pRefSphy = m_pResMan->LoadRes(szSPHYPath, false);
		if (pRefSphy != ARes::NullObject())
			m_aRefRes.push_back(pRefSphy);
		else
		{
			g_pResLog->Log("%s, SPHY file %s exist, but failed to load", __FUNCTION__, szSPHYPath);
		}
	}

	char szSMTLPath[MAX_PATH];
	strcpy_s(szSMTLPath, szPath);
	AResChangeFileExt(szSMTLPath, sizeof(szSMTLPath), ".smtl");
	if (AResIsFileExist(szSMTLPath))
	{
		AResPtr pRefSmtl = m_pResMan->LoadRes(szSMTLPath, false);
		if (pRefSmtl != ARes::NullObject())
			m_aRefRes.push_back(pRefSmtl);
		else
		{
			g_pResLog->Log("%s, SMTL file %s exist, but failed to load", __FUNCTION__, szSMTLPath);
		}
	}

	for (int nMtlIdx = 0; nMtlIdx < pResSkin->GetMaterialNum(); ++nMtlIdx)
	{
		IResFxMaterial* pFxMtl = pResSkin->GetMaterial(nMtlIdx);
		
		for (int nParamIdx = 0; nParamIdx < pFxMtl->GetUIParamNum(); ++nParamIdx)
		{
			IResFxUIParam* pUIParam = pFxMtl->GetUIParam(nParamIdx);
			if (pUIParam->IsParamTexture())
			{
				AResPtr pRefTexFile = m_pResMan->LoadRes(pUIParam->GetTexPath(), true);
				if (pRefTexFile != ARes::NullObject())
					m_aRefRes.push_back(pRefTexFile);
				else
				{
					g_pResLog->Log("%s, Failed to load res %s", __FUNCTION__, pUIParam->GetTexPath());
				}
			}
		}
	}

	return true;
}

void AResSki::Release()
{
	base_class::Release();
	m_aRefRes.clear();
}

int AResSki::GetRefResNum() const
{
	return static_cast<int>(m_aRefRes.size());
}

AResPtr AResSki::GetRefRes(int nIdx) const
{
	return m_aRefRes[nIdx];
}