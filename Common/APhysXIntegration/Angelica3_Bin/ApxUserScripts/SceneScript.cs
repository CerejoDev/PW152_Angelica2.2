using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;
using System.Windows.Forms;
using System.Drawing;

enum ShootMode
{
    SM_EYE_TO_CENTER = 0,
    SM_EYE_TO_MOUSE = 1,
    SM_MOUSE_FALLING = 2,
    SM_FRONT_FALLING = 3,

    SM_END_BOUND
};
enum ShootStuffID
{
    SSI_SPHERE = 0,
    SSI_BOX = 1,
    SSI_BARREL = 2,
    SSI_BREAKABLEBOX,
    SSI_BOMB,

    SSI_END_BOUND
};

class SceneScript : MIScriptBehavior
{
    private List<MIObjBase> shootActors = new List<MIObjBase>();

    int iBindObj = 0;
    
    ShootMode m_iShootMode = ShootMode.SM_EYE_TO_CENTER;
    ShootStuffID m_iShootStuff = ShootStuffID.SSI_SPHERE;

    MIObjBase CameraSpotLightObj;

    bool m_bFPS = false;

    float m_fPhysBlendWeight = 0;
    string m_strPhysBone = "Bip01 Spine";
    bool bEnableIK = true;

    private string GetShootModeDes(ShootMode mode)
    {
        string str;
        switch(mode)
        {
            case ShootMode.SM_EYE_TO_CENTER:
                str = "随摄像机";
                break;
            case ShootMode.SM_EYE_TO_MOUSE:
                str = "弹向鼠标";
                break;
            case ShootMode.SM_FRONT_FALLING:
                str = "前方落体";
                break;
            case ShootMode.SM_MOUSE_FALLING:
                str = "鼠标落体";
                break;
            default:
                str = "未知";
                break;
        }
        return str;
    }
    private string GetShootStuffDes(ShootStuffID StuffID)
    {
        string str;
        switch(StuffID)
        {
            case ShootStuffID.SSI_SPHERE:
                str = "球";
                break;
            case ShootStuffID.SSI_BOX:
                str = "箱子";
                break;
            case ShootStuffID.SSI_BREAKABLEBOX:
                str = "破箱子";
                break;
            case ShootStuffID.SSI_BARREL:
                str = "木桶";
                break;
            case ShootStuffID.SSI_BOMB:
                str = "炸弹";
                break;
            default:
                str = "未知";
                break;
        }
        return str;
    }

    private void GetShootStuffTypeAndFile(ref MObjTypeID type, ref string szFile)
    {
        if (m_iShootStuff == ShootStuffID.SSI_BOMB)
        {
            type = MObjTypeID.OBJ_TYPEID_FORCEFIELD;
            szFile = "ForceField\\explo.xff";
        }
        else if (m_iShootStuff == ShootStuffID.SSI_SPHERE)
        {
            type = MObjTypeID.OBJ_TYPEID_ECMODEL;
            if (0x03000000 <= MScene.GetAngelicaSDKVersion())
                szFile = "Models\\PhysXComponents\\球\\球.ecm3";
            else
                szFile = "Models\\PhysXComponents\\球\\球.ecm";
        }
        else if (m_iShootStuff == ShootStuffID.SSI_BOX)
        {
            type = MObjTypeID.OBJ_TYPEID_ECMODEL;
            if (0x03000000 <= MScene.GetAngelicaSDKVersion())
                szFile = "Models\\PhysXComponents\\箱子\\箱子.ecm3";
            else
                szFile = "Models\\PhysXComponents\\箱子\\箱子.ecm";
        }
        else if (m_iShootStuff == ShootStuffID.SSI_BREAKABLEBOX)
        {
            type = MObjTypeID.OBJ_TYPEID_ECMODEL;
            if (0x03000000 <= MScene.GetAngelicaSDKVersion())
                szFile = "Models\\PhysXComponents\\箱子破损\\箱子破损.ecm3";
            else
                szFile = "Models\\PhysXComponents\\箱子破损\\箱子破损.ecm";
        }
        else if (m_iShootStuff == ShootStuffID.SSI_BARREL)
        {
            type = MObjTypeID.OBJ_TYPEID_ECMODEL;
            if (0x03000000 <= MScene.GetAngelicaSDKVersion())
                szFile = "Models\\PhysXComponents\\桶\\桶.ecm3";
            else
                szFile = "Models\\PhysXComponents\\桶\\桶.ecm";
        }
        else
        {
            MLogWrapper.Instance.LogText("error shoot stuff");
        }
    }

