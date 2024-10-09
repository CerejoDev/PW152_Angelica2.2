
#include "stdafx.h"

#include <A3DIKSolver.h>
#include <A3DIKSolver2Joints.h>
#include <A3DIKGoal.h>
#include <A3DSkinModel.h>
#include "IKHandTarget.h"
#include "Carryable.h"
#include "Pushable.h"
#include "ECBodyControl.h"
#include "UpperBodyControl.h"

#ifdef _ANGELICA3
	#include <A3DAnimNode.h>
	#include <A3DIKGoal.h>
	#include <A3DIKTrigger.h>
	#include <A3DIKSolver2Joints.h>
#endif

#define IS_KEY_PRESSED(x) (GetAsyncKeyState(x) & 0x8000)

//----------------------------------------------
// APhysXCC Push notification...

static A3DSkeleton* s_pSk = NULL;

void RenderSk(A3DWireCollector* pWC, A3DBone* pParent, A3DBone* pSelf)
{
	if (pSelf == NULL)
		return;

	if (pParent)
	{
		A3DVECTOR3 vParent = pParent->GetAbsoluteTM().GetTransPart();
		A3DVECTOR3 vSelf = pSelf->GetAbsoluteTM().GetTransPart();

		pWC->Add3DLine(vParent, vSelf, 0xffffff00);
	}


	A3DMATRIX4 mat;
	mat = pSelf->GetAbsoluteTM();
	A3DOBB obb;
	obb.Center = A3DVECTOR3(0, 0, 0);
	obb.Extents = A3DVECTOR3(0.01f, 0.01f, 0.01f);
	obb.XAxis = mat.GetRow(0);
	obb.YAxis = mat.GetRow(1);
	obb.ZAxis = mat.GetRow(2);
	pWC->AddOBB(obb, 0xff008000, &mat);

	for (int i = 0; i < pSelf->GetChildNum(); i++)
	{
		RenderSk(pWC, pSelf, pSelf->GetChildPtr(i));
	}
}

void ApxActorBase::RenderTest(A3DWireCollector* pWC)
{
	//pWC->Add3DLine(smatFoot.GetTransPart(), smatFoot.GetTransPart() + smatFoot.GetRow(0), 0xffff0000);
	//pWC->Add3DLine(smatFoot.GetTransPart(), smatFoot.GetTransPart() + smatFoot.GetRow(1), 0xff00ff00);
	//pWC->Add3DLine(smatFoot.GetTransPart(), smatFoot.GetTransPart() + smatFoot.GetRow(2), 0xff0000ff);

	//pWC->Add3DLine(smatHead.GetTransPart(), smatHead.GetTransPart() + smatHead.GetRow(0), 0xffff0000);
	//pWC->Add3DLine(smatHead.GetTransPart(), smatHead.GetTransPart() + smatHead.GetRow(1), 0xff00ff00);
	//pWC->Add3DLine(smatHead.GetTransPart(), smatHead.GetTransPart() + smatHead.GetRow(2), 0xff0000ff);

	//pWC->Add3DLine(smatHand.GetTransPart(), smatHand.GetTransPart() + smatHand.GetRow(0), 0xffff0000);
	//pWC->Add3DLine(smatHand.GetTransPart(), smatHand.GetTransPart() + smatHand.GetRow(1), 0xff00ff00);
	//pWC->Add3DLine(smatHand.GetTransPart(), smatHand.GetTransPart() + smatHand.GetRow(2), 0xff0000ff);

	//render skeleton for debug
	if (s_pSk)
	{
		A3DMATRIX4 mat;

		for (int i = 0; i < s_pSk->GetRootBoneNum(); i++)
		{
			A3DBone* pBone = s_pSk->GetBone(s_pSk->GetRootBone(i));
			mat = pBone->GetAbsoluteTM();
			A3DOBB obb;
			obb.Center = A3DVECTOR3(0, 0, 0);
			obb.Extents = A3DVECTOR3(0.01f, 0.01f, 0.01f);
			obb.XAxis = mat.GetRow(0);
			obb.YAxis = mat.GetRow(1);
			obb.ZAxis = mat.GetRow(2);
			pWC->AddOBB(obb, 0xff008000, &mat);
			
			RenderSk(pWC, NULL, pBone);
		}
	}
}



class ApxAPhysXCCHitReport: public APhysXCCHitReport
{
public:
	
	ApxAPhysXCCHitReport() 
	{
		gPhysXCCHitReport->Register(this);
	}
	virtual ~ApxAPhysXCCHitReport()
	{
		gPhysXCCHitReport->UnRegister(this);
	}

	virtual void onShapeHit(const APhysXCCShapeHit& hit)
	{
		// to notify push action...
		if(hit.pushing)
		{
			ApxActorGroundMoveController* GMController = (ApxActorGroundMoveController*)hit.controller->userData;
			if(GMController)
			{
				APhysXRigidBodyObject* pRBObj = (APhysXRigidBodyObject*)(hit.object);
				IPhysXObjBase* pPhysx = NULL;
				if (pRBObj->GetNxActorNum())
				{
					pPhysx = ObjManager::GetInstance()->GetPhysXObject(*pRBObj->GetNxActor(0)); 
				}

				GMController->NotifyPushObj(pPhysx);
			}
		}
	}
};

static ApxAPhysXCCHitReport s_apxAPhysXCCHitReport;

//----------------------------------------------
// implementation of IActorGroundMoveController...

void ApxActorGroundMoveController::PostTickMove(float dt)
{
	UpdateMoveState();

	if(IsInNormalState())
	{
		if(!IsInAir() && m_bPreMoveInAir)
			Land();

		if(m_vPreMoveVelocity.y > 0.0f && GetVelocity().y < 0.0f)
			ReachApex();

		//----------------------------------------------
		// compute the pre-land time and notify it...
		// the quadratic equation for t is as following:
		// 0.5 * g * t^2 - v * t - h =0;

		float h = GetDistToGround();
		if(h > 0.0f)
		{
			float v = GetVelocity().y;
			float g = GetGravity();
			
			float t = v + NxMath::sqrt(v*v + 2*h*g);
			t /= g;

			PreLandNotify(t);
		}
	}
}

void ApxActorGroundMoveController::UpdateMoveState()
{
	if(IsInAir()) 
	{
		if (!IsInSwing())
			m_uGroundMoveState = ApxActor_GroundMoveState_InAir;
	}
	else
	{
		NxVec3 vel = GetVelocity();
		if(!NxMath::equals(vel.x, 0.0f, 0.01f) || !NxMath::equals(vel.z, 0.0f, 0.01f))
		{
			m_uGroundMoveState = ApxActor_GroundMoveState_Move;
			
			vel.y = 0.0f;
			float fVel = vel.magnitude();
			a_ClampFloor(fVel, 1.0f);
			m_pActorBase->SetBlendWalkRunSpeed(fVel, 0.5f);
		}
		else
		{
			if (!IsInPush())
				m_uGroundMoveState = ApxActor_GroundMoveState_StandInPlace;
		}
	}

	if (m_uGroundMoveState == ApxActor_GroundMoveState_StandInPlace || m_uGroundMoveState == ApxActor_GroundMoveState_Move	)
		m_pActorBase->EnableFootIK(true, true);
	else
	{
		m_pActorBase->EnableFootIK(false, false);
		m_pActorBase->m_fOffSetY = 0;
	}
	
	OnUpdateMoveState();
}

void ApxActorGroundMoveController::OnStartJump(){ if(m_pActorBase) m_pActorBase->OnStartJump(); }
void ApxActorGroundMoveController::OnLand(){if(m_pActorBase) m_pActorBase->OnLand();}
void ApxActorGroundMoveController::OnReachApex(){if(m_pActorBase) m_pActorBase->OnReachApex();}

void ApxActorGroundMoveController::OnStartSwing(){if(m_pActorBase) m_pActorBase->OnStartSwing();}
void ApxActorGroundMoveController::OnEndSwing(){if(m_pActorBase) m_pActorBase->OnEndSwing();}

void ApxActorGroundMoveController::OnPushObj(IPhysXObjBase* pObj) {if(m_pActorBase) m_pActorBase->OnPushObj(pObj);}
void ApxActorGroundMoveController::PreLandNotify(float fTimeBeforeLand) {if(m_pActorBase) m_pActorBase->PreLandNotify(fTimeBeforeLand);}
void ApxActorGroundMoveController::OnUpdateMoveState() {if(m_pActorBase) m_pActorBase->OnUpdateMoveState();}

void ApxAttackActionInfo::UpdateDamageSource()
{
	mApxDamage.mSourceFlags = 0;
	if (0 == mApxAttack.mpTarget)
		return;

	A3DMATRIX4 matPose(A3DMATRIX4::IDENTITY);
	mApxAttack.mpTarget->GetGPose(matPose);
	NxMat34 matNxPose;
	APhysXConverter::A2N_Matrix44(matPose, matNxPose);

	NxMat34 mat;
	matNxPose.getInverse(mat);
	NxVec3 localHitDir;
	mat.M.multiply(mApxDamage.mHitDir, localHitDir);

	NxVec3 front(0, 0, 1);
	float fVal = localHitDir.dot(front);
	if (0 > fVal)
		mApxDamage.mSourceFlags = APX_ADS_FRONT;
	else
		mApxDamage.mSourceFlags = APX_ADS_REAR;

	if ((fVal > 0.9f) || (fVal < -0.9f))
		mApxDamage.mSourceFlags |= APX_ADS_MIDDLE;
	else 
	{
		NxVec3 left(-1, 0, 0);
		float fValL = localHitDir.dot(left);
		if (fValL < 0)
			mApxDamage.mSourceFlags |= APX_ADS_RIGHT;
		else
			mApxDamage.mSourceFlags |= APX_ADS_LEFT;
	}
}
//----------------------------------------------
// implementation of ApxWeightBlender...

void ApxWeightBlender::SetTargetWeight(float fTargetWeight, float fBlendTime)
{
	if(fBlendTime <= 1e-4f)
	{
		// just ignore the blend time...
		m_fCurWeight = fTargetWeight;
		m_fWeightDeltaFactor = 0.0f;
		m_fBlendTime = 0.0f;
	}
	else
	{
		m_fBlendTime = fBlendTime;
		m_fWeightDeltaFactor = (fTargetWeight - m_fCurWeight) / fBlendTime;

	}
}

void ApxWeightBlender::Tick(float dt)
{
	if(!NxMath::equals(m_fWeightDeltaFactor, 0.0f, 1e-5f))
	{
		m_fCurWeight += dt * m_fWeightDeltaFactor;
		m_fBlendTime -= dt;
		if(m_fBlendTime<0.0f)
		{
			m_fCurWeight += m_fBlendTime * m_fWeightDeltaFactor;
			m_fWeightDeltaFactor = 0.0f;
		}
	}
}


//----------------------------------------------
// implementation of ApxActionController...

void ApxActionController::ReleaseActionQueue()
{
	while(!m_arrActionQueue.IsEmpty())
	{
		ApxActionInfo* pAction = NULL;
		m_arrActionQueue.PopFront(pAction);
		if(pAction) delete pAction;
	}
}

void ApxActionController::ReleaseAllActions()
{
	if(m_pCurActionInfo)
	{
		delete m_pCurActionInfo;
		m_pCurActionInfo = NULL;
	}

	ReleaseActionQueue();
}

void ApxActionController::Release()
{
	ReleaseAllActions();
}

bool ApxActionController::StartPlayCurAction()
{
	bool bRtnRealPlay = false;
	if(m_pCurActionInfo)
	{
		ApxActionAnimInfo& animInfo = m_pCurActionInfo->mAnimInfo;
		ApxComboAttackActionInfo* pComboAttack = m_pCurActionInfo->IsComboAttackActionInfo();
		if(pComboAttack)
		{
			if (pComboAttack->m_uComboActionID == 1 )
			{
				animInfo = pComboAttack->mSeqAnim[0];
				pComboAttack->mApxAttack.mDamageDelayStartSec = pComboAttack->mSeqActDamDelayStartSec[0];
				pComboAttack->mApxAttack.mDamageDurationSec = pComboAttack->mSeqActDamDurationSec[0];
			}
			else if(pComboAttack->m_uComboActionID == 2)
			{
				animInfo = pComboAttack->mSeqAnim[1];
				pComboAttack->mApxAttack.mDamageDelayStartSec = pComboAttack->mSeqActDamDelayStartSec[1];
				pComboAttack->mApxAttack.mDamageDurationSec = pComboAttack->mSeqActDamDurationSec[1];
			}
		}

		if(!animInfo.mActionName.IsEmpty())
		{
			
			if(PlayAnim(animInfo))
			{
				OnActionStart(m_pCurActionInfo);
				m_fTimeAfterPlay = 0.0f;
				m_bNotifyActionEnd = false;
				bRtnRealPlay = true;
			}
			else
			{
				if(m_bNotifyActionEnd)
				{
					// release current action
					delete m_pCurActionInfo;
					m_pCurActionInfo = NULL;
				}
			}

		}
		else
		{
			delete m_pCurActionInfo;
			m_pCurActionInfo = NULL;
		}
	}
	return bRtnRealPlay;
}

bool ApxActionController::CanActionPlay(const ApxActionInfo& apxActionInfo)
{
	bool bCanPlay = false;
	if(m_fTimeAfterPlay >= m_pCurActionInfo->mIgnoreActionTime)
	{
		if(m_pCurActionInfo->mAnimInfo.mActionName == apxActionInfo.mAnimInfo.mActionName)
		{
			if(m_fTimeAfterPlay >= m_pCurActionInfo->mCoolDownTime)
				bCanPlay = true;
		}
		else
		{
			bCanPlay = true;
		}

	}

	return bCanPlay;
}

