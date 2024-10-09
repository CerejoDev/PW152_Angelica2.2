/*
* FILE: AResLogImpl.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/8
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResLogImpl_H_
#define _AResLogImpl_H_

#include "IResLog.h"
#include <ALog.h>

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
//	Class AResLogImpl
//	
///////////////////////////////////////////////////////////////////////////

class AResLogImpl : public IResLog
{

public:		//	Types

public:		//	Constructor and Destructor

	AResLogImpl(void);
	virtual ~AResLogImpl(void);

public:		//	Attributes

public:		//	Operations

protected:	//	Attributes

	ALog m_logInst;

protected:	//	Operations

	// Init a log file
	//		szLogFile	will be the logs path
	//		szHelloMsg	is the hello message in the log
	//		bAppend		is the flag to append at the end of the log file
	virtual bool Init(const char* szLogFile, const char* szHelloMsg, bool bAppend=false);

	// Release the log file
	//		this call will close the log file pointer and write a finished message
	virtual bool Release();

	// Output a variable arguments log message;
	virtual bool Log(const char* fmt, ...);

	// Output a string as a log message;
	virtual bool LogString(const char* szString);

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResLogImpl_H_


