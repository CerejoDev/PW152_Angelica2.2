/*
 * ObjAttr.h: brushes object attribute for movement
 *
 * DESCRIPTION: object attribute for movement
 *
 * CREATED BY: YangXiao, 2010/12/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJATTR_H__D0277489_B173_41CC_8B5A_21560F925C8D__INCLUDED_)
#define AFX_OBJATTR_H__D0277489_B173_41CC_8B5A_21560F925C8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <A3DVector.h>

class NxStream;

class BrushMove
{
public:
	virtual bool TickMove(float dtSec, A3DVECTOR3& vPos, A3DVECTOR3& vDir) = 0;

	BrushMove(){}
	virtual ~BrushMove(){}
};
class LineBrushMove: public BrushMove
{
protected:
	A3DVECTOR3 vStart;
	A3DVECTOR3 vEnd;
	A3DVECTOR3 vSpeed;
public:
	LineBrushMove(const A3DVECTOR3& start, const A3DVECTOR3& end, float speed);
	void Reset();

	virtual ~LineBrushMove();

	virtual bool TickMove(float dtSec, A3DVECTOR3& vPos, A3DVECTOR3& vDir);
};
class LineBrushMoveWithRot: public LineBrushMove
{
protected:
	float fRotSpeed;//自转角速度 rad/s
public:
	//input RotSpeed: degree per second 
	LineBrushMoveWithRot(const A3DVECTOR3& start, const A3DVECTOR3& end, float speed, float RotSpeed);
	virtual ~LineBrushMoveWithRot();
	virtual bool TickMove(float dtSec, A3DVECTOR3& vPos, A3DVECTOR3& vDir);
};
class CircleBrushMove: public BrushMove
{
protected:
	A3DVECTOR3 vCenter;
	A3DVECTOR3 vAxis;//rot axis, only Y-Axis now
	float fSpeed;//rad per second
public:
	//input speed: degree per second
	CircleBrushMove(const A3DVECTOR3& center, float speed, const A3DVECTOR3& axis = A3DVECTOR3(0, 1, 0));
	virtual ~CircleBrushMove();

	virtual bool TickMove(float dtSec, A3DVECTOR3& vPos, A3DVECTOR3& vDir);
};

class BrushObjAttr 
{
	

public:
	BrushObjAttr(const BrushObjAttr& attr);
	BrushObjAttr& operator=(const BrushObjAttr& attr);

	enum TYPE{ NONE = 0, LINEAR, LINEARWITHROT, CIRCLE };
	BrushObjAttr();
	virtual ~BrushObjAttr();

	void Update(TYPE type);

	TYPE GetType() const { return m_type; }

	bool TickMove(float dtSec, A3DVECTOR3& vPos, A3DVECTOR3& vDir);

	bool Serialize(NxStream& nxStream);
	bool DeSerialize(const NxStream& nxStream);

	//////////////////////////////////////////////////////////////////////////
	//attribute
	A3DVECTOR3 vStart;
	A3DVECTOR3 vEnd;
	float fLineSpeed;

	float fRotSpeedSelf;//自转角速度°/s
	float fRotSpeed;//公转角速度°/s
	A3DVECTOR3 vCenter;//公转中心

protected:
	BrushMove *m_pBrushMove;
	TYPE m_type;
};

#endif // !defined(AFX_OBJATTR_H__D0277489_B173_41CC_8B5A_21560F925C8D__INCLUDED_)
