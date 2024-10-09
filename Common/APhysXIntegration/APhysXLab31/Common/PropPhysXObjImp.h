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
	CObjProperty& operator= (const CObjProperty& rhs) { IPropPhysXObjBase::operator=(rhs); }
	virtual bool SaveToFile(NxStream& nxStream)
		{ return IPropPhysXObjBase::SaveToFile(nxStream); }
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
		{ return IPropPhysXObjBase::LoadFromFile(nxStream, outIsLowVersion); }
	virtual bool OnSendToPropsGrid(CBCGPPropList& lstProp)
		{ return IPropPhysXObjBase::OnSendToPropsGrid(lstProp); }
	virtual bool OnPropertyChanged(const CBCGPProp& prop)
		{ return IPropPhysXObjBase::OnPropertyChanged(prop); }
	virtual void OnUpdatePropsGrid(CBCGPPropList& lstProp)
		{ IPropPhysXObjBase::OnUpdatePropsGrid(lstProp); }

protected:
	friend class CPhysXObjStatic;
	friend class CPhysXObjParticle;
	virtual CObjProperty* CreateMyselfInstance(bool snapValues) const;

private:
	CObjProperty();
	CObjProperty(const CObjProperty& rhs);
};

class CMgrProperty : public IPropPhysXObjBase
{
public:
	CMgrProperty& operator= (const CMgrProperty& rhs) { IPropPhysXObjBase::operator=(rhs); }
	virtual bool SaveToFile(NxStream& nxStream)
		{ return false; }
	virtual bool LoadFromFile(NxStream& nxStream, bool& outIsLowVersion)
		{ outIsLowVersion = false; return false; }
	virtual bool OnSendToPropsGrid(CBCGPPropList& lstProp)
		{ return IPropPhysXObjBase::OnSendToPropsGrid(lstProp); }
	virtual bool OnPropertyChanged(const CBCGPProp& prop)
		{ return IPropPhysXObjBase::OnPropertyChanged(prop); }
	virtual void OnUpdatePropsGrid(CBCGPPropList& lstProp)
		{ IPropPhysXObjBase::OnUpdatePropsGrid(lstProp); }

protected:
	virtual CMgrProperty* CreateMyselfInstance(bool snapValues) const { return 0; }
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
	virtual bool OnSendToPropsGrid(CBCGPPropList& lstProp);
	virtual bool OnPropertyChanged(const CBCGPProp& prop);
	virtual void OnUpdatePropsGrid(CBCGPPropList& lstProp);

protected:
	friend class CPhysXObjDynamic;
	friend class CPhysXObjSkinModel;
	friend class CPhysXObjECModel;
	virtual CDynProperty* CreateMyselfInstance(bool snapValues) const;

	void EnterRuntimeNotify(APhysXScene& aPhysXScene);
	void LeaveRuntimeNotify();

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
	void SetActionPlayer(const IActionPlayerBase* pAC);
	bool SetHostModelChannel(const APhysXCollisionChannel channel) const { return m_CCMgr.SetHostModelChannel(channel); }

	bool ApplyCCType();
	bool ApplyCCFlags(const PropID& pid);
	bool ApplyRBFlags(const PropID& pid);

private:
	RoleType	m_roleType;
	APType		m_apType;
	int			m_rbFlags;
	int			m_ccFlags;

	IActionPlayerBase* m_pAC;
	CAPhysXCCMgr	m_CCMgr;
};

#endif