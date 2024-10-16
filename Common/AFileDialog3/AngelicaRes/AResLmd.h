/*
* FILE: AResLmd.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/06/30
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResLmd_H_
#define _AResLmd_H_

#include <vector>
#include "AResSingle.h"

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
//	Class AResLmd
//	
///////////////////////////////////////////////////////////////////////////

class AResLmd : public AResSingle
{

public:		//	Types

	typedef AResSingle base_class;

public:		//	Constructor and Destructor

	explicit AResLmd(AResManager* pResMan);
	virtual ~AResLmd(void);

public:		//	Attributes

public:		//	Operations

protected:	//	Attributes

	std::vector<AResPtr> m_aRefRes;

protected:	//	Operations

	virtual bool Init(const char* szPath);
	virtual void Release();
	//	Files that this resource ref to
	virtual int GetRefResNum() const;
	virtual AResPtr GetRefRes(int nIdx) const;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResLmd_H_


