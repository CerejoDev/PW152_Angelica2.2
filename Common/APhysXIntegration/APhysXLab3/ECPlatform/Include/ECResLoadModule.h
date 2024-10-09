/*
 * FILE: ECResLoadModule.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/9/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ECResLoadModule_H_
#define _ECResLoadModule_H_

#include <AExpDecl.h>
#include <AArray.h>

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

class ECResLoader;
class ECModelLoader;
class ECBuildingLoader;
class ECDungeonLoader;
class ECPlatform;
class CECModel;
class ECBuilding;
class ECDungeon;
class ECThread;
class A3DTerrain2;
class GFXPhysXSceneShell;

//	Resource loaded callback
typedef DWORD (*LPFNECRESLOADED)(ECResLoader* pResLoader);

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECResLoadModule
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECResLoadModule
{
public:		//	Types

	//	Flags used to create loading threads
	enum
	{
		THREAD_OBJECTS	= 0x0001,	//	Object loading thread
		THREAD_TERRAIN	= 0x0002,	//	Terrain loading thread
		THREAD_ALL		= 0x0003,
	};

	struct LOADERLIST
	{
		CRITICAL_SECTION		cs;			//	Lock
		APtrArray<ECResLoader*>	aLoaders;	//	Loader list

		LOADERLIST();
		~LOADERLIST();

		//	Lock list
		void Lock() { ::EnterCriticalSection(&cs); }
		//	Unlock list
		void Unlock() { ::LeaveCriticalSection(&cs); }
		//	Add a loader
		void AddLoader(ECResLoader* pLoader);
		//	Remove all loaders
		void RemoveLoaders(bool bDelLoaders);
	};

	//	Terrain block item
	struct TRNBLOCK
	{
		A3DTerrain2*	pTerrain;	//	Terrain object
		int				row;		//	Row and column
		int				col;
		int				iBlkIndex;	//	Block index
	};

	friend class ECSceneModule;
	friend DWORD _ObjectLoadThreadFunc(a_uiptr UserData);
	friend DWORD _TerrainLoadThreadFunc(a_uiptr UserData);

public:		//	Constructor and Destructor

	ECResLoadModule();
	virtual ~ECResLoadModule();

public:		//	Attributes

public:		//	Operations

	//	Initialize
	bool Init(ECPlatform* pPlatform,GFXPhysXSceneShell* pGFXPhysXSceneShell);
	//	Release
	void Release();
	//	Clear load module. When this is called, all resources in loaded and to-unload
	//	lists will be released by calling ECResLoader::Unload, then all ECResLoader objects
	//	in all lists will be deleted.
	void Clear();

	//	Bind terrain with this load module. A load module can bind more than one
	//	terrains, but for the reason of multithread loading, 
	//	all terrain bind/unbind operations should be done when terrain loading
	//	thread doesn't work (isn't created or is suspended).
	bool BindTerrain(bool bBind, A3DTerrain2* pTerrain);

	//	Create loading threads
	//	dwThreadFlags: mask made by THREAD_xxx enums
	bool CreateLoadThreads(DWORD dwThreadFlags=THREAD_ALL);
	//	Terminate all loading threads.
	//	bClear: true, call Clear() after all loading thread terminated
	void TerminateLoadThreads(bool bClear=true);
	//	Suspend loading threads, this function just suspend loading threads and
	//	won't effect loaded resources. ECResLoader objects in all list are also kept.
	void SuspendLoadThreads();
	//	Resume loading threads
	void ResumeLoadThreads();
	//	Check if load module is suspended
	bool AreLoadThreadsSuspended();

	/*	Load all to be loaded resources and terrain data in function's caller thread,
		this function also release all resources is currently in unload lists.

		bGetTrnCandBlks: Get candidate blocks from A3DTerrain2. If this function
			is called after ECResLoadModule::Tick() in a same frame, set bGetTrnCandBlks
			to false, for in Tick() candidate blocks has just been got out. 
			In other case (for example, want to load terrain blocks immediately 
			after A3DTerrain2::SetLoadCenter), bGetTrnCandBlks should be set to true.
	*/
	bool LoadResInSingleThread(bool bGetTrnCandBlks);

	//	Tick routine.
	bool Tick(DWORD dwDeltaTime);

	/*	Load/Unload object methods.

		Note: 
		1.	Always use new/delete to create/destroy a ECResLoader object, don't use it
			as local or static variables.
		2.	Application create (new) ECResLoader objects and pass them to Loadxxx functions,
			then application should leave these loaders to platform and not delete them,
			platform will delete them automatically after call resources-loaded-callback
			functions (LPFNECRESLOADED).
		3.	Application should't use a ECResLoader pointer again after pass it to
			Unloadxxx methods, these pointers will be released by platform
			automatically at proper time.
		4.	ECResLoader class override new/delete operators, so don't worry about that
			it's created (new) and deleted in different lib (dll, exe).
	*/
	//	Load/Unload ECModel object
	bool LoadECModel(ECModelLoader* pLoader);
	void UnloadECModel(CECModel* pModel) { return InnerUnloadECModel(pModel, true); }
	//	Load building object
	bool LoadBuilding(ECBuildingLoader* pLoader);
	void UnloadBuilding(ECBuilding* pBuilding) { return InnerUnloadBuilding(pBuilding, true); }

	// Load/unload Dungeon object
	//bool LoadDungeon(ECDungeonLoader* pLoader);
	//void UnloadDungeon(ECDungeon* pDungeon) { return InnerUnloadDungeon(pDungeon, true); }

	//	Load user resource
	bool LoadUserRes(ECResLoader* pLoader) { return InnerLoadRes(pLoader); }
	void UnloadUserRes(ECResLoader* pLoader) { return InnerUnloadRes(pLoader); }

	//	Set user's callback for loaded resources
	void SetUserResLoadedCallback(LPFNECRESLOADED lpfn) { m_lpfnUserLoaded = lpfn; }

	//	Get platfrom object
	ECPlatform* GetECPlatform() const { return m_pPlatform; }

	//	Inner Load/Unload resource methods
	bool InnerLoadRes(ECResLoader* pLoader);
	void InnerUnloadRes(ECResLoader* pLoader);

	GFXPhysXSceneShell* GetPhysXSceneShell()const { return m_pGFXPhysXSceneShell; }
	void SetPhysXSceneShell(GFXPhysXSceneShell* pGFXPhysXSceneShell) {  m_pGFXPhysXSceneShell = pGFXPhysXSceneShell; }

