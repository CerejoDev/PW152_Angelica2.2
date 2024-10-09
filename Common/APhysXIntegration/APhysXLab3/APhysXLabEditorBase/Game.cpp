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

	std::vector<IObjBase*>  selObjs;
	selObjs.reserve(nCount);
	IObjBase* pObject = 0;
	for (int i = 0; i < nCount; ++i)
	{
		pObject = selGroup.GetObject(i);
		assert(0 != pObject);
		selObjs.push_back(pObject);
	}

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

	for (int i = 0; i < nCount; ++i)
		selObjs[i]->ToDie();
	selGroup.ReplaceWithObject(0);
}

void CEditor::CloneObjects()
{
	if (!QuerySceneMode(Scene::SCENE_MODE_PLAY_OFF))
		return;

	IObjBase* pObjRayHit = GetRayHitObject();
	if (0 == pObjRayHit)
		return;

	IPropObjBase* pHitObjProp = pObjRayHit->GetProperties();
	CPhysXObjSelGroup* pSelGroup = pHitObjProp->GetSelGroup();
	std::vector<IObjBase*> sampleObjs;
	if (0 != pSelGroup)
		pSelGroup->GetAllObjects(sampleObjs);
	else
		sampleObjs.push_back(pObjRayHit);
	m_cmdQueue.SubmitBeforeExecution(CmdCreateObj(GetLoadPackage()));

	IObjBase* pCloneObj = 0;
	std::vector<IObjBase*> cloneObjs;
	const size_t nCount = sampleObjs.size();
	for (unsigned int i = 0; i < nCount; ++i)
	{
		pCloneObj = dynamic_cast<IObjBase*>(sampleObjs[i]->GetCloned());
		if (0 == pCloneObj)
		{
			a_LogOutput(1, "Warning in %s: Clone object fail!", __FUNCTION__);
			continue;
		}
		if (!pCloneObj->WakeUp(GetLoadPackage()))
		{
			a_LogOutput(1, "Warning in %s: Wake up the cloned object fail!", __FUNCTION__);
			ObjManager::GetInstance()->ReleaseObject(pCloneObj);
			continue;
		}
		cloneObjs.push_back(pCloneObj);
	}

	if (0 < cloneObjs.size())
	{
		IObjBase* pNewObj = cloneObjs[0];
		GetStatus().RaiseFlag(GAME_CREATE_AND_PICK_STH);
		SetPickedObject(pNewObj, false);
		GetRayHitOwner().SetRayHit(pNewObj);
	}
	GetSelGroup().ReplaceWithObjects(cloneObjs);
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
	if (SCENE_MODE_EMPTY == newSM)
		m_cmdQueue.ClearAllCmds();
}

void CEditor::OnLButtonDown(const int x, const int y, const unsigned int nFlags)
{
	if (IsRuntime())
	{
		Scene::OnLButtonDown(x, y, nFlags);
		return;
	}

	if (QuerySceneMode(SCENE_MODE_EMPTY))
		return;

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
			return;
		}

		IObjBase* pObjRayHit = GetRayHitOwner().GetRayHit();
		if (0 != pObjRayHit)
		{
			if (MK_CONTROL & nFlags)
			{
				m_cmdQueue.SubmitBeforeExecution(CmdSelAppend());
				selGroup.AppendObject(*pObjRayHit);
				return;
			}
			if (MK_SHIFT & nFlags)
			{
				SetPickedObject(pObjRayHit);
				return;
			}
		}

		if (selGroup.GetFocusObject() != pObjRayHit)
			m_cmdQueue.SubmitBeforeExecution(CmdSelReplace());
		selGroup.ReplaceWithObject(pObjRayHit);
	}
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
	PhysRay ray = ScreenPosToRay(*GetRender()->GetViewport(), x, y);
	POINT point;
	point.x = x; point.y = y;
	if (RayTraceObject(point, ray, hitInfo))
		selGroup.UpdateRefFrameOnMouseMove(x, y, nFlags, true);
}

bool CEditor::OnMouseWheel(const int x, const int y, const int zDelta, const unsigned int nFlags)
{
	return Scene::OnMouseWheel(x, y, zDelta, nFlags);
}