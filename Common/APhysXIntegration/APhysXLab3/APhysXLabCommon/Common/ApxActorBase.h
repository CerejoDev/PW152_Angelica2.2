
/*----------------------------------------------------------------------

	ApxActorBase.h:

		define the base class of all actors. Actors here represent the objects
		which can be controlled by players or AI in the scene. Avatars, NPCs and
		Monsters all belong to Actor.

	Created by He Wenfeng, June 14, 2011

----------------------------------------------------------------------*/
#pragma	once

#ifndef _APHYSXLAB_APXACTORBASE_H_
#define _APHYSXLAB_APXACTORBASE_H_

#define APX_COMMON_ROUTINE
#ifdef	APX_COMMON_ROUTINE

	#define CR_VIRTUAL

#else

	#define CR_VIRTUAL	virtual

#endif

#include "IKSolverArm.h"
#include "ApxObjSkinModelSocket.h"
#ifdef _ANGELICA3
	#include "ECCounter.h"
#else
	#include "ECCounterFor2.h"
#endif
#include "ActorState.h"

//----------------------------------------------
// extra class references...
class APhysXCharacterController;
class ApxActorBase;
class A3DIKTrigger;
class A3DAnimNodeBlendWalkRun;
class CIKHandTarget;
class CCarryable;
class Pushable;
class ECPlayerBodyCtrl;
class UpperBodyCtrl;
class A3DIKSoverFootPlacement;
//----------------------------------------------
// control actor's movement

enum ApxActorMoveType
{
	ApxActor_MoveType_GroundMove,
	ApxActor_MoveType_Fly,
	ApxActor_MoveType_Swim,
};

class ApxActorMoveController: public APhysXFlagOperation
{
public:

	enum 
	{
		ApxActor_MoveFlag_Driven	= 1 << 0,		// if raised, the user drive the CC in this frame, otherwise, stay in place or free fall.
	};

public:

	ApxActorMoveController(ApxActorMoveType moveType):m_MoveType(moveType) { SetToDefault();}
	virtual ~ApxActorMoveController() {}

	void SetToDefault()
	{
		m_vPos.zero();
		m_LastPos = m_vPos;
		m_vHeadDir.set(0.0f, 0.0f, 1.0f);
		m_vHeadUp.set(0.0f, 1.0f, 0.0f);

		m_vExtents.zero();

		m_vMoveDir.zero();
		m_fMoveSpeed = 0.0f;

		m_pActorBase = NULL;
		
		m_uMoveFlags = 0;
	}

	void InitActorBase(ApxActorBase* pActorBase) { m_pActorBase = pActorBase; }
	ApxActorBase* GetActorBase() { return m_pActorBase; }

	bool SetPos(const NxVec3& vPos)
	{
		if(CheckVolumeValid(vPos, m_vExtents))
		{
			m_vPos = vPos;
			OnSetPos();
			return true;
		}
		else
			return false;
	}
	const NxVec3& GetPos() const { return m_vPos; }

	NxVec3 GetDeltaMove() const
	{
		return m_vPos - m_LastPos;
	}

	void SetHeadDir(const NxVec3& vHeadDir)
	{
		m_vHeadDir = vHeadDir;
		OnSetHeadDir();
	}
	
	const NxVec3& GetHeadDir() const { return m_vHeadDir; }

	// Do not do this!...
	//void SetHeadUp(const NxVec3& vHeadUp)
	//{
	//	m_vHeadUp = vHeadUp;
	//	OnSetHeadUp();
	//}
	const NxVec3& GetHeadUp() const { return m_vHeadUp; }

	bool SetExtents(const NxVec3& vExtents)
	{
		if(CheckVolumeValid(m_vPos, vExtents))
		{
			m_vExtents = vExtents;
			OnSetExtents();
			return true;
		}
		else
			return false;
	}
	const NxVec3& GetExtents() const { return m_vExtents; }

	void SetMoveDir(const NxVec3& vMoveDir) { if (IsBlocked()) return; m_vMoveDir = vMoveDir; OnSetMoveDir();}
	const NxVec3& GetMoveDir() const { return m_vMoveDir; }
	void SetMoveSpeed(float fSpeed) { m_fMoveSpeed = fSpeed;  OnSetMoveSpeed(); }
	float GetMoveSpeed() const { return m_fMoveSpeed; }

	ApxActorMoveType GetMoveType() const { return m_MoveType; }

	void Tick(float dt) 
	{
		PreTickMove(dt);
		TickMove(dt);
		PostTickMove(dt);
	}

	void RaiseFlag(APhysXU32 flag) { APhysXFlagOperation::RaiseFlag(m_uMoveFlags, flag); }
	void ClearFlag(APhysXU32 flag) { APhysXFlagOperation::ClearFlag(m_uMoveFlags, flag); }
	bool CheckFlag(APhysXU32 flag) const { return APhysXFlagOperation::CheckFlag(m_uMoveFlags, flag); }


	//----------------------------------------------
	// virtual functions to be override...
	virtual NxVec3 GetVelocity() const
	{
		if(CheckFlag(ApxActor_MoveFlag_Driven))
			return m_fMoveSpeed * m_vMoveDir;
		return NxVec3(0.0f);
	}
	virtual void PrepareTick() {}
	virtual bool CheckVolumeValid(const NxVec3& vPos, const NxVec3& vExtents) { return true; }

	virtual bool IsBlocked() const { return false; }
	virtual void SetBlocked(bool bBlocked) { }

protected:

	virtual void PreTickMove(float dt) {}
	virtual void PostTickMove(float dt) {}
	virtual void TickMove(float dt) 
	{
		m_LastPos = m_vPos;
		if(CheckFlag(ApxActor_MoveFlag_Driven)&& !IsBlocked())
		{
			m_vPos += dt * m_fMoveSpeed * m_vMoveDir;
		}
	}	
	
