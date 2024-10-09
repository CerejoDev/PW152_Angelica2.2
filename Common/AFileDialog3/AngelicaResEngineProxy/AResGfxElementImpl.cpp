/*
* FILE: AResGfxElementImpl.cpp
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
#include "AResGfxElementImpl.h"

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
//	Implement AResGfxElementImpl
//	
///////////////////////////////////////////////////////////////////////////

AResGfxElementImpl::AResGfxElementImpl(void)
: m_Type(TYPE_INVALID)
{
}

AResGfxElementImpl::~AResGfxElementImpl(void)
{
}

void AResGfxElementImpl::InitGfxContainer(A3DGFXElement* pElement)
{
	m_strSubGfxPath = "Gfx\\" + (AString)pElement->GetProperty(ID_GFXOP_CONTAINER_GFX_PATH);
}

void AResGfxElementImpl::InitModel(A3DGFXElement* pElement)
{
	m_strSmdPath = "Gfx\\Models\\" + (AString)pElement->GetProperty(ID_GFXOP_MODEL_PATH);
}

void AResGfxElementImpl::InitECModel(A3DGFXElement* pElement)
{
	m_strEcmPath = (AString)pElement->GetProperty(ID_GFXOP_ECMODEL_PATH);
}

void AResGfxElementImpl::InitSound(A3DGFXElement* pElement)
{
	RandStringContainer* pRandPaths = (RandStringContainer*)pElement->GetProperty(ID_GFXOP_SOUND_FILE);
	assert( pRandPaths );
	for (int nFileIdx = 0; nFileIdx < pRandPaths->GetSize(); ++nFileIdx)
	{
		AString strSfxPath;
		strSfxPath.Format("Sfx\\%s", pRandPaths->GetString(nFileIdx));
		m_aRandSoundPath.push_back(strSfxPath);
	}
}

void AResGfxElementImpl::InitNormal(A3DGFXElement* pElement)
{
	for (int nTexIdx = 0; nTexIdx < pElement->GetTexCount(); ++nTexIdx)
	{
		A3DGFXTexture* pGfxTex = pElement->GetGfxTex(nTexIdx);
		if (!pGfxTex->GetTexFile() || !pGfxTex->GetTexFile()[0])
			continue;

		AString strTexFile;
		strTexFile.Format("Gfx\\Textures\\%s", pGfxTex->GetTexFile());
		m_aTexs.push_back(strTexFile);
	}
}

bool AResGfxElementImpl::Init(A3DGFXElement* pElement)
{
	assert( pElement );

	switch (pElement->GetEleTypeId())
	{
	case ID_ELE_TYPE_GFX_CONTAINER:
		m_Type = TYPE_GFXCONTAINER;
		InitGfxContainer(pElement);
		break;
	case ID_ELE_TYPE_MODEL:
		m_Type = TYPE_MODEL;
		InitModel(pElement);
		break;
	case ID_ELE_TYPE_ECMODEL:
		m_Type = TYPE_ECMODEL;
		InitECModel(pElement);
		break;
	case ID_ELE_TYPE_SOUND:
		m_Type = TYPE_SOUND;
		InitSound(pElement);
		break;
	default:
		m_Type = TYPE_NORMAL;
		InitNormal(pElement);
		break;
	}

	return true;
}

void AResGfxElementImpl::Release()
{
	m_strSubGfxPath.Empty();
	m_strSmdPath.Empty();
	m_strEcmPath.Empty();
	m_aRandSoundPath.clear();
	m_aTexs.clear();
}


//	Get this gfx element's type
IResGfxElement::TYPE AResGfxElementImpl::GetType() const
{
	return m_Type;
}

//	Get path methods
const char* AResGfxElementImpl::GetSubGfxPath() const
{
	return m_strSubGfxPath;
}

const char* AResGfxElementImpl::GetSmdPath() const
{
	return m_strSmdPath;
}

const char* AResGfxElementImpl::GetEcmPath() const
{
	return m_strEcmPath;
}

int AResGfxElementImpl::GetRandSoundNum() const
{
	return static_cast<int>(m_aRandSoundPath.size());
}

const char* AResGfxElementImpl::GetRandSoundPath(int nIdx) const
{
	return m_aRandSoundPath[nIdx];
}

int AResGfxElementImpl::GetTexNum() const
{
	return static_cast<int>(m_aTexs.size());
}

const char* AResGfxElementImpl::GetTexPath(int nIdx) const
{
	return m_aTexs[nIdx];
}