static bool IsSameComboAttack(const ApxActionInfo* pAction1, const ApxActionInfo* pAction2)
{
	const ApxComboAttackActionInfo* pComboAttack1 = pAction1->IsComboAttackActionInfo();
	const ApxComboAttackActionInfo* pComboAttack2 = pAction2->IsComboAttackActionInfo();

	if (pComboAttack1 && pComboAttack2 && pAction1->mAnimInfo.mActionName == pAction2->mAnimInfo.mActionName)
	{
		if (pComboAttack1->mSeqAnim[0].mActionName == pComboAttack2->mSeqAnim[0].mActionName)
		{
			if (!pComboAttack1->mSeqAnim[0].mActionName.IsEmpty())
			{
				if (pComboAttack1->mSeqAnim[1].mActionName == pComboAttack2->mSeqAnim[1].mActionName)
					return true;
			}
		}
	}

	return false;
}

bool ApxActionController::PlayAction(const ApxActionInfo& apxActionInfo)
{
	// currently, we just ignore the action with empty animation.
	if(apxActionInfo.mAnimInfo.mActionName.IsEmpty())
		return false;

	if(apxActionInfo.mPlayMode == ApxActionPlayMode_InstantPlay)
	{
		ReleaseAllActions();
		m_pCurActionInfo = apxActionInfo.Clone();
		return StartPlayCurAction();
	}

	if(apxActionInfo.mPlayMode == ApxActionPlayMode_CheckPlay)
	{
		if(!IsPlayingAction())
		{
			m_pCurActionInfo = apxActionInfo.Clone();
			return StartPlayCurAction();
		}
		return false;
	}

	if(apxActionInfo.mPlayMode == ApxActionPlayMode_NormalPlay)
	{
		if(!IsPlayingAction())
		{
			m_pCurActionInfo = apxActionInfo.Clone();
			return StartPlayCurAction();
		}
		else if(m_arrActionQueue.GetSize() == 0)		// if there are any actions in the waiting queue, just ignore normal play action.
		{
			APHYSX_ASSERT(m_pCurActionInfo);
			if(CanActionPlay(apxActionInfo))
			{
				m_arrActionQueue.PushBack(apxActionInfo.Clone());
				return true;
			}
		} // if there are any action in the waiting queue, just ignore normal play action.
		return false;
	}

	if (apxActionInfo.mPlayMode == ApxActionPlayMode_ComboPlay)
	{
		// handle the case of combo play...
		if(!IsPlayingAction())
		{
			m_pCurActionInfo = apxActionInfo.Clone();
			return StartPlayCurAction();
		}
		else if (m_arrActionQueue.GetSize() == 0)
		{
			APHYSX_ASSERT(m_pCurActionInfo);
			if(IsSameComboAttack(m_pCurActionInfo, &apxActionInfo))
			{
				// the action is the combo action...
				const ApxComboAttackActionInfo* pComboAttack = m_pCurActionInfo->IsComboAttackActionInfo();
				if(m_fTimeAfterPlay < pComboAttack->mComboPlayTime * m_fPlaySpeed)
				{
					ApxActionInfo* pAction = apxActionInfo.Clone();
					pComboAttack = pAction->IsComboAttackActionInfo();
					APHYSX_ASSERT(pComboAttack);
					pComboAttack->SetComboActionID(1);
					m_arrActionQueue.PushBack(pAction);
					return true;
				}
			}
			else
			{
				if(CanActionPlay(apxActionInfo))
				{
					m_arrActionQueue.PushBack(apxActionInfo.Clone());
					return true;
				}
			}
		}
		else if (m_arrActionQueue.GetSize() == 1)
		{
			APHYSX_ASSERT(m_pCurActionInfo);
			if(IsSameComboAttack(m_pCurActionInfo, &apxActionInfo) &&
				IsSameComboAttack(m_arrActionQueue.GetFront(), &apxActionInfo) )
			{
				// the action is the combo action...
				const ApxComboAttackActionInfo* pInputComboAttack = apxActionInfo.IsComboAttackActionInfo();
				if (!pInputComboAttack->mSeqAnim[1].mActionName.IsEmpty())
				{
					const ApxComboAttackActionInfo* pComboAttack = m_pCurActionInfo->IsComboAttackActionInfo();
					if (m_fTimeAfterPlay < pComboAttack->mComboPlayTime * 2 * m_fPlaySpeed)
					{
						ApxActionInfo* pAction = pInputComboAttack->Clone();
						pComboAttack = pAction->IsComboAttackActionInfo();
						APHYSX_ASSERT(pComboAttack);
						pComboAttack->SetComboActionID(2);
						m_arrActionQueue.PushBack(pAction);
						return true;
					}
				}
			}
		}
	}
	return false;
}


void ApxActionController::Tick(float dt)
{
	dt *= m_fPlaySpeed;

	PreTick(dt);

	// we tick the animation...
	TickAnim(dt);

	// at last, we check whether the action is finished...
	if(IsPlayingAction())
	{
		// advance the time counter...
		m_fTimeAfterPlay += dt;

		float fActionSpan = GetCurActionSpan();

		if (!(fActionSpan < 0.0f))
		{
			PreActionEnd(m_pCurActionInfo, fActionSpan - m_fTimeAfterPlay);
		}

		//------------------------------------------------
		// revise by wenfeng, Aug. 17, 2011
		// try to avoid waiting the action to be finished since if the action is stopped,
		// there will be a temp interval that channel's action will be NULL and the full 
		//	weight 1.0 has no effect...

#if 0
		// check if the current action is done?
		if(IsAnimStopped(m_pCurActionInfo->mAnimInfo))
		{
			OnActionEnd(m_pCurActionInfo);
			// clear m_pCurActionInfo...
			delete m_pCurActionInfo;
			m_pCurActionInfo = NULL;

			if(!m_arrActionQueue.IsEmpty())
			{
				m_arrActionQueue.PopFront(m_pCurActionInfo);

				StartPlayCurAction();

			}

		}
#endif


	}
}

void ApxActionController::OnActionStart(ApxActionInfo* pActionInfo)
{ 
	// set the action's playback speed...
	SetPlaySpeed(pActionInfo->mPlaySpeed);

	if(m_pActorBase) 
		m_pActorBase->OnActionStart(m_apxActionLayer, pActionInfo); 
}

void ApxActionController::OnActionEnd(ApxActionInfo* pActionInfo) 
{ 
	if(m_pActorBase) 
		m_pActorBase->OnActionEnd(m_apxActionLayer, pActionInfo); 
}

void ApxActionController::PreActionEnd(ApxActionInfo* pActionInfo, float fLeftTime) 
{
	// firstly, call m_pActorBase's callback...
	if(m_pActorBase) m_pActorBase->PreActionEnd(m_apxActionLayer, pActionInfo, fLeftTime);

	//------------------------------------------------
	// revise by wenfeng, Aug. 17, 2011
	// try to avoid waiting the action to be finished, 
	// so we end the action a little time in advance


	// then, try to figure out whether the current action will be finished...
	float fBlendTime = pActionInfo->mAnimInfo.mBlendInTime;
	// try to stop current action...
	if(fBlendTime > fLeftTime && !m_bNotifyActionEnd)
	{
		OnActionEnd(m_pCurActionInfo);
		// clear m_pCurActionInfo...
		delete m_pCurActionInfo;
		m_pCurActionInfo = NULL;

		// note: we only notify the action end once, and
		// there is no need to notify the action end again
		m_bNotifyActionEnd = true;


		// check if there is any action in the waiting list...
		if(!m_arrActionQueue.IsEmpty())
		{
			m_arrActionQueue.PopFront(m_pCurActionInfo);
			StartPlayCurAction();
		}
	}
}
ApxWeaponInfo::ApxWeaponInfo()
{
	bUsingXAJHWay = false;
	pWeaponObj = 0;
	smsHostSocketType = ApxSM_SocketType_Bone;
	smsWPNSocketType = ApxSM_SocketType_Bone;
}

bool ApxWeaponInfo::operator!=(const ApxWeaponInfo& awi) const
{
	if (bUsingXAJHWay != awi.bUsingXAJHWay)
		return true;
	if (strWeaponPathName != awi.strWeaponPathName)
		return true;
	if (0 == pWeaponObj)
	{
		if (0 != awi.pWeaponObj)
			return true;
	}
	else
	{
		if (0 == awi.pWeaponObj)
			return true;
	}

	if (strHostSocketName != awi.strHostSocketName)
		return true;
	if (strWPNSocketName != awi.strWPNSocketName)
		return true;

	if (bUsingXAJHWay)
	{
		if (strWPNECMNickName != awi.strWPNECMNickName)
			return true;
		if (strWPNAttacher != awi.strWPNAttacher)
			return true;
	}
	else
	{
		if (smsHostSocketType != awi.smsHostSocketType)
			return true;
		if (smsWPNSocketType != awi.smsWPNSocketType)
			return true;
	}
	return false;
}

//----------------------------------------------
// implementation of ApxActorBase...
bool ApxActorBase::IsMoving() const
{
	if (m_pGroundMoveController->CheckFlag(ApxActorMoveController::ApxActor_MoveFlag_Driven))
		return true;
	if (m_vGroundTargetDir.equals(NxVec3(0.0f), APHYSX_FLOAT_EPSILON))
		return false;
	return true;
}

void ApxActorBase::SetHeadDir(const NxVec3& vHeadDir, bool bEnforce)
{
	if (IsMoving())
		return;

	float dotVal = vHeadDir.dot(m_vHeadFaceDir);
	if (1.0f > dotVal)
		ApplyMoveDir(vHeadDir, bEnforce);
}

const NxVec3& ApxActorBase::GetHeadUp() const
{
	if (m_pGroundMoveController)
		return m_pGroundMoveController->GetHeadUp();
	
	static NxVec3 up(0, 1, 0);
	return up;
}

bool ApxActorBase::PlayAction(ApxActionLayer layer, const ApxActionInfo& apxActionInfo)
{
	// fist, we check whether this action can be played?
	for(int i=0; i<ApxActionLayer_Num; i++)
	{
		if(i!= layer && (m_ActionControllerPtrs[i]->GetCurrentActionORMask() & ApxActionORMask_FullBody) ==0)
			return false;
	}

	// then, just call that layer's controller's PlayAction method.
	return m_ActionControllerPtrs[layer]->PlayAction(apxActionInfo);
}

void ApxActorBase::SwitchToRagDoll(const ApxDamageInfo& apxDamageInfo)
{
	CDynProperty* pProp = m_pObjDyn->GetProperties();
	if (pProp->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
		return;

	CAPhysXCCMgr& ccm = pProp->GetCCMgr();
	m_beforRagdollState.ccType = ccm.GetCCType();
	m_beforRagdollState.ccFlags = ccm.GetCurrentFlags();
	m_beforRagdollState.dm = pProp->GetDrivenMode();

	for(int i=0; i<ApxActionLayer_Num; i++)
		m_ActionControllerPtrs[i]->ReleaseAllActions();

	bool bIsLock = pProp->GetFlags().ReadFlag(OBF_DRIVENMODE_ISLOCKED);
	if (bIsLock)
		pProp->GetFlags().ClearFlag(OBF_DRIVENMODE_ISLOCKED);
	pProp->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX);
	if (bIsLock)
		pProp->GetFlags().RaiseFlag(OBF_DRIVENMODE_ISLOCKED);

	ccm.SwtichCC(CAPhysXCCMgr::CC_TOTAL_EMPTY);
	m_pObjDyn->OnAttackedHitEvent(apxDamageInfo);

	m_ActorState.TranState(ActorState::Ragdoll);
}

void ApxActorBase::CloseRagDoll()
{
	CDynProperty* pProp = m_pObjDyn->GetProperties();
	if (pProp->QueryDrivenMode(m_beforRagdollState.dm))
		return;

	bool bIsLock = pProp->GetFlags().ReadFlag(OBF_DRIVENMODE_ISLOCKED);
	if (bIsLock)
		pProp->GetFlags().ClearFlag(OBF_DRIVENMODE_ISLOCKED);
	
	//-------------------------------------------------------------
	// Note: wenfeng, Nov. 18, 2011
	//		Delay setting the drive mode to DRIVEN_BY_PART_PHYSX, otherwise,
	// some snap effect of bones driven by physics will be weird.

	if(m_beforRagdollState.dm == IPropPhysXObjBase::DRIVEN_BY_PART_PHYSX)
	{
		pProp->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_ANIMATION);
		m_iDelaySetPartPhysicsDMCounter = 0;
	}
	else
	{
		pProp->SetDrivenMode(m_beforRagdollState.dm);
	}
	
	if (bIsLock)
		pProp->GetFlags().RaiseFlag(OBF_DRIVENMODE_ISLOCKED);

	if (!m_ActorState.TranState(ActorState::Normal, 2.0f))
	{
		CAPhysXCCMgr& ccm = pProp->GetCCMgr();
		ccm.SwtichCC(m_beforRagdollState.ccType,true);
		ccm.SetCurrentFlags(m_beforRagdollState.ccFlags);
		// reset speed to affect subclass members
		float speed = m_pGroundMoveController->GetMoveSpeed();
		m_pGroundMoveController->SetMoveSpeed(speed);
		// reset dir to affect CC members
		NxVec3 dir = m_pGroundMoveController->GetHeadDir();
		m_pGroundMoveController->SetHeadDir(dir);

		AString strActName;
		A3DSkinModel* pSM = m_pObjDyn->GetSkinModel();
		if (0 != pSM)
		{
			if (0 != pSM->GetAction("起身"))
				strActName = "起身";
			else if (0 != pSM->GetAction("战斗站立"))
				strActName = "战斗站立";
		}

		ApxActionInfo info;
		info.mAnimInfo.mActionName = strActName;
		info.mAnimInfo.mActionORMask = ApxActionORMask_None;
		info.mAnimInfo.mBasicAction = true;
		info.mAnimInfo.mBlendWalkRun = false;
		info.mAnimInfo.mLooping = false;
		info.mAnimInfo.mRestart = true;
		info.mPlayMode = ApxActionPlayMode_InstantPlay;
		PlayAction(ApxActionLayer_Move, info);
	}
}

bool ApxActorBase::CheckPosValidate(const A3DVECTOR3& vFootPos) const
{
	CDynProperty* pProp = m_pObjDyn->GetProperties();
	CAPhysXCCMgr& ccm = pProp->GetCCMgr();

	return ccm.CheckVolumeValid(APhysXConverter::A2N_Vector3(vFootPos), m_pGroundMoveController->GetExtents(), APhysXCCBase::APX_VT_CAPSULE);
}

