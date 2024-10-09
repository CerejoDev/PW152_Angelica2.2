#include "StdAfx.h"
#include "IKSolverArm.h"

#ifdef _ANGELICA3
	#include <A3DIKGoal.h>

IKSolverArm::IKSolverArm(A3DEngine* pA3DEngine) : A3DIKSolver2Joints(pA3DEngine)
{
	m_state = State_Normal;
	m_dwTransCnt = 0;
	m_dwTransTime = m_dwFKToIKTime;

	m_pSolverInterface = NULL;
}

IKSolverArm::~IKSolverArm(void)
{
}

bool IKSolverArm::Solver()
{
	if (m_pSolverInterface && !m_pSolverInterface->PreSolver())
		return false;

	A3DVECTOR3 vPos;//IKÎ»ÖÃ
	if (IsEnabled() && GetIKGoal()->GetGoalPos(vPos) && m_dwTransTime)
	{
		//hand position from animation		
		A3DBone* pHandBone = GetBoneByJoint(GetEEJoint());
		A3DVECTOR3 vPosAnim = pHandBone->GetAbsoluteTM().GetTransPart();//¶¯»­Î»ÖÃ
		float fFactor = (float)m_dwTransCnt / m_dwTransTime;
		
		if (m_state == State_FK2IK)
			vPos = vPos * fFactor + vPosAnim * (1 - fFactor);
		else if (m_state == State_IK2FK)
			vPos = vPos * (1 - fFactor) + vPosAnim * fFactor;

		SetIKGoal_Generic(vPos);
	}

	bool bRet = __super::Solver();
	
	if (m_pSolverInterface)
		bRet = (bRet && m_pSolverInterface->PostSolver());
	
	return bRet;
}

//state from fk to ik (m_dwTransTime), delay dwResistance ms
//validate when current state is normal
void IKSolverArm::SetStateFK2IK(DWORD dwResistance)
{
	//if (m_state == State_Normal)
	{
		m_state = State_FK2IK;
		Enable(true);
		m_dwTransCnt = 0;
		m_dwTransResistance = dwResistance;
	}
}
//validate when current state is normal
//state from ik to fk (m_dwTransTime), delay dwResistance ms
void IKSolverArm::SetStateIK2FK(DWORD dwResistance)
{
	//if (m_state == State_Normal)
	{
		m_state = State_IK2FK;
		m_dwTransCnt = 0;
		m_dwTransResistance = dwResistance;
	}
}

void IKSolverArm::Tick(DWORD dwDelta)
{
	if (m_state == State_FK2IK || m_state == State_IK2FK)
	{
		if (m_dwTransResistance)
		{
			if (m_dwTransResistance < dwDelta)
				m_dwTransResistance = 0;
			else
				m_dwTransResistance -= dwDelta;
			
			return;
		}

		m_dwTransCnt += dwDelta;
		a_Clamp(m_dwTransCnt, (DWORD)0, m_dwTransTime);
	}

	if (m_dwTransCnt == m_dwTransTime)
	{
		if (m_state == State_FK2IK || m_state == State_IK2FK)
		{
			if (m_state == State_IK2FK)
				Enable(false);			

			m_state = State_Normal;
		}

		m_dwTransCnt = 0;
	}
}


//	Bind IK solver with specified skin model
bool IKSolverArm::BindWithModel(A3DSkinModel* pSkinModel)
{
	m_dwTransCnt = 0;

	return __super::BindWithModel(pSkinModel);	
}
//	Unbind IK solver from skin model
void IKSolverArm::UnbindFromModel()
{
	m_dwTransCnt = 0;

	return __super::UnbindFromModel();
}

bool IKSolverArm::PostRefreshSkeleton()
{
	bool bRet = __super::PostRefreshSkeleton();

	if (m_pSolverInterface)
		return bRet && m_pSolverInterface->PostRefreshSkeleton();
	
	return true;
}

#else
	int gRemove_Warning_LNK4221 = 0;
#endif