    private void ShootStuff()
    {
        Vector3 hitPos = new Vector3(0, 0, 0);
        MPhysRay ray = new MPhysRay();
        if (m_iShootMode == ShootMode.SM_EYE_TO_MOUSE || m_iShootMode == ShootMode.SM_MOUSE_FALLING)
        {
            Point pt;
            if (!MInput.Instance.GetMousePos(out pt))
                return;
            
            if (MScene.Instance.ScreenPosToRay(pt, ref ray))
            {
                if (m_iShootMode == ShootMode.SM_EYE_TO_MOUSE)
                    ray.vDelta.Normalize();
                else
                {
                    if (!MScene.Instance.RayTracePhysXObjectAndTerrain(pt, ray, out hitPos))
                        return;
                }
            }
        }

        MObjTypeID type = MObjTypeID.OBJ_TYPEID_ECMODEL;
        string szFile = "";
        GetShootStuffTypeAndFile(ref type, ref szFile);


        MIObjBase obj = MScene.Instance.CreateObjectRuntime(type, szFile, hitPos, false, false, null, null, false);
        if (obj == null)
        {
            MLogWrapper.Instance.LogText("File: " + szFile + " load error");
            return;
        }
        shootActors.Add(obj);

        MCameraController pCamera = MScene.Instance.GetActiveSmartCamCtrler(true);
        Vector3 vEyeDir = pCamera.GetDir();
        Vector3 vEyePos = pCamera.GetPos();

        float fVelocity = 20.0f;
        float scale = 1;
        Vector3 pos = vEyePos;
	    Vector3 vel = fVelocity * vEyeDir;
	    if (ShootMode.SM_EYE_TO_MOUSE == m_iShootMode)
		    vel = fVelocity * ray.vDelta;    
	    else if (ShootMode.SM_FRONT_FALLING == m_iShootMode)
	    {
            vel.Clear();
		    pos += vEyeDir * 10;
		    float y = MScene.Instance.GetTerrainHeight(pos.X, pos.Z);
            if (y > pos.Y) pos.Y = y + 2; 
	    }
	    else if (ShootMode.SM_MOUSE_FALLING == m_iShootMode)
	    {
            vel.Clear();
		    pos = hitPos;
		    pos.Y += 4;
	    }
	    
        obj.SetGPos(pos);
        obj.SetGScale(scale);
        obj.EnterRuntime();

        if (ShootStuffID.SSI_BOMB != m_iShootStuff)
	    {
            MPhysXObjDynamic dynObj = obj as MPhysXObjDynamic;
            if (dynObj != null)
            {
                dynObj.SetLinearVelocity(vel);
            }
        }
    }

    //preparing work
    public override void Start()
    {
        MScene scene = (MScene)ObjBase;
        if (scene != null)
        {
            //Vector3 vPos = scene.GetMCameraController().GetPos();
            //vPos = vPos + new Vector3(0, 0, 5);
            //obj = (MIPhysXObjBase)scene.CreateObjectRuntime(MObjTypeID.OBJ_TYPEID_STATIC, "Models\\UnlitModel\\testbuilding.umd", vPos, true, true, "abc", null, false);
            //MIScriptBehavior behavior = (MIScriptBehavior)ScriptFactory.Instance.CreateInstance("Building");
            //obj.AddScript(behavior);

            //---------------------------------------------------------------------
            // try to add a camera spot light
            MCameraController CameraController = scene.GetActiveSmartCamCtrler(true);
            Vector3 pos = new Vector3(0.0f);
            CameraSpotLightObj = scene.CreateObjectRuntime(MObjTypeID.OBJ_TYPEID_SPOTLIGHT, null, pos, false, true, null, null, false);
            CameraController.AddChild(CameraSpotLightObj);
            Matrix4 matID = new Matrix4();
            matID = Matrix4.Identity;
            CameraSpotLightObj.SetLPose(matID);

            // scene.GetCurrentMA().GetMActorBase().SetAutoTurn(!m_bFPS);
        }
    }

