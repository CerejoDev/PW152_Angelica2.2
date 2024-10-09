#pragma once

#include "MCarryable.h"

namespace APhysXCommonDNet
{
	//reference to ApxActionPlayMode
	public enum class MActionPlayMode
	{
		ActionPlayMode_NormalPlay,			// play the action until current action is finished.

		ActionPlayMode_InstantPlay,			// play the action instantly and clear the action queue.

		ActionPlayMode_CheckPlay,			// check if current action play is finished, if not, just ignore this action.

		ActionPlayMode_ComboPlay,

	};

	//reference to ApxActionOverrideMask
	public enum class MActionOverrideMask
	{
		ActionORMask_None			=		0,					// both upper-body and lower-body can not be overridden, such as full-body action
		ActionORMask_UpperBody		=		1 << 0,				// upper-body can be overridden, such as walk/run action
		ActionORMask_LowerBody		=		1 << 1,				// lower-body can be overridden, such as attack action


		ActionORMask_FullBody		=							// both upper-body and lower-body can be overridden, such as idle action
		ActionORMask_UpperBody | ActionORMask_LowerBody,
	};

	public ref class MActionAnimInfo
	{
	internal:
		MActionAnimInfo(const ApxActionAnimInfo& aaai)
		{
			mActionName = gcnew System::String(aaai.mActionName);		
			mActionORMask = aaai.mActionORMask;
			mBlendInTime = aaai.mBlendInTime;
			mLooping = aaai.mLooping;
			mRestart = aaai.mRestart;			
			mBlendWalkRun = aaai.mBlendWalkRun;
			mBasicAction = aaai.mBasicAction;
		}

		void CopyToApxActionAnimInfo(ApxActionAnimInfo& aaai)
		{
			if (mActionName != nullptr)
			{
				pin_ptr<const wchar_t> wch = PtrToStringChars(mActionName);
				aaai.mActionName = WC2AS(wch);
			}
			else
				aaai.mActionName.Empty();
			aaai.mActionORMask = mActionORMask;
			aaai.mBlendInTime = mBlendInTime;
			aaai.mLooping = mLooping;
			aaai.mRestart = mRestart;
			aaai.mBlendWalkRun = mBlendWalkRun;
			aaai.mBasicAction = mBasicAction;
		}

	public:
		System::String^ mActionName;		
		unsigned int mActionORMask;

		float mBlendInTime;					// time to blend in the current animation...
		bool mLooping;						// if mLooping is true, we will ignore the following data members...
		bool mRestart;						// if mRestart is true, we will restart the animation even if we play the same animation.			

		bool mBlendWalkRun;                 //play blend walk run animation, ignore mActionName when is true
		bool mBasicAction;	

	public:
		MActionAnimInfo()
		{
			Set(nullptr, MActionOverrideMask::ActionORMask_None, 0.2f, false, true);
		}
		MActionAnimInfo(System::String^ strActionName)
		{
			Set(strActionName, MActionOverrideMask::ActionORMask_None, 0.2f, false, true);			
		}
		MActionAnimInfo(System::String^ strActionName, MActionOverrideMask mask)
		{
			Set(strActionName, mask, 0.2f, false, true);
		}
		MActionAnimInfo(System::String^ strActionName, MActionOverrideMask mask, float fBlendTime)
		{
			Set(strActionName, mask, fBlendTime, false, true);
		}
		MActionAnimInfo(System::String^ strActionName, MActionOverrideMask mask, float fBlendTime, bool bLoop)
		{
			Set(strActionName, mask, fBlendTime, bLoop, true);
		}
		MActionAnimInfo(System::String^ strActionName, MActionOverrideMask mask, float fBlendTime, bool bLoop, bool bRestart)
		{
			Set(strActionName, mask, fBlendTime, bLoop, bRestart);
		}
		void Set(System::String^ strActionName, MActionOverrideMask mask, float fBlendTime, bool bLoop, bool bRestart)
		{
			mActionName = strActionName;
			mActionORMask = (unsigned int)mask;
			mBlendInTime = fBlendTime;
			mLooping = bLoop;
			mRestart = bRestart;
			mBlendWalkRun = false;
			mBasicAction = true;
		}
	};

	ref class MAttackActionInfo;
	ref class MComboAttackActionInfo;

