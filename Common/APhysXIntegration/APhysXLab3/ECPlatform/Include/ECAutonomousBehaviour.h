/*
 * FILE: ECAutonomousBehaviour.h
 *
 * DESCRIPTION: The header and interface of Autonomous Behaviour in the engine
 *
 * CREATED BY: Changzhi Li, 2004/11/02
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once 

class ECAutonomousGroup;

class ECAutonomousBehaviour
{
public:
	ECAutonomousBehaviour (void) {}

	// initialize position, velocity of all members
	virtual void InitAutoAgentBehavior (ECAutonomousGroup* pGroup) = 0;

	// not affect members position, velocity
	virtual void Reset (ECAutonomousGroup* pGroup) {}

	virtual void Update (ECAutonomousGroup* pGroup, DWORD dwDeltaTime,const A3DVECTOR3& vHostPlayerPos) = 0;
	void Release (void) {}
};

class ECAutoBehaviourWander : public ECAutonomousBehaviour
{
public:
	ECAutoBehaviourWander () { }
	void InitAutoAgentBehavior (ECAutonomousGroup * pGroup);
	void Update (ECAutonomousGroup * pGroup, DWORD dwDeltaTime,const A3DVECTOR3& vHostPlayerPos);
	void Reset (ECAutonomousGroup* pGroup);
};

class ECAutoBehaviourHover : public ECAutonomousBehaviour
{
public:
	ECAutoBehaviourHover () : m_vTarget(0, 0, 0) { }
	void InitAutoAgentBehavior (ECAutonomousGroup * pGroup);
	void Update (ECAutonomousGroup * pGroup, DWORD dwDeltaTime,const A3DVECTOR3& vHostPlayerPos);
	void Reset (ECAutonomousGroup * pGroup);

	void SetTargetPosition (A3DVECTOR3 target) { m_vTarget = target; }
	A3DVECTOR3 GetTargetPosition (void) { return m_vTarget; }
	
protected:
	A3DVECTOR3 m_vTarget;
};

class ECAutoBehaviourBoid : public ECAutonomousBehaviour
{
public:
	ECAutoBehaviourBoid () { }
	void InitAutoAgentBehavior (ECAutonomousGroup * pGroup);
	void Update (ECAutonomousGroup * pGroup, DWORD dwDeltaTime,const A3DVECTOR3& vHostPlayerPos);
	void Reset (ECAutonomousGroup * pGroup);
};

class ECAutoBehaviourArrival : public ECAutonomousBehaviour
{
public:
	ECAutoBehaviourArrival () {}
	void InitAutoAgentBehavior (ECAutonomousGroup * pGroup);
	void Update (ECAutonomousGroup * pGroup, DWORD dwDeltaTime,const A3DVECTOR3& vHostPlayerPos);
	void Reset (ECAutonomousGroup * pGroup);

	void SetTargetPosition (const A3DVECTOR3& vTarget) { m_vTarget = vTarget; }
	A3DVECTOR3 GetTargetPosition (void) { return m_vTarget; }

protected:
	A3DVECTOR3	m_vTarget;
};

class ECAutoBehaviourFlee : public ECAutonomousBehaviour
{
public:
	ECAutoBehaviourFlee () {}
	void InitAutoAgentBehavior (ECAutonomousGroup * pGroup);
	void Update (ECAutonomousGroup * pGroup, DWORD dwDeltaTime,const A3DVECTOR3& vHostPlayerPos);
	void Reset (ECAutonomousGroup * pGroup);

	void SetTargetPosition (const A3DVECTOR3& vTarget) { m_vTarget = vTarget; }
	A3DVECTOR3 GetTargetPosition (void) { return m_vTarget; }

protected:
	A3DVECTOR3	m_vTarget;
};


class ECAutoBehaviourFleeAndBack : public ECAutonomousBehaviour
{
	enum FleeAndBackState_e
	{
		BEGIN_STATE,
		FLEE_STATE,
		BACK_STATE,
	};

public:
	ECAutoBehaviourFleeAndBack();

	void InitAutoAgentBehavior (ECAutonomousGroup * pGroup);
	void Update (ECAutonomousGroup * pGroup, DWORD dwDeltaTime,const A3DVECTOR3& vHostPlayerPos);
	void Reset (ECAutonomousGroup * pGroup){};

	void SetTargetPosition (const A3DVECTOR3& vTarget) { m_vTarget = vTarget; }
	A3DVECTOR3 GetTargetPosition (void) { return m_vTarget; }

	void SetFleeDist(float dist) { m_fFleeDist = dist;}
	void SetBackDist(float dist) { m_fBackDist = dist;}

	float GetFleeDist() { return m_fFleeDist;}
	float GetBackDist() { return m_fBackDist;}

protected:
	void BeginProcess(ECAutonomousGroup * pGroup, DWORD dwDeltaTime);
	void FleeProcess(ECAutonomousGroup * pGroup, DWORD dwDeltaTime);
	void BackProcess(ECAutonomousGroup * pGroup, DWORD dwDeltaTime);

	A3DVECTOR3	m_vTarget;
	FleeAndBackState_e m_State;
	bool m_bStartFlee;

	float m_fFleeDist;// FLEE_DIS 20		//¥•∑¢Ã”…¢µƒæ‡¿Î
	float m_fBackDist;// BACK_DIS 70		//¥•∑¢ªÿπÈµƒæ‡¿Î

};