
#include "stdafx.h"
#include "AppInterface.h"

#ifdef _ANGELICA3
#include "Collision.h"
#include "ObjManager.h"
#include <A3DTerrain2.h>

bool AppInterface::GetIKGroundPosAndNormal(const A3DVECTOR3& vRefPos, A3DVECTOR3& vGndPos, A3DVECTOR3& vGndNormal)
{
	bool bStartSolid = false;
	
	bool bRet = VertRayTrace(vRefPos, vGndPos, vGndNormal, 4, &bStartSolid);

/*	if (bRet)
	{
		if (m_i % 2 == 0)
		{
			m_vLastRefPosL = vGndPos;
			m_vLastRefDirL = vGndNormal;
		}
		else
		{
			m_vLastRefPosR = vGndPos;
			m_vLastRefDirR = vGndNormal;
		}
		m_i++; 
	}
	else
	{
		m_i = 0;
		m_vLastRefPosL.Clear();
		m_vLastRefPosR.Clear();
		m_vLastRefDirL.Clear();
		m_vLastRefDirR.Clear();
	}
	
	*/

	return bRet;
}

void AppInterface::Render(A3DWireCollector* pWC) const
{
	//pWC->Add3DLine(m_vLastRefPosL, m_vLastRefPosL + m_vLastRefDirL, 0x80800000);
	//pWC->Add3DLine(m_vLastRefPosR, m_vLastRefPosR + m_vLastRefDirR, 0x80800000);
	//pWC->AddSphere(m_vLastRefPosL + m_vLastRefDirL, 0.01f, 0x80800000);
	//pWC->AddSphere(m_vLastRefPosR + m_vLastRefDirR, 0.01f, 0x80800000);
}

AppInterface::AppInterface()
{
	m_i = 0;
	m_vLastRefPosL.Clear();
	m_vLastRefPosR.Clear();
	m_vLastRefDirL.Clear();
	m_vLastRefDirR.Clear();
	m_pTerrain = NULL;
}

AppInterface* AppInterface::GetInstance()
{
	static AppInterface instance;
	return &instance;
}
#else

#include "EC_ModelMan.h"
#include "A3DSkillGfxComposer.h"

A3DGFXExMan* AfxGetGFXExMan()
{
	static A3DGFXExMan s_gfxExMan;
	return &s_gfxExMan;
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

#ifdef _ANGELICA21
float AfxGetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm, bool bUseRayTrace)
#else
float AfxGetGrndNorm(const A3DVECTOR3& vPos, A3DVECTOR3* pNorm)
#endif
{
	if (s_CallBackOnGetGrndNorm)
		return (*s_CallBackOnGetGrndNorm)(vPos, pNorm);

	if (pNorm) *pNorm = _unit_y;
	return 0;
}

extern A3DDevice* g_pDevice;

A3DDevice* AfxGetA3DDevice()
{
	return g_pDevice;
}

#ifdef _ANGELICA21
A3DSkinModel* AfxLoadA3DSkinModel(const char* szFile, int iSkinFlag, DWORD dwTexFlag)
{
	A3DSkinModel* pSkinModel = new A3DSkinModel;

	if (!pSkinModel->Init(g_pDevice->GetA3DEngine()) || !pSkinModel->Load(szFile, iSkinFlag, dwTexFlag))
	{
		delete pSkinModel;
		return NULL;
	}

	return pSkinModel;
}
#else
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
#endif

void AfxReleaseA3DSkinModel(A3DSkinModel* pModel)
{
	pModel->Release();
	delete pModel;
}

CECModelMan* AfxGetECModelMan()
{
	static CECModelMan s_ecmodel_man;
	return &s_ecmodel_man;
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

void AfxECModelAddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, A3DSkinModel* pA3DSkinModel)
{
}

void AfxSetA3DSkinModelSceneLightInfo(A3DSkinModel* pA3DSkinModel)
{
	return;
	/*A3DSkinModel::LIGHTINFO LightInfo = pA3DSkinModel->GetLightInfo();
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

	pA3DSkinModel->SetLightInfo(LightInfo);*/
}

void AfxSkillGfxShowCaster(clientid_t idCaster, DWORD dwModifier)
{
}

