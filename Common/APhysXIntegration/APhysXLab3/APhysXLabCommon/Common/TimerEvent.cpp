/*
 * FILE: TimerEvent.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/12/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

TimerEvent::TimerEvent(IEventNotify& notify, float delayTime, DelayType type, int nbEvents)
{
	m_Type = type;
	m_DelayTime = delayTime;
	m_Counter = 0.0f;
	m_nbAllEvents = nbEvents;
	m_nbNowEvents = 0;
	m_pNotify = notify.Clone();
}

TimerEvent::TimerEvent(const TimerEvent& rhs)
{
	*this = rhs;
}

TimerEvent& TimerEvent::operator= (const TimerEvent& rhs)
{
	m_Type = rhs.m_Type;
	m_DelayTime = rhs.m_DelayTime;
	m_Counter = 0.0f;
	m_nbAllEvents = rhs.m_nbAllEvents;
	m_nbNowEvents = 0;
	m_pNotify = 0;
	if (0 != rhs.m_pNotify)
		m_pNotify = rhs.m_pNotify->Clone();
	return *this;
}

TimerEvent::~TimerEvent()
{
	delete m_pNotify;
}

bool TimerEvent::Tick(float dtSec)
{
	if (0 != m_pNotify)
		m_pNotify->OnPreTickNotify(dtSec, *this);

	if (DT_TIME == m_Type)
		m_Counter += dtSec;
	else
		m_Counter += 1;
	if (m_Counter < m_DelayTime)
		return false;
	m_Counter = 0.0f;

	if (0 != m_nbAllEvents)
	{
		m_nbNowEvents += 1;
		if (0 != m_pNotify)
			m_pNotify->OnEventNotify(*this);
	}

	if (0 > m_nbAllEvents)
		return false;
	if (m_nbNowEvents < m_nbAllEvents)
		return false;
	return true;
}

void TimerEvent::OnNotifyDie(const IObjBase& obj)
{
	if (0 != m_pNotify)
		m_pNotify->OnNotifyDie(obj);
}

TimerEventMgr::TimerEventMgr()
{
	m_pRuntimeScene = 0;
}

TimerEventMgr::~TimerEventMgr()
{
	LeaveRuntime();
}

void TimerEventMgr::RegisterEvent(IEventNotify& notify, float delayTime, TimerEvent::DelayType type, int nbEvents)
{
	TimerEvent* pTE = new TimerEvent(notify, delayTime, type, nbEvents);
	if (0 != pTE)
		m_TEvents.Add(pTE);
}

void TimerEventMgr::RegisterEvent(TimerEvent* pTEvent)
{
	if (0 == pTEvent)
		return;

	TimerEvent* pTE = new TimerEvent(*pTEvent);
	if (0 != pTE)
		m_TEvents.Add(pTE);
}

void TimerEventMgr::EnterRuntime(APhysXScene& aPhysXScene)
{
	if (0 != m_pRuntimeScene)
	{
		assert(!"Shouldn't be here! Debug it!");
		return;
	}
	m_pRuntimeScene = &aPhysXScene;
}

void TimerEventMgr::LeaveRuntime()
{
	m_pRuntimeScene = 0;
	int nCount = m_TEvents.GetSize();
	for (int i = 0; i < nCount; ++i)
		delete m_TEvents[i];
	m_TEvents.RemoveAll(false);
}

void TimerEventMgr::TickTimerEvents(float dtSec)
{
	if (0 == m_pRuntimeScene)
		return;

	int idx = 0;
	while (idx < m_TEvents.GetSize())
	{
		if (m_TEvents[idx]->Tick(dtSec))
		{
			delete m_TEvents[idx];
			m_TEvents.RemoveAtQuickly(idx);
		}
		else
			idx++;
	}
}

void TimerEventMgr::OnNotifyDie(const IObjBase& obj)
{
	int nCount = m_TEvents.GetSize();
	for (int i = 0; i < nCount; ++i)
		m_TEvents[i]->OnNotifyDie(obj);
}
