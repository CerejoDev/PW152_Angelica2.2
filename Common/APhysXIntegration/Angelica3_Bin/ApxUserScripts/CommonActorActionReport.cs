using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;
using System.Windows.Forms;
using System.Drawing;

class ENDelHitEffect : MEventNotify
{
    public ENDelHitEffect(MIObjBase obj)
    {
        m_pObj = obj;
    }
    public override void OnEventNotify(MTimerEvent timer)
    {
        MScene.Instance.ReleaseObject(m_pObj);
    }
    public override void OnNotifyDie(MIObjBase obj)
    {
        if (obj == m_pObj)
            m_pObj = null;
    }

    private MIObjBase m_pObj;
}

class CommonActorActionReport : MActorActionReport
{
    protected MIObjBase m_pObjBase = null;
    protected CommonActorBehavior m_pCmnActor = null;
    protected MPushable m_pushable = new MPushable();
    protected Random rand = null;

    MGroundMoveState m_LastState;
    bool m_bJump = false;
    bool m_bLanding = false;

    bool m_bPush = false;
    bool m_bLastPush = false;
    bool m_bHasPushed = false;//has been call onpush this frame

    protected bool m_bAttacking = false;

    protected int m_nAttackType = 1;//0固定攻击，1移动攻击，2滑步攻击

    public void InitCommonReport(MIObjBase pObjBase, CommonActorBehavior pCmnActor)
    {
        m_pObjBase = pObjBase;
        m_pCmnActor = pCmnActor;
        rand = CommonActorBehavior.GetRandom();
    }
    public override void OnActionEnd(MActionLayer apxActionLayer, MActionInfo pActionInfo)
    {
        base.OnActionEnd(apxActionLayer, pActionInfo);
        m_pCmnActor.OnActionEnd(apxActionLayer, pActionInfo);
    }
    public override void OnActionStart(MActionLayer apxActionLayer, MActionInfo pActionInfo)
    {
        base.OnActionStart(apxActionLayer, pActionInfo);
    }
    public override void OnChangeEquipment()
    {
        base.OnChangeEquipment();
    }
    public override bool OnAttackedPreHitEvent(MActorBase pCaster, int attackType, uint idxCasterTeam)
    {
        if (!m_pCmnActor.QueryLifeState(CommonActorBehavior.LifeState.LS_LIVE))
            return false;
        if (!m_pCmnActor.IsEnemyTeam(idxCasterTeam))
            return false;

        if (null == m_pCmnActor.MoveTarget)
            m_pCmnActor.SetMoveTarget(true, pCaster.GetObjDyn());
        Vector3 dir = pCaster.GetPos() - HostActor.GetPos();
        dir.Y = 0;
        dir.Normalize();
        HostActor.SetHeadDir(dir);
        Vector3 hitPos = HostActor.GetPos();
        hitPos = hitPos + dir * m_pCmnActor.CurrentAttackInfo.infoAttack.mAttackRange;
        m_pCmnActor.UpdateHitPosAndDir(ref hitPos, ref dir, false, false);

        if ((int)CommonActorBehavior.AttackType.AT_RANGED == attackType)
            return true;

//        float randValue = (float)rand.NextDouble();
//        if (randValue < m_pCmnActor.DodgeChance)
//            return false;
        return true;
    }