void ApxActorBase::ResetCCBeforeRagdoll()
{
	CDynProperty* pProp = m_pObjDyn->GetProperties();
	CAPhysXCCMgr& ccm = pProp->GetCCMgr();
	ccm.SwtichCC(m_beforRagdollState.ccType,true);
	ccm.SetCurrentFlags(m_beforRagdollState.ccFlags);
	// reset speed to affect subclass members
	float speed = m_pGroundMoveController->GetMoveSpeed();
	m_pGroundMoveController->SetMoveSpeed(speed);
	// reset dir to affect CC members
	NxVec3 dir = m_pGroundMoveController->GetHeadDir();
	m_pGroundMoveController->SetHeadDir(dir);
	m_vHeadFaceDir = dir;
}


inline static bool OccupyUpperBody(APhysXU32 uMask)
{
	return ((uMask & ApxActionORMask_UpperBody) == 0);
}
inline static bool OccupyLowerBody(APhysXU32 uMask)
{
	return ((uMask & ApxActionORMask_LowerBody) == 0);
}
inline static bool OccupyFullBody(APhysXU32 uMask)
{
	return (OccupyUpperBody(uMask) && OccupyLowerBody(uMask));
}
inline static bool OccupyNoBody(APhysXU32 uMask)
{
	return !(OccupyUpperBody(uMask) || OccupyLowerBody(uMask));
}

void ApxActorBase::OnActionStart(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo)
{
	APhysXU32 myORMask = pActionInfo->mAnimInfo.mActionORMask;

	APhysXU32 MLORMask = m_ActionControllerPtrs[ApxActionLayer_Move]->GetCurrentActionORMask();
	APhysXU32 ALORMask = m_ActionControllerPtrs[ApxActionLayer_Active]->GetCurrentActionORMask();
	APhysXU32 PLORMask = m_ActionControllerPtrs[ApxActionLayer_Passive]->GetCurrentActionORMask();

	ApxActionController* pCurActionController = m_ActionControllerPtrs[apxActionLayer];
	float fBlendTime = pActionInfo->mAnimInfo.mBlendInTime;

	//------------------------------------------------------------------------------
	// Beware:	by wenfeng, Jul. 19, 2011
	//			the following code snippet to handle the action layer's weight and 
	//		channel is really awesome and complicated. Try to find other ways to 
	//		full-check and revise them for clear and easy to understand.
	//

	// to check and decide which channel to be active and how the weight is.
	// we will handle the case according to specific layer
	if(apxActionLayer == ApxActionLayer_Move)
	{
		if(OccupyNoBody(ALORMask) && OccupyNoBody(PLORMask))
		{
			// other layers have no action, play this action in full-body mode
			pCurActionController->SetActiveChannel(ApxActionChannel_FullBody, fBlendTime);
			m_ActionControllerPtrs[ApxActionLayer_Active]->SetActiveChannel(ApxActionChannel_None, fBlendTime);
			m_ActionControllerPtrs[ApxActionLayer_Passive]->SetActiveChannel(ApxActionChannel_None, fBlendTime);
		}
		else
		{
			// other layers have some action
			if(OccupyFullBody(myORMask))
			{
				// just ignore this action since it is a full-body action
				pCurActionController->SetActiveChannel(ApxActionChannel_None,fBlendTime);
			}
			else if (OccupyUpperBody(myORMask))
			{
				// this action is upper-body action

				if(OccupyUpperBody(ALORMask) || OccupyUpperBody(PLORMask))
				{
					// upper-body has been occupied
					pCurActionController->SetActiveChannel(ApxActionChannel_None,fBlendTime);
				}
				else
				{
					// upper-body is empty for playing action
					pCurActionController->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
					
					m_ActionControllerPtrs[ApxActionLayer_Active]->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
					m_ActionControllerPtrs[ApxActionLayer_Passive]->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
				}
			}
			else if (OccupyLowerBody(myORMask))
			{
				// this action is lower-body action
				if(OccupyLowerBody(ALORMask) || OccupyLowerBody(PLORMask))
				{
					// lower-body has been occupied
					pCurActionController->SetActiveChannel(ApxActionChannel_None,fBlendTime);
				}
				else
				{
					// lower-body is empty for playing action
					pCurActionController->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
					
					m_ActionControllerPtrs[ApxActionLayer_Active]->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
					m_ActionControllerPtrs[ApxActionLayer_Passive]->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
				}
			}
			else if (OccupyNoBody(myORMask))
			{
				if(!OccupyLowerBody(ALORMask) && ! OccupyLowerBody(PLORMask))
				{
					pCurActionController->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
					
					m_ActionControllerPtrs[ApxActionLayer_Active]->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
					m_ActionControllerPtrs[ApxActionLayer_Passive]->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
				}
				else if(!OccupyUpperBody(ALORMask) && ! OccupyUpperBody(PLORMask))
				{
					pCurActionController->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
					
					m_ActionControllerPtrs[ApxActionLayer_Active]->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
					m_ActionControllerPtrs[ApxActionLayer_Passive]->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
				}
				else
				{
					pCurActionController->SetActiveChannel(ApxActionChannel_None,fBlendTime);
				}
			}
		}	// if(OccupyNoBody(ALORMask) && OccupyNoBody(PLORMask))
	} // if(apxActionLayer == ApxActionLayer_Move)
	else if (apxActionLayer == ApxActionLayer_Active)
	{
		if(OccupyNoBody(MLORMask) && OccupyNoBody(PLORMask))
		{
			// other layers have no action, play this action in full-body mode
			pCurActionController->SetActiveChannel(ApxActionChannel_FullBody, fBlendTime);
			
			m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_None, fBlendTime);
			m_ActionControllerPtrs[ApxActionLayer_Passive]->SetActiveChannel(ApxActionChannel_None, fBlendTime);
		}
		else
		{
			// other layers have some action
			if(OccupyFullBody(myORMask))
			{
				// just ignore this action since it is a full-body action
				pCurActionController->SetActiveChannel(ApxActionChannel_None,fBlendTime);
			}
			else if (OccupyUpperBody(myORMask))
			{
				// this action is upper-body action
				if(OccupyUpperBody(MLORMask) )
				{
					// upper-body has been occupied
					pCurActionController->SetActiveChannel(ApxActionChannel_None,fBlendTime);
				}
				else if (OccupyUpperBody(PLORMask))
				{
					pCurActionController->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
				}
				else
				{
					// upper-body is empty for playing action
					pCurActionController->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);

					m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
					m_ActionControllerPtrs[ApxActionLayer_Passive]->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
				}
			}
			else if (OccupyLowerBody(myORMask))
			{
				// this action is lower-body action
				if(OccupyLowerBody(MLORMask) )
				{
					// lower-body has been occupied
					pCurActionController->SetActiveChannel(ApxActionChannel_None,fBlendTime);
				}
				else if (OccupyLowerBody(PLORMask))
				{
					pCurActionController->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
				}
				else
				{
					// lower-body is empty for playing action
					pCurActionController->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
					
					m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
					m_ActionControllerPtrs[ApxActionLayer_Passive]->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
				}
			}
			else if (OccupyNoBody(myORMask))
			{
				// in active layer, we assume all actions at least occupy upper/lower body.
				APHYSX_ASSERT(0);
			}
		}
	} // if(apxActionLayer == ApxActionLayer_Active)
	else if (apxActionLayer == ApxActionLayer_Passive)
	{
		if(OccupyNoBody(MLORMask) && OccupyNoBody(ALORMask))
		{
			// other layers have no action, play this action in full-body mode
			pCurActionController->SetActiveChannel(ApxActionChannel_FullBody, fBlendTime);
			
			m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_None, fBlendTime);
			m_ActionControllerPtrs[ApxActionLayer_Active]->SetActiveChannel(ApxActionChannel_None, fBlendTime);
		}
		else
		{
			// other layers have some action
			if(OccupyFullBody(myORMask))
			{
				// force to play this action since the passive layer has high priority.
				pCurActionController->SetActiveChannel(ApxActionChannel_FullBody, fBlendTime);
				
				m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_None, fBlendTime);
				m_ActionControllerPtrs[ApxActionLayer_Active]->SetActiveChannel(ApxActionChannel_None, fBlendTime);
			}
			else if (OccupyUpperBody(myORMask))
			{
				// this action is upper-body action
				if(OccupyUpperBody(MLORMask) || OccupyUpperBody(ALORMask))
				{
					pCurActionController->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
				}
				else
				{
					// upper-body is empty for playing action
					pCurActionController->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
					
					m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
					m_ActionControllerPtrs[ApxActionLayer_Active]->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
				}
			}
			else if (OccupyLowerBody(myORMask))
			{
				// this action is lower-body action
				if(OccupyLowerBody(MLORMask) || OccupyLowerBody(ALORMask))
				{
					pCurActionController->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
				}
				else
				{
					// lower-body is empty for playing action
					pCurActionController->SetActiveChannel(ApxActionChannel_LowerBody, fBlendTime);
					
					m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
					m_ActionControllerPtrs[ApxActionLayer_Active]->SetActiveChannel(ApxActionChannel_UpperBody, fBlendTime);
				}
			}
			else if (OccupyNoBody(myORMask))
			{
				// in active layer, we assume all actions at least occupy upper/lower body.
				APHYSX_ASSERT(0);
			}
		}
	} // if (apxActionLayer == ApxActionLayer_Passive)

	ApxAttackActionInfo* pAttack = pActionInfo->IsAttackActionInfo();
	if (0 != pAttack)
	{
		GeneralAttackMgr* pGAM = IAssailable::GetGeneralAttackMgr();
		if (0 != pGAM)
			pGAM->OnPlayAttackBegin(*pAttack);
		GetWeaponPose(m_WeaponLastPose, &m_WeaponLastCapsule);
	}

	// call user's callback...
	if(m_pUserActionReport) 
	{
		m_pUserActionReport->OnActionStart(apxActionLayer, pActionInfo);
	}
#ifdef _ANGELICA21
	UpdateTransIKOffset();
#endif
}

void ApxActorBase::OnActionEnd(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo) 
{
#if 0
	ApxActionController* pCurActionController = m_ActionControllerPtrs[apxActionLayer];
	float fBlendTime = pActionInfo->mAnimInfo.mBlendInTime;
	fBlendTime = 1.0f;

	if(apxActionLayer == ApxActionLayer_Move)
	{
		// do nothing since the ApxActionLayer_Move layer always has succedent actions...

	}
	else if (apxActionLayer == ApxActionLayer_Active)
	{
		if(pCurActionController->GetWeight()>0.0f && !pCurActionController->HasSuccedentAction())
		{
			pCurActionController->SetTargetWeight(0.0f, fBlendTime);
			// pCurActionController->PrepareTransition(0.3f);

			m_ActionControllerPtrs[ApxActionLayer_Move]->SetTargetWeight(1.0f, fBlendTime);
			m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_FullBody);
		}
	}
	else if (apxActionLayer == ApxActionLayer_Passive)
	{
		if(pCurActionController->GetWeight()>0.0f && !pCurActionController->HasSuccedentAction())
		{
			pCurActionController->SetTargetWeight(0.0f, fBlendTime);

			if(!m_ActionControllerPtrs[ApxActionLayer_Move]->GetCurrentAction())
			{
				m_ActionControllerPtrs[ApxActionLayer_Move]->SetTargetWeight(1.0f, fBlendTime);
				m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_FullBody);
			}

		}
	}

#endif 

	m_bInCrossOverBleand = false;
	ApxAttackActionInfo* pAttack = pActionInfo->IsAttackActionInfo();
	if (0 != pAttack)
	{
		GeneralAttackMgr* pGAM = IAssailable::GetGeneralAttackMgr();
		if (0 != pGAM)
			pGAM->OnPlayAttackEnd(*pAttack);
	}

	// call user's callback...
	if(m_pUserActionReport) 
		m_pUserActionReport->OnActionEnd(apxActionLayer, pActionInfo);
}

void ApxActorBase::PreActionEnd(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo, float fLeftTime)
{
	ApxActionController* pCurActionController = m_ActionControllerPtrs[apxActionLayer];
	float fBlendTime = pActionInfo->mAnimInfo.mBlendInTime;
	
	// adjust the active channel...
	if(fBlendTime > fLeftTime && fLeftTime > 0.1f && !m_bInCrossOverBleand)
	{
		fBlendTime = fLeftTime;
		m_bInCrossOverBleand = true;

		if(apxActionLayer == ApxActionLayer_Move)
		{
			// do nothing since the ApxActionLayer_Move layer always has succedent actions...

		}
		else if (apxActionLayer == ApxActionLayer_Active)
		{
			if(pCurActionController->GetActiveChannel() != ApxActionChannel_None && !pCurActionController->HasSuccedentAction())
			{
				pCurActionController->SetActiveChannel(ApxActionChannel_None, fBlendTime);

				m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_FullBody, fBlendTime);
			}
		}
		else if (apxActionLayer == ApxActionLayer_Passive)
		{
			if(pCurActionController->GetActiveChannel() != ApxActionChannel_None && !pCurActionController->HasSuccedentAction())
			{
				pCurActionController->SetActiveChannel(ApxActionChannel_None, fBlendTime);

				// if(!m_ActionControllerPtrs[ApxActionLayer_Move]->GetCurrentAction())
				{
					m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_FullBody, fBlendTime);
				}

			}
		}
	}
}

void ApxActorBase::ApplyMoveDir(const NxVec3& vMoveDir, bool bEnforce)
{
	if (0 == m_pGroundMoveController)
		return;

	if (m_pGroundMoveController->IsInSwing())
	{
		m_vGroundTargetDir.zero();
		m_pGroundMoveController->SetMoveDir(vMoveDir);
		return;
	}

	NxVec3 vDir(vMoveDir);
	if(vDir.y != 0.0f)
	{
		vDir.y = 0.0;
		vDir.normalize();
	}
	m_pGroundMoveController->SetMoveDir(vDir);

	if (0 != m_pBodyCtrl)
	{
		if (m_bAutoTurn || bEnforce)
			UpdateRotateBody(vDir);
	}

	float dotVal = vDir.dot(m_vHeadFaceDir);
	if (1.0f > dotVal)
	{
		m_fTurnT = 0.0f;
		m_fTheta = NxMath::acos(dotVal);
	}
}

