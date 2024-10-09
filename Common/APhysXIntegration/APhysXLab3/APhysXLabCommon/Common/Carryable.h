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

//�ɰᶯ������,box��
class CCarryable
{

public:
	//����״̬
	enum State
	{
		Free,		//����ģʽ
		ToFree,     //��Ҫ���ɣ����ӳ�˲�䣬���ܰ󶨹�����Ӱ�죬�����ӽ׶�
		Hold,       //�������ܰ󶨹�����Ӱ��
		ToHold,     //��Ҫ�����𣬲��ܰ󶨹���Ӱ��
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
	
	//��ȡ��ʱ��������ϵ�������ֵ�λ��
	bool GetBindPosWorld(A3DVECTOR3& vLBindPos, A3DVECTOR3& vRBindPos) const;
	//��ȡ��ʱ��������ϵ�������ֵĳ���
	bool GetBindDirWorld(A3DVECTOR3& vLBindDir, A3DVECTOR3& vRBindDir) const;

	//�����ӣ��ڶ������ŵ�fBindFrameRatio(0-1)ʱ�������ɫ��, strLChild, strRChild���Ұ󶨵�����(pModel�������localname)
	bool TryToCarry(A3DSkinModel* pModel, A3DBone* pLHand, A3DBone* pRHand, const TCHAR* strAnimation, 
		float fBindFrameRatio, const TCHAR* strLChild = NULL, const TCHAR* strRChild = NULL);

	//�����ӣ�0��fReadyRatio׼���׽׶Σ�fReadyRatio��fThrowRatio+fReadyRatioʱ���ף�fThrowRatio+fReadyRatio��1����
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
	//�������า�������һЩ����
	virtual void OnStateChanged(State oldState, State newState) { }

	//�������ӵ�λ��
	bool DetectHandle(A3DVECTOR3& vStart, A3DVECTOR3& vEnd, A3DVECTOR3& vStartNormal, A3DVECTOR3& vEndNormal, A3DMATRIX4* pMat = NULL);

	//�������
	void BindToBone(A3DBone* pLHandBone, A3DBone* pRHandBone);

	//����m_matBind�����ݶ���strAnimation�ĵ�iFrame֡
	void CalBindMat(A3DSkinModel* pModel, const TCHAR* strAnimation, int iFrame);
	
	//get bone global pose at specific frame of  action
	bool GetBoneGPoseAtFrame(A3DSkinModel* pModel, const char* szAct, const char* szBoneName, int iFrame, A3DMATRIX4& matOut);

	bool isEmpty() const { return m_pPhysXObj == NULL; }

	//��ȡ��ǰ���ӵ���̬����
	A3DMATRIX4 GetAbsPose(bool bWithScale = true) const;

	//ray trace the carryable box
	bool RayTrace(const PhysRay& ray, PhysRayTraceHit& traceHit) const;

	//sweep the carryable box
	bool LinearCapsuleSweep(const NxCapsule& worldcapsule, const NxVec3& motion, NxSweepQueryHit& Hit) const;

	//get user defined handle from local name
	bool GetUserDefHandle(const TCHAR* strLChild, const TCHAR* strRChild);

	//
	void SetBindPosAndDir(const A3DVECTOR3& vLWorldPos, const A3DVECTOR3& vRWorldPos, const A3DVECTOR3& vLWorldDir, const A3DVECTOR3& vRWorldDir);

	//����obb���漰Զ�浽����ʵ��ľ���
	void CalPhysXEdge(A3DOBB obb);
	
protected:	

	//�������ӵ�����
	IPhysXObjBase* m_pPhysXObj;
	//scene ָ�룬�ṩRayTrace����
	Scene*         m_pScene;
	
	//�������ӵĽ�ɫ
	A3DSkinModel*  m_pPorter;

	//�󶨵��Ĺ���
	A3DBone*   m_pBindBone;
	//�󶨹����µ�����ϵ
	A3DMATRIX4 m_matBind;
	//������ϵ�µ�����
	A3DMATRIX4 m_matInBind;
	//��ʱ����������ϵ�µ�����
	A3DMATRIX4 m_matInWorld;
	A3DQUATERNION m_quatInWorld;
	//��ǰ��λ�ü���ת�����������£�
	A3DVECTOR3 m_vPosFreBind; 
	A3DQUATERNION m_quatPreBind;

	//��ʱ���ֲ�����ϵ�������ֵ�λ�ü�����
	A3DVECTOR3 m_lBindLocal;
	A3DVECTOR3 m_rBindLocal;
	A3DVECTOR3 m_lBindDirLocal;
	A3DVECTOR3 m_rBindDirLocal;

	//��ʱ��ȫ������ϵ�������ֵİ�λ�ü�����
	A3DVECTOR3 m_lBindWorld;
	A3DVECTOR3 m_rBindWorld;
	A3DVECTOR3 m_lBindDirWorld;
	A3DVECTOR3 m_rBindDirWorld;

	//free mode & bind mode trans time
	DWORD      m_dwTransTime;
	DWORD      m_dwPreTransTime;//prepare for throw
	DWORD      m_dwTransCnt;// 

	State      m_state;//���ӵ�״̬

	float      m_fDistNearEdge;//distance aabb edge to object edge
	float      m_fDistFarEdge;//distance aabb edge to object edge

	IObjBase*    m_pLBindObj;//���ֹҵİ󶨽ڵ�
	IObjBase*    m_pRBindObj;//���ֹҵİ󶨽ڵ�

	DWORD        m_dwTickDelta;
	float        m_fThrowAcc;//accelerate
	float        m_fThrowSpeed;//��ʼ�ٶ�v0

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
