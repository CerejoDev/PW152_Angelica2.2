/*
 * FILE: APhysXCCMgr.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/12/02
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_APHYSXCCMGR_H_
#define _APHYSXLAB_APHYSXCCMGR_H_

#include <APhysX.h>

class IAPWalk;
class BrushcharCtrler;
class CPhysXObjDynamic;

class CAPhysXCCMgr
{
public:
	enum CCType
	{
		// For compatible old archive...
		// Do NOT change the order, just add new items following the sequence 
		CC_UNKNOWN_TYPE	= 0,
		CC_TOTAL_EMPTY	= 1,
		CC_APHYSX_LWCC	= 2,
		CC_APHYSX_NXCC	= 3,
		CC_APHYSX_EXTCC	= 4,
		CC_APHYSX_DYNCC	= 5,
		CC_BRUSH_CDRCC	= 6,
	};
	static const TCHAR* GetCCTypeText(const CCType ccType);
	static bool GetCCType(const TCHAR* pStr, CCType& outRT);

public:
	bool newStyleTest;
	CAPhysXCCMgr();
	~CAPhysXCCMgr();

	void InitHostObject(CPhysXObjDynamic& hostDynObj) { m_pHostDynObj = &hostDynObj; }
	const CPhysXObjDynamic* GetHostObject() const { return m_pHostDynObj; }

	APhysXScene* GetRuntimeScene() const { return m_pAPhysXScene; }
	void OnEnterRuntime(APhysXScene& aPhysXScene, IAPWalk* pWalk);
	void OnLeaveRuntime();

	void SetDefaultVolume(const NxVec3& vol) { m_defVolume = vol; }
	NxVec3 GetDefaultVolume() const { return m_defVolume; }

	bool	 SwtichCC(const CCType& toType, bool bEnsureSuccess = false);
	bool	 QueryCCType(const CCType& cc) const;
	CCType	 GetCCType() const;
	const TCHAR* GetCCTypeText() const { return GetCCTypeText(GetCCType()); }

	APhysXCCBase*		 GetAPhysXCCBase() const { return m_pAPhysXCCBase; }
	APhysXHostCCBase*	 GetAPhysXHostCCBase() const { return m_pAPhysXHostCC; }
	BrushcharCtrler*	 GetBrushHostCC() const { return m_pBrushCDRCC; }
	APhysXDynCharCtrler* GetAPhysXDynCC() const { return m_pAPhysXDynCC; }
	APhysXCharacterController* GetAPhysXExtCC() const { return m_pAPhysXExtCC; }
	NxActor* GetHiddenActor() const;
	bool IsHangMode() const;
	bool IsInAir() const;

	int  GetCurrentFlags() const;
	void SetCurrentFlags(int flags);

	bool ReadFlag(const APhysXLWCharCtrler::APhysXLWCCFlag& flag) const;
	bool ReadFlag(const APhysXDynCharCtrler::APhysXCCFlag& flag)  const;
	bool ReadFlag(const APhysXCharacterController::APhysXCCFlag& flag) const;
	bool ReadFlagSlide() const;
	bool ReadFlagAttachEnforceOpen() const;
	bool ReadFlagAttachEnforceClose() const;
	void SetFlag(const APhysXLWCharCtrler::APhysXLWCCFlag& flag, bool enable);
	void SetFlag(const APhysXDynCharCtrler::APhysXCCFlag& flag, bool enable);
	void SetFlag(const APhysXCharacterController::APhysXCCFlag& flag, bool enable);
	void SetFlagSlide(bool enable);
	void SetFlagAttachEnforceOpen(bool enable);
	void SetFlagAttachEnforceClose(bool enable);

	void RegisterHostModels(IObjBase* pObj);
	void UnRegisterHostModels(IObjBase* pObj);

	void SyncNonDynCCPoseToModel() const; // deprecated
	void SyncDynCCPoseToModel() const;  // deprecated
	bool DoMoveCC(float deltaTimeSec); // deprecated

	void PreMoveCC(float deltaTimeSec, const NxVec3* pFootPos = 0);
	bool MoveCC(float deltaTimeSec, const NxVec3& moveDir);
	bool SyncNonDynCCPose(NxVec3& footPos, NxVec3& faceDir, NxVec3& up) const;
	bool SyncDynCCPose(NxVec3& footPos, NxVec3& faceDir, NxVec3& up) const;
	bool CheckVolumeValid(const NxVec3& posFoot, const NxVec3& volume, const APhysXCCBase::APhysXVolumeType type);

private:
	typedef APhysXCCBase::APhysXVolumeType   VolumeType;

	void GetRelevantFlagsFromCCToDynCC(int& inoutFlags);
	void GetRelevantFlagsFromDynCCToCC(int& inoutFlags);

	void GetCreateInfo(NxVec3& outFootPos, NxVec3& outVolume, VolumeType& outVT);
	bool CheckCCCreatePosition(const CCType& toCreateType, const NxVec3& posFoot, const NxVec3& volume, const VolumeType type, NxVec3& outGoodPos);

	bool PrepareHostModelCNL(APhysXCollisionChannel& cnlBK);
	void RecoverHostModelCNL(APhysXCollisionChannel cnlBK);

	bool ApplyCCType(CCType ccType, int ccFlags, bool bEnsureSuccess = false);
	bool CreateAPhysXLWCC(const NxVec3& posFoot, const NxVec3& volume, const VolumeType& vt, int ccFlags);
	bool CreateAPhysXNxCC(const NxVec3& posFoot, const NxVec3& volume, const VolumeType& vt, int ccFlags);
	bool CreateAPhysXExtCC(const NxVec3& posFoot, const NxVec3& volume, const VolumeType& vt, int ccFlags);
	bool CreateAPhysXDynCC(const NxVec3& posFoot, const NxVec3& volume, const VolumeType& vt, int ccFlags);
	bool CreateBrushCDRCC(const NxVec3& posFoot, const NxVec3& volume, const VolumeType& vt, int ccFlags);

	void ReleaseAPhysXLWCC();
	void ReleaseAPhysXNxCC();
	void ReleaseAPhysXExtCC();
	void ReleaseAPhysXDynCC();
	void ReleaseBrushCDRCC();

public:
	void* m_UserData;

private:
	APhysXCCBase*			   m_pAPhysXCCBase;
	APhysXHostCCBase*		   m_pAPhysXHostCC;

	APhysXLWCharCtrler*		   m_pAPhysXLWCC;
	APhysXNxCharCtrler*		   m_pAPhysXNxCC;
	APhysXCharacterController* m_pAPhysXExtCC;
	APhysXDynCharCtrler*	   m_pAPhysXDynCC;
	BrushcharCtrler*		   m_pBrushCDRCC;

	CPhysXObjDynamic*	m_pHostDynObj;
	APhysXScene*		m_pAPhysXScene;
	IAPWalk*			m_pWalk;
	NxVec3	m_defVolume;

	bool	m_bIsRumtime;
	CCType	m_ccEditType;
	int		m_ccEditFlags;
	CCType	m_ccRumtimeType;
	int		m_ccRumtimeFlags;
};

#endif
