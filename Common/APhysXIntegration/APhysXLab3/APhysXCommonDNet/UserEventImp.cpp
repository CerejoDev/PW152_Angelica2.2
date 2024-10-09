#include "Stdafx.h"
#include "UserEventImp.h"
#include "MTimerEvent.h"

/*#include "APhysXLabCoreHeaders.h"
#include "MTimerEvent.h"
#include "MA3DQuaternion.h"
#include "MIObjBase.h"
#include <vcclr.h>
#include "MA3DVector3.h"
#include "MA3DMatrix4.h"
#include "MIKeyEvent.h"
#include "MPhysXObjBase.h"
#include "MScene.h"
#include "UserEventImp.h"
#include "MRegion.h"*/

using namespace System;

namespace APhysXCommonDNet
{
	void UserEventImp::RuntimeBegin(const CRegion& place)
	{

	}
	void UserEventImp::RuntimeEnd(const CRegion& place)
	{

	}
	void UserEventImp::EnterRegion(const CRegion& place, unsigned int nbEntities, IObjBase** entities)
	{
		MIObjBase^ obj = MScene::Instance->GetMIObjBase(&place);
		MRegion^ region = dynamic_cast<MRegion^>(obj);
		if (region != nullptr)
		{
			region->EnterRegion(nbEntities, entities);
		}
	}
	void UserEventImp::LeaveRegion(const CRegion& place, unsigned int nbEntities, IObjBase** entities)
	{
		MIObjBase^ obj = MScene::Instance->GetMIObjBase(&place);
		MRegion^ region = dynamic_cast<MRegion^>(obj);
		if (region != nullptr)
		{
			region->LeaveRegion(nbEntities, entities);
		}
	}

	EventNotifyImp::EventNotifyImp()
	{
		m_pMEN = nullptr;
	}
	void EventNotifyImp::SetEventNotify(MEventNotify^ pEN)
	{
		m_pMEN = pEN;
	}
	EventNotifyImp* EventNotifyImp::Clone() const
	{
		EventNotifyImp* pNew = new EventNotifyImp();
		pNew->SetEventNotify(m_pMEN);
		return pNew;
	}
	void EventNotifyImp::OnEventNotify(TimerEvent& timer)
	{
		if (m_pMEN)
			m_pMEN->OnEventNotify(gcnew MTimerEvent(timer));
	}
	void EventNotifyImp::OnNotifyDie(const IObjBase& obj)
	{
		MIObjBase^ MObj = MScene::Instance->GetMIObjBase(&obj);
		if (m_pMEN)
			m_pMEN->OnNotifyDie(MObj);
	}
	void EventNotifyImp::OnPreTickNotify(float dtSec, const TimerEvent& timer)
	{
		if (m_pMEN)
			m_pMEN->OnPreTickNotify(dtSec, gcnew MTimerEvent(timer));
	}
}