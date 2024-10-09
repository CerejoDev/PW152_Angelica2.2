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
	virtual void RuntimeBegin(const Region& place) = 0;
	virtual void RuntimeEnd(const Region& place) = 0;

	virtual void EnterRegion(const Region& place, unsigned int nbEntities, IObjBase* entities) = 0;
	virtual void LeaveRegion(const Region& place, unsigned int nbEntities, IObjBase* entities) = 0;
};

#endif
