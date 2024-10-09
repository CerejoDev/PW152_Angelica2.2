/*
 * FILE: ECInputFilter.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "ECInputModule.h"
#include <hashmap.h>

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECInputFilter
//	
///////////////////////////////////////////////////////////////////////////

class ECInputFilter
{
public:		//	Types

	//	Mouse event
	enum MouseEvent
	{
		EVENT_UNKNOWN = -1,		//	Unknown
		EVENT_LBUTTONDOWN = 0,	//	Left button down
		EVENT_LBUTTONCLK,		//	Left button click
		EVENT_LBUTTONDBLCLK,	//	Left button double click
		EVENT_LBUTTONUP,		//	Left button up
		EVENT_RBUTTONDOWN,		//	Right button down
		EVENT_RBUTTONCLK,		//	Right button click
		EVENT_RBUTTONDBLCLK,	//	Right button double click
		EVENT_RBUTTONUP,		//	Right button up
		EVENT_MBUTTONDOWN,		//	Middle button down
		EVENT_MBUTTONUP,		//	Middle button up
		EVENT_MOUSEMOVE,		//	Mouse moves
		EVENT_MOUSEWHEEL,		//	Mouse wheel rolls
	};

	//	Clear logic key flag
	enum
	{
		CLEAR_MAP1	= 0x01,
		CLEAR_MAP2	= 0x02,
		CLEAR_ALL	= 0x03,
	};

	//	Control key combo-flag
	enum
	{
		CKCOMBO_SCA = 0,
		CKCOMBO_CA,
		CKCOMBO_SA,
		CKCOMBO_SC,
		CKCOMBO_S,
		CKCOMBO_C,
		CKCOMBO_A,
		CKCOMBO_NONE,
		NUM_CKCOMBO,
	};

	struct VK_GROUP
	{
		DWORD	dwVKey1;
		DWORD	dwVKey2;
	};

	typedef abase::hash_map<int, VK_GROUP> LK2VKMap;

public:		//	Constructor and Destructor

	ECInputFilter(ECInputModule* pInputModule);
	virtual ~ECInputFilter() {}

public:		//	Attributes

public:		//	Operations

	/*	Handle logic key

		lk: logic key id
		bPress: true, key is pressed down; false, key is released up
		dwCtrlKeys: current control key states (EC_KSF_xxx combo flags).
	*/
	virtual void HandleLogicKey(int lk, bool bPress, DWORD dwCtrlkeys) = 0;

	/*	Handle mouse event

		me: mouse event id
		x, y: mouse position in main window's client area
		iWheelDelta: mouse wheel roll delta
		dwCtrlKeys: current control key states (EC_KSF_xxx combo flags).
	*/	
	virtual void HandleMouseEvent(MouseEvent me, int x, int y, int iWheelDelta, DWORD dwCtrlKeys) = 0;

	//	Check whether specified logic key is pressed
	bool IsLogicKeyPressed(int iLogicKey);
	//	Check whether specified logic key is toggled
	bool IsLogicKeyToggled(int iLogicKey);

	//	Get logic key that is coresponding to specified vk
	int FindLogicKey(int vk, DWORD dwCtrlKeys);
	//	Remap logic key
	bool RemapLogicKey(int lk, int vk1, DWORD dwCtrlKeys1, int vk2, DWORD dwCtrlKeys2);
	//	Get logic key info
	bool GetLogicKeyInfo(int lk, int& vk1, DWORD& dwCtrlKeys1, int& vk2, DWORD& dwCtrlKeys2);

protected:	//	Attributes

	static DWORD	m_dwPressMask[NUM_CKCOMBO];

	ECInputModule*	m_pInputModule;		//	Input module

	int			m_VK2LKMap[EC_NUM_VIRTUALKEY][NUM_CKCOMBO];		//	Virtual key to logic key map
	LK2VKMap	m_LK2VKMap;		//	Logic key map to virtual key map

protected:	//	Operations

	//	Translate virtual key to logic key
	int ConvertVKToLK(int vk, DWORD dwCtrlKeys);
	//	Compress virtual key and control key states
	DWORD CompressVK(int vk, DWORD dwCtrlKeys);
	//	Decompress virtual key and control key states
	void DecompressVK(DWORD dwSrc, int& vk, DWORD& dwCtrlKeys);
	//	Register logic key
	void RegisterLogicKey(int lk, int vk1, int iCKCombo1, int vk2, int iCKCombo2);
	//	Clear specified lk-vk map if it exists
	void ClearLogicKeyMap(int lk, int iClearFlag);
	//	Comvert control key states to combo index
	int GetCKComboIndex(DWORD dwCtrlKeys);
	//	Clear all logic keys
	void ClearAllLogicKeys();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


