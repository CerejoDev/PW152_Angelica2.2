
#include "stdafx.h"

#ifdef _ANGELICA3
	#include <A3DAnimNode.h>
#endif

// data for the old simple avatar
/*
static int s_iUpperBodyBones[] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 
									21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
									36, 37, 38, 39, 40, 41, 42, 43, 44, 45};
static int s_iLowerBodyBones[] = {0, 1, 2, 3, 46, 47, 48, 49, 50, 51, 52, 53};
*/



#define AVATAR_UPPERBODY_ROOT_BONE_ID 4

// this is a recursive functions...
static void ApxGetOffspringBones_r(A3DSkinModel* pSkinModel, int iBoneID, APhysXArray<int>& arrOffspringBones)
{
	if(pSkinModel)
	{
		A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();
		A3DBone* pCurBone = pSkeleton->GetBone(iBoneID);

		if(pCurBone)
		{
			for(int i=0; i<pCurBone->GetChildNum(); i++)
			{
				int iChildBoneID = pCurBone->GetChild(i);
				arrOffspringBones.pushBack(iChildBoneID);
				ApxGetOffspringBones_r(pSkinModel, iChildBoneID, arrOffspringBones);
			}
		}
	}
}

void ApxSkinModelActionController::Init()
{
	ApxActionController::Init();
	CECModel* pECModel = m_pDynObj->GetECModel();

	APHYSX_ASSERT(m_pSkinModel);
	APHYSX_ASSERT(pECModel);
	

	// init both two channels...
	APhysXArray<int> arrUpperBodyBones, arrLowerBodyBones;
	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	for(int i=0; i<pSkeleton->GetBoneNum(); i++)
	{
		arrLowerBodyBones.pushBack(i);
	}

	// add the root bone of the upper-body branch to array...
	arrUpperBodyBones.pushBack(AVATAR_UPPERBODY_ROOT_BONE_ID);
	// add all the offspring of that root bone...
	ApxGetOffspringBones_r(m_pSkinModel, AVATAR_UPPERBODY_ROOT_BONE_ID, arrUpperBodyBones);


	for(unsigned int i=0; i<arrUpperBodyBones.size(); i++)
	{
		arrLowerBodyBones.deleteEntry(arrUpperBodyBones[i]);
	}

	for(int i = 0, j = 1 ; i<A3DSkinModel::ACTCHA_MAX && j<APX_SKINMODEL_ACTIONCHANNEL_NUM; i++)
	{
		if(!m_pSkinModel->GetActionChannel(i))
		{
			// tmp code, this will build full-body channel...
			if (j == ApxActionChannel_UpperBody)
			{
				m_pSkinModel->BuildActionChannel(i, arrUpperBodyBones.size(), &arrUpperBodyBones[0]);
				pECModel->SetActionEventMask(i, 0xffffffff);
			}
			else if (j == ApxActionChannel_LowerBody)
			{
				m_pSkinModel->BuildActionChannel(i, arrLowerBodyBones.size(), &arrLowerBodyBones[0]);
				pECModel->SetActionEventMask(i, 0);
			}

			m_ActionChannels[j] = i;
			j++;
		}
	}

	m_bUseWalkRun = true;
	m_pAnimBlend1 = NULL;
	m_pAnimBlend2 = NULL;
	OnChangeActiveChannel(0.0f);
}

