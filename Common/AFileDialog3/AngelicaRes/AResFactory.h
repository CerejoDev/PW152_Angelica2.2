/*
* FILE: AResFactory.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/06/30
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResFactory_H_
#define _AResFactory_H_

#include <vector>
#include <hash_map>
#include "ARes.h"

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

class AResManager;

struct _hash_str
{
	_hash_str() {}
	_hash_str(const _hash_str & hf) {}

	inline unsigned long operator()(const std::string& s) const
	{
		unsigned long h = 0;
		for(std::string::const_iterator itr = s.begin(); itr != s.end(); ++itr)
		{
			h = h * 31 + *(unsigned char *)(*itr);
		}
		return h;
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AResFactory
//	
///////////////////////////////////////////////////////////////////////////

class AResFactory
{

public:		//	Types

	typedef stdext::hash_map<AResString, AResPtr> AResPathInstanceMap;

public:		//	Constructor and Destructor

	AResFactory(AResManager* pResMan);
	virtual ~AResFactory(void);

public:		//	Attributes

public:		//	Operations

	AResPtr MakeResource(const char* szPath, bool bRequireExist);
	void ReleaseRes(AResPtr& );
	void Release();

protected:	//	Attributes

	AResManager* m_pResMan;
	AResPathInstanceMap m_mapRes;

protected:	//	Operations

	AResType GetTypeFromPath(const char* szPath);
	AResPtr MakeResourceImpl(const char* szPath);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResFactory_H_


