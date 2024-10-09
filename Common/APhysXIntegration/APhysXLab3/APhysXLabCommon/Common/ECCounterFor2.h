/*
 * FILE: ECCounter.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/2/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "ABaseDef.h"

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
//	Class ECCounter
//	
///////////////////////////////////////////////////////////////////////////

#ifndef _ANGELICA3

class ECCounter
{
public:		//	Types

public:		//	Constructor and Destructor

	ECCounter()
	{
		m_dwCounter = 0;
		m_dwPeriod	= 0;
		m_bPause	= false;
	}

	ECCounter(DWORD dwCnt, DWORD dwPeriod)
	{
		m_dwCounter = dwCnt;
		m_dwPeriod	= dwPeriod;
		m_bPause	= false;
	}

public:		//	Attributes

public:		//	Operations

	//	Set / Get period
	void SetPeriod(DWORD dwPeriod) { m_dwPeriod = dwPeriod; }
	DWORD GetPeriod() { return m_dwPeriod; }
	//	Set / Get counter
	void SetCounter(DWORD dwCounter) { m_dwCounter = dwCounter; }
	DWORD GetCounter() { return m_dwCounter; }

	//	Has counter reached period ?
	bool IsFull() { return (m_dwCounter >= m_dwPeriod) ? true : false; }
	//	Reset counter
	void Reset(bool bFull=false) { m_dwCounter = bFull ? m_dwPeriod : 0; }
	//	Set pause flag
	void SetPause(bool bPause) { m_bPause = bPause; }

	//	Increase counter
	bool IncCounter(DWORD dwCounter)
	{ 
		if (!m_bPause)
			m_dwCounter += dwCounter;

		return (m_dwCounter >= m_dwPeriod) ? true : false;
	}

	//	Decrease counter
	void DecCounter(DWORD dwCounter)
	{ 
		if (m_bPause)
			return;

		if (m_dwCounter <= dwCounter)
			m_dwCounter = 0;
		else
			m_dwCounter -= dwCounter;
	}

protected:	//	Attributes

	DWORD	m_dwCounter;		//	Counter
	DWORD	m_dwPeriod;			//	Count period
	bool	m_bPause;			//	Pause flag

protected:	//	Operations

};
#endif
///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