	virtual void OnSetPos() {}
	virtual void OnSetExtents() {}
	virtual void OnSetMoveDir() {}
	virtual void OnSetMoveSpeed() {}
	virtual void OnSetHeadDir() {}
	virtual void OnSetHeadUp() {}

protected:
	
	// Note: pos here is the center of the volume, but not the foot pos.
	NxVec3 m_vPos;
	NxVec3 m_LastPos;

	NxVec3 m_vHeadDir;
	NxVec3 m_vHeadUp;

	// Bounding Extents: we assume the actor is in shape of capsule but not box, 
	// so only x and y components of Extents is considering, and Extents are half
	// size of dimensions.
	NxVec3 m_vExtents;

	NxVec3 m_vMoveDir;
	float m_fMoveSpeed;

	const ApxActorMoveType m_MoveType;

	ApxActorBase* m_pActorBase;

	APhysXU32 m_uMoveFlags;
};

class ApxActorFlyController:public ApxActorMoveController
{
public:

	ApxActorFlyController(ApxActorMoveType moveType):ApxActorMoveController(ApxActor_MoveType_Fly) { SetToDefault();}
	virtual ~ApxActorFlyController() {}
};

class ApxActorSwimController:public ApxActorMoveController
{

public:

	ApxActorSwimController(ApxActorMoveType moveType):ApxActorMoveController(ApxActor_MoveType_Swim) { SetToDefault();}
	virtual ~ApxActorSwimController() {}
};

class ApxActorGroundMoveController:public ApxActorMoveController
{
protected:
	enum
	{
		// normal states...
		ApxActor_GroundMoveState_StandInPlace,		// just stand in place without any movement
		ApxActor_GroundMoveState_Move,				// walk, run, jog all correspond to this state
		ApxActor_GroundMoveState_InAir,				// for more detailed state, you should refer to the velocity.z value. { Up, Down, Land }


		// some special states...
		ApxActor_GroundMoveState_Push = 10,			// when the actor is blocked by some dynamic actor, he/she can push the actors. 
													// Only take effect when the internal CC is APhysXCC...

		ApxActor_GroundMoveState_Swing,				// in case of APhysXDynCharCtrler...

	};

//----------------------------------------------
public:
	ApxActorGroundMoveController():ApxActorMoveController(ApxActor_MoveType_GroundMove) { SetToDefault();}
	virtual ~ApxActorGroundMoveController() {}

	void SetToDefault()
	{
		m_iJumpCounter = 0;
		m_iMaxJumpTimes = 2;

		m_uGroundMoveState = ApxActor_GroundMoveState_StandInPlace;

		m_sfGravity = 9.8f;
	}

	void SetMaxJumpTimes(int iMaxJumpTimes) { m_iMaxJumpTimes = iMaxJumpTimes; }
	int GetMaxJumpTimes() const { return m_iMaxJumpTimes; }

	void Jump(float fUpSpeed)
	{
		if(CanJump())
		{
			DoJump(fUpSpeed);
			OnStartJump();
			m_iJumpCounter++;
		}
	}

	bool CanJump() const
	{
		if(IsInAir())
		{
			if(m_iJumpCounter>0 && m_iJumpCounter<m_iMaxJumpTimes)
				return true;
			else
				return false;
		}
		else 
			return true;
	}

	bool IsFreeFall() const
	{
		if(IsInAir() && m_iJumpCounter == 0) 
			return true;
		else
			return false;
	}

	APhysXU32 GetGroundMoveState() const { return m_uGroundMoveState; }

	void StartSwing(const NxVec3& vDir)
	{
		if(DoStartSwing(vDir))
		{
			OnStartSwing();
			m_uGroundMoveState = ApxActor_GroundMoveState_Swing;
		}
	}
	void EndSwing()
	{
		if(m_uGroundMoveState == ApxActor_GroundMoveState_Swing)
		{
			DoEndSwing();
			OnEndSwing();
			m_uGroundMoveState = ApxActor_GroundMoveState_InAir;
		}
	}
	
	bool IsInSwing() const { return m_uGroundMoveState == ApxActor_GroundMoveState_Swing; }

	void NotifyPushObj(IPhysXObjBase* pPhysXObj)
	{
		m_uGroundMoveState = ApxActor_GroundMoveState_Push;
		OnPushObj(pPhysXObj);
	}


	bool IsInNormalState() const
	{
		return !(IsInPush() || IsInSwing());
	}

	void SetGravity(float fGravity) { m_sfGravity = fGravity;  OnSetGravity();}
	float GetGravity() { return m_sfGravity; }

	virtual bool IsPushingObj(IPhysXObjBase* pObj) const { return false; }
	virtual bool IsInPush() const { return m_uGroundMoveState == ApxActor_GroundMoveState_Push; }
	virtual bool IsInAir() const = 0;

	// get the valid ground pos that can stand. try ray-trace/sweep method to implement it.
	virtual void GetGroundPos(NxVec3& vPos) const = 0;	

protected:

	virtual void DoJump(float fUpSpeed) = 0;
	virtual float GetDistToGround() = 0;
	
	// Dynamic CC should override these functions...
	virtual bool DoStartSwing(const NxVec3& vDir) { return false; }
	virtual void DoEndSwing() {}
	
	virtual void OnSetGravity() {}

	void Land()
	{
		OnLand();
		m_iJumpCounter = 0;
	}

	void ReachApex()
	{
		OnReachApex();
	}

	void UpdateMoveState();

	virtual void PreTickMove(float dt) 
	{
		m_vPreMoveVelocity = GetVelocity();
		m_bPreMoveInAir = IsInAir();
	}