    public override bool OnPreAttackedQuery()
    {
        if (null != m_pCmnActor)
        {
            if (m_pCmnActor.QueryLifeState(CommonActorBehavior.LifeState.LS_DEAD))
                return false;
        }

        if (m_pCmnActor.QueryFightState(CommonActorBehavior.FightState.FS_ATTACK))
        {
            float val = (float)rand.NextDouble();
            if (0.5 < val)
                return false;
        }
        return true;
    }
    public override void OnPreAttackedNotify(MAttackActionInfo pAttActInfo, MPhysXRBDesc pAttacker)
    {
        if ((int)CommonActorBehavior.AttackType.AT_RAYHIT == pAttActInfo.mAttack.mAttackType)
        {
            m_pCmnActor.OnSetDefaultDamageInfo(ref pAttActInfo.mDamage);
        }   
        else
        {
            pAttActInfo.mAttack.mAttackType = (int)CommonActorBehavior.AttackType.AT_CONTACT;
            m_pCmnActor.OnSetDefaultDamageInfo(ref pAttActInfo.mDamage);
            if (null != pAttacker)
            {
                Vector3 vel = pAttacker.mContactRBLinearVelocity;
                float speed = vel.Normalize();
                float sv = speed * pAttacker.mSkeletonRBMass;
                pAttActInfo.mDamage.mHitMomentum = pAttActInfo.mDamage.mHitMomentum * sv;
                float fscale = sv * 2;
                pAttActInfo.mDamage.mHitDamage = pAttActInfo.mDamage.mHitDamage * fscale;
            }
        }
    }
    public override bool OnAttackingCheckHit(MAttackActionInfo pAttActInfo)
    {
        if (null == pAttActInfo.mAttack.mpTarget)
            return false;
        if (!pAttActInfo.mAttack.mpTarget.OnAttackedPreHitEvent(HostActor, pAttActInfo.mAttack.mAttackType, m_pCmnActor.TeamGroup))
            return false;
        if (pAttActInfo.mAttack.mIsPhysXDetection)
            return true;

        Vector3 posTarget = pAttActInfo.mAttack.mpTarget.GetCurrentPos();
        Vector3 dist = posTarget - HostActor.GetPos();
        float d = dist.Magnitude();
        if (pAttActInfo.mAttack.mAttackRange < d)
            return false;

        dist.Y = 0;
        dist.Normalize();
        Vector3 faceDir = HostActor.GetHeadDir();
        float dotVal = Vector3.DotProduct(faceDir, dist);
        float threshold = 0;
        if (0 > pAttActInfo.mAttack.mAttHalfRadianRange)
            threshold = 0.5f;
        else
            threshold = (float)System.Math.Cos(pAttActInfo.mAttack.mAttHalfRadianRange);
        if (threshold > dotVal)
            return false;

        return true;
    }
    public override void OnAttackingHitEventPostNotify(MDamageInfo pDamageInfo, MIAssailable pTarget, bool bTargetIsEnable)
    {
        if (bTargetIsEnable)
        {
            Vector3 dir = pTarget.GetCurrentPos() - HostActor.GetPos();
            dir.Y = 0;
            dir.Normalize();
            HostActor.SetHeadDir(dir);
        }
        else
        {
            MIObjBase next = m_pCmnActor.GetNextEnemy();
            m_pCmnActor.SetMoveTarget(false, next);
            MIPhysXObjBase nextObj = next as MIPhysXObjBase;
            if (null == nextObj)
                HostActor.SetHitTarget(null, 10);
            else
                HostActor.SetHitTarget(nextObj.GetMIAssailable(), 10);
            if (null != next)
            {
                Vector3 dir = next.GetGPos() - HostActor.GetPos();
                dir.Y = 0;
                dir.Normalize();
                HostActor.SetHeadDir(dir);

                Vector3 hitPos = HostActor.GetPos();
                hitPos = hitPos + dir * m_pCmnActor.CurrentAttackInfo.infoAttack.mAttackRange;
                m_pCmnActor.UpdateHitPosAndDir(ref hitPos, ref dir, false, false);
            }
        }
    }
    public override bool OnAttackedHitEvent(MDamageInfo pDamageInfo)
    {
        // try to play a hit gfx...
        MIObjBase objHitGfx = MScene.Instance.CreateObjectRuntime(MObjTypeID.OBJ_TYPEID_PARTICLE, "gfx\\AttackHit.gfx3", pDamageInfo.mHitPos, false, true, null, null, false);
        if (null != objHitGfx)
        {
            ENDelHitEffect actNotify = new ENDelHitEffect(objHitGfx);
            MScene.Instance.RegisterTimerEvent(actNotify, 5, MDelayType.DT_TIME, 1);
        }

        if (null != m_pCmnActor)
        {
            int damage = -(int)(pDamageInfo.mHitDamage + 0.5f);
            m_pCmnActor.ChangeCurrentHealthPoint(damage);
            bool bRtn = m_pCmnActor.QueryLifeState(CommonActorBehavior.LifeState.LS_DEAD);
            if (bRtn)
            {
                OnDead(pDamageInfo);
                return false;
            }
            m_pCmnActor.OnHitDamageNotify(pDamageInfo); 
        }

        bool bIsUsingBendBody = true;
        if (!HostActor.IsBendBodyEnable())
            bIsUsingBendBody = false;

        if (bIsUsingBendBody)
        {
            HostActor.BendBody(pDamageInfo.mHitDir, 90, 0.1f, 0.2f);
        }
        else
        {
            MActionAnimInfo AnimInfo = new MActionAnimInfo();
            AnimInfo.mActionName = m_pCmnActor.GetDamageActionName(pDamageInfo.mSourceFlags);
            AnimInfo.mLooping = false;
            AnimInfo.mRestart = true;
            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
            MActionInfo info = new MActionInfo(AnimInfo);
            info.mCoolDownTime = 0.0f;
            info.mIgnoreActionTime = 0.0f;
            info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
            info.mPlaySpeed = 1.5f;
            // HostActor.PlayAction(MActionLayer.MActionLayer_Passive, info);
            HostActor.PlayAction(MActionLayer.MActionLayer_Active, info);
        }
        return true;
    }
    private void OnDead(MDamageInfo pDamageInfo)
    {
        if (HostActor.MCarryableBox != null)
            HostActor.TryToThrow(null, 0, 0, 0);

        if (m_pCmnActor.DeadMethodToRagdoll)
        {
            // enter physX ragdoll
            if (0x03000000 <= MScene.GetAngelicaSDKVersion())
                pDamageInfo.mHitMomentum = 0.4f;
            else
                pDamageInfo.mHitMomentum = 1;
            HostActor.SwitchToRagDoll(pDamageInfo);
        }
        else
        {
            // play dead animation
            MActionAnimInfo AnimInfo = new MActionAnimInfo();
            AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActDeath;
            AnimInfo.mLooping = false;
            AnimInfo.mRestart = false;
            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_None;
            MActionInfo info = new MActionInfo(AnimInfo);
            info.mCoolDownTime = 0.0f;
            info.mIgnoreActionTime = 0.0f;
            info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
            HostActor.PlayAction(MActionLayer.MActionLayer_Active, info);
            AnimInfo = new MActionAnimInfo();
            AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActDeathLoop;
            AnimInfo.mLooping = true;
            AnimInfo.mRestart = false;
            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_None;
            info = new MActionInfo(AnimInfo);
            info.mCoolDownTime = 0.0f;
            info.mIgnoreActionTime = 0.0f;
            info.mPlayMode = MActionPlayMode.ActionPlayMode_NormalPlay;
            HostActor.PlayAction(MActionLayer.MActionLayer_Active, info);
        }
    }
    public override void OnRebornNotify()
    {
        if (m_pCmnActor.DeadMethodToRagdoll)
        {
            // leave physX ragdoll
            HostActor.CloseRagDoll();
        }
        else
        {
            // play reborn animation
            MActionAnimInfo RAnimInfo = new MActionAnimInfo();
            RAnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActStanding;
            RAnimInfo.mLooping = false;
            RAnimInfo.mRestart = false;
            RAnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_FullBody;
            MActionInfo infoR = new MActionInfo(RAnimInfo);
            infoR.mCoolDownTime = 0.0f;
            infoR.mIgnoreActionTime = 2.6f;
            infoR.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
            HostActor.PlayAction(MActionLayer.MActionLayer_Active, infoR);
        }
    }
    public override void OnStartSwing()
    {
        base.OnStartSwing();
        MActionAnimInfo AnimInfo = new MActionAnimInfo();
        AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActDynCCSwing;
        AnimInfo.mLooping = true;
        AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_FullBody;
        MActionInfo info = new MActionInfo(AnimInfo);
        info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
        HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
        m_LastState = MGroundMoveState.MActor_GroundMoveState_Swing;
    }
    public override void OnEndSwing()
    {
        base.OnEndSwing();
    }
    public override void OnLand()
    {
        base.OnLand();
        m_bJump = false;
        m_bLanding = false;
    }
    public override void OnPushObj(MIPhysXObjBase obj)
    {
        base.OnPushObj(obj);

        if (m_bHasPushed)//该帧已经调用过OnPushObj函数
            return;

        m_bHasPushed = true;

        m_LastState = MGroundMoveState.MActor_GroundMoveState_Push;
        m_bPush = true;
        if (m_bLastPush && m_pushable.GetMIPhysXObjBase() == obj)
            return;

        MLogWrapper.Instance.LogText(obj.GetObjName());

        HostActor.RelasePushable(0, 0);
        m_pushable.Init(obj);
        if (!HostActor.TryToPush(m_pushable, "Push", 0.0f))
        {
            m_pushable.Init(null);
            m_bPush = false;
            return;
        }
        
        MActionAnimInfo AnimInfo = new MActionAnimInfo();
        AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActPushing;
        AnimInfo.mLooping = true;
        AnimInfo.mRestart = false;
        AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
        MActionInfo info = new MActionInfo(AnimInfo);
        info.mCoolDownTime = 0.0f;
        info.mIgnoreActionTime = 0.0f;
        info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
        HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
    }
    public override void OnReachApex()
    {
        base.OnReachApex();
    }
    public override void OnStartJump()
    {
        base.OnStartJump();

        if (HostActor == null || !HostActor.IsHostActor())
            return;
        MActionAnimInfo AnimInfo = new MActionAnimInfo();
        AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActJumpStart;
        AnimInfo.mLooping = false;
        AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_UpperBody;
        MActionInfo info = new MActionInfo(AnimInfo);
        info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
        HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);

