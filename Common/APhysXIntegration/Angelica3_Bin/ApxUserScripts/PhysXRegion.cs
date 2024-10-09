using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;

class PhysXRegion : MRegionScript
{
    private MPhysXObjDynamic m_obj = null;//obj in region

    public override void Start()
    {
        base.Start();
        m_obj = (MPhysXObjDynamic)Region.GetChildByLocalName("MyBridge");
    }
    public override void End()
    {
        base.End();
    }
    public override void EnterRegion(MIObjBase[] objs)
    {
        MMainActor actor = MScene.Instance.GetCurrentMA();
        if (actor == null)
            return;

        base.EnterRegion(objs);

        for (int i = 0; i < objs.Length; i++ )
        {
            if (objs[i] == actor.GetPhysXObjBase())
            {
                //MActorBase entering
                //do something
                if (m_obj != null)
                {
                    m_obj.ResetPose(true);
                    m_obj.SetDrivenMode(MDrivenMode.DRIVEN_BY_PURE_PHYSX);
                }

                break;
            }
        }
    }
    public override void LeaveRegion(MIObjBase[] objs)
    {
        MMainActor actor = MScene.Instance.GetCurrentMA();
        if (actor == null)
            return;

        base.LeaveRegion(objs);

        for (int i = 0; i < objs.Length; i++)
        {
            if (objs[i] == actor.GetPhysXObjBase())
            {
                //MActorBase leaving
                //do something
                if (m_obj != null)
                {
                    m_obj.ResetPose(true);
                    m_obj.SetDrivenMode(MDrivenMode.DRIVEN_BY_ANIMATION);
                }

                break;
            }
        }
    }
}

