/*
object pushable
*/

#pragma once
#ifndef _APHYSXLAB_PUSHABLE_H_
#define _APHYSXLAB_PUSHABLE_H_

class Scene;

class Pushable
{
public:
	Pushable();
	virtual ~Pushable();

	void Init(IPhysXObjBase* pObj, Scene* pScene = NULL);
	void Release();

	IPhysXObjBase* GetPhysXObjBase() { return m_pPhysXObj; }

	bool TryToPush(A3DSkinModel* pModel, A3DBone* pLHand, A3DBone* pRHand, const char* strAnimation, 
		float fBindFrameRatio);
	void ReleasePush();

	//��ȡ��ʱ��������ϵ�������ֵ�λ��
	bool GetBindPosWorld(A3DVECTOR3& vLBindPos, A3DVECTOR3& vRBindPos) const;
	//��ȡ��ʱ��������ϵ�������ֵĳ���
	bool GetBindDirWorld(A3DVECTOR3& vLBindDir, A3DVECTOR3& vRBindDir) const;

	void UpdateBindPosDir();

	bool IsPushing() const { return m_pPorter != NULL; }

	static void RenderTest(A3DWireCollector* pWC);

protected:

	//get bone global pose at specific frame of  action
	bool GetBoneGPoseAtFrame(A3DSkinModel* pModel, const char* szAct, const char* szBoneName, int iFrame, A3DMATRIX4& matOut);

	//sweep the pushable box
	bool LinearCapsuleSweep(const NxCapsule& worldcapsule, const NxVec3& motion, NxSweepQueryHit& Hit) const;

	bool GetObjAABB(A3DAABB& aabb) const;

protected:
	IPhysXObjBase* m_pPhysXObj;

	//scene ָ�룬�ṩRayTrace����
	Scene*         m_pScene;

	//�ƶ����ӵĽ�ɫ
	A3DSkinModel*  m_pPorter;

	A3DBone*       m_pLBone;
	A3DBone*       m_pRBone;

	//��ʱ���ֲ�����ϵ�������ֵ�λ�ü�����(��ɫ����ϵ��)
	A3DVECTOR3 m_lBindLocal;
	A3DVECTOR3 m_rBindLocal;
	A3DVECTOR3 m_lBindDirLocal;
	A3DVECTOR3 m_rBindDirLocal;

	//��ʱ��ȫ������ϵ�������ֵİ�λ�ü�����
	A3DVECTOR3 m_lBindWorld;
	A3DVECTOR3 m_rBindWorld;
	A3DVECTOR3 m_lBindDirWorld;
	A3DVECTOR3 m_rBindDirWorld;

	float      m_fArmLen;

	//for test
	static A3DVECTOR3 s_v1;
	static A3DVECTOR3 s_v2;
	static A3DVECTOR3 s_v1Normal;
	static A3DVECTOR3 s_v2Normal;
};


#endif
