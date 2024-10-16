/*
* FILE: IResSkinModel.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/6
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _IResSkinModel_H_
#define _IResSkinModel_H_

#include "IRes.h"

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

class IResSkin;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class IResSkinModel
//	
///////////////////////////////////////////////////////////////////////////

class _AFILE_EXP_ IResSkinModel : public IRes
{

public:		//	Types

public:		//	Constructor and Destructor

	IResSkinModel();
	virtual ~IResSkinModel(void) = 0;

public:		//	Attributes

public:		//	Operations

	virtual const char* GetSkeletonFileName() const = 0;
	virtual int GetSkinNum() const = 0;
	virtual const IResSkin* GetSkin(int nIdx) const = 0;

	virtual int GetTrackSetFileNum() const = 0;
	virtual const char* GetTrackSetFile(int nIdx) const = 0;

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_IResSkinModel_H_


