/*
 * FILE: ECAutonomousGroup.h
 *
 * DESCRIPTION: The basic implementation of IAutonomousGroup interface
 *
 * CREATED BY: Changzhi Li, 2004/11/02
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once 

#include "ECAutonomousAgent.h"

class A3DTerrain2;
class A3DTerrainWater;
class ECAutonomousBehaviour;
class ECAutonomousAgent;


class ECAutonomousGroup
{
public:
	ECAutonomousGroup();
	~ECAutonomousGroup(){};

	void SetActiveBehaviour(ECAutonomousBehaviour * pBehaviour);
	void AddMember(ECAutonomousAgent* agent);
	void SetCenter(A3DVECTOR3 vCenter);	
	void SetBoundaryRadius(float r);
	void SetTerrain2(A3DTerrain2* pTerrain);
	void SetTerrainWater(A3DTerrainWater* pTerrainWater);
	// This function should be called after setActiveBehavior/AddMember, and before update. 
	void SetupAgentBehavior(void);

	void Release(void);
	void Reset(void);
	void MoveCenter(A3DVECTOR3 vDelta);

	void Update(DWORD dwDeltaTime,const A3DVECTOR3& vHostPlayerPos);
	void Render(A3DViewport* pViewport, bool bCheckVis = TRUE);

	ECAutonomousAgentVector& GetAllMembers(void);
	const ECAutonomousAgentVector& GetAllMembers(void) const;	
	ECAutonomousBehaviour * GetActiveBehaviour(void) const;
	A3DVECTOR3 GetCenter(void) const;		
	float	GetBoundaryRadius(void);

	A3DTerrain2* GetTerrain2();
	A3DTerrainWater* GetTerrainWater();

protected:
	ECAutonomousAgentVector m_vpMembers;
	ECAutonomousBehaviour * m_pActiveBehaviour;

	A3DVECTOR3		m_vCenter;
	A3DVECTOR3		m_vHostPlayerPos;
	float			m_fBoundaryRadius;

	A3DTerrain2 *			m_pTerrain;
	A3DTerrainWater *		m_pTerrainWater;	
};