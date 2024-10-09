#pragma once

#include "Capsule.h"


namespace APhysXCommonDNet
{
	value class Vector3;

	public value class MCapsule
	{
	internal:
		static A3DCAPSULE ToA3DCAPSULE(MCapsule capsule);
		static CHBasedCD::CCapsule ToCCapsule(MCapsule capsule);
		static MCapsule FromCCapsule(const CHBasedCD::CCapsule& capsule);

	public:

		//get center to bottom height.
		//character model can be positioned GetPos() + A3DVector(0, -GetCenterHei(), 0).  
		float GetCenterHei() { return m_centerHei; }

		//get & set capsule center position
		Vector3 GetPos() { return m_pos; }
		void SetPos(Vector3 pos){ m_pos = pos; }

		//get & set capsule height
		float GetHeight() { return m_height; }
		void SetHeight(float height) { m_height = height; m_centerHei = fabsf(m_up.Y) * (m_height/2) + m_radius;}

		//get & set capsule radius
		float GetRadius() { return m_radius; }
		void SetRadius(float radius) { m_radius = radius; m_centerHei = fabsf(m_up.Y) * (m_height/2) + m_radius;}

		//get & set capsule up direction
		void SetUp(Vector3 up);
		Vector3 GetUp()
		{
			return m_up;
		}

		//get right direction
		Vector3 GetRight()
		{
			return m_right;
		}
		//get front direction
		Vector3 GetFront()
		{
			return m_front;
		}

	protected:
		Vector3 m_pos;//Capsule center position
		float m_height;//capsule height
		float m_radius;//capsule radius

		float m_centerHei;//center to bottom height

		Vector3 m_up; //capsule up direction
		Vector3 m_right; //capsule right direction (used when capsule brush trace)
		Vector3 m_front; //capsule front direction (used when capsule brush trace)

		void InitDir(); //generate right direction and front direction


	};

}