	virtual void PostTickMove(float dt) ;

	
	//----------------------------------------------
	// user call back...
	virtual void OnStartJump();
	virtual void OnLand();
	virtual void OnReachApex();
	virtual void OnStartSwing();
	virtual void OnEndSwing();
	virtual void OnPushObj(IPhysXObjBase* pPhysXObj);
	virtual void PreLandNotify(float fTimeBeforeLand);
	virtual void OnUpdateMoveState();

protected:

	int m_iJumpCounter;
	int m_iMaxJumpTimes;

	NxVec3 m_vPreMoveVelocity;
	bool m_bPreMoveInAir;

	APhysXU32 m_uGroundMoveState;
	float m_sfGravity;
};


//----------------------------------------------
// control actor's action

enum	ApxActionPlayMode
{
	ApxActionPlayMode_NormalPlay,			// play the action until current action is finished.

	ApxActionPlayMode_InstantPlay,			// play the action instantly and clear the action queue.
	
	ApxActionPlayMode_CheckPlay,			// check if current action play is finished, if not, just ignore this action.

	ApxActionPlayMode_ComboPlay,

};

enum	ApxActionOverrideMask
{
	ApxActionORMask_None			=		0,					// both upper-body and lower-body can not be overridden, such as full-body action
	ApxActionORMask_UpperBody		=		1 << 0,				// upper-body can be overridden, such as walk/run action
	ApxActionORMask_LowerBody		=		1 << 1,				// lower-body can be overridden, such as attack action


	ApxActionORMask_FullBody		=							// both upper-body and lower-body can be overridden, such as idle action
		ApxActionORMask_UpperBody | ApxActionORMask_LowerBody,
};

struct ApxActionAnimInfo
{
	AString mActionName;		

	APhysXU32 mActionORMask;

	float mBlendInTime;					// time to blend in the current animation...
	bool mLooping;						// if mLooping is true, we will ignore the following data members...
	bool mRestart;						// if mRestart is true, we will restart the animation even if we play the same animation.			

	bool mBlendWalkRun;                 //play blend walk run animation, ignore mActionName when is true
	bool mBasicAction;					// the basic actions means the actions can be played directly by SkinModel, and non-basic
										// actions corresponds to those actions edited in ModEditor and played by ECModel...
	ApxActionAnimInfo()
	{
		mActionORMask = ApxActionORMask_None;
		mBlendInTime = 0.2f;
		mLooping = false;
		mRestart = true;
		mBlendWalkRun = false;
		mBasicAction = true;
	}
};

struct ApxAttackActionInfo;
struct ApxComboAttackActionInfo;

struct ApxActionInfo
{

	ApxActionAnimInfo mAnimInfo;

	ApxActionPlayMode mPlayMode;
		
	float mCoolDownTime;				// if current action is playing, wait until cool down time to play it again.
	float mIgnoreActionTime;			// if current action is playing, just ignore other normal-play action in mIgnoreActionTime.
	
	float mPlaySpeed;

	ApxActionInfo()
	{
		mPlayMode = ApxActionPlayMode_NormalPlay;

		mCoolDownTime = 0.3f;
		mIgnoreActionTime = 0.3f;

		mPlaySpeed = 1.0f;
	}

	virtual ~ApxActionInfo(){}

	virtual ApxActionInfo* Clone() const
	{
		ApxActionInfo* pCloned = new ApxActionInfo;
		*pCloned = *this;
		return pCloned;
	}

	virtual const ApxAttackActionInfo*  IsAttackActionInfo() const { return NULL; }
	virtual ApxAttackActionInfo*  IsAttackActionInfo() { return NULL; }

	virtual const ApxComboAttackActionInfo* IsComboAttackActionInfo() const { return NULL; }
	virtual ApxComboAttackActionInfo* IsComboAttackActionInfo() { return NULL; }
};

enum ApxDamageSource
{
	APX_ADS_FRONT	= (1<<0),
	APX_ADS_REAR	= (1<<1),
	APX_ADS_LEFT	= (1<<2),
	APX_ADS_MIDDLE	= (1<<3),
	APX_ADS_RIGHT	= (1<<4),
};

struct ApxDamageInfo
{
	ApxDamageInfo()
	{
		mpAttacker = 0;
		mHitRBActor = 0;
		mHitPos.zero();
		mHitDir.zero();
		mHitMomentum = 0;
		mHitDamage = 0;
		mSourceFlags = 0;
	}
	IAssailable* mpAttacker;
	NxActor* mHitRBActor;
	NxVec3 mHitPos;
	NxVec3 mHitDir;
	float mHitMomentum;
	float mHitDamage;
	int mSourceFlags;
};

class ApxTargetHistory;
struct ApxAttackInfo
{
	ApxAttackInfo() 
	{
		mIsPhysXDetection = true;
		mAttackType = 0;
		mTeamGroup = 0;
		mAttackRange = 0;
		mAttHalfRadianRange = -1;
		mDamageDelayStartSec = 0;
		mDamageDurationSec = 0;
		mpTarget = 0;
	}

	// input angle value. 
	// if input angle < 0,  attack only one target
	// if input angle > 0 :  can attack multi-targets
	// if input angle > 360, the applied value is angle mod 360.
	static float ConvertAttackAngularRange(float angleValue)
	{
		if (0.0f > angleValue)
			return -1.0;
		float half = NxMath::mod(angleValue, 360) * 0.5f;
		float fRtn =  half * NxPi / 180.0f;
		return fRtn;
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
	float mAttackRange;          // will be omitted if using PhysX detection 
	float mAttHalfRadianRange;   // will be omitted if using PhysX detection
	float mDamageDelayStartSec;
	float mDamageDurationSec;
	IAssailable* mpTarget;
};

struct ApxAttackActionInfo : public ApxActionInfo
{
	ApxAttackInfo mApxAttack;
	ApxDamageInfo mApxDamage;
	void UpdateDamageSource();

	virtual ~ApxAttackActionInfo(){}