protected:	//	Attributes

	ECPlatform*		m_pPlatform;			//	Platform object
	ECThread*		m_pObjThread;			//	Loading thread used to handle objects (like models, buildings etc.)
	ECThread*		m_pTrnThread;			//	Loading thread used to handle terrain

	LOADERLIST		m_LoadedRes;			//	Loaded resources
	LOADERLIST		m_ToLoadRes;			//	Resources to be loaded
	LOADERLIST		m_ToUnloadRes;			//	Resources to be unloaded
	LOADERLIST		m_InnerLoadedRes;		//	Inner loaded resources
	LOADERLIST		m_InnerToLoadRes;		//	Inner resources to be loaded
	LOADERLIST		m_InnerToUnloadRes;		//	Inner resources to be unloaded

	CRITICAL_SECTION		m_csTrnList;	//	m_aTerrains lock
	CRITICAL_SECTION		m_csTrnBlocks;	//	m_aTrnBlocks lock
	APtrArray<A3DTerrain2*>	m_aTerrains;	//	Bound terrains
	AArray<TRNBLOCK>		m_aTrnBlocks;	//	Terrain blocks
	bool					m_bBlkFlood;	//	true means too many blocks are waiting to be loaded, have to do 
												//	terrain loading in main thread

	LPFNECRESLOADED		m_lpfnUserLoaded;	//	Callback for user's loaded resources
	LPFNECRESLOADED		m_lpfnInnerLoaded;	//	Callback for inner loaded resources

	GFXPhysXSceneShell*		m_pGFXPhysXSceneShell;

protected:	//	Operations

	void InnerUnloadECModel(CECModel* pModel, bool bInner);
	void InnerUnloadBuilding(ECBuilding* pBuilding, bool bInner);
	void InnerUnloadDungeon(ECDungeon* pDungeon, bool bInner);
	//	Load resources in specified list
	bool LoadResOfList(LOADERLIST* pList,bool bSingleThread = false);
	//	Unload resources in specified list
	void UnloadResOfList(LOADERLIST* pList);
	//	Unload terrain blocks
	void UnloadTerrainBlocks();

	//	Set inner callback for loaded resources
	void SetInnerResLoadedCallback(LPFNECRESLOADED lpfn) { m_lpfnInnerLoaded = lpfn; }
	//	Dispatch loaded through callback
	void DispatchLoadedRes();

	//	Object loading thread function
	DWORD ObjectLoadThreadFunc();
	//	Terrain loading thread function
	DWORD TerrainLoadThreadFunc();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_ECResLoadModule_H_
