#include "Stdafx.h"

#include "MCameraController.h"
#include "MInput.h"
#include "ScriptFactory.h"
#include "UserEventImp.h"
#include "MTimerEvent.h"
#include <time.h>

using namespace System;
using namespace System::Collections::Generic;

namespace APhysXCommonDNet
{
	void MScene::_renderExtra(CRender* pRender)
	{
		MScene^ scene = MScene::Instance;
		if (scene != nullptr)
		{
			scene->renderExtraEvent(scene->GetMRender());
		}
		scene->ScriptOnRender(scene->GetMRender());
	}

	static void _renderExtraCallBack(CRender* pRender)
	{
		MScene::_renderExtra(pRender);
	}

	APhysXU32 MScene::GetVersion()
	{
		return Scene::GetVersion();
	}
	MScene::MScene()
	{
		_pScene = new Scene();
		m_bReleaseScene = true;
		CommonConstructor();
	}
	MScene::MScene(Scene* pSecne)
	{
		//scene given from outside, not release it
		_pScene = pSecne;
		m_bReleaseScene = false;
		CommonConstructor();
	}

	void MScene::CommonConstructor()
	{
		m_scriptBehaviors = gcnew System::Collections::Generic::List<MIScriptBehavior^>();
		m_Objs = gcnew System::Collections::Generic::List<MIObjBase^>();
		m_MainActors = gcnew List<MMainActor^>();
		m_RoleActors = gcnew List<MIRoleActor^>();
		m_pCurrentMA = nullptr;
		_instance = this;
		m_pCamCtrlAgent = nullptr;
		m_pNormalCamCtrler = nullptr;
		m_render = nullptr;

		af_Initialize();
		String^ strBaseDir =  gcnew String(af_GetBaseDir());
		m_ScriptFolder = strBaseDir + gcnew String("\\ApxUserScripts");
	}
	
	MScene::~MScene()
	{
		this->!MScene();
	}

	bool MScene::Init(IntPtr hInst, IntPtr hWnd, bool bFullScreen)
	{
		
		if (_pScene->Init((HINSTANCE)hInst.ToPointer(), (HWND)hWnd.ToPointer(), bFullScreen))
		{
			m_pCamCtrlAgent = gcnew MCCamCtrlAgent(_pScene->GetCamCtrlAgent());
			m_pNormalCamCtrler = gcnew MCameraController(_pScene->GetNormalCamCtrl());
			m_render = gcnew MRender(_pScene->GetRender());
			_pScene->RegisterRenderExtra(_renderExtraCallBack);
			MInput::Instance->_hwnd = (HWND)hWnd.ToPointer();
			_pScene->RegisterRegionEvent(UserEventImp::GetInstancePtr()); 
			return true;
		}
		return false;
	}

	void MScene::Release()
	{
		if (_pScene == nullptr)
			return; 
		ScriptEnd();

		_pScene->Release();
		if (m_bReleaseScene)
			delete _pScene;
		_pScene = NULL;
	}

	A3DTerrain2*	MScene::GetTerrain()
	{
		return _pScene->GetTerrain();
	}
	A3DSkySphere*	MScene::GetSky()
	{
		return _pScene->GetSky();
	}

