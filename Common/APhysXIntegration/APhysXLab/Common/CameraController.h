/*
 * FILE: CameraController.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/03
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_CAMERA_CONTROLLER_H_
#define _APHYSXLAB_CAMERA_CONTROLLER_H_

#include <A3D.h>

class CCameraController
{
public:
	enum Mode
	{
		MODE_FREE_FLY  = 0,
		MODE_BINDING   = 1,
	};

	CCameraController(A3DCamera& InitedCamera); 

public:
	const A3DVECTOR3& GetPos() const { return m_Camera.GetPos(); }
	A3DCamera& GetCamera() const { return m_Camera; }
	void SetScreenCenterPos(const int x, const int y) { m_CenterX = x; m_CenterY = y; }

	bool EntryDefBindingMode(A3DVECTOR3& outFocusPos);
	bool EntryDefBindingMode(A3DVECTOR3& outFocusPos, A3DTerrain2& Terrain, A3DViewport* pVP = 0);
	void EntryBindingMode(const A3DVECTOR3& BingPos, A3DTerrain2* pTerrain = 0, const bool IsKeepDir = false);
	void EntryFreeFlyMode(const bool bRestorePose = false);

	Mode GetMode() const { return m_Mode; };
	bool QueryMode(const Mode& mode) const { return mode == m_Mode; }

	void UpdateFocusFactor(const int zDelta, A3DTerrain2* pTerrain);
	void MoveCamera(float deltaTimeSec, A3DTerrain2* pTerrain = 0, const A3DVECTOR3& DeltaMove = A3DVECTOR3(0));
	void RotateCamera(const int dx, const int dy, A3DTerrain2* pTerrain = 0);

	void EnableMoveCamera(const bool bEnable) { m_bEnableMove = bEnable; }
	void AddTerrainHeigt(A3DTerrain2& Terrain);

private:
	void SetFocusPosition(const A3DVECTOR3& pos);
	A3DVECTOR3 CalculateCameraPosition(A3DTerrain2& Terrain) const;

	A3DVECTOR3& GetPosGroundCull(A3DVECTOR3& Pos, A3DTerrain2& Terrain) const;
	void MoveCameraBinding(const A3DVECTOR3& DeltaMove, A3DTerrain2* pTerrain = 0);
	void MoveCameraFreeFly(float deltaTimeSec, A3DTerrain2* pTerrain = 0);

	bool HasVerticalRotation(const int dy, const A3DVECTOR3& vDir);
	void RotateCameraBinding(const int dx, const int dy, A3DTerrain2* pTerrain = 0);
	void RotateCameraFreeFly(const int dx, const int dy);
	
private:
	// forbidden behavior
	CCameraController(const CCameraController& rhs);
	CCameraController& operator= (const CCameraController& rhs);

private:
	static const A3DVECTOR3 g_vUp;
	static const float g_DefFactor;
	static const float g_MinFactor;
	static const float g_MaxFactor;

	bool		m_bEnableMove;
	int			m_CenterX;
	int			m_CenterY;
	Mode        m_Mode;

	float		m_fRadius; // radius from focus position to camera
	A3DVECTOR3  m_vFocusPos;
	A3DCamera&	m_Camera;

	A3DVECTOR3  m_camPosBack;
	A3DVECTOR3  m_camDirBack;
};
#endif