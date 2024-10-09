/*
 * ObjAttr.cpp: implementation of the BrushObjAttr class.
 *
 * DESCRIPTION: attribute of brushes object
 *
 * CREATED BY: YangXiao, 2010/12/31
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */
//////////////////////////////////////////////////////////////////////

#include "BrushObjAttr.h"
#include <A3DMatrix.h>
#include <NxSimpleTypes.h>
#include <NxStream.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LineBrushMove::LineBrushMove(const A3DVECTOR3& start, const A3DVECTOR3& end, float speed)
{
	vStart = start;
	vEnd = end;

	A3DVECTOR3 dir(vEnd - vStart);
	dir.Normalize();
	vSpeed = (float)fabs(speed) * dir;
}
LineBrushMove::~LineBrushMove()
{

}
void LineBrushMove::Reset()
{
	if (DotProduct(vEnd - vStart, vSpeed) < 0)
		vSpeed = -vSpeed;
}

bool LineBrushMove::TickMove(float dtSec, A3DVECTOR3& vPos, A3DVECTOR3& vDir)
{
	A3DVECTOR3 step(vSpeed * dtSec);
	A3DVECTOR3 vTarget = DotProduct(vEnd - vStart, vSpeed) > 0 ? vEnd : vStart;
	vPos += step;
	float dpt = DotProduct(vTarget - vPos, vSpeed);
	if (dpt < 0)
	{
		vSpeed = -vSpeed;
	}
	
	return true;
}

//speed 
CircleBrushMove::CircleBrushMove(const A3DVECTOR3& center, float speed, const A3DVECTOR3& axis /* = A3DVECTOR3 */)
{
	vCenter = center;
	const float pi = 3.1415926f;
	fSpeed = speed*pi / 180;
	vAxis = axis;
}
CircleBrushMove::~CircleBrushMove()
{

}
bool CircleBrushMove::TickMove(float dtSec, A3DVECTOR3& vPos, A3DVECTOR3& vDir)
{
	A3DMATRIX4 mat;
	mat.RotateAxis(vCenter, vAxis, dtSec * fSpeed);
	vPos = vPos * mat;
	mat._41 = mat._42 = mat._43 = 0;
	vDir = vDir * mat;

	return true;
}

LineBrushMoveWithRot::LineBrushMoveWithRot(const A3DVECTOR3& start, const A3DVECTOR3& end, float speed, float RotSpeed)
:LineBrushMove(start, end, speed)
{
	const float pi = 3.1415926f;
	fRotSpeed = RotSpeed * pi / 180;
}
LineBrushMoveWithRot::~LineBrushMoveWithRot()
{

}
bool LineBrushMoveWithRot::TickMove(float dtSec, A3DVECTOR3& vPos, A3DVECTOR3& vDir)
{
	LineBrushMove::TickMove(dtSec, vPos, vDir);
	A3DMATRIX4 mat;
	mat.RotateY(dtSec * fRotSpeed);
	vDir = vDir * mat;
	return true;
}

BrushObjAttr::BrushObjAttr()
{
	m_pBrushMove = NULL;
	m_type = NONE;

	//attribute init
	vStart.Clear();
	vEnd.Clear();
	fLineSpeed = 0;
	fRotSpeed = 0;
	fRotSpeedSelf = 0;
	vCenter.Clear();
}

BrushObjAttr::~BrushObjAttr()
{
	delete m_pBrushMove;
}

bool BrushObjAttr::TickMove(float dtSec, A3DVECTOR3& vPos, A3DVECTOR3& vDir)
{
	if (m_pBrushMove)
		m_pBrushMove->TickMove(dtSec, vPos, vDir);
	return true;
}

void BrushObjAttr::Update(TYPE type)
{
	delete m_pBrushMove;
	m_type = type;
	switch(type)
	{
	case LINEAR:
		m_pBrushMove = new LineBrushMove(vStart, vEnd, fLineSpeed);
		break;
	case LINEARWITHROT:
		m_pBrushMove = new LineBrushMoveWithRot(vStart, vEnd, fLineSpeed, fRotSpeedSelf);
		break;
	case CIRCLE:
		m_pBrushMove = new CircleBrushMove(vCenter, fRotSpeed);
		break;
	default:
		m_pBrushMove = NULL;
		break;
	}
}

bool BrushObjAttr::Serialize(NxStream& nxStream)
{
	nxStream.storeByte((NxU8)m_type);
	int vSize = sizeof(A3DVECTOR3);
	nxStream.storeBuffer(&vStart, vSize);
	nxStream.storeBuffer(&vEnd, vSize);
	nxStream.storeFloat(fLineSpeed);
	nxStream.storeFloat(fRotSpeedSelf);
	nxStream.storeFloat(fRotSpeed);
	nxStream.storeBuffer(&vCenter, vSize);
	return true;
}
bool BrushObjAttr::DeSerialize(const NxStream& nxStream)
{
	m_type = (TYPE)nxStream.readByte();
	int vSize = sizeof(A3DVECTOR3);
	nxStream.readBuffer(&vStart, vSize);
	nxStream.readBuffer(&vEnd, vSize);
	fLineSpeed = nxStream.readFloat();
	fRotSpeedSelf = nxStream.readFloat();
	fRotSpeed = nxStream.readFloat();
	nxStream.readBuffer(&vCenter, vSize);
	Update(m_type);
	return true;
}

BrushObjAttr::BrushObjAttr(const BrushObjAttr& attr)
{
	vStart = attr.vStart;
	vEnd = attr.vEnd;
	fLineSpeed = attr.fLineSpeed;

	fRotSpeed = attr.fRotSpeed;
	fRotSpeedSelf = attr.fRotSpeedSelf;
	vCenter = attr.vCenter;

	m_type = attr.m_type;
	m_pBrushMove = NULL;
	Update(m_type);
}
BrushObjAttr& BrushObjAttr::operator=(const BrushObjAttr& attr)
{
	if (this == &attr)
		return *this;

	vStart = attr.vStart;
	vEnd = attr.vEnd;
	fLineSpeed = attr.fLineSpeed;
	
	fRotSpeed = attr.fRotSpeed;
	fRotSpeedSelf = attr.fRotSpeedSelf;
	vCenter = attr.vCenter;
	
	m_type = attr.m_type;
	m_pBrushMove = NULL;
	Update(m_type);
	return *this;
}
