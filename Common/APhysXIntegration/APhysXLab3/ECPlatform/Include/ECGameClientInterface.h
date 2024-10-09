/*
 * FILE: ECGameClientInterface.h
 *
 * DESCRIPTION: ECGameClientInterface
 *
 * CREATED BY: Niuyadong, 2009/11/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "netclient.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECGameClientInterface
//	
///////////////////////////////////////////////////////////////////////////
namespace GNET
{
	class ECGameClientInterface: public NetClient, public Timer::Observer
	{
	public:
		enum STATE{
			STATE_CLOSED,
			STATE_CONNECTING,
			STATE_KEEPING,
		};
	public:
		typedef int(* callback_t)(void*, Session::ID, int,ECGameClientInterface*);
		virtual int Connect(const char* host, unsigned short port) ;
		virtual void Disconnect() ;
		const char* GetLastError();

		void Attach(callback_t funptr) { m_callback = funptr; }
		void Detach() { m_callback = NULL; }
		void SetZoneID(char zoneid) { m_zoneid = zoneid; }
		char GetZoneID() const { return m_zoneid; }
		unsigned int GetSID() { return m_sid; }
	public:

	protected:
		int ConnectTo(const char* host, unsigned short port);
		int ConnectTo(struct in_addr *, unsigned short port);
	protected:
		callback_t m_callback;
		char m_zoneid;
		int m_state;
		Session::ID m_sid;
		Thread::Mutex locker;
		std::map<Session::ID, DWORD> pingmap;
		std::string hostaddr;
		unsigned int m_ping;
		DWORD m_lastsend;
		DWORD m_lastrecv;

		bool m_bIsUseUsbKey;
	};
}

