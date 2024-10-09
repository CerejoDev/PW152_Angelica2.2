/*
 * FILE: ECPlatformSetting.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Niuyadong, 2009/9/1
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ECPLATFORMSETTINGS_H_
#define _ECPLATFORMSETTINGS_H_

#include <ABaseDef.h>
#include <AExpDecl.h>
#include <A3DVector.h>

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
class ECPlatformInit;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECPlatformSettings
//	
///////////////////////////////////////////////////////////////////////////


class _AEXP_DECL ECPlatformSettings
{
public:		//	Types

	struct GRAPHICS_PARAM
	{
		int		iScreenW;	//	The width of screen
		int		iScreenH;	//	The Height of screen
		bool	bFullScreen;	//	Full screen flag
		bool	bVSync;			//	Vertical sync flag
	};

	struct SCRIPT_PARAM
	{
		bool	bEnableDebug;			//	Enable script debug
	};

public:		//	Constructor and Destructor

	ECPlatformSettings();
	virtual ~ECPlatformSettings();

public:		//	Attributes

public:		//	Operations

	//	Initialize platformSettings
	bool Init(const ECPlatformInit& PlatformParam);
	//	Release platform
	void Release();

	//-----------------Platform-----------------
	bool GetFlagModuleA3D(){return m_bModuleA3D;}
	bool GetFlagModuleUtility(){return m_bModuleUtility;}
	bool GetFlagModuleNetworks(){return m_bModuleNetworks;}
	bool GetFlagModuleInput(){return m_bModuleInput;}

	//-----------------Graphic-----------------
	bool IsFullScreen()	{ return GraphicsParam.bFullScreen;}
	bool IsVSync()		{ return GraphicsParam.bVSync;}
	int	GetScreenW()	{ return GraphicsParam.iScreenW;}
	int	GetScreenH()	{ return GraphicsParam.iScreenH;}

	//-----------------Scene-----------------

	//-----------------Input-----------------




	//-----------------Script-----------------
	bool GetScriptDebug() { return ScriptParam.bEnableDebug; }

protected:	//	Attributes
	bool	m_bModuleA3D;
	bool	m_bModuleUtility;
	bool	m_bModuleNetworks;
	bool	m_bModuleInput;

	GRAPHICS_PARAM		GraphicsParam;
	SCRIPT_PARAM		ScriptParam;


protected:	//	Operations

	//	Set default settings
	void SetDefaultSettings();

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_ECPLATFORM_H_
