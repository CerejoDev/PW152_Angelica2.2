using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;


public class CommonActorBehavior : MIScriptBehavior
{
    public Random rand = null;
    public enum LifeState { LS_LIVE = 1, LS_DEAD, LS_REBORN };
    public enum MoveState { MS_STAND = 1, MS_MOVING, MS_PUSH };
    public enum FightState { FS_NONE = 1, FS_ATTACK, FS_INJURED };

    public enum AttackType
    {
        AT_DEFAULT = 0x00,
        AT_RANGED  = 0x01,
        AT_CONTACT = 0x02,  // from PhysX contact report
        AT_RAYHIT  = 0x03
    };

    public class AttackInfo
    {
        public bool mBasicAction = true;
        public string strActName;
        public MAttackInfo infoAttack;
        public MWeaponInfo infoWeapon;
        public float attackR = 2;
        public float closestDist = 1.2f;
        public float standAttackR = 1.7f;
    }

    public class AttackDesc
    {
        public bool bPhysXDetection = true;
        public string strActionName;
        public float fStartTimeSec = 0;
        public float fDurationTimeSec = 0;
    }

    public class AnimationResDesc
    {
        public string strActDeath;
        public string strActDeathLoop;
        public string strActStanding;
        public string strActDynCCSwing;
        public string strActPushing;
        public string strActJumpStart;
        public string strActJumpLoop;
        public string strActJumpEnd;
        public string strActRunForward;
        public string strActRunBack;
        public string strActPickUpSTH;
        public string strActPickHoldOn;
        public string strActThrowOut;

        public string strActAttSingle;
        public string strActAttCombo2;
        public string strActDamage;

        public string strMeleeWeapon;
        public string strRangedWeapon;
    }

    private AnimationResDesc descAniRes = new AnimationResDesc();
    private AttackInfo attCurrent;
    private AttackInfo attMelee;
    private AttackInfo attRanged;
    private AttackDesc[] descMelee = new AttackDesc[3];

    public AnimationResDesc GetAnimationRes
    {
        get { return descAniRes; }
    }
    public AttackInfo CurrentAttackInfo
    {
        get { return attCurrent; }
    }
    public void ChangeCurrentAttackType()
    {
        if (attCurrent.infoAttack.mAttackType == attMelee.infoAttack.mAttackType)
            attCurrent = attRanged;
        else
            attCurrent = attMelee;
        m_actorBase.ChangeEquipment(attCurrent.infoWeapon);
        AttackRadius = attCurrent.attackR;
        closestDist = attCurrent.closestDist;
        standAttackR = attCurrent.standAttackR;
    }
    public bool QueryAttackType(AttackType aType)
    {
        if ((int)aType == attCurrent.infoAttack.mAttackType)
            return true;
        return false;
    }

    //--------------------------------------------------
    protected MActorBase m_actorBase = null;
    protected MIObjBase m_moveTarget = null;
    protected MIAssailable m_activeEnemy = null;
    protected LifeState stateLife = LifeState.LS_LIVE;
    protected MoveState stateMove = MoveState.MS_STAND;
    protected FightState stateFight = FightState.FS_NONE;

    private int maxHP = 1000;
    private int curHP = 1000;
    private uint color = 0xffff0000;
    private float rebornInterval = 5;
    private float afterDeadTime = -1;
    private bool deadToRagdoll = true;
    private bool autoComboAttack = false;
    private uint idxTeamGroup = 1;
    private int[] enemyTeamGroup = new int[32];
    private string[] damActionName = new string[3];

    private bool autoAttackEnemyOnce = false;
    private bool bIsLocalHitFlag = false;
    private bool bIsHorDirFlag = false;
    private bool bIsRandomFlag = false;
    private Vector3 m_HitPos;
    private Vector3 m_HitDir;

    private float stunInterval = 1;
    private float stunTime = -1;

    private float attackCoolDown = 1.5f;
    private float currentAttCool = -1;

    private float damTimeFromLastAttacked = -1;
    private float besiegeInterval = 2;
    private float dodgeChance = 0;
    private float dodgeCoolDownTime = -1;
    private float dodgeCoolDownInterval = 6;

    private float attackR = 2;
    private float eyeshotR = 2;
    private float closestDist = 1.2f;
    private float standAttackR = 1.7f;

