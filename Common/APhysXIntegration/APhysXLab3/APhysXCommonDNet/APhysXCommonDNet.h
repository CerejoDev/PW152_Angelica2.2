// APhysXCommonDNet.h

#pragma once

#include <A3DVector.h>

using namespace System;

namespace APhysXCommonDNet {

	public ref class Class1
	{
		// TODO: Add your methods for this class here.
		int x, y;

	public:
		Class1(){ x = 0; y = 0;}

		void SetXY(int x, int y);
		
	};

	void Class1::SetXY(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

/*	public ref class Vec3DNet
	{
		A3DVECTOR3* pVec3;
	public:
		Vec3DNet()
		{
			pVec3 = new A3DVECTOR3();
		}
		A3DVECTOR3* GetA3DVec3() 
		{
			return pVec3;
		}
		!Vec3DNet();
		~Vec3DNet()
		{
			this->!Vec3DNet();
		}
		Vec3DNet(float x, float y, float z)
		{
			pVec3 = new A3DVECTOR3(x, y, z);
		}

		float GetX()
		{
			return pVec3->x;
		}
		float GetY()
		{
			return pVec3->y;
		}
		float GetZ()
		{
			return pVec3->z;
		}
	};*/
}
