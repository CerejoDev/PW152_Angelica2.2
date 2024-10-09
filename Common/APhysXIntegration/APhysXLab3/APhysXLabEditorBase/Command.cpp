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
}

CmdContext::ObjInfo::~ObjInfo()
{
	delete m_pProp;
	m_pProp = 0;
}

bool CmdContext::ObjInfo::SaveProp()
{
	if (0 != m_pProp)
		return true;
	m_pProp = m_pObj->CreatePropSnap();
	if (0 != m_pProp)
		return true;
	return false;
}

void CmdContext::ObjInfo::LoadProp()
{
	if (0 != m_pProp)
		*(m_pObj->GetProperties()) = *m_pProp;
}

CmdContext::CmdInfo::CmdInfo(const ICommandEditor* pCmd)
{
	m_pCmd = pCmd;
}

CmdContext::CmdInfo::~CmdInfo()
{
	delete m_pCmd;
}

bool CmdContext::CmdInfo::IsEqual(const CPhysXObjSelGroup& selGroup) const
{
	IObjBase* pSel = 0;
	IObjBase* pInfo = 0;
	const int nSelCount = selGroup.Size();
	for (int i = 0; i < nSelCount; ++i)
	{
		pSel = selGroup.GetObject(i);
		pInfo = &(m_info[i].GetObj());
		if (pSel != pInfo)
		{
			assert(!"Shouldn't be here! Debug it!");
			return false;
		}
	}
	return true;
}

bool CmdContext::CmdInfo::SaveTargetObjInfo(bool bSaveProp, const CPhysXObjSelGroup& selGroup)
{
	bool bsp = bSaveProp;
	if (!bsp)
	{
		if (0 != m_pCmd)
		{
			if (m_pCmd->IsImpactSelObjProps())
				bsp = true;
		}
	}

	const size_t nSelCount = selGroup.Size();
	const size_t nInfoCount = m_info.size();
	if (nInfoCount == nSelCount)
	{
		assert(true == IsEqual(selGroup));
		if (bsp)
			return SaveProps();
		return true;
	}

	if (0 != nInfoCount)
	{
		assert(!"Shouldn't be here! Debug it!");
		m_info.clear();
	}

	m_info.reserve(nSelCount);
	for (unsigned int i = 0; i < nSelCount; ++i)
	{
		IObjBase* pObj = selGroup.GetObject(i);
		assert(0 != pObj);
		m_info.push_back(ObjInfo(*pObj));
	}

	if (bsp)
		return SaveProps();
	return true;
}

void CmdContext::CmdInfo::LoadTargetObjInfo(CPhysXObjSelGroup& selGroup)
{
	LoadProps();
	selGroup.ReplaceWithObject(0);

	size_t nCount = m_info.size();
	if (0 == nCount)
		return;

	std::vector<IObjBase*> objs;
	for (size_t i = 0; i < nCount; ++i)
		objs.push_back(&(m_info[i].GetObj()));
	selGroup.ReplaceWithObjects(objs);
}

bool CmdContext::CmdInfo::SaveProps()
{
	ObjContainer::iterator it = m_info.begin();
	ObjContainer::iterator itEnd = m_info.end();
	for (; it != itEnd; ++it)
	{
		if (!it->SaveProp())
			return false;
	}
	return true;
}

void CmdContext::CmdInfo::LoadProps()
{
	ObjContainer::iterator it = m_info.begin();
	ObjContainer::iterator itEnd = m_info.end();
	for (; it != itEnd; ++it)
		it->LoadProp();
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
		theUndoCmd.UndoNotifyObjsCmdEndExecution(it->GetObj(), it->GetProp());
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
	bool bSaveProp = cmd.IsImpactSelObjProps();
	CmdInfo* pCmdInfo = *m_itCurCmd;
	assert(0 != pCmdInfo);
	if (!pCmdInfo->SaveTargetObjInfo(bSaveProp, selGroup))
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
		if (!pCmdInfo->SaveTargetObjInfo(false, selGroup))
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