	bool MScene::Replay(System::String^ filepath)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(filepath);
		AString strfile = WC2AS(wch);
		return _pScene->Replay(strfile);
	}

	bool MScene::LoadTerrainAndPhysXTerrain(System::String^ szFile)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(szFile);
		AString strfile = WC2AS(wch);
		return _pScene->LoadTerrainAndPhysXTerrain(strfile);
	}
	
	MIObjBase^  MScene::CreateObjectRuntime(MObjTypeID objType, String^ szFile, Vector3 pos, bool OnGround, bool isEnterRuntime, String^ strObjName, String^ strScriptName, bool bUseDefScript)
	{
		//check if objType validate
		unsigned int n = (unsigned int)objType;
		for ( int count = 0; n; n &= n -1)
		{
			count++;
			if (count > 1)
			{
				assert(!"invalidate objType");
				return nullptr;
			}
		}

		ObjManager::ObjTypeID ObjID = static_cast<ObjManager::ObjTypeID>(objType);

		AString strfile;
		if (nullptr != szFile)
		{
			pin_ptr<const wchar_t> wch = PtrToStringChars(szFile);
			strfile = WC2AS(wch);
		}

		IObjBase* pObjBase;
		if (0 == strfile.GetLength())
		{
			assert(false == ObjManager::IsPhysXObjType(ObjID));
			pObjBase = _pScene->CreateObjectRuntime(ObjID, Vector3::ToA3DVECTOR3(pos), isEnterRuntime);
		}
		else
		{
			assert(true == ObjManager::IsPhysXObjType(ObjID));
			pObjBase = _pScene->CreateObjectRuntime(ObjID, strfile, Vector3::ToA3DVECTOR3(pos), OnGround, isEnterRuntime);
		}
		
		if (pObjBase == nullptr)
			return nullptr;

		MIObjBase^ obj = MIObjBase::CreateInstance(pObjBase, nullptr);
		if (strObjName != nullptr)
			obj->SetObjName(strObjName);

		// to do: support multi-script name
		if (strScriptName != nullptr)
		{
			MIScriptBehavior^ behavior = (MIScriptBehavior^)ScriptFactory::Instance->CreateInstance(strScriptName);
			obj->AddScript(behavior);

			behavior->ObjBase = obj;
			behavior->Start();
		}

		if (bUseDefScript && DefScriptFile != nullptr)
		{
			MIScriptBehavior^ behavior = (MIScriptBehavior^)ScriptFactory::Instance->CreateInstance(DefScriptFile);
			obj->InserScript(0, behavior);

			behavior->ObjBase = obj;
			behavior->Start();
		}

		m_Objs->Add(obj);
		return obj;
	}
	void MScene::ReleaseObject(MIObjBase^ pObject)
	{
		if (pObject == nullptr)
			return;

		pObject->ScriptEnd();
		ObjManager::GetInstance()->ReleaseObject(pObject->GetIObjBase());
		
		MIObjBase^ parent = pObject->GetParent();
		if (parent == nullptr)
			m_Objs->Remove(pObject);
		else
			parent->RemoveChild(pObject);
	}

	bool MScene::Tick()
	{
		static DWORD dwLastSleepEnd = 0;
		static DWORD dwLastFrame = 0;
		DWORD dwNowTime = a_GetTime();
		DWORD dwTickTime = dwLastFrame ? dwNowTime - dwLastFrame : 1;
		dwLastFrame = dwNowTime;

		a_ClampRoof(dwTickTime, DWORD(100)); //	Limit time of a logic tick
		float dtSec = dwTickTime * 0.001f;
		dtSec = _pScene->GetAPhysXScene()->GetExactSimulateTime(dtSec);
		if (dtSec < 0.0001f)
			return false;

		float reproDT = _pScene->GetReproTickSec();
		if (0 < reproDT)
			dtSec = reproDT;

		if (_pScene->IsPhysXSimEnable())
		{
			_pScene->GetAPhysXScene()->GetPerfStats()->StartPerfStat("script pretick");
			ScriptPreTick(dtSec);
			_pScene->GetAPhysXScene()->GetPerfStats()->EndPerfStat();
			
#if 0
			int iLoops = (int)(dtSec / _pScene->GetAPhysXScene()->GetSimulateTimeStep());
			for(int i = 0; i<iLoops; i++)
				_pScene->Tick(_pScene->GetAPhysXScene()->GetSimulateTimeStep());
#else
			_pScene->Tick(dtSec);
#endif
			_pScene->GetAPhysXScene()->GetPerfStats()->StartPerfStat("script posttick");
			ScriptPostTick(dtSec);
			_pScene->GetAPhysXScene()->GetPerfStats()->EndPerfStat();

			int mti = _pScene->GetMinTickTime();
			if (0 < mti)
			{
				if (_pScene->QuerySceneMode(Scene::SCENE_MODE_PLAY_ON))
				{
					DWORD dwAllTime = dwLastSleepEnd ? a_GetTime() - dwLastSleepEnd : mti;
					if (mti > (int)dwAllTime)
					{
						DWORD slp = mti - dwAllTime;

						LARGE_INTEGER freq;
						QueryPerformanceFrequency(&freq);
						LARGE_INTEGER start, end;
						QueryPerformanceCounter(&start);
						double dTime = 0;
						while(true)
						{
							QueryPerformanceCounter(&end);
							dTime = (double)(end.QuadPart - start.QuadPart) / (double)(freq.QuadPart);
							dTime *= 1000;
							if (dTime > slp)
								break;
						}
					}
				}
			}
		}
		else
		{
			_pScene->Tick(dtSec);
		}
		dwLastSleepEnd = a_GetTime();

		return true;
	}
	bool MScene::Render()
	{
		return _pScene->Render();
	}

	void MScene::OnLButtonDown(int x, int y, unsigned int nFlags)
	{
		_pScene->OnLButtonDown(x, y, nFlags);
		MKeyEventMgr::Instance->OnLButtonDown(x, y, nFlags);
	}
	void MScene::OnLButtonUp(int x, int y, unsigned int nFlags)
	{
		_pScene->OnLButtonUp(x, y, nFlags);
		MKeyEventMgr::Instance->OnLButtonUp(x, y, nFlags);
	}
	void MScene::OnMButtonUp(int x, int y, unsigned int nFlags)
	{
		_pScene->OnMButtonUp(x, y, nFlags);
		MKeyEventMgr::Instance->OnMButtonUp(x, y, nFlags);
	}
	void MScene::OnRButtonUp(int x, int y, unsigned int nFlags)
	{
		MKeyEventMgr::Instance->OnRButtonUp(x, y, nFlags);
	}
	void MScene::OnMouseMove(int x, int y, unsigned int nFlags)
	{
		_pScene->OnMouseMove(x, y, nFlags);
		MKeyEventMgr::Instance->OnMouseMove(x, y, nFlags);
	}
	void MScene::OnMouseWheel(int x, int y, int zDelta, unsigned int nFlags)
	{
		_pScene->OnMouseWheel(x, y, zDelta, nFlags);
		MKeyEventMgr::Instance->OnMouseWheel(x, y, zDelta, nFlags);
	}
	void MScene::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		MKeyEventMgr::Instance->OnKeyDown(nChar, nRepCnt, nFlags);
	}

	void MScene::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		MKeyEventMgr::Instance->OnKeyUp(nChar, nRepCnt, nFlags);
	}
	
	bool MScene::GetCurrentMouseClientPoint(HWND hWnd, POINT& outPT)
	{
		return _pScene->GetCurrentMouseClientPoint(hWnd, outPT);
	}

	bool MScene::ScreenPosToRay(System::Drawing::Point point, MPhysRay% physxray)
	{
		if (!_pScene->GetRender())
			return false;

		PhysRay ray;
		_pScene->ScreenPointToRay(point.X, point.Y, ray);
		physxray.vStart.X = ray.vStart.x;
		physxray.vStart.Y = ray.vStart.y;
		physxray.vStart.Z = ray.vStart.z;
		physxray.vDelta.X = ray.vDelta.x;
		physxray.vDelta.Y = ray.vDelta.y;
		physxray.vDelta.Z = ray.vDelta.z;
		return true;
	}

	bool MScene::RayTraceObject(System::Drawing::Point pt, MPhysRay^ ray, [Out]MPhysRayTraceHit% hitInfo, bool onlyPhysX)
	{
		POINT point;
		point.x = pt.X;
		point.y = pt.Y;
		PhysRay physxray;
		physxray.vStart = Vector3::ToA3DVECTOR3(ray->vStart);
		physxray.vDelta = Vector3::ToA3DVECTOR3(ray->vDelta);

		PhysRayTraceHit rayhit;
		if (_pScene->RayTraceObject(point, physxray, rayhit, onlyPhysX))
		{
			hitInfo.t = rayhit.t;
			hitInfo.vHit = Vector3::FromA3DVECTOR3(rayhit.vHit);
			hitInfo.vNormal = Vector3::FromA3DVECTOR3(rayhit.vNormal);
			hitInfo.obj = GetMIObjBase((IObjBase*)rayhit.UserData);
			return true;
		}
		return false;
	}

	bool MScene::RayTracePhysXObjectAndTerrain(System::Drawing::Point pt, MPhysRay^ ray, [Out]Vector3% HitPos)
	{
		APhysXRayHitInfo hitInfo;
		NxRay nxRay;
		nxRay.orig = APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(ray->vStart));
		nxRay.dir  = APhysXConverter::A2N_Vector3(Vector3::ToA3DVECTOR3(ray->vDelta));
		nxRay.dir.normalize();
		if (!APhysXPicker::Raycast(_pScene->GetAPhysXScene(), nxRay, hitInfo, APX_RFTYPE_ALL))
		{
			A3DVECTOR3 mouseHitPos;
			if (!_pScene->GetTerrainPosByCursor(pt.X, pt.Y, mouseHitPos))
				return false;
			hitInfo.mHitPos = APhysXConverter::A2N_Vector3(mouseHitPos);
		}
		HitPos.X = hitInfo.mHitPos.x;
		HitPos.Y = hitInfo.mHitPos.y;
		HitPos.Z = hitInfo.mHitPos.z;
		return true;
	}
	void MScene::SetDrivenMode(MDrivenMode dm)
	{
		Scene::DrivenMode sdm = static_cast<Scene::DrivenMode>(dm);
		_pScene->SetDrivenMode(sdm);
	}
	MDrivenMode MScene::GetDrivenMode()
	{
		Scene::DrivenMode sdm = _pScene->GetDrivenMode();
		return static_cast<MDrivenMode>(sdm);
	}
	bool MScene::QueryDrivenMode(MDrivenMode dm)
	{
		Scene::DrivenMode sdm = static_cast<Scene::DrivenMode>(dm);
		return _pScene->QueryDrivenMode(sdm);
	}
	void MScene::RegisterTimerEvent(MEventNotify^ notify, float delayTime, MDelayType dt, int nbEvents)
	{
		EventNotifyImp en;
		en.SetEventNotify(notify);
		TimerEvent::DelayType type = static_cast<TimerEvent::DelayType>(dt);
		_pScene->RegisterTimerEvent(en, delayTime, type, nbEvents);
	}
	void MScene::RegisterTimerEvent(MTimerEvent^ MTEvent)
	{
		EventNotifyImp en;
		en.SetEventNotify(MTEvent->GetMEventNotify());
		_pScene->RegisterTimerEvent(&TimerEvent(en, MTEvent->GetDelayTime(), MTEvent->GetType(), MTEvent->GetNbAllEvents()));
	}
	void MScene::UnRegisterTimerEvent(MTimerEvent^ MTEvent)
	{
		EventNotifyImp en;
		en.SetEventNotify(MTEvent->GetMEventNotify());
		_pScene->UnRegisterTimerEvent(&TimerEvent(en, MTEvent->GetDelayTime(), MTEvent->GetType(), MTEvent->GetNbAllEvents()));
	}
	void MScene::RegisterPhysXContactDamage(MActorBase^ MActor)
	{
		if (nullptr != MActor)
			_pScene->RegisterPhysXContactDamage(MActor->GetAAB());
	}
	void MScene::UnRegisterPhysXContactDamage(MActorBase^ MActor)
	{
		if (nullptr != MActor)
			_pScene->UnRegisterPhysXContactDamage(MActor->GetAAB());
	}
	void MScene::SavePhysXDemoScene(System::String^ filepath)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(filepath);
		AString strfile = WC2AS(wch);
		return _pScene->SavePhysXDemoScene(strfile);
	}

	bool MScene::LoadPhysXDemoScene(System::String^ filepath)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(filepath);
		AString strfile = WC2AS(wch);

		return _pScene->LoadPhysXDemoScene(strfile);
	}

	bool MScene::ImportAndExportFFData(bool bIsImport, System::String^ filepath)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(filepath);
		AString strfile = WC2AS(wch);

		return _pScene->GetSelGroup().ImportAndExportFFData(bIsImport, strfile);
	}

	bool MScene::CreateFFSubObject()
	{
		return _pScene->CreateFFSubObject();
	}

	void MScene::ChangeCameraMode(bool bEnforceSyncReplayCam)
	{
		_pScene->ChangeCameraMode(bEnforceSyncReplayCam);
	}
	void MScene::JumpMainActor()
	{
		_pScene->JumpMainActor();
	}
	void MScene::SwitchMainActor()
	{
		int idx = _pScene->SwitchMainActor();
		if (0 > idx)
		{
			m_pCurrentMA = nullptr;
			return;
		}

		m_pCurrentMA = m_MainActors[idx];
		assert(m_pCurrentMA->GetMainActor() == _pScene->GetCurrentActor());
	}
	void MScene::SwitchMainActorWalkRun()
	{
		_pScene->SwitchMainActorWalkRun();
	}

	System::String^ MScene::GetCurrentMainActorCCType()
	{
		const TCHAR* pStr = _pScene->GetCurrentMainActorCCType();

		AString cctype = _TWC2AS(pStr);
		System::String^ str = gcnew System::String(cctype.GetBuffer(0)); 
		
		return str;
	}
	void MScene::SwitchMainActorCCType(MCCType ct)
	{
		CAPhysXCCMgr::CCType cctype = static_cast<CAPhysXCCMgr::CCType>(ct);
		_pScene->SwitchMainActorCCType(cctype);
	}
	void MScene::ChangeMainActorPushForce(float deltaValue)
	{
		_pScene->ChangeMainActorPushForce(deltaValue);
	}

	void MScene::EnableShowTerrianGrid(bool bEnable)
	{
		_pScene->EnableShowTerrianGrid(bEnable);
	}
	bool MScene::IsShowTerrianGridEnabled()
	{
		return _pScene->IsShowTerrianGridEnabled();
	}

	void MScene::EnablePhysXProfile(bool bEnable)
	{
		_pScene->EnablePhysXProfile(bEnable);
	}
	bool MScene::IsPhysXProfileEnabled()
	{
		return _pScene->IsPhysXProfileEnabled();
	}

	void MScene::EnablePhysXDebugRender(bool bEnable)
	{
		_pScene->EnablePhysXDebugRender(bEnable);
	}
	bool MScene::IsPhysXDebugRenderEnabled()
	{
		return _pScene->IsPhysXDebugRenderEnabled();
	}

	void MScene::EnablePhysXHW(bool bEnable)
	{
		_pScene->EnablePhysXHW(bEnable);
	}
	bool MScene::IsPhysHWEnabled()
	{
		return _pScene->IsPhysHWEnabled();
	}

	bool MScene::PhysXSimASingleStep()
	{
		return _pScene->PhysXSimASingleStep();
	}
	void MScene::ChangePhysXPauseState()
	{
		_pScene->ChangePhysXPauseState();
	}
	bool MScene::IsPhysXSimPauseState()
	{
		return _pScene->IsPhysXSimPauseState();
	}

	void MScene::SetRPTMode(int mode)
	{
		if (0 < mode && mode < 5)
			_pScene->SetRPTMode(RagdollPerfTest(mode));
	}
	void MScene::EnumNextRPTMode()
	{
		return _pScene->EnumNextRPTMode();
	}
	void MScene::SwitchAllPhysXState()
	{
		return _pScene->SwitchAllPhysXState();
	}
	void MScene::ControlModelClothes()
	{
		return _pScene->ControlModelClothes();
	}
	void MScene::EntryPlayMode()
	{
		SetCurrentDirectoryA(af_GetBaseDir());
		int randSeed = _pScene->GetRandSeed();
		if (0 == randSeed)
		{
			randSeed = int(System::DateTime::Now.Ticks);
			_pScene->SetRandSeed(randSeed);
		}
		MIScriptBehavior::InitRandom(randSeed);
		_pScene->SetSceneMode(Scene::SCENE_MODE_PLAY_ON);

		m_Objs->Clear();
		m_RoleActors->Clear();
		ReleaseAllMMainActor();

		//init MPhysXObjBase from IPhysXObjBase
		APtrArray<IObjBase*> objs;
		ObjManager::GetInstance()->GetRootLiveObjects(objs, ObjManager::OBJ_TYPEID_ALL);
		int nCount = objs.GetSize();
		for (int i = 0; i < nCount; ++i)
		{
			MIObjBase^ pPhysXObj = MIObjBase::CreateInstance(objs[i], nullptr);
			m_Objs->Add(pPhysXObj);
		}

		IRoleActor* pRA = 0;
		nCount = _pScene->GetNPCActorCount();
		for (int j = 0; j < nCount; ++j)
		{
			pRA = _pScene->GetNPCActor(j);
			if (0 == pRA)
				continue;
			MIRoleActor^ pMRA = gcnew MIRoleActor(pRA, GetMIObjBase(pRA->GetDynObj()));
			m_RoleActors->Add(pMRA);
			RegisterPhysXContactDamage(pMRA->GetMActorBase());
		}

		assert(0 == _pScene->GetMainActorCount());
		InitScriptForPlayMode();
	}

	void MScene::InitScriptForPlayMode()
	{
		if (_pScene->GetScripts().size() == 0)
		{
			//Add default script if non
			std::vector<AString> scripts;
			scripts.push_back("SceneScript.cs");
			_pScene->SetScripts(scripts);
		}

		String^ errMsg = gcnew String("");
		ScriptFactory::Instance->Compile(ScriptFolder, errMsg);
		if (errMsg != "")
			System::Windows::Forms::MessageBox::Show(errMsg);
		
		MKeyEventMgr::Instance->UnRegisterAll();
		ScriptStart();
	}

	void MScene::ResetScene()
	{
		ScriptEnd();
		ReleaseAllMMainActor();
		_pScene->SetSceneMode(Scene::SCENE_MODE_EMPTY);
		m_Objs->Clear();
		m_RoleActors->Clear();
	}

	CRender* MScene::GetRender()
	{
		return _pScene->GetRender();
	}

	MIObjBase^ MScene::GetMIObjBase(const IObjBase* pObjBase)
	{
		if (pObjBase == NULL)
			return nullptr;

		for (int i = 0; i < m_Objs->Count; i++)
		{
			MIObjBase^ obj = m_Objs[i]->GetMIObjBase(pObjBase);
			if (obj != nullptr)
				return obj;
		}
		return nullptr;
	}

	MIObjBase^ MScene::GetMIObjBase(System::String^ objName)
	{
		if (objName == nullptr)
			return nullptr;

		for (int i = 0; i < m_Objs->Count; i++)
		{
			MIObjBase^ obj = m_Objs[i]->GetMIObjBase(objName);
			if (obj != nullptr)
				return obj;
		}
		return nullptr;
	}

	List<MIObjBase^>^ MScene::GetMIObjBase(MObjTypeID TypeID)
	{
		List<MIObjBase^>^ objs = gcnew List<MIObjBase^>();
		for(int i = 0; i < m_Objs->Count; i++)
		{
			ObjManager::ObjTypeID type = m_Objs[i]->GetIObjBase()->GetProperties()->GetObjType();
			if ((int)TypeID & (int)type)
				objs->Add(m_Objs[i]);
		}
		return objs;
	}

	void MScene::AddSceneScript(MIScriptBehavior^ scriptbehavior)
	{
		m_scriptBehaviors->Add(scriptbehavior);
	}
	void MScene::RemoveSceneScript(MIScriptBehavior^ scriptbehavior)
	{
		m_scriptBehaviors->Remove(scriptbehavior);
	}
	void MScene::RemoveAllSceneScript()
	{
		m_scriptBehaviors->Clear();
	}

	//script operation
	void MScene::ScriptStart()
	{
		//scene script
		__super::ScriptStart();

		//obj script
		for (int i = 0; i < m_Objs->Count; i++)
		{
			m_Objs[i]->ScriptStart();
		}
	}
	void MScene::ScriptEnd()
	{
		//scene script
		__super::ScriptEnd();

		//obj script
		for (int i = 0; i < m_Objs->Count; i++)
		{
			m_Objs[i]->ScriptEnd();
		}
	}
	void MScene::ScriptPreTick(float dtSec)
	{
		//scene script
		__super::ScriptPreTick(dtSec);

		//obj script
		for (int i = 0; i < m_Objs->Count; i++)
		{
			m_Objs[i]->ScriptPreTick(dtSec);
		}
	}
	void MScene::ScriptPostTick(float dtSec)
	{
		//scene script
		__super::ScriptPostTick(dtSec);

		//obj script
		for (int i = 0; i < m_Objs->Count; i++)
		{
			m_Objs[i]->ScriptPostTick(dtSec);
		}
	}

	float MScene::GetTerrainHeight(float x, float z)
	{
		A3DVECTOR3 vPos(x, 0, z);

		float height;
		if (::GetTerrainHeight(GetTerrain(), vPos, height))
			return height;
		
		// assert(0);
		return 0;
	}

	bool MScene::GetTerrainPosByCursor(int x, int y, Vector3% vOutPos)
	{
		A3DVECTOR3 vPos;
		if (_pScene->GetTerrainPosByCursor(x, y, vPos))
		{
			vOutPos = Vector3::FromA3DVECTOR3(vPos);
			return true;
		}
		return false;
	}
	MIAssailable^ MScene::GetMIAssailable(IAssailable* pIA)
	{
		if (0 == pIA)
			return nullptr;

		MActorBase^ mab = nullptr;
		for (int i = 0; i < m_RoleActors->Count; ++i)
		{
			mab = m_RoleActors[i]->GetMActorBase();
			if (mab->GetAAB() == pIA)
				return mab;
		}
		for (int j = 0; j < m_MainActors->Count; ++j)
		{
			mab = m_MainActors[j]->GetMActorBase();
			if (mab->GetAAB() == pIA)
				return mab;
		}

		IPhysXObjBase* pPhysXObj = dynamic_cast<IPhysXObjBase*>(pIA);
		if (0 != pPhysXObj)
		{
			for (int i = 0; i < m_Objs->Count; i++)
			{
				MIObjBase^ obj = m_Objs[i]->GetMIObjBase(pPhysXObj);
				MIPhysXObjBase^ PhysXObj = dynamic_cast<MIPhysXObjBase^>(obj);
				if (PhysXObj != nullptr)
					return PhysXObj;
			}
		}
		return nullptr;
	}

	MIAssailable^ MScene::GetMIAssailable(MIObjBase^ MObj)
	{
		if (nullptr == MObj)
			return nullptr;

		IObjBase* pObj = MObj->GetIObjBase();
		assert(0 != pObj);
		ObjManager::ObjTypeID ot = pObj->GetProperties()->GetObjType();
		if (!ObjManager::IsPhysXObjType(ot))
			return nullptr;

		IAssailable* pIA = _pScene->GetActorBase(pObj);
		if (0 == pIA)
		{
			IPhysXObjBase* pPhysXObj = dynamic_cast<IPhysXObjBase*>(pObj);
			pIA = pPhysXObj;
		}
		return GetMIAssailable(pIA);
	}

	MActorBase^ MScene::GetMActorBase(ApxActorBase* pAAB)
	{
		if (0 == pAAB)
			return nullptr;

		MActorBase^ mab = nullptr;
		for (int i = 0; i < m_RoleActors->Count; ++i)
		{
			mab = m_RoleActors[i]->GetMActorBase();
			if (mab->GetAAB() == pAAB)
				return mab;
		}
		for (int j = 0; j < m_MainActors->Count; ++j)
		{
			mab = m_MainActors[j]->GetMActorBase();
			if (mab->GetAAB() == pAAB)
				return mab;
		}
		return nullptr;
	}

	MActorBase^ MScene::GetMActorBase(MIObjBase^ MObj)
	{
		if (nullptr == MObj)
			return nullptr;

		for (int i = 0; i < m_RoleActors->Count; ++i)
		{
			if (m_RoleActors[i]->GetPhysXObjBase() == MObj)
				return m_RoleActors[i]->GetMActorBase();
		}
		for (int j = 0; j < m_MainActors->Count; ++j)
		{
			if (m_MainActors[j]->GetPhysXObjBase() == MObj)
				return m_MainActors[j]->GetMActorBase();
		}
		return nullptr;
	}

	MMainActor^	MScene::CreateMainActor(Vector3 pos, System::String^ strEcmFile)
	{
		CMainActor *pActor = nullptr;
		A3DVECTOR3 vPos = Vector3::ToA3DVECTOR3(pos);
		if (strEcmFile != nullptr)
		{
			pin_ptr<const wchar_t> wch = PtrToStringChars(strEcmFile);
			AString strfile = WC2AS(wch);
			pActor = _pScene->CreateMainActor(vPos, strfile);
		}
		else
			pActor = _pScene->CreateMainActor(vPos);
				
		if (pActor == nullptr)
			return nullptr;

		CPhysXObjDynamic* pDynObj = pActor->GetDynObj();
		if (nullptr == pDynObj)
			return nullptr;

		MPhysXObjDynamic^ pObj = gcnew MPhysXObjDynamic(pDynObj, nullptr);
		m_Objs->Add(pObj);
		// for children objects(e.x. weapon), there is no corresponding M-objects

		MMainActor^ actor = gcnew MMainActor(pActor, pObj);
		m_MainActors->Add(actor);
		if (nullptr == m_pCurrentMA)
			m_pCurrentMA = actor;

		CDynProperty* pProp = pDynObj->GetProperties();
		int nCount = pProp->GetScriptCount();
		for (int i = 0; i < nCount; ++i)
		{
			System::String^ scriptName = gcnew System::String(pProp->GetScriptName(i));
			MIScriptBehavior^ behavior = (MIScriptBehavior^)ScriptFactory::Instance->CreateInstanceFromFile(scriptName);
			pObj->AddScript(behavior);
			behavior->ObjBase = pObj;
			behavior->Start();
		}

		System::String^ defFile = DefScriptFile;
		if (defFile != "")
		{
			MIScriptBehavior^ behavior = (MIScriptBehavior^)ScriptFactory::Instance->CreateInstanceFromFile(defFile);
			pObj->InserScript(0, behavior);
			behavior->ObjBase = pObj;
			behavior->Start();
		}
		return actor;
	}

	void MScene::ReleaseAllMMainActor()
	{
		for (int i = 0; i < m_MainActors->Count; ++i)
			_pScene->ReleaseMainActor(m_MainActors[i]->GetMainActor());
		m_MainActors->Clear();
		m_pCurrentMA = nullptr;
	}

	MCameraController^ MScene::GetMNormalCamCtrler()
	{
		return m_pNormalCamCtrler;
	}	
	MCCamCtrlAgent^ MScene::GetMCamCtrlerAgent()
	{
		return m_pCamCtrlAgent;
	}	
	MCameraController^ MScene::GetActiveSmartCamCtrler(bool bIsReferReproEvent)
	{
		return gcnew MCameraController(_pScene->GetActiveSmartCamCtrl(bIsReferReproEvent));
	}

	void MScene::ScriptOnRender(MRender^ render)
	{
		//scene script
		__super::ScriptOnRender(render);

		//obj script
		for (int i = 0; i < m_Objs->Count; i++)
		{
			m_Objs[i]->ScriptOnRender(render);
		}
	}

	APhysXScene& MScene::GetAPhysXScene()
	{
		return *_pScene->GetAPhysXScene();
	}
	
	String^ MScene::DefScriptFile::get()
	{
		AString defScript = _pScene->GetDefScript();
		return gcnew String(defScript.operator const char*());
	}
	void MScene::DefScriptFile::set(String^ value)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(value);
		AString defscript = WC2AS(wch);
		_pScene->SetDefScript(defscript);
	}

	array<System::String^>^ MScene::GetScriptNames()
	{
		const std::vector<AString>& scripts = _pScene->GetScripts();
		array<System::String^>^ scriptnames = gcnew array<System::String^>(int(scripts.size()));
		for (size_t i = 0; i < scripts.size(); i++)
		{
			String^ strname = gcnew String(scripts[i].operator const char*());
			scriptnames[int(i)] = strname;
		}
		return scriptnames;
	}

	void MScene::SetScriptNames(array<System::String^>^ scripts)
	{
		std::vector<AString> scriptnames;
		for (int i = 0; i < scripts->Length; i++)
		{
			pin_ptr<const wchar_t> wch = PtrToStringChars(scripts[i]);
			AString script = WC2AS(wch);
			scriptnames.push_back(script);
		}
		_pScene->SetScripts(scriptnames);
	}

	void MScene::DumpPhysX()
	{
		char szFile[1024],tbuf[1024];
		strcpy(szFile, af_GetBaseDir());
		struct tm *today;
		time_t long_time;
		time( &long_time );					/* Get time as long integer. */
		today = localtime( &long_time );	/* Convert to local time. */
		strftime(tbuf,100,"\\PhysX\\%y-%m-%d-%H-%M-%S.xml",today);
		strcat(szFile,tbuf);
		gPhysXEngine->CoreDumpPhysX(szFile, APhysXEngine::APX_COREDUMP_FT_XML);
	}

	bool MScene::RemoveChild(IObjBase* pParent, IObjBase* pObj)
	{
		return pParent->RemoveChild(pObj);
	}
	bool MScene::AddChild(IObjBase* pParent, IObjBase* pObj)
	{
		return pParent->AddChild(pObj);
	}

	//get client rect
	bool MScene::GetClientRect(long% width, long% height)
	{
		if (MInput::Instance->_hwnd == NULL)
			return false;

		RECT rect;
		bool bRet = ::GetClientRect(MInput::Instance->_hwnd, &rect) ? true : false;
		if (bRet)
		{
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
		}
		return bRet;
	}

	bool MScene::UpdatePropForBuildScene(MIObjBase^ MObj)
	{
		if (0 == _pScene)
			return false;

		if (nullptr == MObj)
			return false;

		IObjBase* pObj = MObj->GetIObjBase(); 
		return _pScene->UpdatePropForBuildScene(pObj);
	}

	void MScene::ToBreakAndRagdoll()
	{
		if (0 != _pScene)
			_pScene->ToBreakAndRagdoll();
	}
}