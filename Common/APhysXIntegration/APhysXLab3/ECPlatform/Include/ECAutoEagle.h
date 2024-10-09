/*
 * FILE: ECAutoEagle.h
 *
 * DESCRIPTION: One kind of autonomous agent in Angelica Engine, to simulate eagle behaviour
 *
 * CREATED BY: Changzhi Li, 2004/11/22
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once 

#include "ECAutonomousAgent.h"

class ECAutoEagle : public ECAutonomousAgent
{
public:
	ECAutoEagle () ;

	virtual bool		Init (bool bUseECModel,A3DEngine *pEngine,A3DGfxEngine* pGfxEngine);
	virtual void		ApplySteeringForce (float fElapsedTime);		// time in sec

	virtual A3DVECTOR3	SteerForWander (float fElapsedTime);	// time in sec
	virtual A3DVECTOR3	SteerForSeek (const A3DVECTOR3& target);
	virtual A3DVECTOR3	SteerForFlee (const A3DVECTOR3& target);	
	virtual A3DVECTOR3	SteerForSeparation (float maxDistance, float cosMaxAngle, const ECAutonomousGroup* pGroup);
	virtual A3DVECTOR3	SteerForAlignment (float maxDistance, float cosMaxAngle, const ECAutonomousGroup* pGroup);
	virtual A3DVECTOR3	SteerForCohesion (float maxDistance, float cosMaxAngle, const ECAutonomousGroup* pGroup);

	virtual void	RegenerateLocalSpace (A3DVECTOR3 vNewForward, float fBankAngle);
	virtual void	Update (DWORD dwDeltaTime, ActionList_e action = ACTION_FIRST);		// time in msec
	virtual void	Reset (void);
	
	virtual A3DVECTOR3 RespondToBoundary (const A3DVECTOR3& vCenter, float fBoundaryRadius);
	virtual void	ClampPosition (A3DTerrain2 * pTerrain, A3DTerrainWater * pTerrainWater, DWORD dwDeltaTime);

protected:
	void	ClampAboveTerrain (A3DVECTOR3 vPredicted, A3DTerrain2 * pTerrain);
	void	ClampAboveTerrainWater (A3DVECTOR3 vPredicted, A3DTerrainWater * pTerrainWater);
};