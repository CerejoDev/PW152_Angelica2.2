/*
* FILE: AResFxUIParamImpl.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/7
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResFxUIParamImpl_H_
#define _AResFxUIParamImpl_H_

#include "IResFxUIParam.h"

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

class A3DFXUIParam;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AResFxUIParamImpl
//	
///////////////////////////////////////////////////////////////////////////

class AResFxUIParamImpl : public IResFxUIParam
{

public:		//	Types

public:		//	Constructor and Destructor

	AResFxUIParamImpl(void);
	virtual ~AResFxUIParamImpl(void);

public:		//	Attributes

public:		//	Operations

	bool Init(A3DFXUIParam* pParam);

protected:	//	Attributes

	bool m_bIsTex;
	AString m_strTexPath;

protected:	//	Operations

	virtual bool IsParamTexture() const;
	virtual const char* GetTexPath() const;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResFxUIParamImpl_H_


