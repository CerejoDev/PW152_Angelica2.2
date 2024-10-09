using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;


public class Blood : MIScriptBehavior
{
    public enum ActorState { BC_LIVE = 1, BC_DEAD, BC_REBORN };

    private int maxHP = 200;
    private int curHP = 200;
    private uint color = 0xffff0000;
    private float rebornInterval = 5;
    private float afterDeadTime = -1;
    private MActorBase m_actorBase = null;
    private ActorState state = ActorState.BC_LIVE;

    public uint Color
    {
        get { return color; }
        set { color = value; }
    }
    public float RebornInterval
    {
        get { return rebornInterval; }
        set
        {
            if (0 < value)
                rebornInterval = value;
        }
    }
    public int MaxHealthPoint
    {
        get { return maxHP; }
        set 
        {
            int deltaChange = value - maxHP;
            ChangeMaxHealthPoint(deltaChange);
        }
    }
    public void ChangeMaxHealthPoint(int deltaVal)
    {
        int newMaxHP = maxHP + deltaVal;
        if (0 < newMaxHP)
        {
            maxHP = newMaxHP;
            if (curHP > maxHP)
                curHP = maxHP;
        }
    }
    public int CurrentHealthPoint
    {
        get { return curHP; }
        set
        {
            int deltaChange = value - curHP;
            ChangeCurrentHealthPoint(deltaChange);
        }
    }
    public void ChangeCurrentHealthPoint(int deltaVal)
    {
        if (state == ActorState.BC_DEAD)
            return;

        curHP = curHP + deltaVal;
        if (curHP > maxHP)
            curHP = maxHP;
        if (1 > curHP)
        {
            curHP = 0;
            OnEnterDeadState();
        }
    }
    public void SetActorBase(MActorBase mab)
    {
        m_actorBase = mab;
    }
    public override void Start()
    {
        m_actorBase = MScene.Instance.GetMActorBase(ObjBase);
    }
    public bool QueryActorState(ActorState qState)
    {
        if (qState == state)
            return true;
        return false;
    }
    public override void OnRender(MRender render)
    {
        if (ActorState.BC_DEAD == state)
            return;

        float perBlood = (float)curHP / (float)maxHP;
        Vector3 MonsterPos = m_actorBase.GetPos();
        MonsterPos.Y = MonsterPos.Y + 1.0f;
        render.AddBloodBar(MonsterPos, 25, perBlood, color);
    }
    protected virtual void OnEnterDeadState()
    {
        if (ActorState.BC_DEAD == state)
            return;
        state = ActorState.BC_DEAD;
        afterDeadTime = 0;
    }
    protected virtual void OnEnterRebornState()
    {
        afterDeadTime = -1;
        curHP = maxHP;
        state = ActorState.BC_REBORN;
        m_actorBase.RebornNotify();
    }

    public override void PreTick(float dtSec)
    {
        if (ActorState.BC_REBORN == state)
        {
            state = ActorState.BC_LIVE;
        }
        else if (ActorState.BC_DEAD == state)
        {
            afterDeadTime += dtSec;
            if (afterDeadTime > RebornInterval)
                OnEnterRebornState();
        }
    }
}
 
