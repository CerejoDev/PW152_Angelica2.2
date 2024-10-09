#pragma once

#using <System.Drawing.dll>
#using <mscorlib.dll>

#include "MMainActior.h"
#include "MTimerEvent.h"

//////////////////////////////////////////////////////////////////////////
//managed wrapper for scene
//////////////////////////////////////////////////////////////////////////
class Scene;
class CCameraController;

#pragma make_public(Scene)

namespace APhysXCommonDNet
{
	value class Vector3;
	ref class MIPhysXObjBase;
	ref class MIScriptBehavior;
	ref class MCCamCtrlAgent;
	ref class MCameraController;
	ref class MRender;
	ref class MActorBase;

	using System::Runtime::InteropServices::OutAttribute;

	public enum class MObjTypeID{
		//according to ObjManager::ObjTypeID
		OBJ_TYPEID_EMPTY		= 0,
		OBJ_TYPEID_STATIC		= (1<<0),
		OBJ_TYPEID_SKINMODEL	= (1<<1),
		OBJ_TYPEID_ECMODEL		= (1<<2),
		OBJ_TYPEID_PARTICLE		= (1<<3),
		OBJ_TYPEID_FORCEFIELD	= (1<<4),
		OBJ_TYPEID_REGION		= (1<<5),
		OBJ_TYPEID_DUMMY		= (1<<6),
		OBJ_TYPEID_SMSOCKET		= (1<<7),
		OBJ_TYPEID_SOCKETSM		= (1<<8),
		OBJ_TYPEID_SPOTLIGHT	= (1<<9),
		OBJ_TYPEID_CAMERA		= (1<<10),

		OBJ_TYPEID_PHYSX		= OBJ_TYPEID_STATIC | OBJ_TYPEID_SKINMODEL | OBJ_TYPEID_ECMODEL | OBJ_TYPEID_PARTICLE | OBJ_TYPEID_FORCEFIELD,
		OBJ_TYPEID_ALL			= 0xffffffff,
	};

	public value struct MPhysRay
	{
		Vector3 vStart;
		Vector3 vDelta;
	};

	public value struct MPhysRayTraceHit
	{
		Vector3 vHit;
		Vector3 vNormal;

		float t;
		MIObjBase^ obj;
	};

