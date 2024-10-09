/*
* FILE: AResInterfaceImpl.cpp
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
#include "AResLogImpl.h"
#include "AResInterfaceImpl.h"
#include "AResGfxImpl.h"
#include "AResAttImpl.h"
#include "AResECModelImpl.h"
#include "AResSkinImpl.h"
#include "AResSkinModelImpl.h"
#include "AResFxMaterialImpl.h"
#include "AResUnlitModelImpl.h"
#include "AResUnlitModelSetImpl.h"
#include "AResSpeedTreeImpl.h"

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
//	Implement AResInterfaceImpl
//	
///////////////////////////////////////////////////////////////////////////

AResInterfaceImpl::AResInterfaceImpl(void)
: m_pA3DEngine(NULL)
, m_pA3DDevice(NULL)
, m_pA3DGfxEngine(NULL)
, m_pA3DGfxExMan(NULL)
, m_pPhysXShell(NULL)
, m_pResLog(NULL)
, m_pSkillGfxEventMan(NULL)
{
}

AResInterfaceImpl::~AResInterfaceImpl(void)
{
	Release();
}

bool AResInterfaceImpl::Init(A3DEngine* pA3DEngine
							 , A3DGfxEngine* pA3DGfxEngine
							 , GFXPhysXSceneShell* pPhysXShell
							 , A3DSkillGfxEventMan* pSkillGfxEventMan)
{
	m_pResLog = new AResLogImpl();
	m_pResLog->Init("Logs\\ARes.log", "AResource Module Log");
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();
	m_pA3DGfxEngine = pA3DGfxEngine;
	m_pA3DGfxExMan = pA3DGfxEngine->GetA3DGfxExMan();
	m_pPhysXShell = pPhysXShell;
	m_pSkillGfxEventMan = pSkillGfxEventMan;
	return m_pA3DEngine 
		&& m_pA3DDevice
		&& m_pA3DGfxEngine
		&& m_pA3DGfxExMan
		&& m_pPhysXShell
		&& m_pSkillGfxEventMan;
}

void AResInterfaceImpl::Release()
{
	A3DRELEASE(m_pResLog);	
}

IResLog* AResInterfaceImpl::GetResLog() const
{
	return m_pResLog;
}

IResSkinModel* AResInterfaceImpl::LoadSmd(const char* szPath)
{
	std::auto_ptr<AResSkinModelImpl> pSmdImpl(new AResSkinModelImpl(this));
	if (!pSmdImpl->Init(szPath))
		return NULL;

	return pSmdImpl.release();
}

IResECModel* AResInterfaceImpl::LoadEcm(const char* szPath)
{
	std::auto_ptr<AResECModelImpl> pEcmImpl(new AResECModelImpl(this));
	if (!pEcmImpl->Init(szPath))
		return NULL;

	return pEcmImpl.release();
}

IResGfx* AResInterfaceImpl::LoadGfx(const char* szPath)
{
	std::auto_ptr<AResGfxImpl> pGfxImpl(new AResGfxImpl(this));
	if (!pGfxImpl->Init(szPath))
		return NULL;

	return pGfxImpl.release();
}

IResAtt* AResInterfaceImpl::LoadAtt(const char* szPath)
{
	std::auto_ptr<AResAttImpl> pAttImpl(new AResAttImpl(this));
	if (!pAttImpl->Init(szPath))
		return NULL;

	return pAttImpl.release();
}

IResUnlitModel* AResInterfaceImpl::LoadUmd(const char* szPath)
{
	std::auto_ptr<AResUnlitModelImpl> pUmdImpl(new AResUnlitModelImpl(this));
	if (!pUmdImpl->Init(szPath))
		return NULL;

	return pUmdImpl.release();
}

IResUnlitModelSet* AResInterfaceImpl::LoadUmds(const char* szPath)
{
	std::auto_ptr<AResUnlitModelSetImpl> pUmdsImpl(new AResUnlitModelSetImpl(this));
	if (!pUmdsImpl->Init(szPath))
		return NULL;

	return pUmdsImpl.release();
}

IResSkin* AResInterfaceImpl::LoadSki(const char* szPath)
{
	std::auto_ptr<AResSkinImpl> pSkiImpl(new AResSkinImpl(this));
	if (!pSkiImpl->Init(szPath))
		return NULL;

	return pSkiImpl.release();
}

IResFxMaterial* AResInterfaceImpl::LoadAfm(const char* szPath)
{
	std::auto_ptr<AResFxMaterialImpl> pAfmImpl(new AResFxMaterialImpl(this));
	if (!pAfmImpl->Init(szPath))
		return NULL;

	return pAfmImpl.release();
}

IResSpeedTree* AResInterfaceImpl::LoadSpt(const char* szPath)
{
	std::auto_ptr<AResSpeedTreeImpl> pSptImpl(new AResSpeedTreeImpl(this));
	if (!pSptImpl->Init(szPath))
		return NULL;

	return pSptImpl.release();
}

void AResInterfaceImpl::ReleaseRes(IRes* pRes)
{
	delete pRes;
}