void ApxActorBase::UpdateRotateBody(const NxVec3& vDir)
{
	if (m_pBodyCtrl == NULL)
		return;

	float fYaw = glb_VectorToYaw(APhysXConverter::N2A_Vector3(vDir));
	if (m_TargetInfo.bHasTarget)
	{	
		A3DVECTOR3 vTargetDir = m_TargetInfo.vPos - APhysXConverter::N2A_Vector3(GetPos());
		vTargetDir.y = 0.0f;
		vTargetDir.Normalize();
		//if (fDist <= m_TargetInfo.fRange)
		{
			float fTargetDir = glb_VectorToYaw(vTargetDir);
			glb_ClampYaw(fTargetDir);
			float fDelta = (float)fabs(fTargetDir - fYaw);
			if (fDelta >= 180.0f)
				fDelta = 360.0f - fDelta;
			if (fDelta > (90.0f+1e-4f))
			{
				fYaw += 180.0f;
				glb_ClampYaw(fYaw);
				//m_bForwardAct = false;
			}
			m_pBodyCtrl->RotateHead(EC_DOYAW_NEAR, fTargetDir, fYaw, 3600.0f);	
			return;
		}

	}

	m_pBodyCtrl->RotateBody(EC_DOYAW_NEAR, fYaw, 360.0f);
}

NxVec3 ApxActorBase::GetInputMoveDir(UserInputFlag uifs, const NxVec3& camHDirNow)
{
	NxVec3 vDir(0.0f);
	NxVec3 camHDir(camHDirNow);
	if (m_pRec->IsReplaying())
		camHDir = APhysXConverter::A2N_Vector3(m_pRec->GetOriginalCamera()->GetDirH());

	ApxActorGroundMoveController* pAAGMC = GetGroundMoveController();
	if (pAAGMC->IsInSwing())
	{
		CDynProperty* pProp = m_pObjDyn->GetProperties();
		CAPhysXCCMgr& ccm = pProp->GetCCMgr();
		APhysXHostCCBase* pCC = ccm.GetAPhysXHostCCBase();
		NxVec3 g;
		pCC->GetNxScene().getGravity(g);
		g.normalize();

		if (UIF_MOVE_UP & uifs)
			vDir -= g;
		if (UIF_MOVE_DOWN & uifs)
			vDir += g;

		NxVec3 ccHDir = pCC->GetHorizonDir(); 
		NxVec3 up = pCC->GetUpDir();
		NxVec3 ccdir = pCC->GetDir();
		NxVec3 axis = up.cross(ccdir);
		float d = camHDir.dot(axis);
		if (0 > d)
		{
			if (UIF_MOVE_LEFT & uifs)
				vDir -= ccHDir;
			if (UIF_MOVE_RIGHT & uifs)
				vDir += ccHDir;
		}
		else if (0 < d)
		{
			if (UIF_MOVE_LEFT & uifs)
				vDir += ccHDir;
			if (UIF_MOVE_RIGHT & uifs)
				vDir -= ccHDir;
		}
	}
	else
	{
		static const NxVec3 g_vUp(0, 1, 0);
		NxVec3 vRight;
		vRight.cross(g_vUp, camHDir);
		if (UIF_MOVE_RIGHT & uifs)
			vDir += vRight;
		if (UIF_MOVE_LEFT & uifs)
			vDir -= vRight;
		if (UIF_MOVE_UP & uifs)
			vDir += camHDir;
		if (UIF_MOVE_DOWN & uifs)
			vDir -= camHDir;
	}
	vDir.normalize();
	return vDir;
}

void ApxActorBase::SetGroundMoveDir(const NxVec3& vMoveDir) 
{ 
	if(m_pGroundMoveController)
	{
		m_pGroundMoveController->RaiseFlag(ApxActorMoveController::ApxActor_MoveFlag_Driven);
		m_bSetCurPosToTargetPos = true;
	}

	NxVec3 vDir(vMoveDir);
	if (!m_vGroundTargetDir.isZero())
	{
		if (vMoveDir.isZero())
			vDir = m_vGroundTargetDir;
		else
			m_vGroundTargetDir.zero();
	}
	ApplyMoveDir(vDir);
}

NxVec3 ApxActorBase::GetGroundMoveDir() const
{
	return m_pGroundMoveController->GetMoveDir();
}

void ApxActorBase::SetGroundTargetPos(const NxVec3& vTargetPos)
{
	m_vGroundTargetPos = vTargetPos;
	if(m_pGroundMoveController) m_pGroundMoveController->GetGroundPos(m_vGroundTargetPos);
	m_vGroundTargetDir = m_vGroundTargetPos - GetPos();
	m_vGroundTargetDir.y = 0.0f;
	float fM = m_vGroundTargetDir.normalize();
	if(fM > 1e-4f)
	{
		ApplyMoveDir(m_vGroundTargetDir);
		m_bSetCurPosToTargetPos = false;
	}
	else
	{
		m_vGroundTargetDir.zero();
	}
}

bool ApxActorBase::IsGroundTargetPosReached() const
{
	if(m_vGroundTargetDir.isZero())
		return true;

	NxVec3 vDelta = m_vGroundTargetPos - GetPos();
	vDelta.y = 0.0f;
	if(m_vGroundTargetDir.dot(vDelta) <= 0.0f)
		return true;
	else
		return false;
}

void ApxActorBase::Tick(float dt)
{
	bool bToReturn = false;
	if (!m_EnableTick || m_ActorState.GetState() == ActorState::Ragdoll2Normal)
		bToReturn = true;

	if (m_pObjDyn->GetProperties()->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
		bToReturn = true;

	if (bToReturn)
	{
		for(int i=0; i<ApxActionLayer_Num; i++)
			m_ActionControllerPtrs[i]->Tick(dt);

		m_ActorState.Tick(dt);
		return;
	}

	// firstly, reset some state for this tick
	if(m_pGroundMoveController)
	{
		m_pGroundMoveController->ClearFlag(ApxActorMoveController::ApxActor_MoveFlag_Driven);
		m_pGroundMoveController->PrepareTick();
	}

	//set weight
	if (HasHitTarget() || m_bHasTestDir || !m_bAutoTurn)
	{
		A3DVECTOR3 vDir = m_pObjDyn->GetSkinModel()->GetDir();
		vDir.y = 0;
		vDir.Normalize();
		A3DVECTOR3 vRight = m_pObjDyn->GetSkinModel()->GetRight();
		vRight.y = 0;
		vRight.Normalize();

		A3DVECTOR3 vMoveDir;

		if (m_bHasTestDir)
		{
			m_bHasTestDir = false;
			vMoveDir = m_vTestDir;
		}
		else
			vMoveDir = APhysXConverter::N2A_Vector3(m_pGroundMoveController->GetMoveDir());	

		float fOffForward = glb_VectorToYaw(vMoveDir) - glb_VectorToYaw(vDir);
		glb_ClampYaw(fOffForward);

		for (int i = 0; i < ApxActionLayer_Num; i++)
		{
			m_ActionControllerPtrs[i]->SetMoveYaw(fOffForward);

		}
	}
	else
	{
		for (int i = 0; i < ApxActionLayer_Num; i++)
		{
			m_ActionControllerPtrs[i]->SetUseWalkRun(true);
		}
	}

	bool bIsReached = IsGroundTargetPosReached();
	if (!bIsReached)
		UpdateTargetDir();

	Scene* pScene = m_pObjDyn->GetScene();
	CMainActor* pCMA = pScene->GetCurrentActor();
	ApxActorBase* pCMAAB = 0;
	if (0 != pCMA)
		pCMAAB = pCMA->GetActorBase();

	UserInputFlag uifs = UIF_NONE;
	if (MODE_BINDING == m_pObjDyn->GetScene()->GetActiveSmartCamCtrl(false)->GetMode())
	{
		if (m_pRec->IsReplaying())
		{
			uifs = TickUserInput();
			if (this == pCMAAB)
			{
				AutoData ad(0);
				if (m_pRec->ReproItemData(RIT_MA_MOVE_CTRL, &ad))
					uifs = UserInputFlag(ad.nVal);
			}
		}
		else
		{
			uifs = TickUserInput();
			if (this == pCMAAB)
				m_pRec->LogItemData(RIT_MA_MOVE_CTRL, AutoData(uifs));
		}
	}
	ApplyUserInput(uifs);

	// check whether I have reached the target pos.
	if(bIsReached)
	{
		if(!m_bSetCurPosToTargetPos)
		{
			m_vGroundTargetDir.zero();
			//------------------------------------------------------
			// to revise...
			// Note: m_vGroundTargetPos maybe not the exact ground pos you can stand,
			// so we use current y component as the ground pos reference.
			NxVec3 vTargetPos = m_vGroundTargetPos;
			vTargetPos.y = GetPos().y;
			m_pGroundMoveController->GetGroundPos(vTargetPos);

			NxVec3 vUp = m_pGroundMoveController->GetHeadUp();
			vTargetPos += vUp * m_pGroundMoveController->GetExtents().y;
			SetPos(vTargetPos);
			m_bSetCurPosToTargetPos = true;
		}
	}
	else if(m_pGroundMoveController)
	{
		m_pGroundMoveController->RaiseFlag(ApxActorMoveController::ApxActor_MoveFlag_Driven);
	}

	// update the actor's HeadDir...
	//use EcBodyControl
	if (HasHitTarget())
	{
		m_TargetInfo.Tick(dt);
		if (!HasHitTarget())
			ResetHitTarget();
	}

	if (m_bUseBodyCtrl)
	{
		m_vHeadFaceDir = m_pGroundMoveController->GetMoveDir();
		assert(true == NxMath::equals(0.0f, m_vHeadFaceDir.y, APHYSX_FLOAT_EPSILON));
		m_pGroundMoveController->SetHeadDir(m_vHeadFaceDir);
		UpdateRotateBody(m_vHeadFaceDir);
		if (m_pBodyCtrl)
			m_pBodyCtrl->Tick(dt);
	}
	else
	{
		if (0.0f <= m_fTurnT && m_pGroundMoveController)
		{
			m_fTurnT += dt * APHYSX_PI;

			float t = 0;
			if (!NxMath::equals(m_fTheta, 0.0f, APHYSX_FLOAT_EPSILON))
			{
				t = m_fTurnT / m_fTheta;
				if (m_fTurnT >= m_fTheta)
				{
					t = 1.0f;
					m_fTurnT = -1.0f;
				}
			}

			m_vHeadFaceDir = APhysXUtilLib::Slerp(m_vHeadFaceDir, m_pGroundMoveController->GetMoveDir(), t);
			m_pGroundMoveController->SetHeadDir(m_vHeadFaceDir);
		}
	}
	if (m_pUpperBodyCtrl)
	{
		m_pUpperBodyCtrl->Tick(dt);
	}

	// tick the GroundMoveController...
	if(m_pGroundMoveController)
		m_pGroundMoveController->Tick(dt);

#ifdef _ANGELICA21
	m_FootOffBlender.Tick(dt);
#endif
	// update dynamic model's pose by GroundMoveController
	UpdateObjPoseByMoveController();

#ifdef _ANGELICA3
	bool bEnableSwivelAngle = true;
	if (bEnableSwivelAngle)
	{
		// try to adjust the swivel angle of the FOOT IK

		// try to compute the swivel angle...
		A3DSkinModel* pSkinModel = m_pObjDyn->GetSkinModel();
		A3DBone* pBone = pSkinModel->GetSkeleton()->GetBone("Bip01 Spine1", NULL);
		A3DQUATERNION qRot = pBone->GetRelativeQuat();
		A3DVECTOR3 vAxisY = pBone->GetRelativeTM().GetCol(1);
		vAxisY.Normalize();
		float fAngle = acos(DotProduct(vAxisY, A3DVECTOR3(0, 1, 0)));
		bool bSign = CrossProduct(vAxisY, A3DVECTOR3(0, 1, 0)).x < 0;

		if(bSign) fAngle = -fAngle;


		// tmp code... Log the swivel angle of Foot IK...

		static float s_fSwivelAngel = 0.0f;
		if(IS_KEY_PRESSED(VK_MULTIPLY))
			s_fSwivelAngel += 0.1f;

		if(IS_KEY_PRESSED(VK_DIVIDE))
			s_fSwivelAngel -= 0.1f;

		if(IS_KEY_PRESSED(VK_HOME))
			s_fSwivelAngel = 0.0f;
			
		a_Clamp(s_fSwivelAngel, -APHYSX_PI, APHYSX_PI);

		if (0 != m_pLIKTrigger)
		{
			for (int i = 0; i < m_pLIKTrigger->GetIKSolverNum(); i++)
			{
				if(m_pLIKTrigger->GetIKSolver(i)->IsEnabled())
				{
					A3DIKSoverFootPlacement* pFPSolver = (A3DIKSoverFootPlacement*)(m_pLIKTrigger->GetIKSolver(i));
					// pFPSolver->SetSwivelAngle(s_fSwivelAngel);
					pFPSolver->SetSwivelAngle(fAngle);

					// a_LogOutput(1, "Left Foot Swivel Angel: %f",pFPSolver->GetSwivelAngle());
				}
			}
		}
		if (0 != m_pRIKTrigger)
		{
			for (int i = 0; i < m_pRIKTrigger->GetIKSolverNum(); i++)
			{
				if(m_pRIKTrigger->GetIKSolver(i)->IsEnabled())
				{
					A3DIKSoverFootPlacement* pFPSolver = (A3DIKSoverFootPlacement*)(m_pRIKTrigger->GetIKSolver(i));
					// pFPSolver->SetSwivelAngle(s_fSwivelAngel);
					pFPSolver->SetSwivelAngle(fAngle);

					// a_LogOutput(1, "Right Foot Swivel Angel: %f",pFPSolver->GetSwivelAngle());
				}
			}	
		}
	}
#endif

	// at last, we tick the action controller...
	for(int i=0; i<ApxActionLayer_Num; i++)
	{
		m_ActionControllerPtrs[i]->Tick(dt);
	}

	DWORD dwTick = (DWORD)(dt * 1000);
	if (m_pLHandTarget)
		m_pLHandTarget->Tick(dwTick);
	if (m_pRHandTarget)
		m_pRHandTarget->Tick(dwTick);
	if (m_pCarryable)
		m_pCarryable->Tick(dwTick);

	//set hand position
	if (m_pCarryable)
	{
		if (m_pCarryable->GetState() == CCarryable::Free)
			m_pCarryable = NULL;
		else
		{
			A3DVECTOR3 vLPos, vRPos;
			if (!IsIKEnabled())//we update it in PreSolver function when ik enabled
				m_pCarryable->UpdateBindPosDir();
			bool bRet = m_pCarryable->GetBindPosWorld(vLPos, vRPos);
			if (bRet)
			{
				m_pLHandTarget->SetIKGoalPos(vLPos);
				m_pRHandTarget->SetIKGoalPos(vRPos);
			}
		}
	}
	if (m_pPushable)
	{
		if (!m_pLHandTarget->IsIKEnable())
		{
			m_pPushable = NULL;
		}
		else
		{
			A3DVECTOR3 vLPos, vRPos;
			if (!IsIKEnabled())//we update it in PreSolver function when ik enabled
				m_pPushable->UpdateBindPosDir();

			bool bRet = m_pPushable->GetBindPosWorld(vLPos, vRPos);
			if (bRet)
			{
				m_pLHandTarget->SetIKGoalPos(vLPos);
				m_pRHandTarget->SetIKGoalPos(vRPos);
			}
		}

	}
	//------------------------------------------------------
	// set the drive mode to DRIVEN_BY_PART_PHYSX according 
	// to s_iDelaySetPartPhysicsDMCounter

	if(m_iDelaySetPartPhysicsDMCounter>=0)
	{
		m_iDelaySetPartPhysicsDMCounter++;
		if(m_iDelaySetPartPhysicsDMCounter > 3)
		{
			CDynProperty* pProp = m_pObjDyn->GetProperties();

			bool bIsLock = pProp->GetFlags().ReadFlag(OBF_DRIVENMODE_ISLOCKED);
			if (bIsLock)
				pProp->GetFlags().ClearFlag(OBF_DRIVENMODE_ISLOCKED);


			pProp->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PART_PHYSX);

			if (bIsLock)
				pProp->GetFlags().RaiseFlag(OBF_DRIVENMODE_ISLOCKED);

			m_iDelaySetPartPhysicsDMCounter = -1;
		}
	}
}

