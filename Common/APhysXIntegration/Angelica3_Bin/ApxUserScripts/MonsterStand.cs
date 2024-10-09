using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;


public class MonsterStand : CommonActorBehavior
{
    protected MIPhysXObjBase m_PhysXObj = null;

    public override void Start()
    {
        m_PhysXObj = (MIPhysXObjBase)ObjBase;
        base.Start();
        MonsterActionReport action = new MonsterActionReport();
        m_actorBase.SetMActionReport(action);
        base.Color = 0xff00ff00;
        MaxHealthPoint = 200;
        CurrentHealthPoint = 200;
        TeamGroup = 2;
        AutoComboAttack = true;
        EyeshotRadius = 5;
        if (0x03000000 > MScene.GetAngelicaSDKVersion())
            RebornInterval = 300;
    }

    protected virtual void OnEnterStandState()
    {
        if (QueryMoveState(CommonActorBehavior.MoveState.MS_STAND))
            return;
        base.stateMove = CommonActorBehavior.MoveState.MS_STAND;
        Vector3 MonsterPos = m_actorBase.GetPos();
        m_actorBase.SetGroundTargetPos(MonsterPos);
    }
    public override void OnSetDefaultDamageInfo(ref MDamageInfo pDamageInfo)
    {
        base.OnSetDefaultDamageInfo(ref pDamageInfo);
        pDamageInfo.mHitDamage = 10;
    }
    public override void PreTick(float dtSec)
    {
        if (null == m_actorBase)
            return;

        bool bIsLive = base.QueryLifeState(CommonActorBehavior.LifeState.LS_LIVE);
        base.PreTick(dtSec);
        if (!bIsLive)
            return;

        if (!QueryMoveState(CommonActorBehavior.MoveState.MS_STAND))
            OnEnterStandState();
    }
}

