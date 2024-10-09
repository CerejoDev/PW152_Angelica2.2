/*
 * FILE: Command.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/02/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */
#include "stdafx.h"
#include "Command.h"
#include <algorithm>

CmdContext::ObjInfo::ObjInfo(IObjBase& obj)
{
	m_pObj = &obj;
	m_pProp = 0;
	m_pParent = 0;
}

CmdContext::ObjInfo::~ObjInfo()
{
	delete m_pProp;
	m_pProp = 0;
}

bool CmdContext::ObjInfo::SaveAllProps(bool bToSavePaChs, bool bToSaveProps)
{
	if (SaveMyselfProp(bToSavePaChs, bToSaveProps))
	{
		ObjContainer::iterator it = m_Children.begin();
		ObjContainer::iterator itEnd = m_Children.end();
		for (; it != itEnd; ++it)
		{
			if (!it->SaveAllProps(bToSavePaChs, bToSaveProps))
			{
				m_Children.clear();
				return false;
			}
		}
		return true;
	}
	return false;
}

void CmdContext::ObjInfo::LoadAllProps()
{
	LoadMyselfProp();

	ObjContainer::iterator it = m_Children.begin();
	ObjContainer::iterator itEnd = m_Children.end();
	for (; it != itEnd; ++it)
		it->LoadAllProps();
}

void CmdContext::ObjInfo::LoadParent() const
{
	if (0 != m_pParent)
		m_pParent->AddChild(m_pObj);

	ObjContainer::const_iterator it = m_Children.begin();
	ObjContainer::const_iterator itEnd = m_Children.end();
	for (; it != itEnd; ++it)
		it->LoadParent();
}

bool CmdContext::ObjInfo::SaveMyselfProp(bool bToSavePaChs, bool bToSaveProps)
{
	if (bToSavePaChs)
		m_pParent = m_pObj->GetParent();

	if (bToSaveProps)
	{
		if (0 != m_pProp)
		{
			assert(!"Shouldn't be here! Debug it!");
			delete m_pProp;
			m_pProp = 0;
		}
		m_pProp = m_pObj->CreatePropSnap();
		if (0 == m_pProp)
			return false;
	}

	if (0 == m_Children.size())
	{
		int nChildren = m_pObj->GetChildNum();
		m_Children.reserve(nChildren);
		for (int i = 0; i < nChildren; ++i)
		{
			IObjBase* pObj = dynamic_cast<IObjBase*>(m_pObj->GetChild(i));
			assert(0 != pObj);
			m_Children.push_back(ObjInfo(*pObj));
		}
	}
	return true;
}

void CmdContext::ObjInfo::LoadMyselfProp()
{
	if (0 != m_pProp)
	{
		*(m_pObj->GetProperties()) = *m_pProp;
		A3DMATRIX4 pose = m_pProp->GetPose();
		m_pObj->SetGPose(pose);
		float scale = m_pProp->GetScale();
		m_pObj->SetGScale(scale);
	}
	if (0 != m_pParent)
		m_pParent->AddChild(m_pObj);
}

CmdContext::CmdInfo::CmdInfo(const ICommandEditor* pCmd)
{
	m_pCmd = pCmd;
	m_Label = -1;
	m_nSavedType = 0;
}

CmdContext::CmdInfo::~CmdInfo()
{
	delete m_pCmd;
	m_pCmd = 0;
	m_Label = -1;
	m_nSavedType = 0;
}

bool CmdContext::CmdInfo::SaveInfo(bool bToSavePaChs, bool bToSaveProps)
{
	ObjContainer::iterator it = m_info.begin();
	ObjContainer::iterator itEnd = m_info.end();
	for (; it != itEnd; ++it)
	{
		if (!it->SaveAllProps(bToSavePaChs, bToSaveProps))
			return false;
	}

	if (bToSavePaChs)
		m_nSavedType |= ICommandEditor::IT_PARENT_CHILD;
	if (bToSaveProps)
		m_nSavedType |= ICommandEditor::IT_PROPERTIES;
	return true;
}

void CmdContext::CmdInfo::LoadProps()
{
	ObjContainer::iterator it = m_info.begin();
	ObjContainer::iterator itEnd = m_info.end();
	for (; it != itEnd; ++it)
		it->LoadAllProps();
}

bool CmdContext::CmdInfo::IsEqual(const CPhysXObjSelGroup& selGroup) const
{
	const int nSelCount = selGroup.Size();
	size_t nAll = m_SelSequence.size();
	if (nAll != nSelCount)
	{
		assert(!"Shouldn't be here! Debug it!");
		return false;
	}

	IObjBase* pSel = 0;
	for (int i = 0; i < nSelCount; ++i)
	{
		pSel = selGroup.GetObject(i);
		if (pSel != m_SelSequence[i])
		{
			assert(!"Shouldn't be here! Debug it!");
			return false;
		}
	}
	return true;
}

