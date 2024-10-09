#pragma once

/*----------------------------------------------------------------------

Carrable.h:

object that can be carried, such as box ...

Created by Yang Xiao, September 29, 2011

----------------------------------------------------------------------*/


class A3DViewport;
class IPhysXObjBase;
class Scene;
class A3DSkinModel;

//可搬动的物体,box等
class CCarryable
{

public:
	//箱子状态
	enum State
	{
		Free,		//自由模式
		ToFree,     //将要自由，被扔出瞬间，不受绑定骨骼的影响，抛箱子阶段
		Hold,       //被搬起，受绑定骨骼的影响
		ToHold,     //将要被搬起，不受绑定骨骼影响
	};

	
public:
	CCarryable(void);
	virtual ~CCarryable(void);

	void Release();
	bool Load(const char* szFile);
	bool Init(IPhysXObjBase* pPhysXObj, Scene* pScene = NULL);

	virtual void Tick(unsigned int uTimeDelta);

	const IPhysXObjBase* GetObject() const { return m_pPhysXObj; }
	bool GetPos(A3DVECTOR3& vPos) const;
	void SetPos(const A3DVECTOR3& vPos);
	bool GetDirAndUp(A3DVECTOR3& vDir, A3DVECTOR3& vUp) const;
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	
	//获取绑定时世界坐标系下左右手的位置
	bool GetBindPosWorld(A3DVECTOR3& vLBindPos, A3DVECTOR3& vRBindPos) const;
	//获取绑定时世界坐标系下左右手的朝向
	bool GetBindDirWorld(A3DVECTOR3& vLBindDir, A3DVECTOR3& vRBindDir) const;

	//搬箱子，在动画播放到fBindFrameRatio(0-1)时箱子与角色绑定, strLChild, strRChild左右绑定点名称(pModel子物体的localname)
	bool TryToCarry(A3DSkinModel* pModel, A3DBone* pLHand, A3DBone* pRHand, const TCHAR* strAnimation, 
		float fBindFrameRatio, const TCHAR* strLChild = NULL, const TCHAR* strRChild = NULL);

	//扔箱子，0到fReadyRatio准备抛阶段，fReadyRatio到fThrowRatio+fReadyRatio时刻抛，fThrowRatio+fReadyRatio到1收手
	bool TryToThrow(const char* strAnimation, float fReadyRatio, float fThrowRatio, float fSpeed);

	State GetState() const { return m_state; }
	void  SetState(State state);

	//model coordinate
	bool GetModelAABB(A3DAABB& aabb) const;
	//world coordinate
	bool GetPhysXAABB(A3DAABB& aabb) const;

	bool GetOBB(A3DOBB& obb) const;

	//update bind position and direction
	bool UpdateBindPosDir();

	static void RenderTest(A3DWireCollector* pWC);

protected:
	//state change event, can be override
	//用来子类覆盖以完成一些操作
	virtual void OnStateChanged(State oldState, State newState) { }

	//检测搬箱子的位置
	bool DetectHandle(A3DVECTOR3& vStart, A3DVECTOR3& vEnd, A3DVECTOR3& vStartNormal, A3DVECTOR3& vEndNormal, A3DMATRIX4* pMat = NULL);

	//与骨骼绑定
	void BindToBone(A3DBone* pLHandBone, A3DBone* pRHandBone);

	//更新m_matBind，根据动画strAnimation的第iFrame帧
	void CalBindMat(A3DSkinModel* pModel, const TCHAR* strAnimation, int iFrame);
	
	//get bone global pose at specific frame of  action
	bool GetBoneGPoseAtFrame(A3DSkinModel* pModel, const char* szAct, const char* szBoneName, int iFrame, A3DMATRIX4& matOut);

	bool isEmpty() const { return m_pPhysXObj == NULL; }

	//获取当前箱子的姿态矩阵
	A3DMATRIX4 GetAbsPose(bool bWithScale = true) const;

	//ray trace the carryable box
	bool RayTrace(const PhysRay& ray, PhysRayTraceHit& traceHit) const;

	//sweep the carryable box
	bool LinearCapsuleSweep(const NxCapsule& worldcapsule, const NxVec3& motion, NxSweepQueryHit& Hit) const;

	//get user defined handle from local name
	bool GetUserDefHandle(const TCHAR* strLChild, const TCHAR* strRChild);

	//
	void SetBindPosAndDir(const A3DVECTOR3& vLWorldPos, const A3DVECTOR3& vRWorldPos, const A3DVECTOR3& vLWorldDir, const A3DVECTOR3& vRWorldDir);

	//计算obb近面及远面到物理实体的距离
	void CalPhysXEdge(A3DOBB obb);
	
protected:	

	//待搬箱子的物理
	IPhysXObjBase* m_pPhysXObj;
	//scene 指针，提供RayTrace方法
	Scene*         m_pScene;
	
	//搬起箱子的角色
	A3DSkinModel*  m_pPorter;

	//绑定到的骨骼
	A3DBone*   m_pBindBone;
	//绑定骨骼下的坐标系
	A3DMATRIX4 m_matBind;
	//绑定坐标系下的坐标
	A3DMATRIX4 m_matInBind;
	//绑定时，世界坐标系下的坐标
	A3DMATRIX4 m_matInWorld;
	A3DQUATERNION m_quatInWorld;
	//绑定前的位置及旋转（世界坐标下）
	A3DVECTOR3 m_vPosFreBind; 
	A3DQUATERNION m_quatPreBind;

	//绑定时，局部坐标系下左右手的位置及朝向
	A3DVECTOR3 m_lBindLocal;
	A3DVECTOR3 m_rBindLocal;
	A3DVECTOR3 m_lBindDirLocal;
	A3DVECTOR3 m_rBindDirLocal;

	//绑定时，全局坐标系下左右手的绑定位置及朝向
	A3DVECTOR3 m_lBindWorld;
	A3DVECTOR3 m_rBindWorld;
	A3DVECTOR3 m_lBindDirWorld;
	A3DVECTOR3 m_rBindDirWorld;

	//free mode & bind mode trans time
	DWORD      m_dwTransTime;
	DWORD      m_dwPreTransTime;//prepare for throw
	DWORD      m_dwTransCnt;// 

	State      m_state;//箱子的状态

	float      m_fDistNearEdge;//distance aabb edge to object edge
	float      m_fDistFarEdge;//distance aabb edge to object edge

	IObjBase*    m_pLBindObj;//左手挂的绑定节点
	IObjBase*    m_pRBindObj;//右手挂的绑定节点

	DWORD        m_dwTickDelta;
	float        m_fThrowAcc;//accelerate
	float        m_fThrowSpeed;//初始速度v0

	A3DMATRIX4   m_matLast;

	bool         m_bUpdateBind;//true if UpdateBindPosDir() in this frame

	//for test
	static A3DVECTOR3 s_vFakeStart;
	static A3DVECTOR3 s_vFakeEnd;
	static A3DAABB    s_aabb;
	static A3DVECTOR3 s_motion;
	static A3DMATRIX4        s_mat;
	static CCoordinateDirection s_coordinate; 
	static IPhysXObjBase*       s_pPhysX;
	static A3DOBB               s_obb;
	static A3DVECTOR3 s_vLeft1;
	static A3DVECTOR3 s_vRight1;
	static A3DVECTOR3 s_vLeft2;
	static A3DVECTOR3 s_vRight2;
};
