/*
 * FILE: Game.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/06/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"


CEditor::CEditor()
{
	m_pObjPicked = 0;
	GetOSRMgr().RegisterReceiver(*this);
	m_cmdQueue.SetSelGroup(GetSelGroup());
}

IObjBase* CEditor::CreateObject(const ObjID objType, const POINT& pos)
{
	A3DVECTOR3 vHitPos;
	if (!GetTerrainPosByCursor(pos.x, pos.y, vHitPos))
		return 0;

	IObjBase* pObject = CreateObjectRuntime(objType, vHitPos, false);
	if (0 == pObject)
		return 0;

	IPropObjBase* pProp = pObject->GetProperties();
	pProp->GetFlags().RaiseFlag(OBF_DRAW_BINDING_BOX);
	pProp->GetFlags().ClearFlag(OBF_RUNTIME_ONLY);

	m_cmdQueue.SubmitBeforeExecution(CmdCreateObj(GetLoadPackage()));
	GetStatus().RaiseFlag(GAME_CREATE_AND_PICK_STH);
	GetRayHitOwner().SetRayHit(pObject);
	SetPickedObject(pObject);
	return pObject;
}

IPhysXObjBase* CEditor::CreateObject(const ObjID objType, const char* szFile, const POINT& pos)
{
	IPhysXObjBase* pObject = CreateObjectAndLoad(objType, szFile);
	if (0 == pObject)
		return 0;

	A3DVECTOR3 vHitPos;
	if (GetTerrainPosByCursor(pos.x, pos.y, vHitPos))
		pObject->SetLPos(vHitPos);

	m_cmdQueue.SubmitBeforeExecution(CmdCreateObj(GetLoadPackage()));
	GetStatus().RaiseFlag(GAME_CREATE_AND_PICK_STH);
	GetRayHitOwner().SetRayHit(pObject);
	SetPickedObject(pObject);
	return pObject;
}

void CEditor::RemoveAllSelectedObjects()
{
	CPhysXObjSelGroup& selGroup = GetSelGroup();
	int nCount = selGroup.Size();
	if (0 == nCount)
		return;

	bool bAddToCmdRecorder = true;
	if (0 != GetPickedObject())
	{
		if (GetStatus().ReadFlag(GAME_CREATE_AND_PICK_STH))
		{
			GetStatus().ClearFlag(GAME_CREATE_AND_PICK_STH);
			bAddToCmdRecorder = false;
		}
	}

	if (bAddToCmdRecorder)
	{
		m_cmdQueue.SubmitBeforeExecution(CmdReleaseObj(GetLoadPackage()));
	}
	else
	{
		m_cmdQueue.CancelLastSubmit();
		m_cmdQueue.SubmitBeforeExecution(CmdSelReplace());
	}

	IObjBase* pObject = 0;
	ApxObjBase* pParent = 0;
	std::vector<IObjBase*>  selObjs;
	selGroup.GetTopAncestorObjects(selObjs);
	selGroup.ReplaceWithObject(0);

	size_t nbObjs = selObjs.size();
	for (size_t i = 0; i < nbObjs; ++i)
	{
		pObject = selObjs[i];
		pParent = pObject->GetParent();
		if (0 != pParent)
			pParent->RemoveChild(pObject);
		pObject->ToDie();
	}
}

void CEditor::CloneObjects()
{
	if (!QuerySceneMode(Scene::SCENE_MODE_PLAY_OFF))
		return;

	LoadPackage& lp = GetLoadPackage();
	CPhysXObjSelGroup& selGroup = GetSelGroup();

	int nSelCount = selGroup.Size();
	IObjBase* pObjRayHit = GetRayHitObject();
	if (0 == pObjRayHit)
	{
		if (0 == nSelCount)
			return;
	}

	m_cmdQueue.SubmitBeforeExecution(CmdCreateObj(lp));
	IObjBase* pFocusObj = 0;
	if (0 != nSelCount)
	{
		std::vector<IObjBase*> cloneObjs;
		pFocusObj = m_optClone.CloneObjects(lp, selGroup, cloneObjs);
		selGroup.ReplaceWithObjects(cloneObjs);
	}
	else
	{
		pFocusObj = m_optClone.CloneObject(lp, pObjRayHit);
		selGroup.ReplaceWithObject(pFocusObj);
	}

	if (0 != pFocusObj)
	{
		GetStatus().RaiseFlag(GAME_CREATE_AND_PICK_STH);
		SetPickedObject(pFocusObj, false);
		GetRayHitOwner().SetRayHit(pFocusObj);
	}
}

void CEditor::SaveAnObject(const char* szFilePath)
{
	Scene::SaveAnObject(szFilePath);
}

IObjBase* CEditor::LoadAnObject(const char* szFilePath, const POINT& pos)
{
	A3DVECTOR3 vHitPos;
	if (!GetTerrainPosByCursor(pos.x, pos.y, vHitPos))
		return 0;

	IObjBase* pObject = Scene::LoadAnObject(szFilePath);
	if (0 == pObject)
		return 0;

	IPropObjBase* pProp = pObject->GetProperties();
	pProp->GetFlags().RaiseFlag(OBF_DRAW_BINDING_BOX);
	pProp->GetFlags().ClearFlag(OBF_RUNTIME_ONLY);

	m_cmdQueue.SubmitBeforeExecution(CmdCreateObj(GetLoadPackage()));
	GetStatus().RaiseFlag(GAME_CREATE_AND_PICK_STH);
	GetRayHitOwner().SetRayHit(pObject);
	SetPickedObject(pObject);
	return pObject;
}

void CEditor::SetPickedObject(IObjBase* pObject, bool changeSelGroup)
{
	m_pObjPicked = pObject;
	if (changeSelGroup)
	{
		if (0 != pObject)
		{
			if (!GetStatus().ReadFlag(GAME_CREATE_AND_PICK_STH))
				m_cmdQueue.SubmitBeforeExecution(CmdSelReplace());
			GetSelGroup().ReplaceWithObject(pObject);
		}
	}
}

void CEditor::OnNotifySleep(const IObjBase& obj)
{
	if (m_pObjPicked == &obj)
		SetPickedObject();
}

void CEditor::OnSceneModeChange(const SceneMode& newSM)
{
	SetPickedObject();
	switch(newSM)
	{
	case SCENE_MODE_EMPTY:
		m_cmdQueue.ClearAllCmds();
		break;
	case SCENE_MODE_PLAY_ON:
		m_cmdQueue.AddLabelToCurrentCmd(0x0000FFAA);
		break;
	case SCENE_MODE_PLAY_OFF:
		m_cmdQueue.RollbackToLabel(0x0000FFAA, true);
		break;
	}
}

bool CEditor::OnLButtonDown(const int x, const int y, const unsigned int nFlags)
{
	if (IsRuntime())
		return Scene::OnLButtonDown(x, y, nFlags);

	if (QuerySceneMode(SCENE_MODE_EMPTY))
		return false;

	if (0 != GetPickedObject())
	{
		if (GetStatus().ReadFlag(GAME_CREATE_AND_PICK_STH))
			GetStatus().ClearFlag(GAME_CREATE_AND_PICK_STH);
		else
			m_cmdQueue.SubmitBeforeExecution(CmdPropChange());
		SetPickedObject(0);
		if (MK_SHIFT & nFlags)
			CloneObjects();
	}
	else
	{
		CPhysXObjSelGroup& selGroup = GetSelGroup();
		if (selGroup.UpdateRefFrameOnLButtonDown(x, y, nFlags))
		{
			m_cmdQueue.SubmitBeforeExecution(CmdPropChange());
			return true;
		}

		IObjBase* pObjRayHit = GetRayHitOwner().GetRayHit();
		if (0 != pObjRayHit)
		{
			if (MK_CONTROL & nFlags)
			{
				m_cmdQueue.SubmitBeforeExecution(CmdSelAppend());
				selGroup.AppendObject(*pObjRayHit);
				return true;
			}
			if (MK_SHIFT & nFlags)
			{
				SetPickedObject(pObjRayHit);
				return true;
			}
		}

		if (selGroup.GetFocusObject() != pObjRayHit)
			m_cmdQueue.SubmitBeforeExecution(CmdSelReplace());
		selGroup.ReplaceWithObject(pObjRayHit);
	}
	return true;
}

void CEditor::OnLButtonUp(const int x, const int y, const unsigned int nFlags)
{
	if (IsRuntime())
		Scene::OnLButtonUp(x, y, nFlags);
}

void CEditor::OnMouseMove(const int x, const int y, const unsigned int nFlags)
{
	if (IsRuntime())
	{
		Scene::OnMouseMove(x, y, nFlags);
		return;
	}

	if (QuerySceneMode(SCENE_MODE_EMPTY))
		return;

	CPhysXObjSelGroup& selGroup = GetSelGroup();
	// drag and drop the object's reference frame
	if (MK_LBUTTON & nFlags)
	{
		selGroup.UpdateRefFrameOnMouseMove(x, y, nFlags, false);
		return;
	}
	// move the picked object position 
	IObjBase* pObjPicked = GetPickedObject();
	if (0 != pObjPicked)
	{
		A3DVECTOR3 vHitPos;
		if (GetTerrainPosByCursor(x, y, vHitPos))
		{
			assert(0 != pObjPicked->GetProperties()->GetSelGroup());
			selGroup.SetGroupPos(vHitPos);
		}
		return;
	}
	// hit object detection 
	PhysRayTraceHit hitInfo;
	PhysRay ray;
	ScreenPointToRay(x, y, ray);
	POINT point;
	point.x = x; point.y = y;
	RayTraceObject(point, ray, hitInfo);
	selGroup.UpdateRefFrameOnMouseMove(x, y, nFlags, true);
}

bool CEditor::OnMouseWheel(const int x, const int y, const int zDelta, const unsigned int nFlags)
{
	return Scene::OnMouseWheel(x, y, zDelta, nFlags);
}