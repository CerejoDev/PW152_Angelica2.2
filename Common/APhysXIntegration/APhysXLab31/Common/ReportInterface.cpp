/*
* FILE: ReportInterface.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/04/08
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#include "ReportInterface.h"

void ObjStateReportMgr::OnNotifyBorn(const IObjBase& obj)
{
	IOSR::iterator it = m_Receivers.begin();
	IOSR::iterator itEnd = m_Receivers.end();
	for (; it != itEnd; ++it)
		(*it)->OnNotifyBorn(obj);
}

void ObjStateReportMgr::OnNotifyDie(const IObjBase& obj)
{
	IOSR::iterator it = m_Receivers.begin();
	IOSR::iterator itEnd = m_Receivers.end();
	for (; it != itEnd; ++it)
		(*it)->OnNotifyDie(obj);
}

void ObjStateReportMgr::OnNotifyWakeUp(const IObjBase& obj) 
{
	IOSR::iterator it = m_Receivers.begin();
	IOSR::iterator itEnd = m_Receivers.end();
	for (; it != itEnd; ++it)
		(*it)->OnNotifyWakeUp(obj);
}

void ObjStateReportMgr::OnNotifySleep(const IObjBase& obj)
{
	IOSR::iterator it = m_Receivers.begin();
	IOSR::iterator itEnd = m_Receivers.end();
	for (; it != itEnd; ++it)
		(*it)->OnNotifySleep(obj);
}

void ObjStateReportMgr::OnNotifySelChange(const IObjBase* pNewFocus, bool bSelected, bool bFocusOn)
{
	IOSR::iterator it = m_Receivers.begin();
	IOSR::iterator itEnd = m_Receivers.end();
	for (; it != itEnd; ++it)
		(*it)->OnNotifySelChange(pNewFocus, bSelected, bFocusOn);
}

void ObjStateReportMgr::OnUpdateStateRedraw()
{
	IOSR::iterator it = m_Receivers.begin();
	IOSR::iterator itEnd = m_Receivers.end();
	for (; it != itEnd; ++it)
		(*it)->OnUpdateStateRedraw();
}

void ObjStateReportMgr::RegisterReceiver(IObjStateReport& osr)
{
	IObjStateReport* pOSR = &osr;

	IOSR::iterator it = m_Receivers.begin();
	IOSR::iterator itEnd = m_Receivers.end();
	for (; it != itEnd; ++it)
	{
		if (pOSR == *it)
			return;
	}
	m_Receivers.push_back(pOSR);
}

void ObjStateReportMgr::UnregisterReceiver(IObjStateReport& osr)
{
	IObjStateReport* pOSR = &osr;
	IOSR::iterator it = m_Receivers.begin();
	IOSR::iterator itEnd = m_Receivers.end();
	for (; it != itEnd; ++it)
	{
		if (pOSR == *it)
		{
			IOSR::iterator itLast = itEnd;
			--itLast;
			*it = *itLast;
			m_Receivers.pop_back();
			return;
		}
	}
}

void ObjStateReportMgr::ClearAllReceiver()
{
	m_Receivers.clear();
}
