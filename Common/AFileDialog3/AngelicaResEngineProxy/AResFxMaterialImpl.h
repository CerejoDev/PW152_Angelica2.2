/*
* FILE: AResFxMaterialImpl.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/7
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResFxMaterialImpl_H_
#define _AResFxMaterialImpl_H_

#include "IResFxMaterial.h"
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
class AResFxUIParamImpl;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AResFxMaterialImpl
//	
///////////////////////////////////////////////////////////////////////////

class AResFxMaterialImpl : public IResFxMaterial
{

public:		//	Types

	typedef std::vector<AResFxUIParamImpl*> ResFxUIParamArray;

public:		//	Constructor and Destructor

	explicit AResFxMaterialImpl(AResInterfaceImpl* pResImpl);
	virtual ~AResFxMaterialImpl(void);

public:		//	Attributes

public:		//	Operations

	bool Init(const char* szPath);
	bool Init(A3DFXMaterial* pFx);
	void Release();

protected:	//	Attributes

	AString m_strFxFileName;
	ResFxUIParamArray m_aFxUIParams;
	AResInterfaceImpl* m_pResImpl;

protected:	//	Operations

	//	Get reference .fx file path
	virtual const char* GetFXFileName() const;
	//	Get UI param number
	virtual int GetUIParamNum() const;
	//	Get UI param by index
	virtual IResFxUIParam* GetUIParam(int nIdx) const;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResFxMaterialImpl_H_


