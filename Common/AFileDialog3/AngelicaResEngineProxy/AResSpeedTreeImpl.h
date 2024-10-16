/*
* FILE: AResSpeedTreeImpl.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/7
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResSpeedTreeImpl_H_
#define _AResSpeedTreeImpl_H_

#include "IResSpeedTree.h"
#include <vector>

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

class AResInterfaceImpl;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AResSpeedTreeImpl
//	
///////////////////////////////////////////////////////////////////////////

class AResSpeedTreeImpl : public IResSpeedTree
{

public:		//	Types

public:		//	Constructor and Destructor

	explicit AResSpeedTreeImpl(AResInterfaceImpl* pResImpl);
	virtual ~AResSpeedTreeImpl(void);

public:		//	Attributes

public:		//	Operations

	bool Init(const char* szPath);
	void Release();

protected:	//	Attributes

	std::vector<AString> m_aTexFiles;
	AResInterfaceImpl* m_pResImpl;

protected:	//	Operations

	virtual int GetTexNum() const;
	virtual const char* GetTexFileName(int nIdx) const;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResSpeedTreeImpl_H_


