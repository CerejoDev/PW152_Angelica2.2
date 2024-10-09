/*
 * FILE: CameraController.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/03
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include <windows.h>
#include <zmouse.h>
#include <cmath>
#include "CameraController.h"

const A3DVECTOR3 CCameraController::g_vUp(0, 1, 0);
const float CCameraController::g_DefFactor = 9;
const float CCameraController::g_MinFactor = 1;
const float CCameraController::g_MaxFactor = 30;

CCameraController::CCameraController(A3DCamera& InitedCamera) : m_Camera(InitedCamera)
{
	assert(g_DefFactor >= g_MinFactor);
	assert(g_DefFactor <= g_MaxFactor);

	m_bEnableMove = true;
	m_CenterX = 0;
	m_CenterY = 0;
	m_Mode = MODE_FREE_FLY;
	m_fRadius = g_DefFactor;
	m_vFocusPos.Set(0, 0, 0);
}

void CCameraController::SetFocusPosition(const A3DVECTOR3& pos)
{
	m_vFocusPos = pos + A3DVECTOR3(0, 0.8f, 0);
}

A3DVECTOR3& CCameraController::GetPosGroundCull(A3DVECTOR3& Pos, A3DTerrain2& Terrain) const
{
	float fHeight = Terrain.GetPosHeight(Pos);
	a_ClampFloor(Pos.y, fHeight + 1);
	return Pos;
}

void CCameraController::AddTerrainHeigt(A3DTerrain2& Terrain)
{
	A3DVECTOR3 vPos = m_Camera.GetPos();
	vPos.y += Terrain.GetPosHeight(vPos);
	m_Camera.SetPos(vPos);
}

bool CCameraController::EntryDefBindingMode(A3DVECTOR3& outFocusPos)
{
	m_Mode = MODE_BINDING;
	m_camPosBack = m_Camera.GetPos();
	m_camDirBack = m_Camera.GetDir();

	m_fRadius = g_DefFactor;
	outFocusPos = m_Camera.GetPos() + m_Camera.GetDir() * m_fRadius;
	m_vFocusPos = outFocusPos;
	return true;
}

bool CCameraController::EntryDefBindingMode(A3DVECTOR3& outFocusPos, A3DTerrain2& Terrain, A3DViewport* pVP)
{
	assert(m_CenterX > 0);
	assert(m_CenterY > 0);
	if (0 >= m_CenterX) return false;
	if (0 >= m_CenterY) return false;

	// make sure camera dir is looking at ground direction
	A3DVECTOR3 cameraDir = m_Camera.GetDir();
	if (0 < cameraDir.y)
	{
		cameraDir.y = -cameraDir.y;
		a_ClampRoof(cameraDir.y, -0.1f);
		cameraDir.Normalize();
	}

	if (0 == pVP)
	{
		m_camPosBack = m_Camera.GetPos();
		m_camDirBack = m_Camera.GetDir();

		outFocusPos = m_camPosBack + m_camDirBack * 10;
		outFocusPos.y = Terrain.GetPosHeight(outFocusPos);
	}
	else
	{
		A3DVECTOR3 cameraPos = m_Camera.GetPos();
		A3DVECTOR3 ZInfinite(m_CenterX, m_CenterY, 1);
		pVP->InvTransform(ZInfinite, ZInfinite);
		A3DVECTOR3 vDelta = ZInfinite - cameraPos;

		RAYTRACERT rt;
		if (!Terrain.RayTrace(cameraPos, vDelta, 1, &rt))
			return false;

		m_camPosBack = m_Camera.GetPos();
		m_camDirBack = m_Camera.GetDir();
		outFocusPos = rt.vHitPos;
	}

	m_Mode = MODE_BINDING;
	m_fRadius = g_DefFactor;
	SetFocusPosition(outFocusPos);
	m_Camera.SetDirAndUp(cameraDir, g_vUp);
	A3DVECTOR3 cameraPos = CalculateCameraPosition(Terrain);
	m_Camera.SetPos(cameraPos);
	return true;
}

void CCameraController::EntryBindingMode(const A3DVECTOR3& BingPos, A3DTerrain2* pTerrain, const bool IsKeepDir)
{
	m_Mode = MODE_BINDING;
	SetFocusPosition(BingPos);

	A3DVECTOR3 cameraDir = m_Camera.GetDir();
	if (!IsKeepDir)
	{
		cameraDir = m_vFocusPos - m_Camera.GetPos();
		cameraDir.Normalize();
		m_Camera.SetDirAndUp(cameraDir, g_vUp);
	}

	A3DVECTOR3 cameraPos;
	if (0 == pTerrain)
		cameraPos = m_vFocusPos - cameraDir * m_fRadius;
	else
		cameraPos = CalculateCameraPosition(*pTerrain);

	m_Camera.SetPos(cameraPos);
}

void CCameraController::EntryFreeFlyMode(const bool bRestorePose)
{
	m_Mode = MODE_FREE_FLY;
	if (bRestorePose)
	{
		m_Camera.SetPos(m_camPosBack);
		m_Camera.SetDirAndUp(m_camDirBack, g_vUp);
	}
	else
	{
		m_camPosBack = m_Camera.GetPos();
		m_camDirBack = m_Camera.GetDir();
	}
}

void CCameraController::UpdateFocusFactor(const int zDelta, A3DTerrain2* pTerrain)
{
	const int nDelta = zDelta / WHEEL_DELTA;
	m_fRadius -= nDelta;
	if (1 > m_fRadius)
	{
		m_fRadius = 1;
		return;
	}
	if (30 < m_fRadius)
	{
		m_fRadius = 30;
		return;
	}

	A3DVECTOR3 cameraPos;
	if (0 == pTerrain)
		cameraPos = m_Camera.GetPos() + m_Camera.GetDir() * nDelta;
	else
		cameraPos = CalculateCameraPosition(*pTerrain);

	m_Camera.SetPos(cameraPos);
}

void CCameraController::MoveCamera(float deltaTimeSec, A3DTerrain2* pTerrain, const A3DVECTOR3& DeltaMove)
{
	if (!m_bEnableMove)
		return;

	if (QueryMode(MODE_FREE_FLY))
	{
		MoveCameraFreeFly(deltaTimeSec, pTerrain);
		return;
	}
	
	MoveCameraBinding(DeltaMove, pTerrain);
}

void CCameraController::MoveCameraFreeFly(float deltaTimeSec, A3DTerrain2* pTerrain)
{
	A3DVECTOR3 vDirH = m_Camera.GetDir();
	A3DVECTOR3 vRightH = m_Camera.GetRight();
	A3DVECTOR3 vUpH = m_Camera.GetUp();

	A3DVECTOR3 vDelta(0);

	if (GetAsyncKeyState('W') & 0x8000)
		vDelta = vDirH;
	else if (GetAsyncKeyState('S') & 0x8000)
		vDelta = -vDirH;

	if (GetAsyncKeyState('A') & 0x8000)
		vDelta = vDelta - vRightH;
	else if (GetAsyncKeyState('D') & 0x8000)
		vDelta = vDelta + vRightH;

	if (GetAsyncKeyState('Q') & 0x8000)
		vDelta = vDelta + vUpH;
	else if (GetAsyncKeyState('Z') & 0x8000)
		vDelta = vDelta - vUpH;

	float fSpeed = 20.0f;
	if (GetAsyncKeyState(VK_SHIFT))
		fSpeed *= 2.5f;

	vDelta = a3d_Normalize(vDelta) * (fSpeed * deltaTimeSec);
	m_Camera.Move(vDelta);

	if (0 != pTerrain)
	{
		A3DVECTOR3 vPos = m_Camera.GetPos();
		m_Camera.SetPos(GetPosGroundCull(vPos, *pTerrain));
	}
}

void CCameraController::MoveCameraBinding(const A3DVECTOR3& DeltaMove, A3DTerrain2* pTerrain)
{
	if (DeltaMove.IsZero())
		return;

	m_vFocusPos += DeltaMove;

	A3DVECTOR3 cameraPos;
	if (0 == pTerrain)
		cameraPos = m_Camera.GetPos() + DeltaMove;
	else
		cameraPos = CalculateCameraPosition(*pTerrain);

	m_Camera.SetPos(cameraPos);
}

void CCameraController::RotateCamera(const int dx, const int dy, A3DTerrain2* pTerrain)
{
	if (QueryMode(MODE_FREE_FLY))
	{
		RotateCameraFreeFly(dx, dy);
		return;
	}
	
	RotateCameraBinding(dx, dy, pTerrain);
}

void CCameraController::RotateCameraFreeFly(const int dx, const int dy)
{
	const float f1 = 0.03f;
	if (0 != dx)
		m_Camera.DegDelta(dx * f1);
	
	if (0 != dy)
		m_Camera.PitchDelta(-dy * f1);
}

bool CCameraController::HasVerticalRotation(const int dy, const A3DVECTOR3& vDir)
{
	static const float Epsilon = 0.001f;

	if (0 == dy)
		return false;

	if ((Epsilon > (1 + vDir.y)) && (0 > dy))
	{
		if (Epsilon > abs(vDir.x))
			return false;
	}

	if ((Epsilon > (1 - vDir.y)) && (0 < dy))
	{
		if (Epsilon > abs(vDir.x))
			return false;
	}

	return true;
}

void CCameraController::RotateCameraBinding(const int dx, const int dy, A3DTerrain2* pTerrain)
{
	const float f1 = 0.01f;

	bool bIsChanged = false;
	A3DVECTOR3 cameraPos = m_Camera.GetPos();
	A3DVECTOR3 cameraDir = m_Camera.GetDir();

	if (0 != dx)
	{
		bIsChanged = true;
		cameraPos = a3d_RotatePosAroundLine(cameraPos, m_vFocusPos, g_vUp, dx * f1);
	}

	if (HasVerticalRotation(dy, cameraDir))
	{
		bIsChanged = true;
		A3DVECTOR3 axis = CrossProduct(g_vUp, cameraDir);
		cameraPos = a3d_RotatePosAroundLine(cameraPos, m_vFocusPos, axis, -dy * f1); 
	}

	if (!bIsChanged)
		return;

	cameraDir = m_vFocusPos - cameraPos;
	m_Camera.SetDirAndUp(cameraDir, g_vUp);
	
	if (0 != pTerrain)
		cameraPos = CalculateCameraPosition(*pTerrain);
	m_Camera.SetPos(cameraPos);
}

A3DVECTOR3 CCameraController::CalculateCameraPosition(A3DTerrain2& Terrain) const
{
	A3DVECTOR3 vDir = m_Camera.GetDir() * -1;

	float fovY, front, back, ratio; 
	m_Camera.GetProjectionParam(&fovY, &front, &back, &ratio);
	float yHalfSide = front * tan(fovY * 0.5);
	float xHalfSide = yHalfSide * ratio;

	// calculate the left-bottom and right-bottom position...
	// in the near plane of viewing frustum
	A3DVECTOR3 xAxis = CrossProduct(g_vUp, vDir);
	xAxis.Normalize();
	A3DVECTOR3 vBottomMiddle = m_vFocusPos + m_Camera.GetUp() * -1 * yHalfSide;
	A3DVECTOR3 vBottomLeft  = vBottomMiddle - xAxis * xHalfSide;
	A3DVECTOR3 vBottomRight = vBottomMiddle + xAxis * xHalfSide;

	// do the raycast to get the closest information
	RAYTRACERT* pRT = 0;
	RAYTRACERT rtLeft;
	RAYTRACERT rtRight;
	bool bLeft  = Terrain.RayTrace(vBottomLeft, vDir, m_fRadius, &rtLeft);
	bool bRight = Terrain.RayTrace(vBottomRight, vDir, m_fRadius, &rtRight);

	if (!bLeft && !bRight)
		return m_vFocusPos + vDir * (m_fRadius + front);

	if (!bLeft && bRight)
		pRT = &rtRight;
	else if (bLeft && !bRight)
		pRT = &rtLeft;
	else
		pRT = (rtLeft.fFraction < rtRight.fFraction)? &rtLeft : &rtRight;

	// adjust the result according to the angle
	float theta = DEG2RAD(90) - acos(DotProduct(m_Camera.GetDir(), pRT->vNormal));
	float disH = yHalfSide / tan(theta);

	A3DVECTOR3 vDis = vDir * (m_fRadius * pRT->fFraction - disH);
	return m_vFocusPos + vDis;
}