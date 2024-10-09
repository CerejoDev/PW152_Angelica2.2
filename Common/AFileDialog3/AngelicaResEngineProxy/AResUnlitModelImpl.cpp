/*
* FILE: AResUnlitModelImpl.cpp
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
#include "AResInterfaceImpl.h"
#include "AResFxMaterialImpl.h"
#include "AResUnlitModelImpl.h"

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

class UnlitModelWrapper : public A3DUnlitModel
{
public:
	~UnlitModelWrapper()
	{
		Release();
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AResUnlitModelImpl
//	
///////////////////////////////////////////////////////////////////////////

AResUnlitModelImpl::AResUnlitModelImpl(AResInterfaceImpl* pResImpl)
: m_pResImpl(pResImpl)
{
}

AResUnlitModelImpl::~AResUnlitModelImpl(void)
{
	Release();
}

bool AResUnlitModelImpl::Init(const char* szPath)
{
	std::auto_ptr<UnlitModelWrapper> pUnlitModel(new UnlitModelWrapper);
	if (!pUnlitModel->Init(m_pResImpl->GetA3DDevice()) || !pUnlitModel->Load(m_pResImpl->GetA3DDevice(), szPath))
		return false;

	if (!Init(pUnlitModel.get()))
		return false;

	return true;
}

bool AResUnlitModelImpl::Init(A3DUnlitModel* pFxMtl)
{
	for (int nIdx = 0; nIdx < pFxMtl->GetNumMeshes(); ++nIdx)
	{
		A3DUnlitMesh* pMesh = pFxMtl->GetMesh(nIdx);
		std::auto_ptr<AResFxMaterialImpl> pResMtl(new AResFxMaterialImpl(m_pResImpl));
		if (!pResMtl->Init(pMesh->GetFXMaterial()))
			return false;

		m_aResMtls.push_back(pResMtl.release());
	}

	return true;
}

void AResUnlitModelImpl::Release()
{
	for (ResFxMtlArray::iterator itr = m_aResMtls.begin()
		; itr != m_aResMtls.end()
		; ++itr)
	{
		A3DRELEASE(*itr);
	}

	m_aResMtls.clear();
}

int AResUnlitModelImpl::GetNumMtls() const
{
	return static_cast<int>(m_aResMtls.size());
}

IResFxMaterial* AResUnlitModelImpl::GetMeshMaterial(int nIdx) const
{
	return m_aResMtls[nIdx];
}