bool ApxActorBase::ChangeEquipment(const ApxWeaponInfo* pNewWeapon, APhysXScene* pPhysXScene, IEngineUtility* pEU)
{
	if (0 == pNewWeapon)
	{
		ReleaseEquipment(m_pCurrentEquipment);
		delete m_pCurrentEquipment;
		m_pCurrentEquipment = 0;
		return true;
	}
	else
	{
		if (0 != m_pCurrentEquipment)
		{
			if (*m_pCurrentEquipment == *pNewWeapon)
				return true;
		}
		else
		{
			m_pCurrentEquipment = new ApxWeaponInfo;
			if (0 == m_pCurrentEquipment)
				return false;
		}
		if (0 == pEU)
			return false;

		ApxObjBase* pCurrentSocketSM = 0;
		if (0 != m_pCurrentEquipment->pWeaponObj)
			pCurrentSocketSM = m_pCurrentEquipment->pWeaponObj->GetParent();
		CPhysXObjDynamic* pObj = DoChangeEquipment(*pNewWeapon, pPhysXScene, *pEU);
		if (0 != pObj)
		{
			if (pObj->GetParent() != pCurrentSocketSM)
				ReleaseEquipment(m_pCurrentEquipment);
			*m_pCurrentEquipment = *pNewWeapon;
			m_pCurrentEquipment->pWeaponObj = pObj;
			OnChangeEquipment();
			return true;
		}
	}
	return false;
}

CPhysXObjDynamic* ApxActorBase::DoChangeEquipment(const ApxWeaponInfo& awiNewWeapon, APhysXScene* pPhysXScene, IEngineUtility& eu)
{
	assert(0 != m_pObjDyn);
	if (awiNewWeapon.bUsingXAJHWay)
		return DoChangeEquipmentXAWay(awiNewWeapon, pPhysXScene, eu);

	ApxObjSkinModelToSocket* pSMToSocket = 0;
	ApxObjSocketToSkinModel* pSocketToSM = 0;
	ObjManager* pObjMgr = ObjManager::GetInstance();
	bool IsInRuntime = m_pObjDyn->GetProperties()->IsRuntime();
	Scene& scene = *(m_pObjDyn->GetScene());

	// create and set socket1
	TString strHost = GetSocketLocalName(true, awiNewWeapon.smsHostSocketType, awiNewWeapon.strHostSocketName);
	IObjBase* pObjP = m_pObjDyn->GetChildByLocalName(strHost);
	if (0 != pObjP)
	{
		assert(ObjManager::OBJ_TYPEID_SMSOCKET == pObjP->GetProperties()->GetObjType());
		pSMToSocket = dynamic_cast<ApxObjSkinModelToSocket*>(pObjP);
	}
	if (0 == pSMToSocket)
	{
		IObjBase* pObj = pObjMgr->CreateObject(ObjManager::OBJ_TYPEID_SMSOCKET, false, scene);
		if (0 == pObj)
			return 0;
		IPropObjBase* pProp = pObj->GetProperties();
		pProp->GetFlags().ClearFlag(OBF_DRAW_BINDING_BOX);
		pProp->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
		if (IsInRuntime && (0 != pPhysXScene))
			pObj->EnterRuntime(*pPhysXScene);
		pSMToSocket = dynamic_cast<ApxObjSkinModelToSocket*>(pObj);
		m_pObjDyn->AddChild(pSMToSocket);
		pSMToSocket->GetProperties()->SetObjLocalName(strHost);
	}
	if (!pSMToSocket->Attach(m_pObjDyn, awiNewWeapon.strHostSocketName, awiNewWeapon.smsHostSocketType))
		return 0;

	// create and set socket2
	TString strClient = GetSocketLocalName(false, awiNewWeapon.smsWPNSocketType, awiNewWeapon.strWPNSocketName);
	IObjBase* pObjC = pSMToSocket->GetChildByLocalName(strClient);
	if (0 != pObjC)
	{
		assert(ObjManager::OBJ_TYPEID_SOCKETSM == pObjC->GetProperties()->GetObjType());
		pSocketToSM = dynamic_cast<ApxObjSocketToSkinModel*>(pObjC);
	}
	if (0 == pSocketToSM)
	{
		IObjBase* pObj = pObjMgr->CreateObject(ObjManager::OBJ_TYPEID_SOCKETSM, false, scene);
		if (0 == pObj)
			return 0;
		IPropObjBase* pProp = pObj->GetProperties();
		pProp->GetFlags().ClearFlag(OBF_DRAW_BINDING_BOX);
		pProp->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
		if (IsInRuntime && (0 != pPhysXScene))
			pObj->EnterRuntime(*pPhysXScene);
		pSocketToSM = dynamic_cast<ApxObjSocketToSkinModel*>(pObj);
		pSMToSocket->AddChild(pSocketToSM);
		pSocketToSM->GetProperties()->SetObjLocalName(strClient);
	}

	// create and set weapon model
	IObjBase* pOldWeapon = pSocketToSM->GetBindingObject();
	IObjBase* pObject = pObjMgr->CreateObject(ObjManager::OBJ_TYPEID_ECMODEL, false, scene);
	if (0 == pObject)
		return 0;
	CPhysXObjDynamic* pObjDynWeapon = dynamic_cast<CPhysXObjDynamic*>(pObject);
	if (!pObjDynWeapon->LoadModel(eu, _TWC2AS(awiNewWeapon.strWeaponPathName), pPhysXScene, false))
	{
		pObjMgr->ReleaseObject(pObjDynWeapon);
		return 0;
	}
	IPropObjBase* pProp = pObjDynWeapon->GetProperties();
	pProp->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
	if (IsInRuntime && (0 != pPhysXScene))
	{
		pObjDynWeapon->EnterRuntime(*pPhysXScene);
		APhysXSkeletonRBObject* pSRBO = pObjDynWeapon->GetSkeletonRBObject();
		if (0 != pSRBO)
		{
			pPhysXScene->LockWritingScene();
			pSRBO->SetCollisionGroup(APX_COLLISION_GROUP_ATTACHER);
			pPhysXScene->UnlockWritingScene();
		}
	}
	if (pSocketToSM->Attach(pObjDynWeapon, awiNewWeapon.strWPNSocketName, awiNewWeapon.smsWPNSocketType))
	{
		pObjMgr->ReleaseObject(pOldWeapon);
		return pObjDynWeapon;
	}
	return 0;
}

CPhysXObjDynamic* ApxActorBase::DoChangeEquipmentXAWay(const ApxWeaponInfo& awiNewWeapon, APhysXScene* pPhysXScene, IEngineUtility& eu)
{
	assert(0 != m_pObjDyn);
	assert(true == awiNewWeapon.bUsingXAJHWay);
	ObjManager* pObjMgr = ObjManager::GetInstance();
	bool IsInRuntime = m_pObjDyn->GetProperties()->IsRuntime();
	Scene& scene = *(m_pObjDyn->GetScene());

	IObjBase* pObject = pObjMgr->CreateObject(ObjManager::OBJ_TYPEID_ECMODEL, false, scene);
	if (0 == pObject)
		return 0;
	CPhysXObjDynamic* pObjDynWeapon = dynamic_cast<CPhysXObjDynamic*>(pObject);
	if (!pObjDynWeapon->LoadModel(eu, _TWC2AS(awiNewWeapon.strWeaponPathName), pPhysXScene, false))
	{
		pObjMgr->ReleaseObject(pObjDynWeapon);
		return 0;
	}
	IPropPhysXObjBase* pProp = pObjDynWeapon->GetProperties();
	pProp->GetFlags().RaiseFlag(OBF_RUNTIME_ONLY);
	pProp->SetDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX, true);
	if (IsInRuntime && (0 != pPhysXScene))
	{
		pObjDynWeapon->EnterRuntime(*pPhysXScene);
		APtrArray<NxActor*> nxActors;
		pObjDynWeapon->GetNxActors(nxActors);
		int nAlls = nxActors.GetSize();
		if (1 < nAlls)
		{
			for (int i = 0; i < nAlls; ++i)
			{
				if (!nxActors[i]->readBodyFlag(NX_BF_DISABLE_GRAVITY))
					nxActors[i]->setContactReportFlags(NX_IGNORE_PAIR);
			}
		}
	}

	CECModel* pHostECM = m_pObjDyn->GetECModel();
	if (!pHostECM->AddChildModel(_TWC2AS(awiNewWeapon.strWPNECMNickName), false, _TWC2AS(awiNewWeapon.strHostSocketName), 
		pObjDynWeapon->GetECModel(), _TWC2AS(awiNewWeapon.strWPNSocketName)))
	{
		pObjMgr->ReleaseObject(pObjDynWeapon);
		return 0;
	}

	pHostECM->CloseChildModelLink(_TWC2AS(awiNewWeapon.strWPNECMNickName));
	if (!pHostECM->OpenChildModelLink(_TWC2AS(awiNewWeapon.strWPNECMNickName), _TWC2AS(awiNewWeapon.strHostSocketName),
		_TWC2AS(awiNewWeapon.strWPNAttacher)))
	{
		pHostECM->RemoveChildModel(_TWC2AS(awiNewWeapon.strWPNECMNickName), false);
		pObjMgr->ReleaseObject(pObjDynWeapon);
		return 0;
	}
	pObjDynWeapon->SetParentModel(m_pObjDyn, _TWC2AS(awiNewWeapon.strWPNECMNickName));
	m_pObjDyn->AddChild(pObjDynWeapon);
	return pObjDynWeapon;
}

void ApxActorBase::ReleaseEquipment(ApxWeaponInfo* pAWI)
{
	if (0 == pAWI)
		return;

	if (pAWI->bUsingXAJHWay)
	{
		CECModel* pHostECM = m_pObjDyn->GetECModel();
		pHostECM->CloseChildModelLink(_TWC2AS(pAWI->strWPNECMNickName));
		pHostECM->RemoveChildModel(_TWC2AS(pAWI->strWPNECMNickName), false);
		if (0 != pAWI->pWeaponObj)
		{
			pAWI->pWeaponObj->SetParentModel(0, "0");
			pAWI->pWeaponObj->ToDie();
			ObjManager::GetInstance()->ReleaseObject(pAWI->pWeaponObj);
		}
	}
	else
	{
		TString strHost = GetSocketLocalName(true, pAWI->smsHostSocketType, pAWI->strHostSocketName);
		IObjBase* pObjP = m_pObjDyn->GetChildByLocalName(strHost);
		if (0 == pObjP)
			return;

		TString strClient = GetSocketLocalName(false, pAWI->smsWPNSocketType, pAWI->strWPNSocketName);
		IObjBase* pObjC = pObjP->GetChildByLocalName(strClient);
		if (0 == pObjC)
			return;

		pObjP->RemoveChild(pObjC);
		pObjC->ToDie();
		ObjManager::GetInstance()->ReleaseObject(pObjC);
	}
	pAWI->pWeaponObj = 0;
}

TString ApxActorBase::GetSocketLocalName(bool bIsHost, ApxSkinModelSocketType smsType, const TString& strName)
{
	TString str = strName;
	if (ApxSM_SocketType_Bone == smsType)
		str += _T("_Bone");
	else if (ApxSM_SocketType_Hook == smsType)
		str += _T("_Hook");
	if (bIsHost)
		str += _T("_ToHost");
	else
		str += _T("_ToClient");
	return str;
}