void ApxSkinModelActionController::SetActionName(const char* strLAction, const char* strLInvAction, const char* strRAction, const char* strRInvAction,
				   const char* strFAction, const char* strBAction, const char* strFWalk)
{
#ifdef _ANGELICA3
	ReleaseAminBlend();

	m_pAnimBlend1 = new A3DAnimNodeBlendBase;
	m_pAnimBlend2 = new A3DAnimNodeBlendBase;

	for (int i = 0; i < 2; i++)
	{
		A3DAnimNodeBlendBase* pBlend;
		if (i == 0)
			pBlend = m_pAnimBlend1;
		else 
			pBlend = m_pAnimBlend2;

		A3DAnimNodeAction* pAnimNode;
		A3DAnimNodeBlendWalkRun* pWRNode;

		pAnimNode = new A3DAnimNodeAction;
		pAnimNode->Init(m_pSkinModel, strLAction);
		AnimBlendChild BlendChild;
		BlendChild.mAnimNode = pAnimNode;
		BlendChild.mName = strLAction;
		BlendChild.mWeight = 0;
		pBlend->AddChild(BlendChild);

		pAnimNode = new A3DAnimNodeAction;
		pAnimNode->Init(m_pSkinModel, strLInvAction);
		BlendChild.mAnimNode = pAnimNode;
		BlendChild.mName = strLInvAction;
		BlendChild.mWeight = 0;
		pBlend->AddChild(BlendChild);

		pAnimNode = new A3DAnimNodeAction;
		pAnimNode->Init(m_pSkinModel, strRAction);
		BlendChild.mAnimNode = pAnimNode;
		BlendChild.mName = strRAction;
		BlendChild.mWeight = 0;
		pBlend->AddChild(BlendChild);

		pAnimNode = new A3DAnimNodeAction;
		pAnimNode->Init(m_pSkinModel, strRInvAction);
		BlendChild.mAnimNode = pAnimNode;
		BlendChild.mName = strRInvAction;
		BlendChild.mWeight = 0;
		pBlend->AddChild(BlendChild);

		pAnimNode = new A3DAnimNodeAction;
		pAnimNode->Init(m_pSkinModel, strFAction);
		BlendChild.mAnimNode = pAnimNode;
		BlendChild.mName = strFAction;
		BlendChild.mWeight = 0;
		pBlend->AddChild(BlendChild);

		pAnimNode = new A3DAnimNodeAction;
		pAnimNode->Init(m_pSkinModel, strBAction);
		BlendChild.mAnimNode = pAnimNode;
		BlendChild.mName = strBAction;
		BlendChild.mWeight = 0;
		pBlend->AddChild(BlendChild);

		pWRNode = new A3DAnimNodeBlendWalkRun;//blend walk run
		pWRNode->Init(m_pSkinModel, strFWalk, strFAction);
		BlendChild.mAnimNode = pWRNode;
		BlendChild.mName = "walk_Run";
		BlendChild.mWeight = 0;
		pBlend->AddChild(BlendChild);

		pBlend->SetChildWeight(0, 0);
		pBlend->SetChildWeight(1, 0);
		pBlend->SetChildWeight(2, 0);
		pBlend->SetChildWeight(3, 0);
		pBlend->SetChildWeight(4, 0);
		pBlend->SetChildWeight(5, 0);
		pBlend->SetChildWeight(6, 1);
	}
#endif

	OnChangeActiveChannel(0.0f);
	SetUseWalkRun(true);
}

void ApxSkinModelActionController::ReleaseAminBlend()
{
#ifdef _ANGELICA3
	int i;
	if (m_pAnimBlend1)
	{
		for (i = 0; i < m_pAnimBlend1->GetChildrenNum(); i++)
		{
			//m_pAnimBlend1->GetChild(i).mAnimNode->Stop();
			A3DRELEASE((m_pAnimBlend1->GetChild(i)).mAnimNode);
		}
		m_pAnimBlend1->RemoveAllChildren();
	}

	if (m_pAnimBlend2)
	{
		for (i = 0; i < m_pAnimBlend2->GetChildrenNum(); i++)
		{
			//m_pAnimBlend2->GetChild(i).mAnimNode->Stop();
			A3DRELEASE((m_pAnimBlend2->GetChild(i)).mAnimNode);
		}
		m_pAnimBlend2->RemoveAllChildren();
	}
	
	A3DRELEASE(m_pAnimBlend1);
	A3DRELEASE(m_pAnimBlend2);
#endif
}

void ApxSkinModelActionController::Release()
{
	ApxActionController::Release();

	// release the action channels...
	if(m_pSkinModel)
	{
		//release AnimBlend
		ReleaseAminBlend();

#ifdef _ANGELICA3
		for(int i=1; i<APX_SKINMODEL_ACTIONCHANNEL_NUM; i++)
			m_pSkinModel->ReleaseActionChannel(m_ActionChannels[i]);
#endif
	}
}

