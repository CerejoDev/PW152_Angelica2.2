/*
 * FILE: GameProperty.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/10/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_GAMEPROPERTY_H_
#define _APHYSXLAB_GAMEPROPERTY_H_

#include "IPropPhysXObjBase.h"
#include "CoordinateDirection.h"
#include "Command.h"
#include "IPropObjBase.h"
#include "PhysXObjSelGroup.h"
#include "ReportInterface.h"

class CGame;
class CPhysXObjMgr;

class CGameProperty : public APhysXSerializerBase
{
public:
	static APhysXU32 GetVersion() { return 0xAA000002; }

	enum GameMode
	{
		GAME_MODE_EMPTY	   = 0, 
		GAME_MODE_EDIT     = 1,
//		GAME_MODE_SIMULATE = 2,	 // simulate physics...
		GAME_MODE_PLAY     = 3,  // there will be an avatar run in the game...
		GAME_MODE_SIMULATE  = GAME_MODE_PLAY  // Deprecated. replaced by play mode
	};

	enum ShootMode
	{
		SM_EYE_TO_CENTER = 0,
		SM_EYE_TO_MOUSE	,
		SM_MOUSE_FALLING,
		SM_FRONT_FALLING,
	//	SM_KINEMATIC,
		SM_END_FLAG               // this is a flag. No function defined.
	};

	enum ShootStuffID
	{
		SSI_SPHERE = 0,
		SSI_BOX,
		SSI_BARREL,
	//	SSI_BREAKABLEBOX,
	//	SSI_BOMB,
		SSI_END_BOUND
	};

public:
	typedef CCoordinateDirection::EditType EditType;
	typedef IPropPhysXObjBase::DrivenMode   DrivenMode;

	CGameProperty(CGame& game, CPhysXObjMgr& objMgr);
	bool IsReady() const { return m_status.ReadFlag(GAME_INIT_ENDOK); }

	bool SaveToFile(NxStream& nxStream, A3DCamera& camera);
	bool LoadFromFile(NxStream& nxStream, A3DCamera& camera, bool& outIsLowVersion);
	const char* GetActiveSceneFile() const;

	bool IsRuntime() const;
	void SetGameMode(const GameMode& gm);
	bool QueryGameMode(const GameMode& gm) const;
	const TCHAR* GetGameModeDesc(bool bIsFullDesc = false) const;

	const char* GetTerrainFile() const { return m_strTrnFile; }
	void EnableShowTerrianGrid(const bool bEnable);
	bool IsShowTerrianGridEnabled() const { return m_status.ReadFlag(GAME_DRAW_TERRAIN_GRID); }

	void EnablePhysXProfile(const bool bEnable);
	bool IsPhysXProfileEnabled() const { return m_status.ReadFlag(GAME_DRAW_PHYSX_PRIFILE); }

	void EnablePhysXDebugRender(const bool bEnable);
	bool IsPhysXDebugRenderEnabled() const { return m_status.ReadFlag(GAME_DRAW_PHYSX_DBGRENDER); }

	void EnablePhysXHW(const bool bEnable);
	bool IsPhysHWEnabled() const { return m_status.ReadFlag(GAME_ENABLE_PHYSX_HW); }

	bool PhysXSimASingleStep();
	void ChangePhysXPauseState();
	bool IsPhysXSimPauseState() const { return m_status.ReadFlag(GAME_IS_PHYSX_SIM_PAUSE); }

	void	   SetDrivenMode(const DrivenMode& dm);
	DrivenMode GetDrivenMode() const;
	bool	   QueryDrivenMode(const DrivenMode& dm) const;

	void	   SetRefFrameEditMode(const EditType& et);
	EditType   GetRefFrameEditMode() const;
	bool	   QueryRefFrameEditMode(const EditType& et) const { return et == GetRefFrameEditMode(); }

	ShootStuffID GetShootType() const { return m_iSSID; }
	ShootMode GetShootMode() const { return m_iSMode; }
	void ChangeShootStuffType(int shootID = -1);
	void ChangeShootMode(int shootMode = -1);

	CmdQueue& GetCmdQueue() { return m_cmdQueue; }
	CPhysXObjSelGroup& GetSelGroup() { return m_SelGroup; }
	ObjStateReportMgr& GetOSRMgr() { return m_OSRMgr; }
	IObjBase*	GetPickedObject() const { return m_pObjPicked; }
	IObjBase*	GetRayHitObject(CPoint* poutPT = 0) const {	return m_RHOwner.GetRayHit(poutPT); }
	bool GetHitObjectFileName(AString& outstrFilePath, CPoint* poutPT = 0);

	const POINT& GetLastMousePos() const { return m_ptLastMousePos; }
	void SetLastMousePos(const CPoint& pt);

private:
	enum GameFlag
	{
		GAME_INIT_START				= (1<<0),
		GAME_INIT_ENDOK				= (1<<1),
		GAME_DRAW_TERRAIN_GRID		= (1<<2),
		GAME_DRAW_PHYSX_PRIFILE		= (1<<3),
		GAME_DRAW_PHYSX_DBGRENDER	= (1<<4),
		GAME_ENABLE_PHYSX_HW		= (1<<5),
		GAME_IS_PHYSX_SIM_PAUSE		= (1<<6),
		GAME_IS_PHYSX_SIM_STEP		= (1<<7),
		GAME_IS_PICK_PHYSX_STH		= (1<<8),
		GAME_CREATE_AND_PICK_STH	= (1<<9),
	};

	struct GameModeDesc 
	{
		const GameMode	iState;
		const TCHAR*	pDesc;
		const TCHAR*	pFullDesc;
	};

	static const GameModeDesc gGMDesc[];

private:
	friend class CGame;

	void SetAPhysXInfo(APhysXEngine& engine, APhysXScene& scene);
	void SetTerrainFileName(const char* pName);
	void SetActiveSceneFile(const char* pPath);

	void SetPickedObject(IObjBase* pObject = 0, bool changeSelGroup = true);
	void SetRayHitObject(IObjBase* pObject = 0, const CPoint& pt = CPoint());
	bool IsPhysXSimEnable();

private:
	CGame*		  m_pGame;
	CPhysXObjMgr* m_pObjMgr;

	APhysXEngine* m_pPhysXEngine;
	APhysXScene*  m_pPhysXScene;
	CFlagMgr<GameFlag> m_status;

	mutable const GameModeDesc* m_pGMD;
	DrivenMode m_iDrivenMode;

	IObjBase* m_pObjPicked;
	POINT m_ptLastMousePos;
	CmdQueue		  m_cmdQueue;
	RayHitOwner		  m_RHOwner;
	CPhysXObjSelGroup m_SelGroup;
	ObjStateReportMgr m_OSRMgr;

	ShootMode	 m_iSMode;
	ShootStuffID m_iSSID;

	AString m_strTrnFile;
	AString m_strActiveScene;
};

#endif