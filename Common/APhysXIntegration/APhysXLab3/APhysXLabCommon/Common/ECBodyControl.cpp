/*
 * FILE: ECBodyControl.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2007/8/1
 *
 * HISTORY: 
 *
 * Copyright (c) 2007 Archosaur Studio, All Rights Reserved.
 */

#include "stdafx.h"
#include "ECBodyControl.h"
//#include "ECUtility.h"

#include <A3DSkeleton.h>
#include <A3DBone.h>
#include <A3DBoneController.h>
#include <A3DMacros.h>
#include <A3DFuncs.h>
#include <A3DSkinModel.h>


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

A3DVECTOR3 glb_YawToVector(float fYaw)
{
	float fRad = DEG2RAD(fYaw);

	A3DVECTOR3 v;
	v.x = 10.0f * cosf(fRad);
	v.z = 10.0f * sinf(fRad);
	v.y = 0.0f;
	v.Normalize();

	return v;
}

//	Clamp yaw to [-180.0f, 180.0f]
void glb_ClampYaw(float& fYaw)
{
	if (fYaw >= -180.0f && fYaw <= 180.0f)
	{
		//	Below clamp calculation will bring little error to original number, this will 
		//	cause problem in yaw's equal comparing. So if original yaw has been in valid
		//	range, return directly.
		return;
	}

	static float fInv = 1.0f / 360.0f;

	fYaw += 180.0f;
	if (fYaw < 0.0f)
	{
		int n = (int)(-fYaw * fInv) + 1;
		fYaw = fYaw + 360.0f * n;
	}
	else
	{
		int n = (int)(fYaw * fInv);
		fYaw = fYaw - 360.0f * n;
	}

	fYaw -= 180.0f;
	ASSERT(fYaw >= -180.0f && fYaw <= 180.0f);
}


float glb_VectorToYaw(const A3DVECTOR3& v)
{
	A3DVECTOR3 v1 = v;
	v1.y = 0.0f;
	v1.Normalize();
	return RAD2DEG((float)atan2(v1.z, v1.x));
}

//	Select the lesser angle between fYaw1 and fYaw2 and calculate (fYaw1 - fYaw2)
float glb_YawLessSub(float fYaw1, float fYaw2)
{
	if (fYaw1 == fYaw2)
		return 0.0f;

	ASSERT(fYaw1 >= -180.0f && fYaw1 <= 180.0f);
	ASSERT(fYaw2 >= -180.0f && fYaw2 <= 180.0f);

	float fDeltaYaw = fYaw1 - fYaw2;
	if (fDeltaYaw < -180.0f)
		fDeltaYaw += 360.0f;
	else if (fDeltaYaw > 180.0f)
		fDeltaYaw -= 360.0f;

	return fDeltaYaw;
}

float glb_DoYaw(float fCurYaw, float fDestYaw, float fSpeed, float fTime, int iYawFlag/* EC_DOYAW_NEAR */)
{
	if (fCurYaw == fDestYaw)
		return fDestYaw;

	ASSERT(fDestYaw >= -180.0f && fDestYaw <= 180.0f);

	float fDeltaYaw, fYawDir;

	if (fDestYaw < -200.0f)
	{
		fDeltaYaw = FLT_MAX;
		fYawDir = (iYawFlag == EC_DOYAW_CW) ? -1.0f : 1.0f;
	}
	else
	{
		if (iYawFlag == EC_DOYAW_CW)
		{
			fDeltaYaw = (fDestYaw < fCurYaw) ? fCurYaw-fDestYaw : fCurYaw+360.0f-fDestYaw;
			fYawDir = -1.0f;
		}
		else if (iYawFlag == EC_DOYAW_CCW)
		{
			fDeltaYaw = (fDestYaw < fCurYaw) ? 360.0f-fCurYaw+fDestYaw : fDestYaw-fCurYaw;
			fYawDir = 1.0f;
		}
		else	//	EC_DOYAW_NEAR
		{
			fYawDir = 1.0f;
			fDeltaYaw = glb_YawLessSub(fDestYaw, fCurYaw);

			if (fDeltaYaw < 0.0f)
			{
				fYawDir = -1.0f;
				fDeltaYaw = -fDeltaYaw;
			}
		}
	}

	float fCurDelta = fSpeed * fTime;
	if (fCurDelta >= fDeltaYaw)
		fCurYaw = fDestYaw;
	else
		fCurYaw += fCurDelta * fYawDir;

	glb_ClampYaw(fCurYaw);

	return fCurYaw;
}



