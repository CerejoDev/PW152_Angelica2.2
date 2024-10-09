#pragma once

//state of keyboard, mouse ...
namespace APhysXCommonDNet
{
	
	public ref class MInput
	{
	public:
		static property MInput^ Instance
		{
			MInput^ get()
			{
				if (_instance == nullptr)
				{
					return gcnew MInput();
				}
				return _instance;
			}
		}
		bool IsKeyDown(int vKey);

		bool GetMousePos([Out]System::Drawing::Point% outPT);

		bool IsMouseLBtnDown();

		bool IsMouseRBtnDown();

		bool IsMouseMBtnDown();

	internal:
		HWND _hwnd;

	private:
		static MInput^ _instance = nullptr;
		MInput();
	};

}