	public ref class MActionInfo
	{
	internal:
		MActionInfo(const ApxActionInfo& actionInfo)
		{
			mAnimInfo = gcnew MActionAnimInfo(actionInfo.mAnimInfo);
			mPlayMode = static_cast<MActionPlayMode>(actionInfo.mPlayMode);
			mCoolDownTime = actionInfo.mCoolDownTime;
			mIgnoreActionTime = actionInfo.mIgnoreActionTime;
		}

		ApxActionInfo* GetApxActionInfo() 
		{
			if(m_pApxActionInfo == NULL)
				m_pApxActionInfo = CreateApxActionInfo();
			CopyToApxActionInfo(m_pApxActionInfo);
			return m_pApxActionInfo;
		}

	public:
		MActionAnimInfo^ mAnimInfo;
		MActionPlayMode mPlayMode;

		float mCoolDownTime;				// if current action is playing, wait until cool down time to play it again.
		float mIgnoreActionTime;			// if current action is playing, just ignore other normal-play action in mIgnoreActionTime.

		float mPlaySpeed;

		MActionInfo(MActionAnimInfo^ animInfo)
		{
			Set(animInfo, MActionPlayMode::ActionPlayMode_NormalPlay, 0.3f, 0.3f);			
		}
		MActionInfo(MActionAnimInfo^ animInfo, MActionPlayMode mode)
		{
			Set(animInfo, mode, 0.3f, 0.3f);
		}
		MActionInfo(MActionAnimInfo^ animInfo, MActionPlayMode mode, float cooldownTime)
		{
			Set(animInfo, mode, cooldownTime, 0.3f);
		}
		MActionInfo(MActionAnimInfo^ animInfo, MActionPlayMode mode, float cooldownTime, float ignoreTime)
		{
			Set(animInfo, mode, cooldownTime, ignoreTime);
		}

		!MActionInfo(){ Release();}
		~MActionInfo() { Release();}

		void Set(MActionAnimInfo^ animInfo, MActionPlayMode mode, float coolDownTime, float IgnoreActionTime)
		{
			mAnimInfo = animInfo;
			mPlayMode = mode;
			mCoolDownTime = coolDownTime;
			mIgnoreActionTime = IgnoreActionTime;
			mPlaySpeed = 1.0f;
		}

		virtual void Release()
		{
			if(m_pApxActionInfo)
			{
				delete m_pApxActionInfo;
				m_pApxActionInfo = NULL;
			}
		}
		virtual MAttackActionInfo^ IsAttackActionInfo() { return nullptr; }
		virtual MComboAttackActionInfo^ IsComboAttackActionInfo() { return nullptr; }

	protected:
		virtual ApxActionInfo* CreateApxActionInfo()
		{
			return (new ApxActionInfo);
		}
		
		virtual void CopyToApxActionInfo(ApxActionInfo* pInOutInfo)
		{
			if (0 == pInOutInfo)
				return;

			if (nullptr != mAnimInfo)
				mAnimInfo->CopyToApxActionAnimInfo(pInOutInfo->mAnimInfo);
			pInOutInfo->mPlayMode = static_cast<ApxActionPlayMode>(mPlayMode);
			pInOutInfo->mCoolDownTime = mCoolDownTime;
			pInOutInfo->mIgnoreActionTime = mIgnoreActionTime;
			pInOutInfo->mPlaySpeed = mPlaySpeed;
		}

		ApxActionInfo* m_pApxActionInfo;
	};

	public enum class MDamageSource
	{
		MDS_FRONT	= (1<<0),
		MDS_REAR	= (1<<1),
		MDS_LEFT	= (1<<2),
		MDS_MIDDLE	= (1<<3),
		MDS_RIGHT	= (1<<4),
	};

	ref class MActorBase;
	public ref struct MDamageInfo
	{
		MDamageInfo() 
		{
			mpAttacker = nullptr;
			mHitRBActorPtr = 0;
			mHitPos.Clear();
			mHitDir.Clear();
			mHitMomentum = 0;
			mHitDamage = 0;
			mSourceFlags = 0;
		}
		MIAssailable^ mpAttacker;
		void* mHitRBActorPtr;
		Vector3 mHitPos;
		Vector3 mHitDir;
		float mHitMomentum;
		float mHitDamage;
		int mSourceFlags;
	};

