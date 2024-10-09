

#include "stdafx.h"

#include "UpperBodyControl.h"
#include "ECBodyControl.h"

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

UpperBodyCtrl::UpperBodyCtrl()
{
	m_pPlayer = NULL;
	m_fTargetBendDeg = 0;
	m_fBendDegCur = 0;
	m_fBendDegVel = 0;

	m_pHeadCtrl = NULL;
	m_pNeckCtrl = NULL;
	m_pSpineCtrl0 = NULL;
	m_pSpineCtrl1 = NULL;
	m_pSpineCtrl2 = NULL;
	m_pSpineCtrl3 = NULL;
	m_pSpineCtrl4 = NULL;

	m_pHead = NULL;
	m_pNeck = NULL;
	m_pSpine0 = NULL;
	m_pSpine1 = NULL;
	m_pSpine2 = NULL;
	m_pSpine3 = NULL;
	m_pSpine4 = NULL;

	m_state = Free;

	m_pBoneCtrlInterface = NULL;
}
UpperBodyCtrl::~UpperBodyCtrl()
{
	Release();
}

//	Initialize object
bool UpperBodyCtrl::BindPlayer(ApxActorBase* pPlayer)
{
	CPhysXObjDynamic* pDynObj = pPlayer->GetObjDyn();
	if (0 == pDynObj)
		return false;

	Release();
	m_pPlayer = pPlayer;
	A3DSkeleton* pSkeleton = pDynObj->GetSkinModel()->GetSkeleton();

	m_pHeadCtrl = CreateBoneController(pSkeleton, "Bip01 Head", m_pHead);
	m_pNeckCtrl = CreateBoneController(pSkeleton, "Bip01 Neck", m_pNeck);
	m_pSpineCtrl0 = CreateBoneController(pSkeleton, "Bip01 Spine", m_pSpine0);
	m_pSpineCtrl1 = CreateBoneController(pSkeleton, "Bip01 Spine", m_pSpine1);
	m_pSpineCtrl2 = CreateBoneController(pSkeleton, "Bip01 Spine1", m_pSpine2);
	m_pSpineCtrl3 = CreateBoneController(pSkeleton, "Bip01 Spine2", m_pSpine3);
	m_pSpineCtrl4 = CreateBoneController(pSkeleton, "Bip01 Spine4", m_pSpine4);
	return true;
}

//	Create look at controller for specified bone and set reference axis for it
A3DBoneBodyTurnController* UpperBodyCtrl::CreateBoneController(A3DSkeleton* pSkeleton, const char* szBone, A3DBone* &pBone)
{
	int iBoneIdx = -1;
	pBone = pSkeleton->GetBone(szBone, &iBoneIdx);
	if (!pBone)
	{
		a_LogOutput(1, ("Warning in %s: Failed to get bone %s"), __FUNCTION__, szBone);
		return false;
	}

	//A3DBoneRotController* pController = new A3DBoneRotController(pBone);
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

void UpperBodyCtrl::Release()
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

	m_state = Free;

	m_pBoneCtrlInterface = NULL;
}

void UpperBodyCtrl::Tick(float fDeltaTime)
{		
	if (m_state == Bend)//弯曲
	{
		m_fBendDegCur += m_fBendDegVel * fDeltaTime;
		a_ClampRoof(m_fBendDegCur, m_fTargetBendDeg);
		
		if (m_pSpineCtrl2)
			m_pSpineCtrl2->SetTurnDeg(0.2f * m_fBendDegCur);
		if (m_pSpineCtrl3)
			m_pSpineCtrl3->SetTurnDeg(0.3f * m_fBendDegCur);
		if (m_pSpineCtrl4)
			m_pSpineCtrl4->SetTurnDeg(0.5f * m_fBendDegCur);

		if (m_fBendDegCur == m_fTargetBendDeg)
		{
			m_state = Renew;
			m_fBendDegVel = m_fTargetBendDeg / m_fReTime;
		}
	}
	else if (m_state == Renew)//从弯曲中恢复
	{
		m_fBendDegCur -= m_fBendDegVel * fDeltaTime;
		a_ClampFloor(m_fBendDegCur, 0.0f);

		if (m_pSpineCtrl2)
			m_pSpineCtrl2->SetTurnDeg(0.2f * m_fBendDegCur);
		if (m_pSpineCtrl3)
			m_pSpineCtrl3->SetTurnDeg(0.3f * m_fBendDegCur);
		if (m_pSpineCtrl4)
			m_pSpineCtrl4->SetTurnDeg(0.5f * m_fBendDegCur);

		if (m_fBendDegCur == 0)//已恢复到初始位置
		{
			m_state = Free;
			m_fBendDegVel = 0;

			if (m_pBoneCtrlInterface)
				m_pBoneCtrlInterface->OnEndBoneCtrl();
		}
	}
	else if (m_state == Free)//自由状态，不受BoneController影响
	{
		if (m_pSpineCtrl2)
			m_pSpineCtrl2->SetTurnDeg(0);
		if (m_pSpineCtrl3)
			m_pSpineCtrl3->SetTurnDeg(0);
		if (m_pSpineCtrl4)
			m_pSpineCtrl4->SetTurnDeg(0);
	}
}

void UpperBodyCtrl::BendBody(const A3DVECTOR3& vBendDir, float fBendDeg, float fBendTime, float fReTime)
{
	m_fTargetBendDeg = fBendDeg;
	m_fBendDegCur = 0;
	m_fReTime = fReTime;
	m_vBendDirH = vBendDir;
	m_fBendDegVel = fBendDeg / (fBendTime);
	
	A3DVECTOR3 vUp(0, 1, 0);
	m_vRotAxis = CrossProduct(vUp, vBendDir);

#ifdef _ANGELICA3

	m_pSpineCtrl2->SetTurnType(A3DBoneBodyTurnController::BODYTURN_AXIS_GLOBAL, &m_vRotAxis);
	m_pSpineCtrl3->SetTurnType(A3DBoneBodyTurnController::BODYTURN_AXIS_GLOBAL, &m_vRotAxis);
	m_pSpineCtrl4->SetTurnType(A3DBoneBodyTurnController::BODYTURN_AXIS_GLOBAL, &m_vRotAxis);

#endif

	m_state = Bend;

	if (m_pBoneCtrlInterface)
		m_pBoneCtrlInterface->OnStartBoneCtrl();
}