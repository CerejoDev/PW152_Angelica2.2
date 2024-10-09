/*
 * FILE: Game.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/06/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_GAME_H_
#define _APHYSXLAB_GAME_H_

#include "Scene.h"
#include "Command.h"
#include "CloneOperation.h"

class CEditor : public Scene, private IObjStateReport
{
public:
	CEditor();

	CmdQueue& GetCmdQueue() { return m_cmdQueue; };
	CloneOpt& GetCloneOpt() { return m_optClone; };
	IObjBase* GetPickedObject() const { return m_pObjPicked; }

	IObjBase*		CreateObject(const ObjID objType, const POINT& pos);
	IPhysXObjBase*	CreateObject(const ObjID objType, const char* szFile, const POINT& pos);
	void			RemoveAllSelectedObjects();
	void			CloneObjects();

	void SaveAnObject(const char* szFilePath);
	IObjBase* LoadAnObject(const char* szFilePath, const POINT& pos);

	virtual bool OnLButtonDown(const int x, const int y, const unsigned int nFlags);
	virtual void OnLButtonUp(const int x, const int y, const unsigned int nFlags);
	virtual void OnMouseMove(const int x, const int y, const unsigned int nFlags);
	virtual bool OnMouseWheel(const int x, const int y, const int zDelta, const unsigned int nFlags);

private:
	void SetPickedObject(IObjBase* pObject = 0, bool changeSelGroup = true);

	virtual void OnNotifySleep(const IObjBase& obj);
	virtual void OnSceneModeChange(const SceneMode& newSM);

private:
	IObjBase*  m_pObjPicked;
	CmdQueue   m_cmdQueue;
	CloneOpt   m_optClone;
};

#endif