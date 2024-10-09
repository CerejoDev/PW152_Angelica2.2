/*
* FILE: AResGfxElementImpl.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/7
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResGfxElementImpl_H_
#define _AResGfxElementImpl_H_

#include "IResGfxElement.h"
#include <AString.h>
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

class A3DGFXElement;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AResGfxElementImpl
//	
///////////////////////////////////////////////////////////////////////////

class AResGfxElementImpl : public IResGfxElement
{

public:		//	Types

public:		//	Constructor and Destructor

	AResGfxElementImpl(void);
	virtual ~AResGfxElementImpl(void);

public:		//	Attributes

public:		//	Operations

	bool Init(A3DGFXElement* pElement);
	void Release();

protected:	//	Attributes

	TYPE m_Type;
	AString m_strSubGfxPath;
	AString m_strSmdPath;
	AString m_strEcmPath;
	std::vector<AString> m_aRandSoundPath;
	std::vector<AString> m_aTexs;

protected:	//	Operations

	void InitGfxContainer(A3DGFXElement* pElement);
	void InitModel(A3DGFXElement* pElement);
	void InitECModel(A3DGFXElement* pElement);
	void InitSound(A3DGFXElement* pElement);
	void InitNormal(A3DGFXElement* pElement);

	//	Get this gfx element's type
	virtual TYPE GetType() const;

	//	Get path methods
	virtual const char* GetSubGfxPath() const;
	virtual const char* GetSmdPath() const;
	virtual const char* GetEcmPath() const;
	virtual int GetRandSoundNum() const;
	virtual const char* GetRandSoundPath(int nIdx) const;
	virtual int GetTexNum() const;
	virtual const char* GetTexPath(int nIdx) const;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResGfxElementImpl_H_


