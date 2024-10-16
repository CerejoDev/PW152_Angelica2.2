/*
* FILE: ARes.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/06/30
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#include "stdafx.h"
#include "AResPreRequireHeader.h"
#include "ARes.h"
#include "AResManager.h"

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

class AResNullObject : public ARes
{
public:

	AResNullObject()
		: ARes(NULL)
	{

	}

protected:

	virtual bool Init(const char* szPath)
	{
		return true;
	}

	virtual void Release() 
	{

	}

	virtual int GetRefResNum() const 
	{
		return 0;
	}

	virtual AResPtr GetRefRes(int nIdx) const
	{
		return ARes::NullObject();
	}

	virtual AResString GetFile() const
	{
		return STR_EMPTY;
	}

	virtual bool GetLastModifyTime(FILETIME* pLastWriteTime) const
	{
		return false;
	}
};


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement ARes
//	
///////////////////////////////////////////////////////////////////////////

AResPtr ARes::NullObject()
{
	static AResPtr obj = MakeSharedPtr<AResNullObject>();
	return obj;
}