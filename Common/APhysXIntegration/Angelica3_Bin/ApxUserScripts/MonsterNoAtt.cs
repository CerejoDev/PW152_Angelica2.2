using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;


public class MonsterNoAtt : CommonActorBehavior
{
    public float MoveRadius
    {
        get { return moveR; }
        set
        {
            if (0 < value)
                moveR = value;
        }
    }
    public float CatchRadius
    {
        get { return catchR; }
        set
        {
            if (0 < value)
                catchR = value;
        }
    }
    public Vector3 centerPos;
    private float moveR = 10;
    private float catchR = 100;

    private bool IsRunSpeed = false;
    private float fRunSpeed = 3.0f;
    private float fWalkSpeed = 1.5f;
    private float idleTime = -1;
    private float coolTime = -1;

    protected MIPhysXObjBase m_PhysXObj = null;

    public override void Start()
    {
        m_PhysXObj = (MIPhysXObjBase)ObjBase;
        base.Start();
        MonsterActionReport action = new MonsterActionReport();
        m_actorBase.SetMActionReport(action);
        base.Color = 0xff00ff00;
        centerPos = ObjBase.GetGPos();
        IsRunSpeed = false;
        MaxHealthPoint = 200;
        CurrentHealthPoint = 200;
        TeamGroup = 2;
        AutoComboAttack = true;
        EyeshotRadius = 5;
    }

