#include "Stdafx.h"
#include "MActorBase.h"
#include "ScriptFactory.h"
#include "MCameraController.h"
#include "MLogWrapper.h"
#include "MCarryable.h"
#include "MPushable.h"

using namespace System;

namespace APhysXCommonDNet
{
	MAttackActionInfo::MAttackActionInfo(const ApxAttackActionInfo& AAAInfo) : MActionInfo(AAAInfo)
	{
		mAttack = gcnew MAttackInfo;
		MActorBase::ConvertAttackInfo(AAAInfo.mApxAttack, mAttack);
		mDamage = gcnew MDamageInfo;
		MActorBase::ConvertDamageInfo(AAAInfo.mApxDamage, mDamage);
	}

	void MAttackActionInfo::CopyToApxActionInfo(ApxActionInfo* pInOutInfo)
	{
		if (0 == pInOutInfo)
			return;

		MActionInfo::CopyToApxActionInfo(pInOutInfo);
		ApxAttackActionInfo* pAAAI = pInOutInfo->IsAttackActionInfo();
		if (0 == pAAAI)
			return;

		MActorBase::ConvertAttackInfo(mAttack, pAAAI->mApxAttack);
		MActorBase::ConvertDamageInfo(mDamage, pAAAI->mApxDamage);
	}

	MComboAttackActionInfo::MComboAttackActionInfo(const ApxComboAttackActionInfo& ACAAInfo) : MAttackActionInfo(ACAAInfo)
	{
		mComboPlayTime = ACAAInfo.mComboPlayTime;
		mComboEnhanceFactor = ACAAInfo.mComboEnhanceFactor;
		mSeqAnim = gcnew array<MActionAnimInfo^>(2);
		mSeqAnim[0] = gcnew MActionAnimInfo(ACAAInfo.mSeqAnim[0]);
		mSeqAnim[1] = gcnew MActionAnimInfo(ACAAInfo.mSeqAnim[1]);
		mSeqActDamDelayStartSec = gcnew array<float>(2);
		mSeqActDamDelayStartSec[0] = ACAAInfo.mSeqActDamDelayStartSec[0];
		mSeqActDamDelayStartSec[1] = ACAAInfo.mSeqActDamDelayStartSec[1];
		mSeqActDamDurationSec = gcnew array<float>(2);
		mSeqActDamDurationSec[0] = ACAAInfo.mSeqActDamDurationSec[0];
		mSeqActDamDurationSec[1] = ACAAInfo.mSeqActDamDurationSec[1];
	}

	void MComboAttackActionInfo::CopyToApxActionInfo(ApxActionInfo* pInOutInfo)
	{
		if (0 == pInOutInfo)
			return;

		MAttackActionInfo::CopyToApxActionInfo(pInOutInfo);
		ApxComboAttackActionInfo* pACAAI = pInOutInfo->IsComboAttackActionInfo();
		if (0 == pACAAI)
			return;

		pACAAI->mComboPlayTime = mComboPlayTime;
		pACAAI->mComboEnhanceFactor = mComboEnhanceFactor;
		if(mSeqAnim[0] != nullptr)
			mSeqAnim[0]->CopyToApxActionAnimInfo(pACAAI->mSeqAnim[0]);
		pACAAI->mSeqActDamDelayStartSec[0] = mSeqActDamDelayStartSec[0];
		pACAAI->mSeqActDamDurationSec[0] = mSeqActDamDurationSec[0];
		if(mSeqAnim[1] != nullptr)
			mSeqAnim[1]->CopyToApxActionAnimInfo(pACAAI->mSeqAnim[1]);
		pACAAI->mSeqActDamDelayStartSec[1] = mSeqActDamDelayStartSec[1];
		pACAAI->mSeqActDamDurationSec[1] = mSeqActDamDurationSec[1];
	}

	MActorActionReport^ MActorActionReport::CreateInstance(System::String^ strActionFileName)
	{
		Object^ obj = ScriptFactory::Instance->CreateInstanceFromFile(strActionFileName);
		return static_cast<MActorActionReport^>(obj);
	}
	MActorActionReport::MActorActionReport()
	{
		m_pActorBase = nullptr;
	}

