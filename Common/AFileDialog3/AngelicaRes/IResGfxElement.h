/*
* FILE: IResGfxElement.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/6
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _IResGfxElement_H_
#define _IResGfxElement_H_

#include "AFileExpDecl.h"

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
//	Class IResGfxElement
//	
///////////////////////////////////////////////////////////////////////////

class _AFILE_EXP_ IResGfxElement
{

public:		//	Types

	enum TYPE
	{
		TYPE_GFXCONTAINER,
		TYPE_MODEL,
		TYPE_ECMODEL,
		TYPE_SOUND,
		TYPE_NORMAL,
		TYPE_INVALID = 0xFFFFFFFF,
	};

public:		//	Constructor and Destructor

	IResGfxElement();
	virtual ~IResGfxElement(void) = 0;

public:		//	Attributes

public:		//	Operations

	//	Get this gfx element's type
	virtual TYPE GetType() const = 0;
	
	//	Get path methods
	virtual const char* GetSubGfxPath() const = 0;
	virtual const char* GetSmdPath() const = 0;
	virtual const char* GetEcmPath() const = 0;
	virtual int GetRandSoundNum() const = 0;
	virtual const char* GetRandSoundPath(int nIdx) const = 0;
	virtual int GetTexNum() const = 0;
	virtual const char* GetTexPath(int nIdx) const = 0;

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_IResGfxElement_H_