    public override void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags)
    {
        MScene scene = (MScene)ObjBase;
        if (nChar == (uint)Keys.Space)
        {
            ShootStuff();
        }
        else if (nChar == (uint)Keys.U)
        {
            for (int i = 0; i < shootActors.Count; i++)
                scene.ReleaseObject(shootActors[i]);
            shootActors.Clear();
        }
        else if (nChar == (uint)Keys.L)
        {
            MCameraController camera = scene.GetActiveSmartCamCtrler(true);
            if (camera.QueryMode(MCameraMode.MODE_FREE_FLY))
                return;

            List<MIObjBase> objs = scene.GetMIObjBase(MObjTypeID.OBJ_TYPEID_ALL);
            if (0 == objs.Count)
                return;
            iBindObj %= objs.Count;
            MIObjBase pObj = objs[iBindObj];
            if (0 < camera.GetRelationshipCode(pObj))
            {
                MCamOptTask_EntryBindingMode camParams = new MCamOptTask_EntryBindingMode(objs[iBindObj]);
                scene.GetMCamCtrlerAgent().ExecuteCameraOperation(camParams);
            }
            iBindObj++;
        }
        else if (nChar == (uint)Keys.M)
        {
            MCameraController camera = scene.GetActiveSmartCamCtrler(true);
            if (camera.QueryMode(MCameraMode.MODE_FREE_FLY))
                return;
            MMainActor actor = scene.GetCurrentMA();
            MCamOptTask_EntryBindingMode camParams = new MCamOptTask_EntryBindingMode(actor.GetPhysXObjBase());
            scene.GetMCamCtrlerAgent().ExecuteCameraOperation(camParams);
        }
        else if (nChar == (uint)Keys.Delete)
        {
            MCameraController camera = scene.GetActiveSmartCamCtrler(true);
            MIObjBase obj = camera.GetFocusObj();
            MMainActor actor = scene.GetCurrentMA();
            if (obj != null && obj != actor.GetPhysXObjBase())
                MScene.Instance.ReleaseObject(obj);
        }

        else if (nChar == (uint)Keys.B)
        {
            if (MInput.Instance.IsKeyDown((int)Keys.ControlKey))
                MScene.Instance.ToBreakAndRagdoll();
        }
        else if (nChar == (uint)Keys.K)
        {
            MMainActor MA = scene.GetCurrentMA();
            if (MA == null)
            {
                MCameraController camera = scene.GetActiveSmartCamCtrler(true);
                Vector3 vPos = camera.GetPos();
                Vector3 vDir = camera.GetDirH();
                vPos = vPos + vDir * 5;
                string strModel = "";
                if (0x03000000 <= MScene.GetAngelicaSDKVersion())
                    strModel = "Models\\Players\\形象\\女\\躯干\\人族女.ecm3";
                else
                    strModel = "Models\\Players\\形象\\男\\躯干\\男.ecm";
                MA = MScene.Instance.CreateMainActor(vPos, strModel);
                MA.EnterCameraBinding(true);

                MActorBase actor = MA.GetMActorBase();
                WomanActionReport action = new WomanActionReport();
                actor.SetMActionReport(action);
                actor.RegisterKeyEvent();
                actor.SetGroundMoveSpeed(3);
                actor.InitHandIK("Bip01 L Hand", "Bip01 R Hand");
                actor.SetBlendAction("run_Left_fix2", "run_Left_fix2_inv", "run_Right_fix2", "run_Right_fix2_inv",
                    "run_Forward_speedup", "run_Back_Move2", "walk_Forward_Move2");

                if (actor.IsEnableDamageFromPhysXContact())
                    scene.RegisterPhysXContactDamage(actor);
            }
        }
        else if (nChar == (uint)Keys.D8)
        {
            m_iShootStuff++;
            if (m_iShootStuff >= ShootStuffID.SSI_END_BOUND)
                m_iShootStuff = (ShootStuffID)(0);
        }
        else if (nChar == (uint)Keys.D9)
        {
            m_iShootMode++;
            if (m_iShootMode >= ShootMode.SM_END_BOUND)
                m_iShootMode = (ShootMode)(0);
        }
        else if (nChar == (uint)Keys.H)
        {
            // turn on/off camera spot light...
            CameraSpotLightObj.Enable(!CameraSpotLightObj.IsEnabled());
        }
        else if (nChar == (uint)Keys.Y)
        {
            MMainActor MA = scene.GetCurrentMA();
            if (MA != null)
            {
                MA.GetMActorBase().SetUseBodyCtrl(!MA.GetMActorBase().IsUseBodyCtrl());
            }
        }

#if (_ANGELICA21)
        else if (nChar == (uint)Keys.Home)
        {
            MMainActor MA = scene.GetCurrentMA();
            if (MA != null)
            {
                //float fWeight = MA.GetMActorBase().GetPhysBlendWeight("Bip01 Spine");
                m_fPhysBlendWeight += 0.05f;
                if (m_fPhysBlendWeight > 1)
                    m_fPhysBlendWeight = 1;
                MA.GetMActorBase().SwitchToPhys(m_strPhysBone, m_fPhysBlendWeight);
            }
        }
        else if (nChar == (uint)Keys.End)
        {
            MMainActor MA = scene.GetCurrentMA();
            if (MA != null)
            {
                //float fWeight = MA.GetMActorBase().GetPhysBlendWeight("Bip01 Spine");
                m_fPhysBlendWeight -= 0.05f;
                if (m_fPhysBlendWeight < 0)
                    m_fPhysBlendWeight = 0;
                MA.GetMActorBase().SwitchToPhys(m_strPhysBone, m_fPhysBlendWeight);
            }
        }
        else if (nChar == (uint)Keys.Multiply)
        {
            MMainActor MA = scene.GetCurrentMA();
            if (MA != null)
            {
                string strNew = MA.GetMActorBase().GetParentBone(m_strPhysBone);
                if (strNew != null)
                {
                    MA.GetMActorBase().SwitchToPhys(m_strPhysBone, 0);
                    m_strPhysBone = strNew;
                    MA.GetMActorBase().SwitchToPhys(m_strPhysBone, m_fPhysBlendWeight);
                }
            }
        }
        else if (nChar == (uint)Keys.Divide)
        {
            MMainActor MA = scene.GetCurrentMA();
            if (MA != null)
            {
                string strNew = MA.GetMActorBase().GetFirstChildBone(m_strPhysBone);
                if (strNew != null)
                {
                    MA.GetMActorBase().SwitchToPhys(m_strPhysBone, 0);
                    m_strPhysBone = strNew;
                    MA.GetMActorBase().SwitchToPhys(m_strPhysBone, m_fPhysBlendWeight);
                }
            }
        }
        else if (nChar == (uint)Keys.Insert)
        {
            MMainActor MA = scene.GetCurrentMA();
            if (MA != null)
            {
                string strNew = MA.GetMActorBase().GetNextSiblingBone(m_strPhysBone);
                if (strNew != null)
                {
                    MA.GetMActorBase().SwitchToPhys(m_strPhysBone, 0);
                    m_strPhysBone = strNew;
                    MA.GetMActorBase().SwitchToPhys(m_strPhysBone, m_fPhysBlendWeight);
                }
            }
        }
#endif
    }


    //leave play mode
    public override void End()
    {
    }

    //pre scene tick
    public override void PreTick(float dtSec)
    {
    }

    //post scene tick
    public override void PostTick(float dtSec)
    {
        MScene scene = MScene.Instance;
        int count = scene.GetMainActorCount();
        if (0 >= count)
            return;

        MMainActor curActor = scene.GetCurrentMA();
        Vector3 pos = scene.GetCurrentMA().GetPos();
        Random rand = GetRandom();
        for (int i = 0; i < count; i++)
        {
            MMainActor actor = scene.GetMainActor(i);
            if (curActor == actor)
                continue;

            Vector3 delta = actor.GetDeltaMove();
            if (delta.SquaredMagnitude() < 1E-5f)
            {
                //generate new target
                double r = 20 * rand.NextDouble();
                double angle = Math.PI * rand.NextDouble() * 2;

                Vector3 vTarget = pos;
                vTarget.X += (float)(r * Math.Cos(angle));
                vTarget.Z += (float)(r * Math.Sin(angle));

                vTarget.Y = scene.GetTerrainHeight(vTarget.X, vTarget.Z);
                actor.WalkTo(vTarget);
            }
        }

        MCameraController camera = scene.GetActiveSmartCamCtrler(true);
        if (camera.QueryMode(MCameraMode.MODE_BINDING) && m_bFPS)
        {
            //
            MActorBase actorBase = curActor.GetMActorBase();
            if (actorBase.IsAutoTurn())
                actorBase.SetAutoTurn(false);
            //actorBase.SetHeadDir(camera.GetDirH());
        }
    }

    public override void OnRender(MRender render)
    {
    	int w = 0, h = 0;
        if  (MScene.Instance.GetClientRect(ref w, ref h))
        {
            int x = w - 150, y = h - 90;
            string strSM = "弹物模式(9): " + GetShootModeDes(m_iShootMode);
            string strStuff = "弹物类别(8): " + GetShootStuffDes(m_iShootStuff);
            render.RenderText(x, y, strStuff, 0xffff0000);
            y = y + 30;
            render.RenderText(x, y, strSM, 0xffff0000);
            y = y + 30;
            
            MScene scene = (MScene)ObjBase;
            MMainActor mainActor = scene.GetCurrentMA();
            if (mainActor != null)
            {
                MActorBase actor = mainActor.GetMActorBase();
                string str = "Enable IK ";
                if (actor.IsIKEnabled())
                    str += "true";
                else
                    str += "false";
                render.RenderText(x, y, str, 0xffff0000);

                y = y + 30;

                x = 500;
                y = 300;
                str = "v: " + mainActor.GetMActorBase().GetGroundMoveSpeed();
                render.RenderText(x, y, str, 0xffff0000);
                y = y + 30;
                str = "use bonecontroller: ";
                if (mainActor.GetMActorBase().IsUseBodyCtrl())
                    str += "true";
                else
                    str += "false";
                render.RenderText(x, y, str, 0xffff0000);

                y = y + 30;
                render.RenderText(x, y, m_strPhysBone + ": " + m_fPhysBlendWeight.ToString(), 0xffff0000);
                Vector3 vPos = actor.GetPos();

                Vector3 vDir = actor.GetHeadDir();
                Vector3 vTestDir = actor.GetTestDir();
                render.AddWire3DLine(vPos, vPos + vDir, 0xff00ff00);
                render.AddWire3DLine(vPos, vPos + vTestDir, 0xffff0000);
            }
            
        }
    }


    private void Pause(bool bPause)
    {
        for (int i = 0; i < ObjBase.ScriptBehaviors.Count; i++)
        {
            ObjBase.ScriptBehaviors[i].IsPause = bPause;
        }
        List<MIObjBase> objs = MScene.Instance.GetMIObjBase(MObjTypeID.OBJ_TYPEID_ALL);
        for (int i = 0; i < objs.Count; i++)
        {
            Pause(objs[i], bPause);
        }
    }
    private void Pause(MIObjBase obj, bool bPause)
    {
        List<MIScriptBehavior> scripts = obj.ScriptBehaviors;
        for (int i = 0; i < scripts.Count; i++)
        {
            scripts[i].IsPause = bPause;
        }
        for (int i = 0; i < obj.GetChildNum(); i++)
        {
            Pause(obj.GetChild(i), bPause);
        }
    }
}
