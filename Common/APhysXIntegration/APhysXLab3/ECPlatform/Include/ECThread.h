/*
* FILE: ECThread.h
*
* DESCRIPTION: Thread controller.
*
* CREATED BY: Niuyadong, 2009/4/14
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#pragma once

#include <AExpDecl.h>
#include <ABaseDef.h>
#include <Windows.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

//	Thread work function
typedef DWORD (*LPFNECTHREADWORK)(a_uiptr UserData);

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECThread
//	
///////////////////////////////////////////////////////////////////////////

class ECThread
{
public:		//	Types

public:		//	Constructor and Destructor

	ECThread();
	virtual ~ECThread();

public:		//	Attributes

public:		//	Operations

	//	Thread function
	static unsigned int WINAPI ECThreadFunc(void* lpParam);
	//	Get global thread counter number
	static volatile int GetThreadCnt() { return ECThread::m_iCount; }

	//	Create thread object.
	//	When a thread is created, it's in suspend state, user should call resume to start it
	//	bAlwaysBusy: true, thread function runs loop no matter there is work to do or not
	bool Create(LPFNECTHREADWORK lpWorkFunc, a_uiptr UserData, int iPriority=THREAD_PRIORITY_NORMAL, bool bAlwaysBusy=false);
	//	Terminate thread
	void Terminate();

	//	Suspend thread
	void Suspend();
	//	Resume thread
	void Resume();
	//	Check if thread is suspended
	volatile bool IsSuspended() const { return m_bSuspend; }

	//	Set to-work event to signal state
	void SetToWorkEvent();

protected:	//	Attributes

	static volatile int	m_iCount;

	HANDLE				m_hThread;			//	Thread handle
	HANDLE				m_hToSuspend;		//	Event handle used to suspend thread
	HANDLE				m_hToResume;		//	Event handle used to resume thread
	HANDLE				m_hToWork;			//	Event handle used to notify thread there is work to do
	HANDLE				m_hToExit;			//	Event handle used to terminate thread

	bool				m_bStart;			//	true, thread has started to work
	volatile bool		m_bSuspend;			//	true, thread has been suspended

	LPFNECTHREADWORK	m_lpWorkFunc;		//	User's work function
	a_uiptr				m_UserData;			//	User data passed to work function

protected:	//	Operations

};

