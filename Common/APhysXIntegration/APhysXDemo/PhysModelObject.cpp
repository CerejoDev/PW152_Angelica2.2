/*
 * FILE: PhysModelObject.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/5/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "EC_Model.h"
#include "PhysModelObject.h"
#include "Render.h"
#include "Game.h"
#include "InterfaceWrapper.h"
#include "A3DModelPhysics.h"
#include "A3DModelPhysSync.h"
#include "A3DModelPhysSyncData.h"
#include "A3DSkinPhysSync.h"
#include "A3DSkinPhysSyncData.h"

#include "APhysX.h"

#include "ClothSkinConfig.h"

#include <A3DMacros.h>
#include <A3DViewport.h>
#include <A3DSkeleton.h>
#include <A3DBone.h>
#include <A3DDevice.h>
#include <A3DEngine.h>
#include <A3DSkinMan.h>

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

//	2-stick model parameters
const char* l_2s_szUpperStick = "upper_stick";
const char* l_2s_szLowerStick = "lower_stick";
const char* l_2s_szJoint = "2s_joint";
float l_2s_fStickLen = 1.0f;
float l_2s_fStickRad = 0.15f;

const char* l_szClothActor = "cloth_actor";
const char* l_szClothActor2 = "cloth_actor2";

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CPhysModelObject
//	
///////////////////////////////////////////////////////////////////////////

CPhysModelObject::CPhysModelObject()
{
	m_pSkinModel	= NULL;
	m_pPhysSyncData	= NULL;
	m_pModelPhys	= NULL;
	m_iType			= TYPE_UNKNOWN;

	m_pClothSkinConfig = NULL;
	m_iClothSkinStartID = 0;

	m_pCurAction = NULL;
	m_iCurActionPlayTime = 0;

	m_iClothOnDelayTime = 0;

}

CPhysModelObject::~CPhysModelObject()
{
}

//	Load object data from file
bool CPhysModelObject::Init(int iType)
{
	m_iType = iType;

/*
	// obsolete code...
	if (m_iType == TYPE_2STICKS)
	{
		//	Load skin model
		m_pSkinModel = LoadSkinModel("Models\\2Sticks\\2Sticks.smd");
		if (!m_pSkinModel)
		{
			a_LogOutput(1, "CPhysModelObject::Init2Sticks, Failed to load model");
			return false;
		}
	}
*/

	return true;
}

static float s_fScale = 1.0f;

//	Load object from file
bool CPhysModelObject::LoadModel(const char* szFile)
{
	if(!CPhysObject::LoadModel(szFile))
		return false;

	// load skin model...
	m_pSkinModel = LoadSkinModel(szFile);
	if (!m_pSkinModel)
	{
		return false;
	}

	//-----------------------------------------------------
	// tmp code...

#if 0
	
	for(int i=0; i<m_pSkinModel->GetSkinNum(); i++)
	{
		A3DSkin* pCurSkin = m_pSkinModel->GetA3DSkin(i);

		AfxOutputDebugString("\n");
		AfxOutputDebugString(pCurSkin->GetFileName());
	}

	A3DSkinModelActionCore* pSMAction = m_pSkinModel->GetFirstAction();
	while(pSMAction)
	{
		AfxOutputDebugString("\n");
		AfxOutputDebugString(pSMAction->GetName());

		pSMAction = m_pSkinModel->GetNextAction();	

	}
	

#endif

	//-----------------------------------------------------

	// load sync data...
	AString strPhysXSyncFile = szFile;
	af_ChangeFileExt(strPhysXSyncFile, ".mphy");

	if (m_pPhysSyncData)
	{
		delete m_pPhysSyncData;
	}

	m_pPhysSyncData = new A3DModelPhysSyncData;
	if(!m_pPhysSyncData->Load(strPhysXSyncFile))
	{
		delete m_pPhysSyncData;
		m_pPhysSyncData = NULL;

		// and we should release skin model...
		A3DRELEASE(m_pSkinModel);
	
		return false;
	}

	//------------------------------------------------------
	// load cloth skins...
	m_iClothSkinStartID = m_pSkinModel->GetSkinNum();

	af_ChangeFileExt(strPhysXSyncFile, ".csk");
	if(m_pClothSkinConfig)
	{
		delete m_pClothSkinConfig;
	}

	m_pClothSkinConfig = new CClothSkinConfig;
	m_pClothSkinConfig->LoadConfig(strPhysXSyncFile);


	// try some scaling operation...

#if 0

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	int iRootBoneID = pSkeleton->GetRootBone(0);
	A3DBone* pBone = pSkeleton->GetBone(iRootBoneID);
	pBone->SetWholeScale(s_fScale);
	s_fScale += 0.2f;
	m_pSkinModel->Update(0);

#endif

	return true;

}

