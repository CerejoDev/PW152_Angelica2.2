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

class IOutputWnd
{
public:
	virtual	~IOutputWnd() = 0 {}
	virtual void OutputText(const char* szMsg) = 0;
};

class CALogWrapper
{
public:
	static CALogWrapper* GetInstance()
	{
		static CALogWrapper	log;
		return  &log;
	};

	bool Init(const AString& logFileBasicName, IOutputWnd* pOP = 0);
	void Release();
	void SetOutputWnd(IOutputWnd* pOP) { m_pOPWnd = pOP; }

private:
	CALogWrapper() { m_bIsInited = false; m_pOPWnd = 0; }
	~CALogWrapper() { if (m_bIsInited) Release(); }

	static void _LogOutput(const char* szMsg)
	{
		CALogWrapper* pLog = GetInstance();
		assert(true == pLog->m_bIsInited);
		pLog->m_Log.Log(szMsg);
		if (0 != pLog->m_pOPWnd)
			pLog->m_pOPWnd->OutputText(szMsg);
	}

private:
	bool m_bIsInited;
	ALog m_Log;
	IOutputWnd* m_pOPWnd;
};
#endif