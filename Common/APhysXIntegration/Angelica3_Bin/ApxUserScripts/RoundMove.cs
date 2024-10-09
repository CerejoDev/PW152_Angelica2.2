using System;
using System.Collections.Generic;
using System.Text;
using APhysXCommonDNet;
using System.Windows.Forms;

class RoundMove : MIScriptBehavior
{
    private float _fRotSpeed = (float)(20 * Math.PI / 180);
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

        Quaternion quat = ObjBase.GetLRot();

        Quaternion rotQ = Quaternion.RotationAxis(new Vector3(0, 1, 0), dtSec * _fRotSpeed);
        
        quat = rotQ * quat;

        ObjBase.SetLRot(quat);
    }
    public override void PostTick(float dtSec)
    {
        base.PostTick(dtSec);
    }

    public override void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags)
    {
        base.OnKeyDown(nChar, nRepCnt, nFlags);
        if (nChar == (uint)Keys.Decimal)
        {
            _bPause = !_bPause;
        }
        else if (nChar == (uint)Keys.NumPad3)
        {
            ObjBase.ResetPose(true);
            _bPause = true;
        }
    }
}