//	Release object
void CPhysModelObject::Release()
{
	A3DRELEASE(m_pModelPhys);

	if (m_pPhysSyncData)
	{
		delete m_pPhysSyncData;
		m_pPhysSyncData = NULL;
	}

	if(m_pClothSkinConfig)
	{
		delete m_pClothSkinConfig;
		m_pClothSkinConfig = NULL;
		
	}

	A3DRELEASE(m_pSkinModel);
}

//	Set position
void CPhysModelObject::SetPos(const A3DVECTOR3& vPos)
{
	if (m_pSkinModel)
	{
		m_pSkinModel->SetPos(vPos);
		m_pSkinModel->Update(0);
	}
}

//	Set orientation
void CPhysModelObject::SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	if (m_pSkinModel)
	{
		m_pSkinModel->SetDirAndUp(vDir, vUp);
		m_pSkinModel->Update(0);
	}
}

//	Setup object
bool CPhysModelObject::Setup(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir, const A3DVECTOR3& vUp)
{
	// call parent class's method
	CPhysObject::Setup(vPos, vDir, vUp);

	return true;
}

void CPhysModelObject::ResetPose()
{

	m_pSkinModel->ResetToInitPose();

	m_pSkinModel->SetPos(m_vPos);

	m_pSkinModel->SetDirAndUp(m_vDir, m_vUp);

	m_pSkinModel->Update(0);
	
}

bool CPhysModelObject::InstantializePhysX()
{

	// try some scaling operation...

#if 0
	
	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	int iRootBoneID = pSkeleton->GetRootBone(0);
	A3DBone* pBone = pSkeleton->GetBone(iRootBoneID);
	pBone->SetWholeScale(s_fScale);
	s_fScale += 0.2f;
	m_pSkinModel->Update(0);

#endif

	m_pSkinModel->Update(0);

	m_pModelPhys = new A3DModelPhysics;

	if (!m_pModelPhys->Init(g_Game.GetPhysXScene(), m_pSkinModel))
		return false;

	if (!m_pModelPhys->CreateModelSync(m_pPhysSyncData))
		return false;

	// load cloth skins...
	if(m_pClothSkinConfig && m_pClothSkinConfig->HasClothSkin())
	{
		for(int i=0; i<m_pClothSkinConfig->GetClothSkinNum(); i++)
		{
			ClothSkinItem* pClothSkin = m_pClothSkinConfig->GetClothSkin(i);

			int iSkinSlot;
			if(m_pModelPhys->AddSkinFile(pClothSkin->mSkinFilePath))
			{
				iSkinSlot = m_pSkinModel->GetSkinNum() - 1;	
			}
			else
				continue;

			/*
			if (!pClothSkin->mSkinPhysSyncData->InitBind(g_Game.GetPhysXScene(), m_pSkinModel))
			{
				continue;
			}
			*/


			// if (!m_pModelPhys->OpenClothSkin_HL(iSkinSlot, pClothSkin->mClothLinks.size(), pClothSkin->mClothLinks[0]))
			if (!m_pModelPhys->OpenClothSkin(iSkinSlot, 2))
			{
				continue;
			}

		}
	}



	//	Set current physical state
	ChangePhysicalState(g_Game.GetPhysState());

	//	Restore skin model position
	m_pSkinModel->SetPos(m_vPos);
	m_pSkinModel->Update(0);
	m_pModelPhys->GetModelSync()->Teleport();

	
	return true;

}