bool ApxSkinModelActionController::PlayAnim(const ApxActionAnimInfo& apxAnimInfo)
{
	// play the animation in both two channels ...
	int iNumLoop = (apxAnimInfo.mLooping)? -1: 1;
	int iTransTime = (int)(apxAnimInfo.mBlendInTime * 1000.0f);
	bool bRealPlay = true;

	for(int i=1; i<APX_SKINMODEL_ACTIONCHANNEL_NUM; i++)
	{
		if (apxAnimInfo.mBlendWalkRun)
		{
			//////走跑融合
			//A3DAnimNodeBlendWalkRun* pWalkRun = NULL;
			//if (i == 1)
			//	pWalkRun = m_pActorBase->GetBlendWalkRunUpper();
			//else if(i == 2)
			//	pWalkRun = m_pActorBase->GetBlendWalkRunLower();

			//if (pWalkRun)
			//	m_pSkinModel->PlayAnimNode(pWalkRun,  m_ActionChannels[i], 0, iNumLoop, iTransTime, apxAnimInfo.mRestart);

#ifdef _ANGELICA3
			//融合前后左右走
			A3DAnimNodeBlendBase* pBlend = NULL;
			if (i == 1)
			{
				pBlend = m_pAnimBlend1 ? m_pAnimBlend1 : m_pActorBase->GetBlendWalkRunUpper();
			}
			else if (i == 2)
			{
				pBlend = m_pAnimBlend2 ? m_pAnimBlend2 : m_pActorBase->GetBlendWalkRunLower();
			}
			if (pBlend)
			{
				m_pSkinModel->PlayAnimNode(pBlend, m_ActionChannels[i], 0, iNumLoop, iTransTime, apxAnimInfo.mRestart);
			}
#endif
		}
		else
		{
			if(apxAnimInfo.mBasicAction)
			{
				bRealPlay = m_pSkinModel->GetAction(apxAnimInfo.mActionName) != NULL;

				// basic action...
				if(bRealPlay && !apxAnimInfo.mRestart)
				{
#ifdef _ANGELICA3
					A3DSMActionChannel* pChannel = m_pSkinModel->GetActionChannel(m_ActionChannels[i]);
					if(pChannel->GetTopRankAction())
					{
						A3DSkinModelAction* pTopAction = pChannel->GetTopRankAction()->GetSMAction();
						if(pTopAction && apxAnimInfo.mActionName == pTopAction->GetCoreData()->GetName())
							bRealPlay = false;
					}
#else
					if (apxAnimInfo.mActionName == m_strCurrentActionName)
						bRealPlay = false;
#endif
				}

				if(bRealPlay)
				{
#ifndef _ANGELICA3
					A3DSkinModelActionCore* pAction = m_pSkinModel->GetAction(apxAnimInfo.mActionName);
					m_fCurActTotalTime = pAction->GetActionTime() * 0.001f; 
#endif
					m_pSkinModel->PlayActionByName(apxAnimInfo.mActionName, m_ActionChannels[i], iNumLoop, iTransTime, apxAnimInfo.mRestart);
				}
			}
			else if(m_pDynObj->GetECModel())
			{
				// complex action
				CECModel* pECModel = m_pDynObj->GetECModel();
				ApxAttackActionInfo* pAttackActionInfo = m_pCurActionInfo->IsAttackActionInfo();
				if(pAttackActionInfo)
				{
					A3DVECTOR3 vHitPos = APhysXConverter::N2A_Vector3(pAttackActionInfo->mApxDamage.mHitPos);
					bRealPlay = pECModel->GetComActByName(apxAnimInfo.mActionName) != NULL;
					if(bRealPlay)
					{
						pECModel->PlayAttackAction(m_ActionChannels[i], apxAnimInfo.mActionName, 0, reinterpret_cast<int>(m_pDynObj), 0, &vHitPos);
					}
					
				}
				else
				{
					bRealPlay = pECModel->PlayActionByName(m_ActionChannels[i], apxAnimInfo.mActionName, 1.0f);
				}
				
			}
		}
	}

#ifndef _ANGELICA3
	if (bRealPlay)
		m_strCurrentActionName = apxAnimInfo.mActionName;
#endif
	return bRealPlay;
}

bool ApxSkinModelActionController::IsAnimStopped(const ApxActionAnimInfo& apxAnimInfo)
{
#ifdef _ANGELICA3
	return m_pSkinModel->GetActionChannel(m_ActionChannels[ApxActionChannel_UpperBody])->IsActionStopped();
#else
	if (m_fTimeAfterPlay < m_fCurActTotalTime)
		return false;
	else
		return true;
#endif
	return false;
}