    public float AttackRadius
    {
        get { return attackR; }
        set
        {
            if (0 < value)
                attackR = value;
        }
    }
    public float EyeshotRadius
    {
        get { return eyeshotR; }
        set
        {
            if (0 < value)
                eyeshotR = value;
        }
    }
    public float ClosestDist
    {
        get { return closestDist; }
    }
    public float StandAttackRadius
    {
        get { return standAttackR; }
    }
    public MIObjBase MoveTarget
    {
        get { return m_moveTarget; }
    }
    public MIAssailable ActiveEnemy
    {
        get { return m_activeEnemy; }
    }
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
        if (QueryLifeState(LifeState.LS_DEAD))
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
    public uint TeamGroup
    {
        get { return idxTeamGroup; }
        set
        {
            if (31 >= value)
                idxTeamGroup = value;
        }
    }
    public int GetEnemyTeamGroup()
    {
        return (int)enemyTeamGroup.GetValue(idxTeamGroup);
    }
    public bool SetEnemyTeamGroup(uint idxTeamGroup, int enemyValue)
    {
        if (31 < idxTeamGroup)
            return false;

        int theValue = 1 << (int)idxTeamGroup;
        enemyValue &= ~theValue;
        enemyTeamGroup.SetValue(enemyValue, idxTeamGroup);
        return true;
    }
    public bool IsEnemyTeam(uint idxTeamGroup)
    {
        int enemyTeam = GetEnemyTeamGroup();
        int theValue = 1 << (int)idxTeamGroup;
        int nResult = (int)(enemyTeam & theValue);
        if (0 == nResult)
            return false;
        return true;
    }
    public bool IsInjuredAction(ref string strName)
    {
        int nCount = damActionName.GetLength(0);
        for (int i = 0; i < nCount; ++i)
        {
            if (strName == damActionName[i])
                return true;
        }
        return false;
    }
    public string GetDamageActionName(int damSourceFlags)
    {
        int idx = 0;
        if (0 < ((int)MDamageSource.MDS_LEFT & damSourceFlags))
            idx = 1;
        if (0 < ((int)MDamageSource.MDS_RIGHT & damSourceFlags))
            idx = 2;
        return damActionName[idx];
    }
    public string GetAttackActionName(int idx)
    {
        if (CurrentAttackInfo.mBasicAction)
        {
            if (0 > idx)
            {
                float number = (float)rand.NextDouble();
                if (0.5f > number)
                    idx = 1;
                else
                    idx = 0;
            }
            CurrentAttackInfo.strActName = descMelee[idx].strActionName;
            CurrentAttackInfo.infoAttack.mIsPhysXDetection = descMelee[idx].bPhysXDetection;
            CurrentAttackInfo.infoAttack.mDamageDelayStartSec = descMelee[idx].fStartTimeSec;
            CurrentAttackInfo.infoAttack.mDamageDurationSec = descMelee[idx].fDurationTimeSec;
        }
        return CurrentAttackInfo.strActName;
    }
    public bool QueryLifeState(LifeState qState)
    {
        return (qState == stateLife) ? true : false;
    }
    public bool QueryMoveState(MoveState qState)
    {
        return (qState == stateMove) ? true : false;
    }
    public bool QueryFightState(FightState qState)
    {
        return (qState == stateFight) ? true : false;
    }
    public float StunInterval
    {
        get { return stunInterval; }
        set
        {
            if (!(0 > value))
                stunInterval = value;
        }
    }
    public float AttackCoolDown
    {
        get { return attackCoolDown; }
        set
        {
            if (!(0 > value))
                attackCoolDown = value;
        }
    }
    public bool DeadMethodToRagdoll
    {
        get { return deadToRagdoll; }
        set { deadToRagdoll = value; }
    }
    public bool AutoComboAttack
    {
        get { return autoComboAttack; }
        set { autoComboAttack = value; }
    }
    public float DodgeChance
    {
        get { return dodgeChance; }
    }

