using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;
using System.Windows.Forms;
using System.Drawing;

class MonsterActionReport : CommonActorActionReport
{
    public override void Init(MIObjBase pObjBase)
    {
        CommonActorBehavior cab = null;
        String[] scripts = pObjBase.GetScriptNames();
        int nCount = scripts.Length;
        for (int i = 0; i < nCount; ++i)
        {
            if (scripts[i].Contains("Monster"))
            {
                MIScriptBehavior script = pObjBase.ScriptBehaviors[i];
                cab = (CommonActorBehavior)script;
            }
        }
        base.InitCommonReport(pObjBase, cab);
    }
}

class PASActionReport : MActorActionReport
{
    public override void OnActionEnd(MActionLayer apxActionLayer, MActionInfo pActionInfo)
    {
        System.String strName = HostActor.GetNextActionName();
        if (null == strName)
            strName = HostActor.GetFirstActionName();

        base.OnActionEnd(apxActionLayer, pActionInfo);
        MActionAnimInfo AnimInfo = new MActionAnimInfo();
        AnimInfo.mActionName = strName;
        AnimInfo.mLooping = false;
        AnimInfo.mRestart = false;
        AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_FullBody;
        MActionInfo info = new MActionInfo(AnimInfo);
        info.mCoolDownTime = 0.0f;
        info.mIgnoreActionTime = 0.0f;
        info.mPlayMode = MActionPlayMode.ActionPlayMode_NormalPlay;
        HostActor.PlayAction(MActionLayer.MActionLayer_Active, info);
    }
}