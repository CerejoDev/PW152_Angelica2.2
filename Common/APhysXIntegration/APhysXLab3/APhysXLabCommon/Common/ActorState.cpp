

#include "stdafx.h"

#include "ActorState.h"

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


ActorState::ActorState()
{
	m_state = Normal;
	m_pPlayer = NULL;
	m_fTransTime = 0;
	m_fTransCnt = 0;
	m_pPelvisCtr = NULL;
	m_pPelvisBone = NULL;
}
ActorState::~ActorState()
{
	Release();
}

void ActorState::Release()
{
	if (m_pPelvisCtr && m_pPelvisBone)
	{
		m_pPelvisBone->SetFirstController(NULL);
		m_pPelvisBone = NULL;
		delete m_pPelvisCtr;
		m_pPelvisCtr = NULL;
	}
	m_state = Normal;
	m_pPlayer = NULL;
	m_fTransTime = 0;
	m_fTransCnt = 0;
	m_strAnim.clear();
	m_QuatsRagdoll.clear();
	m_QuatsTarget.clear();
	m_matTarget.clear();
	m_matRagdoll.clear();
}

bool ActorState::BindPlayer(ApxActorBase* pPlayer)
{
	if (NULL == pPlayer)
		return false;
	if (m_pPlayer == pPlayer)
		return true;

	Release();
	m_pPlayer = pPlayer;
	m_strAnim.push_back("Getup_back");
	m_strAnim.push_back("Getup_front");

	m_pPelvisBone = m_pPlayer->GetObjDyn()->GetSkinModel()->GetSkeleton()->GetBone(2);
	m_pPelvisCtr = new A3DBoneMatrixController(m_pPelvisBone);//bone controller
	if (0 != m_pPelvisCtr)
		m_pPelvisBone->SetFirstController(m_pPelvisCtr);
	return true;
}

void ActorState::Tick(float fDeltaTime)
{
	if (Ragdoll2Normal == m_state)
	{
		if (m_bGetUpAnim)
		{
			//start get up
			if (m_fGetUpCnt == 0)
			{
				//play stand up animation
				ApxActionInfo info;
				info.mAnimInfo.mActionName = m_strAnim[m_iAnim];
				info.mAnimInfo.mActionORMask = ApxActionORMask_None;
				info.mAnimInfo.mBasicAction = true;
				info.mAnimInfo.mBlendWalkRun = false;
				info.mAnimInfo.mLooping = false;
				info.mAnimInfo.mRestart = true;
				info.mPlayMode = ApxActionPlayMode_InstantPlay;

				m_pPlayer->PlayAction(ApxActionLayer_Move, info);

				m_fGetUpCnt += fDeltaTime;
			}
			else
			{
				m_fGetUpCnt += fDeltaTime;
				if (m_fGetUpCnt > m_fGetUpTime)
				{
					m_state = Normal;
					m_pPlayer->ResetCCBeforeRagdoll();
					return;
				}
			}
		}
		else
		{
			m_fTransCnt += fDeltaTime;
			a_ClampRoof(m_fTransCnt, m_fTransTime);		

			if (m_fGetUpTime != 0)//aimation exist
			{
				A3DSkinModel* pSkin = m_pPlayer->GetObjDyn()->GetSkinModel();
				A3DSkeleton* pSkeleton = pSkin->GetSkeleton();
				A3DQUATERNION quat;
				A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);

				pSkeleton->GetBone(0)->SetRelativeTM(m_matTarget[0]);
				pSkeleton->GetBone(1)->SetRelativeTM(m_matTarget[1]);
				float fFactor = 1;
				if (m_fTransTime != 0)
					fFactor = m_fTransCnt / m_fTransTime;

				m_pPlayer->GetObjDyn()->SetGPos(m_vPosTarget * fFactor + m_vPosRagdoll * (1 - fFactor));		

				A3DBone* pBone = pSkeleton->GetBone(2);
				A3DQUATERNION q1, q2;
				q1.ConvertFromMatrix(m_matRagdollPelvis);
				q2.ConvertFromMatrix(m_matTargetPelvis);
				quat = SLERPQuad(q1, q2, fFactor);
				quat.ConvertToMatrix(mat);
				A3DVECTOR3 newPos = Mat44_GetTransPart(m_matRagdollPelvis) * (1 - fFactor) + Mat44_GetTransPart(m_matTargetPelvis) * fFactor; 
				Mat44_SetTransPart(newPos, mat);
				A3DMATRIX4 matController = m_matTarget[2].GetInverseTM() * mat * m_matTargetBip01.GetInverseTM();
				m_pPelvisCtr->CombineMatrix(matController, 0);

				for (size_t i = 3; i < m_QuatsRagdoll.size(); i++)
				{
					pBone = pSkeleton->GetBone(i);
					quat = SLERPQuad(m_QuatsRagdoll[i], m_QuatsTarget[i], fFactor);
					quat.ConvertToMatrix(mat);

					newPos = Mat44_GetTransPart(m_matTarget[i]) * fFactor + Mat44_GetTransPart(m_matRagdoll[i]) * (1 - fFactor); 
					Mat44_SetTransPart(newPos, mat);
					pBone->SetRelativeTM(mat);
				}

				if (m_fTransCnt == m_fTransTime)
				{
					m_bGetUpAnim = true;
				}
			}
		}
	}
	else if (Ragdoll == m_state)
	{
		m_pPelvisCtr->CombineMatrix(IdentityMatrix(), 0);
	}
	else if (Normal == m_state)
	{
		m_pPelvisCtr->CombineMatrix(IdentityMatrix(), 0);
	}
}

