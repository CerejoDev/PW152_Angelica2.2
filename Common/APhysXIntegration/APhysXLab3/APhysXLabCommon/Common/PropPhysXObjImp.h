/*
 * FILE: PropPhysXObjImp.h
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
#ifndef _APHYSXLAB_PROPPHYSXOBJIMP_H_
#define _APHYSXLAB_PROPPHYSXOBJIMP_H_

#include "APhysXCCMgr.h"
#include "ActionPlayerBase.h"

class CObjProperty : public IPropPhysXObjBase
{
public:
	CObjProperty& operator= (const CObjProperty& rhs) { IPropPhysXObjBase::operator=(rhs); return *this; }
	virtual bool SaveToFile(NxStream& nxStream)
		{ return IPropPhysXObjBase::SaveToFile(nxStream); }
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
		{ return IPropPhysXObjBase::LoadFromFile(nxStream, outIsLowVersion); }
	virtual bool OnSendToPropsGrid()
		{ return IPropPhysXObjBase::OnSendToPropsGrid(); }
	virtual bool OnPropertyChanged(PropItem& prop)
		{ return IPropPhysXObjBase::OnPropertyChanged(prop); }
	virtual void OnUpdatePropsGrid()
		{ IPropPhysXObjBase::OnUpdatePropsGrid(); }

protected:
	friend class CPhysXObjStatic;
	friend class CPhysXObjParticle;
	virtual CObjProperty* CreateMyselfInstance(bool snapValues) const;
	virtual void EnterRuntimePreNotify(APhysXScene& aPhysXScene)
		{
			IPropPhysXObjBase::EnterRuntimePreNotify(aPhysXScene);
			IPropPhysXObjBase* pBK = dynamic_cast<IPropPhysXObjBase*>(m_pBackupProps);
			if (0 != pBK)
				*pBK= *this;
		}
	virtual void LeaveRuntimePostNotify()
	{
		IPropPhysXObjBase::LeaveRuntimePostNotify();
		CObjProperty* pBK = dynamic_cast<CObjProperty*>(m_pBackupProps);
		if (0 != pBK)
			*this = *pBK;
	}

private:
	CObjProperty();
	CObjProperty(const CObjProperty& rhs);
};

class CDynProperty : public IPropPhysXObjBase
{
public:
	typedef		CAPhysXCCMgr::CCType		CCType;
	typedef		IActionPlayerBase::APType	APType;

	enum RoleType
	{
		ROLE_ACTOR_NONE		= 0,
		ROLE_ACTOR_NPC		= 1,		
		ROLE_ACTOR_AVATAR	= 2,		
	};
	static const TCHAR* GetRoleTypeText(const RoleType rt);
	static bool GetRoleType(const TCHAR* pStr, RoleType& outRT);

	static IRoleCallback* gRoleCallback;

public:
	~CDynProperty();
	CDynProperty& operator= (const CDynProperty& rhs);

	int	GetApxRBFlags() const { return m_rbFlags; }

	bool	 SetRoleType(const RoleType& rt);
	RoleType GetRoleType() const { return m_roleType; }
	bool	 QueryRoleType(const RoleType& rt) const;

	bool	 SetAPType(const APType& ap);
	APType	 GetAPType() const { return m_apType; }
	bool	 QueryAPType(const APType& ap) const;
	IActionPlayerBase* GetActionPlayer() { return m_pAC; }

	bool SetCCType(const CCType& cc);
	CAPhysXCCMgr& GetCCMgr() { return m_CCMgr; }
	const CAPhysXCCMgr& GetCCMgr() const { return m_CCMgr; }

	virtual bool SaveToFile(NxStream& nxStream);
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion);
	virtual bool OnSendToPropsGrid();
	virtual bool OnPropertyChanged(PropItem& prop);
	virtual void OnUpdatePropsGrid();

protected:
	friend class CPhysXObjDynamic;
	friend class CPhysXObjSkinModel;
	friend class CPhysXObjECModel;
	virtual CDynProperty* CreateMyselfInstance(bool snapValues) const;
	virtual void EnterRuntimePreNotify(APhysXScene& aPhysXScene);
	virtual void LeaveRuntimePostNotify();

	void UpdateMove(float deltaTimeSec);
	void DoMoveCC(float deltaTimeSec);

	bool PlayAction(const unsigned long deltaTime);
	void FinishCurrentAction();

private:
	CDynProperty();
	CDynProperty(const CDynProperty& rhs);

	virtual void OnSetToEmpty();
	virtual void InitHostObjectPost();
	virtual void InstancePhysXPostNotify();
	virtual void EnterRuntimePostNotify(APhysXScene& aPhysXScene);
	virtual void LeaveRuntimePreNotify();

	void SetActionPlayer(const IActionPlayerBase* pAC);
	bool ApplyRBFlags(const PropID& pid);

private:
	RoleType	m_roleType;
	APType		m_apType;
	int			m_rbFlags;

	IActionPlayerBase* m_pAC;
	IRoleCallback*	m_RoleCallback;
	CAPhysXCCMgr	m_CCMgr;
};

#endif