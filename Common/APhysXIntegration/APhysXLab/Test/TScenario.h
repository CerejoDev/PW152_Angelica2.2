/*
 * FILE: TScenario.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/09/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_TSCENARIO_H_
#define _APHYSXLAB_TSCENARIO_H_

class IScenario
{
public:
	IScenario(NxScene& scene) : m_Scene(scene) {}
	virtual ~IScenario() {}
	
	virtual void CreateScenario(CGame& rGame, const NxVec3& centerPos, const NxGroupsMask* pGroupMask = 0) = 0;
	virtual void ReleaseScenario() = 0;
	virtual bool ControlScenario(const unsigned int nChar) = 0;
	virtual void TickScenario(float dwDeltaTimeSec) = 0;

protected:
	NxScene& m_Scene;
};

class ScenarioMgr : public IScenario
{
public:
	ScenarioMgr(NxScene& scene);
	~ScenarioMgr();

	void SelectNext();
	
	virtual void CreateScenario(CGame& rGame, const NxVec3& centerPos, const NxGroupsMask* pGroupMask = 0);
	virtual void ReleaseScenario();
	virtual bool ControlScenario(const unsigned int nChar);
	virtual void TickScenario(float dwDeltaTimeSec);

private:
	int m_idxCurrent;
	NxArray<IScenario*> m_pvecScenario;
};

class ScenarioImp : public IScenario
{
public:
	ScenarioImp(NxScene& scene) : IScenario(scene) { m_pGame = 0; }
	~ScenarioImp() { ReleaseScenario(); }

	virtual void CreateScenario(CGame& rGame, const NxVec3& centerPos, const NxGroupsMask* pGroupMask = 0);
	virtual void ReleaseScenario();
	virtual bool ControlScenario(const unsigned int nChar);
	virtual void TickScenario(float dwDeltaTimeSec);

private:
	virtual void OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask = 0) = 0;
	virtual void OnReleaseScenario() {}
	virtual bool OnControlScenario(const unsigned int nChar) { return false; }

protected:
	CGame* m_pGame;
	MMPolicy m_MovementCtrller;
};

class BasicScenario : public ScenarioImp
{
public:
	BasicScenario(NxScene& scene) : ScenarioImp(scene) {}
private:
	virtual void OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask = 0);
};

class KinematicScenario : public ScenarioImp
{
public:
	KinematicScenario(NxScene& scene) : ScenarioImp(scene) {}
private:
	virtual void OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask = 0);
};

class TeeterScenario : public ScenarioImp
{
public:
	TeeterScenario(NxScene& scene) : ScenarioImp(scene) { m_pJoint = 0; }
private:
	virtual void OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask = 0);
	virtual void OnReleaseScenario();
	virtual bool OnControlScenario(const unsigned int nChar);

private:
	NxVec3 m_SideActorPos;
	NxGroupsMask m_GM;
	NxJoint* m_pJoint;
};

class CCTestScenario : public ScenarioImp
{
public:
	CCTestScenario(NxScene& scene) : ScenarioImp(scene) {}
	
private:
	virtual void OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask = 0);
	virtual bool OnControlScenario(const unsigned int nChar);
	
private:
	NxVec3 m_SideActorPos;
	NxGroupsMask m_GM;
};

class CCBlockerSDKScenario : public ScenarioImp
{
public:
	CCBlockerSDKScenario(NxScene& scene) : ScenarioImp(scene) { m_pActorS = m_pActorD = m_pActorK = 0; }
	
private:
	virtual void OnCreateScenario(const NxVec3& centerPos, const NxGroupsMask* pGroupMask = 0);
	virtual void OnReleaseScenario();
	virtual bool OnControlScenario(const unsigned int nChar);

	void CreateTarget(const NxVec3& targetPos);
	void PrintTargetGM();
	
private:
	NxActor* m_pActorS;
	NxActor* m_pActorD;
	NxActor* m_pActorK;

	NxGroupsMask m_GM;
	NxGroupsMask m_GMReverse;
};
#endif