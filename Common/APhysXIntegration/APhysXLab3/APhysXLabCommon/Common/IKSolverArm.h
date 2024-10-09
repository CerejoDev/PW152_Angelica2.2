#pragma once

/*----------------------------------------------------------------------

IKSolverArm.h:

IK Solver for arm inherit from A3DIKSolver2Joints...

Created by Yang Xiao, September 29, 2011

----------------------------------------------------------------------*/

class IKSolverArmInterface
{
public:
	virtual ~IKSolverArmInterface() {}

	//called before Solver()，动画数据已经更新完毕
	virtual bool PreSolver() { return true;}
	virtual bool PostSolver() { return true; }
	virtual bool PostRefreshSkeleton() { return true; }
};

#ifdef _ANGELICA3

#include <A3DMacros.h>
#include <A3DIKSolver2Joints.h>

class IKSolverArmInterface;

class IKSolverArm : public A3DIKSolver2Joints
{
public:
	enum State
	{
		State_Normal,
		State_FK2IK,
		State_IK2FK,
	};

public:
	IKSolverArm(A3DEngine* pA3DEngine);
	virtual ~IKSolverArm(void);

	//state from fk to ik (m_dwTransTime), delay dwResistance ms
	//validate when current state is normal
	void SetStateFK2IK(DWORD dwResistance = 0);
	//validate when current state is normal
	//state from ik to fk (m_dwTransTime), delay dwResistance ms
	void SetStateIK2FK(DWORD dwResistance = 0);

	void Tick(DWORD dwDelta);

	void SetTransTime(DWORD dwTransTime) { m_dwTransTime = dwTransTime; }
	DWORD GetTransTime() const { return m_dwTransTime; }

	void SetSolverInterface(IKSolverArmInterface* pInterface) { m_pSolverInterface = pInterface; }

	virtual bool PostRefreshSkeleton();

public:
	virtual bool Solver();

protected:	//	Operations

	//	Bind IK solver with specified skin model
	virtual bool BindWithModel(A3DSkinModel* pSkinModel);
	//	Unbind IK solver from skin model
	virtual void UnbindFromModel();

protected:
	State   m_state;
	DWORD   m_dwTransCnt;//fk ik trans count
	DWORD   m_dwTransTime;//fk ik trans time

	DWORD   m_dwTransResistance;//resistance of transition between IK and FK 

	IKSolverArmInterface* m_pSolverInterface;
};

#endif