	public ref struct MAttackInfo
	{
		MAttackInfo()
		{
			mIsPhysXDetection = true; 
			mAttackType = 0;
			mTeamGroup = 0;
			mAttackRange = 0;
			mAttHalfRadianRange = -1;
			mDamageDelayStartSec = 0;
			mDamageDurationSec = 0;
			mpTarget = nullptr;
		}
		// input angle value. 
		// if input angle < 0,  attack only one target
		// if input angle > 0 :  can attack multi-targets
		// if input angle > 360, the applied value is angle mod 360.
		void ConvertAttackAngularRange(float angleValue)
		{
			mAttHalfRadianRange = ApxAttackInfo::ConvertAttackAngularRange(angleValue);
		}
		//  here are some member items relationship 
		//
		//                |     PhysX detection     |    None PhysX detection
		//  ______________|_________________________|_________________________
		//  melee attack  | damage delay start:  x  | damage delay start:  x
		//                | damage duration :    y  | damage duration :    0
		//  ______________|_________________________|_________________________ 
		//  ranged attack | damage delay start:  0  | damage delay start:  0
		//                | damage duration :    0  | damage duration :    0  

		bool mIsPhysXDetection;
		int mAttackType;
		unsigned int mTeamGroup;
		float mAttackRange;
		float mAttHalfRadianRange;
		float mDamageDelayStartSec;
		float mDamageDurationSec;
		MIAssailable^ mpTarget;
	};

	public ref class MAttackActionInfo : MActionInfo
	{
	internal:
		MAttackActionInfo(const ApxAttackActionInfo& AAAInfo);

	public:
		MAttackInfo^ mAttack;
		MDamageInfo^ mDamage;

		MAttackActionInfo(MActionAnimInfo^ animInfo, MAttackInfo^ attackInfo, MDamageInfo^ damageInfo)
			: MActionInfo(animInfo)
		{
			mAttack = attackInfo;
			mDamage = damageInfo;
		}
		virtual MAttackActionInfo^ IsAttackActionInfo() override { return this; }

	protected:
		virtual ApxActionInfo* CreateApxActionInfo() override
		{
			return (new ApxAttackActionInfo);
		}

		virtual void CopyToApxActionInfo(ApxActionInfo* pInOutInfo) override;
	};

	public ref class MComboAttackActionInfo : MAttackActionInfo
	{
	internal:
		MComboAttackActionInfo(const ApxComboAttackActionInfo& ACAAInfo);

	public:
		float mComboPlayTime;
		float mComboEnhanceFactor;
		array<MActionAnimInfo^>^ mSeqAnim;		// currently, we can support 3-combo attack action at most.
		array<float>^ mSeqActDamDelayStartSec;
		array<float>^ mSeqActDamDurationSec;

		MComboAttackActionInfo(MActionAnimInfo^ animInfo) : MAttackActionInfo(animInfo, nullptr, nullptr)
		{
			mComboPlayTime = 0.3f;
			mComboEnhanceFactor = 1.0f;
			mSeqAnim = gcnew array<MActionAnimInfo^>(2);
			mSeqActDamDelayStartSec = gcnew array<float>(2);
			mSeqActDamDurationSec = gcnew array<float>(2);
			mPlayMode = MActionPlayMode::ActionPlayMode_ComboPlay;
		}

		virtual MAttackActionInfo^ IsAttackActionInfo() override { return this; }
		virtual MComboAttackActionInfo^ IsComboAttackActionInfo() override { return this; }

	protected:
		virtual ApxActionInfo* CreateApxActionInfo() override
		{
			return (new ApxComboAttackActionInfo);
		}

		virtual void CopyToApxActionInfo(ApxActionInfo* pInOutInfo) override;
	};

	public enum class MActionLayer
	{
		MActionLayer_Move,							// to control and play the action related to actor's basic movement
		MActionLayer_Active,							// to control and play the active action, such as attack, pick up, throw, ...
		MActionLayer_Passive,							// to control and play the passive action, such as be hit

		MActionLayer_Reserved,						// reserved currently

		MActionLayer_Num				=	4,
	};


