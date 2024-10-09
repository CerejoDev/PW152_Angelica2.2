#pragma once

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define  _WIN32_WINNT 0x0501
#endif

class ECCriticalSession
{
private:
	void * pCriticalSection_;
public:
	ECCriticalSession();
	~ECCriticalSession();
	void Enter();
	void Leave();
#if (WINVER >= 0x0400) && (_WIN32_WINNT >= 0x0400)
	bool TryEnter();
#endif

public:
	class Scope
	{
		ECCriticalSession & cs_;
	public:
		Scope(ECCriticalSession & cs) : cs_(cs) { cs_.Enter(); }
		~Scope() { cs_.Leave(); }
	};
};