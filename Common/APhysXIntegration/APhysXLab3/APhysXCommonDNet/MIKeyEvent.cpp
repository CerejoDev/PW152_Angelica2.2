#include "Stdafx.h"
#include "MIKeyEvent.h"

using namespace System;
using namespace System::Collections::Generic;

namespace APhysXCommonDNet
{
	//////////////////////////////////////////////////////////////////////////
	//implementation of MKeyEventMgr
	MKeyEventMgr::MKeyEventMgr()
	{
		_instance = nullptr;
		m_keyEvents = gcnew List<MIKeyEvent^>();
	}

	void MKeyEventMgr::Register(MIKeyEvent^ keyEvent)
	{
		if (!m_keyEvents->Contains(keyEvent))
		{
			m_keyEvents->Add(keyEvent);
		}
	}
	void MKeyEventMgr::UnRegister(MIKeyEvent^ keyEvent)
	{
		m_keyEvents->Remove(keyEvent);
	}
	void MKeyEventMgr::UnRegisterAll()
	{
		m_keyEvents->Clear();
	}

	void MKeyEventMgr::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		for (int i = 0; i < m_keyEvents->Count; i++)
		{
			TRYCALL(m_keyEvents[i]->OnKeyDown(nChar, nRepCnt, nFlags))
		}
	}
	void MKeyEventMgr::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		for (int i = 0; i < m_keyEvents->Count; i++)
		{
			TRYCALL(m_keyEvents[i]->OnKeyUp(nChar, nRepCnt, nFlags))
		}
	}

	void MKeyEventMgr::OnLButtonDown(int x, int y, unsigned int nFlags)
	{
		for (int i = 0; i < m_keyEvents->Count; i++)
		{
			TRYCALL(m_keyEvents[i]->OnLButtonDown(x, y, nFlags))
		}
	}
	void MKeyEventMgr::OnLButtonUp(int x, int y, unsigned int nFlags)
	{
		for (int i = 0; i < m_keyEvents->Count; i++)
		{
			TRYCALL(m_keyEvents[i]->OnLButtonUp(x, y, nFlags))
		}
	}
	void MKeyEventMgr::OnMButtonUp(int x, int y, unsigned int nFlags)
	{
		for (int i = 0; i < m_keyEvents->Count; i++)
		{
			TRYCALL(m_keyEvents[i]->OnMButtonUp(x, y, nFlags))
		}
	}
	void MKeyEventMgr::OnRButtonDown(int x, int y, unsigned int nFlags)
	{
		for (int i = 0; i < m_keyEvents->Count; i++)
		{
			TRYCALL(m_keyEvents[i]->OnRButtonDown(x, y, nFlags))
		}
	}
	void MKeyEventMgr::OnRButtonUp(int x, int y, unsigned int nFlags)
	{
		for (int i = 0; i < m_keyEvents->Count; i++)
		{
			TRYCALL(m_keyEvents[i]->OnRButtonUp(x, y, nFlags))
		}
	}
	void MKeyEventMgr::OnMouseMove(int x, int y, unsigned int nFlags)
	{
		for (int i = 0; i < m_keyEvents->Count; i++)
		{
			TRYCALL(m_keyEvents[i]->OnMouseMove(x, y, nFlags))
		}
	}
	void MKeyEventMgr::OnMouseWheel(int x, int y, int zDelta, unsigned int nFlags)
	{
		for (int i = 0; i < m_keyEvents->Count; i++)
		{
			TRYCALL(m_keyEvents[i]->OnMouseWheel(x, y, zDelta, nFlags))
		}
	}
}