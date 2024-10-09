#include "Stdafx.h"
#include "MCameraController.h"
#include "MA3DVector3.h"

using namespace System;

namespace APhysXCommonDNet
{
	MCameraController::MCameraController(const CCameraController* pController)
	{
		_pCameraController = pController;
		m_pObjBase = (CCameraController*)pController;
	}
	
	Vector3 MCameraController::GetPos()
	{
		return Vector3::FromA3DVECTOR3(_pCameraController->GetCamera().GetPos());
	}

	Vector3 MCameraController::GetDir()
	{
		return Vector3::FromA3DVECTOR3(_pCameraController->GetCamera().GetDir());
	}

	Vector3 MCameraController::GetDirH()
	{
		return Vector3::FromA3DVECTOR3(_pCameraController->GetCamera().GetDirH());
	}
	Vector3 MCameraController::GetRight()
	{
		return Vector3::FromA3DVECTOR3(_pCameraController->GetCamera().GetRight());
	}
	Vector3 MCameraController::GetRightH()
	{
		return Vector3::FromA3DVECTOR3(_pCameraController->GetCamera().GetRightH());
	}
	MCameraMode MCameraController::GetMode()
	{
		return static_cast<MCameraMode>(_pCameraController->GetMode());
	}
	bool MCameraController::QueryMode(MCameraMode mode)
	{
		return _pCameraController->QueryMode(static_cast<CameraMode>(mode));
	}
	MIObjBase^ MCameraController::GetFocusObj()
	{
		if (_pCameraController->QueryMode(MODE_BINDING))
			return MScene::Instance->GetMIObjBase(_pCameraController->GetFocusObj());

		return nullptr;
	}
	bool MCameraController::GetDefBindPos(Vector3% outFocusPos)
	{
		A3DVECTOR3 vFocusPos;
		if (_pCameraController->GetDefBindPos(vFocusPos))
		{
			outFocusPos = Vector3::FromA3DVECTOR3(vFocusPos);
			return true;
		}
		return false;
	}


	MCCamCtrlAgent::MCCamCtrlAgent(CCamCtrlAgent* pAgent)
	{
		_pCamCtrlAgent = pAgent;
	}
	void MCCamCtrlAgent::ExecuteCameraOperation(IMCamOptTaskParams^ micot)
	{
		if (nullptr == micot)
			return;

		ICamOptTask* pParams = micot->CreateParams();
		if (0 == pParams)
			return;

		_pCamCtrlAgent->ExecuteCameraOperation(*pParams);
		delete pParams;
	}
}