void ApxSkinModelActionController::OnChangeActiveChannel(float fBlendTime)
{
	if(m_activeChannel & ApxActionChannel_UpperBody)
	{
		m_apxWB[ApxActionChannel_UpperBody].SetTargetWeight(1.0f, fBlendTime);
	}
	else
	{
		m_apxWB[ApxActionChannel_UpperBody].SetTargetWeight(0.0f, fBlendTime);
	}

	if(m_activeChannel & ApxActionChannel_LowerBody)
	{
		m_apxWB[ApxActionChannel_LowerBody].SetTargetWeight(1.0f, fBlendTime);
	}
	else
	{
		m_apxWB[ApxActionChannel_LowerBody].SetTargetWeight(0.0f, fBlendTime);
	}

}


void ApxSkinModelActionController::PreTick(float dt)
{
	if(!m_pSkinModel) return;

	A3DSMActionChannel* pChannel = NULL;
	for(int i=1; i<APX_SKINMODEL_ACTIONCHANNEL_NUM; i++)
	{
		m_apxWB[i].Tick(dt);
		pChannel = m_pSkinModel->GetActionChannel(m_ActionChannels[i]);
		if (0 != pChannel)
			pChannel->SetWeight(m_apxWB[i].GetWeight());

#if 0
		// tmp code for debugging...

		if((m_apxActionLayer == ApxActionLayer_Move || m_apxActionLayer == ApxActionLayer_Active) && m_apxWB[i].GetWeight()>0.0f && m_apxWB[i].GetWeight()<1.0f)
			a_LogOutput(1, "ActionLayer: %d, Channel: %d, Weight: %f", m_apxActionLayer, i, m_apxWB[i].GetWeight());
#endif

	}	

#ifdef _ANGELICA3
	if (m_pAnimBlend1)
	{
		for (int i = 0; i < m_pAnimBlend1->GetChildrenNum(); i++)
		{
			m_apxWeight1[i].Tick(dt);
			m_pAnimBlend1->SetChildWeight(i, m_apxWeight1[i].GetWeight());
		}
	}
	
	if (m_pAnimBlend2)
	{
		for (int i = 0; i < m_pAnimBlend2->GetChildrenNum(); i++)
		{
			m_apxWeight2[i].Tick(dt);
			m_pAnimBlend2->SetChildWeight(i, m_apxWeight2[i].GetWeight());
		}
	}
#endif
}

void ApxSkinModelActionController::TickAnim(float dt)
{
	// do nothing currently...
}

float ApxSkinModelActionController::GetCurActionSpan()
{
	if(GetCurrentAction() && !GetCurrentAction()->mAnimInfo.mLooping)
	{
#ifdef _ANGELICA3
		A3DSMActionChannel* pActiveChannel = m_pSkinModel->GetActionChannel(m_ActionChannels[ApxActionChannel_UpperBody]);
		A3DAnimInterface* pAnimInt = pActiveChannel->GetTopRankAction();
		if(pAnimInt)
		{
			A3DSkinModelAction* pSMAction = pAnimInt->GetSMAction();
			if(pSMAction)
			{
				return pSMAction->GetCoreData()->GetEndTime() * 0.001f ;
			}
		}
#else
		return m_fCurActTotalTime; 
#endif
	}
	
	return -1.0f;
}

void ApxSkinModelActionController::SetActionLayer(ApxActionLayer apxActionLayer) 
{ 
	ApxActionController::SetActionLayer(apxActionLayer);
	
#if 1

	// some bad behavior...
	if(m_apxActionLayer == ApxActionLayer_Passive)
	{
		A3DSMActionChannel* pChannel = NULL;

		for(int i=1; i<APX_SKINMODEL_ACTIONCHANNEL_NUM; i++)
		{
			pChannel = m_pSkinModel->GetActionChannel(m_ActionChannels[i]);
			if (0 != pChannel)
				pChannel->SetPlayMode(A3DSMActionChannel::PLAY_COMBINE);
		}
	}

#endif

}

const char* ApxSkinModelActionController::GetFirstActionName() const
{
	if (0 == m_pSkinModel)
		return 0;

	A3DSkinModelActionCore* pSMAC = m_pSkinModel->GetFirstAction();
	if (0 == pSMAC)
		return 0;

	return pSMAC->GetName();
}