	public delegate void RenderExtraHandler(MRender^ render);
	public ref class MScene : MIObjBase
	{
	public:
		static APhysXU32 GetVersion();
		static property MScene^ Instance 
		{ 
			MScene^ get() { return _instance; }
		}
		static int GetAngelicaSDKVersion() { return ::GetAngelicaSDKVersion(); }

		event RenderExtraHandler^ renderExtraEvent;

	public:
		MScene();
		MScene(Scene* pSecne);
		!MScene(){ Release();}
		virtual ~MScene();

		virtual bool Init(System::IntPtr hInst, System::IntPtr hWnd, bool bFullScreen);
		virtual void Release();
		MCameraController^ GetMNormalCamCtrler();
		MCameraController^ GetActiveSmartCamCtrler(bool bIsReferReproEvent);
		MCCamCtrlAgent^ GetMCamCtrlerAgent();
		A3DTerrain2*	GetTerrain();
		A3DSkySphere*	GetSky();

		bool Replay(System::String^ filepath);
		bool QueryAsyncKeyDown(int vKey) { return _pScene->QueryAsyncKeyDown(vKey); }

		bool		LoadTerrainAndPhysXTerrain(System::String^ szFile);
		MIObjBase^  CreateObjectRuntime(MObjTypeID objType, System::String^ szFile, Vector3 pos, bool OnGround, bool isEnterRuntime, System::String^ strObjName, System::String^ strScriptName, bool bUseDefScript);
		void		ReleaseObject(MIObjBase^ pObject);

		virtual bool Tick();
		virtual bool Render();
		void RenderNextFrameInfo() { _pScene->RenderNextFrameInfo(); }

		//override it
		virtual void OnLButtonDown(int x, int y, unsigned int nFlags);
		virtual void OnLButtonUp(int x, int y, unsigned int nFlags);
		virtual void OnMButtonUp(int x, int y, unsigned int nFlags);
		virtual void OnRButtonUp(int x, int y, unsigned int nFlags);
		virtual void OnMouseMove(int x, int y, unsigned int nFlags);
		virtual void OnMouseWheel(int x, int y, int zDelta, unsigned int nFlags);
		virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

		bool GetCurrentMouseClientPoint(HWND hWnd, POINT& outPT);
		bool ScreenPosToRay(System::Drawing::Point point, MPhysRay% physxray); 
		bool RayTraceObject(System::Drawing::Point pt, MPhysRay^ ray, [Out]MPhysRayTraceHit% hitInfo, bool onlyPhysX);
		bool RayTracePhysXObjectAndTerrain(System::Drawing::Point pt, MPhysRay^ ray, [Out]Vector3% HitPos);

		void	   SetDrivenMode(MDrivenMode dm);
		MDrivenMode GetDrivenMode();
		bool	   QueryDrivenMode(MDrivenMode dm);

		void RegisterTimerEvent(MEventNotify^ notify, float delayTime, MDelayType dt, int nbEvents);
		void RegisterTimerEvent(MTimerEvent^ MTEvent);
		void UnRegisterTimerEvent(MTimerEvent^ MTEvent);
		void RegisterPhysXContactDamage(MActorBase^ MActor);
		void UnRegisterPhysXContactDamage(MActorBase^ MActor);

		void SavePhysXDemoScene(System::String^ filepath);
		bool LoadPhysXDemoScene(System::String^ filepath);
		bool ImportAndExportFFData(bool bIsImport, System::String^ filepath);
		bool CreateFFSubObject();

		void ChangeCameraMode(bool bEnforceSyncReplayCam);
		void JumpMainActor();
		void SwitchMainActor();
		void SwitchMainActorWalkRun();
		System::String^ GetCurrentMainActorCCType();
		void SwitchMainActorCCType(MCCType ct);
		void ChangeMainActorPushForce(float deltaValue);

		void EnableShowTerrianGrid(bool bEnable);
		bool IsShowTerrianGridEnabled();

		void EnablePhysXProfile(bool bEnable);
		bool IsPhysXProfileEnabled();

		void EnablePhysXDebugRender(bool bEnable);
		bool IsPhysXDebugRenderEnabled();

		void EnablePhysXHW(bool bEnable);
		bool IsPhysHWEnabled();

		bool PhysXSimASingleStep();
		void ChangePhysXPauseState();
		bool IsPhysXSimPauseState();

		void SwitchAllPhysXState();
		void ControlModelClothes();
		void EntryPlayMode();
		void EnumNextRPTMode();
		void SetRPTMode(int mode);

		//set scene to empty
		void ResetScene();
		CRender* GetRender();
		MRender^ GetMRender() { return m_render; }

		//retrieve obj according to objName
		MIObjBase^ GetMIObjBase(System::String^ objName);
		//retrieve obj according to IObjBase
		MIObjBase^ GetMIObjBase(const IObjBase* pObjBase);
		//retrieve obj according to type
		System::Collections::Generic::List<MIObjBase^>^ GetMIObjBase(MObjTypeID TypeID);

		bool RemoveChild(IObjBase* pParent, IObjBase* pObj);
		bool AddChild(IObjBase* pParent, IObjBase* pObj);

		//add & remove script
		void AddSceneScript(MIScriptBehavior^ scriptbehavior);
		void RemoveSceneScript(MIScriptBehavior^ scriptbehavior);
		void RemoveAllSceneScript();

		float GetTerrainHeight(float x, float z);
		bool GetTerrainPosByCursor(int x, int y, [Out]Vector3% vOutPos);

		MMainActor^	CreateMainActor(Vector3 pos, System::String^ strEcmFile);
		MMainActor^ GetCurrentMA() { return m_pCurrentMA; }
		MMainActor^ GetMainActor(int index) { assert(index >= 0 && index < m_MainActors->Count); return m_MainActors[index]; }
		int			GetMainActorCount() { return m_MainActors->Count; }

		MIRoleActor^ GetNPCActor(int index) { assert(index >= 0 && index < m_RoleActors->Count); return m_RoleActors[index]; }
		int			 GetNPCActorCount() { return m_RoleActors->Count; }

		MActorBase^ GetMActorBase(ApxActorBase* pAAB);
		MActorBase^ GetMActorBase(MIObjBase^ MObj);
		MIAssailable^ GetMIAssailable(IAssailable* pIA);
		MIAssailable^ GetMIAssailable(MIObjBase^ MObj);

		virtual array<System::String^>^ GetScriptNames() override;
		void SetScriptNames(array<System::String^>^ scripts);

		//dump physx data to file(\\PhysX\\%y-%m-%d-%H-%M-%S.xml)
		void DumpPhysX();

		//get client rect
		bool GetClientRect(long% width, long% height);
		bool UpdatePropForBuildScene(MIObjBase^ MObj);
		void ToBreakAndRagdoll();

	internal:
		//script operation
		virtual void ScriptStart() override;
		virtual void ScriptEnd() override;
		virtual void ScriptPreTick(float dtSec) override;
		virtual void ScriptPostTick(float dtSec) override;

		void InitScriptForPlayMode();
		void ReleaseAllMMainActor();
		
		//script defined render
		virtual void ScriptOnRender(MRender^ render) override;

		APhysXScene& GetAPhysXScene();
		Scene* GetScene() { return _pScene; }

	private:
		void CommonConstructor();

public:
	property System::String^ DefScriptFile
	{
		System::String^ get();
		void set(System::String^ value);
	}
	property System::String^ ScriptFolder
	{
		System::String^ get() { return m_ScriptFolder; }
		void set(System::String^ value) { m_ScriptFolder = value; }
	}
	property Vector3 Gravity
	{
		Vector3 get() 
		{ 
			NxVec3 vGravity;
			_pScene->GetAPhysXScene()->GetNxScene()->getGravity(vGravity);
			return Vector3(vGravity.x, vGravity.y, vGravity.z);
		}
	}


protected:
	static MScene^ _instance = nullptr;

	Scene* _pScene; 
	bool m_bReleaseScene;

	//root object
	System::Collections::Generic::List<MIObjBase^>^  m_Objs;
	System::Collections::Generic::List<MMainActor^>^ m_MainActors;
	MMainActor^ m_pCurrentMA;

	System::Collections::Generic::List<MIRoleActor^>^ m_RoleActors;

	MCCamCtrlAgent^ m_pCamCtrlAgent;
	MCameraController^ m_pNormalCamCtrler;
	MRender^  m_render;

	//script folder
	System::String^ m_ScriptFolder;

internal:
	static void _renderExtra(CRender* pRender);
	};
}