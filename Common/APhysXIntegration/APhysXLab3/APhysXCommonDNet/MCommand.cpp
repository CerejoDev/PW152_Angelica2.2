#include "Stdafx.h"
#include "MCommand.h"
#include "Command.h"

using namespace System;


namespace APhysXCommonDNet
{
	MCmdQueue::MCmdQueue()
	{
		m_pCmdQueue = new CmdQueue(); 
		m_bNeedRelease = true;
	}

	MCmdQueue::MCmdQueue(CmdQueue* pCmdQueue)
	{
		m_pCmdQueue = pCmdQueue;
		m_bNeedRelease = false;
	}

	MCmdQueue::!MCmdQueue()
	{
		if (m_bNeedRelease)
			delete m_pCmdQueue;
		m_pCmdQueue = nullptr;
	}
	MCmdQueue::~MCmdQueue()
	{
		this->!MCmdQueue();
	}

	bool MCmdQueue::UndoEnable()
	{
		return m_pCmdQueue->UndoEnable();
	}
	bool MCmdQueue::RedoEnable()
	{
		return m_pCmdQueue->RedoEnable();
	}

	void MCmdQueue::Undo()
	{
		m_pCmdQueue->Undo();
	}
	void MCmdQueue::Redo()
	{
		m_pCmdQueue->Redo();
	}
	
	bool MCmdQueue::SubmitBeforeExecution(MCmdType cmdtype)
	{
		Scene* pScene = MScene::Instance->GetScene();
		if (pScene == nullptr)
			return false;

/*		switch(cmdtype)
		{
		//case MCmdType::CmdCreateObj:
		//	return m_pCmdQueue->SubmitBeforeExecution(CmdCreateObj(pScene->GetLoadPackage()));
		//case MCmdType::CmdPropChange:
		//	return m_pCmdQueue->SubmitBeforeExecution(CmdPropChange());
		//case MCmdType::CmdReleaseObj:
		//	return m_pCmdQueue->SubmitBeforeExecution(CmdReleaseObj(pScene->GetLoadPackage()));
		//case MCmdType::CmdSelAppend:
		//	return m_pCmdQueue->SubmitBeforeExecution(CmdSelAppend());
		//case MCmdType::CmdSelRemove:
		//	return m_pCmdQueue->SubmitBeforeExecution(CmdSelRemove());
		//case MCmdType::CmdSelReplace:
		//	return m_pCmdQueue->SubmitBeforeExecution(CmdSelReplace());
		
		default:
			assert(!"unknown command type");
			return false;
		}*/
		return false;
	}
	void MCmdQueue::CancelLastSubmit()
	{
		m_pCmdQueue->CancelLastSubmit();
	}
	void MCmdQueue::ClearAllCmds()
	{
		if (m_pCmdQueue != nullptr)
			m_pCmdQueue->ClearAllCmds();
	}
}