const char* ApxSkinModelActionController::GetNextActionName() const
{	
	if (0 == m_pSkinModel)
		return 0;

	A3DSkinModelActionCore* pSMAC = m_pSkinModel->GetNextAction();
	if (0 == pSMAC)
		return 0;

	return pSMAC->GetName();
}

void ApxSkinModelActionController::SetPlaySpeed(float fSpeed)
{
	ApxActionController::SetPlaySpeed(fSpeed);

#ifdef _ANGELICA3
	A3DSMActionChannel* pChannel = NULL;
	for(int i=1; i<APX_SKINMODEL_ACTIONCHANNEL_NUM; i++)
	{
		pChannel = m_pSkinModel->GetActionChannel(m_ActionChannels[i]);
		pChannel->SetPlaySpeed(fSpeed);
	}
#endif
}

void ApxSkinModelActionController::SetSpeed(float fSpeed)
{
#ifdef _ANGELICA3
	for (int k = 0; k < 2; k++)
	{
		A3DAnimNodeBlendBase* pBlend = NULL;
		if (k == 0)
			pBlend = m_pAnimBlend1;
		else 
			pBlend = m_pAnimBlend2;

		if (pBlend)
		{
			for (int i = 0; i < pBlend->GetChildrenNum(); i++)
			{
				A3DAnimNode* pNode = pBlend->GetChild(i).mAnimNode;
				A3DAnimNodeBlendWalkRun* pWalkRun = dynamic_cast<A3DAnimNodeBlendWalkRun*>(pNode);
				if (pWalkRun)
				{
					pWalkRun->SetVelocity(fSpeed);
				}
			}
		}
		m_fSpeed = fSpeed;
	}
#endif
}