///////////////////////////////////////////////////////////////////////////
//	
//	Implement ECPlayerBodyCtrl
//	
///////////////////////////////////////////////////////////////////////////

ECPlayerBodyCtrl::ECPlayerBodyCtrl()
{
	m_pPlayer		= NULL;
	m_fHeadYaw		= 0.0f;
	m_fBodyYaw		= 0.0f;
	m_fOldHeadYaw	= 0.0f;
	m_bBodyFollow	= false;

	m_pHeadCtrl		= NULL;
	m_pNeckCtrl		= NULL;
	m_pSpineCtrl0	= NULL;
	m_pSpineCtrl1	= NULL;
	m_pSpineCtrl2	= NULL;
	m_pSpineCtrl3	= NULL;
	m_pSpineCtrl4	= NULL;

	m_pHead		= NULL;
	m_pNeck		= NULL;
	m_pSpine0	= NULL;
	m_pSpine1	= NULL;
	m_pSpine2	= NULL;
	m_pSpine3	= NULL;
	m_pSpine4	= NULL;

	m_TurnBody.bTurn	= false;
	m_TurnHead.bTurn	= false;
	m_TurnBody.fDestYew	= 0.0f;
	m_TurnBody.iYawFlag	= -1;
	m_TurnHead.fDestYew	= 0.0f;
	m_TurnHead.iYawFlag	= -1;
	m_fHeadSpeedYaw		= 0.0f;
}

ECPlayerBodyCtrl::~ECPlayerBodyCtrl()
{
	Release();
}

void ECPlayerBodyCtrl::Release()
{
	m_pPlayer = 0;
	if (m_pHeadCtrl)
	{
		m_pHead->SetFirstController(NULL);
		delete m_pHeadCtrl;
		m_pHeadCtrl = NULL;
		m_pHead = NULL;
	}

	if (m_pNeckCtrl)
	{
		m_pNeck->SetFirstController(NULL);
		delete m_pNeckCtrl;
		m_pNeckCtrl = NULL;
		m_pNeck = NULL;
	}

	if (m_pSpineCtrl0)
	{
		m_pSpine0->SetFirstController(NULL);
		delete m_pSpineCtrl0;
		m_pSpineCtrl0 = NULL;
		m_pSpine0 = NULL;
	}

	if (m_pSpineCtrl1)
	{
		m_pSpine1->SetFirstController(NULL);
		delete m_pSpineCtrl1;
		m_pSpineCtrl1 = NULL;
		m_pSpine1 = NULL;
	}

	if (m_pSpineCtrl2)
	{
		m_pSpine2->SetFirstController(NULL);
		delete m_pSpineCtrl2;
		m_pSpineCtrl2 = NULL;
		m_pSpine2 = NULL;
	}

	if (m_pSpineCtrl3)
	{
		m_pSpine3->SetFirstController(NULL);
		delete m_pSpineCtrl3;
		m_pSpineCtrl3 = NULL;
		m_pSpine3 = NULL;
	}

	if (m_pSpineCtrl4)
	{
		m_pSpine4->SetFirstController(NULL);
		delete m_pSpineCtrl4;
		m_pSpineCtrl4 = NULL;
		m_pSpine4 = NULL;
	}
}

