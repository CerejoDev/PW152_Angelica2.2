/*
* FILE: ReportInterface.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/04/08
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSXLAB_IREPORT_H_
#define _APHYSXLAB_IREPORT_H_

#include <vector>

class IObjBase;

class IObjStateReport
{
public:
	virtual ~IObjStateReport() = 0 {}

	virtual void OnNotifyBorn(const IObjBase& obj) {}
	virtual void OnNotifyDie(const IObjBase& obj) {}
	virtual void OnNotifyWakeUp(const IObjBase& obj) {}
	virtual void OnNotifySleep(const IObjBase& obj) {}
	virtual void OnNotifySelChange(const IObjBase* pNewFocus, bool bSelected, bool bFocusOn) {}
	virtual void OnUpdateStateRedraw() {}
};

class ObjStateReportMgr : public IObjStateReport
{
public:
	virtual void OnNotifyBorn(const IObjBase& obj);
	virtual void OnNotifyDie(const IObjBase& obj);
	virtual void OnNotifyWakeUp(const IObjBase& obj);
	virtual void OnNotifySleep(const IObjBase& obj);
	virtual void OnNotifySelChange(const IObjBase* pNewFocus, bool bSelected, bool bFocusOn);
	virtual void OnUpdateStateRedraw();

public:
	void RegisterReceiver(IObjStateReport& osr);
	void UnregisterReceiver(IObjStateReport& osr);
	void ClearAllReceiver();

private:
	typedef		std::vector<IObjStateReport*>	IOSR;
	IOSR m_Receivers;
};
#endif