int CmdContext::CmdInfo::GetTargetObjects(const CPhysXObjSelGroup& selGroup, std::vector<IObjBase*>& outObjs) const
{
	int nRtn = 0;
	if (0 != m_pCmd)
	{
		nRtn = m_pCmd->GetTargetObjects(outObjs);
		if (0 < nRtn)
			return nRtn;
	}
	nRtn = selGroup.GetTopAncestorObjects(outObjs);
	return nRtn;
}

bool CmdContext::CmdInfo::SaveTargetObjInfo(int nImpacType, const CPhysXObjSelGroup& selGroup)
{
	int nAllImpact = nImpacType;
	if (0 != m_pCmd)
		nAllImpact |= m_pCmd->GetImpactType();
	if (ICommandEditor::IT_SELGROUP & nAllImpact)
		SaveSelSequence(selGroup);

	bool bToSavePaChs = false;
	bool bToSaveProps = false;
	if (ICommandEditor::IT_PARENT_CHILD & nAllImpact)
	{
		if (!(ICommandEditor::IT_PARENT_CHILD & m_nSavedType))
			bToSavePaChs = true;
	}
	if (ICommandEditor::IT_PROPERTIES & nAllImpact)
	{
		if (!(ICommandEditor::IT_PROPERTIES & m_nSavedType))
			bToSaveProps = true;
	}
	if (!bToSavePaChs && !bToSaveProps)
		return true;

	bool bIsEmptyInfo = false;
	const size_t nInfoCount = m_info.size();
	if (0 == nInfoCount)
		bIsEmptyInfo = true;

	if (!bIsEmptyInfo)
	{
		std::vector<IObjBase*> selObjs;
		const int nSelCount = GetTargetObjects(selGroup, selObjs);
		bool bHasError = false;
		if (nInfoCount != nSelCount)
		{
			assert(!"Shouldn't be here! Debug it!");
			bHasError = true;
		}
		else
		{
			for (int i = 0; i < nSelCount; ++i)
			{
				if (selObjs[i] != &(m_info[i].GetObj()))
				{
					assert(!"Shouldn't be here! Debug it!");
					bHasError = true;
					break;
				}
			}
		}
		if (bHasError)
		{
			bIsEmptyInfo = true;
			m_info.clear();
			if (ICommandEditor::IT_PARENT_CHILD & m_nSavedType)
			{
				m_nSavedType &= ~ICommandEditor::IT_PARENT_CHILD;
				nAllImpact |= ICommandEditor::IT_PARENT_CHILD;
			}
			if (ICommandEditor::IT_PROPERTIES & m_nSavedType)
			{
				m_nSavedType &= ~ICommandEditor::IT_PROPERTIES;
				nAllImpact |= ICommandEditor::IT_PROPERTIES;
			}
		}
	}

	if (bIsEmptyInfo)
	{
		std::vector<IObjBase*> selObjs;
		const int nSelCount = GetTargetObjects(selGroup, selObjs);
		m_info.reserve(nSelCount);
		for (int i = 0; i < nSelCount; ++i)
		{
			IObjBase* pObj = selObjs[i];
			assert(0 != pObj);
			m_info.push_back(ObjInfo(*pObj));
		}
	}
	return SaveInfo(bToSavePaChs, bToSaveProps);
}

void CmdContext::CmdInfo::LoadTargetObjInfo(CPhysXObjSelGroup& selGroup)
{
	LoadProps();
	if (ICommandEditor::IT_SELGROUP & m_nSavedType)
		selGroup.ReplaceWithObjects(m_SelSequence);

	if (0 <selGroup.Size())
		selGroup.RefreshFrame();
}

void CmdContext::CmdInfo::SaveSelSequence(const CPhysXObjSelGroup& selGroup)
{
	if (ICommandEditor::IT_SELGROUP & m_nSavedType)
	{
		assert(true == IsEqual(selGroup));
		return;
	}

	assert(0 == m_SelSequence.size());
	const int nSelCount = selGroup.Size();
	m_SelSequence.reserve(nSelCount);
	for (int i = 0; i < nSelCount; ++i)
	{
		IObjBase* pObj = selGroup.GetObject(i);
		assert(0 != pObj);
		m_SelSequence.push_back(pObj);
	}
	m_nSavedType |= ICommandEditor::IT_SELGROUP;
}

void CmdContext::CmdInfo::UndoNotifyPreBackward() const
{
	if (0 == m_pCmd)
		return;

	ObjContainer::const_reverse_iterator it = m_info.rbegin();
	ObjContainer::const_reverse_iterator itEnd = m_info.rend();
	for (; it != itEnd; ++it)
		m_pCmd->UndoNotifyObjsCmdStartExecution(it->GetObj(), it->GetProp());
}

