#pragma once

#using <System.dll>
#include "MIKeyEvent.h"

namespace APhysXCommonDNet
{
	ref class MIPhysXObjBase;

	public ref class MIScriptBehavior abstract : MIKeyEvent
	{
	private:
		static System::Random^ m_gRand = nullptr;
		static int m_gRandSeed = 0;

		bool _bPause;
		MIObjBase^ _pObjBase;
	
	public:
		property bool IsPause
		{
			bool get() { return _bPause; }
			void set(bool value) { _bPause = value; }
		}
		
		property MIObjBase^ ObjBase
		{
			MIObjBase^ get() { return _pObjBase; }
			void set(MIObjBase^ value) { _pObjBase = value; }
		}

		static System::Random^ GetRandom()
		{
			return m_gRand;
		}

		static void InitRandom(int rs)
		{
			m_gRandSeed = rs;
			m_gRand = gcnew System::Random(rs);
		}

		MIScriptBehavior()
		{
			_bPause = false;
			_pObjBase = nullptr;
		}
		virtual ~MIScriptBehavior(){}

		virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){}
		virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags){}

		virtual void OnLButtonDown(int x, int y, unsigned int nFlags){}
		virtual void OnLButtonUp(int x, int y, unsigned int nFlags){}
		virtual void OnMButtonUp(int x, int y, unsigned int nFlags){}
		virtual void OnRButtonDown(int x, int y, unsigned int nFlags){}
		virtual void OnRButtonUp(int x, int y, unsigned int nFlags){}
		virtual void OnMouseMove(int x, int y, unsigned int nFlags){}
		virtual void OnMouseWheel(int x, int y, int zDelta, unsigned int nFlags){}

		//preparing work
		virtual void Start(){}
		//leave play mode
		virtual void End(){}

		//render something user defined
		virtual void OnRender(MRender^ render){} 
		//pre scene tick
		virtual void PreTick(float dtSec){}
		//post scene tick
		virtual void PostTick(float dtSec){}

	};

}