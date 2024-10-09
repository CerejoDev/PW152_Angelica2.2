#pragma once

/*----------------------------------------------------------------------

IKHandTarget.h:

define hand position for arm IK computing...

Created by Yang Xiao, September 29, 2011

----------------------------------------------------------------------*/


class A3DWireCollector;
class A3DSkinModel;
class A3DIKSolver;
class A3DIKTrigger;
class IKSolverArm;
class IKSolverArmInterface;

class CIKHandTarget
{
	typedef CCoordinateDirection::EditType EditType;

public:
	enum Flag
	{
		IK_START_CARRY = 1,

		IK_CARRY_MODE = 1<<5,
		IK_DRAG_MODE  = 1<<6,
	};

private:
	enum State
	{
		State_FK,
		State_FK2IK,
		State_IK,
		State_IK2FK,
	};

public:
	CIKHandTarget(void);
	~CIKHandTarget(void);

	void Init(A3DSkinModel* pSkinModel, const char* strIKBoneName);
	void Release();

	void SetPos(const A3DVECTOR3& vPos, bool bAtOnce = false);
	A3DVECTOR3 GetPos() const { return m_refFrame.GetPos(); }
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp) { m_refFrame.SetDirAndUp(vDir, vUp); }
	void GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) const { m_refFrame.GetDirAndUp(vDir, vUp); }

	bool UpdateRefFrameOnLButtonDown(const int x, const int y, const unsigned int nFlags);
	bool UpdateRefFrameOnMouseMove(const int x, const int y, const unsigned int nFlags);

	void Tick(DWORD dwTime);

	void DrawFrame(A3DWireCollector* pWC);
	void SetRefFrameEditMode(const EditType& et);
	EditType GetRefFrameEditMode() const;
	bool QueryRefFrameEditMode(const EditType& et) const { return et == GetRefFrameEditMode(); }

	A3DVECTOR3 GetHandDir() const;
	//
	void SetHandDir(const A3DVECTOR3& vHandDir, const A3DVECTOR3& vFinger, bool bAtOnce = true);
	void SetHandDir(const A3DVECTOR3& vHandDir, bool bAtOnce = true);

	void RaiseIKMode(const Flag& dwFlag) { m_dwFlag |= dwFlag; }
	void ClearIKMode(const Flag& dwFlag) { m_dwFlag &= ~dwFlag; }
	bool ReadIKMode(const Flag& dwFlag) const { return m_dwFlag & dwFlag ? true : false; }

	//dwTime: FK to IK转换时间
	//dwResistance: dwResistance ms后FK到IK转换  
	void StartCarry(DWORD dwTime, DWORD dwResistance = 0);
	//dwTime: IK to FK转换时间
	//dwResistance: dwResistance ms后IK到FK转换  
	void StartThrow(DWORD dwTime, DWORD dwResistance = 0);

	void EnableIK(bool bEnable = true);
	bool IsIKEnable() const;

	A3DBone* GetIKBone() { return m_pIKBone; }

	void SetIKGoalPos(const A3DVECTOR3& vPos);

	void SetSolverInterface(IKSolverArmInterface* pInterface);

	//init hand direction in local coordinate
	void SetInitLHandDir(const A3DVECTOR3& vDir) { m_vInitLHandDir = vDir; m_vInitLHandDir.Normalize();}
	A3DVECTOR3 GetInitLHandDir() const { return m_vInitLHandDir; }

protected:
	void SetIKTargetRot(const float angleRadians, const A3DVECTOR3& localAxis, const A3DVECTOR3& globalAxis);

	void InternalSetHandDir(const A3DVECTOR3& vHandDir);
	void InternalSetHandDir(const A3DVECTOR3& vHandDir, const A3DVECTOR3& vFinger);

	void GetRotMatrix(const A3DVECTOR3& vFrom, const A3DVECTOR3& vTo, A3DMATRIX3& retMat, A3DVECTOR3& vRotAxis);

protected:
	CCoordinateDirection m_refFrame;
	A3DSkinModel*        m_pSkinModel;//绑定的模型

	A3DIKTrigger*        m_pTrigger;//for ik
	IKSolverArm*         m_pIKSolver;// for ik solver
	A3DBone*             m_pIKBone;//hand bone

	A3DQUATERNION        m_Quat;//hand quat

	DWORD                m_dwFlag;

	//通过IK设置末端位置及朝向，过渡时间
	float                m_fTransTime;
	float                m_fTransCnt;

	//状态
	State                m_state;

	//手朝向改变角速度
	float                m_fRotSpeed;
	A3DVECTOR3           m_vTargetDir;//hand direction
	A3DVECTOR3           m_vRotAxis;
	float                m_fRotAngle;

	A3DVECTOR3           m_vInitLHandDir;//init hand direction in local coordinate, z for default
};