	//----------------------------------------------
	// each layer of controller has 3 channels to control
	// different part of body.
	public enum class MActionChannel
	{
		MActionChannel_FullBody,
		MActionChannel_UpperBody,	
		MActionChannel_LowerBody,	
	};
	
	public ref struct MAttDamInfo
	{
		MAttDamInfo() { mAttack = nullptr; mDamage = nullptr; }
		MAttackInfo^ mAttack;
		MDamageInfo^ mDamage;
	};

	public enum class MSkinModelSocketType
	{
		MApxSM_SocketType_Bone,
		MApxSM_SocketType_Hook,
	};

	public ref struct MWeaponInfo
	{
		MWeaponInfo()
		{
			mWeaponName = nullptr;
			mHostSocketType = MSkinModelSocketType::MApxSM_SocketType_Bone;
			mWPNSocketType = MSkinModelSocketType::MApxSM_SocketType_Bone;
			mHostSocketName = nullptr;
			mWPNSocketName = nullptr;

			mbUsingXAJHWay = false;
			mWPNECMNickName = nullptr;
			mWPNAttacher = nullptr;
		}
		System::String^ mWeaponName;
		MSkinModelSocketType mHostSocketType;
		MSkinModelSocketType mWPNSocketType;
		System::String^ mHostSocketName;
		System::String^ mWPNSocketName;

		bool mbUsingXAJHWay;
		System::String^ mWPNECMNickName;
		System::String^ mWPNAttacher;
	};

	public enum class MUserInputFlag
	{
		MUIF_NONE			 = 0,
		MUIF_MOVE_FORWARD    = (1<<0),
		MUIF_MOVE_BACKWARD   = (1<<1),
		MUIF_MOVE_LEFTSTRAFE = (1<<2),
		MUIF_MOVE_RIGHTSTRAFE = (1<<3),
		MUIF_MOVE_UP		 = MUIF_MOVE_FORWARD,
		MUIF_MOVE_DOWN		 = MUIF_MOVE_BACKWARD,
		MUIF_MOVE_LEFT		 = MUIF_MOVE_LEFTSTRAFE,
		MUIF_MOVE_RIGHT		 = MUIF_MOVE_RIGHTSTRAFE,
		MUIF_SPEED_INCREASE	 = (1<<4),
		MUIF_SPEED_DECREASE  = (1<<5),

		MUIF_TESTDIR_TURNPOS = (1<<16),
		MUIF_TESTDIR_TURNNEG = (1<<17),
		MUIF_TEST_MOVE		 = (1<<18),
	};

	class ActorActionReport;
	
