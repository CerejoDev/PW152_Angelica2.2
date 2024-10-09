/*
* FILE: ARes.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/06/30
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _ARes_H_
#define _ARes_H_

#include "AFileExpDecl.h"
#include "AResTypes.h"
#include "AResSharedPtr.h"

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


enum AResType
{
	ARES_SINGLE,	// .*
	ARES_AFM,		// .afm
	ARES_ECM,		// .ecm3
	ARES_GFX,		// .gfx3
	ARES_ATT,		// .att3
	ARES_LMD,		// .lmd
	ARES_UMD,		// .umd
	ARES_UMDS,		// .umds
	ARES_CHF,		// .chf  convex hull file
	ARES_BSP,		// .bsp  binary space partition for umd / umds
	ARES_SMD,		// .SMD
	ARES_SKI,		// .ski
	ARES_SPT,		// .spt  speedtree
};

class A3DDevice;
class ARes;
class AResManager;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ARes
//	
///////////////////////////////////////////////////////////////////////////

class ARes
{

public:		//	Types

public:		//	Constructor and Destructor

	explicit ARes(AResManager* pResMan) 
		: m_pResMan(pResMan)
	{

	}
	virtual ~ARes(void) = 0 {}

public:		//	Attributes

	//	ARes Null Object
	static AResPtr NullObject();

public:		//	Operations

	virtual bool Init(const char* szPath) = 0;
	virtual void Release() = 0;
	virtual bool GetLastModifyTime(FILETIME* pLastWriteTime) const = 0;
	//	Files that this resource ref to
	virtual int GetRefResNum() const = 0;
	virtual AResPtr GetRefRes(int nIdx) const = 0;
	//	File for exact this resource
	virtual AResString GetFile() const = 0; 

protected:	//	Attributes

	AResManager* m_pResMan;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_ARes_H_


