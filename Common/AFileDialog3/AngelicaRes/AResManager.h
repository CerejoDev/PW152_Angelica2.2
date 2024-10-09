/*
* FILE: AResManager.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/06/30
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResManager_H_
#define _AResManager_H_

#include "AFileExpDecl.h"
#include "AResSharedPtr.h"
#include "AResTypes.h"
#include "hashmap.h"

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
class A3DGFXEx;
class A3DGfxEngine;
class A3DGFXExMan;
class A3DDevice;
class ARes;
class AResFactroy;
class AResGfxInterface;
class GFXPhysXSceneShell;
class ECTreeForest;
class IResLog;
class IResInterface;

struct AResInit
{
	A3DEngine*		m_pA3DEngine;
	A3DGfxEngine*	m_pA3DGfxEngine;
};


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AResManager
//	
///////////////////////////////////////////////////////////////////////////

class AResManager
{

public:		//	Types

	friend class SharedPtr<AResManager>;

	enum INITSTATUS
	{
		INIT_OK				= 0,
		INIT_FAILED			= 1,
		INIT_ALREADYINITED	= 2
	};

public:	//	Constructor and Destructor

	AResManager();
	virtual ~AResManager(void);

public:		//	Attributes

public:		//	Operations

	//INITSTATUS Init(const AResInit& ResInit);
	INITSTATUS Init(IResInterface* pResEngineProxy);
	void Release();
	void SetResFactory(const AResFactroyPtr& pResFactory);

	//	AResManager is a singleton object, call CreateResMan multiple times will only the first object created
	//	Create one AResManager object, which creates and manages all resources
	//	Pass one pA3DDevice pointer for AResManager, then AResManager will use this engine
	//	Pass NULL will make AResManager create one A3DEngine for use
	static AResManager* CreateResMan(IResInterface* pResEngineProxy);
	//	If you use outside managed A3DEngine and A3DGfxEngine, then before those engines release
	//	you should call this function to release all related resources be cleared
	static void ReleaseResMan();

	//	Load a resource from relative path under Angelica base directory
	//	Return ARes::NullObject() if file not exist, or file could not be loaded...
	AResPtr LoadRes(const char* szPath, bool bRequireExist);

	//	Get Engine Proxy Interface
	inline IResInterface* GetResInterface() const { return m_pResEngineProxy; }
/*
	//	Simple Wrapper Function
	A3DGFXEx* LoadGfx(const char* szPath);
	void ReleaseGfx(A3DGFXEx* pGfx);
*/
protected:	//	Attributes

	IResInterface* m_pResEngineProxy;
	AResFactroyPtr m_pResFactory;
	bool m_bInited;

protected:	//	Operations
/*
	void CreateErrLog();
	void ReleaseErrLog();

	bool CreateEngine();
	bool CreateA3DGfxEngine(A3DEngine* pA3DEngine);
	bool CreateSptForest(A3DEngine* pA3DEngine);

	void ReleaseEngine();
	void ReleaeA3DGfxEngine();
	void ReleaseSptForest();
*/
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResManager_H_