	public ref class MActorActionReport : MIKeyEvent
	{
	public:
		static MActorActionReport^ CreateInstance(System::String^ strActionName);

		MActorActionReport();
		property MActorBase^ HostActor
		{
			MActorBase^ get() { return m_pActorBase; }
			void set(MActorBase^ value) { m_pActorBase = value; }
		}
		//initialization work
		virtual void Init(MIObjBase^ pObjBase){}
		//----------------------------------------------
		// actor's movement user report...

		// for user's jump call back...
		virtual void OnStartJump(){}
		virtual void OnLand(){}
		virtual void OnReachApex(){}

		// for user's swing call back...
		virtual void OnStartSwing(){}
		virtual void OnEndSwing(){}

		// for user's push action call back...
		virtual void OnPushObj(MIPhysXObjBase^ pPhysX){}

		// for user's pre-land notification call back...
		virtual void PreLandNotify(float fTimeBeforeLand){}

		//----------------------------------------------
		// actor's action user report...
		virtual void OnActionStart(MActionLayer apxActionLayer, MActionInfo^ pActionInfo){}
		virtual void OnActionEnd(MActionLayer apxActionLayer, MActionInfo^ pActionInfo){}

		virtual void OnChangeEquipment(){}
		virtual void OnUpdateMoveState(){}
		virtual bool OnAttackingCheckHit(MAttackActionInfo^ pAttActInfo){ return true; }
		virtual void OnAttackingHitEventPostNotify(MDamageInfo^ pDamageInfo, MIAssailable^ pTarget, bool bTargetIsEnable) {}
		virtual bool OnPreAttackedQuery() { return true; }
		virtual void OnPreAttackedNotify(MAttackActionInfo^ pAttActInfo, MPhysXRBDesc^ pAttacker) {}
		virtual bool OnAttackedPreHitEvent(MActorBase^ pCaster, int attackType, unsigned int idxCasterTeam) { return true; }
		virtual bool OnAttackedHitEvent(MDamageInfo^ pDamageInfo) { return true; }
		virtual void OnRebornNotify(){}

		//----------------------------------------------
		// actor's user input report...
		virtual MUserInputFlag TickUserInput() { return MUserInputFlag::MUIF_NONE; }
		virtual void ApplyUserInput(MUserInputFlag uifs) {}

		//inherit from MIKeyEvent
		virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){}
		virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){}

		//mouse event
		virtual void OnLButtonDown(int x, int y, unsigned int nFlags){}
		virtual void OnLButtonUp(int x, int y, unsigned int nFlags){}
		virtual void OnMButtonUp(int x, int y, unsigned int nFlags){}
		virtual void OnRButtonDown(int x, int y, unsigned int nFlags){}
		virtual void OnRButtonUp(int x, int y, unsigned int nFlags){}
		virtual void OnMouseMove(int x, int y, unsigned int nFlags){}
		virtual void OnMouseWheel(int x, int y, int zDelta, unsigned int nFlags){}

		//carrying state
		virtual void OnCarryingStateChanged(MCarryableState oldState, MCarryableState newState) { }

	protected:
		MActorBase^ m_pActorBase;
	};

	public enum class MGroundMoveState
	{
		// normal states...
		MActor_GroundMoveState_StandInPlace,		// just stand in place without any movement
		MActor_GroundMoveState_Move,				// walk, run, jog all correspond to this state
		MActor_GroundMoveState_InAir,				// for more detailed state, you should refer to the velocity.z value. { Up, Down, Land }

		// some special states...
		MActor_GroundMoveState_Push = 10,			// when the actor is blocked by some dynamic actor, he/she can push the actors. 
		// Only take effect when the internal CC is APhysXCC...

		MActor_GroundMoveState_Swing,				// in case of APhysXDynCharCtrler...
	};

	ref class MPhysXObjDynamic;
	ref class MCarryable;
	ref class MPushable;

	public ref class MActorBase : public MIAssailable
	{
	internal:
		MActorBase(ApxActorBase* pActorBase, MIObjBase^ pObjBase);

	public:
		static void ConvertDamageInfo(MDamageInfo^ pIn, ApxDamageInfo& outADI);
		static void ConvertAttackInfo(MAttackInfo^ pIn, ApxAttackInfo& outAAI);
		static void ConvertDamageInfo(const ApxDamageInfo& in, MDamageInfo^ pOutMDI);
		static void ConvertAttackInfo(const ApxAttackInfo& in, MAttackInfo^ pOutMAI);

		void SetToDefault();
		!MActorBase();
		virtual ~MActorBase();

		void RegisterKeyEvent();
		void UnRegisterKeyEvent();
		// init the action controller and ground-move controller.
		void SetPos(Vector3 vPos);
		Vector3 GetPos();
		void SetExtents(Vector3 vExtents);
		Vector3 GetExtents();
		void SetGroundMoveDir(Vector3 vMoveDir);
		Vector3 GetGroundMoveDir();
		void SetGroundMoveSpeed(float fSpeed);
		float GetGroundMoveSpeed();
		void GroundJump(float fUpSpeed);
		void SetGroundTargetPos(Vector3 vTargetPos);
		bool IsGroundTargetPosReached();
		void SetHeadDir(Vector3 vHeadDir);
		void SetHeadDir(Vector3 vHeadDir, bool bForce);
		Vector3 GetHeadDir();
		MPhysXObjDynamic^ GetObjDyn();

		void SetEnableTick(bool bEnable) { m_pActorBase->SetEnableTick(bEnable); }
		bool GetEnableTick() { return m_pActorBase->GetEnableTick(); }

		bool PlayAction(MActionLayer layer, MActionInfo^ ActionInfo);
		void SwitchToRagDoll(MDamageInfo^ pDamageInfo);
		void CloseRagDoll();
		void RebornNotify();

		bool ChangeEquipment(MWeaponInfo^ newWeapon);
		MWeaponInfo^ GetCurrentEquipment();
		System::String^ GetFirstActionName();
		System::String^ GetNextActionName();

		void SetMActionReport(MActorActionReport^ pActionReport);
		bool IsHostActor() { return m_bHostActor; }
		void SetHostActor(bool bHostActor) { m_bHostActor = bHostActor; }
		void EnterCameraBinding(bool bIsKeepDir);
		MGroundMoveState GetGroundMoveState();
		bool IsInAir() { return m_pActorBase->GetGroundMoveController()->IsInAir(); }
		ApxActorBase* GetAAB() { return m_pActorBase; }
		Vector3 GetInputMoveDir(MUserInputFlag muifs);
		virtual IAssailable* GetIAssailable() { return m_pActorBase; }
		virtual MIObjBase^ GetObject();
		virtual MActorBase^ GetActorBase();
		virtual Vector3 GetCurrentPos();
		virtual Matrix4 GetGPose();
		virtual bool OnPreAttackedQuery();
		virtual void OnPreAttackedNotify(MAttackActionInfo^ pAttActInfo, MPhysXRBDesc^ pAttacker);
		virtual bool OnAttackedPreHitEvent(MActorBase^ pCaster, int attackType, unsigned int idxCasterTeam);

		//@parameter strLFoot: bone name of left foot
		//           strRFoot: bone name of right foot
		bool InitFootIK(System::String^ strLFoot, System::String^ strRFoot);
		bool ReleaseFootIK();
		//enable & disable foot IK
		void EnableFootIK(bool bLFoot, bool bRFoot);

		// enable Hand IK....
		void EnableHandIK(bool bLH, bool bRH);
		void SetHandGoal(Vector3 vGoal, bool bLH);

		void TryHandInHandWith(MActorBase^ abCompanion);
		void EnableHandInHand(bool bEnable) { m_bHandInHand = bEnable; }
		bool IsHandInHandEnabled() { return m_bHandInHand;}
		void SetHandExtents(Vector3 vHandExtents);

		bool InitWalkRunBlend(System::String^ strWalk, System::String^ strRun);
		bool ReleaseWalkRunBlend();

		//hand ik for carrying
		void InitHandIK(System::String^ strLHand, System::String^ strRHand);
		void ReleaseHandIK();
		bool TryToCarry(MCarryable^ pCarryable, System::String^ strAnimation, float fBindFrameRatio);
		bool TryToCarry(MCarryable^ pCarryable, System::String^ strAnimation, float fBindFrameRatio, System::String^ strLChild, System::String^ strRChild);
		bool TryToThrow(System::String^ strAnimation, float fReadyRatio, float fThrowRatio, float fSpeed);

		bool TryToPush(MPushable^ pPushable, System::String^ strAnimation, float fBindFrameRatio);
		//fkeepIKTime: 保持Ik时间
		//fTransTime：IK过渡到FK时间
		bool RelasePushable(float fTransTime, float fKeepIKTime);

		void EnableIK(bool bEnable) { m_pActorBase->EnableIK(bEnable); }
		bool IsIKEnabled() { return m_pActorBase->IsIKEnabled(); }

		void EnableDamageFromPhysXContact(bool bEnable) { m_pActorBase->EnableDamageFromPhysXContact(bEnable); }
		bool IsEnableDamageFromPhysXContact() { return m_pActorBase->IsEnableDamageFromPhysXContact(); }

		void SetHitTarget(MIAssailable^ pObj, float fLockTime);
		void ResetHitTarget();
		bool HasHitTarget() { return m_pActorBase->HasHitTarget(); }
		MIAssailable^ GetHitTarget();

		void SetBlocked(bool bBlocked) { m_pActorBase->SetBlocked(bBlocked); }
		bool IsBlocked() { return m_pActorBase->IsBlocked(); }

		property MCarryable^ MCarryableBox
		{
			MCarryable^ get() { return m_pCarryable; }
		}

		void TurnTestDir(float fDeg) { m_pActorBase->TurnTestDir(fDeg); }
		Vector3 GetTestDir() { return Vector3::FromA3DVECTOR3(m_pActorBase->GetTestDir()); }
		void SetGroundMoveDirWithNoTurn(Vector3 vDir){ m_pActorBase->SetGroundMoveDirWithNoTurn(APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(vDir))); }
		//auto turn body
		bool IsAutoTurn() { return m_pActorBase->IsAutoTurn(); }
		void SetAutoTurn(bool bAutoTurn) { m_pActorBase->SetAutoTurn(bAutoTurn); }
		void SetUseBodyCtrl(bool bUseBodyCtrl){ m_pActorBase->SetUseBodyCtrl(bUseBodyCtrl); }
		bool IsUseBodyCtrl() { return m_pActorBase->IsUseBodyCtrl(); }

		bool IsBendBodyEnable() { return m_pActorBase->IsBendBodyEnable(); }
		bool SetBendBodyEnable(bool bEnable) { return m_pActorBase->SetBendBodyEnable(bEnable); }
		bool IsUpperBoneCtrlFree() { return m_pActorBase->IsUpperBoneCtrlFree(); }
		void BendBody(Vector3 vDir, float fBendDeg, float fBendTime, float fReTime);
		void SetBlendAction(System::String^ strLAction, System::String^ strLInvAction, System::String^ strRAction, System::String^ strRInvAction,
			System::String^ strFAction, System::String^ strBAction, System::String^ strFWalk);