bool CPhysModelObject::ReleasePhysX()
{

	A3DRELEASE(m_pModelPhys);

	// note here: we should remove the item from the back to front...
	// remove all cloth skin items added...
	for(int i = m_pSkinModel->GetSkinNum(); i > m_iClothSkinStartID; i--)
	{
		m_pSkinModel->RemoveSkinItem(i-1);
	}

	return true;
}

bool CPhysModelObject::TurnOffPhysX()
{
	
	// turn off the cloth simulation...
	for(int i = m_pSkinModel->GetSkinNum(); i > m_iClothSkinStartID; i--)
	{
		m_pModelPhys->CloseClothSkin(i-1);
	}

	return true;

}

const int c_iDelayTimeInMS = 1000;

bool CPhysModelObject::TurnOnPhysX()
{
	// load cloth skins...
	if(m_pClothSkinConfig && m_pClothSkinConfig->HasClothSkin())
	{
		int iCurSkin = 0; 

		for(int iSkinSlot = m_iClothSkinStartID; iSkinSlot < m_pSkinModel->GetSkinNum(); iSkinSlot++)
		{
			ClothSkinItem* pClothSkin = m_pClothSkinConfig->GetClothSkin(iCurSkin);
			m_pModelPhys->OpenClothSkin(iSkinSlot, 2);
			// m_pModelPhys->OpenClothSkin_HL(iSkinSlot, pClothSkin->mClothLinks.size(), pClothSkin->mClothLinks[0]);
			iCurSkin++;

			// we will delay the physics simulation driven cloth...
			A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSkinSlot);
			if (pSkin)
			{
				pSkin->EnableClothes(false);
				m_iClothOnDelayTime = c_iDelayTimeInMS;

			}
		
		}


		return true;
	}

	return false;
	
}


//	Load skin model from file
CSkinModel* CPhysModelObject::LoadSkinModel(const char* szFile)
{
	CSkinModel* pModel = new CSkinModel;
	if (!pModel)
		return NULL;

	if (!pModel->Init(g_Render.GetA3DEngine()))
	{
		delete pModel;
		a_LogOutput(1, "CPhysModelObject::LoadSkinModel, failed to initialize skin model");
		return NULL;
	}

	if (!pModel->Load(szFile, 0))
	{
		delete pModel;
		a_LogOutput(1, "CPhysModelObject::LoadSkinModel, Failed to load %s", szFile);
		return NULL;
	}

	//	Set light for model
	A3DSkinModel::LIGHTINFO LightInfo;
	memset(&LightInfo, 0, sizeof (LightInfo));
	const A3DLIGHTPARAM& lp = g_Render.GetDirectionalLight()->GetLightparam();

	LightInfo.colAmbient	= g_Render.GetA3DDevice()->GetAmbientValue();
	LightInfo.vLightDir		= lp.Direction;
	LightInfo.colDirDiff	= lp.Diffuse;
	LightInfo.colDirSpec	= lp.Specular;
	LightInfo.bPtLight		= false;

	pModel->SetLightInfo(LightInfo);

	return pModel;
}

