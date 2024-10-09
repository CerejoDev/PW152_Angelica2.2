#pragma once

#include "MCommand.h"

#include "MScene.h"

//wrapper for CEditor

class CEditor;

namespace APhysXCommonDNet
{
	
	public ref class MEditor : public MScene
	{
	public:
		MEditor();
		!MEditor();
		virtual ~MEditor();

		//get internal editor
		CEditor* GetEditor() { return m_pEditor; }
		MCmdQueue^ GetMCmdQueue() { return m_cmdQueue; }

		MRegion^ CreateRegion(System::Drawing::Point pt);
		MIObjBase^ CreateObject(MObjTypeID objType, System::Drawing::Point pt);
		MIPhysXObjBase^	CreateObject(MObjTypeID objType, System::String^ szFile, System::Drawing::Point pos);

		void SaveAnObject(System::String^ filepath);
		IObjBase* LoadAnObject(System::String^ filepath, System::Drawing::Point pos);

		virtual void OnLButtonDown(int x, int y, unsigned int nFlags) override;
		virtual void OnLButtonUp(int x, int y, unsigned int nFlags) override;
		virtual void OnMButtonUp(int x, int y, unsigned int nFlags) override;
		virtual void OnMouseMove(int x, int y, unsigned int nFlags) override;
		virtual void OnMouseWheel(int x, int y, int zDelta, unsigned int nFlags) override;
		virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) override;

		void EnterEditMode();

		MIObjBase^ GetPickedObject();
		void CloneObjects();

		void RemoveAllSelectedObjects();

	private:
		CEditor* m_pEditor;
		MCmdQueue^ m_cmdQueue;
	};

}