	virtual ApxAttackActionInfo* Clone() const
	{
		ApxAttackActionInfo* pCloned = new ApxAttackActionInfo;
		*pCloned = *this;
		return pCloned;
	}

	virtual const ApxAttackActionInfo* IsAttackActionInfo() const { return this; }
	virtual ApxAttackActionInfo* IsAttackActionInfo() { return this; }
};

struct ApxComboAttackActionInfo:public ApxAttackActionInfo
{
	static const int nCount = 2;
	float mComboPlayTime;
	float mComboEnhanceFactor;
	ApxActionAnimInfo mSeqAnim[nCount];		// currently, we can support 3-combo attack action at most.
	float mSeqActDamDelayStartSec[nCount];
	float mSeqActDamDurationSec[nCount];

	ApxComboAttackActionInfo()
	{
		mPlayMode = ApxActionPlayMode_ComboPlay;

		mComboPlayTime = 0.3f;
		mComboEnhanceFactor = 1.0f;
		m_uComboActionID = 0;
		mSeqActDamDelayStartSec[0] = 0.0f;
		mSeqActDamDurationSec[0] = 0.0f;
		mSeqActDamDelayStartSec[1] = 0.0f;
		mSeqActDamDurationSec[1] = 0.0f;
	}
	virtual ~ApxComboAttackActionInfo(){}

	virtual ApxComboAttackActionInfo* Clone() const
	{
		ApxComboAttackActionInfo* pCloned = new ApxComboAttackActionInfo;
		*pCloned = *this;
		return pCloned;
	}	

	virtual const ApxAttackActionInfo* IsAttackActionInfo() const { return this; }
	virtual ApxAttackActionInfo* IsAttackActionInfo() { return this; }

	virtual const ApxComboAttackActionInfo* IsComboAttackActionInfo() const { return this; }
	virtual ApxComboAttackActionInfo* IsComboAttackActionInfo() { return this; }


	friend class ApxActionController;
private:
	void SetComboActionID(APhysXU32 uID) const
	{
		m_uComboActionID = uID;
	}
	mutable APhysXU32 m_uComboActionID;
};

struct ApxWeaponInfo
{
	ApxWeaponInfo();
	bool operator!=(const ApxWeaponInfo& awi) const;
	bool operator==(const ApxWeaponInfo& awi) const{ return !(*this != awi); }

	bool bUsingXAJHWay;
	TString strWeaponPathName;
	CPhysXObjDynamic* pWeaponObj;

	ApxSkinModelSocketType smsHostSocketType;
	ApxSkinModelSocketType smsWPNSocketType;
	TString strHostSocketName;
	TString strWPNSocketName;

	TString strWPNECMNickName;
	TString strWPNAttacher;
};

//----------------------------------------------
// currently, we only support 5 layers of action controllers, 
// each layer is corresponding to an ApxActionController, and
// controls a set of actions.

enum ApxActionLayer
{
	ApxActionLayer_Move,							// to control and play the action related to actor's basic movement
	ApxActionLayer_Active,							// to control and play the active action, such as attack, pick up, throw, ...
	ApxActionLayer_Passive,							// to control and play the passive action, such as be hit
	
	ApxActionLayer_Reserved,						// reserved currently

	ApxActionLayer_Num				=	4,
};

//----------------------------------------------
// a utility class to blend weight[0.0, 1.0]
class ApxWeightBlender
{
public:
	ApxWeightBlender(float fInitWeight = 0.0f)
	{
		Init(fInitWeight);
		m_fWeightDeltaFactor = 0.0f;
		m_fBlendTime = 0.0f;
	}
	
	void Init(float fInitWeight)
	{
		m_fCurWeight = fInitWeight;
	}

	float GetWeight() { return m_fCurWeight; }

	void SetTargetWeight(float fTargetWeight, float fBlendTime);
	void Tick(float dt);

protected:
	// for compute the weight of this layer.
	float m_fCurWeight;
	float m_fWeightDeltaFactor;
	float m_fBlendTime;
};

//----------------------------------------------
// each layer of controller has 3 channels to control
// different part of body.
enum ApxActionChannel
{
	ApxActionChannel_None				= 0,
	ApxActionChannel_UpperBody			= 1 << 0,	
	ApxActionChannel_LowerBody			= 1 << 1,	
	ApxActionChannel_FullBody			= ApxActionChannel_UpperBody | ApxActionChannel_LowerBody,
};


class ApxActionController
{

public:

	ApxActionController()
	{
		SetToDefault();
	}
	virtual ~ApxActionController() {}

	void SetToDefault()
	{
		m_fTimeAfterPlay = 0.0f;
		m_pCurActionInfo = NULL;

		m_apxActionLayer = ApxActionLayer_Move;
		m_activeChannel = ApxActionChannel_None;
		m_pActorBase = NULL;

		m_bNotifyActionEnd = false;

		m_fPlaySpeed = 1.0f;
	}

	void InitActorBase(ApxActorBase* pActorBase) { m_pActorBase = pActorBase; }
	ApxActorBase* GetActorBase() { return m_pActorBase; }
	
	virtual void Init(){}
	virtual void SetActionName(const char* strLAction, const char* strLInvAction, const char* strRAction, const char* strRInvAction,
		const char* strFAction, const char* strBAction, const char* strFWalk) {}
	virtual void Release();

	virtual void PrepareTransition(float fTransitionSeconds) {}

	void ReleaseActionQueue();
	void ReleaseAllActions();

	void Tick(float dt);

	void SetActiveChannel(ApxActionChannel channel, float fBlendTime)
	{
		if(m_activeChannel == channel) return;
		m_activeChannel = channel;
		OnChangeActiveChannel(fBlendTime);
	}
	ApxActionChannel GetActiveChannel() const { return m_activeChannel; }