//	Tick routine
bool CPhysModelObject::Tick(DWORD dwDeltaTime)
{
	if (m_pModelPhys)
	{

		m_pModelPhys->UpdateBeforePhysXSim(dwDeltaTime);
	
		// if the current action we played is over, try next action...
		const int iSleepTime = 800;

		if(m_iPhysState == PHY_STATE_ANIMATION || m_iPhysState == PHY_STATE_PARTSIMULATE)
		{
			if(m_pCurAction)
			{
				m_iCurActionPlayTime += dwDeltaTime;
				//------------------------------------------------------------
				// this is a place easy to incur the error
				// comparison between the int and unsigned int... 
				if(m_iCurActionPlayTime > (int)m_pCurAction->GetEndTime())
				{
					m_iCurActionPlayTime -= m_pCurAction->GetEndTime() + iSleepTime;
				}

				if(m_iCurActionPlayTime>=0 && m_iCurActionPlayTime < (int)dwDeltaTime)
				{
					m_pCurAction = m_pSkinModel->GetNextAction()? m_pSkinModel->GetNextAction(): m_pSkinModel->GetFirstAction();
					
					m_pSkinModel->PlayActionByNameDC(m_pCurAction->GetName());

				}
				
			}
			else
			{
				// m_pSkinModel has no actions...
			}


			// counting the delay time...
			if(m_iClothOnDelayTime >0)
			{
				m_iClothOnDelayTime -= dwDeltaTime;
			}

			if(m_iClothOnDelayTime<0)
			{

				for(int iSkinSlot = m_iClothSkinStartID; iSkinSlot < m_pSkinModel->GetSkinNum(); iSkinSlot++)
				{
					A3DSkin* pSkin = m_pSkinModel->GetA3DSkin(iSkinSlot);
					if (pSkin) 
					{
						pSkin->EnableClothes(true);

						for(int i = 0; i<pSkin->GetClothMeshNum(); i++)
						{
							A3DSkinMeshRef clothSkin = pSkin->GetClothMeshRef(0);
						}

						A3DSkinMeshRef meshSkin = pSkin->GetSkinMeshRef(0);
						
					}

				}

				m_iClothOnDelayTime = 0;
			}

		}


	}
	else
	{
		m_pSkinModel->Update(dwDeltaTime);
	}

	return true; 
}

//	Sync physical data to graphics data
bool CPhysModelObject::Sync()
{
	if (m_pModelPhys)
		m_pModelPhys->SyncAfterPhysXSim();

	return true;
}

//	Render routine
bool CPhysModelObject::Render(A3DViewport* pViewport)
{
	if (m_pSkinModel)
		m_pSkinModel->Render(pViewport);

	// render the aabb of skin model...

#if 0

	if (m_pSkinModel)
	{
		A3DWireCollector* pwc = g_Render.GetA3DEngine()->GetA3DWireCollector();
		const A3DAABB& aabb = m_pSkinModel->GetModelAABB();
		pwc->AddAABB(aabb, 0xffffff00);
	}

#endif

	return true;
}

//	Render proxy
bool CPhysModelObject::RenderProxy(A3DViewport* pViewport)
{
	if (m_pSkinModel)
	{
		A3DWireCollector* pwc = g_Render.GetA3DEngine()->GetA3DWireCollector();
		const A3DAABB& aabb = m_pSkinModel->GetModelAABB();
		pwc->AddAABB(aabb, 0xffffff00);
	}

	return true;
}

//	Change physical state
bool CPhysModelObject::ChangePhysicalState(int iState)
{
	if (!m_pSkinModel || !m_pModelPhys->GetModelSync())
		return false;

	m_iPhysState = iState;

	m_pModelPhys->ChangePhysState(iState);

	if (iState == PHY_STATE_ANIMATION)
	{

		// obsolete code...
		// if (m_iType == TYPE_2STICKS)
		//		m_pSkinModel->PlayActionByNameDC("2sticks_rotate", -1, 0);

		// m_pSkinModel->PlayActionByNameDC("奔跑_通用", -1, 0);
		// m_pSkinModel->PlayActionByNameDC("S032_sex_104_01", -1, 0);
		// m_pSkinModel->PlayActionByNameDC("S032_sex_104_01", 1);


		m_pCurAction = m_pSkinModel->GetFirstAction();
		m_iCurActionPlayTime = 0;

		m_pSkinModel->PlayActionByNameDC(m_pCurAction->GetName(), 1);

	}
	else if (iState == PHY_STATE_PARTSIMULATE)
	{
		// if (m_iType == TYPE_2STICKS)
		//  	m_pSkinModel->PlayActionByNameDC("2sticks_rotate", -1, 0);

		// m_pSkinModel->PlayActionByNameDC("奔跑_通用", -1, 0);
		// m_pSkinModel->PlayActionByNameDC("S032_sex_104_02", -1, 0);

		m_pCurAction = m_pSkinModel->GetFirstAction();
		m_iCurActionPlayTime = 0;

		m_pSkinModel->PlayActionByNameDC(m_pCurAction->GetName(), 1);
	}

	return true;
}

