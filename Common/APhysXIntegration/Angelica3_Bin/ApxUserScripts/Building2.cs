using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;

public class Building2 : MIScriptBehavior
{
    private float _v = 5;
    private float _length = 20;
    private float _acclength = 0;
    private float _fRotSpeed = (float)(20 * Math.PI / 180);
    private string _name = "abc";

    public float Vel
    {
        get { return _v; }
        set { _v = value; }
    }
    public float Length
    {
        get { return _length; }
    }
    public string Name
    {
        get { return _name; }
        set { _name = value; }
    }

    //preparing work
    public override void Start()
    {
        MIPhysXObjBase physxobj = (MIPhysXObjBase)ObjBase;
        physxobj.SetDrivenMode(MDrivenMode.DRIVEN_BY_ANIMATION);
    }

    //leave play mode
    public override void End()
    {
    }

    //pre scene tick
    public override void PreTick(float dtSec)
    {
        List<MIScriptBehavior> scripts = MScene.Instance.ScriptBehaviors;
        Vector3 vStart = ObjBase.GetLPos();

        float tsecond = dtSec;
        _acclength += _v * tsecond;

        Vector3 offset = new Vector3(_v * tsecond, 0, 0);

        vStart = vStart + offset;

        //ObjBase.SetLPos(vStart);
        
        Quaternion quat = ObjBase.GetLRot();

        Quaternion rotQ = Quaternion.RotationAxis(new Vector3(0, 1, 0), tsecond * _fRotSpeed);
        // quat = quat * rotQ;
        quat = rotQ * quat;

        ObjBase.SetLRot(quat);

        if (_acclength > _length)
            _v = -Math.Abs(_v);
        else if (_acclength < 0)
            _v = Math.Abs(_v);
    }

    //post scene tick
    public override void PostTick(float dtSec)
    {

        

    }

    public override void OnRender(MRender render)
    {
        string str = "test output ok";
        render.RenderText(20, 20, str, 0x80ff0000);


    }

}
