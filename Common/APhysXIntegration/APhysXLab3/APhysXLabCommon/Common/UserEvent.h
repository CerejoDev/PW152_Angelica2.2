/*
 * FILE: UserEvent.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 20011/05/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_USEREVENT_H_
#define _APHYSXLAB_USEREVENT_H_

class IUserEvent
{
public:
	virtual ~IUserEvent() {}
	virtual void RuntimeBegin(const CRegion& place) = 0;
	virtual void RuntimeEnd(const CRegion& place) = 0;

	virtual void EnterRegion(const CRegion& place, unsigned int nbEntities, IObjBase** entities) = 0;
	virtual void LeaveRegion(const CRegion& place, unsigned int nbEntities, IObjBase** entities) = 0;
};

class TimerEvent;

class IEventNotify
{
public:
	virtual ~IEventNotify() {}
	virtual IEventNotify* Clone() const = 0;
	virtual void OnEventNotify(TimerEvent& timer) = 0;
	// this interface was called during entire life cycle of TimerEvent even in the dead frame or event trigger frame. 
	virtual void OnPreTickNotify(float dtSec, const TimerEvent& timer) {}
	virtual void OnNotifyDie(const IObjBase& obj) {}
};

#endif