    public void SetAttackEnemyEnable(bool bEnable)
    {
        autoAttackEnemyOnce = bEnable;
        if (bEnable)
            bIsRandomFlag = true;
        else
            bIsRandomFlag = false;
    }
    public void SetMoveTarget(bool bIsAttackIt, MIObjBase moveTarget)
    {
        m_moveTarget = moveTarget;
        m_activeEnemy = MScene.Instance.GetMIAssailable(m_moveTarget);
        if (null == m_moveTarget)
        {
            SetAttackEnemyEnable(false);
        }
        else
        {
            if (bIsAttackIt)
                SetAttackEnemyEnable(true);
            else
                m_activeEnemy = null;
        }
    }
    public MIObjBase GetNextEnemy()
    {
        MIObjBase temp = null;
        MIObjBase theEnemy = null;
        float theEnemyDist = AttackRadius;
        MIRoleActor npcRole = null;
        int nCount = MScene.Instance.GetNPCActorCount();
        for (int i = 0; i < nCount; ++i)
        {
            npcRole = MScene.Instance.GetNPCActor(i);
            temp = npcRole.GetPhysXObjBase();
            CommonActorBehavior cmnActor = null;
            String[] scripts = temp.GetScriptNames();
            int nCS = scripts.Length;
            for (int j = 0; j < nCS; ++j)
            {
                MIScriptBehavior script = temp.ScriptBehaviors[j];
                cmnActor = script as CommonActorBehavior;
                if (null != cmnActor)
                    break;
            }
            if (null != cmnActor)
            {
                if (cmnActor.QueryLifeState(CommonActorBehavior.LifeState.LS_LIVE))
                {
                    if (cmnActor.IsEnemyTeam(TeamGroup))
                    {
                        Vector3 dist = m_actorBase.GetPos() - temp.GetGPos();
                        float fd = dist.Magnitude();
                        if (fd < theEnemyDist)
                        {
                            theEnemy = temp;
                            theEnemyDist = fd;
                        }
                    }
                }
            }
        }
        return theEnemy;
    }
    public void UpdateHitPosAndDir(ref Vector3 hitGPos, ref Vector3 hitGDir, bool bToLocal, bool bIsHorDir)
    {
        bIsRandomFlag = false;
        m_HitPos = hitGPos;
        m_HitDir = hitGDir;
        bIsHorDirFlag = false;
        bIsLocalHitFlag = false;
        if (bToLocal)
        {
            if (null != m_activeEnemy)
            {
                Matrix4 pose = m_activeEnemy.GetGPose();
                Matrix4 poseInv = pose.GetInverseTM();
                m_HitPos = m_HitPos * poseInv;
            }
            bIsLocalHitFlag = true;
        }
        if (bIsHorDir)
        {
            m_HitDir.Y = 0;
            m_HitDir.Normalize();
        }
    }
    private bool RefreshHitPosAndDir()
    {
        bool bRtnVal = false;
        m_HitDir = m_actorBase.GetHeadDir();
        if (bIsRandomFlag)
        {
            if (null != m_activeEnemy)
            {
                if (!attCurrent.infoAttack.mIsPhysXDetection)
                {
                    m_HitPos = m_activeEnemy.GetCurrentPos();
                    m_HitDir = m_HitPos - m_actorBase.GetPos();
                    m_HitDir.Y = 0;
                    m_HitDir.Normalize();
                    m_HitPos.Y += 0.8f;
                    m_HitPos.Y += (float)rand.NextDouble();
                    m_HitPos.X += (float)(0.1 * rand.NextDouble());
                    m_HitPos.Z += (float)(0.1 * rand.NextDouble());
                    bRtnVal = true;
                }
            }
            bIsRandomFlag = false;
            return bRtnVal;
        }
        if (bIsLocalHitFlag)
        {
            if (null != m_activeEnemy)
            {
                Matrix4 pose = m_activeEnemy.GetGPose();
                m_HitPos = m_HitPos * pose;
                m_HitDir = m_HitPos - m_actorBase.GetPos();
                bIsLocalHitFlag = false;
                bRtnVal = true;
            }
        }
        if (bIsHorDirFlag)
        {
            m_HitDir = m_HitPos - m_actorBase.GetPos();
            m_HitDir.Y = 0;
            m_HitDir.Normalize();
            bIsHorDirFlag = false;
            bRtnVal = true;
        }
        return bRtnVal;
    }

