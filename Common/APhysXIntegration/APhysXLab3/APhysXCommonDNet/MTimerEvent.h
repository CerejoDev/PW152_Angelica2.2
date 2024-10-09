#pragma once

namespace APhysXCommonDNet
{
	public enum class MDelayType
	{
		DT_TIME  = 0,
		DT_FRAME = 1,
	};

	ref class MEventNotify;

	public ref class MTimerEvent
	{
	public:
		MTimerEvent(MEventNotify^ notify, float delayTime, MDelayType dt, int nbEvents);

		TimerEvent::DelayType GetType();
		void SetType(MDelayType type);

		float GetDelayTime();
		void SetDelayTime(float delayTime);

		int GetNbAllEvents();
		int GetNbNowEvents();

	internal:
		MTimerEvent(TimerEvent& timer);
		MTimerEvent(const TimerEvent& timer);
		MEventNotify^ GetMEventNotify() { return m_pNotify; }

	private:
		void CommonInit(const TimerEvent& timer);

	private:
		bool m_bReadOnly;
		TimerEvent::DelayType m_Type;
		float m_DelayTime;
		int m_nbAllEvents;

		TimerEvent* m_pTimerEvent;
		MEventNotify^ m_pNotify;
	};

}