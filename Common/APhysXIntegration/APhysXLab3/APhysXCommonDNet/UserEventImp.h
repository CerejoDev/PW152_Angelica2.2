#pragma once

#include "UserEvent.h"
//implementation of IUserEvent
namespace APhysXCommonDNet
{
	
	class UserEventImp : public IUserEvent
	{
	public:
		virtual void RuntimeBegin(const CRegion& place);
		virtual void RuntimeEnd(const CRegion& place);

		virtual void EnterRegion(const CRegion& place, unsigned int nbEntities, IObjBase** entities);
		virtual void LeaveRegion(const CRegion& place, unsigned int nbEntities, IObjBase** entities);

		static IUserEvent* GetInstancePtr()
		{
			static UserEventImp eventImp;
			return &eventImp;
		}
	};

	public ref class MEventNotify
	{
	public:
		virtual void OnEventNotify(MTimerEvent^ timer) {}
		virtual void OnPreTickNotify(float dtSec, MTimerEvent^ timer) {}
		virtual void OnNotifyDie(MIObjBase^ obj) {}
	};

	class EventNotifyImp : public IEventNotify
	{
	public:
		EventNotifyImp();
		void SetEventNotify(MEventNotify^ pEN);

		virtual EventNotifyImp* Clone() const;
		virtual void OnEventNotify(TimerEvent& timer);
		virtual void OnNotifyDie(const IObjBase& obj);
		virtual void OnPreTickNotify(float dtSec, const TimerEvent& timer);

	private:
		gcroot<MEventNotify^> m_pMEN;
	};
}