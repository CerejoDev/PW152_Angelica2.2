/*
 * FILE: SubThread.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_SUBTHREAD_INTERFACE_H_
#define _APHYSXLAB_SUBTHREAD_INTERFACE_H_

#include <windows.h>
#include <process.h>
#include <queue>
#include <APhysXMutex.h>

#pragma warning(push)
#pragma warning(disable: 4786)

template<class T>
class IQueueThread
{
public:
	typedef typename T   ElementType;

	static unsigned WINAPI gThreadProc(LPVOID lpParameter);
	
public:
	IQueueThread();
	virtual ~IQueueThread() = 0;

	int  GetTaskCount() const { return m_queue.size(); };
	void AddTask(const ElementType& it);
	void RemoveAllTask();

	unsigned ThreadTask();

private:
	bool PopFront(ElementType& outT);

	// Make sure this function is NOT pure function!
	// To avoid runtime error: "pure function was called"...
	// in the case of thread context switches happens during destruction...
	// the derive object was destructed, the base object is still living and...
	// the thread is still working, then crash. 
	virtual void DoTask(const ElementType& it) {};

private:
	template<class _Ty>
	class CQueue : public std::queue<_Ty>
	{
	public:
		void clear() { c.clear(); }
	};

private:
	HANDLE m_hEvent;
	HANDLE m_hThread;

	APhysXMutex m_queueLocker;
	CQueue<ElementType> m_queue;
};

// Implementation

template<class T>
unsigned WINAPI IQueueThread<T>::gThreadProc(LPVOID lpParameter)
{
	IQueueThread* pQT = (IQueueThread*)lpParameter;
	return pQT->ThreadTask();
}

template<class T>
IQueueThread<T>::IQueueThread() : m_hThread(0)
{
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (0 != m_hEvent)
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, &gThreadProc, this, 0, NULL);
}

template<class T>
IQueueThread<T>::~IQueueThread()
{
	RemoveAllTask();
	if (NULL != m_hEvent)
	{
		SetEvent(m_hEvent);
		WaitForSingleObject(m_hThread, INFINITE);
	}
	CloseHandle(m_hThread);
	CloseHandle(m_hEvent);
	m_hThread = 0;
	m_hEvent  = 0;
}

template<class T>
void IQueueThread<T>::AddTask(const ElementType& it)
{
	APhysXMutexLock autoLocker(m_queueLocker);
	m_queue.push(it);
}

template<class T>
void IQueueThread<T>::RemoveAllTask()
{
	APhysXMutexLock autoLocker(m_queueLocker);
	m_queue.clear();
}

template<class T>
bool IQueueThread<T>::PopFront(ElementType& outT)
{
	APhysXMutexLock autoLocker(m_queueLocker);

	if (0 == GetTaskCount())
		return false;

	outT = m_queue.front();
	m_queue.pop();
	return true;
}

template<class T>
unsigned IQueueThread<T>::ThreadTask()
{
	ElementType it;
	
	do 
	{
		while (PopFront(it))
		{
			DoTask(it);
		}
	} while (WAIT_OBJECT_0 != WaitForSingleObject(m_hEvent, 1000));
	
	_endthreadex(0);
	return 0;
}

#pragma warning(pop)
#endif