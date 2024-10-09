#include "Stdafx.h"
#include "MEditor.h"
#include "Game.h"

using namespace System;


namespace APhysXCommonDNet
{
	MEditor::MEditor() : MScene(new CEditor)
	{
		m_pEditor = (CEditor*)GetScene();
		m_bReleaseScene = true;
		m_cmdQueue = gcnew MCmdQueue(&m_pEditor->GetCmdQueue());
	}
	
	MEditor::!MEditor()
	{
		m_pEditor = nullptr;
		if (m_cmdQueue != nullptr)
		{
			m_cmdQueue->ClearAllCmds();
			m_cmdQueue = nullptr;
		}
		
	}
	MEditor::~MEditor()
	{
		this->!MEditor();
	}

	MRegion^ MEditor::CreateRegion(System::Drawing::Point pt)
	{
		POINT point;
		point.x = pt.X;
		point.y = pt.Y;
		CRegion* pRegion = dynamic_cast<CRegion*>(m_pEditor->CreateObject(ObjManager::OBJ_TYPEID_REGION, point));

		MRegion^ region = gcnew MRegion(pRegion);
		m_Objs->Add(region);
		return region;
	}

	MIObjBase^ MEditor::CreateObject(MObjTypeID objType, System::Drawing::Point pt)
	{
		if (int(MObjTypeID::OBJ_TYPEID_PHYSX) & int(objType))
			return nullptr;

		if (MObjTypeID::OBJ_TYPEID_REGION == objType)
			return CreateRegion(pt); 

		POINT point;
		point.x = pt.X;
		point.y = pt.Y;

		IObjBase* pObj = 0;
		if (MObjTypeID::OBJ_TYPEID_DUMMY == objType)
			pObj = m_pEditor->CreateObject(ObjManager::OBJ_TYPEID_DUMMY, point);
		else if (MObjTypeID::OBJ_TYPEID_SMSOCKET == objType)
			pObj = m_pEditor->CreateObject(ObjManager::OBJ_TYPEID_SMSOCKET, point);
		else if (MObjTypeID::OBJ_TYPEID_SOCKETSM == objType)
			pObj = m_pEditor->CreateObject(ObjManager::OBJ_TYPEID_SOCKETSM, point);
		else if (MObjTypeID::OBJ_TYPEID_SPOTLIGHT == objType)
			pObj = m_pEditor->CreateObject(ObjManager::OBJ_TYPEID_SPOTLIGHT, point);

		if (0 != pObj)
		{
			MIObjBase^ pMObj = gcnew MIObjBase(pObj);
			m_Objs->Add(pMObj);
			return pMObj;
		}
		return nullptr;
	}

	MIPhysXObjBase^	MEditor::CreateObject(MObjTypeID objType, System::String^ szFile, System::Drawing::Point pos)
	{
		if (!(int(MObjTypeID::OBJ_TYPEID_PHYSX) & int(objType)))
			return nullptr;

		ObjManager::ObjTypeID type = static_cast<ObjManager::ObjTypeID>(objType);
		POINT pt;
		pt.x = pos.X;
		pt.y = pos.Y;
		pin_ptr<const wchar_t> wch = PtrToStringChars(szFile);
		AString strfile = WC2AS(wch);

		IPhysXObjBase* pPhysXObj = m_pEditor->CreateObject(type, strfile, pt);
		if (pPhysXObj == NULL)
			return nullptr;

		MIPhysXObjBase^ physXobj = gcnew MIPhysXObjBase(pPhysXObj);
		m_Objs->Add(physXobj);
		return physXobj;
	}

	void MEditor::SaveAnObject(System::String^ filepath)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(filepath);
		AString strfile = WC2AS(wch);
		m_pEditor->SaveAnObject(strfile);
	}

	IObjBase* MEditor::LoadAnObject(System::String^ filepath, System::Drawing::Point pos)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(filepath);
		AString strfile = WC2AS(wch);
		if (strfile.IsEmpty())
			return 0;

		POINT pt;
		pt.x = pos.X;
		pt.y = pos.Y;
		// note here: didn't create MObject
		return m_pEditor->LoadAnObject(strfile, pt);
	}

	void MEditor::OnLButtonDown(int x, int y, unsigned int nFlags)
	{
		if (m_pEditor->QuerySceneMode(Scene::SCENE_MODE_PLAY_ON))
			__super::OnLButtonDown(x, y, nFlags);
		else
			m_pEditor->OnLButtonDown(x, y, nFlags);
	}
	void MEditor::OnLButtonUp(int x, int y, unsigned int nFlags)
	{
		if (m_pEditor->QuerySceneMode(Scene::SCENE_MODE_PLAY_ON))
			__super::OnLButtonUp(x, y, nFlags);
		else
			m_pEditor->OnLButtonUp(x, y, nFlags);
	}
	void MEditor::OnMButtonUp(int x, int y, unsigned int nFlags)
	{
		if (m_pEditor->QuerySceneMode(Scene::SCENE_MODE_PLAY_ON))
			__super::OnMButtonUp(x, y, nFlags);
		else
			m_pEditor->OnMButtonUp(x, y, nFlags);
	}
	void MEditor::OnMouseMove(int x, int y, unsigned int nFlags)
	{
		if (m_pEditor->QuerySceneMode(Scene::SCENE_MODE_PLAY_ON))
			__super::OnMouseMove(x, y, nFlags);
		else
			m_pEditor->OnMouseMove(x, y, nFlags);
	}
	void MEditor::OnMouseWheel(int x, int y, int zDelta, unsigned int nFlags)
	{
		if (m_pEditor->QuerySceneMode(Scene::SCENE_MODE_PLAY_ON))
			__super::OnMouseWheel(x, y, zDelta, nFlags);
		else
			m_pEditor->OnMouseWheel(x, y, zDelta, nFlags);
	}

	MIObjBase^ MEditor::GetPickedObject()
	{
		IObjBase* pObj = m_pEditor->GetPickedObject();
		
		return GetMIObjBase(pObj);
	}

	void MEditor::CloneObjects()
	{
		m_pEditor->CloneObjects();
	}

	void MEditor::EnterEditMode()
	{
		ScriptEnd();
		m_Objs->Clear();
		m_RoleActors->Clear();
		ReleaseAllMMainActor();
		m_pEditor->SetSceneMode(CEditor::SCENE_MODE_PLAY_OFF);
	}

	void MEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (m_pEditor->QuerySceneMode(Scene::SCENE_MODE_PLAY_ON))
			__super::OnKeyDown(nChar, nRepCnt, nFlags);
	}

	void MEditor::RemoveAllSelectedObjects()
	{
		m_pEditor->RemoveAllSelectedObjects();
	}
}