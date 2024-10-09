/*
* FILE: AResLogImpl.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/8
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResEnginePreRequireHeader.h"
#include "AResLogImpl.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement AResLogImpl
//	
///////////////////////////////////////////////////////////////////////////

AResLogImpl::AResLogImpl(void)
{
}

AResLogImpl::~AResLogImpl(void)
{
}

// Init a log file
//		szLogFile	will be the logs path
//		szHelloMsg	is the hello message in the log
//		bAppend		is the flag to append at the end of the log file
bool AResLogImpl::Init(const char* szLogFile, const char* szHelloMsg, bool bAppend/*=false*/)
{
	return m_logInst.Init(szLogFile, szHelloMsg, bAppend);
}

// Release the log file
//		this call will close the log file pointer and write a finished message
bool AResLogImpl::Release()
{
	return m_logInst.Release();
}

// Output a variable arguments log message
bool AResLogImpl::Log(const char* fmt, ...)
{
	char szErrorMsg[2048];
	va_list args_list;

	va_start(args_list, fmt);	   
	_vsnprintf_s(szErrorMsg, sizeof(szErrorMsg), sizeof (szErrorMsg), fmt, args_list);
	va_end(args_list);

	return LogString(szErrorMsg);
}

// Output a string as a log message
bool AResLogImpl::LogString(const char* szString)
{
	return m_logInst.LogString(szString);
}