	bool PlayAction(const ApxActionInfo& apxActionInfo);
	bool IsPlayingAction() const 
	{
		return (GetCurrentAction() != NULL);
	}
	ApxActionInfo* GetCurrentAction() const { return m_pCurActionInfo; }
	APhysXU32 GetCurrentActionORMask() const
	{
		if(GetCurrentAction())
		{
			return GetCurrentAction()->mAnimInfo.mActionORMask;
		}
		else
			return ApxActionORMask_FullBody;
	}

	bool HasSuccedentAction() const { return !m_arrActionQueue.IsEmpty(); }

	virtual void SetActionLayer(ApxActionLayer apxActionLayer) { m_apxActionLayer = apxActionLayer; }
	ApxActionLayer GetActionLayer() const { return m_apxActionLayer;}

	virtual float GetCurActionSpan() { return -1.0; }

	virtual const char* GetFirstActionName() const { return 0; }
	virtual const char* GetNextActionName() const { return 0; }

	// set the action's playback speed...
	virtual void SetPlaySpeed(float fPlaySpeed) { m_fPlaySpeed = fPlaySpeed; }

	virtual void SetMoveYaw(float fOffForwardDeg) {}
	virtual void SetSpeed(float fSpeed) { }
	virtual bool IsUseWalkRun() const { return true; }
	virtual void SetUseWalkRun(bool bUseWalkRun) {  } 

protected:
	// return true if the action was play really;
	bool StartPlayCurAction();

	// have to be overridden by child classes...
	
	//----------------------------------------------
	// return true in most cases, however if currently we are playing the same animation 
	// and the apxAnimInfo.mRestart == false, just return false
	virtual bool PlayAnim(const ApxActionAnimInfo& apxAnimInfo) = 0;
	
	virtual bool IsAnimStopped(const ApxActionAnimInfo& apxAnimInfo) = 0;
	virtual void OnChangeActiveChannel(float fBlendTime) = 0;
	
	virtual void PreTick(float dt) {}
	virtual void TickAnim(float dt) {}
	
	virtual void OnActionStart(ApxActionInfo* pActionInfo);
	virtual void OnActionEnd(ApxActionInfo* pActionInfo);
	virtual void PreActionEnd(ApxActionInfo* pActionInfo, float fLeftTime);

	bool CanActionPlay(const ApxActionInfo& apxActionInfo);

protected:

	float m_fTimeAfterPlay;
	
	ApxActionInfo* m_pCurActionInfo;

	APhysXSimpleQueue<ApxActionInfo* > m_arrActionQueue;

	ApxActionLayer m_apxActionLayer;
	ApxActionChannel m_activeChannel;
	
	ApxActorBase* m_pActorBase;

	bool m_bNotifyActionEnd;

	float m_fPlaySpeed;

};

//----------------------------------------------
// Actor Base class...

enum UserInputFlag
{
	UIF_NONE			 = 0,
	UIF_MOVE_FORWARD     = (1<<0),
	UIF_MOVE_BACKWARD    = (1<<1),
	UIF_MOVE_LEFTSTRAFE	 = (1<<2),
	UIF_MOVE_RIGHTSTRAFE = (1<<3),
	UIF_MOVE_UP			 = UIF_MOVE_FORWARD,
	UIF_MOVE_DOWN		 = UIF_MOVE_BACKWARD,
	UIF_MOVE_LEFT		 = UIF_MOVE_LEFTSTRAFE,
	UIF_MOVE_RIGHT		 = UIF_MOVE_RIGHTSTRAFE,
	UIF_SPEED_INCREASE	 = (1<<4),
	UIF_SPEED_DECREASE   = (1<<5),

	UIF_TESTDIR_TURNPOS	 = (1<<16),
	UIF_TESTDIR_TURNNEG	 = (1<<17),
	UIF_TEST_MOVE		 = (1<<18),
};

// derive from this class to define user's callback.
class ApxActorActionReport : public IAssailable
{
	friend class ApxActorBase;
protected:
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
	virtual void OnPushObj(IPhysXObjBase* pPhysXObj) {}

	// for user's pre-land notification call back...
	virtual void PreLandNotify(float fTimeBeforeLand) {}

	virtual void OnUpdateMoveState(){}

	//----------------------------------------------
	// actor's action user report...
	virtual void OnActionStart(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo){}
	virtual void OnActionEnd(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo) {}
	virtual void OnChangeEquipment(){}

	//----------------------------------------------
	// actor's user input report...
	virtual UserInputFlag TickUserInput() { return UIF_NONE; }
	virtual void ApplyUserInput(UserInputFlag uifs) {}
};

class ApxValueBlender
{
public:
	ApxValueBlender(float fInitValue = 0.0f)
	{
		Init(fInitValue);
		m_fDeltaFactor = 0.0f;
		m_fBlendTime = 0.0f;
	}

	void Init(float fInitValue)
	{
		m_fCurValue = fInitValue;
	}

	float GetValue() { return m_fCurValue; }

	void SetTargetValue(float fTargetValue, float fBlendTime)
	{
		if(fBlendTime <= 1e-4f)
		{
			// just ignore the blend time...
			m_fCurValue = fTargetValue;
			m_fDeltaFactor = 0.0f;
			m_fBlendTime = 0.0f;
		}
		else
		{
			m_fBlendTime = fBlendTime;
			m_fDeltaFactor = (fTargetValue - m_fCurValue) / fBlendTime;

		}
	}

	void Tick(float dt)
	{
		if(::fabsf(m_fDeltaFactor) > 1e-5f)
		{
			m_fCurValue += dt * m_fDeltaFactor;
			m_fBlendTime -= dt;
			if(m_fBlendTime<0.0f)
			{
				m_fCurValue += m_fBlendTime * m_fDeltaFactor;
				m_fDeltaFactor = 0.0f;
			}
		}
	}

protected:
	// for compute the weight of this layer.
	float m_fCurValue;
	float m_fDeltaFactor;
	float m_fBlendTime;
};

class ApxActorBase : public ApxActorActionReport, public IKSolverArmInterface
{
private:
	enum
	{
		Apx_Actor_Equipment_Default,
	}; 