    public bool IsAttackEnable()
    {
        if (QueryLifeState(LifeState.LS_DEAD))
            return false;
        if (0 < stunTime)
            return false;
        if (0 < currentAttCool)
            return false;
        if (QueryFightState(FightState.FS_INJURED))
            return false;
        if (autoComboAttack)
        {
            if (QueryFightState(FightState.FS_ATTACK))
                return false;
        }
        return true;
    }
    public override void Start()
    {
        rand = GetRandom();
        TeamGroup = 1;
        uint nLen = (uint)enemyTeamGroup.GetLength(0);
        for (uint i = 0; i < nLen; ++i)
            SetEnemyTeamGroup(i, unchecked((int)0xffffffff));
        
/*        
        damActionName[0] = "战士受伤1";
        damActionName[1] = "战士受伤2";
        damActionName[2] = "战士受伤3";
*/
        if (0x03000000 <= MScene.GetAngelicaSDKVersion())
        {
            descMelee[0] = new AttackDesc();
            descMelee[0].bPhysXDetection = true;
            descMelee[0].strActionName = "人族_战士_寸击";
            descMelee[0].fStartTimeSec = 0.35f;
            descMelee[0].fDurationTimeSec = 0.4f;
            descMelee[1] = new AttackDesc();
            descMelee[1].bPhysXDetection = true;
            descMelee[1].strActionName = "人族_战士_普攻2";
            descMelee[1].fStartTimeSec = 0.3f;
            descMelee[1].fDurationTimeSec = 0.5f;
            descMelee[2] = new AttackDesc();
            descMelee[2].bPhysXDetection = true;
            descMelee[2].strActionName = "人族_战士_XX一击";
            descMelee[2].fStartTimeSec = 0.3f;
            descMelee[2].fDurationTimeSec = 0.43f;

            damActionName[0] = "受伤_正面_Big";
            damActionName[1] = "受伤_左击_Big";
            damActionName[2] = "受伤_右击_Big";
            descAniRes.strActDeath = "人族飞剑死亡";
            descAniRes.strActDeathLoop = "人族飞剑死亡循环";
            descAniRes.strActStanding = "羽族地面站立";
            descAniRes.strActDynCCSwing = "人族翅膀悬浮";
            descAniRes.strActPushing = "Push";
            descAniRes.strActJumpStart = "魔族地面跳跃起";
            descAniRes.strActJumpLoop = "魔族地面跳跃循环";
            descAniRes.strActJumpEnd = "魔族地面跳跃落";
            descAniRes.strActRunForward = "人族地面快速前进";
            descAniRes.strActRunBack = "人族地面快速倒退";
            descAniRes.strActPickUpSTH = "搬箱子_拾取";
            descAniRes.strActPickHoldOn = "搬箱子_待机";
            descAniRes.strActThrowOut = "搬箱子_抛后恢复";
            descAniRes.strActAttSingle = "猎人_普攻2";
            descAniRes.strActAttCombo2 = "猎人_战域";
            descAniRes.strActDamage = "猎人受伤2";
            descAniRes.strMeleeWeapon = "Models\\weapons\\剑\\剑.ecm3";
            descAniRes.strRangedWeapon = "Models\\weapons\\法镰\\法镰.ecm3";
        }
        else
        {
            descMelee[0] = new AttackDesc();
            descMelee[0].bPhysXDetection = true;
            descMelee[0].strActionName = "小左砍_武当";
            descMelee[0].fStartTimeSec = 0.35f;
            descMelee[0].fDurationTimeSec = 0.6f;
            descMelee[1] = new AttackDesc();
            descMelee[1].bPhysXDetection = true;
            descMelee[1].strActionName = "右斩_武当";
            descMelee[1].fStartTimeSec = 0.3f;
            descMelee[1].fDurationTimeSec = 0.5f;
            descMelee[2] = new AttackDesc();
            descMelee[2].bPhysXDetection = true;
            descMelee[2].strActionName = "左撩转_衡山";
            descMelee[2].fStartTimeSec = 0.3f;
            descMelee[2].fDurationTimeSec = 0.43f;

            damActionName[0] = "受伤3_武当";
            damActionName[1] = "受伤1_武当";
            damActionName[2] = "受伤2_武当";
            descAniRes.strActDeath = "死亡_五毒";
            descAniRes.strActDeathLoop = "死亡循环_五毒";
            descAniRes.strActStanding = "站立_华山";
            descAniRes.strActDynCCSwing = "打坐持续_武当";
            descAniRes.strActPushing = "zs_格挡前进_华山";
            descAniRes.strActJumpStart = "跳跃起_通用";
            descAniRes.strActJumpLoop = "跳跃循环_通用";
            descAniRes.strActJumpEnd = "跳跃落_通用";
            descAniRes.strActRunForward = "普通奔跑_通用";
            descAniRes.strActRunBack = "普通奔跑_通用";
            descAniRes.strActPickUpSTH = "搬箱子_拾取";
            descAniRes.strActPickHoldOn = "搬箱子_待机";
            descAniRes.strActThrowOut = "搬箱子_抛后恢复";
            descAniRes.strActAttSingle = "zd_双刀普攻1_日月";
            descAniRes.strActAttCombo2 = "zd_上撩扇_逍遥";
            descAniRes.strActDamage = "受伤7_华山";
            descAniRes.strMeleeWeapon = "Models\\weapons\\单剑\\zwq武器华山单剑05\\zwq武器华山单剑05.ecm";
            descAniRes.strRangedWeapon = "Models\\weapons\\法杖\\zwq武器峨眉法杖01\\zwq武器峨眉法杖01.ecm";
        }

        DeadMethodToRagdoll = ObjBase.HasPhysXBody();
        m_actorBase = MScene.Instance.GetMActorBase(ObjBase);
        m_actorBase.InitWalkRunBlend("人族地面慢速前进", "人族地面快速前进");
        m_actorBase.SetGroundMoveSpeed(1.5f);
        m_actorBase.InitFootIK("Bip01 L Foot", "Bip01 R Foot");
        m_actorBase.EnableFootIK(false, false);

   /*     descMelee[0] = new AttackDesc();
        descMelee[0].bPhysXDetection = true;
        descMelee[0].strActionName = "连击_attack1";
        descMelee[0].fStartTimeSec = 0.35f;
        descMelee[0].fDurationTimeSec = 0.4f;
        descMelee[1] = new AttackDesc();
        descMelee[1].bPhysXDetection = true;
        descMelee[1].strActionName = "连击_attack2";
        descMelee[1].fStartTimeSec = 0.3f;
        descMelee[1].fDurationTimeSec = 0.5f;
        descMelee[2] = new AttackDesc();
        descMelee[2].bPhysXDetection = true;
        descMelee[2].strActionName = "连击_attack3";
        descMelee[2].fStartTimeSec = 0.3f;
        descMelee[2].fDurationTimeSec = 0.4f;*/

        attMelee = new AttackInfo();
        attMelee.mBasicAction = true;
        attMelee.strActName = descMelee[0].strActionName;
        attMelee.infoAttack = new MAttackInfo();
        attMelee.infoAttack.mIsPhysXDetection = descMelee[0].bPhysXDetection;
        attMelee.infoAttack.mAttackType = (int)AttackType.AT_DEFAULT;
        attMelee.infoAttack.mAttackRange = 3.0f;
        attMelee.infoAttack.ConvertAttackAngularRange(140);
        attMelee.infoAttack.mDamageDelayStartSec = descMelee[0].fStartTimeSec;
        attMelee.infoAttack.mDamageDurationSec = descMelee[0].fDurationTimeSec;
        attMelee.infoWeapon = new MWeaponInfo();
        attMelee.infoWeapon.mWeaponName = descAniRes.strMeleeWeapon;
        attMelee.infoWeapon.mHostSocketType = MSkinModelSocketType.MApxSM_SocketType_Hook;
        attMelee.infoWeapon.mHostSocketName = "HH_righthandweapon";
        attMelee.infoWeapon.mWPNSocketType = MSkinModelSocketType.MApxSM_SocketType_Hook;
        attMelee.infoWeapon.mWPNSocketName = "CC_weapon";
        bool bEnableXAJH = false;
        if (0x02010000 == MScene.GetAngelicaSDKVersion())
            bEnableXAJH = true;
        if (bEnableXAJH)
        {
            attMelee.infoWeapon.mbUsingXAJHWay = true;
            attMelee.infoWeapon.mWPNECMNickName = "Right_Weapon";
            attMelee.infoWeapon.mWPNAttacher = "A_weapon";
            attMelee.infoWeapon.mHostSocketName = "HH_righthandweapon01";
        }
        attMelee.attackR = 2;
        attMelee.closestDist = 1.2f;
        attMelee.standAttackR = 1.7f;
        attCurrent = attMelee;
        m_actorBase.ChangeEquipment(attCurrent.infoWeapon);
        AttackRadius = attCurrent.attackR;
        closestDist = attCurrent.closestDist;
        standAttackR = attCurrent.standAttackR;

        attRanged = new AttackInfo();
        attRanged.mBasicAction = false;
        attRanged.strActName = "法师_地面_远程单体攻击_法师普攻1_执行";
        attRanged.infoAttack = new MAttackInfo();
        attMelee.infoAttack.mIsPhysXDetection = true;
        attRanged.infoAttack.mAttackType = (int)AttackType.AT_RANGED;
        attRanged.infoAttack.mAttackRange = 40;
        attRanged.infoAttack.mDamageDelayStartSec = 0;
        attRanged.infoWeapon = new MWeaponInfo();
        attRanged.infoWeapon.mWeaponName = descAniRes.strRangedWeapon;
        attRanged.infoWeapon.mHostSocketType = MSkinModelSocketType.MApxSM_SocketType_Hook;
        attRanged.infoWeapon.mHostSocketName = "HH_righthandweapon";
        attRanged.infoWeapon.mWPNSocketType = MSkinModelSocketType.MApxSM_SocketType_Hook;
        attRanged.infoWeapon.mWPNSocketName = "CC_weapon";
        attRanged.attackR = attRanged.infoAttack.mAttackRange;
        attRanged.closestDist = 20;
        attRanged.standAttackR = 30;
    }
    public override void OnRender(MRender render)
    {
        Vector3 MonsterPos = m_actorBase.GetPos();
        MonsterPos.Y = MonsterPos.Y + 1.5f;

        uint FS_color = 0xff00ff00;
        if (QueryFightState(FightState.FS_ATTACK))
            FS_color = 0xffff0000;
        else if (QueryFightState(FightState.FS_INJURED))
            FS_color = 0xff0000ff;
        render.AddWireSphere(MonsterPos, 0.1f, FS_color);

        if (QueryLifeState(LifeState.LS_DEAD))
            return;

        float perBlood = (float)curHP / (float)maxHP;
        MonsterPos.Y = MonsterPos.Y - 0.5f;
        render.AddBloodBar(MonsterPos, 25, perBlood, color);
    }
    protected virtual void OnEnterDeadState()
    {
        if (QueryLifeState(LifeState.LS_DEAD))
            return;
        stateLife = LifeState.LS_DEAD;
        stateFight = FightState.FS_NONE;
        SetMoveTarget(false, null);
        afterDeadTime = 0;
    }
    protected virtual void OnEnterRebornState()
    {
        afterDeadTime = -1;
        curHP = maxHP;
        stateLife = LifeState.LS_REBORN;
        m_actorBase.RebornNotify();
    }
    public virtual void OnSetDefaultDamageInfo(ref MDamageInfo pDamageInfo)
    {
        pDamageInfo.mHitMomentum = 5;
        pDamageInfo.mHitDamage = 40;
    }
    public virtual bool OnHitDamageNotify(MDamageInfo pDamageInfo)
    {
        if (0 > stunTime)
            stunTime = StunInterval;
        currentAttCool = -1;
        stateFight = FightState.FS_INJURED;
        if (null == m_activeEnemy)
        {
            if (null != pDamageInfo.mpAttacker)
            {
                MActorBase attacker = pDamageInfo.mpAttacker.GetActorBase();
                if (null != attacker)
                {
                    Vector3 dist = attacker.GetPos() - m_actorBase.GetPos();
                    float fd = dist.Magnitude();
                    if (EyeshotRadius > fd)
                        SetMoveTarget(true, attacker.GetObjDyn());
                }
            }
        }

        if (!(0 > damTimeFromLastAttacked))
        {
            if (!(damTimeFromLastAttacked > besiegeInterval))
            {
                dodgeChance *= 1.2f;
                if (dodgeChance < 0.01f)
                    dodgeChance = 0.2f;
                else if (dodgeChance > 1.0f)
                    dodgeChance = 1.0f;
                dodgeCoolDownTime = dodgeCoolDownInterval;
            }
        }
        damTimeFromLastAttacked = 0;
        return true;
    }

