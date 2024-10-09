/*
 * FILE: ECInputModule.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/4/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <AExpDecl.h>
#include <AStack.h>
#include <APoint.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Flags of SHIFT, CONTROL, ALT key states
#define EC_KSF_SHIFT		0x0001
#define EC_KSF_CONTROL		0x0002
#define EC_KSF_ALT			0x0004
#define EC_KSF_CTRLKEYS		0x0007
#define EC_KSF_FIRSTPRESS	0x0100

#define EC_NUM_VIRTUALKEY	256		//	Number of virtual key

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class ECInputFilter;
class ECPlatform;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECInputModule
//	
///////////////////////////////////////////////////////////////////////////

class _AEXP_DECL ECInputModule
{
public:		//	Types

	//	Mouse press information
	struct _AEXP_DECL PRESS
	{
		int		iPressed;		//	Pressed flag, 0, not pressed; 1, pressed by single click; 2, pressed by double clicks
		DWORD	dwPressTime;	//	Start pressing time stamp
		APointI	ptPress;		//	Press position
		APointI	ptAccuOff;		//	Accumulate offset after button is pressed

		PRESS();
		bool Pressed(LPARAM lParam, DWORD dwTime, bool bDblClick);
		bool Released();
	};

	friend class ECInputFilter;

public:	//	Constructor and Destructor

	ECInputModule();
	virtual ~ECInputModule();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(ECPlatform* pPlatform);
	//	Release object
	void Release();

	//	SHIFT is pressed ?
	static bool IsShiftPressed(DWORD dwCtrlFlags) { return (dwCtrlFlags & EC_KSF_SHIFT) ? true : false; }
	//	CONTROL is pressed ?
	static bool IsCtrlPressed(DWORD dwCtrlFlags) { return (dwCtrlFlags & EC_KSF_CONTROL) ? true : false; }
	//	ALT is pressed ?
	static bool IsAltPressed(DWORD dwCtrlFlags) { return (dwCtrlFlags & EC_KSF_ALT) ? true : false; }
	//	Is first pressed ?
	static bool IsFirstPressed(DWORD dwCtrlFlags) { return (dwCtrlFlags & EC_KSF_FIRSTPRESS) ? true : false; }

	//	Tick routine
	bool Tick();
	//	Deal windows message, return true if this message is processed
	bool DealWindowsMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, DWORD dwTime);

	//	Get current input filter
	ECInputFilter* GetCurFilter() { return m_pCurFilter; }

	// Set current input filter
	void SetCurFilter(ECInputFilter* pInputFilter) { m_pCurFilter = pInputFilter;}

	//	Capture mouse
	void CaptureMouse(bool bCapture);
	//	Get mouse current position in client area
	void GetMousePos(int* pix, int* piy);
	//	Set mouse cursor position
	//	x, y: position in client area
	void SetMousePos(int x, int y);
	//	Is mouse captured by us ?
	bool IsMouseCaptured();
	//	Clear mouse capture counter
	void ClearMouseCaptureCnt() { m_iMouCapCnt = 0; }
	//	Show mouse cursor
	void ShowCursor(bool bShow);
	//	Is mouse cursor shown ?
	bool IsCursorShown();
	//	Check if input controller occupy mouse message
	bool IsMouseOccupied();

	//	Is virtual Key being pressed ?
	bool KeyIsBeingPressed(int iVirtualKey);
	//	Is virtual key being toggled ?
	bool KeyIsBeingToggled(int iVirtualKey);
	//	Get current control key states
	//	Return: EC_KSF_xxx combo flags
	DWORD GetCtrlKeyStates();

	//	Get mouse button pressing info
	const PRESS& GetLBtnPressinfo() { return m_LBPress; }
	const PRESS& GetRBtnPressInfo() { return m_RBPress; }

protected:	//	Attributes

	ECPlatform* m_pPlatform;		//	Platform object

	bool		m_aKeyPress[EC_NUM_VIRTUALKEY];	//	Key pressing states

	int			m_iMouCapCnt;		//	Mouse capture counter
	int			m_iMouShowCnt;		//	Mouse cursor show counter
	DWORD		m_dwCtrlFlags;		//	Control key flags
	PRESS		m_LBPress;			//	Left button press information
	PRESS		m_RBPress;			//	Right button press information

	ECInputFilter*	m_pCurFilter;	//	Common input filter

protected:	//	Operations

	//	Handle key message passed to game
	bool HandleKeyMessage(UINT message, WPARAM wParam, LPARAM lParam, DWORD dwMsgTime);
	//	Handle mouse message passed to game
	bool HandleMouseMessage(UINT message, WPARAM wParam, LPARAM lParam, DWORD dwMsgTime);

	//	Refresh key press state
	void RefreshKeyPressStates();
	//	Check if a click event should occur
	//	x, y: position in main window's client area
	bool GenClickEvent(int x, int y, DWORD dwTime, const PRESS& press);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