//	Initialize 2-stick object
bool CPhysModelObject::Setup2Sticks(const A3DVECTOR3& vPos)
{
	//	Setup skin model position
	m_pSkinModel->SetPos(A3DVECTOR3(0.0f));
	m_pSkinModel->Update(0);

	m_pPhysSyncData = new A3DModelPhysSyncData;

	//	Prepare physical object data ...
	//	Create upper stick
	A3DModelPhysSyncData::ACTOR_DATA* pUpperActorData = m_pPhysSyncData->CreateActor(l_2s_szUpperStick, A3DVECTOR3(0.0f, l_2s_fStickLen * 1.5f, 0.0f));
	APhysXActorDesc& apxActorDesc = *pUpperActorData->GetActorDesc();
	apxActorDesc.mDensity = 1.0f;
	apxActorDesc.mHasBody = true;

	APhysXMaterialDesc& apxMaterialDesc = *pUpperActorData->GetMaterialDesc();
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	//	Add actor shape
	APhysXCapsuleShapeDesc CapsuleDesc;
	CapsuleDesc.mRadius = l_2s_fStickRad;
	CapsuleDesc.mHeight = l_2s_fStickLen;
	CapsuleDesc.UseDefaultMaterial();
	
	pUpperActorData->AddShape(&CapsuleDesc);

	//	Create lower stick
	A3DModelPhysSyncData::ACTOR_DATA* pLowerActorData = m_pPhysSyncData->CreateActor(l_2s_szLowerStick, A3DVECTOR3(0.0f, l_2s_fStickLen * 0.5f, 0.0f));
	apxActorDesc.mDensity = 1.0f;
	apxActorDesc.mHasBody = true;

	apxMaterialDesc = *pLowerActorData->GetMaterialDesc();
	apxMaterialDesc.mDynamicFriction = 0.1f;
	apxMaterialDesc.mRestitution = 1.0f;

	pLowerActorData->AddShape(&CapsuleDesc);

	//	Link actors and bone
	m_pPhysSyncData->LinkBoneToActor(l_2s_szLowerStick, m_pSkinModel, "Bone01");
	m_pPhysSyncData->LinkBoneToActor(l_2s_szUpperStick, m_pSkinModel, "Bone02");
	m_pPhysSyncData->LinkBoneToActor(l_2s_szUpperStick, m_pSkinModel, "Bone03");
	m_pPhysSyncData->LinkActorToBone(l_2s_szLowerStick, m_pSkinModel, "Bone01");
	m_pPhysSyncData->LinkActorToBone(l_2s_szUpperStick, m_pSkinModel, "Bone02");
	m_pPhysSyncData->SetRootBone("Bone01");

	pLowerActorData->GetLinkInfo().GetBoneDataByName("Bone01")->SetAnimMajorFlag(true);
	pUpperActorData->GetLinkInfo().GetBoneDataByName("Bone02")->SetAnimMajorFlag(false);
	pUpperActorData->GetLinkInfo().GetBoneDataByName("Bone03")->SetAnimMajorFlag(false);

	//	Create joint
	APhysXSphericalJointDesc JointDesc;
	JointDesc.mGlobalAnchor = NxVec3(0.0f, l_2s_fStickLen, 0.0f);
	JointDesc.mGlobalAxis = NxVec3(0.0f, 1.0f, 0.0f);
	A3DModelPhysSyncData::JOINT_DATA* pJointData = m_pPhysSyncData->CreateJoint(l_2s_szJoint, &JointDesc);

	//	Link joint and actors
	m_pPhysSyncData->LinkActorAndJoint(l_2s_szUpperStick, l_2s_szJoint, 0);
	m_pPhysSyncData->LinkActorAndJoint(l_2s_szLowerStick, l_2s_szJoint, 1);

	//	Temp code
//	if (!m_pPhysSyncData->Load("Models\\2Sticks\\2Sticks.mphy"))
//		return false;

	//	Temp code
	// m_pPhysSyncData->Save("E:\\2Sticks.mphy");

	if (!m_pModelPhys->CreateModelSync(m_pPhysSyncData))
		return false;

	//	Set current physical state
	ChangePhysicalState(g_Game.GetPhysState());

	//	Restore skin model position
	m_pSkinModel->SetPos(vPos);
	m_pSkinModel->Update(0);
	m_pModelPhys->GetModelSync()->Teleport();

	//	Add cloth skins
	if (!AddClothSkin())
		return false;

	return true;
}

