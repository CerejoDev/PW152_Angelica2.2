using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;
using System.Windows.Forms;


public class VerticalMove : MIScriptBehavior
{
    private float _v = 5;
    private float _length = 40;
    private float _acclength = 0;
    private bool _bPause = false;

    public override void Start()
    {
        base.Start();
        MIPhysXObjBase physxobj = (MIPhysXObjBase)ObjBase;
        physxobj.SetDrivenMode(MDrivenMode.DRIVEN_BY_ANIMATION);
    }
    public override void End()
    {
        base.End();
    }
    public override void PreTick(float dtSec)
    {
        if (_bPause)
            return;

        base.PreTick(dtSec);

        Vector3 vStart = ObjBase.GetLPos();

        float tsecond = dtSec;
        _acclength += _v * tsecond;

        Vector3 offset = new Vector3(0, _v * tsecond, 0);

        vStart = vStart + offset;

        ObjBase.SetLPos(vStart);

        if (_acclength > _length)
            _v = -Math.Abs(_v);
        else if (_acclength < 0)
            _v = Math.Abs(_v);
    }
    public override void PostTick(float dtSec)
    {
        base.PostTick(dtSec);
    }
    public override void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags)
    {
        base.OnKeyDown(nChar, nRepCnt, nFlags);
        if (nChar == (uint)Keys.NumPad0)
        {
            _bPause = !_bPause;
        }
        else if (nChar == (uint)Keys.NumPad3)
        {
            ObjBase.ResetPose(true);
            _acclength = 0;
            _v = Math.Abs(_v);
            _bPause = true;
        }
    }
}

