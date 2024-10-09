#include "Stdafx.h"
#include "MPhysXObjParticle.h"
#include "PhysXObjParticle.h"

using namespace System;

namespace APhysXCommonDNet
{
	MPhysXObjParticle::MPhysXObjParticle(CPhysXObjParticle* pParticle) : MIPhysXObjBase(pParticle)
	{
		m_pParticle = pParticle;
	}
	MPhysXObjParticle::MPhysXObjParticle(CPhysXObjParticle* pParticle, MIObjBase^ parent) : MIPhysXObjBase(pParticle, parent)
	{
		m_pParticle = pParticle;
	}
	MPhysXObjParticle::MPhysXObjParticle()
	{
		m_pParticle = nullptr;
	}
	bool MPhysXObjParticle::ControlState(bool EnablePlay)
	{
		if (m_pParticle != nullptr)
		{
			return m_pParticle->ControlState(EnablePlay);
		}
		return false;
	}
	
}