    protected Vector3 RandomPos()
    {
        Vector3 pos = centerPos;

        double r = MoveRadius * rand.NextDouble();
        double angle = Math.PI * rand.NextDouble() * 2;
        pos.X = (float)(pos.X + r * Math.Cos(angle));
        pos.Z = (float)(pos.Z + r * Math.Sin(angle));
        pos.Y = MScene.Instance.GetTerrainHeight(pos.X, pos.Z);
        return pos;
    }
    protected void SetRunSpeed()
    {
        if (!IsRunSpeed)
        {
            IsRunSpeed = true;
            m_actorBase.SetGroundMoveSpeed(fRunSpeed);
        }
    }
    protected void SetWalkSpeed()
    {
        if (IsRunSpeed)
        {
            IsRunSpeed = false;
            m_actorBase.SetGroundMoveSpeed(fWalkSpeed);
        }
    }
    protected bool IsOutCatchField(ref Vector3 pos)
    {
        Vector3 distCenter = pos - centerPos;
        float dC = distCenter.Magnitude();
        if (CatchRadius < dC)
            return true;
        return false;
    }
    protected virtual void OnEnterStandState()
    {
        if (QueryMoveState(CommonActorBehavior.MoveState.MS_STAND))
            return;
        base.stateMove = CommonActorBehavior.MoveState.MS_STAND;
        idleTime = (float)(1.5 + 3 * rand.NextDouble());
        Vector3 MonsterPos = m_actorBase.GetPos();
        m_actorBase.SetGroundTargetPos(MonsterPos);
    }
    protected virtual void OnEnterLeisureState()
    {
//        if (QueryMoveState(CommonActorBehavior.MoveState.MS_LEISURE))
//            return;
//        base.stateMove = CommonActorBehavior.MoveState.MS_LEISURE;
        idleTime = (float)(1.5 + 3 * rand.NextDouble());
    }
    protected virtual void OnEnterMovingState()
    {
        if (QueryMoveState(CommonActorBehavior.MoveState.MS_MOVING))
        {
            if (!m_actorBase.IsGroundTargetPosReached())
                return;
        }
        base.stateMove = CommonActorBehavior.MoveState.MS_MOVING;
    }
    public override void OnSetDefaultDamageInfo(ref MDamageInfo pDamageInfo)
    {
        base.OnSetDefaultDamageInfo(ref pDamageInfo);
        pDamageInfo.mHitDamage = 10;
    }
    protected virtual bool IsCatchMA(ref Vector3 maPos, out bool isAttackMA, out Vector3 catchMAPos)
    {
        isAttackMA = false;
        catchMAPos = maPos;
        if (0 < coolTime)
            return false;

        Vector3 MonsterPos = m_actorBase.GetPos();
        catchMAPos = MonsterPos;

        Vector3 distMA = MonsterPos - maPos;
        float dM = distMA.Normalize();
        if (AttackRadius >= dM)
            isAttackMA = true;

        if (EyeshotRadius < dM)
        {
            SetWalkSpeed();
            return false;
        }

        bool bIsCatchMA = true;
        catchMAPos = maPos + distMA * ClosestDist;
        if (ClosestDist < dM)
        {
            if (StandAttackRadius >= dM)
            {
                if (QueryMoveState(CommonActorBehavior.MoveState.MS_STAND))
                    bIsCatchMA = false;
            }
        }
        else
            bIsCatchMA = false;

        if (bIsCatchMA)
            SetRunSpeed();
        return bIsCatchMA;
    }
    public override void PreTick(float dtSec)
    {
        if (null == m_actorBase)
            return;

        if (0 < coolTime)
            coolTime -= dtSec;

        bool bIsLive = base.QueryLifeState(CommonActorBehavior.LifeState.LS_LIVE);
        base.PreTick(dtSec);
        if (!bIsLive)
            return;

        Vector3 moveToPos = new Vector3(0);
        bool bEnterMove = false;
        bool bEnterStand = false;
        bool bIsAttackMA = false;

        MMainActor MA = MScene.Instance.GetCurrentMA();
        if (null != MA)
        {
            if (MA.GetMActorBase().GetEnableTick())
            {
                MIPhysXObjBase MObj = MA.GetPhysXObjBase();
                Vector3 maPos = MA.GetPos();
                bool bIsCatchMA = IsCatchMA(ref maPos, out bIsAttackMA, out moveToPos);
                if (bIsCatchMA)
                {
                    if (IsOutCatchField(ref moveToPos))
                    {
                        bIsAttackMA = false;
                        Vector3 MonsterPos = m_actorBase.GetPos();
                        if (IsOutCatchField(ref MonsterPos))
                        {
                            Vector3 dC = MonsterPos - centerPos;
                            dC.Normalize();
                            Vector3 dM = m_actorBase.GetGroundMoveDir();
                            float dot = Vector3.DotProduct(dC, dM);
                            if (0 < dot)
                            {
                                m_actorBase.SetGroundTargetPos(MonsterPos);
                                if (0 > coolTime)
                                    coolTime = 5;
                            }
                        }
                    }
                    else
                    {
                        bEnterMove = true;
                        if (0 < coolTime)
                            coolTime = -1;
                        moveToPos.Y = MScene.Instance.GetTerrainHeight(moveToPos.X, moveToPos.Z);
                    }
                }
            }
            else
            {
                SetWalkSpeed();
            }
        }

        if (QueryMoveState(CommonActorBehavior.MoveState.MS_MOVING))
        {
            if (m_actorBase.IsGroundTargetPosReached())
                bEnterStand = true;
        }

        if (bIsAttackMA)
        {
            Vector3 hp = MA.GetPos();
            Vector3 hd = hp - m_actorBase.GetPos();
            hd.Y = 0;
            hd.Normalize();
            hp.Y += (float)rand.NextDouble();
            hp.X += (float)(0.1 * rand.NextDouble());
            hp.Z += (float)(0.1 * rand.NextDouble());
            m_actorBase.SetHeadDir(hd);
/*            if (0.6 > rand.NextDouble())
                base.PlayAttack(hp, hd, MA.GetPhysXObjBase(), MA.GetMActorBase(), 0);
            else
                base.PlayAttack(hp, hd, MA.GetPhysXObjBase(), MA.GetMActorBase(), 1);*/
        } 
        else
        {
            if (QueryMoveState(CommonActorBehavior.MoveState.MS_STAND))
            {
                if (!bEnterMove)
                {
                    idleTime -= dtSec;
                    if (0 < idleTime)
                        return;
                }

                if (QueryLifeState(CommonActorBehavior.LifeState.LS_DEAD))
                    bEnterStand = true;
                else
                {
                    if (0.65 > rand.NextDouble())
                        bEnterStand = true;
                    else
                    {
                        bEnterMove = true;
                        moveToPos = RandomPos();
                    }
                }
            }
        }

        if (bEnterStand)
        {
            // MLogWrapper.Instance.LogText("Standing In Place!");
            OnEnterStandState();
            SetWalkSpeed();
        }

        if (bEnterMove)
        {
            // MLogWrapper.Instance.LogText("Moving!");
            OnEnterMovingState();
            m_actorBase.SetGroundTargetPos(moveToPos);
        }
    }
}