void ApxSkinModelActionController::SetMoveYaw(float fOffForwardDeg)
{
#ifdef _ANGELICA3
	if (m_pAnimBlend1 == NULL || m_pAnimBlend2 == NULL)
		return;

	const float fBlendTime = 0.4f;

	A3DAnimNode* pBlend1Node1 = NULL, *pBlend1Node2 = NULL;//AnimBlend1
	A3DAnimNode* pBlend2Node1 = NULL, *pBlend2Node2 = NULL;//AnimBlend2
	int index1, index2;


	if (fOffForwardDeg >= 0 && fOffForwardDeg < 90)
	{
		pBlend1Node1 = m_pAnimBlend1->GetChild(4).mAnimNode;
		pBlend1Node2 = m_pAnimBlend1->GetChild(0).mAnimNode;

		pBlend2Node1 = m_pAnimBlend2->GetChild(4).mAnimNode;
		pBlend2Node2 = m_pAnimBlend2->GetChild(0).mAnimNode;

		index1 = 4; index2 = 0;
	}
	else if (fOffForwardDeg >= 90 && fOffForwardDeg <= 180)
	{
		pBlend1Node1 = m_pAnimBlend1->GetChild(5).mAnimNode;
		pBlend1Node2 = m_pAnimBlend1->GetChild(3).mAnimNode;

		pBlend2Node1 = m_pAnimBlend2->GetChild(5).mAnimNode;
		pBlend2Node2 = m_pAnimBlend2->GetChild(3).mAnimNode;

		index1 = 5; index2 = 3;
	}
	else if (fOffForwardDeg >= -90 && fOffForwardDeg < 0)
	{
		pBlend1Node1 = m_pAnimBlend1->GetChild(4).mAnimNode;
		pBlend1Node2 = m_pAnimBlend1->GetChild(2).mAnimNode;

		pBlend2Node1 = m_pAnimBlend2->GetChild(4).mAnimNode;
		pBlend2Node2 = m_pAnimBlend2->GetChild(2).mAnimNode;

		index1 = 4; index2 = 2;
	}
	else if (fOffForwardDeg >= -180 && fOffForwardDeg < -90)
	{
		pBlend1Node1 = m_pAnimBlend1->GetChild(5).mAnimNode;
		pBlend1Node2 = m_pAnimBlend1->GetChild(1).mAnimNode;

		pBlend2Node1 = m_pAnimBlend2->GetChild(5).mAnimNode;
		pBlend2Node2 = m_pAnimBlend2->GetChild(1).mAnimNode;

		index1 = 5; index2 = 1;
	}

	float fOffForwardRad = DEG2RAD(fOffForwardDeg + 90);
	float fVx = fabsf(m_fSpeed * cosf(fOffForwardRad));
	float fVz = fabsf(m_fSpeed * sinf(fOffForwardRad));

	float fVf, fTf, fVs, fTs;//前向速度fVf，前向Duration fTf，侧向速度fVs, 侧向Duration fTs
	A3DSklTrackSet* pRunTrackSet = ((A3DAnimNodeAction*)pBlend1Node1)->GetSMAction()->GetCoreData()->GetTrackSet();
	fVf = pRunTrackSet->GetMoveVelocity().Magnitude();
	fTf = float(((A3DAnimNodeAction*)pBlend1Node1)->GetSMAction()->GetCoreData()->GetActionTime());
	//fTf = pBlend1Node1->GetDuration();
	fTf *= 0.001f;

	pRunTrackSet = ((A3DAnimNodeAction*)pBlend1Node2)->GetSMAction()->GetCoreData()->GetTrackSet();
	fVs = pRunTrackSet->GetMoveVelocity().Magnitude();
	fTs = float(((A3DAnimNodeAction*)pBlend1Node2)->GetSMAction()->GetCoreData()->GetActionTime());
	//fTs = pBlend1Node2->GetDuration();
	fTs *= 0.001f;

	float fWeight = fVx * fVf * fTf / (fVx * fVf * fTf + fVz * fVs * fTs);
	float fT;
	if (fVx > fVz)
		fT = fVs * fTs * fWeight / fVx;
	else
		fT = fVf * fTf * (1 - fWeight) / fVz;

	for (int i = 0; i < m_pAnimBlend1->GetChildrenNum() - 1; i++)
	{
		A3DAnimNodeBlendWalkRun* pWalkRun = dynamic_cast<A3DAnimNodeBlendWalkRun*>(m_pAnimBlend1->GetChild(i).mAnimNode);
		if (pWalkRun)
			continue;

		A3DSklTrackSet* pRunTrackSet = ((A3DAnimNodeAction*)(m_pAnimBlend1->GetChild(i).mAnimNode))->GetSMAction()->GetCoreData()->GetTrackSet();
		float fDur = float(((A3DAnimNodeAction*)(m_pAnimBlend1->GetChild(i).mAnimNode))->GetSMAction()->GetCoreData()->GetActionTime());
		fDur *= 0.001f;
		m_pAnimBlend1->GetChild(i).mAnimNode->SetPlaySpeed(fDur / fT);

		pRunTrackSet = ((A3DAnimNodeAction*)(m_pAnimBlend2->GetChild(i).mAnimNode))->GetSMAction()->GetCoreData()->GetTrackSet();
		fDur = float(((A3DAnimNodeAction*)(m_pAnimBlend2->GetChild(i).mAnimNode))->GetSMAction()->GetCoreData()->GetActionTime());
		fDur *= 0.001f;
		m_pAnimBlend2->GetChild(i).mAnimNode->SetPlaySpeed(fDur / fT);
	}

	for (int i = 0; i < m_pAnimBlend1->GetChildrenNum(); i++)
	{
		if (i != index1 && i != index2)
		{
			m_apxWeight1[i].SetTargetWeight(0, fBlendTime);
			m_apxWeight2[i].SetTargetWeight(0, fBlendTime);
		}
	}
	m_apxWeight1[index2].SetTargetWeight(fWeight, fBlendTime);
	m_apxWeight1[index1].SetTargetWeight(1 - fWeight, fBlendTime);
	m_apxWeight2[index2].SetTargetWeight(fWeight, fBlendTime);
	m_apxWeight2[index1].SetTargetWeight(1 - fWeight, fBlendTime);
#endif
}


void ApxSkinModelActionController::SetUseWalkRun(bool bUseWalkRun) 
{ 
#ifdef _ANGELICA3
	const float fBlendTime = 0.4f;
	m_bUseWalkRun = bUseWalkRun;
	if (m_pAnimBlend1)
	{
		for (int i = 0; i < m_pAnimBlend1->GetChildrenNum() - 1; i++)
		{
			m_apxWeight1[i].SetTargetWeight(0, fBlendTime);
			m_apxWeight2[i].SetTargetWeight(0, fBlendTime);
		}
		m_apxWeight1[6].SetTargetWeight(1, fBlendTime);
		m_apxWeight2[6].SetTargetWeight(1, fBlendTime);
	}
#endif	
}