        m_bJump = true;
    }
    public override void OnUpdateMoveState()
    {
        base.OnUpdateMoveState();

        if (null != m_pCmnActor)
        {
            if (m_pCmnActor.QueryLifeState(CommonActorBehavior.LifeState.LS_DEAD))
                return;
        }

        MGroundMoveState state = HostActor.GetGroundMoveState();
        MActionAnimInfo AnimInfo = new MActionAnimInfo();

        if (!m_bPush)
        {
            if (m_bLastPush)
            {
                float fTransTime = 0, fKeepTime = 0;
                if (state == MGroundMoveState.MActor_GroundMoveState_StandInPlace && !HostActor.IsInAir() || state == MGroundMoveState.MActor_GroundMoveState_Push)
                {
                    fTransTime = 0.3f;
                    fKeepTime = 0.2f;
                }
                HostActor.RelasePushable(fTransTime, fKeepTime);
                m_pushable.Init(null);
            }
            if (state == MGroundMoveState.MActor_GroundMoveState_Move && !HostActor.IsInAir())
            {
                MActionInfo info = new MActionInfo(AnimInfo);
                float fDot = 1;
                if (HostActor.HasHitTarget())
                {
                    Vector3 vTargetDir = HostActor.GetHitTarget().GetCurrentPos() - HostActor.GetPos();
                    vTargetDir.Y = 0;
                    float fDist = vTargetDir.Normalize();
                    fDot = Vector3.DotProduct(HostActor.GetGroundMoveDir(), vTargetDir);
                    if (fDist > 30)
                        HostActor.ResetHitTarget();
                }

                if (fDot >= 0)
                {
                    AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActRunForward;
                    if (0x03000000 <= MScene.GetAngelicaSDKVersion())
                        AnimInfo.mBlendWalkRun = true;
                    else
                        AnimInfo.mBlendWalkRun = false;
                    if (m_nAttackType == 1 || !m_bAttacking)//移动攻击
                        info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
                }
                else
                {
                    AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActRunBack;
                    if (0x03000000 <= MScene.GetAngelicaSDKVersion())
                        AnimInfo.mBlendWalkRun = true;
                    else
                        AnimInfo.mBlendWalkRun = false;
                    if (m_nAttackType == 1 || !m_bAttacking)//移动攻击
                        info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
                }
                if (HostActor.HasHitTarget() && HostActor.IsUseBodyCtrl())
                    AnimInfo.mBlendWalkRun = false;
                AnimInfo.mLooping = true;
                AnimInfo.mRestart = false;

                AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_UpperBody;


                if (m_LastState != state)
                    info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
                m_LastState = state;
                HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
            }
            else if (state == MGroundMoveState.MActor_GroundMoveState_StandInPlace && !HostActor.IsInAir())
            {
                MActionInfo info = new MActionInfo(AnimInfo);
                OnUpdateMoveStandStateParameter(ref info);
                m_LastState = state;
                HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
            }
            else if (state == MGroundMoveState.MActor_GroundMoveState_InAir)
            {
                AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActJumpLoop;
                AnimInfo.mLooping = true;
                AnimInfo.mRestart = false;
                AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_UpperBody;

                MActionInfo info = new MActionInfo(AnimInfo);
                if (!m_bJump)
                    info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
                m_LastState = state;
                HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
            }
            else if (state == MGroundMoveState.MActor_GroundMoveState_Push)//内部认为是推的状态，外部条件不合适（无法找到合适的推的位置等）
            {
                MActionInfo info = new MActionInfo(AnimInfo);
                OnUpdateMoveStandStateParameter(ref info);
                if (m_bLastPush)
                    info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
                m_LastState = state;
                HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
            }
        }
        m_bLastPush = m_bPush;
        m_bPush = false;
        m_bHasPushed = false;
    }
    protected virtual void OnUpdateMoveStandStateParameter(ref MActionInfo ActionInfo)
    {
        ActionInfo.mAnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActStanding;
        ActionInfo.mAnimInfo.mLooping = true;
        ActionInfo.mAnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_FullBody;
        if (m_LastState != HostActor.GetGroundMoveState())
            ActionInfo.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
    }
    public override void PreLandNotify(float fTimeBeforeLand)
    {
        base.PreLandNotify(fTimeBeforeLand);
        if (fTimeBeforeLand > 0.2f)
            m_bLanding = false;

        if (fTimeBeforeLand < 0.2f && !m_bLanding && fTimeBeforeLand > 0.1f)
        {
            m_bLanding = true;
            MActionAnimInfo AnimInfo = new MActionAnimInfo();
            AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActJumpEnd;
            AnimInfo.mLooping = false;
            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_UpperBody;
            MActionInfo info = new MActionInfo(AnimInfo);
            info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
            HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
            m_bJump = true;
        }
    }
}