	//attack target
	struct TARGET_INFO
	{
		bool		bHasTarget;
		A3DVECTOR3	vPos;
		ECCounter	LockTime;
		//float		fRange;
		IAssailable*	pObj;
		TARGET_INFO()
		{
			Reset();
		}
		void Reset()
		{
			bHasTarget = false;
			vPos = 0.0f;
			//fRange = 0.0f;
			pObj = NULL;
		}
		float GetHDist(A3DVECTOR3& v)
		{
			A3DVECTOR3 vDir = vPos - v;
			vDir.y = 0.0f;
			return vDir.MagnitudeH();
		}
		void Tick(float fDeltaTime);

	};

	friend class ApxActorGroundMoveController;
	friend class ApxActionController;
	friend class ECPlayerBodyCtrl;

public:
	ApxActorBase() { SetToDefault();}
	virtual ~ApxActorBase() {}

	void SetToDefault()
	{
		m_vHeadFaceDir.zero();
		m_fTurnT = -1.0f;
		m_fTheta = 0.0f;

		m_vGroundTargetPos.set(0.0f);
		m_vGroundTargetDir.set(0.0f);
		m_bSetCurPosToTargetPos = false;

		m_pCurrentEquipment = 0;

		m_pGroundMoveController = NULL;
		for(int i=0; i<ApxActionLayer_Num; i++)
			m_ActionControllerPtrs[i] = NULL;

		m_pUserActionReport = NULL;
		m_pObjDyn = NULL;
		m_pRec = 0;
		m_backupCNL = APX_COLLISION_CHANNEL_INVALID;
		m_EnableTick = true;
		m_bInCrossOverBleand = false;

#ifdef _ANGELICA3
		m_pLIKTrigger = NULL;
		m_pRIKTrigger = NULL;

		m_pBlendWalkRunUpper = NULL;
		m_pBlendWalkRunLower = NULL;
#endif

		m_pLFIKSolver = NULL;
		m_pRFIKSolver = NULL;
		m_bEnableDamageFromPhysXContact = false;

		m_fOffSetY = 0;

		m_pCarryable = NULL;
		m_pPushable = NULL;
		m_pLHandTarget = NULL;
		m_pRHandTarget = NULL;
		m_iDelaySetPartPhysicsDMCounter = -1;

		m_TargetInfo.Reset();
		m_pBodyCtrl = NULL;

		m_vTestDir = A3DVECTOR3(0, 0, 1);
		m_bHasTestDir = false;
		m_bAutoTurn = true;

		m_BendBodyEnable = true;
		m_bUseBodyCtrl = false;
		m_pUpperBodyCtrl = NULL;

		m_bEnableIK = true;
		mFootIKMaxPitchDeg = 30.f;

#ifdef _ANGELICA21
		m_bUseIKFrame = false;
#endif
	}

	// init the action controller and ground-move controller.
	void Init(CPhysXObjDynamic* pObjDyn, const NxVec3& vExtents);
	void Release();

	void SetPos(const NxVec3& vPos)
	{
		if(m_pGroundMoveController)
			m_pGroundMoveController->SetPos(vPos);
	}
	const NxVec3& GetPos() const 
	{	
		static NxVec3 vZero(0.0f);
		if (m_pObjDyn->GetProperties()->QueryDrivenMode(IPropPhysXObjBase::DRIVEN_BY_PURE_PHYSX))
			return m_pObjDyn->GetGPos();

		if (m_pGroundMoveController)
			return m_pGroundMoveController->GetPos(); 
		else
			return vZero;
	}

	void SetExtents(const NxVec3& vExtents)
	{
		if(m_pGroundMoveController)
			m_pGroundMoveController->SetExtents(vExtents);
	}
	const NxVec3& GetExtents() const 
	{ 
		static NxVec3 vZero(0.0f);
		if(m_pGroundMoveController)
			return m_pGroundMoveController->GetExtents(); 
		else
			return vZero;
	}

	NxVec3 GetInputMoveDir(UserInputFlag uifs, const NxVec3& camHDirNow);

	// note: we ignore the y component of Dir and TargetPos in case of ground moving
	void SetGroundMoveDir(const NxVec3& vMoveDir);
	NxVec3 GetGroundMoveDir() const;
	void SetGroundMoveSpeed(float fSpeed);
	float GetGroundMoveSpeed() const;
	void GroundJump(float fUpSpeed) {if(m_pGroundMoveController)m_pGroundMoveController->Jump(fUpSpeed); }

	void SetGroundTargetPos(const NxVec3& vTargetPos);
	bool IsGroundTargetPosReached() const;

	//body direction in fact
	void SetHeadDir(const NxVec3& vHeadDir, bool bEnforce = false);
	const NxVec3& GetHeadDir() const { return m_vHeadFaceDir; }
	const NxVec3& GetHeadUp() const;

	CPhysXObjDynamic* GetObjDyn() const { return m_pObjDyn; }

	void SetUserActionReport(ApxActorActionReport* pUserActionReport)
	{
		m_pUserActionReport = pUserActionReport;
	}
	ApxActorGroundMoveController* GetGroundMoveController() const { return m_pGroundMoveController; }
	const ApxActionController* GetActionController(ApxActionLayer apxActionLayer) const  { return m_ActionControllerPtrs[apxActionLayer]; }
	
	// make sure layer is less than ApxActionLayer_Num
	bool PlayAction(ApxActionLayer layer, const ApxActionInfo& apxActionInfo);
	void SwitchToRagDoll(const ApxDamageInfo& apxDamageInfo);
	void CloseRagDoll();