const ApxWeaponInfo* ApxActorBase::GetCurrentEquipment() const
{
	if (0 == m_pCurrentEquipment)
		return m_pCurrentEquipment;
	if (m_pCurrentEquipment->strWeaponPathName.IsEmpty())
		return 0;
	return m_pCurrentEquipment;
}

bool ApxActorBase::GetWeaponPose(NxMat34& pose, NxCapsule* pOutCapsule)
{
	if (0 == m_pCurrentEquipment)
		return false;
	if (0 == m_pCurrentEquipment->pWeaponObj)
		return false;

	APhysXSkeletonRBObject* pPSRB = m_pCurrentEquipment->pWeaponObj->GetSkeletonRBObject();
	if (0 == pPSRB)
		return false;
	int nCount = pPSRB->GetNxActorNum();
	NxActor* pActor = 0;
#ifdef _ANGELICA3 
	assert(1 == nCount);
	if (0 >= nCount)
		return false;
	pActor = pPSRB->GetNxActor(0);
#else
	int i = 0;
	for (; i < nCount; ++i)
	{
		pActor = pPSRB->GetNxActor(0);
		if (0 == (NX_IGNORE_PAIR & pActor->getContactReportFlags()))
			break;
	}
	if (i == nCount)
		pActor = 0;
#endif
	assert(0 != pActor);
	if (0 == pActor)
		return false;

	int nShape = pActor->getNbShapes();
	assert(1 == nShape);
	if (0 >= nShape)
		return false;
	NxShape* pShape = pActor->getShapes()[0];
	const NxCapsuleShape* pCap = pShape->isCapsule();
	if (0 == pCap)
		return false;

	NxCapsule capsule;
	pCap->getWorldCapsule(capsule);
	pose = pCap->getGlobalPose();
	pose.t = capsule.p0 + capsule.p1; // always using center position 
	pose.t *= 0.5f;
	if (0 != pOutCapsule)
		*pOutCapsule = capsule;
	return true;
}

bool ApxActorBase::UpdateWeaponPose(NxCapsule* pOutOldCapsule, NxCapsule* pOutNewCapsule, NxMat34* pOutOldPose, NxMat34* pOutNewPose)
{
	if (0 != pOutOldCapsule)
		*pOutOldCapsule = m_WeaponLastCapsule;
	if (0 != pOutOldPose)
		*pOutOldPose = m_WeaponLastPose;
	bool bRtn = GetWeaponPose(m_WeaponLastPose, &m_WeaponLastCapsule);
	if (0 != pOutNewPose)
		*pOutNewPose = m_WeaponLastPose;
	if (0 != pOutNewCapsule)
		*pOutNewCapsule = m_WeaponLastCapsule;
	return bRtn;
}

void ApxActorBase::Init(CPhysXObjDynamic* pObjDyn, const NxVec3& vExtents)
{
	APHYSX_ASSERT(pObjDyn);
	m_pObjDyn = pObjDyn;
	if (0 != m_pObjDyn)
		m_pObjDyn->SetAdvanceAssailable(this);
	m_pRec = &(m_pObjDyn->GetScene()->GetRecording());

	m_backupCNL = APX_COLLISION_CHANNEL_INVALID;
	bool bRtn = m_pObjDyn->GetCollisionChannel(m_backupCNL);
	if (bRtn)
	{
		if ((APX_COLLISION_CHANNEL_COMMON == m_backupCNL) || (APX_COLLISION_CHANNEL_COLLIDE_ALL == m_backupCNL))
			m_pObjDyn->SetCollisionChannel(CPhysXObjDynamic::CNL_INDEPENDENT);
		else
			m_backupCNL = APX_COLLISION_CHANNEL_INVALID;
	}

	for(int i=0; i<ApxActionLayer_Num; i++)
	{
		m_ActionControllerPtrs[i] = new ApxSkinModelActionController(m_pObjDyn);
		m_ActionControllerPtrs[i]->InitActorBase(this);
		m_ActionControllerPtrs[i]->Init();
		m_ActionControllerPtrs[i]->SetActionLayer((ApxActionLayer)i);
//		m_ActionControllerPtrs[i]->SetUseWalkRun(true);
	}

	m_ActionControllerPtrs[ApxActionLayer_Move]->SetActiveChannel(ApxActionChannel_FullBody, 0.0f);
	
	NxVec3 vDir, vUp;
	pObjDyn->GetGDirAndUp(vDir, vUp);
	m_vHeadFaceDir = vDir;
	m_vHeadFaceDir.y = 0;
	m_vHeadFaceDir.normalize();

	NxVec3 vPosFoot = pObjDyn->GetGPos();
	m_vGroundTargetPos = vPosFoot;
	m_bSetCurPosToTargetPos = true;

	m_pGroundMoveController = new ActorGroundMoveCtrlImp(&pObjDyn->GetProperties()->GetCCMgr());
	if (m_pGroundMoveController)
	{
		m_pGroundMoveController->InitActorBase(this);
		NxVec3 vPos = vPosFoot + vUp * vExtents.y;
		m_pGroundMoveController->SetPos(vPos);
		m_pGroundMoveController->SetHeadDir(m_vHeadFaceDir);
		m_pGroundMoveController->SetExtents(vExtents);
		m_pGroundMoveController->SetMaxJumpTimes(20);
	}

	m_pBodyCtrl = new ECPlayerBodyCtrl;
	m_pBodyCtrl->BindPlayer(this);

	m_pUpperBodyCtrl = new UpperBodyCtrl;
	m_pUpperBodyCtrl->BindPlayer(this);

	ApplyMoveDir(m_vHeadFaceDir);
	m_ActorState.BindPlayer(this);

	s_pSk = m_pObjDyn->GetSkinModel()->GetSkeleton();
}

void ApxActorBase::Release()
{
	m_ActorState.Release();

	ResetHitTarget();

	ReleaseHandIK();
	ReleaseFootIK();
	ReleaseWalkRunBlend();

	// don't release the CPhysXObjDynamic model, since:
	// 1. we didn't create it
	// 2. the model maybe still be used by outside codes
	if (APX_COLLISION_CHANNEL_INVALID != m_backupCNL)
	{
		m_backupCNL = APX_COLLISION_CHANNEL_INVALID;
		if (0 != m_pObjDyn)
			m_pObjDyn->SetCollisionChannel(CPhysXObjDynamic::CNL_COMMON);
	}

	m_pObjDyn = NULL;
	m_pRec = 0;

	for(int i=0; i<ApxActionLayer_Num; i++)
	{
		m_ActionControllerPtrs[i]->Release();
		delete m_ActionControllerPtrs[i];
		m_ActionControllerPtrs[i] = NULL;
	}

	delete m_pGroundMoveController ;
	m_pGroundMoveController = NULL;	

	delete m_pCurrentEquipment;
	m_pCurrentEquipment = 0;

	delete m_pBodyCtrl;
	m_pBodyCtrl = NULL;

	delete m_pUpperBodyCtrl;
	m_pUpperBodyCtrl = NULL;
}

#ifdef _ANGELICA3

float ApxActorBase::GetIKGoalOffSetY(A3DIKTrigger* pTrigger)
{
	if (pTrigger == NULL)
		return 0;

	if (pTrigger->GetIKSolverNum() == 0)
		return 0;

	A3DIKSolver* pIKSolver = pTrigger->GetIKSolver(0);
	bool bReach = pIKSolver->IsGoalReached();
	if (pIKSolver->IsEnabled() && !bReach)
	{		
		A3DVECTOR3 vGoalPos;

		bool bRet = pIKSolver->GetIKGoal()->GetGoalPos(vGoalPos);
		if (bRet)
		{
			vGoalPos = ((A3DIKSoverFootPlacement*)pIKSolver)->GetGoalPos();
			A3DIKSolver::IK_JOINT* pJoint = pIKSolver->GetEEJoint();
			A3DBone* pBone = pIKSolver->GetBoneByJoint(pJoint);
			A3DVECTOR3 vPos = pBone->GetAbsoluteTM().GetTransPart();
			return vGoalPos.y - vPos.y;
		}
		return 0;
	}
	return 0;
}

#else

float GetIKGoalOffSetY(A3DIKSolver* pIKSolver)
{
	if(!pIKSolver) return 0.0f;

	bool bReach = pIKSolver->IsGoalReached();
	if (pIKSolver->IsEnabled() && !bReach)
	{		
		A3DVECTOR3 vGoalPos;

		bool bRet = pIKSolver->GetIKGoal()->GetGoalPos(vGoalPos);
		if (bRet)
		{
			vGoalPos = ((A3DIKSoverFootPlacement*)pIKSolver)->GetGoalPos();
			A3DIKSolver::IK_JOINT* pJoint = pIKSolver->GetEEJoint();
			A3DBone* pBone = pIKSolver->GetBoneByJoint(pJoint);
			A3DVECTOR3 vPos = pBone->GetAbsoluteTM().GetTransPart();
			return vGoalPos.y - vPos.y;
		}
		return 0;
	}

	return 0;

}

#endif

void ApxActorBase::UpdateObjPoseByMoveController()
{
	if(m_pGroundMoveController)
	{
		NxVec3 vUp = m_pGroundMoveController->GetHeadUp();
		NxVec3 vFootPos = m_pGroundMoveController->GetPos();
		vFootPos -= vUp * m_pGroundMoveController->GetExtents().y;
		
#if defined(_ANGELICA3) || defined(_ANGELICA22)
		vFootPos.y += m_fOffSetY;
#else
		vFootPos.y += m_FootOffBlender.GetValue();//m_fOffSetY;
#endif;
		m_pObjDyn->SetGPos(vFootPos);
		NxVec3 vNxDir;
		if (!m_bUseBodyCtrl && HasHitTarget())
		{
			vNxDir = APhysXConverter::A2N_Vector3(m_TargetInfo.vPos) - vFootPos;
			vNxDir.y = 0;
			vNxDir.normalize();
		}
		else
		{
			vNxDir = m_pGroundMoveController->GetHeadDir();
		}
		m_pObjDyn->SetGDirAndUp(vNxDir, vUp);


		if (m_pLFIKSolver == NULL || m_pRFIKSolver == NULL)
			return;

		if (m_pObjDyn->GetSkinModel()->IsIKEnabled())
		{
#if defined(_ANGELICA3) || defined(_ANGELICA22)
			
			float lOffSet =  GetIKGoalOffSetY(m_pLIKTrigger);
			float rOffSet =  GetIKGoalOffSetY(m_pRIKTrigger);

			// if (lOffSet > -0.5f && lOffSet < -0.05f || rOffSet > -0.5f && rOffSet < -0.05f)
			if (lOffSet > -0.5f && lOffSet < -0.02f || rOffSet > -0.5f && rOffSet < -0.02f)
			{
				if (m_fOffSetY > -0.5f)
					m_fOffSetY += -0.01f;//lOffSet < rOffSet ? lOffSet : rOffSet;
			}
			else if (lOffSet >= 0.0f && rOffSet >= 0.0f)
			{
				m_fOffSetY += 0.01f;
				a_Clamp(m_fOffSetY, -0.5f, 0.0f);
			}
#else
			
			float lOffSet = GetIKGoalOffSetY(m_pLFIKSolver);
			float rOffSet = GetIKGoalOffSetY(m_pRFIKSolver);


			A3DBone* pFootBone = m_pObjDyn->GetSkinModel()->GetSkeleton()->GetBone(m_pLFIKSolver->GetEEJoint()->iBoneIdx);

			if (m_bUseIKFrame)
			{
				if (pFootBone->IsInTransition())
					return;

			}



#define IK_ADJUST_EPS 0.01f

			//try another method
			float fLOffset = m_pLFIKSolver->GetIKAdjustOffsetY();
			float fROffset = m_pRFIKSolver->GetIKAdjustOffsetY();
			float fOffset = min2(fLOffset, fROffset);

			if( fOffset < 0.5f && fOffset > IK_ADJUST_EPS 
				|| fOffset > -0.5f && fOffset < -IK_ADJUST_EPS) 

			{
				// note the factor value to multiply fMax, which is less than 1.0 so as to avoid the over-adjust case...
				fOffset = m_FootOffBlender.GetValue() + fOffset * 1.0f;		
				// m_FootOffBlender.SetTargetValue(fOffset, 0.5f);
				m_FootOffBlender.SetTargetValue(fOffset, 0.2f);
				//a_LogOutput(1, "update target value: %f\n", fOffset);
			}
#endif
			
		}

	}
}

void ApxActorBase::UpdateTargetDir()
{
	if (m_vGroundTargetDir.isZero())
		return;

	m_vGroundTargetDir = m_vGroundTargetPos - GetPos();
	m_vGroundTargetDir.y = 0.0f;
	float fM = m_vGroundTargetDir.normalize();
	if(fM > 1e-4f)
		ApplyMoveDir(m_vGroundTargetDir);
	else
		m_vGroundTargetDir.zero();
}

A3DIKSolver* ApxActorBase::CreateIKSolver(const AString& strSolverName, const AString& strBoneName)
{
	A3DIKSolver* pSolver = 0;
#ifdef _ANGELICA3
	A3DSkinModel* pSkinModel = m_pObjDyn->GetSkinModel();
	pSolver = A3DIKSolver::CreateSolver(A3DIKSolver::IKS_FOOTPLACEMENT, pSkinModel->GetA3DEngine());
	if (pSolver == NULL)
		return NULL;

	pSolver->SetName(strSolverName);
	APtrStack<A3DBone*> aBones;
	A3DBone* pBone = pSkinModel->GetSkeleton()->GetBone(strBoneName, NULL);
	aBones.Push(pBone);
	aBones.Push(pBone->GetParentPtr());
	aBones.Push(pBone->GetParentPtr()->GetParentPtr());
	A3DIKSolver::IK_JOINT joint;
	while (aBones.GetElementNum())
	{
		A3DBone* pCurBone = aBones.Pop();
		joint.strBoneName = pCurBone->GetName();
		pSolver->AddJoint(joint);
	}
#endif
	return pSolver;
}