void CmdContext::CmdInfo::UndoNotifyPostBackward(const ICommandEditor& theUndoCmd) const
{
	ObjContainer::const_reverse_iterator it = m_info.rbegin();
	ObjContainer::const_reverse_iterator itEnd = m_info.rend();
	for (; it != itEnd; ++it)
	{
		if (ICommandEditor::IT_PARENT_CHILD & m_nSavedType)
			it->LoadParent();
		theUndoCmd.UndoNotifyObjsCmdEndExecution(it->GetObj(), it->GetProp());
	}
}

void CmdContext::CmdInfo::RedoNotifyPreForward(const ICommandEditor& theRedoCmd) const
{
	ObjContainer::const_iterator it = m_info.begin();
	ObjContainer::const_iterator itEnd = m_info.end();
	for (; it != itEnd; ++it)
		theRedoCmd.RedoNotifyObjsCmdStartExetution(it->GetObj(), it->GetProp());
}

void CmdContext::CmdInfo::RedoNotifyPostForward() const
{
	if (0 == m_pCmd)
		return;

	ObjContainer::const_iterator it = m_info.begin();
	ObjContainer::const_iterator itEnd = m_info.end();
	for (; it != itEnd; ++it)
		m_pCmd->RedoNotifyObjsCmdEndExecution(it->GetObj(), it->GetProp());
}

CmdContext::CmdContext()
{
	CmdInfo* pCmd = new CmdInfo(0);
	m_theContexts.push_back(pCmd);
	m_itCurCmd = m_theContexts.begin();
}

CmdContext::~CmdContext()
{
	ReleaseToEnd(m_theContexts.begin());
}

void CmdContext::ReleaseToEnd(CISItor& itFrom)
{
	for_each(itFrom, m_theContexts.end(), DeletePtr<CC_Elem_Type>());
	m_theContexts.erase(itFrom, m_theContexts.end());
}

bool CmdContext::HasBackwardContext() const
{
	if (m_theContexts.begin() == m_itCurCmd)
		return false;
	return true;
}

bool CmdContext::HasForwardContext() const
{
	CISItor itTemp = m_itCurCmd;
	++itTemp;
	if (m_theContexts.end() == itTemp)
		return false;
	return true;
}

bool CmdContext::SaveCurrentContext(const ICommandEditor& cmd, const CPhysXObjSelGroup& selGroup)
{
	CmdInfo* pCmdInfo = *m_itCurCmd;
	assert(0 != pCmdInfo);
	if (!pCmdInfo->SaveTargetObjInfo(cmd.GetImpactType(), selGroup))
		return false;

	pCmdInfo = new CmdInfo(&cmd);
	if (0 == pCmdInfo)
		return false;

	m_theContexts.push_back(pCmdInfo);
	++m_itCurCmd;
	return true;
}

bool CmdContext::LoadForwardContext(CPhysXObjSelGroup& selGroup)
{
	if (!HasForwardContext())
	{
		assert(!"Shouldn't be here! Debug it!");
		return false;
	}

	CmdInfo* pLastCmdInfo = *m_itCurCmd;

	++m_itCurCmd;
	CmdInfo* pRedoCmdInfo = *m_itCurCmd;
	assert(0 != pRedoCmdInfo);

	pLastCmdInfo->RedoNotifyPreForward(*pRedoCmdInfo->GetCmd());
	pRedoCmdInfo->RedoNotifyPostForward();
	pRedoCmdInfo->LoadTargetObjInfo(selGroup);
	return true;
}

bool CmdContext::LoadBackwardContext(CPhysXObjSelGroup& selGroup)
{
	if (!HasBackwardContext())
	{
		assert(!"Shouldn't be here! Debug it!");
		return false;
	}

	if (!HasForwardContext())
	{
		CmdInfo* pCmdInfo = *m_itCurCmd;
		assert(0 != pCmdInfo);
		if (!pCmdInfo->SaveTargetObjInfo(0, selGroup))
			return false;
	}

	CmdInfo* pTheUndoCmdInfo = *m_itCurCmd;
	pTheUndoCmdInfo->UndoNotifyPreBackward();

	--m_itCurCmd;
	CmdInfo* pLastCmdInfo = *m_itCurCmd;
	assert(0 != pLastCmdInfo);

	const ICommandEditor* pTheUndoCmd = pTheUndoCmdInfo->GetCmd();
	if (0 != pTheUndoCmd)
		pLastCmdInfo->UndoNotifyPostBackward(*pTheUndoCmd);

	pLastCmdInfo->LoadTargetObjInfo(selGroup);
	return true;
}

void CmdContext::AbandonContext()
{
	CISItor itTemp = m_itCurCmd;
	++itTemp;
	ReleaseToEnd(itTemp);
}