//	Initialize object
bool ECPlayerBodyCtrl::BindPlayer(ApxActorBase* pPlayer)
{
	CPhysXObjDynamic* pDynObj = pPlayer->GetObjDyn();
	if (0 == pDynObj)
		return false;

	Release();
	m_pPlayer = pPlayer;
	A3DSkeleton* pSkeleton = pDynObj->GetSkinModel()->GetSkeleton();

	//	Create bone controller for head bone
	m_pHeadCtrl = CreateBoneController(pSkeleton, "Bip01 Head", m_pHead);
	if (0 != m_pHeadCtrl)
		m_pHeadCtrl->SetTurnDeg(0.0f);
	
	//	Create bone controller for neck bone
	m_pNeckCtrl = CreateBoneController(pSkeleton, "Bip01 Neck", m_pNeck);
	if (0 != m_pNeckCtrl)
		m_pNeckCtrl->SetTurnDeg(0.0f);

	//	Create bone controller for spine bone
	m_pSpineCtrl0 = CreateBoneController(pSkeleton, "Bip01 Spine", m_pSpine0);
	if (0 != m_pSpineCtrl0)
		m_pSpineCtrl0->SetTurnDeg(0.0f);
	
	//	Create bone controller for spine bone
	m_pSpineCtrl1 = CreateBoneController(pSkeleton, "Bip01 Spine", m_pSpine1);
	if (0 != m_pSpineCtrl1)
		m_pSpineCtrl1->SetTurnDeg(0.0f);
	
	//	Create bone controller for spine bone
	m_pSpineCtrl2 = CreateBoneController(pSkeleton, "Bip01 Spine1", m_pSpine2);
	if (0 != m_pSpineCtrl2)
		m_pSpineCtrl2->SetTurnDeg(0.0f);

	//	Create bone controller for spine bone
	m_pSpineCtrl3 = CreateBoneController(pSkeleton, "Bip01 Spine2", m_pSpine3);
	if (0 != m_pSpineCtrl3)
		m_pSpineCtrl3->SetTurnDeg(0.0f);

	//	Create bone controller for spine bone
	m_pSpineCtrl4 = CreateBoneController(pSkeleton, "Bip01 Spine4", m_pSpine4);
	if (0 != m_pSpineCtrl4)
		m_pSpineCtrl4->SetTurnDeg(0.0f);
	return true;
}

//	Create look at controller for specified bone and set reference axis for it
A3DBoneBodyTurnController* ECPlayerBodyCtrl::CreateBoneController(A3DSkeleton* pSkeleton, const char* szBone, A3DBone* &pBone)
{
	int iBoneIdx = -1;
	pBone = pSkeleton->GetBone(szBone, &iBoneIdx);
	if (!pBone)
	{
		a_LogOutput(1, ("Warning in %s: Failed to get bone %s"), __FUNCTION__, szBone);
		return false;
	}
	
	A3DBoneBodyTurnController* pController = new A3DBoneBodyTurnController(pBone);
	if (!pController)
	{
		a_LogOutput(1, ("Warning in %s: Failed to create A3DBoneBodyTurnController"), __FUNCTION__);
		return false;
	}

	//	Bind bone and controller
	pBone->SetFirstController(pController);
	return pController;
}

