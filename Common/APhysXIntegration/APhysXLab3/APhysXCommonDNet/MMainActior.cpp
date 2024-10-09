#include "Stdafx.h"

using namespace System;

namespace APhysXCommonDNet
{
	MIRoleActor::MIRoleActor(IRoleActor* pActor, MIObjBase^ pObjDyn)
	{
		m_pRActor = pActor;
		m_pObjBase = pObjDyn;
		ApxActorBase* pAAB = pActor->GetActorBase();
		m_pMActorBase = gcnew MActorBase(pAAB, pObjDyn);
	}

	MIRoleActor::~MIRoleActor()
	{
		this->!MIRoleActor();
	}


	MMainActor::MMainActor(CMainActor* pActor, MPhysXObjDynamic^ pObjDyn)
	{
		_pActor = pActor;
		_pObjBase = pObjDyn;
		ApxActorBase* pAAB = pActor->GetActorBase();
		m_pMActorBase = gcnew MActorBase(pAAB, pObjDyn);
	}
	
	MMainActor::!MMainActor()
	{
	}
	MMainActor::~MMainActor()
	{
		this->!MMainActor();
	}

	Vector3 MMainActor::GetPos()
	{
		if (_pActor)
		{
			return Vector3::FromA3DVECTOR3(_pActor->GetPos());
		}

		assert(_pActor);
		return Vector3();
	}
	System::String^ MMainActor::GetCCTypeText()
	{
		if (_pActor)
		{
			const TCHAR* typetext = _pActor->GetCCTypeText();
			return gcnew String(_TWC2AS(typetext));
		}

		assert(_pActor);
		return gcnew String("");
	}

	void MMainActor::SetLButtonDown()
	{
		assert(_pActor);
		if (_pActor)
			return _pActor->SetLButtonDown();
	}
	void MMainActor::SetLButtonUp()
	{
		assert(_pActor);
		if (_pActor)
			return _pActor->SetLButtonUp();
	}
	void MMainActor::UpdateTargetPT(int x, int y)
	{
		assert(_pActor);
		if (_pActor)
			return _pActor->UpdateTargetPT(x, y);
	}

	void MMainActor::WalkTo(Vector3 pos)
	{
		if (_pActor)
		{
			_pActor->WalkTo(Vector3::ToA3DVECTOR3(pos));
		}
	}
	bool MMainActor::IsInAir()
	{
		if (_pActor)
		{
			return _pActor->IsInAir();
		}
		assert(_pActor);
		return false;
	}

	bool MMainActor::IsKeepWalking()
	{
		if (_pActor)
		{
			return _pActor->IsKeepWalking();
		}
		assert(_pActor);
		return false;
	}

	bool MMainActor::IsRunEnable()
	{
		if (_pActor)
		{
			return _pActor->IsRunEnable();
		}
		assert(_pActor);
		return false;
	}

	void MMainActor::EnableRun(bool bEnable)
	{
		if (_pActor)
		{
			_pActor->EnableRun(bEnable);
		}
	}

	void MMainActor::Jump(float fJumpSpeed)
	{
		if (_pActor)
		{
			_pActor->Jump(fJumpSpeed);
		}
	}

	void MMainActor::SwitchCCType(MCCType ct)
	{
		if (_pActor)
		{
			CAPhysXCCMgr::CCType cctype = static_cast<CAPhysXCCMgr::CCType>(ct);
			_pActor->SwitchCCType(cctype);
		}
	}

	void MMainActor::SwitchToRagDoll()
	{
		_pObjBase->SetDrivenModeEnforce(MDrivenMode::DRIVEN_BY_PURE_PHYSX);
	}

	Vector3 MMainActor::GetDeltaMove()
	{
		return Vector3::FromA3DVECTOR3(_pActor->GetDeltaMove());
	}

	void MMainActor::SetMActionReport(MActorActionReport^ pActionReport)
	{
		GetMActorBase()->SetMActionReport(pActionReport);
	}

	void MMainActor::RegisterKeyEvent()
	{
		GetMActorBase()->RegisterKeyEvent();
	}

	void MMainActor::UnRegisterKeyEvent()
	{
		GetMActorBase()->UnRegisterKeyEvent();
	}

	void MMainActor::EnterCameraBinding(bool bIsKeepDir)
	{
		GetMActorBase()->EnterCameraBinding(bIsKeepDir);
	}
}