    public override void PreTick(float dtSec)
    {
        if (!(0 > damTimeFromLastAttacked))
        {
            damTimeFromLastAttacked += dtSec;
            if (damTimeFromLastAttacked > besiegeInterval)
                damTimeFromLastAttacked = -1;
        }
        if (dodgeCoolDownTime > 0.0f)
        {
            dodgeCoolDownTime -= dtSec;
            if (0 > dodgeCoolDownTime)
            {
                dodgeChance *= 0.5f;
                if (dodgeChance < 0.01f)
                {
                    dodgeChance = 0;
                    dodgeCoolDownTime = -1;
                }
                else
                    dodgeCoolDownTime = dodgeCoolDownInterval;
            }
        }

        if (0 < currentAttCool)
            currentAttCool -= dtSec;
        if (0 < stunTime)
            stunTime -= dtSec;

        if (LifeState.LS_REBORN == stateLife)
        {
            stateLife = LifeState.LS_LIVE;
            stateMove = MoveState.MS_STAND;
        }
        else if (LifeState.LS_DEAD == stateLife)
        {
            afterDeadTime += dtSec;
            if (afterDeadTime > RebornInterval)
                OnEnterRebornState();
        }

        if (autoAttackEnemyOnce)
        {
            if (null != m_activeEnemy)
            {
                float fOutDist = 0;
                bool isAttackEnemy = false;
                Vector3 moveToPos;
                bool bIsCatch = IsCatchEnemy(out fOutDist, out isAttackEnemy, out moveToPos);
                if (bIsCatch)
                    m_actorBase.SetGroundTargetPos(moveToPos);
                if (isAttackEnemy)
                {
                    if (RefreshHitPosAndDir())
                        m_actorBase.SetHeadDir(m_HitDir);
                    PlayAttack(m_HitPos, m_HitDir, m_activeEnemy, -1);
                    SetAttackEnemyEnable(false);
                }
            }
        }
        else
        {
            if (null != m_activeEnemy)
            {
                Vector3 dist = m_activeEnemy.GetCurrentPos() - m_actorBase.GetPos();
                float mag = dist.Magnitude();
                if (mag > attMelee.infoAttack.mAttackRange * 2)
                    SetMoveTarget(false, null);
            }
        }

        if (m_actorBase.IsBendBodyEnable())
        {
            if (FightState.FS_INJURED == stateFight && m_actorBase.IsUpperBoneCtrlFree())
                stateFight = FightState.FS_NONE;
        }
    }
    protected virtual bool IsCatchEnemy(out float fOutDist, out bool isAttackEnemy, out Vector3 moveToPos)
    {
        fOutDist = 0;
        isAttackEnemy = false;
        moveToPos = m_actorBase.GetPos();
        if (null == m_activeEnemy)
            return false;
        MIObjBase enemy = m_activeEnemy.GetObject();
        if (null == enemy)
            return false;

        Vector3 MyPos = moveToPos;
        Vector3 TgPos = enemy.GetGPos();

        MPhysRay ray = new MPhysRay();
		ray.vStart = MyPos;
		ray.vDelta = TgPos - MyPos;
        ray.vDelta.Y = 0;
        ray.vDelta.Normalize();
        MPhysRayTraceHit hitInfo = new MPhysRayTraceHit();
        if (enemy.RayTraceObject(ray, out hitInfo, false))
            TgPos = hitInfo.vHit;

        Vector3 dist = MyPos - TgPos;
        fOutDist = dist.Normalize();
        if (AttackRadius >= fOutDist)
            isAttackEnemy = true;

        bool bIsCatchEnemy = true;
        moveToPos = TgPos + dist * closestDist;
        if (closestDist < fOutDist)
        {
            if (standAttackR >= fOutDist)
            {
                if (QueryMoveState(CommonActorBehavior.MoveState.MS_STAND))
                    bIsCatchEnemy = false;
            }
        }
        else
            bIsCatchEnemy = false;
        return bIsCatchEnemy;
    }
    public bool PlayAttack(Vector3 hitPos, Vector3 hitDir, MIAssailable pTargetObj, int nComboCount)
    {
        if (!IsAttackEnable())
           return false;

       if (null != pTargetObj)
        {
            if (!pTargetObj.OnPreAttackedQuery())
                return false;
        }

        MActionAnimInfo AnimInfo = new MActionAnimInfo();
        AnimInfo.mBasicAction = CurrentAttackInfo.mBasicAction;
        AnimInfo.mActionName = GetAttackActionName(0);
        AnimInfo.mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
        MComboAttackActionInfo caai = new MComboAttackActionInfo(AnimInfo);
        caai.mAttack = new MAttackInfo();
        caai.mAttack.mIsPhysXDetection = CurrentAttackInfo.infoAttack.mIsPhysXDetection;
        caai.mAttack.mAttackType = CurrentAttackInfo.infoAttack.mAttackType;
        caai.mAttack.mAttackRange = CurrentAttackInfo.infoAttack.mAttackRange;
        caai.mAttack.mAttHalfRadianRange = CurrentAttackInfo.infoAttack.mAttHalfRadianRange;
        caai.mAttack.mDamageDelayStartSec = CurrentAttackInfo.infoAttack.mDamageDelayStartSec;
        caai.mAttack.mDamageDurationSec = CurrentAttackInfo.infoAttack.mDamageDurationSec;
        caai.mAttack.mTeamGroup = TeamGroup;
        caai.mAttack.mpTarget = pTargetObj;
        caai.mDamage = new MDamageInfo();
        caai.mDamage.mpAttacker = m_actorBase;
        caai.mDamage.mHitPos = hitPos;
        caai.mDamage.mHitDir = hitDir;
        OnSetDefaultDamageInfo(ref caai.mDamage);
        caai.mComboPlayTime = 1;
        caai.mPlaySpeed = 1.5f;
        if ((-1 == nComboCount) || (0 < nComboCount))
        {
            caai.mSeqAnim[0] = new MActionAnimInfo();
            caai.mSeqAnim[0].mBasicAction = CurrentAttackInfo.mBasicAction;
            caai.mSeqAnim[0].mActionName = GetAttackActionName(1);
            caai.mSeqAnim[0].mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
            caai.mSeqActDamDelayStartSec[0] = CurrentAttackInfo.infoAttack.mDamageDelayStartSec;
            caai.mSeqActDamDurationSec[0] = CurrentAttackInfo.infoAttack.mDamageDurationSec;
        }
        if ((-1 == nComboCount) || (1 < nComboCount))
        {
            caai.mSeqAnim[1] = new MActionAnimInfo();
            caai.mSeqAnim[1].mBasicAction = CurrentAttackInfo.mBasicAction;
            caai.mSeqAnim[1].mActionName = GetAttackActionName(2);
            caai.mSeqAnim[1].mActionORMask = (uint)MActionOverrideMask.ActionORMask_LowerBody;
            caai.mSeqActDamDelayStartSec[1] = CurrentAttackInfo.infoAttack.mDamageDelayStartSec;
            caai.mSeqActDamDurationSec[1] = CurrentAttackInfo.infoAttack.mDamageDurationSec;
        }
        if (m_actorBase.PlayAction(MActionLayer.MActionLayer_Active, caai))
        {
            stateFight = FightState.FS_ATTACK;
            if ((int)AttackType.AT_DEFAULT == CurrentAttackInfo.infoAttack.mAttackType)
            {
                if (m_activeEnemy != pTargetObj)
                    m_activeEnemy = pTargetObj;
            }
            if (m_actorBase != null)
            {
                m_actorBase.ResetHitTarget();
                m_actorBase.SetHeadDir(hitDir);
                m_actorBase.SetHitTarget(pTargetObj, 10.0f);
            }
        }
        if (0 < nComboCount)
            m_actorBase.PlayAction(MActionLayer.MActionLayer_Active, caai);
        if (1 < nComboCount)
            m_actorBase.PlayAction(MActionLayer.MActionLayer_Active, caai);

        
        return true;
    }
    public void OnActionEnd(MActionLayer apxActionLayer, MActionInfo pActionInfo)
    {
        if (null != pActionInfo.IsAttackActionInfo())
        {
            if (FightState.FS_INJURED != stateFight)
            {
                stateFight = FightState.FS_NONE;
                currentAttCool = AttackCoolDown;
            }
        }
        else if (IsInjuredAction(ref pActionInfo.mAnimInfo.mActionName))
            stateFight = FightState.FS_NONE;
    }
}
 