//	Tick rotate head routine
void ECPlayerBodyCtrl::Tick(float fDeltaTime)
{
	if (m_TurnBody.bTurn)
	{
		m_fBodyYaw = glb_DoYaw(m_fBodyYaw, m_TurnBody.fDestYew,m_TurnBody.fSpeed, fDeltaTime, m_TurnBody.iYawFlag);
		//	If degree between body and head is too large
		float fDelta = (float)fabs(m_fBodyYaw - m_fHeadYaw);
		if (fDelta >= 180.0f)
			fDelta = 360.0f - fDelta;

		if (fDelta > MAX_HEADBODYDELTA)
		{	
			m_fHeadYaw = m_fBodyYaw;
		}

		m_fHeadYaw = glb_DoYaw(m_fHeadYaw, m_TurnBody.fDestYew,m_TurnBody.fSpeed*1.5f, fDeltaTime, m_TurnBody.iYawFlag);
		if (m_fBodyYaw == m_TurnBody.fDestYew)
		{
			m_TurnBody.bTurn = false;
			m_fHeadYaw = m_fBodyYaw;
		}

	} else if (m_TurnHead.bTurn)
	{
		m_fHeadYaw = glb_DoYaw(m_fHeadYaw, m_TurnHead.fDestYew,m_TurnHead.fSpeed, fDeltaTime, m_TurnHead.iYawFlag);

		float fDelta = (float)fabs(m_fBodyYaw - m_fHeadYaw);
		if (fDelta >= 180.0f)
			fDelta = 360.0f - fDelta;

		if (fDelta > MAX_HEADBODYDELTA)
		{	
			m_fBodyYaw = m_fHeadYaw;
		}

		m_fBodyYaw = glb_DoYaw(m_fBodyYaw, m_TurnBody.fDestYew,m_TurnHead.fSpeed*1.5f, fDeltaTime, m_TurnHead.iYawFlag);
		if (m_fHeadYaw == m_TurnHead.fDestYew&&m_fBodyYaw == m_TurnBody.fDestYew)
		{
			m_TurnBody.fDestYew = m_fHeadYaw;
		}

		if (m_fBodyYaw == m_TurnBody.fDestYew)
		{
			m_TurnBody.bTurn = false;
		}

	}
	
	//m_fBodyYaw = m_TurnBody.fDestYew;
	//m_fHeadYaw = m_TurnBody.fDestYew;
	//m_fHeadYaw = m_fBodyYaw;

	if (m_pPlayer)
	{
		//	Change player's model direction
		A3DVECTOR3 vDir = glb_YawToVector(m_fBodyYaw);
		
		if (m_pPlayer->m_pGroundMoveController)
		{
			m_pPlayer->m_pGroundMoveController->SetHeadDir(APhysXConverter::A2N_Vector3(vDir));
		}		
		
		float fDegTurn = -glb_YawLessSub(m_fHeadYaw, m_fBodyYaw);

		if(m_pPlayer->HasHitTarget())
		{
			if (m_pSpineCtrl1)
				m_pSpineCtrl1->SetTurnDeg(fDegTurn * 0.0f);
			if (m_pSpineCtrl2)
				m_pSpineCtrl2->SetTurnDeg(fDegTurn * 0.1f);
			if (m_pSpineCtrl3)
				m_pSpineCtrl3->SetTurnDeg(fDegTurn * 0.1f);
			if (m_pSpineCtrl4)
				m_pSpineCtrl4->SetTurnDeg(fDegTurn * 0.3f);
			if (m_pNeckCtrl)
				m_pNeckCtrl->SetTurnDeg(fDegTurn * 0.2f);
			if (m_pHeadCtrl)
				m_pHeadCtrl->SetTurnDeg(fDegTurn * 0.3f);
		}
		else
		{
			if (m_pSpineCtrl1)
				m_pSpineCtrl1->SetTurnDeg(fDegTurn * 0.0f);
			if (m_pSpineCtrl2)
				m_pSpineCtrl2->SetTurnDeg(fDegTurn * 0.1f);
			if (m_pSpineCtrl3)
				m_pSpineCtrl3->SetTurnDeg(fDegTurn * 0.1f);
			if (m_pSpineCtrl4)
				m_pSpineCtrl4->SetTurnDeg(fDegTurn * 0.3f);
			if (m_pNeckCtrl)
				m_pNeckCtrl->SetTurnDeg(fDegTurn * 0.2f);
			if (m_pHeadCtrl)
				m_pHeadCtrl->SetTurnDeg(fDegTurn * 0.3f);
		}
	}
}


//	Rotate body
void ECPlayerBodyCtrl::RotateBody(int iYawFlag, float fDestYaw, float fSpeed)
{
	m_TurnHead.bTurn	= false;
	m_TurnBody.bTurn	= true;
	m_TurnBody.fDestYew	= fDestYaw;
	m_TurnBody.iYawFlag	= iYawFlag;
	m_TurnBody.fSpeed	= fSpeed;
}

//	Rotate Head
void ECPlayerBodyCtrl::RotateHead(int iYawFlag, float fHeadDestYaw,float fBodyDestYaw, float fSpeed)
{
	m_TurnBody.bTurn	= false;
	m_TurnBody.fDestYew	= fBodyDestYaw;

	m_TurnHead.bTurn	= true;
	m_TurnHead.fDestYew	= fHeadDestYaw;
	m_TurnHead.iYawFlag	= iYawFlag;
	m_TurnHead.fSpeed	= fSpeed;
}

void ECPlayerBodyCtrl::ResetBoneCtrl()
{
	if (m_pHeadCtrl)
		m_pHeadCtrl->SetTurnDeg(0);
	if (m_pNeckCtrl)
		m_pNeckCtrl->SetTurnDeg(0);
	if (m_pSpineCtrl1)
		m_pSpineCtrl1->SetTurnDeg(0);
	if (m_pSpineCtrl2)
		m_pSpineCtrl2->SetTurnDeg(0);
	if (m_pSpineCtrl3)
		m_pSpineCtrl3->SetTurnDeg(0);
	if (m_pSpineCtrl4)
		m_pSpineCtrl4->SetTurnDeg(0);
}
