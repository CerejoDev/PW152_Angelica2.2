 /*
 * FILE: ECAutonomousAgent.h
 *
 * DESCRIPTION: The basic class of autonomous agent in Angelica Engine
 *
 * CREATED BY: Changzhi Li, 2004/11/02
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once 

#include "ECAutoCaFuncs.h"
#include "A3DSkinModel.h"
#include <AString.h>

class A3DEngine;
class A3DTerrain2;
class A3DTerrainWater;
class ECAutonomousGroup;
class CECModel;
class A3DGfxEngine;

class ECAutonomousAgent
{
public:

	enum ActionList_e
	{
		ACTION_NONE,
		ACTION_FIRST,
		ACTION_SECOND,
		ACTION_NUM
	};

public:
	ECAutonomousAgent ();
	
	virtual ~ECAutonomousAgent () { }
	
	virtual A3DVECTOR3	GetSide (void) const					{ return m_vSide; }
	virtual void		SetSide (const A3DVECTOR3& vSide)		{ m_vSide = vSide; }
	virtual void		SetSide (float x, float y, float z)		{ m_vSide.Set(x,y,z); }

	A3DVECTOR3	GetUp (void) const						{ return m_vUp; }
	void		SetUp (const A3DVECTOR3& vUp)			{ m_vUp = vUp; }
	void		SetUp (float x, float y, float z)		{ m_vUp.Set(x,y,z); }

	A3DVECTOR3	GetForward (void) const					{ return m_vForward; }
	void		SetForward (const A3DVECTOR3& vForward)	{ m_vForward = vForward; }
	void		SetForward (float x, float y, float z)	{ m_vForward.Set (x,y,z); }

	A3DVECTOR3	GetPosition (void) const
	{ 
		return m_vPosition;
	}
	void		SetPosition (const A3DVECTOR3& vPos)	{ m_vPosition = vPos; }
	void		SetPosition (float x, float y, float z)	{ m_vPosition.Set(x,y,z); }
	
	float	GetRadius (void) const	{ return m_fRadius; }
	void	SetRadius (float r)		{ m_fRadius = r; }

	void	ResetLocalSpace (void);
	
	float	GetSpeed (void) const	{ return GetVelocity().Magnitude();}
	void	SetSpeed (float s)		{ SetVelocity(GetForward()*s); }
	
	A3DVECTOR3	GetVelocity (void) const				{ return m_vVelocity; }
	void		SetVelocity (const A3DVECTOR3& v)		{ m_vVelocity = v; }
	void		SetVelocity (float x, float y, float z)	{ m_vVelocity.Set(x, y, z); }

	A3DVECTOR3	GetAcceleration (void) const			{ return m_vAcceleration;}
	void		SetAcceleration (const A3DVECTOR3& a);
	void		SetAcceleration (float x, float y, float z);

	float	GetMaxAcceleration (void) const	{ return m_fAccelerationMax; }
	void	SetMaxAcceleration (float ma)	{ m_fAccelerationMax=ma; }
	
	float	GetMaxSpeed (void) const		{ return m_fSpeedMax; }
	void	SetMaxSpeed (float ms)			{ m_fSpeedMax = ms; }
	float	GetMinSpeed (void) const		{ return m_fSpeedMin; }
	void	SetMinSpeed (float ms)			{ m_fSpeedMin = ms; }

	float	GetBankAngle (void)	const		{ return m_fBankAngle; }
	void	SetBankAngle (float fAngle)		{ m_fBankAngle = fAngle; }
	float	GetMaxBankAngle (void) const	{ return m_fBankAngleMax; }
	void	SetMaxBankAngle (float fMaxbank){ m_fBankAngleMax = fMaxbank; }
	
	void	ApplyBreakingForce (float fBreakingRate);

	void	SetBreakingRate (float fBreakingRate) 
	{ m_fBreakingRate = ECP_AUTOCA::clip(fBreakingRate, 0, 1); }

	A3DVECTOR3	PredictPosition (DWORD dwDeltaTime) const;	// time in msec

	void SetActionName(int idx,char* pName)
	{
		ASSERT(idx>= ACTION_NONE && idx< ACTION_NUM);
		m_strActionName[idx] = pName;
	}

public:	// virtual functions

	virtual void		RegenerateLocalSpace (A3DVECTOR3 vNewForward, float fBankAngle);
	virtual A3DVECTOR3	SteerForWander (float fElapsedTime) { return A3DVECTOR3(0,0,0); }		// time in sec
	virtual A3DVECTOR3	SteerForSeek (const A3DVECTOR3& target) { return A3DVECTOR3(0,0,0); }
	virtual A3DVECTOR3	SteerForFlee (const A3DVECTOR3& target) { return A3DVECTOR3(0,0,0); }

	virtual bool		IsInBoidNeighborhood (const ECAutonomousAgent* other, float minDistance, float maxDistance, float cosMaxAngle);
	virtual A3DVECTOR3	SteerForSeparation (float maxDistance, float cosMaxAngle, const ECAutonomousGroup* pGroup) { return A3DVECTOR3(0,0,0); }
	virtual A3DVECTOR3	SteerForAlignment (float maxDistance, float cosMaxAngle, const ECAutonomousGroup* pGroup) { return A3DVECTOR3(0,0,0); }
	virtual A3DVECTOR3	SteerForCohesion (float maxDistance, float cosMaxAngle, const ECAutonomousGroup* pGroup) { return A3DVECTOR3(0,0,0); }

	// skin model wrapper
	virtual bool	Init (bool bUseECModel,A3DEngine *pEngine,A3DGfxEngine* pGfxEngine);
	void	Release ();
	void	SetLightInfo (const A3DSkinModelLight& Info);
	bool	Load (const char* szFile);
	

	bool Render(A3DViewport* pViewport, bool bCheckVis = TRUE);

	A3DSkinModel* GetSkinModel () const { return m_pSkinModel; }
	

	virtual void	Reset (void) {}
	virtual void	ApplySteeringForce (float fElapsedTime) {}	// time in sec
	virtual A3DVECTOR3 RespondToBoundary (const A3DVECTOR3& vCenter, float fBoundaryRadius) { return A3DVECTOR3(0,0,0); }
	virtual void	ClampPosition (A3DTerrain2 * pTerrain, A3DTerrainWater * pTerrainWater, DWORD dwDeltaTime) {}
	virtual void	Update (DWORD dwDeltaTime, ActionList_e action = ACTION_FIRST) {}		// time in msec

	
	bool IsAhead (const A3DVECTOR3& target) const	{ return IsAhead (target, 0.707f); }
	bool IsAside (const A3DVECTOR3& target) const	{ return IsAside (target, 0.707f); }
	bool IsBehind (const A3DVECTOR3& target) const	{ return IsBehind (target, -0.707f); }
	bool IsAhead (const A3DVECTOR3& target, float cosThreshold) const
	{
		A3DVECTOR3 targetDirection = (target - GetPosition ());
		targetDirection.Normalize();
		return DotProduct(GetForward(), targetDirection) > cosThreshold;
	}
	
	bool IsAside (const A3DVECTOR3& target, float cosThreshold) const
	{
		A3DVECTOR3 targetDirection = (target - GetPosition ());
		targetDirection.Normalize();
		const float dp = DotProduct(GetForward(), targetDirection);
		return (dp < cosThreshold) && (dp > -cosThreshold);
	}
	
	bool IsBehind (const A3DVECTOR3& target, float cosThreshold) const
	{
		A3DVECTOR3 targetDirection = (target - GetPosition());
		targetDirection.Normalize();
		return DotProduct(GetForward(), targetDirection) < cosThreshold;
	}
	
protected:

	A3DVECTOR3	m_vSide;				// side-pointing unit basis vector
	A3DVECTOR3	m_vUp;					// upward-pointing unit basis vector
	A3DVECTOR3	m_vForward;				// forward-pointing unit basis vector

	A3DVECTOR3	m_vPosition;			// origin of local space
	A3DVECTOR3	m_vVelocity;			// maybe velocity direction is not forward
	A3DVECTOR3	m_vAcceleration;		// acceleration = force / mass;

	float		m_fRadius;				// size of bounding sphere, for obstacle avoidance, etc.
	float		m_fAccelerationMax;		// the maximum steering force this vehicle can apply
										// (steering force is clipped to this magnitude)
	
	// velocity's magnitude (speed) is clipped to [m_fSpeedMin, m_fSpeedMax]
	float		m_fSpeedMax;			// the maximum speed this agent is allowed to move
	float		m_fSpeedMin;			// the minimum speed this agent is allowed to move

	float		m_fBankAngle;	// bank angle:倾斜角. Bank. The angle between the lateral axis(横轴) of an aeroplane and the horizontal plane. 
								// angle positive is that between positive side direction and horizontal plane
								// Banking is the angular motion about the longitudinal axis(纵轴) of an aeroplane when turning.

	float		m_fBankAngleMax;	// maximum absolute bank angle, minimum is 0

	float		m_fBreakingRate;
	bool		m_bDoingAction;

	A3DVECTOR3	m_vAccelerationWander;	

	A3DSkinModel * m_pSkinModel;	//Skin Model 和 ECModel 只用一个. 
	CECModel* m_pECModel;
	bool m_bUseECModel;				//是否使用ECModel，初始为false //Wyd TODO: 现在 Agent使用ECModel还有问题需要解决！！
//	A3DEngine* m_pA3DEngine;
	
	ActionList_e m_CurAction;			//当前动作

	AString m_strActionName[ACTION_NUM];
};

typedef abase::vector<ECAutonomousAgent*> ECAutonomousAgentVector;