	void SetEnableTick(bool bEnable) { m_EnableTick = bEnable; }
	bool GetEnableTick() const { return m_EnableTick; }
	void Tick(float dt);

	bool ChangeEquipment(const ApxWeaponInfo* pNewWeapon, APhysXScene* pPhysXScene, IEngineUtility* pEU);
	const ApxWeaponInfo* GetCurrentEquipment() const;
	bool UpdateWeaponPose(NxCapsule* pOutOldCapsule = 0, NxCapsule* pOutNewCapsule = 0, NxMat34* pOutOldPose = 0, NxMat34* pOutNewPose = 0);

	//@parameter strLFoot: bone name of left foot
	//           strRFoot: bone name of right foot
	bool InitFootIK(const AString& strLFoot, const AString& strRFoot);
	bool ReleaseFootIK();
	
	//enable & disable foot IK
	void EnableFootIK(bool bLFoot, bool bRFoot);
	void EnableDamageFromPhysXContact(bool bEnable) { m_bEnableDamageFromPhysXContact = bEnable; }
	bool IsEnableDamageFromPhysXContact() const { return m_bEnableDamageFromPhysXContact; }
	
	void SwitchFootIKYawCorrection();
	bool IsIKYawCorrectionEnabled();
	void IncreFootIKMaxPitchDeg();
	void DecreFootIKMaxPitchDeg();
	float mFootIKMaxPitchDeg;

	//@parameter strWalk: walk animation
	//           strRun:  run animation 
	bool InitWalkRunBlend(const AString& strWalk, const AString& strRun);
	bool ReleaseWalkRunBlend();
	
	void SetBlendWalkRunSpeed(float fSpeed, float fBlendSeconds = 0.0f);
#ifdef _ANGELICA3
	A3DAnimNodeBlendWalkRun* GetBlendWalkRunUpper() { return m_pBlendWalkRunUpper; }
	A3DAnimNodeBlendWalkRun* GetBlendWalkRunLower() { return m_pBlendWalkRunLower; }
#endif

	//hand ik for carrying
	void InitHandIK(const char* strLHand, const char* strRHand);
	void ReleaseHandIK();
	bool TryToCarry(CCarryable* pCarryable, const TCHAR* strAnimation, float fBindFrameRatio, const TCHAR* strLChild = NULL, const TCHAR* strRChild = NULL);
	bool TryToThrow(const char* strAnimation, float fReadyRatio, float fThrowRatio, float fSpeed);

	bool TryToPush(Pushable* pPushable, const char* strAnimation, float fBindFrameRatio);
	//fkeepIKTime: 保持Ik时间
	//fTransTime：IK过渡到FK时间
	bool RelasePushable(float fTransTime, float fKeepIKTime);

	//inherit from IKSolverArmInterface
	virtual bool PreSolver();
	virtual bool PostRefreshSkeleton();

	virtual void GetGPose(A3DMATRIX4& mtPose) const;
	virtual CPhysXObjDynamic* GetObject() { return GetObjDyn(); }
	virtual ApxActorBase* GetActorBase() { return this; }
	virtual bool OnAttackingCheckHit(const ApxAttackActionInfo& apxAttActInfo);
	virtual void OnAttackingHitEventPostNotify(const ApxDamageInfo& apxDamageInfo, IAssailable* pTarget, bool bTargetIsEnable);
	virtual bool OnPreAttackedQuery();
	virtual void OnPreAttackedNotify(ApxAttackActionInfo& inoutAAAI, const PhysXRBDesc* pAttacker);
	virtual bool OnAttackedHitEvent(const ApxDamageInfo& apxDamageInfo);

	void EnableIK(bool bEnable)
	{
		m_pObjDyn->GetSkinModel()->EnableIK(bEnable);
	}
	bool IsIKEnabled() const
	{
		return m_pObjDyn->GetSkinModel()->IsIKEnabled();
	}

	void SetHitTarget(IAssailable*	pObj, float fLockTime);
	void ResetHitTarget();
	bool HasHitTarget() const{ return m_TargetInfo.bHasTarget; }
	bool GetHitTargetPos(NxVec3& vPos) const;  

	void SetBlocked(bool bBlocked) { m_pGroundMoveController->SetBlocked(bBlocked); }
	bool IsBlocked() const { return m_pGroundMoveController->IsBlocked(); }

	void TurnTestDir(float fDeg);
	A3DVECTOR3 GetTestDir() const { return m_vTestDir; }
	void SetGroundMoveDirWithNoTurn(const NxVec3& vDir);
	//auto turn body
	bool IsAutoTurn() const { return m_bAutoTurn; }
	void SetAutoTurn(bool bAutoTurn) { m_bAutoTurn = bAutoTurn; } 
	void SetUseBodyCtrl(bool bUseBodyCtrl);
	bool IsUseBodyCtrl() const { return m_bUseBodyCtrl; }

	bool IsBendBodyEnable() const { return m_BendBodyEnable; }
	bool SetBendBodyEnable(bool bEnable);
	bool IsUpperBoneCtrlFree() const;
	void BendBody(const A3DVECTOR3& vDir, float fBendDeg, float fBendTime, float fReTime);
	void SetBlendAction(const char* strLAction, const char* strLInvAction, const char* strRAction, const char* strRInvAction,
		const char* strFAction, const char* strBAction, const char* strFWalk);

