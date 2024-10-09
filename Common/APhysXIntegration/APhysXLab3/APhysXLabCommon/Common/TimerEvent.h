/*
 * FILE: TimerEvent.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/12/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_TIMEREVNET_H_
#define _APHYSXLAB_TIMEREVNET_H_

class TimerEventMgr;

class TimerEvent
{
public:
	enum DelayType
	{
		DT_TIME  = 0,
		DT_FRAME = 1,
	};

public:
	TimerEvent(IEventNotify& notify, float delayTime, DelayType type = DT_TIME, int nbEvents = 1);
	TimerEvent(const TimerEvent& rhs);
	TimerEvent& operator= (const TimerEvent& rhs);
	~TimerEvent();

	DelayType GetType() const { return m_Type; }
	void SetType(DelayType type) { m_Type = type; }

	float GetDelayTime() const { return m_DelayTime; }
	void SetDelayTime(float delayTime) { m_DelayTime = delayTime; }

	int GetNbAllEvents() const { return m_nbAllEvents; }
	int GetNbNowEvents() const { return m_nbNowEvents; }

protected:
	friend TimerEventMgr;

	bool Tick(float dtSec);
	void OnNotifyDie(const IObjBase& obj);

private:
	DelayType m_Type;
	float m_DelayTime;
	float m_Counter;
	int m_nbAllEvents;
	int m_nbNowEvents;
	IEventNotify* m_pNotify;
};

class TimerEventMgr : public IObjStateReport
{
public:
	TimerEventMgr();
	~TimerEventMgr();
	void RegisterEvent(IEventNotify& notify, float delayTime, TimerEvent::DelayType type, int nbEvents);
	void RegisterEvent(TimerEvent* pTEvent);

	void EnterRuntime(APhysXScene& aPhysXScene);
	void LeaveRuntime();
	void TickTimerEvents(float dtSec);

	virtual void OnNotifyDie(const IObjBase& obj);

private:
	APhysXScene* m_pRuntimeScene;
	APtrArray<TimerEvent*>	m_TEvents;
};

#endif
