using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;
using System.Windows.Forms;
using System.Drawing;

class WomanActionReport : CommonActorActionReport
{
    class ThrowTarget
    {
        public Vector3 vAimPos;//扔的目标位置
        public bool bHasAim = false;//朝目标扔还是漫无目的的扔

        public float fMaxSpeed = 10;//最大出手速度，这里假设抛出方向为斜上方45°
        public float fDefSpeed = 7;//默认速度
    }

    class ENRecoverChannel : MEventNotify
    {
        public bool IsEmpty()
        {
            if ((null == m_pObj0) && (null == m_pObj1))
                return true;
            return false;
        }
        public void SetObject0(MPhysXObjDynamic obj, MCollisionChannel cnl)
        {
            m_pObj0 = obj;
            m_backUp0 = cnl;
        }
        public void SetObject1(MPhysXObjDynamic obj, MCollisionChannel cnl)
        {
            m_pObj1 = obj;
            m_backUp1 = cnl;
        }
        public override void OnEventNotify(MTimerEvent timer)
        {
            MLogWrapper.Instance.LogText("OnEventNotify");
            if (null != m_pObj0)
                m_pObj0.SetCollisionChannel(m_backUp0);
            if (null != m_pObj1)
                m_pObj1.SetCollisionChannel(m_backUp1);
        }
        public override void OnNotifyDie(MIObjBase obj)
        {
            if (obj == m_pObj0)
                m_pObj0 = null;
            if (obj == m_pObj1)
                m_pObj1 = null;
        }

        private MPhysXObjDynamic m_pObj0;
        private MPhysXObjDynamic m_pObj1;
        private MCollisionChannel m_backUp0;
        private MCollisionChannel m_backUp1;
    }

    private ThrowTarget _throwTarget = new ThrowTarget();

    private bool m_bIsCarrying = false;

    string m_strTestAction = "人族_战士_XX一击";
    //string m_strTestAction = "魔族_狂战_乱砍";

    //string m_strTestAction = "魔族_狂战_压制横扫";//转身
    //string m_strTestAction = "猎人_淬毒箭_执行";//转身
    //string m_strTestAction = "魔族_狂战_普攻1";
    //string m_strTestAction = "魔族_狂战_普攻2";
    //string m_strTestAction = "魔族_狂战_未名斩";
    //string m_strTestAction = "魔族_狂战_关门打狗";
    //string m_strTestAction = "人族_剑仙_普攻1";
    //string m_strTestAction = "人族_剑仙_普攻2";

    //string m_strTestAction = "人族_剑仙_霞切";//扭曲变形
    //string m_strTestAction = "人族_战士_寸击";
    //string m_strTestAction = "人族_战士_击破";
    //string m_strTestAction = "人族_战士_乱舞";
    //string m_strTestAction = "人族_战士_普攻1";
    //string m_strTestAction = "人族_战士_普攻2";//扭曲变形

