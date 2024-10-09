/*
* FILE: AResECModelImpl.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/6
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResECModelImpl_H_
#define _AResECModelImpl_H_

#include "IResECModel.h"
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

class CECModel;
class AResComActImpl;
class AResInterfaceImpl;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AResECModelImpl
//	
///////////////////////////////////////////////////////////////////////////

class AResECModelImpl : public IResECModel
{

public:		//	Types

	typedef std::vector<AResComActImpl*> ResComActArray;

public:		//	Constructor and Destructor

	explicit AResECModelImpl(AResInterfaceImpl* pResImpl);
	virtual ~AResECModelImpl(void);

public:		//	Attributes

public:		//	Operations

	bool Init(const char* szPath);

protected:	//	Attributes

	CECModel* m_pECModel;
	AResInterfaceImpl* m_pResImpl;
	std::vector<AString> m_aCoGfxPaths;
	ResComActArray m_aResComActs;

protected:	//	Operations

	virtual const char* GetSkinModelFileName() const;
	virtual int GetChildCount() const;
	virtual const char* GetChildModelPath(int nIdx) const;
	virtual const char* GetIKSolverFile() const;

	//	for cogfx map
	virtual int GetCoGfxPathNum() const;
	virtual bool GetCoGfxPath(int nIdx, char* szPath, int iBufLen) const;

	virtual int GetComActCount() const;
	virtual const IResComAct* GetComAct(int nIdx) const;

	//	for additional skins
	virtual int GetAdditionalSkinNum() const;
	//	get additional skins 
	virtual bool GetAdditionalSkinPath(int nIdx, char* szPath, int iBufLen) const;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResECModelImpl_H_