void AfxSkillGfxShowDamage(clientid_t idCaster, clientid_t idTarget, int nDamage, int nDivisions, DWORD dwModifier)
{
	_SGC::A3DSkillGfxMan::GetDamageShowFunc()(idCaster, idTarget, nDamage, 1, dwModifier);
}

bool AfxSkillGfxAddDamageData(clientid_t nCaster, clientid_t nTarget, unsigned char SerialId, DWORD dwModifier, int nDamage)
{
	_SGC::A3DSkillGfxMan* pMan = _SGC::AfxGetSkillGfxEventMan();
	_SGC::A3DSkillGfxEvent* pEvent = pMan->GetSkillGfxEvent(nCaster, SerialId);

	if (pEvent)
	{
		TARGET_DATA td;
		td.dwModifier = dwModifier;
		td.idTarget = nTarget;
		td.nDamage = nDamage;

		if (!pEvent->AddOneTarget(td))
			_SGC::A3DSkillGfxMan::GetDamageShowFunc()(nCaster, nTarget, nDamage, 1, td.dwModifier);

		return true;
	}

	return false;
}

const char* AfxGetECMHullPath()
{
	return "CHData";
}

A3DCamera* AfxGetA3DCamera()
{
	return static_cast<A3DCamera*>(g_pDevice->GetA3DEngine()->GetActiveCamera());
}

bool AfxPlaySkillGfx(const AString& strAtkFile, unsigned char SerialID, clientid_t nCasterID, clientid_t nCastTargetID, const A3DVECTOR3* pFixedPoint, int nDivisions, TargetDataVec& Targets)
{
	using namespace _SGC;
	A3DSkillGfxComposerMan* pMan = AfxGetSkillGfxComposerMan();
	_SGC::A3DSkillGfxMan* pEventMan = AfxGetSkillGfxEventMan();

	pMan->Play(
		strAtkFile,
		SerialID,
		nCasterID,
		nCastTargetID,
		pFixedPoint);

	_SGC::A3DSkillGfxEvent* pEvent = pEventMan->GetSkillGfxEvent(nCasterID, SerialID);

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

namespace _SGC
{
	static A3DSkillGfxComposerMan _composer_man;

	A3DSkillGfxComposerMan* AfxGetSkillGfxComposerMan()
	{
		return &_composer_man;
	}
}

void AfxSetCamOffset(const A3DVECTOR3& vOffset)
{
	A3DCamera* pCamera = static_cast<A3DCamera*>(g_pDevice->GetA3DEngine()->GetActiveCamera());
	pCamera->SetPos(pCamera->GetPos() + vOffset);
}

void AfxBeginShakeCam()
{
}

void AfxEndShakeCam()
{
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

extern APhysXScene*  gPhysXScene;

bool IKInterfaceImp::GetIKGroundPosAndNormal(const A3DVECTOR3& vRefPos, A3DVECTOR3& vGndPos, A3DVECTOR3& vGndNormal, float fUpDist, float fDownDist, void* IKUserData)
{
	if (m_HasPhysXCC)
	{
		NxVec3 orig(vRefPos.x, vRefPos.y + fUpDist, vRefPos.z);
		NxVec3 motion(0, -fUpDist - fDownDist, 0);

		NxRaycastHit rcHit;
		rcHit.flags = 0;
		if (1 > APhysXCharacterControllerManager::ClosestRaycastExcludesAllCCs(*gPhysXScene, orig, motion, NX_ALL_SHAPES, rcHit))
			return false;

		vGndPos = APhysXConverter::N2A_Vector3(rcHit.worldImpact);
		vGndNormal = APhysXConverter::N2A_Vector3(rcHit.worldNormal);
		if (DotProduct(vGndNormal, A3DVECTOR3(0 ,1, 0)) < 0.4)
			return false;
		return true;
	}
	else
	{
		if(m_pTerrain)
		{
			float fHeight = m_pTerrain->GetPosHeight(vRefPos, &vGndNormal);
			if(vRefPos.y - fHeight < 3.0f)
			{
				vGndPos = vRefPos;
				vGndPos.y = fHeight;
				return true;
			}
		}
	}
	return false;
}


#endif
