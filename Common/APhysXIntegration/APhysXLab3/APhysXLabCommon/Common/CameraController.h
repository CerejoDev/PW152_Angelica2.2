/*
 * FILE: CameraController.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/03
 *
 * HISTORY: 
 * 
 * 2011.7.6 reversed by yx
 * base class ApxObjBase
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_CAMERA_CONTROLLER_H_
#define _APHYSXLAB_CAMERA_CONTROLLER_H_

#include "Recording.h"

enum CameraMode
{
	MODE_FREE_FLY  = 0,//free camera mode
	MODE_BINDING   = 1,//binding mode
};

enum CameraMoveFlag
{
	CMF_NONE			 = 0,
	CMF_MOVE_FORWARD     = (1<<0),
	CMF_MOVE_BACKWARD    = (1<<1),
	CMF_MOVE_LEFTSTRAFE	 = (1<<2),
	CMF_MOVE_RIGHTSTRAFE = (1<<3),
	CMF_MOVE_UP			 = (1<<4),
	CMF_MOVE_DOWN		 = (1<<5),
	CMF_HIGH_SPEED		 = (1<<6),
};

enum ItemCode
{
	IC_MODE			= (1<<0),
	IC_RADIUS		= (1<<1),
	IC_FOCUSPOS		= (1<<2),

	IC_POS			= (1<<3),
	IC_DIR			= (1<<4),
	IC_UP			= (1<<5),

	IC_BACKPOS		= (1<<6),
	IC_BACKDIR		= (1<<7),
	IC_BACKUP		= (1<<8),
	IC_BACK_FLAG	= (1<<9),
};

class CECWrapper
{
public:
	CECWrapper();
	void InitCamPose(A3DCamera& cam);
	void SyncCoreDataFrom(const CameraExtCtrl& inOther, RecItemType dirtyItem);
	void SyncCoreDataTo(CameraExtCtrl& outOther, RecItemType dirtyItem) const;

	APhysXU32 GetVersion() const { return 0xCF000001; }
	bool LoadData(NxStream& stream, bool& outIsLowVersion);
	void SaveData(NxStream& stream);

	CameraMode GetMode() const { return CameraMode(m_cecData.nMode); }
	void SetMode(CameraMode cm, int* pInOutItemCodes);

	float GetRadius() const { return m_cecData.fRadius; }
	bool SetRadius(float r, int* pInOutItemCodes);

	const A3DVECTOR3& GetFocusPos() const { return m_cecData.vFocusPos; }
	void SetFocusPos(const A3DVECTOR3& fp, int* pInOutItemCodes);

	const A3DVECTOR3& GetCamBKPos() const { return m_cecData.camPosBack; }
	const A3DVECTOR3& GetCamBKDir() const { return m_cecData.camDirBack; }
	const A3DVECTOR3& GetCamBKUp() const { return m_cecData.camUpBack; }
	bool GetCamBackUpFlag() const { return m_cecData.bHasBackUpPose; }
	void SetCamBackUp(const A3DVECTOR3& p, const A3DVECTOR3& d, const A3DVECTOR3& u, int* pInOutItemCodes);
	void SetCamBackUpFlag(bool bVal, int* pInOutItemCodes);
	void SetCamBackUpPos(const A3DVECTOR3& p, int* pInOutItemCodes);
	void SetCamBackUpDir(const A3DVECTOR3& d, int* pInOutItemCodes);
	void SetCamBackUpUp(const A3DVECTOR3& u, int* pInOutItemCodes);

private:
	CameraExtCtrl m_cecData;
};

class CRecording;

class CCameraController : public IObjBase, public IPropObjBase
{
protected:
	//operations inherit from ApxObjBase
	virtual void OnUpdateGRot();
	virtual void OnUpdateGPos();									

	virtual void OnTick(float dt);
	virtual bool OnDoWakeUpTask(IEngineUtility& eu, APhysXScene* pPSToRuntime) { return IObjBase::OnDoWakeUpTask(eu, pPSToRuntime); }
	//operations inherit from IObjBase
	virtual bool OnResetPose(){ return true; }
	virtual bool OnGetPos(A3DVECTOR3& vOutPos) const;
	virtual void OnSetPos(const A3DVECTOR3& vPos);
	virtual bool OnGetDir(A3DVECTOR3& vOutDir) const;
	virtual bool OnGetUp(A3DVECTOR3& vOutUp) const;
	virtual void OnSetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	virtual bool OnGetPose(A3DMATRIX4& matOutPose, bool& outWithScaled) const;
	virtual void OnSetPose(const A3DMATRIX4& matPose);
	virtual bool OnSetScale(const float& scale) { return false; }
	virtual void BuildExtraRenderDataMyself(ExtraRenderable& er, int dwColor){}

	virtual bool SaveToFile(NxStream& nxStream) { m_CoreData.SaveData(nxStream); return true; }
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion) { return m_CoreData.LoadData(nxStream, outIsLowVersion); }
	virtual bool OnSendToPropsGrid() { return true; }
	virtual bool OnPropertyChanged(PropItem& prop) { return true; }
	virtual void OnUpdatePropsGrid() {}
	virtual IPropObjBase* CreateMyselfInstance(bool snapValues) const { return NULL; }
	virtual void EnterRuntimePreNotify(APhysXScene& aPhysXScene);
 	virtual void LeaveRuntimePostNotify();
	virtual ApxObjBase* NewObjAndCloneData() const;

	virtual void OnFocusObjChanged();
	virtual void OnUpdatePosChanged(const NxVec3& deltaMove);

private:
	virtual void OnDoSleepTask() {}
	virtual	bool OnEnterRuntime(APhysXScene& aPhysXScene){ return true; }
	virtual void OnLeaveRuntime() {}

public:
	CCameraController(A3DCamera& InitedCamera, TString* pName = 0); 
	~CCameraController(); 
	A3DCamera& GetCamera() const { return m_Camera; }
	void SetScreenSize(const int x, const int y);
	void SetUserInputFlagsForTickFreeFly(int flags);

	void LogChanges(CRecording& rec);
	int ReproChanges(CRecording& rec);
	void SyncCameraData(const CCameraController& other, int changeFlags);
	void ClearItemChangeFlags() { m_ItemChangeFlags = 0; } 

	//return the bing position according to the camera position
	bool GetDefBindPos(A3DVECTOR3& outFocusPos, A3DViewport* pVP = 0) const;

	CameraMode GetMode() const { return m_CoreData.GetMode(); };
	bool QueryMode(const CameraMode& mode) const { return mode == m_CoreData.GetMode(); }

	void EntryBindingMode(IObjBase* pBindObj, const bool bStorePose = false, const bool IsKeepDir  = false);
	void EntryFreeFlyMode(const bool bRestorePose = false);

	float GetFocusFactorPercent() const;
	void UpdateFocusFactor(const int zDelta);
	void RotateCamera(const int dx, const int dy);

	void EnableMoveCamera(const bool bEnable) { m_bEnableMove = bEnable; }
	void AddTerrainHeight(A3DTerrain2& Terrain);

	void SetTerrain(A3DTerrain2* pTerr) { m_pTerr = pTerr; }
	A3DTerrain2* GetTerrain() const { return m_pTerr; }

private:
	void SetFocusPosition(const A3DVECTOR3& pos);
	A3DVECTOR3 CalculateCameraPosition(A3DTerrain2& Terrain) const;
	void SetBindPosition(bool bKeepDir = false);

	void ApplyUserInputForFreeFly(int cmfs, float deltaTimeSec);
	A3DVECTOR3& GetPosGroundCull(A3DVECTOR3& Pos, A3DTerrain2& Terrain) const;

	bool HasVerticalRotation(const int dy, const A3DVECTOR3& vDir);
	void RotateCameraBinding(const int dx, const int dy, A3DTerrain2* pTerrain = 0);
	void RotateCameraFreeFly(const int dx, const int dy);
	
private:
	// forbidden behavior
	CCameraController(const CCameraController& rhs);
	CCameraController& operator= (const CCameraController& rhs)
		{ IPropObjBase::operator=(rhs); return *this; }
public:
	static const A3DVECTOR3 g_vUp;
	static const float g_DefFactor;
	static const float g_MinFactor;
	static const float g_MaxFactor;

private:
	int			m_ItemChangeFlags;
	int         m_UserInputFreFlyFlags;

	int			m_nWidth;
	int			m_nHeight;
	bool		m_bEnableMove;
	A3DCamera&	m_Camera;
	CECWrapper	m_CoreData;
	A3DTerrain2* m_pTerr;
};

class ICamOptTask
{
public:
	ICamOptTask();
	void SetWorker(CCameraController& worker) { m_pWorker = &worker; }
	bool ExecuteTask();

private:
	virtual void OnExecuteTask(CCameraController& worker) = 0;

private:
	CCameraController* m_pWorker;
};

class CamOptTask_EntryFreeFlyMode : public ICamOptTask
{
public:
	CamOptTask_EntryFreeFlyMode(bool bRestorePose) { m_bRestorePose = bRestorePose; }
	bool m_bRestorePose;

private:
	virtual void OnExecuteTask(CCameraController& worker);
};

class CamOptTask_EntryBindingMode : public ICamOptTask
{
public:
	CamOptTask_EntryBindingMode(IObjBase* pBindObj, const bool bStorePose = false, const bool IsKeepDir = false);
	IObjBase* m_pBindObj;
	bool m_bStorePose;
	bool m_bIsKeepDir;

private:
	virtual void OnExecuteTask(CCameraController& worker);
};

class CamOptTask_RotateCamera : public ICamOptTask
{
public:
	CamOptTask_RotateCamera(int dx, int dy) { m_dx = dx; m_dy = dy; }
	int m_dx;
	int m_dy;

private:
	virtual void OnExecuteTask(CCameraController& worker);
};

class CamOptTask_UpdateFocusFactor : public ICamOptTask
{
public:
	CamOptTask_UpdateFocusFactor(int zDelta) { m_zDelta = zDelta; }
	int m_zDelta;

private:
	virtual void OnExecuteTask(CCameraController& worker);
};

class CCamCtrlAgent
{
public:
	CCamCtrlAgent(CCameraController& normalCam, CRecording& InitRec);
	~CCamCtrlAgent();

	void NotifyReplayBegin(A3DCamera& reproCam);
	void NotifyReplayEnd();

	bool IsTheSameWithReplayCamera() const { return m_bIsSameReplay; }
	bool EnforceSyncReplayCamera();

	const CCameraController* GetActiveCamera(bool bIsReferReproEvent = false) const;
	void ExecuteCameraOperation(ICamOptTask& icot);
	void Tick(float dtSec, bool bIsPause);

private:
	int TickUserInputForFreeFly(bool bQueryOnly);

private:
	bool m_bIsSameReplay;
	CRecording& m_Recording;
	CCameraController* m_pNormalCam;
	CCameraController* m_pReplayCam;
};
#endif