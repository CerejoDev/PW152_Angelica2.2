using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;

public class PlayActionSequence : MIScriptBehavior
{
    public override void Start()
    {
        MIPhysXObjBase physxobj = (MIPhysXObjBase)ObjBase;
        physxobj.SetDrivenMode(MDrivenMode.DRIVEN_BY_ANIMATION);

        MActorBase actorBase = MScene.Instance.GetMActorBase(ObjBase);
        PASActionReport action = new PASActionReport();
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
