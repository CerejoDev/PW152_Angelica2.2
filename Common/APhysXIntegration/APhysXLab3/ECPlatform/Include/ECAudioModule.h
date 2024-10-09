/*
 * FILE: ECAudioModule.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2011/6/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AExpDecl.h"
#include "A3DVector.h"

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

class ECPlatform;

namespace AudioEngine
{
	class EventSystem;
	class Event;
	class EventInstance;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECAudioModule
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECAudioModule
{
public:		//	Types

	friend class ECPlatform;

protected:	//	Types

public:	//	Constructor and Destructor

	ECAudioModule();
	virtual ~ECAudioModule();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	bool Tick(const A3DVECTOR3& pos, const A3DVECTOR3& vel, const A3DVECTOR3& forward, const A3DVECTOR3& up);

	//	Create event instance
	//	bAutoDel: true means the returned instance will be destroyed automatically when it stopped
	AudioEngine::EventInstance* CreateEventInst(const char* szFullPath, bool bAutoDel);
	//	Release event instance
	void ReleaseEventInst(AudioEngine::EventInstance*& pInst);
	//	Quickly play a event and don't worry about anything else like: how it is played; when it is destroyed etc.
	//	iLayer: see AudioEngine::LAYER_INDEX
	bool QuicklyPlay(const char* szFullPath, int iLayer);
	//	Get event object
	AudioEngine::Event* GetEvent(const char* szFullPath);

	//	Get event system interface
	AudioEngine::EventSystem* GetEventSystem() { return m_pAudioSystem; }

protected:	//	Attributes

	ECPlatform*					m_pECPlatform;		//	Platform object
	AudioEngine::EventSystem*	m_pAudioSystem;		//	Audio system

protected:	//	Operations

	//	Initialize object
	bool Init(ECPlatform* pECPlatform);
	//	Release object
	void Release();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


