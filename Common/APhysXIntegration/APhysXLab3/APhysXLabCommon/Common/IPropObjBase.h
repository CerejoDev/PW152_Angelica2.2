/*
 * FILE: IPropObjBase.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_IOBJPROPBASE_H_
#define _APHYSXLAB_IOBJPROPBASE_H_

#include "ReportInterface.h"

class IObjBase;
class CPhysXObjSelGroup;
class PropItem;

enum FlagsInfo
{
	// PhysX objs
	OBF_ENABLE_PHYSX_BODY   = (1<<0),   // to update: hidden InstancePhysXObj() func, anychange only by this flag
	OBF_DRIVENMODE_ISLOCKED = (1<<1),

	// only region
	OBF_ENABLE_EVENT_REGION_RTBEGIN  = (1<<2),
	OBF_ENABLE_EVENT_REGION_RTEND    = (1<<3),
	OBF_ENABLE_EVENT_REGION_ENTER    = (1<<4),
	OBF_ENABLE_EVENT_REGION_LEAVE    = (1<<5),
	
	// above flags support serialization and don't change the value
	SAVE_SEPARATOR_OBF_ALIAS= (1<<6),  // SAVE_SEPARATOR_OBF_ALIAS don't occupy a bit

	// all
	OBF_RUNTIME_ONLY		= SAVE_SEPARATOR_OBF_ALIAS,
	OBF_DRAW_BINDING_BOX	= (1<<7),
	OBF_IS_SHOT_STUFF		= (1<<8),
	OBF_DRIVENMODE_ISLOCKED_ONSIM = (1<<9),
};

enum FlagsInternal
{
	OBFI_INITED_HOST_OBJECT		= (1<<0),
	OBFI_INITED_PHYSX_AABB		= (1<<1),
	OBFI_INITED_APHYSX_FLAGS	= (1<<2),
	OBFI_DISABLE_DEF_DMONSIM	= (1<<3),
	// above flags support serialization and don't change the value
	SAVE_SEPARATOR_OBFI_ALIAS	= (1<<4),  // SAVE_SEPARATOR_OBFI_ALIAS don't occupy a bit
	OBFI_IS_IN_RUNTIME			= SAVE_SEPARATOR_OBFI_ALIAS,
	OBFI_HAS_ASYN_WORK			= (1<<5),  // flag of asynchronous operation unfinished
	OBFI_GOTO_SLEEP				= (1<<6),
	OBFI_GOTO_DESTROY			= (1<<7),
	OBFI_ENABLE_ACTION_PLAY		= (1<<8),
	OBFI_RESTART_ACTION_PLAY	= (1<<9),
	OBFI_ENFORCE_SETDRIVENMODE  = (1<<10),
	OBFI_KEEP_OBJECT_INDEX		= (1<<11),
};

class RayHitOwner : public IObjStateReport
{
public:
	RayHitOwner();

	void OnDecorateObj(const IObjBase& obj);
	IObjBase* GetRayHit(POINT* poutPT = 0) const;
	void SetRayHit(IObjBase* pNewHit, const POINT* pPT = 0);

	virtual void OnNotifySleep(const IObjBase& obj);

private:
	IObjBase* m_pRH;
	POINT m_pt;
};

class IPropObjBase : public APhysXSerializerBase
{
public:
	typedef ObjManager::ObjTypeID		ObjTypeID;

	static const TCHAR*		szEmpty;
	static APhysXU32 GetVersion() { return 0xAA000004; }

	enum ObjState
	{
		OBJECT_STATE_NULL   = 0,
		OBJECT_STATE_ACTIVE = 1,
		OBJECT_STATE_SLEEP  = 2,
		OBJECT_STATE_CORPSE = 3,
	};
	static const TCHAR* GetObjStateText(const ObjState objState);

	static void SetRenderManager(IRenderManager* pRM);
	static void SetStateReport(IObjStateReport* pSR);

private:
	static IRenderManager* pRenderManager;
	static IObjStateReport* pStateReport;

public:
	IPropObjBase();
	IPropObjBase(const IPropObjBase& rhs);
	virtual ~IPropObjBase();
	IPropObjBase& operator= (const IPropObjBase& rhs);

	IObjBase*	GetHostObject()	const { return m_pHostObject; }
	ObjTypeID	GetObjType()	const { return m_nTypeID; }
	ObjState	GetObjState()	const { return m_ObjState; }
	TString		GetObjName()	const { return m_strObjName; }
	TString		GetObjLocalName() const { return m_strLocalName; }
	const char*	GetObjName_cstr() const
	{
		m_strTempName = _TWC2AS(m_strObjName);
		return  m_strTempName;
	}
	const char*	GetObjLocalName_cstr() const
	{
		m_strTempName = _TWC2AS(m_strLocalName);
		return  m_strTempName;
	}
	bool SetObjLocalName(const TCHAR* szName);

	bool IsActive() const;
	bool IsRuntime() const;
	CFlagMgr<FlagsInfo>& GetFlags() { return m_Flags; }
	const CFlagMgr<FlagsInfo>& GetFlags() const { return m_Flags; }

	void  SetUserData(void* pUD) { m_pUserData = pUD; }
	void* GetUserData() const { return m_pUserData; } 

	void			   SetSelGroup(CPhysXObjSelGroup* pSelG) { m_pSelGroup = pSelG; }
	CPhysXObjSelGroup* GetSelGroup() const { return m_pSelGroup; } 

	A3DVECTOR3 GetPos(bool bRuntimePos = true) const;
	void	   SetPos(const A3DVECTOR3& vPos);
	A3DVECTOR3 GetDir(bool bRuntimeDir = true) const;
	A3DVECTOR3 GetUp(bool bRuntimeUp = true) const;
	void	   SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	A3DMATRIX4 GetPose(bool bRuntimePose = true) const;
	void	   SetPose(const A3DMATRIX4& mat44);
	float	   GetScale(bool bRuntimeScale = true) const;
	void	   SetScale(const float& fScale);

	//obj script operation
	int GetScriptCount() const;
	const AString& GetScriptName(int index) const;
	void SetScriptName(int index, const AString& scriptName);
	//get & set all script names, separated with ;
	AString GetScriptNames() const;
	void SetScriptNames(const char* scriptNames);

	virtual bool SaveToFile(NxStream& nxStream) = 0;
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion) = 0;
	virtual bool OnSendToPropsGrid() = 0;
	virtual bool OnPropertyChanged(PropItem& prop) = 0;
	virtual void OnUpdatePropsGrid() = 0;

protected:
	friend class IObjBase;
	friend class IPhysXObjBase;
	friend class ObjManager;

protected:
	virtual IPropObjBase* CreateMyselfInstance(bool snapValues) const = 0;
	virtual bool EnableSetPoseAndScale() const;
	virtual void EnterRuntimePreNotify(APhysXScene& aPhysXScene) = 0;
	virtual void LeaveRuntimePostNotify() = 0;

	bool InitTypeIDAndHostObject(const ObjTypeID& type, IObjBase& hostObject, int UID);
	bool GetDrawBindingBoxInfo(int& outColor) const;
	void SetObjName(const TCHAR* szFile);
	int GetUID() const { return m_ObjIndex; }

	void SetObjState(const ObjState& os);
	bool IsObjAliveState() const;
	bool QueryObjState(const ObjState& os) const { return os == m_ObjState; }

private:
	virtual void OnSetToEmpty() {}
	virtual void InitHostObjectPost() {}
	virtual void SetScalePostNotify(const float& oldScale) {}

	virtual void EnterRuntimePostNotify(APhysXScene& aPhysXScene) {}
	virtual void LeaveRuntimePreNotify() {}

	int	 GetSaveMask(int EndBitEnum) const;
	void SaveFlags(NxStream& nxStream);
	void LoadFlags(NxStream& nxStream);
	bool LoadOldArchive(NxStream& nxStream);
	bool CheckPoseAxis();

	void OnObjStateChange(const ObjState& oldState) const;
	void SetToEmpty();
	void SetObjIndex(int idx);

	void CopyDataForSnap(const IPropObjBase& rhs);

protected:
	struct OldArchiveData
	{
		bool oadEnable;
		int oadCurrentDM; 
		int oadDefOnSimDM;
		float oadLH;
		AString oadFilePath;
	};

	OldArchiveData m_oadCompatible;
	CFlagMgr<FlagsInternal> m_Status;
	IObjBase*		m_pHostObject;
	IPropObjBase*		m_pBackupProps;

private:
	void*				m_pUserData;
	CPhysXObjSelGroup*  m_pSelGroup;

	bool		m_bIsUnrealShadow;
	ObjTypeID	m_nTypeID;
	ObjState	m_ObjState;
	int			m_ObjIndex;

	TString m_strObjName;
	TString m_strLocalName;
	mutable AString m_strTempName;

	float m_fScale;
	A3DMATRIX4 m_matPose;

	CFlagMgr<FlagsInfo>	  m_Flags;
	std::vector<AString>  m_strScripts;
};

#endif