    public override void Init(MIObjBase pObjBase)
    {
        CommonActorBehavior cmnActor = null;
        String[] scripts = pObjBase.GetScriptNames();
        int nCount = scripts.Length;
        for (int i = 0; i < nCount; ++i)
        {
            if ("CommonActorBehavior.cs" == scripts[i])
            {
                MIScriptBehavior script = pObjBase.ScriptBehaviors[i];
                cmnActor = (CommonActorBehavior)script;
            }
        }
        cmnActor.RebornInterval = 7;
        cmnActor.AttackCoolDown = 0;
        base.InitCommonReport(pObjBase, cmnActor);
    }
    public override void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags)
    {
        base.OnKeyDown(nChar, nRepCnt, nFlags);

        if (HostActor == null || !HostActor.IsHostActor())
            return;

        if (nChar == (uint)Keys.J)
        {
            MScene.Instance.JumpMainActor();
        }
        else if (nChar == (uint)Keys.D1)
        {
            MIAssailable pTargetObj = m_pCmnActor.ActiveEnemy;
            if (null != pTargetObj)
                m_pCmnActor.SetAttackEnemyEnable(true);
            else
            {
                Vector3 hitDir = HostActor.GetHeadDir();
                Vector3 hitPos = HostActor.GetPos();
                hitPos = hitPos + hitDir * m_pCmnActor.CurrentAttackInfo.infoAttack.mAttackRange;
                m_pCmnActor.PlayAttack(hitPos, hitDir, pTargetObj, -1);
            }
        }
        else if (nChar == (uint)Keys.N)
        {
            m_pCmnActor.ChangeCurrentAttackType();
        }
        else if (nChar == (uint)Keys.NumPad1)
        {
            MActionAnimInfo AnimInfo = new MActionAnimInfo();
            AnimInfo.mActionName = m_pCmnActor.CurrentAttackInfo.strActName;
            AnimInfo.mLooping = false;
            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;

            MActionInfo info = new MActionInfo(AnimInfo);
            HostActor.PlayAction(MActionLayer.MActionLayer_Active, info);
        }
        else if (nChar == (uint)Keys.NumPad2)
        {
            MActionAnimInfo AnimInfo = new MActionAnimInfo();
            AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActAttSingle;
            AnimInfo.mLooping = false;
            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
            
            MComboAttackActionInfo info = new MComboAttackActionInfo(AnimInfo);
            info.mAttack = new MAttackInfo();
            info.mAttack = m_pCmnActor.CurrentAttackInfo.infoAttack;
            info.mAttack.mIsPhysXDetection = m_pCmnActor.CurrentAttackInfo.infoAttack.mIsPhysXDetection;
            info.mAttack.mAttackType = m_pCmnActor.CurrentAttackInfo.infoAttack.mAttackType;
            info.mAttack.mAttackRange = m_pCmnActor.CurrentAttackInfo.infoAttack.mAttackRange;
            info.mAttack.mAttHalfRadianRange = m_pCmnActor.CurrentAttackInfo.infoAttack.mAttHalfRadianRange;
            info.mAttack.mDamageDelayStartSec = m_pCmnActor.CurrentAttackInfo.infoAttack.mDamageDelayStartSec;
            info.mAttack.mDamageDurationSec = m_pCmnActor.CurrentAttackInfo.infoAttack.mDamageDurationSec;
            info.mAttack.mTeamGroup = m_pCmnActor.TeamGroup;
            info.mDamage = new MDamageInfo();
            info.mDamage.mpAttacker = HostActor;
            info.mDamage.mHitMomentum = 0.3f;
            info.mDamage.mHitDamage = 100;
			info.mSeqAnim[0] = new MActionAnimInfo();
            info.mSeqAnim[0].mActionName = m_pCmnActor.GetAnimationRes.strActAttCombo2;
            info.mSeqAnim[0].mLooping = false;
            info.mSeqAnim[0].mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
//            info.mSeqActDamDelayStartSec[0] = CurrentAttackInfo.infoAttack.mDamageDelayStartSec;
//            info.mSeqActDamDurationSec[0] = CurrentAttackInfo.infoAttack.mDamageDurationSec;
            HostActor.PlayAction(MActionLayer.MActionLayer_Active, info);
        }
        else if (nChar == (uint)Keys.NumPad3)
        {
            MActionAnimInfo AnimInfo = new MActionAnimInfo();
            AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActDamage;
            AnimInfo.mLooping = false;
            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;

            MActionInfo info = new MActionInfo(AnimInfo);
            info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
            info.mCoolDownTime = 2;
            HostActor.PlayAction(MActionLayer.MActionLayer_Passive, info);
        }
        else if (nChar == (uint)Keys.NumPad4)
        {
            MScene.Instance.ChangeMainActorPushForce(3);
        }
        else if (nChar == (uint)Keys.NumPad5)
        {
            MScene.Instance.ChangeMainActorPushForce(-3);
        }
        else if (nChar == (uint)Keys.I)
        {
            MScene.Instance.SwitchMainActorCCType(MCCType.CC_UNKNOWN_TYPE);
        }
        else if (nChar == (uint)Keys.E)
        {
            if (HostActor.MCarryableBox != null)
            {
                if (!HostActor.TryToThrow("搬箱子_抛后恢复", 0.4f, 0.2f, _throwTarget.fDefSpeed))
                    MLogWrapper.Instance.LogText("try to throw fail");
                else
                {
                    _throwTarget.bHasAim = false;
                }
            }
        }
        else if (nChar == (uint)Keys.B)
        {
            //enable IK or not
            bool bEnable = HostActor.IsIKEnabled();
            HostActor.EnableIK(!bEnable);
        }
            ////can not work... to do 调试攻击后无法跑动
        else if (nChar == (uint)Keys.T)
        {
            if (HostActor.HasHitTarget())
                HostActor.SetHitTarget(HostActor.GetHitTarget(), 10);

            MActionAnimInfo AnimInfo = new MActionAnimInfo();

            AnimInfo.mActionName = m_strTestAction;
            AnimInfo.mLooping = false;
            AnimInfo.mRestart = false;
            if (m_nAttackType == 1)//移动攻击
                AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
            else//固定攻击或滑步攻击
                AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_None;


            MAttackInfo AttackInfo = new MAttackInfo();
            AttackInfo = m_pCmnActor.CurrentAttackInfo.infoAttack;
            MDamageInfo DamageInfo = new MDamageInfo();
            DamageInfo.mHitDamage = 40;
            DamageInfo.mHitMomentum = 5;
            DamageInfo.mpAttacker = HostActor;
            MAttackActionInfo info = new MAttackActionInfo(AnimInfo, AttackInfo, DamageInfo);

            //MActionInfo info = new MActionInfo(AnimInfo);
            if (m_nAttackType == 0 || m_nAttackType == 2)//固定攻击滑步攻击冲掉Move通道的动作
                info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;

            if (m_nAttackType == 0)
                HostActor.SetBlocked(true);

            m_bAttacking = true;

            if (m_nAttackType == 0 || m_nAttackType == 2)//固定攻击滑步攻击
                HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
            else//移动攻击
                HostActor.PlayAction(MActionLayer.MActionLayer_Active, info);

        }
        else if (nChar == (uint)Keys.V)//change attack type
        {
            m_nAttackType++;
            m_nAttackType = m_nAttackType % 3;
        }
        else if (nChar == (uint)Keys.NumPad7)
        {
            HostActor.BendBody(HostActor.GetTestDir(), 50, 1, 1);
        }
        else if (nChar == (uint)Keys.NumPad9)
        {
            if (HostActor.IsBendBodyEnable())
                HostActor.SetBendBodyEnable(false);
            else
                HostActor.SetBendBodyEnable(true);
        }
    }

    public override void OnActionEnd(MActionLayer apxActionLayer, MActionInfo pActionInfo)
    {
        base.OnActionEnd(apxActionLayer, pActionInfo);

        if (pActionInfo.mAnimInfo.mActionName == m_strTestAction)
        {
            m_bAttacking = false;
            if (m_nAttackType == 0)
                HostActor.SetBlocked(false);
        }
    }

    protected override void OnUpdateMoveStandStateParameter(ref MActionInfo ActionInfo)
    {
        if (m_bIsCarrying)
        {
            ActionInfo.mAnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActPickHoldOn;
            ActionInfo.mAnimInfo.mLooping = true;
            ActionInfo.mAnimInfo.mRestart = false;
            ActionInfo.mAnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
            ActionInfo.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
        }
        else
        {
            base.OnUpdateMoveStandStateParameter(ref ActionInfo);
            if (HostActor.IsBlocked())
            {
                ActionInfo.mAnimInfo.mActionName = "";
            }
        }
    }
    public override void OnCarryingStateChanged(MCarryableState oldState, MCarryableState newState)
    {
        MActionAnimInfo AnimInfo = new MActionAnimInfo();
        if (oldState == MCarryableState.Free && newState == MCarryableState.ToHold)
        {
            m_bIsCarrying = false;
            AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActPickUpSTH;
            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
            AnimInfo.mLooping = false;
            AnimInfo.mRestart = false;
            MActionInfo info = new MActionInfo(AnimInfo);
            info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
            HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
        }
        else if (oldState == MCarryableState.ToHold && newState == MCarryableState.Hold)
        {
            m_bIsCarrying = true;
            AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActPickHoldOn;
            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
            AnimInfo.mLooping = true;
            AnimInfo.mRestart = false;
            MActionInfo info = new MActionInfo(AnimInfo);
            info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
            HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
        }
        else if (oldState == MCarryableState.Hold && newState == MCarryableState.ToFree)
        {
            m_bIsCarrying = false;
            AnimInfo.mActionName = m_pCmnActor.GetAnimationRes.strActThrowOut;
            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
            AnimInfo.mLooping = false;
            AnimInfo.mRestart = false;
            MActionInfo info = new MActionInfo(AnimInfo);
            info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
            HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);
        }
        else if (oldState == MCarryableState.ToFree && newState == MCarryableState.Free)
        {
            MPhysXObjDynamic dynObj = HostActor.MCarryableBox.PhysXObjBase as MPhysXObjDynamic;
            if (dynObj != null)
            {
                dynObj.SetDrivenModeEnforce(MDrivenMode.DRIVEN_BY_PURE_PHYSX);
                float fVel = _throwTarget.fDefSpeed;
                if (_throwTarget.bHasAim)
                {
                    AABB aabb = new AABB();
                    dynObj.GetAABB(ref aabb);
                    //计算速度大小，假设速度方向为斜上方45°
                    Vector3 vDelta = _throwTarget.vAimPos - aabb.Center;
                    

                    float g = Math.Abs(MScene.Instance.Gravity.Y);
                    float fHorizonLen = vDelta.MagnitudeH();
                    fVel = _throwTarget.fMaxSpeed;
                    if (fHorizonLen > vDelta.Y)
                    {
                        fVel = fHorizonLen * (float)Math.Sqrt(g / (fHorizonLen - vDelta.Y));
                        if (fVel > _throwTarget.fMaxSpeed)
                            fVel = _throwTarget.fMaxSpeed;
                    }
                }
                Vector3 vDir = HostActor.GetHeadDir();
                vDir.Normalize();
                vDir.Y += 1;
                vDir.Normalize();
                dynObj.SetLinearVelocity(vDir * fVel);

                ENRecoverChannel cnlNotify = new ENRecoverChannel();
                MCollisionChannel cnlHost = MCollisionChannel.CNL_UNKNOWN;
                if (HostActor.GetObjDyn().GetCollisionChannel(ref cnlHost))
                {
                    if (MCollisionChannel.CNL_COMMON == cnlHost)
                    {
                        HostActor.GetObjDyn().SetCollisionChannel(MCollisionChannel.CNL_INDEPENDENT);
                        cnlNotify.SetObject0(HostActor.GetObjDyn(), cnlHost);
                    }
                }
                MCollisionChannel cnlThrow = MCollisionChannel.CNL_UNKNOWN;
                if (dynObj.GetCollisionChannel(ref cnlThrow))
                {
                    if (MCollisionChannel.CNL_COMMON == cnlThrow)
                    {
                        dynObj.SetCollisionChannel(MCollisionChannel.CNL_INDEPENDENT);
                        cnlNotify.SetObject1(dynObj, cnlThrow);
                    }
                }
                if (cnlHost == cnlThrow)
                {
                    if (cnlHost != MCollisionChannel.CNL_COMMON)
                    {
                        dynObj.SetCollisionChannel(MCollisionChannel.CNL_INDEPENDENT);
                        cnlNotify.SetObject1(dynObj, cnlThrow);
                    }
                }
                if (!cnlNotify.IsEmpty())
                {
                    float dt = 1.0f / fVel;
                    if (0.3f > dt)
                        dt = 0.3f;
                    if (1.0f < dt)
                        dt = 1.0f;
                    MScene.Instance.RegisterTimerEvent(cnlNotify, dt, MDelayType.DT_TIME, 1);
                    MLogWrapper.Instance.LogText("set to PhysX driven");
                }
            }
        }
        else if (oldState == MCarryableState.Hold && newState == MCarryableState.Free)
        {
            MPhysXObjDynamic dynObj = HostActor.MCarryableBox.PhysXObjBase as MPhysXObjDynamic;
            if (dynObj != null)
            {
                dynObj.SetDrivenModeEnforce(MDrivenMode.DRIVEN_BY_PURE_PHYSX);
            }
            m_bIsCarrying = false;
        }
        base.OnCarryingStateChanged(oldState, newState);
    }

    private void CarrySomething(Point pt, MPhysRay ray)
    {
        MPhysRayTraceHit hitInfo = new MPhysRayTraceHit();
        //pick up object
        if (!MScene.Instance.RayTraceObject(pt, ray, out hitInfo, false))
            return;

        MIPhysXObjBase obj = hitInfo.obj as MIPhysXObjBase;
        if (obj == null)
        {
            MLogWrapper.Instance.LogText("hit none");
            return;
        }

        MCarryable carryable = new MCarryable();
        carryable.SetActorActionReport(this);
        if (!carryable.Init(obj))
        {
            MLogWrapper.Instance.LogText(@"MCarryable init error");
            return;
        }

        AABB aabb = new AABB();
        obj.GetAABB(ref aabb);

        Vector3 vDir = aabb.Center - HostActor.GetPos();
        vDir.Y = 0;
        float len = vDir.Normalize();
        float fDot = Vector3.DotProduct(vDir, HostActor.GetHeadDir());

        MLogWrapper.Instance.LogText(vDir.ToString());
        MLogWrapper.Instance.LogText("obj aabb pos: " + aabb.Center.ToString());

        carryable.GetModelAABB(ref aabb);
        float lenLimit = Math.Max(aabb.Extents.X, aabb.Extents.Y);
        lenLimit = Math.Max(lenLimit, aabb.Extents.Z);
        lenLimit = Math.Max(lenLimit * 1.7f, 0.8f);

        if (len < lenLimit && fDot > 0.5f)
        {
            //try to catch
            string strLHandle = "lhandle", strRHandle = "rhandle";//左右手挂点

            if (HostActor.TryToCarry(carryable, "搬箱子_拾取", 0.25f, strLHandle, strRHandle))
            //if (HostActor.TryToCarry(carryable, "羽族地面站立", 0.25f))
                carryable.PhysXObjBase.SetDrivenModeEnforce(MDrivenMode.DRIVEN_BY_ANIMATION);
            else
                MLogWrapper.Instance.LogText(@"catch object fail");
        }
        else
        {
            string str = "len : " + len + " fdot : " + fDot;
            MLogWrapper.Instance.LogText(str + " can not catch it ");
        }
    }
    private bool TryAttackTarget(ref MPhysRayTraceHit hitInfo)
    {
        if (null == hitInfo.obj)
            return false;

        if (m_pObjBase == hitInfo.obj)
            return false;

        MIObjBase parent = hitInfo.obj.GetParent();
        while (null != parent)
        {
            if (parent == m_pObjBase)
                return false;
            parent = parent.GetParent();
        }

        CommonActorBehavior cmnActor = null;
        String[] scripts = hitInfo.obj.GetScriptNames();
        int nCount = scripts.Length;
        for (int i = 0; i < nCount; ++i)
        {
            MIScriptBehavior script = hitInfo.obj.ScriptBehaviors[i];
            cmnActor = script as CommonActorBehavior;
            if (null != cmnActor)
                break;
        }
        if (null != cmnActor)
        {
            if (cmnActor.QueryLifeState(CommonActorBehavior.LifeState.LS_LIVE))
            {
                if (cmnActor.IsEnemyTeam(m_pCmnActor.TeamGroup))
                {
                    m_pCmnActor.SetMoveTarget(true, hitInfo.obj);
                    Vector3 hitGPos = hitInfo.vHit;
                    Vector3 hitGDir = hitGPos - HostActor.GetPos();
                    m_pCmnActor.UpdateHitPosAndDir(ref hitGPos, ref hitGDir, true, false);
                    return true;
                }
            }
        }

        ////for test, attack Stupid monster
        for (int i = 0; i < nCount; i++)
        {
            MIScriptBehavior script = hitInfo.obj.ScriptBehaviors[i];
            StupidMonster monster = script as StupidMonster;
            if (null != monster)
            {
                m_pCmnActor.SetMoveTarget(true, hitInfo.obj);
                Vector3 hitGPos1 = hitInfo.vHit;
                Vector3 hitGDir1 = hitGPos1 - HostActor.GetPos();
                m_pCmnActor.UpdateHitPosAndDir(ref hitGPos1, ref hitGDir1, true, false);
                return true;
            }
        }

        return false;
    }
    public override void OnLButtonDown(int x, int y, uint nFlags)
    {
        Point pt = new Point(x, y);
        MPhysRay ray = new MPhysRay();
        if (!MScene.Instance.ScreenPosToRay(pt, ref ray))
            return;

        if (MInput.Instance.IsKeyDown((int)Keys.ControlKey))
        {
            CarrySomething(pt, ray);
        }
        else
        {
            if (!MInput.Instance.IsKeyDown((int)Keys.ShiftKey))
            {
                Vector3 vRayHitPos;
                MPhysRayTraceHit hitInfo = new MPhysRayTraceHit();
                if (MScene.Instance.RayTraceObject(pt, ray, out hitInfo, false))
                {
                    if (TryAttackTarget(ref hitInfo))
                        return;
                    if (m_pObjBase == hitInfo.obj)
                        return;
                    if (null != hitInfo.obj)
                        vRayHitPos = hitInfo.obj.GetGPos();
                }

                if (!MScene.Instance.GetTerrainPosByCursor(x, y, out vRayHitPos))
                    return;

                MMainActor actor = MScene.Instance.GetCurrentMA();
                if (actor != null)
                {
                    if (!actor.IsInAir())
                    {
                        actor.WalkTo(vRayHitPos);
                        actor.UpdateTargetPT(x, y);
                        actor.SetLButtonDown();
                        m_pCmnActor.SetMoveTarget(false, null);
                    }
                }
            }
        }
    }
    
    public override void OnMouseMove(int x, int y, uint nFlags)
    {
        MMainActor actor = MScene.Instance.GetCurrentMA();
        if (actor != null)
        {
            if (actor.IsKeepWalking())
                actor.UpdateTargetPT(x, y);
        }
    }

    public override void OnLButtonUp(int x, int y, uint nFlags)
    {
        MMainActor actor = MScene.Instance.GetCurrentMA();
        if (actor != null)
            actor.SetLButtonUp();
    }

    public override void OnMButtonUp(int x, int y, uint nFlags)
    {
        Point pt = new Point(x, y);
        MPhysRay ray = new MPhysRay();
        if (!MScene.Instance.ScreenPosToRay(pt, ref ray))
            return;

        MPhysRayTraceHit hitInfo = new MPhysRayTraceHit();

        if (HostActor.MCarryableBox != null)//throw the box
        {
            if (MScene.Instance.RayTraceObject(pt, ray, out hitInfo, true))
            {
                if (HostActor.MCarryableBox != null)
                {
                    if (!HostActor.TryToThrow("搬箱子_抛后恢复", 0.4f, 0.2f, _throwTarget.fDefSpeed))
                        MLogWrapper.Instance.LogText("try to throw fail");
                    else
                    {
                        //set direction to the aim
                        Vector3 vDir = hitInfo.vHit - HostActor.GetPos();
                        vDir.Y = 0;
                        vDir.Normalize();
                        HostActor.SetHeadDir(vDir);

                        _throwTarget.vAimPos = hitInfo.vHit;
                        _throwTarget.bHasAim = true;
                    }
                }
            }
        }
    }
    public override void OnRButtonDown(int x, int y, uint nFlags)
    {

    }
    public override void OnRButtonUp(int x, int y, uint nFlags)
    {
        if (m_pCmnActor.CurrentAttackInfo.mBasicAction)
            return;

        Point pt = new Point(x, y);
        MPhysRay ray = new MPhysRay();
        if (!MScene.Instance.ScreenPosToRay(pt, ref ray))
            return;

        Vector3 vRayHitPos;
        if (!MScene.Instance.GetTerrainPosByCursor(x, y, out vRayHitPos))
            return;

        Vector3 hitPos = HostActor.GetPos();
        Vector3 hitDir = vRayHitPos - hitPos;
        hitDir.Y = 0;
        float fDist = hitDir.Normalize();
        float attRange = m_pCmnActor.CurrentAttackInfo.infoAttack.mAttackRange;
        if (attRange < fDist)
            fDist = attRange;
        Vector3 hitDist = hitDir * fDist;
        hitPos = hitPos + hitDist;
        if (m_pCmnActor.PlayAttack(hitPos, hitDir, null, -1))
            HostActor.SetHeadDir(hitDir, true);
    }
    public override MUserInputFlag TickUserInput()
    {
        MUserInputFlag rtnFlags = base.TickUserInput();
        if (HostActor == null || !HostActor.IsHostActor())
            return rtnFlags;

        if (MInput.Instance.IsKeyDown((int)Keys.W))
            rtnFlags |= MUserInputFlag.MUIF_MOVE_UP;
        if (MInput.Instance.IsKeyDown((int)Keys.S))
            rtnFlags |= MUserInputFlag.MUIF_MOVE_DOWN;
        if (MInput.Instance.IsKeyDown((int)Keys.A))
            rtnFlags |= MUserInputFlag.MUIF_MOVE_LEFT;
        if (MInput.Instance.IsKeyDown((int)Keys.D))
            rtnFlags |= MUserInputFlag.MUIF_MOVE_RIGHT;

        if (MInput.Instance.IsKeyDown((int)Keys.PageUp))
            rtnFlags |= MUserInputFlag.MUIF_TESTDIR_TURNPOS;
        if (MInput.Instance.IsKeyDown((int)Keys.PageDown))
            rtnFlags |= MUserInputFlag.MUIF_TESTDIR_TURNNEG;
        if (MInput.Instance.IsKeyDown((int)Keys.NumPad8))
            rtnFlags |= MUserInputFlag.MUIF_TEST_MOVE;

        if (MInput.Instance.IsKeyDown((int)Keys.Add))
            rtnFlags |= MUserInputFlag.MUIF_SPEED_INCREASE;
        if (MInput.Instance.IsKeyDown((int)Keys.Subtract))
            rtnFlags |= MUserInputFlag.MUIF_SPEED_DECREASE;
        return rtnFlags;
    }

    public override void ApplyUserInput(MUserInputFlag uifs)
    {
        Vector3 vDir = HostActor.GetInputMoveDir(uifs);
        if (!vDir.IsZero())
        {
            m_pCmnActor.SetMoveTarget(false, null);
            HostActor.SetGroundMoveDir(vDir);
        }

        if (0 != (uifs & MUserInputFlag.MUIF_TESTDIR_TURNPOS))
            HostActor.TurnTestDir(1);
        if (0 != (uifs & MUserInputFlag.MUIF_TESTDIR_TURNNEG))
            HostActor.TurnTestDir(-1);
        if (0 != (uifs & MUserInputFlag.MUIF_TEST_MOVE))
        {
            vDir = HostActor.GetTestDir();
            Vector3 vHead = HostActor.GetHeadDir();
            Vector3 vUp = new Vector3(0, 1, 0);
            Vector3 vLeft = Vector3.CrossProduct(vHead, vUp);

            //vDir = vHead + vLeft;
            //vDir = vLeft;
            //vDir.Normalize();

            HostActor.SetGroundMoveDirWithNoTurn(vDir);
        }

        if (0 != (uifs & MUserInputFlag.MUIF_SPEED_INCREASE))
        {
            float speed = HostActor.GetGroundMoveSpeed() + 0.05f;
            if (speed <= 10)
                HostActor.SetGroundMoveSpeed(speed);
        }
        if (0 != (uifs & MUserInputFlag.MUIF_SPEED_DECREASE))
        {
            float speed = HostActor.GetGroundMoveSpeed() - 0.05f;
            if (speed > 0)
                HostActor.SetGroundMoveSpeed(speed);
        }
    }
}