bool ApxActorBase::ReleaseFootIK()
{
#ifdef _ANGELICA3
	A3DSkinModel* pSkinModel = m_pObjDyn->GetSkinModel();
	if (m_pLIKTrigger)
	{
		pSkinModel->UnbindIKTrigger(m_pLIKTrigger);
		for (int i = 0; i < m_pLIKTrigger->GetIKSolverNum(); i++)
			delete m_pLIKTrigger->GetIKSolver(i);
	}
	if (m_pRIKTrigger)
	{
		pSkinModel->UnbindIKTrigger(m_pRIKTrigger);
		for (int i = 0; i < m_pRIKTrigger->GetIKSolverNum(); i++)
			delete m_pRIKTrigger->GetIKSolver(i);
	}

	delete m_pLIKTrigger;
	delete m_pRIKTrigger;
	m_pLIKTrigger = NULL;
	m_pRIKTrigger = NULL;
#endif
	return true;
}

//@parameter strLFoot: bone name of left foot
//           strRFoot: bone name of right foot
bool ApxActorBase::InitFootIK(const AString& strLFoot, const AString& strRFoot)
{	
#ifdef _ANGELICA3
	ReleaseFootIK();

	A3DIKSolver* pLSolver = CreateIKSolver("lfoot", strLFoot);
	A3DIKSolver* pRSolver = CreateIKSolver("rfoot", strRFoot);
	if (pLSolver == NULL || pRSolver == NULL)
	{
		delete pLSolver;
		delete pRSolver;
		return false;
	}

	A3DIKSoverFootPlacement* pSolverPlacement = dynamic_cast<A3DIKSoverFootPlacement*>(pLSolver);
	if (pSolverPlacement)
	{
		pSolverPlacement->EnableFootYawCorrection(false);
		m_pLFIKSolver = pSolverPlacement;
	}

	pSolverPlacement = dynamic_cast<A3DIKSoverFootPlacement*>(pRSolver);
	if (pSolverPlacement)
	{
		pSolverPlacement->EnableFootYawCorrection(false); 
		m_pRFIKSolver = pSolverPlacement;
	}

	pLSolver->SetIKGoal_Ground();
	pRSolver->SetIKGoal_Ground();
	pLSolver->Enable(true);
	pRSolver->Enable(true);

	A3DIKTrigger* pLIKTrigger = new A3DIKTrigger;
	pLIKTrigger->AddIKSolver(pLSolver);
	pLIKTrigger->SetAllActionFlag(false);
	pLIKTrigger->SetActionChannelMask(0xffff);

	A3DIKTrigger* pRIKTrigger = new A3DIKTrigger;
	pRIKTrigger->AddIKSolver(pRSolver);
	pRIKTrigger->SetAllActionFlag(false);
	pRIKTrigger->SetActionChannelMask(0xffff);

	A3DSkinModel* pSkinModel = m_pObjDyn->GetSkinModel();
	pSkinModel->BindIKTrigger(pLIKTrigger);
	pSkinModel->BindIKTrigger(pRIKTrigger);

	m_pLIKTrigger = pLIKTrigger;
	m_pRIKTrigger = pRIKTrigger;

#else
	
	A3DSkinModel* pSkinModel = m_pObjDyn->GetSkinModel();
	m_pLFIKSolver = (A3DIKSoverFootPlacement *)pSkinModel->CreateIKSolver(A3DIKSolver::IKS_FOOTPLACEMENT, "Left Foot", "Bip01 L Foot");
//	m_pLFIKSolver->InitEEBone("Bip01 L Foot");
	m_pLFIKSolver->SetTransTime(200);
	// m_pLFIKSolver->SetMaxPitchCorrectionDegree(30);
	m_pLFIKSolver->EnableFootYawCorrection(false);
	// m_pLFIKSolver->EnableFootOrtCorrection(false);

	m_pRFIKSolver = (A3DIKSoverFootPlacement *)pSkinModel->CreateIKSolver(A3DIKSolver::IKS_FOOTPLACEMENT, "Right Foot", "Bip01 R Foot");
//	m_pRFIKSolver->InitEEBone("Bip01 R Foot");
	m_pRFIKSolver->SetTransTime(200);
	// m_pRFIKSolver->SetMaxPitchCorrectionDegree(30);
	m_pRFIKSolver->EnableFootYawCorrection(false);
	// m_pRFIKSolver->EnableFootOrtCorrection(false);

	m_FootOffBlender.Init(0);

#endif

	return true;
}

//enable & disable foot IK
void ApxActorBase::EnableFootIK(bool bLFoot, bool bRFoot)
{
#ifdef _ANGELICA3
	int i;
	if (0 != m_pLIKTrigger)
	{
		for (i = 0; i < m_pLIKTrigger->GetIKSolverNum(); i++)
		{
			m_pLIKTrigger->GetIKSolver(i)->Enable(bLFoot);
		}
	}
	if (0 != m_pRIKTrigger)
	{
		for (i = 0; i < m_pRIKTrigger->GetIKSolverNum(); i++)
		{
			m_pRIKTrigger->GetIKSolver(i)->Enable(bRFoot);
		}	
	}
#else
	
	if(m_pLFIKSolver) m_pLFIKSolver->Enable(bLFoot);
	if(m_pRFIKSolver) m_pRFIKSolver->Enable(bRFoot);

	if (!bLFoot || !bRFoot)
		m_FootOffBlender.SetTargetValue(0.0f, 0.2f);

#endif
}

bool ApxActorBase::InitWalkRunBlend(const AString& strWalk, const AString& strRun)
{
	bool bRet = false;
#ifdef _ANGELICA3
	ReleaseWalkRunBlend();
	m_pBlendWalkRunUpper = new A3DAnimNodeBlendWalkRun;
	m_pBlendWalkRunLower = new A3DAnimNodeBlendWalkRun;

	bool bRetUpper = m_pBlendWalkRunUpper->Init(m_pObjDyn->GetSkinModel(), strWalk, strRun);
	bool bRetLower = m_pBlendWalkRunLower->Init(m_pObjDyn->GetSkinModel(), strWalk, strRun);
	if (m_pGroundMoveController && bRetUpper && bRetLower)
	{
		m_pBlendWalkRunUpper->SetVelocity(m_pGroundMoveController->GetMoveSpeed());
		m_pBlendWalkRunLower->SetVelocity(m_pGroundMoveController->GetMoveSpeed());
	}
	else
	{
		ReleaseWalkRunBlend();
	}
#endif
	return bRet;
}
bool ApxActorBase::ReleaseWalkRunBlend()
{
#ifdef _ANGELICA3
	delete m_pBlendWalkRunUpper;
	m_pBlendWalkRunUpper = NULL;
	delete m_pBlendWalkRunLower;
	m_pBlendWalkRunLower = NULL;
#endif
	return true;
}

void ApxActorBase::SetGroundMoveSpeed(float fSpeed)
{ 
	if(m_pGroundMoveController)
	{
		m_pGroundMoveController->SetMoveSpeed(fSpeed);
		for (int i = 0; i < ApxActionLayer_Num; i++)
			m_ActionControllerPtrs[i]->SetSpeed(fSpeed);

#ifdef _ANGELICA3
		if (m_pBlendWalkRunUpper)
			m_pBlendWalkRunUpper->SetVelocity(fSpeed);
		if (m_pBlendWalkRunLower)
			m_pBlendWalkRunLower->SetVelocity(fSpeed);
#endif
	}
}

float ApxActorBase::GetGroundMoveSpeed() const
{
	if (m_pGroundMoveController)
	{
		return m_pGroundMoveController->GetMoveSpeed();
	}
	return 0;
}



void ApxActorBase::SetBlendWalkRunSpeed(float fSpeed, float fBlendSeconds)
{
#ifdef _ANGELICA3
	if (m_pBlendWalkRunUpper)
		m_pBlendWalkRunUpper->SetVelocity(fSpeed, (int)(fBlendSeconds * 1000));
	if (m_pBlendWalkRunLower)
		m_pBlendWalkRunLower->SetVelocity(fSpeed, (int)(fBlendSeconds * 1000));
#endif
}

void ApxActorBase::InitHandIK(const char* strLHand, const char* strRHand)
{
	ReleaseHandIK();

	m_pLHandTarget = new CIKHandTarget;
	m_pRHandTarget = new CIKHandTarget;

	m_pLHandTarget->Init(m_pObjDyn->GetSkinModel(), strLHand);
	m_pRHandTarget->Init(m_pObjDyn->GetSkinModel(), strRHand);

	m_pLHandTarget->SetSolverInterface(this);
	m_pRHandTarget->SetSolverInterface(this);
}
void ApxActorBase::ReleaseHandIK()
{	
	if (m_pLHandTarget)
		m_pLHandTarget->SetSolverInterface(NULL);
	if (m_pRHandTarget)
		m_pRHandTarget->SetSolverInterface(NULL);
	A3DRELEASE(m_pLHandTarget);
	A3DRELEASE(m_pRHandTarget);
	m_pCarryable = NULL;
	m_pPushable = NULL;
}

bool ApxActorBase::TryToCarry(CCarryable* pCarryable, const TCHAR* strAnimation, float fBindFrameRatio, const TCHAR* strLChild, const TCHAR* strRChild)
{
	if (m_pLHandTarget == NULL || m_pRHandTarget == NULL || m_pCarryable)
		return false;

	const IPhysXObjBase* pCarryStuff = pCarryable->GetObject();
	if (0 == pCarryStuff)
		return false;
	if (m_pObjDyn == pCarryStuff)
		return false;
	if (0 != m_pObjDyn->GetRelationshipCode(*pCarryStuff))
		return false;

	if (m_pPushable)
		RelasePushable(0, 0);

	A3DSkinModel* pSkinModel = m_pObjDyn->GetSkinModel();

	
	bool bRet = pCarryable->TryToCarry(pSkinModel, m_pLHandTarget->GetIKBone(), m_pRHandTarget->GetIKBone(), strAnimation, fBindFrameRatio, strLChild, strRChild);
	if (bRet)
	{
		m_pCarryable = pCarryable;
		A3DSkinModelActionCore* pAction = pSkinModel->GetAction(_TWC2AS(strAnimation));
		DWORD dwTime = 0;
		if (pAction)
			dwTime = (DWORD)(pAction->GetActionTime() * fBindFrameRatio);
		m_pLHandTarget->StartCarry(dwTime);
		m_pRHandTarget->StartCarry(dwTime);
	}
	return bRet;
}
bool ApxActorBase::TryToThrow(const char* strAnimation, float fReadyRatio, float fThrowRatio, float fSpeed)
{
	bool bRet = false;
	if (m_pCarryable)
	{
		bRet = m_pCarryable->TryToThrow(strAnimation, fReadyRatio, fThrowRatio, fSpeed);
		if (bRet)
		{
			DWORD dwResistanceTime = 0, dwTansTime = 0;
			if (strAnimation)
			{
				A3DSkinModelActionCore* pAction = m_pObjDyn->GetSkinModel()->GetAction(strAnimation);

				float fUnBindFrameRatio = fReadyRatio + fThrowRatio;
				a_Clamp(fUnBindFrameRatio, 0.f, 1.f);
				if (pAction)
					dwResistanceTime = (DWORD)(pAction->GetActionTime() * fUnBindFrameRatio);
				dwTansTime = (DWORD)pAction->GetActionTime() - dwResistanceTime;
			}
			else
			{
				m_pLHandTarget->EnableIK(false);
				m_pRHandTarget->EnableIK(false);
			}
			
			m_pLHandTarget->StartThrow(dwTansTime, dwResistanceTime);
			m_pRHandTarget->StartThrow(dwTansTime, dwResistanceTime);
		}
	}
	return bRet;
}

bool ApxActorBase::PreSolver()
{
	if (m_pCarryable && m_pCarryable->GetState() != CCarryable::Free)
	{
		m_pCarryable->UpdateBindPosDir();

		A3DVECTOR3 vLPos, vRPos;		
		bool bRet = m_pCarryable->GetBindPosWorld(vLPos, vRPos);
		if (bRet)
		{
			m_pLHandTarget->SetIKGoalPos(vLPos);
			m_pRHandTarget->SetIKGoalPos(vRPos);
		}
	}
	if (m_pPushable && m_pPushable->IsPushing())
	{
		m_pPushable->UpdateBindPosDir();
		A3DVECTOR3 vLPos, vRPos;
		bool bRet = m_pPushable->GetBindPosWorld(vLPos, vRPos);
		if (bRet)
		{
			m_pLHandTarget->SetIKGoalPos(vLPos);
			m_pRHandTarget->SetIKGoalPos(vRPos);
		}
	}
	return IKSolverArmInterface::PreSolver();
}

bool ApxActorBase::PostRefreshSkeleton()
{
	if (m_pCarryable && m_pCarryable->GetState() != CCarryable::Free)
	{

		A3DVECTOR3 vLDir, vRDir;
		bool bRet = m_pCarryable->GetBindDirWorld(vLDir, vRDir);
		if (bRet)
		{
			bool bHold = m_pCarryable->GetState() == CCarryable::Hold;

			m_pLHandTarget->SetHandDir(vLDir, bHold);
			m_pRHandTarget->SetHandDir(vRDir, bHold);
		}
	}
	if (m_pPushable)
	{
		A3DVECTOR3 vLDir, vRDir;
		bool bRet = m_pPushable->GetBindDirWorld(vLDir, vRDir);
		if (bRet)
		{
			m_pLHandTarget->SetHandDir(vLDir, A3DVECTOR3(0, 1, 0));
			m_pRHandTarget->SetHandDir(vRDir, A3DVECTOR3(0, 1, 0));
		}
	}
	return true;
}

bool ApxActorBase::TryToPush(Pushable* pPushable, const char* strAnimation, float fBindFrameRatio)
{
	if (m_pLHandTarget == NULL || m_pRHandTarget == NULL || m_pCarryable)
		return false;

	A3DSkinModel* pSkinModel = m_pObjDyn->GetSkinModel();

	if (pPushable->TryToPush(pSkinModel, m_pLHandTarget->GetIKBone(), m_pRHandTarget->GetIKBone(), strAnimation, fBindFrameRatio))
	{
		m_pPushable = pPushable;
		A3DSkinModelActionCore* pAction = pSkinModel->GetAction(strAnimation);
		DWORD dwTime = 0;
		if (pAction)
			dwTime = (DWORD)(pAction->GetActionTime() * fBindFrameRatio);
		m_pLHandTarget->StartCarry(dwTime);
		m_pRHandTarget->StartCarry(dwTime);
		return true;
	}
	return false;
}

