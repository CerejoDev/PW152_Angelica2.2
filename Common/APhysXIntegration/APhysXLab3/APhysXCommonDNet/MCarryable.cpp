#include "Stdafx.h"
#include "MCarryable.h"

using namespace System;

namespace APhysXCommonDNet
{
	//////////////////////////////////////////////////////////////////////////
	//implementation of MCarryable
	MCarryable::MCarryable()
	{
		m_pCarryable = new CCarryableImp();
		m_pActorActionReport = nullptr;
		m_pCarryable->SetMCarryablePtr(this);
	}
	MCarryable::~MCarryable()
	{
		this->!MCarryable();
	}
	MCarryable::!MCarryable()
	{
		delete m_pCarryable;
	}

	CCarryable* MCarryable::GetCarryable()
	{
		return m_pCarryable;
	}

	void MCarryable::OnStateChanged(MCarryableState oldState, MCarryableState newState)
	{
		if (m_pActorActionReport != nullptr)
		{
			m_pActorActionReport->OnCarryingStateChanged(oldState, newState);

			if (oldState == MCarryableState::ToFree && newState == MCarryableState::Free)
			{
				m_pActorActionReport->HostActor->SetCarryablePtr(nullptr);
				SetActorActionReport(nullptr);
			}
		}
	}

	bool MCarryable::Init(MIPhysXObjBase^ pPhysXObj)
	{
		m_pPhysXObj = pPhysXObj;

		return m_pCarryable->Init(pPhysXObj->GetIPhysXObjBase(), MScene::Instance->GetScene());
	}
	void MCarryable::Release()
	{
		m_pCarryable->Release();
	}

	bool MCarryable::GetModelAABB(AABB% aabb)
	{
		A3DAABB _aabb;
		if (m_pCarryable->GetModelAABB(_aabb))
		{
			aabb = AABB::FromA3DAABB(_aabb);
		}
		return false;
	}

	bool MCarryable::GetPos(Vector3% vPos)
	{
		A3DVECTOR3 v;
		if (m_pCarryable->GetPos(v))
		{
			vPos = Vector3::FromA3DVECTOR3(v);
			return true;
		}
		return false;
	}
	void MCarryable::SetPos(Vector3 vPos)
	{
		m_pCarryable->SetPos(Vector3::ToA3DVECTOR3(vPos));
	}
	bool MCarryable::GetDirAndUp(Vector3% vDir, Vector3% vUp)
	{
		A3DVECTOR3 vDirA3D, vUpA3D;
		if (m_pCarryable->GetDirAndUp(vDirA3D, vUpA3D))
		{
			vDir = Vector3::FromA3DVECTOR3(vDirA3D);
			vUp = Vector3::FromA3DVECTOR3(vUpA3D);
			return true;
		}
		return false;
	}
	void MCarryable::SetDirAndUp(Vector3 vDir, Vector3 vUp)
	{
		m_pCarryable->SetDirAndUp(Vector3::ToA3DVECTOR3(vDir), Vector3::ToA3DVECTOR3(vUp));
	}

	MCarryableState MCarryable::GetState()
	{
		return static_cast<MCarryableState>(m_pCarryable->GetState());
	}

	void MCarryable::SetActorActionReport(MActorActionReport^ pActorActionReport)
	{
		m_pActorActionReport = pActorActionReport;
	}

	//////////////////////////////////////////////////////////////////////////
	//implementation of CCarryableImp
	CCarryableImp::CCarryableImp()
	{
		m_pMCarryable = nullptr;
	}
	CCarryableImp::~CCarryableImp()
	{

	}

	void CCarryableImp::SetMCarryablePtr(MCarryable^ pMCarryable)
	{
		m_pMCarryable = pMCarryable;
	}

	void CCarryableImp::OnStateChanged(CCarryable::State oldState, CCarryable::State newState)
	{
		if (m_pMCarryable)
			m_pMCarryable->OnStateChanged(static_cast<MCarryableState>(oldState), static_cast<MCarryableState>(newState));
	}
}