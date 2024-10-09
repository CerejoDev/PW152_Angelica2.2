/*
 * FILE: IPropPhysXObjBase.h
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
#ifndef _APHYSXLAB_IPROPPHYSXOBJBASE_H_
#define _APHYSXLAB_IPROPPHYSXOBJBASE_H_

#include "IPropObjBase.h"

class IPropPhysXObjBase : public IPropObjBase
{
public:
	static APhysXU32 GetVersion() { return 0xAA000001; }

	enum DrivenMode
	{
		DRIVEN_BY_PURE_PHYSX = 0,
		DRIVEN_BY_ANIMATION  = 1,
		DRIVEN_BY_PART_PHYSX = 2,
	};
	static const TCHAR* GetDrivenModeTextChinese(const DrivenMode dm);
	static const TCHAR* GetDrivenModeText(const DrivenMode dm);
	static bool GetDrivenMode(const TCHAR* pStr, DrivenMode& outDM);

public:
	IPropPhysXObjBase();
	IPropPhysXObjBase(const IPropPhysXObjBase& rhs);
	IPropPhysXObjBase& operator= (const IPropPhysXObjBase& rhs);

	bool		   IsEmptyFilePathName()const { return m_strFilePath.IsEmpty(); }
	AString		   GetFilePathName()	const { return m_strFilePath; }
	const char*	   GetFilePathName_cstr()const { return m_strFilePath.operator const char*(); }

	void	   SetDrivenMode(const DrivenMode& dm, bool bSetDefOnSim = false);
	DrivenMode GetDrivenMode(bool bGetDefOnSim = false) const;
	bool	   QueryDrivenMode(const DrivenMode& dm, bool bQDefOnSim = false) const;

	void SyncPoseDataFromPhysX(const A3DVECTOR3& lastPos);

	virtual bool SaveToFile(NxStream& nxStream) = 0;
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion) = 0;
	virtual bool OnSendToPropsGrid(CBCGPPropList& lstProp) = 0;
	virtual bool OnPropertyChanged(const CBCGPProp& prop) = 0;
	virtual void OnUpdatePropsGrid(CBCGPPropList& lstProp) = 0;

protected:
	friend class IPhysXObjBase;
	friend class CPhysXObjMgr;

	const A3DVECTOR3& GetOriginalAABBLC() const { return m_OriginalAABBLocalCenter; }
	const A3DVECTOR3& GetOriginalAABBExt() const { return m_OriginalAABBExtents; }
	float GetOriginalLoacalHeight() const { return m_OriginalLocalHeight; }

private:
	virtual void OnSetToEmpty();
	virtual bool EnableSetPose() const;
	virtual void SetScalePreNotify(const float& newScale);
	virtual void SetScalePostNotify(const float& oldScale);
	virtual void InstancePhysXPostNotify();

	void SetOriginalAABB(const A3DAABB& aabb);
	void SetOriginalLocalHeight(const A3DAABB& aabb);
	void SetFilePathName(const char* szFile);

private:
	DrivenMode m_dmCurrent;
	DrivenMode m_dmDefOnSim;
	A3DVECTOR3 m_OriginalAABBLocalCenter;   // APhysX AABB center base on current position
	A3DVECTOR3 m_OriginalAABBExtents;		// APhysX AABB extents
	float	   m_OriginalLocalHeight;

	// Must be a relative path.
	AString m_strFilePath;
};

#endif