//	Add cloth skin
bool CPhysModelObject::AddClothSkin()
{
	//	Use cloth 1
	if (0)
	{
		//	Add cloth
		int iSkinSlot = m_pModelPhys->AddSkinFile("Models\\2Sticks\\cloth.ski");
		AArray<int> aRigidMeshIdx;
		AArray<int> aSkinMeshIdx;
		A3DSkinPhysSyncData* pSyncData = m_pModelPhys->BeginEditClothSkin(iSkinSlot, aRigidMeshIdx, aSkinMeshIdx);
		if (!pSyncData)
			return false;

		//	Create a sphere actor at origin
		A3DSkinPhysSyncData::ACTOR_DATA* pClothActor = pSyncData->GetActorByName(l_szClothActor);
		if (!pClothActor)
		{
			pClothActor = pSyncData->CreateActor(l_szClothActor, g_vOrigin);

			APhysXActorDesc* apxActorDesc = pClothActor->GetActorDesc();
			apxActorDesc->mDensity = 1.0f;
			apxActorDesc->mHasBody = true;

			APhysXMaterialDesc* apxMaterialDesc = pClothActor->GetMaterialDesc();
			apxMaterialDesc->mDynamicFriction = 0.1f;
			apxMaterialDesc->mRestitution = 1.0f;

			//	Add actor shape
			APhysXSphereShapeDesc SphereDesc;
			SphereDesc.mRadius = 0.1f;
			SphereDesc.UseDefaultMaterial();

			pClothActor->AddShape(&SphereDesc);
		}

		//	Bind skin physical data
		if (!pSyncData->InitBind(g_Game.GetPhysXScene(), m_pSkinModel))
		{
			m_pModelPhys->EndEditClothSkin(pSyncData);
			return false;
		}

		//	Save data to file
		AString strFile = g_szWorkDir;
		strFile += "\\";
		strFile += pSyncData->GetFileName();
		if (!pSyncData->Save(strFile))
		{
			m_pModelPhys->EndEditClothSkin(pSyncData);
			return false;
		}

		m_pModelPhys->EndEditClothSkin(pSyncData);
		pSyncData = NULL;

		//	Link cloth
		if (!m_pModelPhys->OpenClothSkin(iSkinSlot, 1))
	//	if (!m_pModelPhys->OpenClothSkin_HL(iSkinSlot, l_szClothActor, "HH_cloth", false))
			return false;
	}

	//	Use cloth 2
	if (1)
	{
		//	Add skin
		// int iSkinSlot = m_pModelPhys->AddSkinFile("Models\\2Sticks\\cloth2.ski");
		int iSkinSlot = -1;
		if(m_pModelPhys->AddSkinFile("Models\\2Sticks\\cloth2.ski"))
		{
			iSkinSlot = m_pSkinModel->GetSkinNum() - 1;	
		}

		AArray<int> aRigidMeshIdx;
		AArray<int> aSkinMeshIdx;
		A3DSkinPhysSyncData* pSyncData = m_pModelPhys->BeginEditClothSkin(iSkinSlot, aRigidMeshIdx, aSkinMeshIdx);
		if (!pSyncData)
			return false;

		//	Create a sphere actor at origin
		A3DSkinPhysSyncData::ACTOR_DATA* pClothActor = pSyncData->GetActorByName(l_szClothActor);
		if (!pClothActor)
		{
			pClothActor = pSyncData->CreateActor(l_szClothActor, g_vOrigin);

			APhysXActorDesc* apxActorDesc = pClothActor->GetActorDesc();
			apxActorDesc->mDensity = 1.0f;
			apxActorDesc->mHasBody = true;

			APhysXMaterialDesc* apxMaterialDesc = pClothActor->GetMaterialDesc();
			apxMaterialDesc->mDynamicFriction = 0.1f;
			apxMaterialDesc->mRestitution = 1.0f;

			//	Add actor shape
			APhysXSphereShapeDesc SphereDesc;
			SphereDesc.mRadius = 0.1f;
			SphereDesc.UseDefaultMaterial();

			pClothActor->AddShape(&SphereDesc);

			//	Create second sphere actor
			A3DSkinPhysSyncData::ACTOR_DATA* pClothActor2 = pSyncData->CreateActor(l_szClothActor2, A3DVECTOR3(2.0f, 0.0f, 0.0f));
			apxActorDesc = pClothActor2->GetActorDesc();
			apxActorDesc->mDensity = 1.0f;
			apxActorDesc->mHasBody = true;

			apxMaterialDesc = pClothActor2->GetMaterialDesc();
			apxMaterialDesc->mDynamicFriction = 0.1f;
			apxMaterialDesc->mRestitution = 1.0f;

			pClothActor2->AddShape(&SphereDesc);
		}

		//	Bind skin physical data
		if (!pSyncData->InitBind(g_Game.GetPhysXScene(), m_pSkinModel))
		{
			m_pModelPhys->EndEditClothSkin(pSyncData);
			return false;
		}

		//	Save data to file
		AString strFile = g_szWorkDir;
		strFile += "\\";
		strFile += pSyncData->GetFileName();
		if (!pSyncData->Save(strFile))
		{
			m_pModelPhys->EndEditClothSkin(pSyncData);
			return false;
		}

		m_pModelPhys->EndEditClothSkin(pSyncData);
		pSyncData = NULL;

		//	Link cloth
		CLOTH_LINK aLinks[2];

		aLinks[0].szRBActor	= l_szClothActor;
		aLinks[0].szHookHH	= "HH_cloth";
		aLinks[0].bHHIsBone	= false;

		aLinks[1].szRBActor	= l_szClothActor2;
		aLinks[1].szHookHH	= "HH_cloth2";
		aLinks[1].bHHIsBone	= false;

	//	if (!m_pModelPhys->OpenClothSkin_SL(iSkinSlot, 2, aLinks))
		if (!m_pModelPhys->OpenClothSkin(iSkinSlot,2))
			return false;
	}

	return true;
}

