/*
* FILE: AResSkinModelImpl.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/7
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResSkinModelImpl_H_
#define _AResSkinModelImpl_H_

#include "IResSkinModel.h"

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

class A3DSkinModel;
class IResSkin;
class AResSkinImpl;
class AResInterfaceImpl;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AResSkinModelImpl
//	
///////////////////////////////////////////////////////////////////////////

class AResSkinModelImpl : public IResSkinModel
{

public:		//	Types

	typedef std::vector<AResSkinImpl*> ResSkinArray;

public:		//	Constructor and Destructor

	explicit AResSkinModelImpl(AResInterfaceImpl* pResImpl);
	virtual ~AResSkinModelImpl(void);

public:		//	Attributes

public:		//	Operations

	bool Init(const char* szPath);
	void Release();

protected:	//	Attributes

	AString m_strSkeletonFile;
	ResSkinArray m_aResSkins;
	std::vector<AString> m_aTrackSetFiles;
	AResInterfaceImpl* m_pResImpl;

protected:	//	Operations

	bool Init(A3DSkinModel* pSkinModel);

	virtual const char* GetSkeletonFileName() const;
	virtual int GetSkinNum() const;
	virtual const IResSkin* GetSkin(int nIdx) const;

	virtual int GetTrackSetFileNum() const;
	virtual const char* GetTrackSetFile(int nIdx) const;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResSkinModelImpl_H_


