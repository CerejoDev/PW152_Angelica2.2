#include "Stdafx.h"
#include "PhysXObjDynamic.h"
#include "MPhysXObjDynamic.h"

using namespace System;

namespace APhysXCommonDNet
{
	MPhysXObjDynamic::MPhysXObjDynamic(CPhysXObjDynamic* pPhysXBase) : MIPhysXObjBase(pPhysXBase)
	{
		m_pDynamic = dynamic_cast<CPhysXObjDynamic*>(pPhysXBase);
	}
	MPhysXObjDynamic::MPhysXObjDynamic(CPhysXObjDynamic* pDynamic, MIObjBase^ parent)
		:MIPhysXObjBase(pDynamic, parent)
	{
		m_pDynamic = pDynamic;
	}
	MPhysXObjDynamic::MPhysXObjDynamic()
	{
		m_pDynamic = nullptr;
	}
	
	bool MPhysXObjDynamic::ApplyPhysXBody(bool bEnable)
	{
		if (m_pDynamic != nullptr)
		{
			return m_pDynamic->ApplyPhysXBody(bEnable, &MScene::Instance->GetAPhysXScene());
		}
		return false;
	}

	void MPhysXObjDynamic::SetLinearVelocity(Vector3 v)
	{
		if (m_pDynamic != nullptr)
		{
			APhysXSkeletonRBObject* pPSRB = m_pDynamic->GetSkeletonRBObject();
			if (pPSRB)
			{
				NxVec3 nxv3(v.X, v.Y, v.Z);
				pPSRB->SetLinearVelocity(nxv3);
			}
		}
	}

	void MPhysXObjDynamic::AddForce(Vector3 pt, Vector3 dirANDmag, int iPhysForceType, float fSweepRadius)
	{
		if (nullptr == m_pDynamic)
			return;

		NxVec3 nxpt(pt.X, pt.Y, pt.Z);
		NxVec3 nxdm(dirANDmag.X, dirANDmag.Y, dirANDmag.Z);
		m_pDynamic->AddForce(nxpt, nxdm, iPhysForceType, fSweepRadius);
	}

	bool MPhysXObjDynamic::SetCollisionChannel(MCollisionChannel cnl)
	{
		CPhysXObjDynamic::CollisionChannel cn = static_cast<CPhysXObjDynamic::CollisionChannel>(cnl);
		return m_pDynamic->SetCollisionChannel(cn);
	}
	bool MPhysXObjDynamic::GetCollisionChannel(MCollisionChannel% outChannel)
	{
		outChannel = MCollisionChannel::CNL_UNKNOWN;
		APhysXCollisionChannel cn = APX_COLLISION_CHANNEL_INVALID;
		bool bRtn = m_pDynamic->GetCollisionChannel(cn);
		if (bRtn)
		{
			if (APX_COLLISION_CHANNEL_COMMON == cn)
				outChannel = MCollisionChannel::CNL_COMMON;
			else if (APX_COLLISION_CHANNEL_CC_COMMON_HOST == cn)
				outChannel = MCollisionChannel::CNL_INDEPENDENT_CCHOST;
			else if (APX_COLLISION_CHANNEL_DYNAMIC_CC == cn)
				outChannel = MCollisionChannel::CNL_INDEPENDENT_CCHOST;
			else if (APX_COLLISION_CHANNEL_INVALID > cn)
				outChannel = MCollisionChannel::CNL_INDEPENDENT;
		}
		return bRtn;
	}
}