	void MActorBase::ConvertDamageInfo(MDamageInfo^ pIn, ApxDamageInfo& outADI)
	{
		outADI.mHitRBActor = (NxActor*)pIn->mHitRBActorPtr;
		outADI.mHitPos = APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(pIn->mHitPos));
		outADI.mHitDir = APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(pIn->mHitDir));
		outADI.mHitMomentum = pIn->mHitMomentum;
		outADI.mHitDamage = pIn->mHitDamage;
		outADI.mpAttacker = 0;
		if (nullptr != pIn->mpAttacker)
		{
			MActorBase^ pMAB = pIn->mpAttacker->GetActorBase();
			if (nullptr != pMAB)
				outADI.mpAttacker = pMAB->GetAAB();
			else
				outADI.mpAttacker = pIn->mpAttacker->GetIAssailable();
		}
		outADI.mSourceFlags = pIn->mSourceFlags;
	}
	void MActorBase::ConvertAttackInfo(MAttackInfo^ pIn, ApxAttackInfo& outAAI)
	{    
		outAAI.mIsPhysXDetection = pIn->mIsPhysXDetection;
		outAAI.mAttackType = pIn->mAttackType;
		outAAI.mTeamGroup = pIn->mTeamGroup;
		outAAI.mAttackRange = pIn->mAttackRange;
		outAAI.mAttHalfRadianRange = pIn->mAttHalfRadianRange;
		outAAI.mDamageDelayStartSec = pIn->mDamageDelayStartSec;
		outAAI.mDamageDurationSec = pIn->mDamageDurationSec;
		if (nullptr != pIn->mpTarget)
			outAAI.mpTarget = pIn->mpTarget->GetIAssailable();
		else
			outAAI.mpTarget = 0;
	}
	void MActorBase::ConvertDamageInfo(const ApxDamageInfo& in, MDamageInfo^ pOutMDI)
	{
		pOutMDI->mHitRBActorPtr = in.mHitRBActor;
		pOutMDI->mHitPos = Vector3::FromA3DVECTOR3(APhysXConverter::N2A_Vector3(in.mHitPos));
		pOutMDI->mHitDir = Vector3::FromA3DVECTOR3(APhysXConverter::N2A_Vector3(in.mHitDir));
		pOutMDI->mHitMomentum = in.mHitMomentum;
		pOutMDI->mHitDamage = in.mHitDamage;
		pOutMDI->mpAttacker = nullptr;
		if (0 != in.mpAttacker)
		{
			pOutMDI->mpAttacker = MScene::Instance->GetMActorBase(in.mpAttacker->GetActorBase());
			if (nullptr == pOutMDI->mpAttacker)
				pOutMDI->mpAttacker = MScene::Instance->GetMIAssailable(in.mpAttacker);
		}
		pOutMDI->mSourceFlags = in.mSourceFlags;
	}
	void MActorBase::ConvertAttackInfo(const ApxAttackInfo& in, MAttackInfo^ pOutMAI)
	{
		pOutMAI->mIsPhysXDetection = in.mIsPhysXDetection;
		pOutMAI->mAttackType = in.mAttackType;
		pOutMAI->mTeamGroup = in.mTeamGroup;
		pOutMAI->mAttackRange = in.mAttackRange;
		pOutMAI->mAttHalfRadianRange = in.mAttHalfRadianRange;
		pOutMAI->mDamageDelayStartSec = in.mDamageDelayStartSec;
		pOutMAI->mDamageDurationSec = in.mDamageDurationSec;
		pOutMAI->mpTarget = MScene::Instance->GetMIAssailable(in.mpTarget);
	}

	MActorBase::MActorBase(ApxActorBase* pActorBase, MIObjBase^ pObjBase)
	{
		assert(0 != pActorBase);
		m_bHostActor = false;
		m_pActorBase = pActorBase;
		m_pActorReport = new ActorActionReport();
		m_pActorBase->SetGroundMoveSpeed(1.5f);
		m_pActorBase->SetUserActionReport(m_pActorReport);

		m_pObjBase = pObjBase;
		m_pMActorReport = nullptr;

		m_pCarryable = nullptr;
		m_pPushable = nullptr;
		m_pHitTarget = nullptr;

		m_bHandInHand = false;
	}
	void MActorBase::SetToDefault()
	{
		if (m_pActorBase)
			m_pActorBase->SetToDefault();
	}
	MActorBase::!MActorBase()
	{
		delete m_pActorReport;
		m_pActorReport = 0;
	}
	MActorBase::~MActorBase()
	{
		this->!MActorBase();
	}
	void MActorBase::RegisterKeyEvent()
	{
		if (m_pMActorReport != nullptr)
			MKeyEventMgr::Instance->Register(m_pMActorReport);
	}
	void MActorBase::UnRegisterKeyEvent()
	{
		if (m_pMActorReport != nullptr)
			MKeyEventMgr::Instance->UnRegister(m_pMActorReport);
	}
	void MActorBase::SetPos(Vector3 vPos)
	{
		NxVec3 pos = APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(vPos)); 
		m_pActorBase->SetPos(pos);
	}
	Vector3 MActorBase::GetPos()
	{
		NxVec3 vPos = m_pActorBase->GetPos();
		return Vector3::FromA3DVECTOR3(APhysXConverter::N2A_Vector3(vPos));
	}
	void MActorBase::SetExtents(Vector3 vExtents)
	{
		NxVec3 ext = APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(vExtents));
		m_pActorBase->SetExtents(ext);
	}
	Vector3 MActorBase::GetExtents()
	{
		NxVec3 ext = m_pActorBase->GetExtents();
		return Vector3::FromA3DVECTOR3(APhysXConverter::N2A_Vector3(ext));
	}
	void MActorBase::SetGroundMoveDir(Vector3 vMoveDir)
	{
		NxVec3 vDir = APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(vMoveDir));
		m_pActorBase->SetGroundMoveDir(vDir);
	}
	Vector3 MActorBase::GetGroundMoveDir()
	{
		NxVec3 dir = m_pActorBase->GetGroundMoveController()->GetMoveDir();
		return Vector3::FromA3DVECTOR3(APhysXConverter::N2A_Vector3(dir));
	}
	void MActorBase::SetGroundMoveSpeed(float fSpeed)
	{
		m_pActorBase->SetGroundMoveSpeed(fSpeed);
	}
	float MActorBase::GetGroundMoveSpeed()
	{
		return m_pActorBase->GetGroundMoveController()->GetMoveSpeed();
	}
	void MActorBase::GroundJump(float fUpSpeed)
	{
		m_pActorBase->GroundJump(fUpSpeed);
	}
	void MActorBase::SetGroundTargetPos(Vector3 vTargetPos)
	{
		NxVec3 vTar = APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(vTargetPos));
		m_pActorBase->SetGroundTargetPos(vTar);
	}
	bool MActorBase::IsGroundTargetPosReached()
	{
		return m_pActorBase->IsGroundTargetPosReached();
	}
	void MActorBase::SetHeadDir(Vector3 vHeadDir)
	{
		NxVec3 vTar = APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(vHeadDir));
		m_pActorBase->SetHeadDir(vTar);
	}
	void MActorBase::SetHeadDir(Vector3 vHeadDir, bool bForce)
	{
		NxVec3 vTar = APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(vHeadDir));
		m_pActorBase->SetHeadDir(vTar, bForce);
	}
	Vector3 MActorBase::GetHeadDir()
	{
		NxVec3 vDir = m_pActorBase->GetHeadDir();
		return Vector3::FromA3DVECTOR3(APhysXConverter::N2A_Vector3(vDir));
	}
	MPhysXObjDynamic^ MActorBase::GetObjDyn()
	{
		return dynamic_cast<MPhysXObjDynamic^>(m_pObjBase);
	}
	bool MActorBase::PlayAction(MActionLayer layer, MActionInfo^ ActionInfo)
	{
		return m_pActorBase->PlayAction(static_cast<ApxActionLayer>(layer), *ActionInfo->GetApxActionInfo());
	}
	void MActorBase::SwitchToRagDoll(MDamageInfo^ pDamageInfo)
	{
		ApxDamageInfo adi;
		ConvertDamageInfo(pDamageInfo, adi);
		m_pActorBase->SwitchToRagDoll(adi);
	}
	void MActorBase::CloseRagDoll()
	{
		m_pActorBase->CloseRagDoll();
	}
	void MActorBase::RebornNotify()
	{
		m_pActorBase->SetEnableTick(true);
		if (m_pMActorReport)
			m_pMActorReport->OnRebornNotify();
	}

	MSkinModelSocketType MActorBase::ConvertSockeyType(ApxSkinModelSocketType st)
	{
		if (ApxSM_SocketType_Bone == st)
			return MSkinModelSocketType::MApxSM_SocketType_Bone;
		if (ApxSM_SocketType_Hook == st)
			return MSkinModelSocketType::MApxSM_SocketType_Hook;
		assert(!"Shouldn't be here!");
		return MSkinModelSocketType::MApxSM_SocketType_Bone;
	}
	ApxSkinModelSocketType MActorBase::ConvertSockeyType(MSkinModelSocketType st)
	{
		if (MSkinModelSocketType::MApxSM_SocketType_Bone == st)
			return ApxSM_SocketType_Bone;
		if (MSkinModelSocketType::MApxSM_SocketType_Hook == st)
			return ApxSM_SocketType_Hook;
		assert(!"Shouldn't be here!");
		return ApxSM_SocketType_Bone;
	}
	bool MActorBase::ChangeEquipment(MWeaponInfo^ newWeapon)
	{
		ApxWeaponInfo* pAWI = 0;
		ApxWeaponInfo awi;
		if (nullptr != newWeapon)
		{
			pin_ptr<const wchar_t> wn = PtrToStringChars(newWeapon->mWeaponName);
			if (nullptr != wn)
				awi.strWeaponPathName = _TWC2TSting(wn);
			awi.smsHostSocketType = ConvertSockeyType(newWeapon->mHostSocketType);
			awi.smsWPNSocketType = ConvertSockeyType(newWeapon->mWPNSocketType);
			pin_ptr<const wchar_t> hsn = PtrToStringChars(newWeapon->mHostSocketName);
			if (nullptr != hsn)
				awi.strHostSocketName = _TWC2TSting(hsn);
			pin_ptr<const wchar_t> wsn = PtrToStringChars(newWeapon->mWPNSocketName);
			if (nullptr != wsn)
				awi.strWPNSocketName = _TWC2TSting(wsn);

			awi.bUsingXAJHWay = newWeapon->mbUsingXAJHWay;
			pin_ptr<const wchar_t> wnn = PtrToStringChars(newWeapon->mWPNECMNickName);
			if (nullptr != wnn)
				awi.strWPNECMNickName = _TWC2TSting(wnn);
			pin_ptr<const wchar_t> wa = PtrToStringChars(newWeapon->mWPNAttacher);
			if (nullptr != wa)
				awi.strWPNAttacher = _TWC2TSting(wa);
			pAWI = &awi;
		}
		Scene* pScene = MScene::Instance->GetScene();
		return m_pActorBase->ChangeEquipment(pAWI, pScene->GetAPhysXScene(), &pScene->GetLoadPackage());
	}
	MWeaponInfo^ MActorBase::GetCurrentEquipment()
	{
		const ApxWeaponInfo* pAWI = m_pActorBase->GetCurrentEquipment();
		if (0 == pAWI)
			return nullptr;

		MWeaponInfo^ mwi = gcnew MWeaponInfo;
		mwi->mWeaponName = gcnew System::String(pAWI->strWeaponPathName);
		mwi->mHostSocketType = ConvertSockeyType(pAWI->smsHostSocketType);
		mwi->mWPNSocketType  = ConvertSockeyType(pAWI->smsWPNSocketType);
		mwi->mHostSocketName = gcnew System::String(pAWI->strHostSocketName);
		mwi->mWPNSocketName  = gcnew System::String(pAWI->strWPNSocketName);

		mwi->mbUsingXAJHWay = pAWI->bUsingXAJHWay;
		mwi->mWPNECMNickName  = gcnew System::String(pAWI->strWPNECMNickName);
		mwi->mWPNAttacher  = gcnew System::String(pAWI->strWPNAttacher);
		return mwi;
	}
	System::String^ MActorBase::GetFirstActionName()
	{
		const ApxActionController* pAAC = m_pActorBase->GetActionController(ApxActionLayer_Active);
		if (0 == pAAC)
			return nullptr;

		const char* pName = pAAC->GetFirstActionName();
		if (0 == pName)
			return nullptr;
		return gcnew System::String(pName);
	}

	System::String^ MActorBase::GetNextActionName()
	{
		const ApxActionController* pAAC = m_pActorBase->GetActionController(ApxActionLayer_Active);
		if (0 == pAAC)
			return nullptr;

		const char* pName = pAAC->GetNextActionName();
		if (0 == pName)
			return nullptr;
		return gcnew System::String(pName);
	}

	void MActorBase::SetMActionReport(MActorActionReport^ pActionReport)
	{
		m_pActorReport->SetActorActionReport(pActionReport);
		m_pMActorReport = pActionReport;		
		pActionReport->HostActor = this;
		pActionReport->Init(m_pObjBase);
	}

	void MActorBase::EnterCameraBinding(bool bIsKeepDir)
	{
		CamOptTask_EntryBindingMode ot(m_pActorBase->GetObjDyn(), false, bIsKeepDir);
		ot.m_bStorePose = false;
		ot.m_bIsKeepDir = bIsKeepDir;
		MScene::Instance->GetScene()->GetCamCtrlAgent()->ExecuteCameraOperation(ot);
		SetHostActor(true);
	}

	MGroundMoveState MActorBase::GetGroundMoveState()
	{
		const ApxActorGroundMoveController* pMoveController = m_pActorBase->GetGroundMoveController();
		if (pMoveController != NULL)
		{
			return static_cast<MGroundMoveState>(pMoveController->GetGroundMoveState());
		}
		return MGroundMoveState::MActor_GroundMoveState_StandInPlace;
	}

	Vector3 MActorBase::GetInputMoveDir(MUserInputFlag muifs)
	{
		Vector3 vCamHDir = MScene::Instance->GetMNormalCamCtrler()->GetDirH();
		NxVec3 camHDir(vCamHDir.X, vCamHDir.Y, vCamHDir.Z);
		NxVec3 dir = m_pActorBase->GetInputMoveDir(static_cast<UserInputFlag>(muifs), camHDir);
		Vector3 rtnDir(dir.x, dir.y, dir.z);
		return rtnDir;
	}

	//@parameter strLFoot: bone name of left foot
	//           strRFoot: bone name of right foot
	bool MActorBase::InitFootIK(System::String^ strLFoot, System::String^ strRFoot)
	{
		pin_ptr<const wchar_t> wchl = PtrToStringChars(strLFoot);
		AString astrLFoot = WC2AS(wchl);
		pin_ptr<const wchar_t> wchr = PtrToStringChars(strRFoot);
		AString astrRFoot = WC2AS(wchr);

		return m_pActorBase->InitFootIK(astrLFoot, astrRFoot);
	}
	bool MActorBase::ReleaseFootIK()
	{
		return m_pActorBase->ReleaseFootIK();
	}
	
	//enable & disable foot IK
	void MActorBase::EnableFootIK(bool bLFoot, bool bRFoot)
	{
		m_pActorBase->EnableFootIK(bLFoot, bRFoot);
	}

	bool MActorBase::InitWalkRunBlend(System::String^ strWalk, System::String^ strRun)
	{
		pin_ptr<const wchar_t> wchl = PtrToStringChars(strWalk);
		AString astrWalk = WC2AS(wchl);
		pin_ptr<const wchar_t> wchr = PtrToStringChars(strRun);
		AString astrRun = WC2AS(wchr);
		return m_pActorBase->InitWalkRunBlend(astrWalk, astrRun);
	}
	bool MActorBase::ReleaseWalkRunBlend()
	{
		return m_pActorBase->ReleaseWalkRunBlend();
	}

	void MActorBase::InitHandIK(System::String^ strLHand, System::String^ strRHand)
	{
		pin_ptr<const wchar_t> wchl = PtrToStringChars(strLHand);
		AString astrLHand = WC2AS(wchl);
		pin_ptr<const wchar_t> wchr = PtrToStringChars(strRHand);
		AString astrRHand = WC2AS(wchr);

		m_pActorBase->InitHandIK(astrLHand, astrRHand);
	}
	void MActorBase::ReleaseHandIK()
	{
		m_pActorBase->ReleaseHandIK();
	}
	void MActorBase::EnableHandIK(bool bLH, bool bRH)
	{
//		m_pActorBase->EnableHandIK(bLH, bRH);
	}
	void MActorBase::SetHandGoal(Vector3 vGoal, bool bLH)
	{
//		m_pActorBase->SetHandGoal(Vector3::ToA3DVECTOR3(vGoal), bLH);
	}
	void MActorBase::TryHandInHandWith(MActorBase^ abCompanion)
	{
//		m_pActorBase->TryHandInHandWith(*abCompanion->GetAAB());
	}
	void MActorBase::SetHandExtents(Vector3 vHandExtents)
	{
//		m_pActorBase->SetHandExtents(Vector3::ToA3DVECTOR3(vHandExtents));
	}
	bool MActorBase::TryToCarry(MCarryable^ pCarryable, System::String^ strAnimation, float fBindFrameRatio, String^ strLChild, String^ strRChild)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(strAnimation);
		TString astrAnimation = _TWC2TSting(wch);
		TString astrLChild, astrRChild;
		const TCHAR* pStrLChild = NULL;
		const TCHAR* pStrRChild = NULL;
		if (strLChild != nullptr)
		{
			wch = PtrToStringChars(strLChild);
			astrLChild = _TWC2TSting(wch);
			pStrLChild = astrLChild;
		}
		if (strRChild != nullptr)
		{
			wch = PtrToStringChars(strRChild);
			astrRChild = _TWC2TSting(wch);
			pStrRChild = astrRChild;
		}

		bool bRet = m_pActorBase->TryToCarry(pCarryable->GetCarryable(), astrAnimation, fBindFrameRatio, pStrLChild, pStrRChild);

		if (bRet)
		{
			m_pCarryable = pCarryable;
			pCarryable->SetActorActionReport(m_pMActorReport);
		}
		return bRet;
	}
	bool MActorBase::TryToCarry(MCarryable^ pCarryable, System::String^strAnimation, float fBindFrameRatio)
	{
		return TryToCarry(pCarryable, strAnimation, fBindFrameRatio, nullptr, nullptr);
	}
	bool MActorBase::TryToThrow(System::String^ strAnimation, float fReadyRatio, float fThrowRatio, float fSpeed)
	{
		bool bRet = false;
		if (strAnimation != nullptr)
		{
			pin_ptr<const wchar_t> wch = PtrToStringChars(strAnimation);
			AString astrAnimation = WC2AS(wch);
			bRet = m_pActorBase->TryToThrow(astrAnimation, fReadyRatio, fThrowRatio, fSpeed);
		}
		else
		{
			bRet = m_pActorBase->TryToThrow(NULL, fReadyRatio, fThrowRatio, fSpeed);
		}
		
		return bRet;
	}

	bool MActorBase::TryToPush(MPushable^ pPushable, System::String^ strAnimation, float fBindFrameRatio)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(strAnimation);
		AString astrAnimation = WC2AS(wch);
		bool bRet = m_pActorBase->TryToPush(pPushable->GetPushable(), astrAnimation, fBindFrameRatio);
		if (bRet)
		{
			m_pPushable = pPushable;
		}
		return bRet;
	}
	bool MActorBase::RelasePushable(float fTransTime, float fKeepIKTime)
	{
		return m_pActorBase->RelasePushable(fTransTime, fKeepIKTime);
	}
	MIObjBase^ MActorBase::GetObject()
	{
		return GetObjDyn();
	}
	MActorBase^ MActorBase::GetActorBase()
	{
		return this;
	}
	Vector3	MActorBase::GetCurrentPos()
	{
		return GetPos();
	}
	Matrix4	MActorBase::GetGPose()
	{
		return GetObjDyn()->GetGPose();
	}
	bool MActorBase::OnPreAttackedQuery()
	{
		if (m_pMActorReport)
			return m_pMActorReport->OnPreAttackedQuery();
		return true;
	}
	void MActorBase::OnPreAttackedNotify(MAttackActionInfo^ pAttActInfo, MPhysXRBDesc^ pAttacker)
	{
		if (m_pMActorReport)
			m_pMActorReport->OnPreAttackedNotify(pAttActInfo, pAttacker);
	}
	bool MActorBase::OnAttackedPreHitEvent(MActorBase^ pCaster, int attackType, unsigned int idxCasterTeam)
	{
		if (m_pMActorReport)
			return m_pMActorReport->OnAttackedPreHitEvent(pCaster, attackType, idxCasterTeam);
		return true;
	}
	void MActorBase::SetHitTarget(MIAssailable^ pObj, float fLockTime)
	{
		if (pObj != nullptr)
		{
			m_pHitTarget = pObj;
			//IPhysXObjBase* pPhysXObj = dynamic_cast<IPhysXObjBase*>(pObj->GetIObjBase());
			m_pActorBase->SetHitTarget(pObj->GetIAssailable(), fLockTime);
		}
		else
			ResetHitTarget();
	}
	void MActorBase::ResetHitTarget()
	{
		m_pHitTarget = nullptr;
		m_pActorBase->ResetHitTarget();
	}
	MIAssailable^ MActorBase::GetHitTarget()
	{
		if (m_pActorBase->HasHitTarget())
			return m_pHitTarget;
		return nullptr;
	}
	void MActorBase::SetBlendAction(System::String^ strLAction, System::String^ strLInvAction, System::String^ strRAction, System::String^ strRInvAction,
		System::String^ strFAction, System::String^ strBAction, System::String^ strFWalk)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(strLAction);
		AString astrLAction = WC2AS(wch);
		
		wch = PtrToStringChars(strLInvAction);
		AString astrLInvAction = WC2AS(wch);

		wch = PtrToStringChars(strRAction);
		AString astrRAction = WC2AS(wch);

		wch = PtrToStringChars(strRInvAction);
		AString astrRInvAction = WC2AS(wch);

		wch = PtrToStringChars(strFAction);
		AString astrFAction = WC2AS(wch);

		wch = PtrToStringChars(strBAction);
		AString astrBAction = WC2AS(wch);

		wch = PtrToStringChars(strFWalk);
		AString astrFWalk = WC2AS(wch);

		m_pActorBase->SetBlendAction(astrLAction, astrLInvAction, astrRAction, astrRInvAction, astrFAction, astrBAction, astrFWalk);
	}

	void MActorBase::BendBody(Vector3 vDir, float fBendDeg, float fBendTime, float fReTime)
	{
		m_pActorBase->BendBody(Vector3::ToA3DVECTOR3(vDir), fBendDeg, fBendTime, fReTime);
	}