	void ResetCCBeforeRagdoll();
	bool CheckPosValidate(const A3DVECTOR3& vFootPos) const;

#ifdef _ANGELICA21
	void UpdateTransIKOffset();
	void SwitchToPhys(const char* strBone, float fPhyWeight = 1, bool bIncChild = true, bool bChangePhyState = true);
	const char* GetParentBone(const char* strBone);
	const char* GetFirstChildBone(const char* strBone);
	const char* GetNextSiblingBone(const char* strBone);
#endif
	static void RenderTest(A3DWireCollector* pWC);

protected:
	void UpdateObjPoseByMoveController();
	void UpdateTargetDir();
	void UpdateRotateBody(const NxVec3& vDir);

#ifdef _ANGELICA3
	//ik offset from goal position to actual position
	float GetIKGoalOffSetY(A3DIKTrigger* pTrigger);
#endif
	A3DIKSolver* CreateIKSolver(const AString& strSolverName, const AString& strBoneName);

private:
	void ApplyMoveDir(const NxVec3& vMoveDir, bool bEnforce = false);
	bool IsMoving() const;
	CPhysXObjDynamic* DoChangeEquipment(const ApxWeaponInfo& awiNewWeapon, APhysXScene* pPhysXScene, IEngineUtility& eu);
	CPhysXObjDynamic* DoChangeEquipmentXAWay(const ApxWeaponInfo& awiNewWeapon, APhysXScene* pPhysXScene, IEngineUtility& eu);
	void ReleaseEquipment(ApxWeaponInfo* pAWI);
	bool GetWeaponPose(NxMat34& pose, NxCapsule* pOutCapsule = 0);
	TString GetSocketLocalName(bool bIsHost, ApxSkinModelSocketType smsType, const TString& strName);

// to be overridden...
protected:
	virtual void OnStartJump(){ if(m_pUserActionReport) m_pUserActionReport->OnStartJump();}
	virtual void OnLand(){ if(m_pUserActionReport) m_pUserActionReport->OnLand();}
	virtual void OnReachApex(){if(m_pUserActionReport) m_pUserActionReport->OnReachApex();}

	// for user's swing call back...
	virtual void OnStartSwing(){if(m_pUserActionReport) m_pUserActionReport->OnStartSwing();}
	virtual void OnEndSwing(){if(m_pUserActionReport) m_pUserActionReport->OnEndSwing();}

	// for user's push action call back...
	virtual void OnPushObj(IPhysXObjBase* pPhysXObj) {if(m_pUserActionReport) m_pUserActionReport->OnPushObj(pPhysXObj);}

	// for user's pre-land notification call back...
	virtual void PreLandNotify(float fTimeBeforeLand) {if(m_pUserActionReport) m_pUserActionReport->PreLandNotify(fTimeBeforeLand);}
	virtual void OnUpdateMoveState() {if(m_pUserActionReport) m_pUserActionReport->OnUpdateMoveState();}
	//----------------------------------------------
	// actor's action user report...
	virtual void OnActionStart(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo);
	virtual void OnActionEnd(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo);
	virtual void PreActionEnd(ApxActionLayer apxActionLayer, ApxActionInfo* pActionInfo, float fLeftTime);

	virtual void OnChangeEquipment(){ if(m_pUserActionReport) m_pUserActionReport->OnChangeEquipment();}
	virtual UserInputFlag TickUserInput()
	{
		if (m_pUserActionReport)
			return m_pUserActionReport->TickUserInput();
		return UIF_NONE;
	}
	virtual void ApplyUserInput(UserInputFlag uifs) { if (m_pUserActionReport) m_pUserActionReport->ApplyUserInput(uifs); }

private:
	struct StatePara 
	{
		CAPhysXCCMgr::CCType ccType;
		int ccFlags;
		IPropPhysXObjBase::DrivenMode dm;
	};

protected:
	NxVec3 m_vHeadFaceDir;
	float m_fTurnT;
	float m_fTheta;

	NxVec3 m_vGroundTargetPos;
	NxVec3 m_vGroundTargetDir;
	bool m_bSetCurPosToTargetPos;

	ApxWeaponInfo* m_pCurrentEquipment;
	NxCapsule m_WeaponLastCapsule;
	NxMat34 m_WeaponLastPose;

	ApxActorGroundMoveController* m_pGroundMoveController;
	// currently, we have 3 action controller.
	ApxActionController* m_ActionControllerPtrs[ApxActionLayer_Num];

	ApxActorActionReport* m_pUserActionReport;
	CPhysXObjDynamic* m_pObjDyn;
	CRecording*	m_pRec;
	StatePara m_beforRagdollState;
	APhysXCollisionChannel m_backupCNL;

	bool m_EnableTick;
	bool m_bInCrossOverBleand;
	bool m_bEnableIK;
	bool m_bEnableDamageFromPhysXContact;

#ifdef _ANGELICA3
	//ik trigger
	A3DIKTrigger* m_pLIKTrigger;
	A3DIKTrigger* m_pRIKTrigger;

	A3DAnimNodeBlendWalkRun* m_pBlendWalkRunUpper;
	A3DAnimNodeBlendWalkRun* m_pBlendWalkRunLower;

#endif

	A3DIKSoverFootPlacement* m_pLFIKSolver;
	A3DIKSoverFootPlacement* m_pRFIKSolver;

	float m_fOffSetY;//offset of character model

	//hand ik for carrying
	CIKHandTarget* m_pLHandTarget;//搬箱子的左手IKTarget
	CIKHandTarget* m_pRHandTarget;//搬箱子的右手IKTarget
	CCarryable*    m_pCarryable;//
	Pushable*      m_pPushable;//things pushable
	int m_iDelaySetPartPhysicsDMCounter;

	TARGET_INFO m_TargetInfo;//hit target info
	ECPlayerBodyCtrl* m_pBodyCtrl;

	A3DVECTOR3 m_vTestDir;
	bool       m_bHasTestDir;//move toward test direction
	bool       m_bAutoTurn;//auto turn body
	bool       m_bUseBodyCtrl;//use body ctrl

	bool	m_BendBodyEnable;
	UpperBodyCtrl*  m_pUpperBodyCtrl;
	ActorState      m_ActorState;

#ifdef _ANGELICA21
	ApxValueBlender m_FootOffBlender;
	bool m_bUseIKFrame;
#endif
	
};

#endif