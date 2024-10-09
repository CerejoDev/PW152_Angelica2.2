/*
* FILE: AResECModelImpl.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/6
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResEnginePreRequireHeader.h"
#include "AResInterfaceImpl.h"
#include "AResECModelImpl.h"
#include "AResComActImpl.h"

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
//	Implement AResECModelImpl
//	
///////////////////////////////////////////////////////////////////////////

AResECModelImpl::AResECModelImpl(AResInterfaceImpl* pResImpl)
: m_pECModel(NULL)
, m_pResImpl(pResImpl)
{
	assert( m_pResImpl );
}

AResECModelImpl::~AResECModelImpl(void)
{
	A3DRELEASE(m_pECModel);
	for (ResComActArray::iterator itr = m_aResComActs.begin()
		; itr != m_aResComActs.end()
		; ++itr)
	{
		A3DRELEASE(*itr);
	}
	m_aResComActs.clear();
}

bool AResECModelImpl::Init(const char* szPath)
{
	std::auto_ptr<CECModel> pECModel(new CECModel);
	if (!pECModel->Init(m_pResImpl->GetA3DGfxEngine(), m_pResImpl->GetPhysXShell())
		|| !pECModel->Load(szPath, true, 0, true, true, false))
	{
		return false;
	}

	CoGfxMap& coGfxMap = pECModel->GetStaticData()->GetCoGfxMap();
	for (CoGfxMap::iterator itr = coGfxMap.begin()
		; itr != coGfxMap.end()
		; ++itr)
	{
		GFX_INFO* pInfo = itr->second;
		char szPath[MAX_PATH];
		_snprintf_s(szPath, sizeof(szPath), "Gfx\\%s", pInfo->GetFilePath());
		m_aCoGfxPaths.push_back(szPath);
	}

	for (int iComActIdx = 0; iComActIdx < pECModel->GetComActCount(); ++iComActIdx)
	{
		A3DCombinedAction* pComAct = pECModel->GetComActByIndex(iComActIdx);
		std::auto_ptr<AResComActImpl> pResComAct(new AResComActImpl());
		if (!pResComAct->Init(pComAct))
			continue;

		m_aResComActs.push_back(pResComAct.release());
	}

	m_pECModel = pECModel.release();
	return true;
}


const char* AResECModelImpl::GetSkinModelFileName() const
{
	return m_pECModel->GetStaticData()->GetSkinModelFileName();
}

const char* AResECModelImpl::GetIKSolverFile() const
{
	return m_pECModel->GetStaticData()->GetIKSolverFileName();
}

int AResECModelImpl::GetChildCount() const
{
	return m_pECModel->GetChildCount();
}

const char* AResECModelImpl::GetChildModelPath(int nIdx) const
{
	CECModel* pChildModel = m_pECModel->GetChildModel(nIdx);
	assert( pChildModel );
	return pChildModel->GetFilePath();
}


//	for cogfx map
int AResECModelImpl::GetCoGfxPathNum() const
{
	return static_cast<int>(m_aCoGfxPaths.size());
}

bool AResECModelImpl::GetCoGfxPath(int nIdx, char* szPath, int iBufLen) const
{
	if (nIdx < 0 || nIdx >= GetCoGfxPathNum())
		return false;

	_snprintf_s(szPath, iBufLen, iBufLen, "%s", m_aCoGfxPaths[nIdx]);
	return true;
}

//	for additional skins
int AResECModelImpl::GetAdditionalSkinNum() const
{
	return m_pECModel->GetAdditionalSkinCount();
}

//	get additional skins 
bool AResECModelImpl::GetAdditionalSkinPath(int nIdx, char* szPath, int iBufLen) const
{
	if (nIdx < 0 || nIdx >= GetAdditionalSkinNum())
		return false;

	_snprintf_s(szPath, iBufLen, iBufLen, "%s", m_pECModel->GetAdditionalSkin(nIdx));
	return true;
}

int AResECModelImpl::GetComActCount() const
{
	return static_cast<int>(m_aResComActs.size());
}

const IResComAct* AResECModelImpl::GetComAct(int nIdx) const
{
	return m_aResComActs[nIdx];
}