#ifdef _ANGELICA21
		void SwitchToPhys(System::String^ strBone, float fPhyWeight);

		System::String^ GetParentBone(System::String^ strBone);
		System::String^ GetFirstChildBone(System::String^ strBone);
		System::String^ GetNextSiblingBone(System::String^ strBone);
#endif

	internal:
		void SetCarryablePtr(MCarryable^ pCarryable) { m_pCarryable = pCarryable; }
		MSkinModelSocketType ConvertSockeyType(ApxSkinModelSocketType st);
		ApxSkinModelSocketType ConvertSockeyType(MSkinModelSocketType st);

	protected:
		bool m_bHostActor;	//is user controlled actor
		ApxActorBase*		m_pActorBase;
		ActorActionReport*	m_pActorReport;

		MIObjBase^			m_pObjBase;
		MActorActionReport^ m_pMActorReport;

		MCarryable^         m_pCarryable;
		MPushable^          m_pPushable;

		MIAssailable^          m_pHitTarget;

		bool m_bHandInHand;
	};

	class ActorActionReport : public ApxActorActionReport
	{
	public:
		ActorActionReport();
		virtual ~ActorActionReport();
		void SetActorActionReport(MActorActionReport^ pActionReport);

		virtual void GetGPose(A3DMATRIX4& mtPose) const
		{
			assert(!"Shouldn't be here!");
		}
		virtual IObjBase* GetObject()
		{
			assert(!"Shouldn't be here!");
			return 0;
		}
		virtual ApxActorBase* GetActorBase()
		{
			assert(!"Shouldn't be here!");
			return 0;
		}

	protected:
		//----------------------------------------------
		// actor's movement user report...

		// for user's jump call back...
		virtual void OnStartJump();
		virtual void OnLand();
		virtual void OnReachApex();

		// for user's swing call back...
		virtual void OnStartSwing();
		virtual void OnEndSwing();

		// for user's push action call back...
		virtual void OnPushObj(IPhysXObjBase* pPhysXObj);

		// for user's pre-land notification call back...
		virtual void PreLandNotify(float fTimeBeforeLand);

		//----------------------------------------------
		// actor's action user report...
		virtual void OnActionStart(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo);
		virtual void OnActionEnd(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo);

		virtual void OnChangeEquipment();
		virtual void OnUpdateMoveState();
		virtual bool OnAttackingCheckHit(const ApxAttackActionInfo& apxAttActInfo);
		virtual void OnAttackingHitEventPostNotify(const ApxDamageInfo& apxDamageInfo, IAssailable* pTarget, bool bTargetIsEnable);
		virtual bool OnPreAttackedQuery();
		virtual void OnPreAttackedNotify(ApxAttackActionInfo& inoutAAAI, const PhysXRBDesc* pAttacker);
		virtual bool OnAttackedHitEvent(const ApxDamageInfo& apxDamageInfo);

		//----------------------------------------------
		// actor's user input report...
		virtual UserInputFlag TickUserInput();
		virtual void ApplyUserInput(UserInputFlag uifs);

		MActionInfo^ ConvertActionInfo(ApxActionInfo* pActionInfo);

	protected:
		gcroot<MActorActionReport^> m_pActorActionReport;
	};
}