/*
* FILE: IResECModel.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/6
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _IResECModel_H_
#define _IResECModel_H_

#include "IRes.h"
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

class IResComAct;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class IResECModel
//	
///////////////////////////////////////////////////////////////////////////

class _AFILE_EXP_ IResECModel : public IRes
{

public:		//	Types

public:		//	Constructor and Destructor

	IResECModel();
	virtual ~IResECModel(void) = 0;

public:		//	Attributes

public:		//	Operations

	virtual const char* GetSkinModelFileName() const = 0;
	virtual int GetChildCount() const = 0;
	virtual const char* GetChildModelPath(int nIdx) const = 0;
	virtual const char* GetIKSolverFile() const = 0;

	//	for cogfx map
	virtual int GetCoGfxPathNum() const = 0;
	//	expect to return a full relative path (relative to angelica work dir) for gfx file
	virtual bool GetCoGfxPath(int nIdx, char* szPath, int iBufLen) const = 0;

	//	for additional skins
	virtual int GetAdditionalSkinNum() const = 0;
	//	get additional skins 
	virtual bool GetAdditionalSkinPath(int nIdx, char* szPath, int iBufLen) const = 0;

	virtual int GetComActCount() const = 0;
	virtual const IResComAct* GetComAct(int nIdx) const = 0;

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_IResECModel_H_


