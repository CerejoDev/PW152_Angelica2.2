/*
 * BrushcharCtrler.cpp: implementation of the BrushcharCtrler class.
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

#include "stdafx.h"
#include "BrushcharCtrler.h"
#include "BrushesObj.h"
#include "Collision.h"
#include "BrushesObjMgr.h"
#include <A3DTerrain2.h>
#include "EngineInterface.h"

using namespace CHBasedCD;

BrushcharCtrler::BrushcharCtrler(const BrushcharCtrlerDes& brushccDes): m_speed(brushccDes.fSpeed)
{
	m_capsule.SetRadius(brushccDes.fCapsuleRadius);
	m_capsule.SetHeight(brushccDes.fCylinerHei);
	m_capsule.SetUp(A3DVECTOR3(0,1,0));
	m_state = ONGROUND;

	m_pBindObj = NULL;

	CBrushesObjMgr::GetInstance()->RegisterDynObjReceiver(this);

	//dist to ground (invalidate value)
	m_fDistToGround = -1;

	//init cdr
	ResetCDRInfo(true);
	m_CDRInfo.fCapsuleR = m_capsule.GetRadius();
	m_CDRInfo.fCapsuleHei = m_capsule.GetHeight();
	m_CDRInfo.fCapsuleCenterHei = m_capsule.GetCenterHei();
	m_CDRInfo.vCapsuleUp = m_capsule.GetUp();
	m_CDRInfo.fSlopeThresh = brushccDes.fSlopeThresh;
	m_CDRInfo.fSlopMaxH = brushccDes.fSlopeMaxH;
	m_CDRInfo.fStepHeight = brushccDes.fStepHeight;
	m_CDRInfo.fMaxFallSpeed = brushccDes.fMaxFallSpeed;
	m_CDRInfo.fGravityAccel = brushccDes.fGravityAccel;
}

BrushcharCtrler::~BrushcharCtrler()
{
	CBrushesObjMgr::GetInstance()->UnRegisterDynObjReceiver(this);
}

void BrushcharCtrler::ResetCDRInfo(bool bClearVel)
{
	if (bClearVel)
	{
		m_CDRInfo.vClipVel.Clear();
		m_CDRInfo.vAbsVelocity.Clear();
	}
	
	m_CDRInfo.vTPNormal.Clear();
	m_CDRInfo.bBlocked	 = false;
	m_CDRInfo.bOnSurface = false;
	m_CDRInfo.bCanStay	 = false;
	m_CDRInfo.fCurRoll	= 0.0f;
	m_CDRInfo.dwCollideFlag = 0;
	m_CDRInfo.bShortSlop = false;
}

void BrushcharCtrler::FollowBindObj()//����������˶�
{
	if (0 == m_pBindObj)
		return;

	A3DMATRIX4 mat = m_pBindObj->GetTrans();
	A3DVECTOR3 vPos = m_BindMatPos * mat ;//
	A3DVECTOR3 vDelta(vPos - m_capsule.GetPos());
	if (vDelta.SquaredMagnitude() <= 1E-5f)
		return;

	ICollisionSupplier* pCS = GetCollisionSupplier();
	CBrushesObjMgr* pBOM = pCS->GetBrushObjMgr();
	pBOM->SkipMovingObjCollision(true);
	A3DVECTOR3 vNormal;
	env_trace_t env_trace;
	env_trace.vStart = m_capsule.GetPos();
	env_trace.vDelta = vDelta;
	env_trace.dwCheckFlag = CDR_BRUSH;
	env_trace.fCapsuleHei = m_capsule.GetHeight();
	env_trace.fCapsuleR = m_capsule.GetRadius();
	env_trace.vCapsuleUp = m_capsule.GetUp();
	
	if (pCS->CollideWithEnv(&env_trace))
		vDelta = vDelta * env_trace.fFraction;
	pBOM->SkipMovingObjCollision(false);
	
	vPos = m_capsule.GetPos() + vDelta;
	
	BrushCollideParam para;
	para.vStart = vPos;
	para.vDelta = A3DVECTOR3(0, -0.08f, 0);
	para.fCapsuleCenterHei = m_capsule.GetCenterHei();
	para.fCapsuleHei = m_capsule.GetHeight();
	para.fCapsuleR = m_capsule.GetRadius();
	A3DVECTOR3 vHitPos;
	if (m_CDRInfo.vClipVel.y < 0.5f && ::RetrievePlane(&para, vHitPos))
	{
		vPos = vHitPos;
		m_CDRInfo.vTPNormal = para.vHitNormal;
	}
	
	if (para.bStartSolid)
	{
		const abase::vector<CBrushesObj*>& _dynDes = pCS->GetBrushObjMgr()->GetAllDynamicObjs();

		for ( size_t i = 0; i < _dynDes.size(); i++)
		{
			if (m_pBindObj == _dynDes[i])
			{
				A3DMATRIX4 mat = m_DynDes[i].invmat;
				m_BindMatPos = m_DynDes[i].vP * mat;
				A3DVECTOR3 vPos = m_BindMatPos * m_pBindObj->GetTrans();
				m_capsule.SetPos(vPos);						
			}
		}
	}
	
	float height = 0;
	GetTerrainHeight(pCS->GetTerrain(), vPos, height);
	if (height > vPos.y - m_capsule.GetCenterHei())
	{
		vPos.y = height + m_capsule.GetCenterHei();
	}
	m_capsule.SetPos(vPos);
}

A3DVECTOR3 BrushcharCtrler::MoveCC(const float deltaSecond, const A3DVECTOR3& MoveDir)
{
#ifdef USING_BRUSH_MAN
	g_brushMan.Build(m_capsule.GetPos());
#endif

	//follow the binding object	
	if (m_pBindObj)
	{	
		A3DMATRIX4 mat = m_pBindObj->GetTrans();
		A3DVECTOR3 vPos = m_BindMatPos * mat ;//
		A3DVECTOR3 vDelta(vPos - m_capsule.GetPos());
		
		if (vDelta.SquaredMagnitude() > 1E-5f)
		{
			FollowBindObj();
			mat._41 = mat._42 = mat._43 = 0;
			m_vDir = m_BindMatDir * mat;
		}		
	}

	if (IsInAir() && m_CDRInfo.vClipVel.y > 0)
	{
		A3DVECTOR3 vTmp = MoveDir;
		vTmp.Normalize();
		vTmp *= m_speed;
		m_CDRInfo.vClipVel.x = vTmp.x;
		m_CDRInfo.vClipVel.z = vTmp.z;
		//m_CDRInfo.vVelDir.Clear();
	}
	//else
	m_CDRInfo.vVelDir = MoveDir;

	m_CDRInfo.t = deltaSecond;
	
	m_CDRInfo.vCenter			= m_capsule.GetPos();
	m_CDRInfo.fSpeed			= m_speed;
	
	A3DVECTOR3 vLastVel = m_CDRInfo.vClipVel;

	OnGroundMove(m_CDRInfo);

	if (vLastVel.y > 0 && vLastVel.y < m_CDRInfo.vClipVel.y)
		m_CDRInfo.vClipVel.y = vLastVel.y - m_CDRInfo.fGravityAccel * m_CDRInfo.t;

	if (!m_CDRInfo.vTPNormal.IsZero())
	{
		m_state = ONGROUND;
		m_CDRInfo.vClipVel.y = 0;
	}
	else
		m_state = INAIR;
	
	A3DVECTOR3 DistDelta(m_CDRInfo.vCenter - m_capsule.GetPos());
	m_capsule.SetPos(m_CDRInfo.vCenter);

	ICollisionSupplier* pCS = GetCollisionSupplier();
	CBrushesObjMgr* pBOM = pCS->GetBrushObjMgr();
	if (!pBOM->HasDynamicObjs())
		return m_CDRInfo.vCenter;

	if (m_CDRInfo.pUser != 0 && !MoveDir.IsZero())//sinked in
	{
		abase::vector<CConvexBrush*> SinkedBrushes;
		abase::vector<DWORD> flags;
		while (m_CDRInfo.pUser != 0)
		{				
			CConvexBrush* pBrush = (CConvexBrush*)(m_CDRInfo.pUser);
			SinkedBrushes.push_back(pBrush);
			flags.push_back(pBrush->GetCollisionFlag());
			pBrush->SetCollisionFlag(CHDATA_SKIP_COLLISION);
			OnGroundMove(m_CDRInfo);
		}
		//
		m_capsule.SetPos(m_CDRInfo.vCenter);
		for (size_t i = 0; i < SinkedBrushes.size(); i++)
		{
			SinkedBrushes[i]->SetCollisionFlag(flags[i]);
		}
	}
	
	if (m_state == ONGROUND)
		TryToBind();
	if (m_pBindObj && !DistDelta.IsZero())
	{
		A3DMATRIX4 mat = m_pBindObj->GetTrans();
		A3DVECTOR3 pos = m_capsule.GetPos();
		mat.InverseTM();
		m_BindMatPos = pos * mat;
		mat._41 = mat._42 = mat._43 = 0;
		m_BindMatDir = m_vDir * mat;
	}
	
	if (!DistDelta.IsZero() && m_DynDes.size() > 0)
	{
		//������Զ�̬�����λ��
		const abase::vector<CBrushesObj*>& _dynDes = pBOM->GetAllDynamicObjs();
		
		for (size_t j = 0; j < _dynDes.size(); ++j)
		{
			m_DynDes[j].invmat = _dynDes[j]->GetTrans();
			m_DynDes[j].invmat.InverseTM();
			m_DynDes[j].vP = m_capsule.GetPos();
		}
	}

	return m_CDRInfo.vCenter;
}

void BrushcharCtrler::JumpCC(float fJumpingSpeed, bool bIsAbsoluteSpeed)
{
	ResetCDRInfo(true);
	if (bIsAbsoluteSpeed)
		m_CDRInfo.vClipVel.y = fJumpingSpeed;
	else
		m_CDRInfo.vClipVel.y += fJumpingSpeed;
	m_state = INAIR;
}

bool BrushcharCtrler::IsPosAvailable(const A3DVECTOR3& vStart, float fRadius, float fHeight)
{
	float h = 0.0f;
	GetTerrainHeight(GetCollisionSupplier()->GetTerrain(), vStart, h);
	BrushCollideParam para;
	para.vStart = vStart;
	para.vDelta = A3DVECTOR3(0, -1, 0);
	para.fCapsuleCenterHei = fRadius + fHeight / 2;
	para.fCapsuleHei = fHeight;
	para.fCapsuleR = fRadius;
	para.vCapsuleUp = A3DVECTOR3(0, 1, 0);
	if (h > vStart.y - para.fCapsuleCenterHei)
		return false;

	A3DVECTOR3 vHitPos;
	::RetrievePlane(&para, vHitPos);

	return !para.bStartSolid;
}

bool BrushcharCtrler::RetrievePlane(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, float fRadius, float fHeight, A3DVECTOR3& vHitPos)
{
	BrushCollideParam para;
	para.vStart = vStart;
	para.vDelta = vDelta;
	para.fCapsuleCenterHei = fRadius + fHeight / 2;
	para.fCapsuleHei = fHeight;
	para.fCapsuleR = fRadius;
	para.vCapsuleUp = A3DVECTOR3(0, 1, 0);

	return (::RetrievePlane(&para, vHitPos));
}

void BrushcharCtrler::UpdateDynDes()
{
	ICollisionSupplier* pCS = GetCollisionSupplier();
	CBrushesObjMgr* pBOM = pCS->GetBrushObjMgr();
	const abase::vector<CBrushesObj*>& _dynDes = pBOM->GetAllDynamicObjs();

	if (_dynDes.size() == 0)
		return;

	if (_dynDes.size() != m_DynDes.size())
	{
		//��¼��Զ�̬�����λ��
		m_DynDes.clear();
		InDynamic indyn;
		for ( size_t i = 0; i < _dynDes.size(); i++)
		{
			indyn.invmat = _dynDes[i]->GetTrans();
			indyn.invmat.InverseTM();
			indyn.vP = m_capsule.GetPos();
			indyn.pBrushObj = _dynDes[i];
			m_DynDes.push_back(indyn);
		}
	}
	else
	{
		//����̬�����ƶ���ֻ���Ǳ���һ�������������ƶ�
		for ( size_t i = 0; i < _dynDes.size(); i++)
		{
			A3DVECTOR3 bindPos = m_DynDes[i].vP * m_DynDes[i].invmat;
			A3DVECTOR3 pos = bindPos * _dynDes[i]->GetTrans(); //m_capsule.GetPos();
			A3DVECTOR3 tmp = pos - m_capsule.GetPos(); //����һ�����������ƶ�ʱ������һ���ļ��ȷ��������ȷ
			tmp.Normalize();
			pos += 0.001f * tmp;
			
			CapusuleBrushTraceInfo info;
			info.Init(pos, m_capsule.GetPos() - pos, m_capsule.GetRadius() + 0.001f, m_capsule.GetHeight() + 0.001f, //��������任��������������΢�Ӵ�
				_dynDes[i]->GetTrans(), m_capsule.GetUp(), _dynDes[i]->GetScale());
			if (_dynDes[i]->CapsuleTrace(&info))//trace ��̬����
			{
				float fFraction1 = info.fFraction, fFraction2 = 1;
				pBOM->SkipMovingObjCollision(true);
				//Trace��̬����
				env_trace_t env_trace;
				env_trace.vStart = m_capsule.GetPos();
				env_trace.vDelta = pos - m_capsule.GetPos();
				env_trace.dwCheckFlag = CDR_BRUSH;
				env_trace.fCapsuleHei = m_capsule.GetHeight() + 0.001f;
				env_trace.fCapsuleR = m_capsule.GetRadius() + 0.001f;
				env_trace.vCapsuleUp = m_capsule.GetUp();
				
				if (pCS->CollideWithEnv(&env_trace))
				{
					fFraction2 = env_trace.fFraction;
				}
				
				fFraction2 = 1 - fFraction2;
				if (fFraction2 > fFraction1)
					fFraction1 = fFraction2;
				A3DVECTOR3 vDelta = (m_capsule.GetPos() - pos) * fFraction1;
				pBOM->SkipMovingObjCollision(false);
				A3DVECTOR3 vPos = pos + vDelta;//ȷ����������λ��
				float height = 0.0f;
				GetTerrainHeight(pCS->GetTerrain(), vPos, height);
				if (height > vPos.y - m_capsule.GetCenterHei())
				{
					vPos.y = height + m_capsule.GetCenterHei();
				}
				m_capsule.SetPos(vPos);
				//���½�����Զ�̬�����λ��
				for (size_t j = 0; j < _dynDes.size(); j++)
				{
					m_DynDes[j].invmat = _dynDes[j]->GetTrans();
					m_DynDes[j].invmat.InverseTM();
					m_DynDes[j].vP = m_capsule.GetPos();
				}
				return;
			}
			else
			{
				m_DynDes[i].invmat = _dynDes[i]->GetTrans();
				m_DynDes[i].invmat.InverseTM();
				m_DynDes[i].vP = m_capsule.GetPos();
			}							
		}
	}
}

void BrushcharCtrler::TryToBind()
{
	const abase::vector<CBrushesObj*>& _dynDes = GetCollisionSupplier()->GetBrushObjMgr()->GetAllDynamicObjs();
	if (_dynDes.size() == 0)
		return;

	A3DVECTOR3 vDelta(0, -0.5f, 0);
	float fFraction = 100;
	int iIndex = -1;
	for (int i = 0; i < (int)_dynDes.size(); i++)
	{
		CapusuleBrushTraceInfo info;
		info.Init(m_capsule.GetPos(), vDelta, m_capsule.GetRadius(), m_capsule.GetHeight(), 
				_dynDes[i]->GetTrans(), m_capsule.GetUp(), _dynDes[i]->GetScale());
		if (_dynDes[i]->CapsuleTrace(&info) && info.fFraction < fFraction)
		{
			//assert(!info.bStartSolid);
			fFraction = info.fFraction;
			iIndex = i;
		}
	}
	if (iIndex != -1)
	{
		if (m_pBindObj != _dynDes[iIndex])
		{
			m_pBindObj = _dynDes[iIndex];
			A3DMATRIX4 mat = m_pBindObj->GetTrans();
			A3DVECTOR3 pos = m_capsule.GetPos();
			mat.InverseTM();
			m_BindMatPos = pos * mat;
			mat._41 = mat._42 = mat._43 = 0;
			m_BindMatDir = m_vDir * mat;
		}		
	}
	else
	{
		m_pBindObj = NULL;
	}
}

void BrushcharCtrler::SetDir(const A3DVECTOR3& dir, bool bUpdateBindDir)
{
	 m_vDir = dir;
	 if (bUpdateBindDir && m_pBindObj)
	 {
		 A3DMATRIX4 mat(m_pBindObj->GetTrans());
		 mat.InverseTM();
		 mat._41 = mat._42 = mat._43 = 0;
		 m_BindMatDir = dir * mat;
	 }
}

bool BrushcharCtrler::ChangeCapsuleVolume(float fRadius, float fCylinderHei)
{
	A3DVECTOR3 vPos = m_capsule.GetPos();
	vPos.y += fRadius + fCylinderHei / 2 - m_capsule.GetCenterHei();
	if (IsPosAvailable(vPos, fRadius, fCylinderHei))
	{
		m_capsule.SetPos(vPos);
		return true;
	}
	return false;
}

void BrushcharCtrler::OnRegisterDynBrushObj(CBrushesObj* pObj) 
{
	InDynamic info;
	info.pBrushObj = pObj;
	info.invmat = pObj->GetTrans();
	info.invmat.InverseTM();
	info.vP = m_capsule.GetPos();

	m_DynDes.push_back(info);
}
void BrushcharCtrler::OnUnRegisterDynBrushObj(CBrushesObj* pObj) 
{
	for (size_t i = 0; i < m_DynDes.size(); i++)
	{
		if (m_DynDes[i].pBrushObj == pObj)
			m_DynDes.erase(m_DynDes.begin() + i);
	}
}

float BrushcharCtrler::GetDistToGround() const
{
	float dist = 10;
	env_trace_t env;
	env.dwCheckFlag = CDR_TERRAIN | CDR_BRUSH;
	env.fCapsuleHei = m_capsule.GetHeight();
	env.fCapsuleR = m_capsule.GetRadius();
	env.vStart = m_capsule.GetPos();
	env.vDelta = A3DVECTOR3(0, -dist, 0);
	env.vTerStart = env.vStart;
	env.vTerStart.y -= m_capsule.GetCenterHei();
	if (GetCollisionSupplier()->CollideWithEnv(&env))
		return env.fFraction * dist;
	return dist;
}

A3DVECTOR3 BrushcharCtrler::GetVelocity() const
{	
	if (m_CDRInfo.bBlocked)
		return A3DVECTOR3(0, 0, 0);
	return (m_CDRInfo.vAbsVelocity);
}