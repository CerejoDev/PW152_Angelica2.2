/*
* FILE: AResGfxInterface.cpp
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
#include "AResEnginePreRequireHeader.h"
#include "AResGfxInterface.h"
#include "AResSkillGfxEventMan.h"

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
//	Implement AResGfxInterface
//	
///////////////////////////////////////////////////////////////////////////

AResGfxInterface::AResGfxInterface(void)
: m_pA3DEngine(NULL)
, m_pA3DSkillGfxEventMan(NULL)
, m_pA3DSkillGfxComposerMan(NULL)
{
}

AResGfxInterface::~AResGfxInterface(void)
{
}

bool AResGfxInterface::Init(A3DGfxEngine* pA3DGfxEngine)
{
	if (!pA3DGfxEngine)
		return false;

	m_pA3DEngine = pA3DGfxEngine->GetA3DEngine();

	m_pA3DSkillGfxEventMan = new AResSkillGfxEventMan();
	if (!m_pA3DSkillGfxEventMan || !m_pA3DSkillGfxEventMan->Init(pA3DGfxEngine->GetA3DGfxExMan()))
	{
		a_LogOutput(1, "%s, failed to initialize A3DSkillGfxEventMan!", __FUNCTION__);
		return false;
	}

	m_pA3DSkillGfxComposerMan = new A3DSkillGfxComposerMan();
	if (!m_pA3DSkillGfxComposerMan || !m_pA3DSkillGfxComposerMan->Init(pA3DGfxEngine))
	{
		a_LogOutput(1, "%s, failed to create A3DSkillGfxComposerMan!", __FUNCTION__);
		return false;
	}

	return true;
}

void AResGfxInterface::Release()
{
	A3DRELEASE(m_pA3DSkillGfxComposerMan);
	A3DRELEASE(m_pA3DSkillGfxEventMan);
}

//	For Skill Gfx Event
A3DSkillGfxEventMan* AResGfxInterface::GetSkillGfxEventMan() const
{
	return m_pA3DSkillGfxEventMan;
}

A3DSkillGfxComposerMan* AResGfxInterface::GetSkillGfxComposerMan() const
{
	return m_pA3DSkillGfxComposerMan;
}

//	For SkinModel
A3DSkinModel* AResGfxInterface::LoadA3DSkinModel(const char* szFile, int iSkinFlag) const
{
	A3DSkinModel* pSkinModel = new A3DSkinModel;

	if (!pSkinModel->Init(m_pA3DEngine) || !pSkinModel->Load(szFile, iSkinFlag))
	{
		delete pSkinModel;
		return NULL;
	}

	return pSkinModel;
}

void AResGfxInterface::ReleaseA3DSkinModel(A3DSkinModel* pModel) const
{
	A3DRELEASE(pModel);
}

//	For Load ECMHull
//	SubClass which Implements this function, should fill pECMHullData with .chf file's content, if any error happens, return a false here
bool AResGfxInterface::LoadECMHullData(const char* szChfFile, ECMHullDataLoad* pECMHullData) const
{
	return false;
}

const char * AResGfxInterface::GetECMHullPath() const
{
	return NULL;
}

bool AResGfxInterface::GetModelUpdateFlag() const
{
	return true;
}
//	Return true if light info has been filled
bool AResGfxInterface::GetModelLight(const A3DVECTOR3& vPos, A3DSkinModelLight& light) const
{
	return false;
}

void AResGfxInterface::ECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel) const
{
	return;
}

float AResGfxInterface::GetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm) const
{
	if (pNorm)
		*pNorm = A3DVECTOR3::vAxisY;

	return 0.0f;
}

bool AResGfxInterface::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel) const
{
	return false;
}

//	For Sound
AM3DSoundBuffer* AResGfxInterface::LoadNonLoopSound(const char* szFile, int nPriority) const
{
	return NULL;
}

AM3DSoundBuffer* AResGfxInterface::LoadLoopSound(const char* szFile) const
{
	return NULL;
}

void AResGfxInterface::ReleaseSoundNonLoop(AM3DSoundBuffer*& pSound) const
{
	m_pA3DEngine->GetAMSoundEngine()->GetAMSoundBufferMan()->Release3DSound(&pSound);
}

void AResGfxInterface::ReleaseSoundLoop(AM3DSoundBuffer*& pSound) const
{
	ReleaseSoundNonLoop(pSound);
}

//	Gfx need the main camera to calculate distance to camera, when tick
//	Change A3DCamera to A3DCameraBase for we only need the A3DCameraBase's information (so A3DOrthoCamera is able to be used)
A3DCameraBase* AResGfxInterface::GetA3DCamera() const
{
	return NULL;
}

//	For others
float AResGfxInterface::GetAverageFrameRate() const
{
	return 30.f;
}

bool AResGfxInterface::GetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount) const
{
	return false;
}

bool AResGfxInterface::PlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets) const
{
	return false;
}

void AResGfxInterface::SkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier) const
{

}

void AResGfxInterface::SkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier) const
{

}

bool AResGfxInterface::SkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage, const ECMODEL_SHAKE*) const
{
	return true;
}


AudioEngine::EventInstance* AResGfxInterface::CreateAudioEventInstance(const char* szEventFullPath) const
{
	return NULL;
}

void AResGfxInterface::ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const
{

}
