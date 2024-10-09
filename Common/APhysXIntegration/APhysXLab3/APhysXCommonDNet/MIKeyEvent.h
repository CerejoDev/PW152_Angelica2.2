#pragma once

//key event handler ...
namespace APhysXCommonDNet
{
	public interface class MIKeyEvent
	{
	public:
		virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

		//mouse event
		virtual void OnLButtonDown(int x, int y, unsigned int nFlags);
		virtual void OnLButtonUp(int x, int y, unsigned int nFlags);
		virtual void OnMButtonUp(int x, int y, unsigned int nFlags);
		virtual void OnRButtonDown(int x, int y, unsigned int nFlags);
		virtual void OnRButtonUp(int x, int y, unsigned int nFlags);
		virtual void OnMouseMove(int x, int y, unsigned int nFlags);
		virtual void OnMouseWheel(int x, int y, int zDelta, unsigned int nFlags);
	};

	ref class MKeyEventMgr : MIKeyEvent
	{
	private:
		MKeyEventMgr();
		static MKeyEventMgr^ _instance = nullptr;
		System::Collections::Generic::List<MIKeyEvent^>^ m_keyEvents;

	public:
		static property MKeyEventMgr^ Instance
		{
			MKeyEventMgr^ get()
			{
				if (_instance == nullptr)
					_instance = gcnew MKeyEventMgr();
				return _instance; 
			}
		}

		void Register(MIKeyEvent^ keyEvent);
		void UnRegister(MIKeyEvent^ keyEvent);
		void UnRegisterAll();
		
	public:
		virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

		//mouse event
		virtual void OnLButtonDown(int x, int y, unsigned int nFlags);
		virtual void OnLButtonUp(int x, int y, unsigned int nFlags);
		virtual void OnMButtonUp(int x, int y, unsigned int nFlags);
		virtual void OnRButtonDown(int x, int y, unsigned int nFlags);
		virtual void OnRButtonUp(int x, int y, unsigned int nFlags);
		virtual void OnMouseMove(int x, int y, unsigned int nFlags);
		virtual void OnMouseWheel(int x, int y, int zDelta, unsigned int nFlags);
	};
}