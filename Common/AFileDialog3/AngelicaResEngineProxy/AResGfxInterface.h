/*
* FILE: AResGfxInterface.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/2
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResGfxInterface_H_
#define _AResGfxInterface_H_

#include "A3DGfxInterface.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DEngine;
class A3DSkillGfxEventMan;
class A3DSkillGfxComposerMan;
struct ECMODEL_SHAKE;

namespace AudioEngine
{
	class EventInstance;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AResGfxInterface
//	
///////////////////////////////////////////////////////////////////////////

class AResGfxInterface : public A3DGfxInterface
{

public:		//	Types

public:		//	Constructor and Destructor

	AResGfxInterface(void);
	virtual ~AResGfxInterface(void);

public:		//	Attributes

public:		//	Operations

	bool Init(A3DGfxEngine* pA3DGfxEngine);
	void Release();

	//	For Skill Gfx Event
	virtual A3DSkillGfxEventMan* GetSkillGfxEventMan() const;
	virtual A3DSkillGfxComposerMan* GetSkillGfxComposerMan() const;

protected:	//	Attributes

	A3DEngine* m_pA3DEngine;
	A3DSkillGfxEventMan* m_pA3DSkillGfxEventMan;
	A3DSkillGfxComposerMan* m_pA3DSkillGfxComposerMan;

protected:	//	Operations

	//	For SkinModel
	virtual A3DSkinModel* LoadA3DSkinModel(const char* szFile, int iSkinFlag) const;
	virtual void ReleaseA3DSkinModel(A3DSkinModel* pModel) const;

	//	For Load ECMHull
	//	SubClass which Implements this function, should fill pECMHullData with .chf file's content, if any error happens, return a false here
	virtual bool LoadECMHullData(const char* szChfFile, ECMHullDataLoad* pECMHullData) const;
	virtual const char * GetECMHullPath() const;

	virtual bool GetModelUpdateFlag() const;
	//	Return true if light info has been filled
	virtual bool GetModelLight(const A3DVECTOR3& vPos, A3DSkinModelLight& light) const;
	virtual void ECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel) const;
	virtual float GetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm) const;
	virtual bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel) const;

	//	For Sound
	virtual AM3DSoundBuffer* LoadNonLoopSound(const char* szFile, int nPriority) const;
	virtual AM3DSoundBuffer* LoadLoopSound(const char* szFile) const;
	virtual void ReleaseSoundNonLoop(AM3DSoundBuffer*& pSound) const;
	virtual void ReleaseSoundLoop(AM3DSoundBuffer*& pSound) const;

	//	Gfx need the main camera to calculate distance to camera, when tick
	//	Change A3DCamera to A3DCameraBase for we only need the A3DCameraBase's information (so A3DOrthoCamera is able to be used)
	virtual A3DCameraBase* GetA3DCamera() const;

	//	For others
	virtual float GetAverageFrameRate() const;
	virtual bool GetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount) const;

	virtual bool PlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets) const;
	virtual void SkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier) const;
	virtual void SkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier) const;
	virtual bool SkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage, const ECMODEL_SHAKE *) const;

	//
	virtual AudioEngine::EventInstance* CreateAudioEventInstance(const char* szEventFullPath) const;
	virtual void ReleaseAudioEventInstance(AudioEngine::EventInstance*& pInstance) const;


};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResGfxInterface_H_


