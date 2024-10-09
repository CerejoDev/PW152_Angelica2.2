/*
 * FILE: ECScriptModule.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/9/4
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ECSCRIPTMODULE_H_
#define _ECSCRIPTMODULE_H_

#include <LuaWrapper/LuaAPI.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Lib api item, see detail in ECScriptModule::LIBAPI
#define EC_LIBAPI_ITEM(api) {#api, Imp##api},

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class ECPlatform;
class CLuaState;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECScriptModule
//	
///////////////////////////////////////////////////////////////////////////

class ECScriptModule
{
public:		//	Types

	//	Predefined lua state
	enum LUASTATE
	{
		STATE_CONFIG = 0,
		STATE_LOGIC,
	};

	/*	Lib api structure, below is an example of using LIBAPI

		ECScriptModule::LIBAPI lib_apis[] = 
		{
			EC_LIBAPI_ITEM(func1),
			EC_LIBAPI_ITEM(func2),
			EC_LIBAPI_ITEM(func3),
			EC_LIBAPI_ITEM(func4)
		}
	*/
	struct LIBAPI
	{
		char*			szApiName;	//	API's name
		lua_CFunction	func;		//	function pointer	
	};

public:		//	Constructor and Destructor

	ECScriptModule(ECPlatform* pPlatform);
	virtual ~ECScriptModule();

public:		//	Attributes

public:		//	Operations

	//	Initialize
	bool Init();
	//	Release
	void Release();

	//	Tick routine
	void Tick(DWORD  dwDeltaTime);

	//	Register APIs in a lib
	bool RegisterLibAPIs(LUASTATE state, const char* szLib, int iAPICnt, LIBAPI* aAPIs);
	//	Load a script file (.lua)
	bool LoadScript(LUASTATE state, const char* szFile);
	/*	Load some script files (.lua).

		aFiles should looks like:

		const char* aFiles[] = 
		{
			"script\\config\\global_api.lua",
			"script\\config\\ECMName.lua",
			"script\\item_desc_text.lua",
			"script\\item_desc_utility.lua",
			"script\\item_desc.lua",
			NULL
		};
	*/
	bool LoadScripts(LUASTATE state, const char** aFiles);

	//	Get lua state
	CLuaState* GetLuaState(LUASTATE state);

protected:	//	Attributes

	ECPlatform*		m_pPlatform;	//	Platform object

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_ECSCRIPTMODULE_H_
