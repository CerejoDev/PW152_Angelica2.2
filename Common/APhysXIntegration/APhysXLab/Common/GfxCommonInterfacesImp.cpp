/*
* FILE: GfxCommonInterfacesImp.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2010/01/21
*
* HISTORY: 
*
* Copyright (c) 2010 Perfect World, All Rights Reserved.
*/

// implement some extern function in GfxCommon lib...
#include <A3D.h>
#include <AMSoundEngine.h>
#include <AMSoundBufferMan.h>

#include "EC_ModelMan.h"
#include "ConvexHullData.h"
#include "A3DSkillGfxComposer.h"

extern A3DDevice* g_pDevice;

const char* AfxGetECMHullPath()
{
	return "CHData";
}

CECModelMan* AfxGetECModelMan()
{
	static CECModelMan s_ecmodel_man;
	return &s_ecmodel_man;
}

A3DGFXExMan* AfxGetGFXExMan()
{
	static A3DGFXExMan s_gfxExMan;
	return &s_gfxExMan;
}

namespace _SGC
{
	static A3DSkillGfxComposerMan _composer_man;

	A3DSkillGfxComposerMan* AfxGetSkillGfxComposerMan()
	{
		return &_composer_man;
	}
}

bool AfxPlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets)
{
	using namespace _SGC;
	A3DSkillGfxComposerMan* pMan = AfxGetSkillGfxComposerMan();
	A3DSkillGfxMan* pEventMan = AfxGetSkillGfxEventMan();

	pMan->Play(
		strAtkFile,
		SerialID,
		nCasterID,
		nCastTargetID,
		pFixedPoint);

	A3DSkillGfxEvent* pEvent = pEventMan->GetSkillGfxEvent(nCasterID, SerialID);

	if (!pEvent)
		return true;

	pEvent = pEvent->GetLast();
	pEvent->SetDivisions(nDivisions);

	TARGET_DATA td;
	td.idTarget = 2;
	td.nDamage = 100;
	td.dwModifier = 0;
	pEvent->AddOneTarget(td);
	return true;
}

void AfxSkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier)
{
	using namespace _SGC;
	A3DSkillGfxMan::GetDamageShowFunc()(idCaster, idTarget, nDamage, 1, dwModifier);
}

void AfxSkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier)
{
}

bool AfxSkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage)
{
	using namespace _SGC;
	A3DSkillGfxMan* pMan = AfxGetSkillGfxEventMan();
	A3DSkillGfxEvent* pEvent = pMan->GetSkillGfxEvent(nCaster, SerialId);

	if (pEvent)
	{
		TARGET_DATA td;
		td.dwModifier = dwModifier;
		td.idTarget = nTarget;
		td.nDamage = nDamage;

		if (!pEvent->AddOneTarget(td))
			A3DSkillGfxMan::GetDamageShowFunc()(nCaster, nTarget, nDamage, 1, td.dwModifier);

		return true;
	}

	return false;
}

typedef bool (*pFunctionGfxGetSurfaceData)(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount);
typedef float (*pFunctionGetGrndNorm)(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm);

static pFunctionGetGrndNorm s_CallBackOnGetGrndNorm;
static pFunctionGfxGetSurfaceData s_CallBackOnGfxGetSurfaceData;

// 增加了一个挂钩函数，用于在编辑器中自定义取地面高度，法向的函数
void Afx_SetGroundFuncHook(pFunctionGfxGetSurfaceData pCallBackOnGetSurfaceData, pFunctionGetGrndNorm pCallBackOnGetGrndNorm)
{
	s_CallBackOnGfxGetSurfaceData = pCallBackOnGetSurfaceData;
	s_CallBackOnGetGrndNorm = pCallBackOnGetGrndNorm;
}

A3DDevice* AfxGetA3DDevice()
{
	return g_pDevice;
}

A3DSkinModel* AfxLoadA3DSkinModel(const char* szFile, int iSkinFlag)
{
	A3DSkinModel* pSkinModel = new A3DSkinModel;

	if (!pSkinModel->Init(g_pDevice->GetA3DEngine()) || !pSkinModel->Load(szFile, iSkinFlag))
	{
		delete pSkinModel;
		return NULL;
	}

	return pSkinModel;
}

void AfxReleaseA3DSkinModel(A3DSkinModel* pModel)
{
	pModel->Release();
	delete pModel;
}

bool AfxGetModelUpdateFlag()
{
	return true;
}

