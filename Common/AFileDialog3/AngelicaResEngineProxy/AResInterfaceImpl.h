/*
* FILE: AResInterfaceImpl.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/6
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResInterfaceImpl_H_
#define _AResInterfaceImpl_H_

#include "IResInterface.h"

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

class A3DEngine;
class A3DDevice;
class A3DGfxEngine;
class A3DGFXExMan;
class ECTreeForest;
class GFXPhysXSceneShell;
class A3DSkillGfxEventMan;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AResInterfaceImpl
//	
///////////////////////////////////////////////////////////////////////////

class AResInterfaceImpl : public IResInterface
{

public:		//	Types

public:		//	Constructor and Destructor

	AResInterfaceImpl(void);
	virtual ~AResInterfaceImpl(void);

public:		//	Attributes

public:		//	Operations

	bool Init(A3DEngine* pA3DEngine
			, A3DGfxEngine* pA3DGfxEngine
			, GFXPhysXSceneShell* pPhysXShell
			, A3DSkillGfxEventMan* pSkillGfxEventMan);

	void Release();

	inline A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	inline A3DDevice* GetA3DDevice() const { return m_pA3DDevice; }
	inline A3DGfxEngine* GetA3DGfxEngine() const { return m_pA3DGfxEngine; }
	inline A3DGFXExMan* GetA3DGfxExMan() const { return m_pA3DGfxExMan; }
	inline GFXPhysXSceneShell* GetPhysXShell() const { return m_pPhysXShell; }
	inline A3DSkillGfxEventMan* GetSkillGfxEventMan() const { return m_pSkillGfxEventMan; }

protected:	//	Attributes

	A3DEngine* m_pA3DEngine;
	A3DDevice* m_pA3DDevice;
	A3DGfxEngine* m_pA3DGfxEngine;
	A3DGFXExMan* m_pA3DGfxExMan;
	GFXPhysXSceneShell* m_pPhysXShell;
	A3DSkillGfxEventMan* m_pSkillGfxEventMan;
	IResLog* m_pResLog;

protected:	//	Operations

	virtual IResLog* GetResLog() const;
	virtual IResSkinModel* LoadSmd(const char* szPath);
	virtual IResECModel* LoadEcm(const char* szPath);
	virtual IResGfx* LoadGfx(const char* szPath);
	virtual IResAtt* LoadAtt(const char* szPath);
	virtual IResUnlitModel* LoadUmd(const char* szPath);
	virtual IResUnlitModelSet* LoadUmds(const char* szPath);
	virtual IResSkin* LoadSki(const char* szPath);
	virtual IResFxMaterial* LoadAfm(const char* szPath);
	virtual IResSpeedTree* LoadSpt(const char* szPath);
	virtual void ReleaseRes(IRes* pRes);

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResInterfaceImpl_H_


