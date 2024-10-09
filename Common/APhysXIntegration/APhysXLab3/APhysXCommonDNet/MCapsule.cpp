#include "Stdafx.h"
#include "MA3DVector3.h"
#include "MCapsule.h"

using namespace System;

namespace APhysXCommonDNet
{

	void MCapsule::SetUp(Vector3 up )
	{
		m_up = up;
		m_up.Normalize();

		InitDir();
	}

	void MCapsule::InitDir()
	{
		Vector3 vAxisX(1, 0, 0);
		Vector3 vAxisZ(0, 0, 1);

		m_right = Vector3::CrossProduct(m_up, vAxisX);
		if (m_right.SquaredMagnitude() < 1E-4f)
			m_right = Vector3::CrossProduct(m_up, vAxisZ);
		m_front = Vector3::CrossProduct(m_right, m_up);

		m_centerHei = fabsf(m_up.Y) * (m_height/2) + m_radius;
	}

	A3DCAPSULE MCapsule::ToA3DCAPSULE(MCapsule capsule)
	{
		A3DCAPSULE cc;
		cc.vAxisY = Vector3::ToA3DVECTOR3(capsule.m_up);
		cc.vAxisX = Vector3::ToA3DVECTOR3(capsule.m_right);
		cc.vAxisZ = Vector3::ToA3DVECTOR3(capsule.m_front);

		cc.fRadius = capsule.m_radius;
		cc.fHalfLen = capsule.m_height/2;
		cc.vCenter = Vector3::ToA3DVECTOR3(capsule.m_pos);

		return cc;
	}
	CHBasedCD::CCapsule MCapsule::ToCCapsule(MCapsule cc)
	{
		CHBasedCD::CCapsule ccapsule;
		ccapsule.SetPos(Vector3::ToA3DVECTOR3(cc.GetPos()));
		ccapsule.SetHeight(cc.GetHeight());
		ccapsule.SetRadius(cc.GetRadius());
		ccapsule.SetUp(Vector3::ToA3DVECTOR3(cc.GetUp()));
		return ccapsule;
	}
	MCapsule MCapsule::FromCCapsule(const CHBasedCD::CCapsule& capsule)
	{
		MCapsule mcapsule;
		mcapsule.SetPos(Vector3::FromA3DVECTOR3(capsule.GetPos()));
		mcapsule.SetHeight(capsule.GetHeight());
		mcapsule.SetRadius(capsule.GetRadius());
		mcapsule.SetUp(Vector3::FromA3DVECTOR3(capsule.GetUp()));

		return mcapsule;
	}
	
}