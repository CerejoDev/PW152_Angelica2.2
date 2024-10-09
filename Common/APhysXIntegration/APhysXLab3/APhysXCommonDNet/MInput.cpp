#include "Stdafx.h"
#include "MInput.h"

using namespace System;


namespace APhysXCommonDNet
{

	bool MInput::IsKeyDown(int vKey)
	{
		return MScene::Instance->QueryAsyncKeyDown(vKey);
	}
	
	bool MInput::GetMousePos([Out]System::Drawing::Point% outPT)
	{
		POINT pt;
		pt.x = 0;
		pt.y = 0;
		bool bRtn = false;
		if (_hwnd != 0)
			bRtn = MScene::Instance->GetCurrentMouseClientPoint(_hwnd, pt);
		outPT.X = pt.x;
		outPT.Y = pt.y;
		return bRtn;
	}

	bool MInput::IsMouseLBtnDown()
	{
		return MScene::Instance->QueryAsyncKeyDown(VK_LBUTTON);
	}

	bool MInput::IsMouseRBtnDown()
	{
		return MScene::Instance->QueryAsyncKeyDown(VK_RBUTTON);
	}

	bool MInput::IsMouseMBtnDown()
	{
		return MScene::Instance->QueryAsyncKeyDown(VK_MBUTTON);
	}

	MInput::MInput()
	{
		_hwnd = 0;
		if (_instance == nullptr)
			_instance = this;
	}
}