bool ApxActorBase::RelasePushable(float fTransTime, float fKeepIKTime)
{
	if (m_pPushable)
	{
		m_pPushable->ReleasePush();
		//m_pPushable = NULL;

		DWORD dwTransTime = (DWORD)(fTransTime * 1000);
		DWORD dwKeepIKTime = (DWORD)(fKeepIKTime * 1000);
		m_pLHandTarget->StartThrow(dwTransTime, dwKeepIKTime);
		m_pRHandTarget->StartThrow(dwTransTime, dwKeepIKTime);

		return true;
	}
	return false;
}

void ApxActorBase::GetGPose(A3DMATRIX4& mtPose) const
{
	m_pObjDyn->GetGPose(mtPose);
}

bool ApxActorBase::OnAttackingCheckHit(const ApxAttackActionInfo& apxAttActInfo)
{
	if (m_pUserActionReport)
		return m_pUserActionReport->OnAttackingCheckHit(apxAttActInfo);
	return GetObjDyn()->OnAttackingCheckHit(apxAttActInfo);
}

void ApxActorBase::OnAttackingHitEventPostNotify(const ApxDamageInfo& apxDamageInfo, IAssailable* pTarget, bool bTargetIsEnable)
{
	if (m_pUserActionReport)
		m_pUserActionReport->OnAttackingHitEventPostNotify(apxDamageInfo, pTarget, bTargetIsEnable); 
}

bool ApxActorBase::OnPreAttackedQuery()
{
	if (m_pUserActionReport)
		return m_pUserActionReport->OnPreAttackedQuery();
	return GetObjDyn()->OnPreAttackedQuery();
}

void ApxActorBase::OnPreAttackedNotify(ApxAttackActionInfo& inoutAAAI, const PhysXRBDesc* pAttacker)
{
	if (m_pUserActionReport)
	{
		m_pUserActionReport->OnPreAttackedNotify(inoutAAAI, pAttacker);
		return;
	}
	GetObjDyn()->OnPreAttackedNotify(inoutAAAI, pAttacker);
}

bool ApxActorBase::OnAttackedHitEvent(const ApxDamageInfo& apxDamageInfo)
{
	bool bRtn = false;
	if (m_pUserActionReport)
		bRtn = m_pUserActionReport->OnAttackedHitEvent(apxDamageInfo); 
	else
		bRtn = GetObjDyn()->OnAttackedHitEvent(apxDamageInfo);

	if (!bRtn)
		SetEnableTick(false);   
	return bRtn;
}

void ApxActorBase::SetHitTarget(IAssailable*	pObj, float fLockTime)
{
	if (pObj)
	{
		m_TargetInfo.pObj = pObj;
		if (fLockTime < 0)
			fLockTime = 0.001f;
		m_TargetInfo.LockTime.SetPeriod((DWORD)(fLockTime * 1000));
		m_TargetInfo.LockTime.Reset();
		m_TargetInfo.LockTime.SetPause(false);
		m_TargetInfo.bHasTarget = true;
		A3DMATRIX4 matPose;
		pObj->GetGPose(matPose);
		m_TargetInfo.vPos = Mat44_GetTransPart(matPose);

		UpdateRotateBody(m_pGroundMoveController->GetMoveDir());

		for (int i = 0; i < ApxActionLayer_Num; i++)
		{
			m_ActionControllerPtrs[i]->SetUseWalkRun(false);
		}
	}
	else
		ResetHitTarget();
}
void ApxActorBase::ResetHitTarget()
{
	m_TargetInfo.Reset();
	for (int i = 0; i < ApxActionLayer_Num; i++)
	{
		m_ActionControllerPtrs[i]->SetUseWalkRun(true);
	}

	NxVec3 vUp;
	m_pObjDyn->GetGDirAndUp(m_vHeadFaceDir, vUp);
	m_vHeadFaceDir.y = 0;
	m_vHeadFaceDir.normalize();
	m_pGroundMoveController->SetHeadDir(m_vHeadFaceDir);
}

bool ApxActorBase::GetHitTargetPos(NxVec3& vPos) const
{
	if (m_TargetInfo.bHasTarget)
	{
		vPos = APhysXConverter::A2N_Vector3(m_TargetInfo.vPos);
		return true;
	}
	return false;
}

void ApxActorBase::TurnTestDir(float fDeg)
{
	fDeg = glb_VectorToYaw(m_vTestDir) + fDeg;
	m_vTestDir = glb_YawToVector(fDeg);
}

void ApxActorBase::SetGroundMoveDirWithNoTurn(const NxVec3& vMoveDir)
{
	if(m_pGroundMoveController)
	{
		m_pGroundMoveController->RaiseFlag(ApxActorMoveController::ApxActor_MoveFlag_Driven);
		m_bSetCurPosToTargetPos = true;
	}


	NxVec3 vDir(vMoveDir);
	if (!m_vGroundTargetDir.isZero())
	{
		if (vMoveDir.isZero())
			vDir = m_vGroundTargetDir;
		else
			m_vGroundTargetDir.zero();
	}

	if (0 == m_pGroundMoveController)
		return;

	if (m_pGroundMoveController->IsInSwing())
	{
		m_vGroundTargetDir.zero();
		m_pGroundMoveController->SetMoveDir(vMoveDir);
		return;
	}

	if(vDir.y != 0.0f)
	{
		vDir.y = 0.0;
		vDir.normalize();
	}
	m_pGroundMoveController->SetMoveDir(vDir);

	m_bHasTestDir = true;
}

void ApxActorBase::SetUseBodyCtrl(bool bUseBodyCtrl)
{ 
	m_bUseBodyCtrl = bUseBodyCtrl; 
	if (!m_bUseBodyCtrl && m_pBodyCtrl) 
		m_pBodyCtrl->ResetBoneCtrl(); 
}

bool ApxActorBase::SetBendBodyEnable(bool bEnable)
{
	if (IsUpperBoneCtrlFree())
	{
		m_BendBodyEnable = bEnable;
		return true;
	}
	return false;
}

bool ApxActorBase::IsUpperBoneCtrlFree() const
{
	if (m_pUpperBodyCtrl)
		return m_pUpperBodyCtrl->GetState() == UpperBodyCtrl::Free;

	return true;
}

void ApxActorBase::BendBody(const A3DVECTOR3& vDir, float fBendDeg, float fBendTime, float fReTime)
{
	if (m_pUpperBodyCtrl)
		m_pUpperBodyCtrl->BendBody(vDir, fBendDeg, fBendTime, fReTime);
}

void ApxActorBase::SetBlendAction(const char* strLAction, const char* strLInvAction, const char* strRAction, const char* strRInvAction,
								  const char* strFAction, const char* strBAction, const char* strFWalk)
{
	for (int i = 0; i < ApxActionLayer_Num; i++)
	{
		m_ActionControllerPtrs[i]->SetActionName(strLAction, strLInvAction, strRAction, strRInvAction, 
			strFAction, strBAction, strFWalk);
	}
}
//////////////////////////////////////////////////////////////////////////
void ApxActorBase::TARGET_INFO::Tick(float fDeltaTime)
{
	if (!bHasTarget)
	{
		return;
	}
	A3DMATRIX4 mat(A3DMATRIX4::IDENTITY); 
	pObj->GetGPose(mat);
	vPos = Mat44_GetTransPart(mat);

	if (!LockTime.IsFull())
	{
		DWORD dwRealTime = (DWORD)(fDeltaTime * 1000);
		LockTime.IncCounter(dwRealTime);
	}
	else
	{
		LockTime.Reset();
		LockTime.SetPause(true);
		Reset();
	}
}

void ApxActorBase::SwitchFootIKYawCorrection()
{
#ifndef _ANGELICA3
	if(m_pLFIKSolver && m_pRFIKSolver)
	{
		m_pLFIKSolver->EnableFootYawCorrection(!m_pLFIKSolver->IsFootYawCorrectionEnabled());
		m_pRFIKSolver->EnableFootYawCorrection(!m_pRFIKSolver->IsFootYawCorrectionEnabled());
	}
#endif
}

bool ApxActorBase::IsIKYawCorrectionEnabled()
{
#ifndef _ANGELICA3
	if(m_pLFIKSolver && m_pRFIKSolver)
	{
		return m_pLFIKSolver->IsFootYawCorrectionEnabled();
	}
#endif
	return false;
}

void ApxActorBase::IncreFootIKMaxPitchDeg()
{
#ifndef _ANGELICA3
	mFootIKMaxPitchDeg += 2.0f;
	a_ClampRoof(mFootIKMaxPitchDeg, 90.0f);

	if(m_pLFIKSolver && m_pRFIKSolver)
	{
		m_pLFIKSolver->SetMaxPitchCorrectionDegree(mFootIKMaxPitchDeg);
		m_pRFIKSolver->SetMaxPitchCorrectionDegree(mFootIKMaxPitchDeg);
	}	
#endif
}

void ApxActorBase::DecreFootIKMaxPitchDeg()
{
#ifndef _ANGELICA3
	mFootIKMaxPitchDeg -= 2.0f;
	a_ClampFloor(mFootIKMaxPitchDeg, 0.0f);

	if(m_pLFIKSolver && m_pRFIKSolver)
	{
		m_pLFIKSolver->SetMaxPitchCorrectionDegree(mFootIKMaxPitchDeg);
		m_pRFIKSolver->SetMaxPitchCorrectionDegree(mFootIKMaxPitchDeg);
	}	
#endif
}

#ifdef _ANGELICA21
void ApxActorBase::UpdateTransIKOffset()
{
	if (0 == m_pLFIKSolver)
		return;
	if (0 == m_pRFIKSolver)
		return;

	A3DBone* pFootBone = m_pObjDyn->GetSkinModel()->GetSkeleton()->GetBone(m_pLFIKSolver->GetEEJoint()->iBoneIdx);
	if (m_bUseIKFrame && pFootBone->IsInTransition())
	{
		if (IsIKEnabled() && m_pLFIKSolver->IsEnabled() && m_pRFIKSolver->IsEnabled())
		{

			float fTrans = 0.2f;

			int ichannel = -1; 
			for(int i = 0; i < ApxActionLayer_Num; i++)
			{

				if (i == ApxActionLayer_Passive)
					continue;

				if (m_ActionControllerPtrs[i]->GetActiveChannel() & ApxActionChannel_LowerBody)
				{
					ichannel = ((ApxSkinModelActionController*)m_ActionControllerPtrs[i])->GetActionChannel(ApxActionChannel_LowerBody);
					break;
				}
			}
			if (ichannel == -1)
				return;

			A3DFootIKOffsetPara fikoPara;
			GetGPose(fikoPara.mSkinModelPose);
			fikoPara.mUpDist = 0.1f;
			fikoPara.mDownDist = 1.5f;

			A3DFootIKOffsetInfo lfInfo, rfInfo;
			float fLOffset = m_pLFIKSolver->GetIKAdjustOffsetY(ichannel, fTrans, lfInfo, &fikoPara);
			float fROffset = m_pRFIKSolver->GetIKAdjustOffsetY(ichannel, fTrans, rfInfo, &fikoPara);
			float fOffset = 0.0f;
			if(lfInfo.mReachGoal && rfInfo.mReachGoal)
				fOffset = min2(fLOffset, fROffset);
			else if(lfInfo.mReachGoal)
				fOffset = fLOffset;
			else if(rfInfo.mReachGoal)
				fOffset = fROffset;
			else
				return;	

			if( fOffset < 0.5f && fOffset > IK_ADJUST_EPS 
				|| fOffset > -0.5f && fOffset < -IK_ADJUST_EPS) 
			{

				fOffset = m_FootOffBlender.GetValue() + fOffset * 1.0f;		
				m_FootOffBlender.SetTargetValue(fOffset, fTrans);

			}
		}
	}
}

void ApxActorBase::SwitchToPhys(const char* strBone, float fPhyWeight, bool bIncChild, bool bChangePhyState)
{
	int index = -1;
	A3DBone* pBone = NULL;
	if (strBone != NULL)
		pBone = m_pObjDyn->GetECModel()->GetA3DSkinModel()->GetSkeleton()->GetBone(strBone, &index);
	if (pBone)
		m_pObjDyn->GetECModel()->GetPhysSync()->SwitchToPhys(pBone, fPhyWeight, bIncChild, bChangePhyState);
}

const char* ApxActorBase::GetParentBone(const char* strBone)
{
	int index = -1;
	A3DBone* pBone = m_pObjDyn->GetSkinModel()->GetSkeleton()->GetBone(strBone, &index);
	if (pBone == NULL)
		return NULL;

	pBone = pBone->GetParentPtr();
	if (pBone)
		return pBone->GetName();
	return NULL;
}
const char* ApxActorBase::GetFirstChildBone(const char* strBone)
{
	int index = -1;
	A3DBone* pBone = m_pObjDyn->GetSkinModel()->GetSkeleton()->GetBone(strBone, &index);
	if (pBone == NULL)
		return NULL;

	if (pBone->GetChildNum() == 0)
		return NULL;

	pBone = pBone->GetChildPtr(0);
	if (pBone)
		return pBone->GetName();
	return NULL;
}
const char* ApxActorBase::GetNextSiblingBone(const char* strBone)
{
	int index = -1;
	A3DBone* pBone = m_pObjDyn->GetSkinModel()->GetSkeleton()->GetBone(strBone, &index);
	if (pBone == NULL)
		return NULL;

	A3DBone* pParent = pBone->GetParentPtr();
	if (pParent)
	{
		int i;
		for (i = 0; i < pParent->GetChildNum(); i++)
		{
			if (pBone == pParent->GetChildPtr(i))
				break;
		}
		i++;
		if (i < pParent->GetChildNum())
			return pParent->GetChildPtr(i)->GetName();
		return pParent->GetChildPtr(0)->GetName();
	}
	return NULL;
}

#endif