bool ActorState::TranState(State state, float fTransTime)
{
	if (m_state == state)
		return true;

	if (Normal == m_state && Ragdoll == state)
	{
		m_state = state;
		return true; 
	}

	if (Ragdoll == m_state && Normal == state) 
	{
		assert(fTransTime >= 0);
		m_state = Ragdoll2Normal;
		m_fTransTime = fTransTime;
		m_fTransCnt = 0;
		m_bGetUpAnim = false;
		m_fGetUpCnt = 0;
		m_fGetUpTime = 0;

		AString strPelvis = "Bip01 Pelvis";
		AString strLUpperArm = "Bip01 L UpperArm";
		AString strRUpperArm = "Bip01 R UpperArm";

		A3DVECTOR3 vPelvisZ, vLUpperArmZ, vRUpperArmZ;
		A3DSkinModel* pSkin = m_pPlayer->GetObjDyn()->GetSkinModel();
		A3DSkeleton* pSkeleton = pSkin->GetSkeleton();
		int index;
		A3DBone* pBone = pSkeleton->GetBone(strPelvis, &index);
		vPelvisZ = pBone->GetAbsoluteTM().GetRow(2);
		pBone = pSkeleton->GetBone(strLUpperArm, &index);
		vLUpperArmZ = pBone->GetAbsoluteTM().GetRow(2);
		pBone = pSkeleton->GetBone(strRUpperArm, &index);
		vRUpperArmZ = pBone->GetAbsoluteTM().GetRow(2);

		m_iAnim = -1;//animation index
		float fMax = -100;
		for ( size_t i = 0; i < m_strAnim.size(); i++)
		{
			A3DMATRIX4 mat;
			float fSum = 0;
			bool bRet = pSkin->GetBoneStateAtFrame(m_strAnim[i], 0, strPelvis, mat, false);
			if (!bRet)
				continue;
			fSum += DotProduct(mat.GetRow(2), vPelvisZ);
			bRet = pSkin->GetBoneStateAtFrame(m_strAnim[i], 0, strLUpperArm, mat, false);
			if (!bRet)
				continue;
			fSum += DotProduct(mat.GetRow(2), vLUpperArmZ);
			bRet = pSkin->GetBoneStateAtFrame(m_strAnim[i], 0, strRUpperArm, mat, false);
			if (!bRet)
				continue;
			fSum += DotProduct(mat.GetRow(2), vRUpperArmZ);
			if (fSum > fMax)
			{
				fMax = fSum;
				m_iAnim = i;
			}
		}
		if (-1 == m_iAnim)
		{
			m_state = Normal;
			return false; 
		}

		//bone state
		m_QuatsRagdoll.clear();
		m_QuatsTarget.clear();
		m_matTarget.clear();
		m_matRagdoll.clear();
		m_QuatsRagdoll.reserve(pSkeleton->GetBoneNum());
		m_QuatsTarget.reserve(pSkeleton->GetBoneNum());
		m_matTarget.reserve(pSkeleton->GetBoneNum());
		m_matRagdoll.reserve(pSkeleton->GetBoneNum());

		A3DQUATERNION quat;
		abase::hash_map<A3DBone*, int> BonesMap;
		A3DMATRIX4 matParent, matSelf;
		for (int i = 0; i < pSkeleton->GetBoneNum(); i++)
		{
			pBone = pSkeleton->GetBone(i);
			matSelf = pBone->GetAbsoluteTM();
			pBone = pBone->GetParentPtr();
			if (pBone)
				matParent = pBone->GetAbsoluteTM();
			else
				matParent = pSkeleton->GetA3DSkinModel()->GetAbsoluteTM();
			matSelf = matSelf * matParent.GetInverseTM();

			quat.ConvertFromMatrix(matSelf);
			m_QuatsRagdoll.push_back(quat);
			m_matRagdoll.push_back(matSelf);
			//quat = pSkeleton->GetBone(i)->GetRelativeQuat();
			//m_QuatsRagdoll.push_back(quat);
			//m_matRagdoll.push_back(pSkeleton->GetBone(i)->GetRelativeTM());
			
			BonesMap[pSkeleton->GetBone(i)] = i;
		}

		A3DMATRIX4* pMat = new A3DMATRIX4[pSkeleton->GetBoneNum()];
		//pMat: world matrix
		bool bIsSuccess = false;
#ifdef _ANGELICA3
		bIsSuccess = pSkin->GetBonesStateAtFrame(m_strAnim[m_iAnim], 0, pMat);
#else
		AArray<A3DMATRIX4> aOutMats;
		bIsSuccess = pSkin->GetBonesStateAtFrame(m_strAnim[m_iAnim], 0, aOutMats);
		if (bIsSuccess)
		{
			int nCount = aOutMats.GetSize();
			for (int i = 0; i < nCount; ++i)
				pMat[i] = aOutMats[i];
		}
#endif
		if (!bIsSuccess)
		{
			delete[] pMat;
			assert("animation not exist" && 0);
			m_state = Normal;
			return false;
		}
		m_fGetUpTime = (float)(pSkin->GetAction(m_strAnim[m_iAnim])->GetActionTime()) * 0.001f;

		//compute skin model position
		m_matRagdollPelvis = pSkeleton->GetBone(2)->GetAbsoluteTM();
		A3DMATRIX4 matModel = pMat[2].GetInverseTM() * m_matRagdollPelvis;

		
		A3DVECTOR3 vUp(0, 1, 0);
		A3DVECTOR3 vRotAxis = CrossProduct(matModel.GetRow(1), vUp);
		vRotAxis.Normalize();
		float fDot = DotProduct(matModel.GetRow(1), vUp);
		A3DMATRIX4 matRot(A3DMATRIX4::IDENTITY);
		matRot.RotateAxis(vRotAxis, acosf(fDot));

		A3DVECTOR3 vPos = Mat44_GetTransPart(matModel);
		A3DMATRIX4 matModelTarget = (matModel * matRot);//target matrix
		Mat44_SetTransPart(vPos, matModelTarget);
		
		A3DVECTOR3 vOff = (Mat44_GetTransPart(pMat[2]) * matModelTarget) - Mat44_GetTransPart(m_matRagdollPelvis);
		vPos = vPos - vOff;
		m_vPosRagdoll = vPos;
		while(true)
		{
			if (m_pPlayer->CheckPosValidate(vPos))
				break;
			vPos.y += 0.05f; 
		}
		m_vPosTarget = vPos;
		m_pPlayer->GetObjDyn()->SetGPos(m_vPosRagdoll);//make sure pelvis at its position
		m_pPlayer->GetObjDyn()->SetGDirAndUp(matModelTarget.GetRow(2), matModelTarget.GetRow(1));//final direction and up
		Mat44_SetTransPart(m_vPosTarget, matModelTarget);

		m_matTargetPelvis = pMat[2] * matModelTarget;
		m_matTargetBip01 = pMat[1] * matModelTarget;

		for (int i = 0; i < pSkeleton->GetBoneNum(); i++)
		{
			pBone = pSkeleton->GetBone(i);
			A3DMATRIX4 matParent(A3DMATRIX4::IDENTITY);
			if (pBone->GetParentPtr())
				matParent = pMat[BonesMap[pBone->GetParentPtr()]];				

			A3DMATRIX4 mat = pMat[BonesMap[pBone]] * matParent.GetInverseTM();
			quat.ConvertFromMatrix(mat);
			m_QuatsTarget.push_back(quat);
			m_matTarget.push_back(mat);
		}

		A3DMATRIX4 matController = m_matTarget[2].GetInverseTM() * m_matRagdollPelvis * m_matTargetBip01.GetInverseTM();
		m_pPelvisCtr->CombineMatrix(matController, 0);
		delete[] pMat; 
		return true;
	}

	return false;
}
