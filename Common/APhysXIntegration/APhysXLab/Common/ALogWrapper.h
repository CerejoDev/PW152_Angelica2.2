/*
 * FILE: ALogWrapper.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/05/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_ALOGWRAPPER_H_
#define _APHYSXLAB_ALOGWRAPPER_H_

#include <AC.h>

class CALogWrapper
{
public:
	static CALogWrapper* GetInstance()
	{
		static CALogWrapper	log;
		return &log;
	};

	bool Init();
	void Release();

private:
	CALogWrapper() {};	

	static void _LogOutput(const char* szMsg)
	{
		GetInstance()->m_Log.Log(szMsg);
	}

private:
	ALog m_Log;
};
#endif