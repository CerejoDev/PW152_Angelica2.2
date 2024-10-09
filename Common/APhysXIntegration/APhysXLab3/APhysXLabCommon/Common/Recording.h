/*
 * FILE: Recording.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2012/01/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2012 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_RECORDING_H_
#define _APHYSXLAB_RECORDING_H_

#include <ADiskFile.h>
#include "SimpleAsyncRecorder.h"

class IUserInputEvent
{
public:
	virtual void EntryPlayMode() {}

	virtual void eventOnKeyDown(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags) {}
	virtual void eventOnKeyUp(unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags) {}

	virtual void eventOnLButtonDblClk(int x, int y, unsigned int nFlags) {}
	virtual void eventOnLButtonDown(int x, int y, unsigned int nFlags) {}
	virtual void eventOnLButtonUp(int x, int y, unsigned int nFlags) {}
	virtual void eventOnMButtonDblClk(int x, int y, unsigned int nFlags) {}
	virtual void eventOnMButtonDown(int x, int y, unsigned int nFlags) {}
	virtual void eventOnMButtonUp(int x, int y, unsigned int nFlags) {}
	virtual void eventOnRButtonDblClk(int x, int y, unsigned int nFlags) {}
	virtual void eventOnRButtonDown(int x, int y, unsigned int nFlags) {}
	virtual void eventOnRButtonUp(int x, int y, unsigned int nFlags) {}
	virtual void eventOnMouseMove(int x, int y, unsigned int nFlags) {}
	virtual void eventOnMouseWheel(int x, int y, int zDelta, unsigned int nFlags) {}
};

enum OptType
{
	OT_KEYBOARD   = 0,
	OT_MOUSE	  = 1,
	OT_SIZE		  = 2,
};

class CRecording;

class IOptEvent
{
public:
	virtual APhysXU32 GetVersion() const = 0;
	virtual OptType	GetOptType() const = 0;

	virtual IOptEvent* Clone() const = 0;
	virtual bool LoadData(NxStream& stream, bool& outIsLowVersion) = 0;
	virtual void SaveData(NxStream& stream) = 0;
	virtual void ExecuteOptEvent(IUserInputEvent& uie, CRecording& rec) const = 0;
	virtual void SaveDebugString(ADiskFile& adfDebug) const = 0;

	void AddUserDebugString(const AString& str);
	void SaveUserDebugData(ADiskFile& adfDebug);

private:
	typedef std::vector<AString>	arrayString;
	arrayString m_strDebug;
};

class OptKeyBoard : public IOptEvent
{
public:
	OptKeyBoard();
	OptKeyBoard(bool bIsDown, unsigned int nChar, unsigned int nRepCnt, unsigned int nFlags);
	virtual APhysXU32 GetVersion() const { return 0x0F000001; }
	virtual OptType	GetOptType() const { return OT_KEYBOARD; }

	virtual IOptEvent* Clone() const;
	virtual bool LoadData(NxStream& stream, bool& outIsLowVersion);
	virtual void SaveData(NxStream& stream);
	virtual void ExecuteOptEvent(IUserInputEvent& uie, CRecording& rec) const;
	virtual void SaveDebugString(ADiskFile& adfDebug) const;

private:
	bool m_bIsKeyDown;
	unsigned int m_nChar;
	unsigned int m_nRepCnt;
	unsigned int m_nFlags;
};

enum OptMouseType
{
	OMT_LBTN_DBLCLK = 0,
	OMT_LBTN_DOWN   = 1,
	OMT_LBTN_UP	    = 2,
	OMT_MBTN_DBLCLK = 3,
	OMT_MBTN_DOWN   = 4,
	OMT_MBTN_UP	    = 5,
	OMT_RBTN_DBLCLK = 6,
	OMT_RBTN_DOWN   = 7,
	OMT_RBTN_UP	    = 8,
	OMT_MOVE	    = 9,
	OMT_WHEEL	    = 10,
};

class OptMouse : public IOptEvent
{
public:
	OptMouse();
	OptMouse(OptMouseType omt, int ptx, int pty, unsigned int nFlags);
	OptMouse(int ptx, int pty, int zd, unsigned int nFlags);
	virtual APhysXU32 GetVersion() const { return 0x1F000001; }
	virtual OptType	GetOptType() const { return OT_MOUSE; }

	virtual IOptEvent* Clone() const;
	virtual bool LoadData(NxStream& stream, bool& outIsLowVersion);
	virtual void SaveData(NxStream& stream);
	virtual void ExecuteOptEvent(IUserInputEvent& uie, CRecording& rec) const;
	virtual void SaveDebugString(ADiskFile& adfDebug) const;

	void GetMousePoint(int& outX, int& outY) const { outX = m_ptX; outY = m_ptY; }

private:
	OptMouseType m_bMType;
	int m_ptX;
	int m_ptY;
	int m_zDelta;
	unsigned int m_nFlags;
};

class OptSize : public IOptEvent
{
public:
	OptSize();
	OptSize(unsigned int nType, int cx, int cy);
	virtual APhysXU32 GetVersion() const { return 0x2F000001; }
	virtual OptType	GetOptType() const { return OT_SIZE; }

	virtual IOptEvent* Clone() const;
	virtual bool LoadData(NxStream& stream, bool& outIsLowVersion);
	virtual void SaveData(NxStream& stream);
	virtual void ExecuteOptEvent(IUserInputEvent& uie, CRecording& rec) const;
	virtual void SaveDebugString(ADiskFile& adfDebug) const;

private:
	unsigned int m_nType;
	int m_nCX;
	int m_nCY;
};

struct CameraExtCtrl
{
public:
	CameraExtCtrl();

public:
	bool		bHasBackUpPose;
	int			nMode;

	float		fRadius; // radius from focus position to camera
	A3DVECTOR3  vFocusPos;

	A3DVECTOR3  camPosBack;
	A3DVECTOR3  camDirBack;
	A3DVECTOR3  camUpBack;
};

class CameraViewportCtrl
{
public:
	CameraViewportCtrl();
	CameraViewportCtrl(const A3DCamera& cam, const A3DViewport& vp);

	void InitCVC(const A3DCamera& cam, const A3DViewport& vp);
	virtual APhysXU32 GetVersion() const { return 0x2F000001; }

	virtual bool LoadData(NxStream& stream, bool& outIsLowVersion);
	virtual void SaveData(NxStream& stream);

	bool HasValidateData() const { return m_bHasValidateData; }
	bool UpdateCamAndVP(A3DCamera* pCam, A3DViewport* pVp, A3DDevice* pDevice = 0) const;

public:
	bool m_bHasValidateData;

	float m_camFov;
	float m_camFront;
	float m_camBack;
	float m_camRatio;

	int m_vpX;
	int m_vpY;
	int m_vpWidth;
	int m_vpHeight;
	float m_vpMinZ;
	float m_vpMaxZ;
	int m_vpColorClear;
};

struct RecSizeInt
{
	int x;
	int y;
};

struct RecRangeFloat
{
	float a;
	float b;
};

struct RecVector3
{
	RecVector3& operator = (const A3DVECTOR3& v)
	{ 
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
	RecVector3& operator = (const NxVec3& v)
	{ 
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	void ToA3DVector3(A3DVECTOR3& out) const
	{
		out.Set(x, y, z);
	}

	A3DVECTOR3 GetA3DVector3() const
	{
		return A3DVECTOR3(x, y, z);
	}
	NxVec3 GetNxVec3() const
	{
		return NxVec3(x, y, z);
	}

	float x;
	float y;
	float z;
};

union AutoData
{
	AutoData() { nVal = 0; }
	AutoData(bool v) { bVal = v; }
	AutoData(int v) { nVal = v; }
	AutoData(float v) { fVal = v; }
	AutoData(const NxVec3& v) { vecVal = v; }
	AutoData(const A3DVECTOR3& v) { vecVal = v; }
	AutoData(int v1, int v2) { szeVal.x = v1; szeVal.y = v2; }

	bool bVal;
	int nVal;
	float fVal;
	RecSizeInt szeVal;
	RecRangeFloat rgnVal;
	RecVector3 vecVal;
};

enum RecItemType
{
	// Note: Do not change the value!!! It's saved in rec file.
	// Must update the "sizeTable" when you add new item.
	RIT_SIGN_UNKNOWN	= 0,
	RIT_CEC_MODE		= 1,
	RIT_CEC_RADIUS		= 2,
	RIT_CEC_FOCUSPOS	= 3,
	RIT_CEC_BACKPOS		= 4,
	RIT_CEC_BACKDIR		= 5,
	RIT_CEC_BACKUP		= 6,
	RIT_CEC_BACK_FLAG	= 7,

	RIT_CVC_FOV			= 8,
	RIT_CVC_FRONT		= 9,
	RIT_CVC_BACK		= 10,

	RIT_CVC_VPPT		= 11,
	RIT_CVC_VPSIZE		= 12,
	RIT_CVC_VPMINMAX	= 13,
	RIT_CVC_VPCOLOR		= 14,

	RIT_POS				= 15,
	RIT_DIR				= 16,
	RIT_UP				= 17,

	RIT_CAM_MOVE_CTRL	= 18,
	RIT_MA_MOVE_CTRL	= 19,
	RIT_CRNT_MOUSE_PT	= 20,
	RIT_COUNT			= 21,
	RIT_SIGN_END,
};

struct RecItemData
{
	static APhysXU32 GetVersion() { return 0xDD000001; }
	static int GetDataSize(RecItemType dType, const APhysXU32 nVersion);

	void ReadData(NxStream& stream, const APhysXU32 nVersion);
	void SaveData(NxStream& stream);

	RecItemType dType;
	AutoData value;
};

enum RecFlagsInfo
{
	FI_HAS_ITEM_DATA	= (1<<0),
	FI_HAS_ASYNC_KEYBRD	= (1<<1),
};

class RecFrameInfo
{
public:
	RecFrameInfo();
	~RecFrameInfo();
	APhysXU32 GetVersion() const { return 0x1D000001; }

	bool LoadData(NxStream& stream, bool& outIsLowVersion);
	void SaveData(NxStream& stream);
	void SaveDebugData(ADiskFile& adfDebug, bool bIsReplay);
	void ClearAllLog();

	void AddEvent(IOptEvent& optEvent);
	bool AddUserDbgInfo(const AString& str);

	RecItemData* ReproItemData();
	void ReproduceAllEvents(IUserInputEvent& uie, CRecording& rec);

	size_t GetEventCount() const { return arrEvents.size(); }
	IOptEvent* GetActiveEvent() const { return m_pActiveEvent; }
	IOptEvent* GetRecentEvent() const;

private:
	IOptEvent* CreateEvent(NxStream& stream, bool& outIsLowVersion);

public:
	typedef std::vector<IOptEvent*>			arrayEvent;
	typedef std::vector<RecItemData>		arrayItem;

	bool bIsInEPIS;
	float dtTickSec;
	IOptEvent*	m_pActiveEvent;
	IOptEvent*	m_pLastReproEvent;
	arrayEvent  arrEvents;

	CFlagMgr<RecFlagsInfo>	 m_Flags;
	SimpleAsyncRec	m_asyncKeyBoard;

	size_t m_idxReproItem;
	arrayItem	arrItems;
};

int GetCountOfBitValueIsOne(int n);

class CCameraController;

class CRecording
{
public:
	CRecording();
	~CRecording();
	APhysXU32 GetVersion() const { return 0x1C000001; }
	void InitScene(Scene& scene);
	void RegisterUIE(IUserInputEvent* pUIE) { m_pReplayUIE = pUIE; }

	void EnableDebugFile(bool bEnable);
	void AddUserDebugString(const AString& str);
	bool IsExistDebugFile() const { return (0 == m_pDebugInfo)? false : true; }

	bool RecordEnable(bool bEnable);
	bool GetRecordEnableState() const;

	// for terrain multi-thread load LOD.  default 2 second
	void  SetReplayDelayTime(float dtSec) { m_maxReplayDelayTime = dtSec; }
	float GetReplayDelayTime() const { return m_maxReplayDelayTime; }

	bool Start(const char* szFileName);
	void End();
	bool EpisodeBegin(const char* szFileName); // actually effect in next frame (current frame is excluded)
	void EpisodeEnd();						   // immediately stop (current frame is excluded)
	bool Replay(const char* szFileName);
	void UpdateTickForRec(float dtSec);

	int GetRandSeed() const;
	void SetRandSeed(int rs);

	bool IsRecording() const;
	bool IsReplaying() const;
	bool IsRecordingEpisode() const;
	bool IsReplayLoadingEpisode(float* pLoadingPercent = 0) const;
	bool IsReplayEnd() const;
	bool IsEventReproducing() const;
	int   GetNextRecordingEpisodeIndex() const;
	float GetReproTickSec() const;
	bool  GetInstantReproEventScreenRay(const int x, const int y, PhysRay& outRay) const;
	const A3DCamera* GetOriginalCamera() const;

	void LogEvent(const IOptEvent& optE);
	void ReproAllEvents();

	void LogItemData(const RecItemType& rit, const AutoData& value);
	bool ReproItemData(const RecItemType& rit, AutoData* poutValue = 0);
	bool ReproItemData(RecItemType& outRit, AutoData& outValue);

	bool QueryAsyncKeyDown(int vKey, bool bQueryOnly = false);
	bool OnReproCamAndVP(const RecItemType& rit, const AutoData& value);

private:
	void ReplayExecute();
	bool CreateRecEpiFile();
	bool LoadRecEpiFile();
	bool ModifyRecFile();
	bool InitDebugFile(const char* szRecFile, bool bIsEpiType);
	void SaveDebugData();
	void SaveObjectPose(IObjBase& obj, const AString& strPrefix);
	void SyncCameraViewToOriginal(A3DDevice* pDevice);
	bool ReadNextFrameInfo();
	bool IsEqualCameraPose() const;
	bool IsEqualCameraAllData() const;

private:
	enum StatusFlags
	{
		SF_ENABLE_RECORD	= (1<<0),
		SF_ENABLE_DEBUG		= (1<<1),
		SF_REPLAY_LOADING	= (1<<2),
		SF_REPLAY_DELAY		= (1<<3),
		SF_REPLAY			= (1<<4),
		SF_REPLAY_END		= (1<<5),
		SF_EPISODE_BEGIN	= (1<<6),
		SF_EPISODE			= (1<<7),
		SF_EPISODE_END		= (1<<8), // recording only
		SF_CREATE_EPIDBFILE	= (1<<9),
	};

	enum RecType
	{
		RT_REC_WHOLE	= 1,
		RT_REC_EPISODE	= 2,
	};

private:
	static A3DVECTOR3 g_vUp;

	CFlagMgr<StatusFlags>	 m_Status;
	Scene*				m_pScene;
	IUserInputEvent*	m_pReplayUIE;
	int					m_randSeed;
	RecFrameInfo		m_frmCurrent;

	typedef std::vector<AString>	arrayString;
	arrayString m_strDebugBuf;
	size_t		m_nCountBeforFirstEvent;

	A3DCamera*		m_pActiveCam;
	A3DViewport*	m_pActiveVP;

	A3DCamera	m_origCam;
	A3DViewport m_origVP;
	CameraViewportCtrl m_origCVC;

	RecType m_RecFileType;
	int m_nNextEpiRecIndex;
	int m_nEpiStartFrame;
	int m_nEpiEndFrame;     // for replay episode only
	int m_nCurrentFrameNum;
	int m_nTotalFrameCount;

	float m_maxReplayDelayTime;
	float m_curReplayDelayTime;

	AString m_RecFileName;
	AString m_EpiFileName;
	APhysXUserStream*	m_pRecFile;
	ADiskFile*	m_pDebugInfo;
	ADiskFile*	m_pEpiDebugInfo;
	ADiskFile	m_adfDebug;
	ADiskFile	m_adfEpiDebug;
};

#endif
