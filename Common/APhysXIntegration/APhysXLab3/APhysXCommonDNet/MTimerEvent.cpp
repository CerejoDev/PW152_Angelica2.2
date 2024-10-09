#include "Stdafx.h"
#include "MTimerEvent.h"

namespace APhysXCommonDNet
{
	MTimerEvent::MTimerEvent(TimerEvent& timer)
	{
		CommonInit(timer);
		m_bReadOnly = false;
	}
	MTimerEvent::MTimerEvent(const TimerEvent& timer)
	{
		CommonInit(timer);
	}
	void MTimerEvent::CommonInit(const TimerEvent& timer)
	{
		m_bReadOnly = true;
		m_Type = timer.GetType();
		m_DelayTime = timer.GetDelayTime();
		m_nbAllEvents = timer.GetNbAllEvents();

		m_pTimerEvent = const_cast<TimerEvent*>(&timer);
		m_pNotify = nullptr;
	}

	MTimerEvent::MTimerEvent(MEventNotify^ notify, float delayTime, MDelayType dt, int nbEvents)
	{
		m_bReadOnly = false;
		m_Type = static_cast<TimerEvent::DelayType>(dt);
		m_DelayTime = delayTime;
		m_nbAllEvents = nbEvents;

		m_pTimerEvent = nullptr;
		m_pNotify = notify;
	}

	TimerEvent::DelayType MTimerEvent::GetType()
	{
		if (nullptr != m_pTimerEvent)
			return m_pTimerEvent->GetType();
		return m_Type;
	}
	void MTimerEvent::SetType(MDelayType type)
	{
		if (m_bReadOnly)
			return;

		TimerEvent::DelayType dt = static_cast<TimerEvent::DelayType>(type);
		if (nullptr != m_pTimerEvent)
			m_pTimerEvent->SetType(dt);
		m_Type = dt;
	}
	float MTimerEvent::GetDelayTime()
	{
		if (nullptr != m_pTimerEvent)
			return m_pTimerEvent->GetDelayTime();
		return m_DelayTime;
	}
	void MTimerEvent::SetDelayTime(float delayTime)
	{
		if (m_bReadOnly)
			return;

		if (nullptr != m_pTimerEvent)
			m_pTimerEvent->SetDelayTime(delayTime);
		m_DelayTime = delayTime;
	}
	int MTimerEvent::GetNbAllEvents()
	{
		if (nullptr != m_pTimerEvent)
			return m_pTimerEvent->GetNbAllEvents();
		return m_nbAllEvents;
	}
	int MTimerEvent::GetNbNowEvents()
	{
		return m_pTimerEvent->GetNbNowEvents();
	}
}