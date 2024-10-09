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
	float fCapsuleRadius;//胶囊的半径
	float fCylinerHei;//胶囊圆柱部分高度
	float fSlopeThresh;//可爬上的最陡斜面
	float fStepHeight;//抬腿高度
	float fMaxFallSpeed;//最大下落速度
	float fSlopeMaxH;//可以爬的最长短陡坡
	float fSpeed;//移动速度
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

	//修改Capsule的大小，成功返回TRUE，失败返回FALSE
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
	void TryToBind();//与动态物体绑定
	void FollowBindObj();//跟随绑定物体移动（不包括旋转）

protected:
	CCapsule m_capsule;
	CDR_INFO   m_CDRInfo;
	float m_speed;
	State m_state;

	A3DVECTOR3 m_vDir;

	CBrushesObj* m_pBindObj;//绑定时将与该obj同步运动
	A3DMATRIX4 m_BindMatInv;
	A3DVECTOR3 m_BindMatPos;//在绑定的obj坐标系下的位置
	A3DVECTOR3 m_BindMatDir;//在绑定的obj坐标系下的朝向
	
	struct InDynamic{//在动态物体坐标系下的描述，用来实现推动效果
		A3DMATRIX4 invmat;
		A3DVECTOR3 vP;
	};
	abase::vector<InDynamic> m_DynDes;//嵌入动态物体前一帧在动态物体坐标下的描述
};

#endif // !defined(AFX_BRUSHCHARCTRLER_H__D0277489_B173_41CC_8B5A_21560F925C8D__INCLUDED_)