void CmdContext::ClearAllContext()
{
	m_itCurCmd = m_theContexts.begin();
	CISItor itTemp = m_itCurCmd;
	++itTemp;
	if (itTemp == m_theContexts.end())
		return;
	ReleaseToEnd(itTemp);
}

void CmdContext::RemoveLastSubmitContext(CPhysXObjSelGroup& selGroup)
{
	if (HasForwardContext())
		return;

	if (m_itCurCmd == m_theContexts.begin())
		return;

	--m_itCurCmd;
	AbandonContext();

	CmdInfo* pCmdInfo = *m_itCurCmd;
	assert(0 != pCmdInfo);
	pCmdInfo->LoadTargetObjInfo(selGroup);
}

void CmdContext::AddLabel(int nLabel)
{
	(*m_itCurCmd)->SetLabel(nLabel);
}

void CmdContext::Rollback(int nLabel, bool bRemoveLabel, CPhysXObjSelGroup& selGroup)
{
	if (nLabel == (*m_itCurCmd)->GetLabel())
	{
		if (bRemoveLabel)
			(*m_itCurCmd)->SetLabel(-1);
		return;
	}

	CISItor itTemp = m_itCurCmd;
	CISItor itTarget = m_theContexts.end();
	CISItor itBegin = m_theContexts.begin();
	while (itTemp != itBegin)
	{
		if (nLabel == (*itTemp)->GetLabel())
		{
			itTarget = itTemp;
			break;
		}
		--itTemp;
	}
	if (itTemp == itBegin)
	{
		if (nLabel == (*itTemp)->GetLabel())
		{
			itTarget = itTemp;
		}
		else
		{
			assert(itTarget == m_theContexts.end());
			return;
		}
	}

	assert(itTarget != m_theContexts.end());
	while (itTarget != m_itCurCmd)
	{
		LoadBackwardContext(selGroup);
	}
	assert(nLabel == (*m_itCurCmd)->GetLabel());
	if (bRemoveLabel)
		(*m_itCurCmd)->SetLabel(-1);
	AbandonContext();
}

CmdQueue::CmdQueue()
{
	m_pSelGroup = 0;
}

bool CmdQueue::UndoEnable() const
{
	if (0 == m_pSelGroup)
		return false;
	if (!m_context.HasBackwardContext())
		return false;
	return true;
}

bool CmdQueue::RedoEnable() const
{
	if (0 == m_pSelGroup)
		return false;
	if (!m_context.HasForwardContext())
		return false;
	return true;
}

void CmdQueue::Undo()
{
	if (!UndoEnable())
		return;
	m_context.LoadBackwardContext(*m_pSelGroup);
}

void CmdQueue::Redo()
{
	if (!RedoEnable())
		return;
	m_context.LoadForwardContext(*m_pSelGroup);
}

void CmdQueue::AddLabelToCurrentCmd(int nLabel)
{
	m_context.AddLabel(nLabel);
}

void CmdQueue::RollbackToLabel(int nLabel, bool bRemoveLabel)
{
	m_context.Rollback(nLabel, bRemoveLabel, *m_pSelGroup);
}

bool CmdQueue::SubmitBeforeExecution(const ICommandEditor& cmd)
{		
	if (0 == m_pSelGroup)
		return false;

	ICommandEditor* pCmd = cmd.Clone();
	if (0 == pCmd)
		return false;

	if (RedoEnable())
		m_context.AbandonContext();

	return m_context.SaveCurrentContext(*pCmd, *m_pSelGroup);
}

void CmdQueue::CancelLastSubmit()
{
	m_context.RemoveLastSubmitContext(*m_pSelGroup);
}

void CmdQueue::ClearAllCmds()
{
	m_context.ClearAllContext();
}

void CmdCreateObj::UndoNotifyObjsCmdStartExecution(IObjBase& obj, IPropObjBase* pCmdProp) const
{
	obj.ToDie();
}

void CmdCreateObj::RedoNotifyObjsCmdEndExecution(IObjBase& obj, IPropObjBase* pCmdProp) const
{
	obj.Revive(m_eu);
}

void CmdReleaseObj::UndoNotifyObjsCmdEndExecution(IObjBase& obj, IPropObjBase* pCmdProp) const
{
	obj.Revive(m_eu);
}

void CmdReleaseObj::RedoNotifyObjsCmdStartExetution(IObjBase& obj, IPropObjBase* pCmdProp) const
{
	obj.ToDie();
}

void CmdParentChange::UndoNotifyObjsCmdStartExecution(IObjBase& obj, IPropObjBase* pCmdProp) const
{
	ApxObjBase* pParent = obj.GetParent();
	if (0 != pParent)
		pParent->RemoveChild(&obj);
}
