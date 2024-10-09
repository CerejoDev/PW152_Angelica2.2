using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;
using System.IO;
using System.Windows.Forms;

class RegionObjInfo
{
    public MIObjBase obj = null;//actor in region
    public float lifeTime = 0;
}
class RegionScript1 : MRegionScript
{
    int num = 5;
    List<RegionObjInfo> objInfos = new List<RegionObjInfo>();

    float accTime = 0;

    public override void EnterRegion(MIObjBase[] objs)
    {
        List<MMainActor> actors = new List<MMainActor>();
        MScene.Instance.GetMainActorCount();
        MScene scene = MScene.Instance;
        for (int i = 0; i < scene.GetMainActorCount(); i++)
        {
            actors.Add(scene.GetMainActor(i));
        }
        Random rand = GetRandom();
        for (int i = 0; i < objs.Length; i++)
        {
            for (int j = 0; j < actors.Count; j++)
            {
                if (actors[j].GetPhysXObjBase() == objs[i])//is actor?
                {
                    objs[i].SetGScale(2);
                    RegionObjInfo info = new RegionObjInfo();
                    info.obj = objs[i];
                    info.lifeTime = (float)rand.NextDouble() * 5 + 5;
                    objInfos.Add(info);
                    break;
                }
            }
        }

        base.EnterRegion(objs);
    }
    public override void LeaveRegion(MIObjBase[] objs)
    {
        for (int i = 0; i < objs.Length; i++)
        {

            for (int j = 0; j < objInfos.Count; j++)
            {
                if (objInfos[j].obj == objs[i])
                {
                    objs[i].SetGScale(1);
                    objInfos.Remove(objInfos[j]);
                    break;
                }
            }

        }

        base.LeaveRegion(objs);
    }
    public override void Start()
    {
        base.Start();
        //generate num people
        MScene scene = MScene.Instance;
        Vector3 vPos = new Vector3();

        string strModel = "";
        if (0x03000000 <= MScene.GetAngelicaSDKVersion())
            strModel = "Models\\Players\\形象\\女\\躯干\\人族女.ecm3";
        else
            strModel = "Models\\Players\\形象\\男\\躯干\\男.ecm";
        for (int i = 0; i < num; i++)
        {
            vPos = RandomPos();
            MMainActor actor = scene.CreateMainActor(vPos, strModel);
        }
    }
    public override void End()
    {
        base.End();
    }
    public override void PostTick(float dtSec)
    {
        base.PostTick(dtSec);

        //shower
        if (objInfos.Count > 0)
        {
            accTime += dtSec;
            if (accTime > 1)
            {
                accTime -= 1;
                Vector3 pos = RandomPos();
                pos.Y = MScene.Instance.GetTerrainHeight(pos.X, pos.Z) + 6;
                string strModel = "";
                if (0x03000000 <= MScene.GetAngelicaSDKVersion())
                    strModel = "Models\\PhysXComponents\\球\\球.ecm3";
                else
                    strModel = "Models\\PhysXComponents\\球\\球.ecm";
                MIObjBase sphere = MScene.Instance.CreateObjectRuntime(MObjTypeID.OBJ_TYPEID_ECMODEL, strModel, pos, false, false, null, null, false);
                MPhysXObjDynamic dynsphere = (MPhysXObjDynamic)sphere;

                sphere.SetGScale(0.5f);
                sphere.EnterRuntime();
                if (dynsphere != null)
                {
                    Vector3 center = Region.GetGPos();
                    center.Y = MScene.Instance.GetTerrainHeight(center.X, center.Z);

                    Vector3 dir = center - pos;
                    dir.Magnitude();
                    dynsphere.SetLinearVelocity(dir * 5);
                }
            }
        }
        else
            accTime = 0;

        //obj die
        for (int i = 0; i < objInfos.Count; i++)
        {
            if (objInfos[i].lifeTime <= 0)
                continue;
            objInfos[i].lifeTime -= dtSec;
            if (objInfos[i].lifeTime < 0)
            {
                MIPhysXObjBase physxObj = (MIPhysXObjBase)objInfos[i].obj;
                if (physxObj != null)
                {
                    physxObj.SetDrivenModeEnforce(MDrivenMode.DRIVEN_BY_PURE_PHYSX);
                }
                objInfos.Remove(objInfos[i]);
                i--;
            }

        }
    }



    Vector3 RandomPos()
    {
        MShapeType type = Region.GetShapeType();

        Vector3 pos = new Vector3(0, 0, 0);
        Vector3 ext = Region.GetVolume(true);

        Random rand = GetRandom();
        switch (type)
        {
            case MShapeType.ST_BOX:
                {
                    double x = (2 * ext.X * rand.NextDouble());
                    double z = (2 * ext.Z * rand.NextDouble());
                    pos.X = (float)(pos.X - ext.X + x);
                    pos.Z = (float)(pos.Z - ext.Z + z);
                }

                break;
            case MShapeType.ST_SPHERE:
                {
                    double r = ext.X * rand.NextDouble();
                    double angle = Math.PI * rand.NextDouble() * 2;
                    pos.X = (float)(pos.X + r * Math.Cos(angle));
                    pos.Z = (float)(pos.Z + r * Math.Sin(angle));
                }
                break;
            case MShapeType.ST_CAPSULE:
                {
                    double r = ext.X * rand.NextDouble();
                    double angle = Math.PI * rand.NextDouble() * 2;
                    pos.X = (float)(pos.X + r * Math.Cos(angle));
                    pos.Z = (float)(pos.Z + r * Math.Sin(angle));
                }
                break;
        }

        Matrix4 mat = Region.GetGPose();
        pos = pos * mat;

        return pos;
    }
}