bool CPhysModelObject::RayTrace(const PhysRay& ray, PhysRayTraceHit& hitInfo)
{
	if(m_pSkinModel)
	{
		PhysAABB rtAABB;
		A3DAABB aabb = m_pSkinModel->GetModelAABB();
		aabb.CompleteMinsMaxs();
		rtAABB.vMin = aabb.Mins;
		rtAABB.vMax = aabb.Maxs;

		if(RayTraceAABB(ray, rtAABB, hitInfo))
		{
			// need we make some further raytracing such as raytracing the mesh?...
			hitInfo.UserData = this;
			return true;
		}
		else
		{
			return false;
		}


	}
	else
		return false;

}

A3DVECTOR3 CPhysModelObject::GetPos() 
{ 
	if(m_pSkinModel)
		return m_pSkinModel->GetPos();
	else
		return A3DVECTOR3(0);
}

void CPhysModelObject::GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) 
{ 
	if(m_pSkinModel)
	{
		vDir = m_pSkinModel->GetDir();
		vUp = m_pSkinModel->GetUp();
	}
}

void CPhysModelObject::Move(const A3DVECTOR3& vDelta)
{
	if (m_pSkinModel)
	{
		A3DVECTOR3 vCurPos = m_pSkinModel->GetPos();
		m_pSkinModel->SetPos(vCurPos + vDelta);
		m_pSkinModel->Update(0);
		if(m_pModelPhys)
		{
			//m_pModelPhys->Teleport();
		}
	}


}