void AfxSetA3DSkinModelSceneLightInfo(A3DSkinModel* pA3DSkinModel)
{
	A3DSkinModel::LIGHTINFO LightInfo = pA3DSkinModel->GetLightInfo();
	GfxLightParamList& ls = AfxGetGFXExMan()->GetLightParamList();

	if (ls.size())
	{
		A3DLIGHTPARAM* param = &ls[0]->GetLightParam();
		LightInfo.bPtLight = true;
		LightInfo.colPtDiff = param->Diffuse * param->Diffuse.a * 2.0f;
		LightInfo.colPtAmb =  param->Ambient * param->Ambient.a * 2.0f;
		LightInfo.fPtRange = param->Range;
		LightInfo.vPtAtten = A3DVECTOR3(param->Attenuation0, param->Attenuation1, param->Attenuation2);
		LightInfo.vPtLightPos = param->Position;
	}
	else
		LightInfo.bPtLight = false;

	pA3DSkinModel->SetLightInfo(LightInfo);
}

void AfxECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel)
{
}

A3DCamera* AfxGetA3DCamera()
{
	return static_cast<A3DCamera*>(g_pDevice->GetA3DEngine()->GetActiveCamera());
}

float AfxGetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm)
{
	if (s_CallBackOnGetGrndNorm)
		return (*s_CallBackOnGetGrndNorm)(vPos, pNorm);

	if (pNorm) *pNorm = _unit_y;
	return 0;
}

void AfxEndShakeCam()
{
}

const A3DLIGHTPARAM& AfxGetLightparam()
{
	static const A3DCOLORVALUE diffuse(0.8f, 0.8f, 0.9f, 1.0f);
	static const A3DCOLORVALUE spec(1.0f, 1.0f, 1.0f, 1.0f);
	static const A3DCOLORVALUE ambient(0.0f, 0.0f, 0.0f, 1.0f);

	static A3DLIGHTPARAM p;
	memset(&p, 0, sizeof(p));
	p.Type		= A3DLIGHT_DIRECTIONAL;
	p.Diffuse	= diffuse;
	p.Specular	= spec;
	p.Ambient	= ambient;
	p.Position	= A3DVECTOR3(0.0f, 0.0f, 0.0f);
	p.Direction	= Normalize(A3DVECTOR3(0.0f, -0.7f, 1.0f));
	return p;
}

bool AfxRayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel)
{
	return false;
}

AM3DSoundBuffer* AfxLoadNonLoopSound(const char* szFile, int nPriority)
{
	AMSoundBufferMan* pMan = g_pDevice->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();
	return pMan->Load3DSound(szFile, true);
}

AM3DSoundBuffer* AfxLoadLoopSound(const char* szFile)
{
	return AfxLoadNonLoopSound(szFile, 0);
}

void AfxReleaseSoundNonLoop(AM3DSoundBuffer*& pSound)
{
	g_pDevice->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->Release3DSound(&pSound);
}

void AfxReleaseSoundLoop(AM3DSoundBuffer*& pSound)
{
	AfxReleaseSoundNonLoop(pSound);
}

void AfxSetCamOffset(const A3DVECTOR3& vOffset)
{
	A3DCamera* pCamera = static_cast<A3DCamera*>(g_pDevice->GetA3DEngine()->GetActiveCamera());
	pCamera->SetPos(pCamera->GetPos() + vOffset);
}

void AfxBeginShakeCam()
{
}

float AfxGetAverageFrameRate()
{
	return 30.f;
}

bool gGfxGetSurfaceData(const A3DVECTOR3& vCenter, float fRadus, A3DVECTOR3* pVerts, int& nVertCount, WORD* pIndices, int& nIndexCount)
{
	if (s_CallBackOnGfxGetSurfaceData)
		return (*s_CallBackOnGfxGetSurfaceData)(vCenter, fRadus, pVerts, nVertCount, pIndices, nIndexCount);

	pVerts[0] = A3DVECTOR3(vCenter.x-fRadus, vCenter.y, vCenter.z+fRadus);
	pVerts[1] = A3DVECTOR3(vCenter.x+fRadus, vCenter.y, vCenter.z+fRadus);
	pVerts[2] = A3DVECTOR3(vCenter.x-fRadus, vCenter.y, vCenter.z-fRadus);
	pVerts[3] = A3DVECTOR3(vCenter.x+fRadus, vCenter.y, vCenter.z-fRadus);
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 1;
	pIndices[4] = 3;
	pIndices[5] = 2;
	nVertCount = 4;
	nIndexCount = 6;
	return true;
}

namespace _SGC
{
	class A3DSkillGfxMan;
	extern A3DSkillGfxMan* AfxGetSkillGfxEventMan();
}
