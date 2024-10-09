/*
* FILE: IResInterface.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/6
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _IResInterface_H_
#define _IResInterface_H_

#include <cassert>
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

class IRes;
class IResSkinModel;
class IResECModel;
class IResGfx;
class IResUnlitModel;
class IResUnlitModelSet;
class IResSkin;
class IResFxMaterial;
class IResSpeedTree;
class IResAtt;
class IResLog;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class IResInterface
//	
///////////////////////////////////////////////////////////////////////////

class _AFILE_EXP_ IResInterface
{

public:		//	Types

public:		//	Constructor and Destructor

	IResInterface();
	virtual ~IResInterface(void) = 0;

public:		//	Attributes

public:		//	Operations

	virtual IResLog* GetResLog() const = 0;
	virtual IResSkinModel* LoadSmd(const char* szPath) = 0;
	virtual IResECModel* LoadEcm(const char* szPath) = 0;
	virtual IResGfx* LoadGfx(const char* szPath) = 0;
	virtual IResAtt* LoadAtt(const char* szPath) = 0;
	virtual IResUnlitModel* LoadUmd(const char* szPath) = 0;
	virtual IResUnlitModelSet* LoadUmds(const char* szPath) = 0;
	virtual IResSkin* LoadSki(const char* szPath) = 0;
	virtual IResFxMaterial* LoadAfm(const char* szPath) = 0;
	virtual IResSpeedTree* LoadSpt(const char* szPath) = 0;
	virtual void ReleaseRes(IRes* pRes) = 0;

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


struct AResWrapper
{
public:
	AResWrapper(IRes* pRes, IResInterface* pInterface)
		: m_pRes(pRes)
		, m_pResInterface(pInterface)
	{
		assert( pRes && pInterface );
	}

	~AResWrapper()
	{
		m_pResInterface->ReleaseRes(m_pRes);
	}

private:
	IRes* m_pRes;
	IResInterface* m_pResInterface;
};

#endif	//	_IResInterface_H_


