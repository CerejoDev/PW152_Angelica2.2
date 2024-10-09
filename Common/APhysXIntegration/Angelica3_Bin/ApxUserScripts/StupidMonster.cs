using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;
using System.Windows.Forms;

class MonsterHurtEvent : MEventNotify
{
    public MonsterHurtEvent(StupidMonsterReport report)
    {
        actorReport = report;
    }
    public override void OnEventNotify(MTimerEvent timer)
    {
        base.OnEventNotify(timer);
        if (actorReport != null)
        {
            actorReport.Hurting = false;
        }
    }
    public override void OnNotifyDie(MIObjBase obj)
    {
        base.OnNotifyDie(obj);

        actorReport = null;
    }

    public override void OnPreTickNotify(float dtSec, MTimerEvent timer)
    {
        base.OnPreTickNotify(dtSec, timer);

        //侧走

    }

    private StupidMonsterReport actorReport = null;
    private Vector3 m_vHurtDir = new Vector3(0, 0, 0);
}

public class StupidMonster : MIScriptBehavior
{
    public override void Start()
    {
        MIPhysXObjBase physxobj = (MIPhysXObjBase)ObjBase;
        physxobj.SetDrivenMode(MDrivenMode.DRIVEN_BY_ANIMATION);

        MActorBase actorBase = MScene.Instance.GetMActorBase(ObjBase);
        StupidMonsterReport action = new StupidMonsterReport();
        actorBase.SetMActionReport(action);

        MActionAnimInfo AnimInfo = new MActionAnimInfo();
        AnimInfo.mActionName = actorBase.GetFirstActionName();
        AnimInfo.mLooping = false;
        AnimInfo.mRestart = false;
        AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_FullBody;
        MActionInfo info = new MActionInfo(AnimInfo);
        info.mCoolDownTime = 0.0f;
        info.mIgnoreActionTime = 0.0f;
        info.mPlayMode = MActionPlayMode.ActionPlayMode_NormalPlay;
        actorBase.PlayAction(MActionLayer.MActionLayer_Active, info);
    }
}

class StupidMonsterReport : MActorActionReport
{
    private int iActionIndex = 0;
    private string[] strActions = { "猎人_普攻2", "猎人_普攻1", "人族_战士_普攻1", "人族_战士_寸击" };
    bool bHurts = false;
    Vector3 vHurtDir = new Vector3(0, 0, 0);
    float fMoveSpeedHurt = 5;
    float fMoveSpeedBak;

    public bool Hurting
    {
        get { return bHurts; }
        set 
        {
            if (!bHurts && value)
            {
                fMoveSpeedBak = HostActor.GetGroundMoveSpeed();
                HostActor.SetGroundMoveSpeed(fMoveSpeedHurt);
            }
            else if (bHurts && !value)
                HostActor.SetGroundMoveSpeed(fMoveSpeedBak);

            bHurts = value; 
        }
    }
    public Vector3 HurtDir
    {
        get { return vHurtDir; }
        set 
        { 
            vHurtDir = value; 
        }
    }

    public override void Init(MIObjBase pObjBase)
    {
        HostActor.SetBlendAction("run_Left_fix2", "run_Left_fix2_inv", "run_Right_fix2", "run_Right_fix2_inv",
                    "run_Forward_speedup", "run_Back_Move2", "walk_Forward_Move2");

    }

    public override void OnActionEnd(MActionLayer apxActionLayer, MActionInfo pActionInfo)
    {
        System.String strName = strActions[iActionIndex];
        iActionIndex++;
        if (iActionIndex >= strActions.Length)
            iActionIndex = 0;

        base.OnActionEnd(apxActionLayer, pActionInfo);
        MActionAnimInfo AnimInfo = new MActionAnimInfo();
        AnimInfo.mActionName = strName;
        AnimInfo.mLooping = false;
        AnimInfo.mRestart = false;
        AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
        MActionInfo info = new MActionInfo(AnimInfo);
        info.mCoolDownTime = 0.0f;
        info.mIgnoreActionTime = 0.0f;
        info.mPlayMode = MActionPlayMode.ActionPlayMode_NormalPlay;
        HostActor.PlayAction(MActionLayer.MActionLayer_Active, info);
    }

    public override bool OnAttackedPreHitEvent(MActorBase pCaster, int attackType, uint idxCasterTeam)
    {
        return true;
    }
    public override bool OnPreAttackedQuery()
    {
        return true;
    }
    public override void OnPreAttackedNotify(MAttackActionInfo pAttActInfo, MPhysXRBDesc pAttacker)
    {
        
    }
    public override bool OnAttackedHitEvent(MDamageInfo pDamageInfo)
    {
        //bend to ...
        HostActor.BendBody(pDamageInfo.mHitDir, 120, 0.2f, 0.2f);
        MonsterHurtEvent hurtEvent = new MonsterHurtEvent(this);
        MScene.Instance.RegisterTimerEvent(hurtEvent, 0.4f, MDelayType.DT_TIME, 1);
        vHurtDir = pDamageInfo.mHitDir;
        Hurting = true;
        
        return true;
    }

    public override void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags)
    {
        base.OnKeyDown(nChar, nRepCnt, nFlags);
    }

    public override MUserInputFlag TickUserInput()
    {        
        if (bHurts && !vHurtDir.IsZero())
        {
            MActionAnimInfo AnimInfo = new MActionAnimInfo();
            MActionInfo info = new MActionInfo(AnimInfo);
            info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
            if (0x03000000 <= MScene.GetAngelicaSDKVersion())
            {
                AnimInfo.mBlendWalkRun = true;
                AnimInfo.mActionName = "人族地面快速前进";
            }
            else
            {
                AnimInfo.mBlendWalkRun = false;
                AnimInfo.mActionName = "普通奔跑_通用";
            }
            AnimInfo.mLooping = true;
            AnimInfo.mRestart = false;

            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_UpperBody;
            HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);

            //HostActor.SetGroundMoveSpeed(HostActor.GetGroundMoveSpeed() * 0.9f);
            HostActor.SetGroundMoveDirWithNoTurn(vHurtDir);
        }
        else
        {
            MActionAnimInfo AnimInfo = new MActionAnimInfo();
            MActionInfo info = new MActionInfo(AnimInfo);
            info.mPlayMode = MActionPlayMode.ActionPlayMode_InstantPlay;
            if (0x03000000 <= MScene.GetAngelicaSDKVersion())
                AnimInfo.mActionName = "羽族地面站立";
            else
                AnimInfo.mActionName = "站立_华山";
            AnimInfo.mLooping = true;
            AnimInfo.mRestart = false;

            AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_FullBody;
            HostActor.PlayAction(MActionLayer.MActionLayer_Move, info);

        }

        return base.TickUserInput();
    }
}
