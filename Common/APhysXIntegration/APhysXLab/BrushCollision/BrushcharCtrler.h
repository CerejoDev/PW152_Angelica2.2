/*
 * BrushcharCtrler.h: interface for the Brush collision based charactor controller class
 *
 * DESCRIPTION: Actor used to collision detection(Capsule and Brush)
 *
 * CREATED BY: YangXiao, 2010/12/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BRUSHCHARCTRLER_H__D0277489_B173_41CC_8B5A_21560F925C8D__INCLUDED_)
#define AFX_BRUSHCHARCTRLER_H__D0277489_B173_41CC_8B5A_21560F925C8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ECCDR.h"
#include "Capsule.h"

class CBrushesObj;

using namespace CHBasedCD;

struct BrushcharCtrlerDes{
	float fCapsuleRadius;//���ҵİ뾶
	float fCylinerHei;//����Բ�����ָ߶�
	float fSlopeThresh;//�����ϵ��б��
	float fStepHeight;//̧�ȸ߶�
	float fMaxFallSpeed;//��������ٶ�
	float fSlopeMaxH;//����������̶���
	float fSpeed;//�ƶ��ٶ�
	BrushcharCtrlerDes()
	{
		fCapsuleRadius = 0.3f;
		fCylinerHei = 1;
		fSlopeThresh = 0.7071f;
		fStepHeight = 0.8f;
		fMaxFallSpeed = -20.f;
		fSlopeMaxH = 0.2f;
		fSpeed = 0;
	}
};

class BrushcharCtrler 
{
	enum State { INAIR, ONGROUND };
public:
	BrushcharCtrler(const BrushcharCtrlerDes& brushccDes);
	virtual ~BrushcharCtrler();

	virtual float GetStepOffset() const { return m_CDRInfo.fStepHeight; }
	virtual void SetStepOffset(const float offset) { m_CDRInfo.fStepHeight = offset; }

	void SetPos(const A3DVECTOR3& vPos) { m_capsule.SetPos(vPos); }
	A3DVECTOR3 GetPos() const { return m_capsule.GetPos(); }

	CCapsule& GetCapsule() { return m_capsule; }

	virtual A3DVECTOR3 MoveCC(const float deltaSecond, const A3DVECTOR3& MoveDir);
	virtual void  JumpCC(float fJumpingSpeed, bool bIsAbsoluteSpeed = false);

	float GetSpeed() const { return m_speed; }
	void  SetSpeed(const float v) { m_speed = v; }

	//�޸�Capsule�Ĵ�С���ɹ�����TRUE��ʧ�ܷ���FALSE
	bool ChangeCapsuleVolume(float fRadius, float fCylinderHei);

	A3DVECTOR3 GetFootPosition() const { return m_capsule.GetPos() - A3DVECTOR3(0, m_capsule.GetCenterHei(), 0); }

	static bool IsPosAvailable(const A3DVECTOR3& vStart, float fRadius, float fHeight);

	static bool RetrievePlane(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float fRadius, float fHeight, A3DVECTOR3& vHitPos);

	bool IsInAir() const { return m_state == INAIR; }

	void SetCurVel(const A3DVECTOR3& vel){ m_CDRInfo.vClipVel = vel; }

	void UpdateDynDes();

	void SetDir(const A3DVECTOR3& dir, bool bUpdateBindDir = true);
	A3DVECTOR3 GetDir() const { return m_vDir; }

protected:
	void ResetCDRInfo(bool bClearVel);
	void TryToBind();//�붯̬�����
	void FollowBindObj();//����������ƶ�����������ת��

protected:
	CCapsule m_capsule;
	CDR_INFO   m_CDRInfo;
	float m_speed;
	State m_state;

	A3DVECTOR3 m_vDir;

	CBrushesObj* m_pBindObj;//��ʱ�����objͬ���˶�
	A3DMATRIX4 m_BindMatInv;
	A3DVECTOR3 m_BindMatPos;//�ڰ󶨵�obj����ϵ�µ�λ��
	A3DVECTOR3 m_BindMatDir;//�ڰ󶨵�obj����ϵ�µĳ���
	
	struct InDynamic{//�ڶ�̬��������ϵ�µ�����������ʵ���ƶ�Ч��
		A3DMATRIX4 invmat;
		A3DVECTOR3 vP;
	};
	abase::vector<InDynamic> m_DynDes;//Ƕ�붯̬����ǰһ֡�ڶ�̬���������µ�����
};

#endif // !defined(AFX_BRUSHCHARCTRLER_H__D0277489_B173_41CC_8B5A_21560F925C8D__INCLUDED_)
