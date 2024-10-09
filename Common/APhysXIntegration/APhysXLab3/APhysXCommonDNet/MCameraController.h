#pragma once
//wrapper for CCameraController

namespace APhysXCommonDNet
{
	value class Vector3;

	public enum class MCameraMode
	{
		MODE_FREE_FLY  = 0,
		MODE_BINDING   = 1,
	};

	public ref class MCameraController : MIObjBase
	{
	private:
		const CCameraController* _pCameraController;

	internal:
		MCameraController(const CCameraController* pController);
	
	public:
		
		Vector3 GetPos();
		Vector3 GetDir();
		Vector3 GetDirH();
		Vector3 GetRight();
		Vector3 GetRightH();

		MCameraMode GetMode();
		bool QueryMode(MCameraMode mode);
		
		MIObjBase^ GetFocusObj();
		bool GetDefBindPos(Vector3% outFocusPos);
	};

	public interface class IMCamOptTaskParams
	{
	public:
		virtual ICamOptTask* CreateParams();

	};

	public ref class MCamOptTask_EntryBindingMode : IMCamOptTaskParams
	{
	public:
		MIObjBase^ pBindObj;
		bool bStorePose;
		bool bIsKeepDir;

		MCamOptTask_EntryBindingMode(MIObjBase^ pObj)
		{
			pBindObj = pObj;
			bStorePose = false;
			bIsKeepDir  =false;
		}

		virtual ICamOptTask* CreateParams()
		{
			CamOptTask_EntryBindingMode* pObj = new CamOptTask_EntryBindingMode(0);
			if (0 != pObj)
			{
				pObj->m_pBindObj = pBindObj->GetIObjBase();
				pObj->m_bStorePose = bStorePose;
				pObj->m_bIsKeepDir = bIsKeepDir;
			}
			return pObj;
		}
	};

	public ref class MCCamCtrlAgent
	{
	private:
		CCamCtrlAgent* _pCamCtrlAgent;

	internal:
		MCCamCtrlAgent(CCamCtrlAgent* pAgent);

	public:
		void ExecuteCameraOperation(IMCamOptTaskParams^ micot);
	};
}