#ifdef _ANGELICA21
	void MActorBase::SwitchToPhys(System::String^ strBone, float fPhyWeight)
	{
		if (strBone != nullptr)
		{
			pin_ptr<const wchar_t> wch = PtrToStringChars(strBone);
			AString astrBone = WC2AS(wch);
			m_pActorBase->SwitchToPhys(astrBone, fPhyWeight);
		}
		else
			m_pActorBase->SwitchToPhys(NULL, fPhyWeight);
	}

	System::String^ MActorBase::GetParentBone(System::String^ strBone)
	{
		if (strBone == nullptr) return nullptr;

		pin_ptr<const wchar_t> wch = PtrToStringChars(strBone);
		AString astrBone = WC2AS(wch);

		const char* pStr = m_pActorBase->GetParentBone(astrBone);
		if (pStr)
			return gcnew String(pStr);
		return nullptr;
	}
	System::String^ MActorBase::GetFirstChildBone(System::String^ strBone)
	{
		if (strBone == nullptr) return nullptr;

		pin_ptr<const wchar_t> wch = PtrToStringChars(strBone);
		AString astrBone = WC2AS(wch);

		const char* pStr = m_pActorBase->GetFirstChildBone(astrBone);
		if (pStr)
			return gcnew String(pStr);
		return nullptr;
	}
	System::String^ MActorBase::GetNextSiblingBone(System::String^ strBone)
	{
		if (strBone == nullptr) return nullptr;

		pin_ptr<const wchar_t> wch = PtrToStringChars(strBone);
		AString astrBone = WC2AS(wch);

		const char* pStr = m_pActorBase->GetNextSiblingBone(astrBone);
		if (pStr)
			return gcnew String(pStr);
		return nullptr;
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	ActorActionReport::ActorActionReport()
	{
		m_pActorActionReport = nullptr;
	}
	ActorActionReport::~ActorActionReport()
	{
		
	}
	void ActorActionReport::SetActorActionReport(MActorActionReport^ pActionReport)
	{
		m_pActorActionReport = pActionReport;
	}
	void ActorActionReport::OnStartJump()
	{
		if (m_pActorActionReport)
			m_pActorActionReport->OnStartJump();
	}
	void ActorActionReport::OnLand()
	{
		if (m_pActorActionReport)
			m_pActorActionReport->OnLand();
	}
	void ActorActionReport::OnReachApex()
	{
		if (m_pActorActionReport)
			m_pActorActionReport->OnReachApex();
	}

	// for user's swing call back...
	void ActorActionReport::OnStartSwing()
	{
		if (m_pActorActionReport)
			m_pActorActionReport->OnStartSwing();
	}
	void ActorActionReport::OnEndSwing()
	{
		if (m_pActorActionReport)
			m_pActorActionReport->OnEndSwing();
	}

	// for user's push action call back...
	void ActorActionReport::OnPushObj(IPhysXObjBase* pPhysXObj)
	{
		if (m_pActorActionReport) 
		{
			MIPhysXObjBase^ pObj = dynamic_cast<MIPhysXObjBase^>(MScene::Instance->GetMIObjBase(pPhysXObj));
			m_pActorActionReport->OnPushObj(pObj);
		}
	}

	// for user's pre-land notification call back...
	void ActorActionReport::PreLandNotify(float fTimeBeforeLand)
	{
		if (m_pActorActionReport) 
			m_pActorActionReport->PreLandNotify(fTimeBeforeLand);
	}

	//----------------------------------------------
	// actor's action user report...
	void ActorActionReport::OnActionStart(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo)
	{
		if (m_pActorActionReport) 
			m_pActorActionReport->OnActionStart(static_cast<MActionLayer>(apxActionLayer), ConvertActionInfo(pActionInfo));
	}
	void ActorActionReport::OnActionEnd(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo)
	{
		if (m_pActorActionReport) 
			m_pActorActionReport->OnActionEnd(static_cast<MActionLayer>(apxActionLayer), ConvertActionInfo(pActionInfo));
	}

	void ActorActionReport::OnChangeEquipment()
	{
		if (m_pActorActionReport)  
			m_pActorActionReport->OnChangeEquipment();
	}
	void ActorActionReport::OnUpdateMoveState()
	{
		if (m_pActorActionReport) 
			m_pActorActionReport->OnUpdateMoveState();
	}
	bool ActorActionReport::OnAttackingCheckHit(const ApxAttackActionInfo& apxAttActInfo)
	{
		if (!m_pActorActionReport)
			return false;

		MAttackActionInfo^ maai = gcnew MAttackActionInfo(apxAttActInfo);
		return m_pActorActionReport->OnAttackingCheckHit(maai);
	}
	void ActorActionReport::OnAttackingHitEventPostNotify(const ApxDamageInfo& apxDamageInfo, IAssailable* pTarget, bool bTargetIsEnable)
	{
		if (!m_pActorActionReport)
			return;

		MDamageInfo^ mdi = gcnew MDamageInfo;
		MActorBase::ConvertDamageInfo(apxDamageInfo, mdi);
		MIAssailable^ mt = MScene::Instance->GetMIAssailable(pTarget);
		m_pActorActionReport->OnAttackingHitEventPostNotify(mdi, mt, bTargetIsEnable);
	}
	bool ActorActionReport::OnPreAttackedQuery()
	{
		if (m_pActorActionReport)
			return m_pActorActionReport->OnPreAttackedQuery();
		return true;
	}
	void ActorActionReport::OnPreAttackedNotify(ApxAttackActionInfo& inoutAAAI, const PhysXRBDesc* pAttacker)
	{
		if (m_pActorActionReport)
		{
			MPhysXRBDesc^ desc = nullptr;
			if (0 != pAttacker)
				desc = gcnew MPhysXRBDesc(*pAttacker);
			MAttackActionInfo^ maai = gcnew MAttackActionInfo(inoutAAAI);
			m_pActorActionReport->OnPreAttackedNotify(maai, desc);
			MActorBase::ConvertAttackInfo(maai->mAttack, inoutAAAI.mApxAttack);
			MActorBase::ConvertDamageInfo(maai->mDamage, inoutAAAI.mApxDamage);
		}
	}
	bool ActorActionReport::OnAttackedHitEvent(const ApxDamageInfo& apxDamageInfo)
	{
		if (!m_pActorActionReport)
			return false;

		MDamageInfo^ mdi = gcnew MDamageInfo;
		MActorBase::ConvertDamageInfo(apxDamageInfo, mdi);
		return m_pActorActionReport->OnAttackedHitEvent(mdi);
	}

	//----------------------------------------------
	// actor's user input report...
	UserInputFlag ActorActionReport::TickUserInput()
	{
		if (m_pActorActionReport)  
			return static_cast<UserInputFlag>(m_pActorActionReport->TickUserInput());
		return UIF_NONE;
	}
	void ActorActionReport::ApplyUserInput(UserInputFlag uifs)
	{
		if (m_pActorActionReport)  
			m_pActorActionReport->ApplyUserInput(static_cast<MUserInputFlag>(uifs));
	}

	MActionInfo^ ActorActionReport::ConvertActionInfo(ApxActionInfo* pActionInfo)
	{
		if (0 == pActionInfo)
			return nullptr;

		ApxComboAttackActionInfo* pACAAI = pActionInfo->IsComboAttackActionInfo();
		if (0 != pACAAI)
			return gcnew MComboAttackActionInfo(*pACAAI);

		ApxAttackActionInfo* pAAAI = pActionInfo->IsAttackActionInfo();
		if (0 != pAAAI)
			return gcnew MAttackActionInfo(*pAAAI);

		return gcnew MActionInfo(*pActionInfo);
	}
}