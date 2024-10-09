/*
* FILE: AResFxMaterialImpl.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/7
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResEnginePreRequireHeader.h"
#include <A3DFXMaterial.h>
#include <A3DFXMaterialMan.h>
#include "AResFxMaterialImpl.h"
#include "AResFxUIParamImpl.h"
#include "AResInterfaceImpl.h"

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
//	Implement AResFxMaterialImpl
//	
///////////////////////////////////////////////////////////////////////////

AResFxMaterialImpl::AResFxMaterialImpl(AResInterfaceImpl* pResImpl)
: m_pResImpl(pResImpl)
{
	assert( pResImpl );
}

AResFxMaterialImpl::~AResFxMaterialImpl(void)
{
	Release();
}

void AResFxMaterialImpl::Release()
{
	for (ResFxUIParamArray::iterator itr = m_aFxUIParams.begin()
		; itr != m_aFxUIParams.end()
		; ++itr)
	{
		delete (*itr);
	}

	m_aFxUIParams.clear();
}

bool AResFxMaterialImpl::Init(const char* szPath)
{
	A3DFXMaterialMan* pFxMtlMan = m_pResImpl->GetA3DEngine()->GetA3DFXMaterialMan();
	A3DFXMaterial* pFxMtl = pFxMtlMan->LoadFXMaterial(szPath);
	if (!pFxMtl)
		return false;

	if (!Init(pFxMtl))
	{
		pFxMtlMan->ReleaseFXMaterial(pFxMtl);
		return false;
	}

	pFxMtlMan->ReleaseFXMaterial(pFxMtl);
	return true;
}

bool AResFxMaterialImpl::Init(A3DFXMaterial* pFx)
{
	m_strFxFileName = pFx->GetFXFileName();
	for (int nIdx = 0; nIdx < pFx->GetUIParamNum(); ++nIdx)
	{
		A3DFXUIParam* pUIParam = pFx->GetUIParam(nIdx);
		std::auto_ptr<AResFxUIParamImpl> pResUIParam(new AResFxUIParamImpl);
		if (!pResUIParam->Init(pUIParam))
			continue;

		m_aFxUIParams.push_back(pResUIParam.release());
	}

	return true;
}


//	Get reference .fx file path
const char* AResFxMaterialImpl::GetFXFileName() const
{
	return m_strFxFileName;
}

//	Get UI param number
int AResFxMaterialImpl::GetUIParamNum() const
{
	return static_cast<int>(m_aFxUIParams.size());
}

//	Get UI param by index
IResFxUIParam* AResFxMaterialImpl::GetUIParam(int nIdx) const
{
	return m_aFxUIParams[nIdx];
}