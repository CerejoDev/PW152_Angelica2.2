/*
 * FILE: ECNetwork.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/4/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <AString.h>
#include <AArray.h>
#include <AList2.h>
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
namespace GNET
{
	class Protocol;
	class ECGameClientInterface;
	class Octets;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECNetPrtcInterface
//	
///////////////////////////////////////////////////////////////////////////
class ECNetPrtcInterface
{
public:
	//	On session opening
	virtual bool OnSessionOpen() = 0;
	//	On session closing
	virtual bool OnSessionClose() = 0;
	/*	Process protocol.

	Return value:

	0: process fail.
	1: process successfully
	2: protocol processing should be postponed
	3: process finished but don't remove this protocol
	*/
	virtual int ProcessProtocol(GNET::Protocol* pProtocol) = 0;
	virtual ~ECNetPrtcInterface(){};
protected:
private:
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECNetwork
//	
///////////////////////////////////////////////////////////////////////////

class ECNetwork
{
public:		//	Types

	//	Overtime check ID
	enum
	{
		OT_CHALLENGE = 0,
		OT_ENTERGAME,
	};

	//	Overtime check
	struct OVERTIME
	{
		int		iCheckID;
		bool	bCheck;
		DWORD	dwTime;
		DWORD	dwTimeCnt;
	};

	//	Initialize parameters
	struct INIT_PARAMS
	{
		char	szIP[256];
		int		iPort;
		char	szServerName[256];
		int		iServerID;
	};

	//friend class ECNetPrtcHandler;


public:	//	Constructor and Destructor

	ECNetwork();
	virtual ~ECNetwork();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(ECNetPrtcInterface* pPrtcHandler,GNET::ECGameClientInterface* pGameClient);
	//	Release object
	void Release();

	//	Get connected flag
	bool IsConnected() { return m_bConnected; }
	void SetConnectedFlag(bool bFlag) { m_bConnected = bFlag; }

	//	Get Connection broken flag
	bool IsLinkBroken() { return m_bLinkBroken;	}
	void SetLinkBroken(bool bFlag) { m_bLinkBroken = bFlag; }

	//	Open session
	bool Open(const INIT_PARAMS& ip);
	//	Close sesssion
	void Close();

	//	Update
	bool Update(DWORD dwDeltaTime);

	//	Set link ID
	void SetLinkID(DWORD idLink) { m_idLink = idLink; }
	//	Get link ID
	DWORD GetLinkID() { return m_idLink; }

	//	Add a protocol to m_aNewProtocols and prepare to be processed
	int AddNewProtocol(GNET::Protocol* pProtocol);
	//	Process protocols in m_aNewProtocols
	bool ProcessNewProtocols();
	//	Clear all processed protocols in m_aOldProtocols
	void ClearOldProtocols();


	//	Get overtime counter
	bool GetOvertimeCnt(DWORD& dwTotalTime, DWORD& dwCnt);
	//	Start / End overtime
	void DoOvertimeCheck(bool bStart, int iCheckID, DWORD dwTime);

	//	Get net manager
	GNET::ECGameClientInterface* GetNetManager() { return m_pNetMan; }
	//	Get protocol handler
	ECNetPrtcInterface* GetPrtcHandler() { return m_pPrtcHandler; }

	// Send protocl
	bool SendNetData(const GNET::Protocol& p, bool bUrg = false);

protected:	//	Attributes

	GNET::ECGameClientInterface*	m_pNetMan;			//	Net manager
	ECNetPrtcInterface*	m_pPrtcHandler;		//	Protocol handler

	volatile bool		m_bConnected;		//	Connected flag
	CRITICAL_SECTION	m_csNewPrtcs;		//	Lock of new received protocols
	INIT_PARAMS			m_InitParams;		//	Initalize parameters

	DWORD		m_idLink;			//	Link ID
	bool		m_bProcNewPtrc;		//	true, is processing new protocols
	bool		m_bLinkBroken;		//	Connection was broken
	bool		m_bExitConnect;
	HANDLE		m_hConnectThread;	//	Handle of connect thread

	OVERTIME	m_ot;				//	Overtime info

	APtrArray<GNET::Protocol*>	m_aNewPrtcs;		//	New received protocols
	APtrArray<GNET::Protocol*>	m_aOldPrtcs;		//	Processed protocols
	APtrList<GNET::Protocol*>	m_DelayPrtcList;	//	Postponed protocols

protected:	//	Operations

	//	Connect thread function
	static unsigned int WINAPI ConnectThread(void* pArg);

	//	Connect server
	bool Connect();

	//	When connection was broken, this function is called
	void LinkBroken(bool bDisconnect);
	//	On overtime happens
	void OnOvertimeHappen();

	//	Send net data
	bool SendNetData(const GNET::Protocol* p) { return p ? SendNetData(*p) : false; }


};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


