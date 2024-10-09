/*
* FILE: PhysXObjSelGroup.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2010/12/21
*
* HISTORY: 
*
* Copyright (c) 2010 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _APHYSXLAB_PHYSXOBJSELGROUP_H_
#define _APHYSXLAB_PHYSXOBJSELGROUP_H_

#include <vector>
#include "ReportInterface.h"

class IObjBase;
class IPropObjBase;
class IPropPhysXObjBase;
class CDynProperty;
class CAPhysXCCMgr;
class IObjStateReport;
class RayHitOwner;

enum PropID
{
	PID_START_BOUND = 0,
	PID_Unknown = PID_START_BOUND,
	PID_GROUP_Basic,
	PID_ITEM_Name,
	PID_ITEM_Type,
	PID_ITEM_State,
	PID_ITEM_File,
	PID_GROUP_DrivenBy,
	PID_ITEM_CurntDVN,
	PID_ITEM_OnSimDVN,
	PID_GROUP_PoseScale,
	PID_SUBGROUP_Pos_xyz,
	PID_SGITEM_Pos_x,
	PID_SGITEM_Pos_y,
	PID_SGITEM_Pos_z,
	PID_SUBGROUP_Dir_xyz,
	PID_SGITEM_Dir_x,
	PID_SGITEM_Dir_y,
	PID_SGITEM_Dir_z,
	PID_SUBGROUP_Up_xyz,
	PID_SGITEM_Up_x,
	PID_SGITEM_Up_y,
	PID_SGITEM_Up_z,
	PID_ITEM_Scale,
	//--------------------------
	//-----dynamic obj props----  
	PID_GROUP_RoleBehavior,
	PID_ITEM_Role,
	PID_ITEM_Behavior,
	PID_ITEM_PhysX_BodyEnable,
	PID_GROUP_Apx_RBFlags,
	PID_ITEM_Apx_PushForce,
	PID_ITEM_Apx_StandForce,
	PID_ITEM_Apx_AttachEnable,
	PID_GROUP_CC_Flags,
	PID_ITEM_CC_Type,
	PID_ITEM_LWCC_RevisePosEOpen,
	PID_ITEM_CCDCC_Slide,
	PID_ITEM_CC_PushForce,
	PID_ITEM_CC_StandForce,
	PID_ITEM_DynCC_AutoHangEnd,
	PID_ITEM_DynCC_PassiveEffect,
	PID_ITEM_CCDCC_AttachEOpen,
	PID_ITEM_CCDCC_AttachEClose,
	PID_END_BOUND
};

class CMultiValueProp : public CBCGPProp
{
public:
	static const _variant_t varMultiFlag;

	CMultiValueProp(const CString& strName, const _variant_t& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL);

	virtual void SetValue(const _variant_t& varValue);

private:
	void SetNewValue(const _variant_t& val);

private:
	bool m_bIsMultiValue;
	VARTYPE m_OriginType;
};

class CPhysXObjSelGroup : public IObjStateReport
{
public:
	typedef CCoordinateDirection::EditType EditType;

	static A3DWireCollector* pWCTopZBuf;
	static const TCHAR*		szEmpty;
	static const _variant_t varEmpty;
	static bool ExtractVector(const CBCGPProp& prop, A3DVECTOR3& outVec);

public:
	CPhysXObjSelGroup();
	void SetStateReport(IObjStateReport* pStateReport) { m_pStateReport = pStateReport; }
	void SetRayHitOwner(RayHitOwner& rho) { m_pRHO = &rho; }
	void SetPropList(CBCGPPropList& lstProp) { m_pPropList = &lstProp; }
	void SetGroupPos(const A3DVECTOR3& newPos_idx0);

	bool EnterRuntime(APhysXScene& aPhysXScene);
	void LeaveRuntime();
	bool IsRunTime() const { return m_Status.ReadFlag(SF_IS_GAME_RUNTIME); }
	bool IsDrawItemEnable() const;
	void DrawFrame(A3DWireCollector* pWC);

	IObjBase* GetFocusObject() const;
	IObjBase* GetObject(int index) const;
	void GetAllObjects(std::vector<IObjBase*>& objs) const;
	int Size() const { return int(m_aObjects.size()); }
	bool IsIncluding(const IObjBase& obj) const;

	void AppendObject(IObjBase& obj);
	void RemoveObject(const IObjBase& obj);
	void ReplaceWithObject(IObjBase* pObj);
	void ReplaceWithObjects(const std::vector<IObjBase*>& objs);

	void SetRefFrameEditMode(const EditType& et) { m_focusRefFrame.SetEditType(et); }
	EditType GetRefFrameEditMode() const { return m_focusRefFrame.GetEditType(); }
	bool UpdateRefFrameOnLButtonDown(const int x, const int y, const unsigned int nFlags);
	bool UpdateRefFrameOnMouseMove(const int x, const int y, const unsigned int nFlags, bool OnlyRefFrame);

	void UpdatePropItems();
	bool PropertyChanged(const CBCGPProp& prop);

	bool ReadUpdateMark(const PropID& propID);
	bool RaiseUpdateMark(const PropID& propID, bool bToDel = false);
	bool RaiseUpdateMarkToCCFlags();

	bool IsMerge(const IObjBase* pObj) const;
	void ExecuteUpdateItem(const PropID& nID, const _variant_t& objVal, bool bIsMerge);
	void FillGroupBasic(const IPropObjBase& objProp);
	void FillGroupPoseScale(const IPropObjBase& objProp);
	void FillGroupDrivenBy(const IPropPhysXObjBase& objProp);
	void FillGroupRoleBehavior(const CDynProperty& objProp);
	void FillGroupApxRBFlags(const CDynProperty& objProp);
	void FillGroupCCFlags(const CDynProperty& objProp);
	void OnUpdateBehaviorEnable(const CDynProperty& objProp);

	virtual void OnNotifySleep(const IObjBase& obj);

private:
	enum PropFlags
	{
		PF_IS_LOCKED		= (1<<0),
		PF_IS_DIRTY			= (1<<1),
		PF_TO_DELETE		= (1<<2),
		PF_IS_GROUP			= (1<<3),
		PF_IS_CHIEF			= (1<<4),
		PF_HAS_NEW_VALUE	= (1<<5),
	};

	struct ItemInfo : public CFlagMgr<PropFlags>
	{
		ItemInfo() { ClearAll(); pMultiValueMask = &varEmpty; }
		void ClearAll()
		{
			nCount = 0;
			pItem  = 0;
			pLeader = 0;
			newValue.Clear();
			ClearAllFlags();
		}

		void SetNewValue(const _variant_t& nv, bool bIsMerge);

		int nCount;
		CBCGPProp* pItem;
		CBCGPProp* pLeader;
		_variant_t newValue;
		const _variant_t* pMultiValueMask;
	};

	struct SelObj
	{
		IObjBase* operator->() { return pObj; }

		IObjBase* pObj;
		float	   originScale;
		A3DVECTOR3 distOffset;
	};

	enum StateFlags
	{
		SF_REFRESH_FRAME_DISABLE = (1<<0),
		SF_IS_GAME_RUNTIME		 = (1<<1),
		SF_HAS_SOMETHING_DIRTY	 = (1<<2),
		SF_PROPLIST_LAYOUT_DIRTY = (1<<3),
	};

private:
	// forbidden behavior
	CPhysXObjSelGroup(const CPhysXObjSelGroup& rhs);
	CPhysXObjSelGroup& operator= (const CPhysXObjSelGroup& rhs);

	void SetGroupRot(const float angleRadians, const A3DVECTOR3& axis);
	void SetGroupScale();
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	bool SetScale(const float& scale);

	void OnAddObject(IObjBase& obj);
	void RefreshFramePose();
	void RefreshFrameScale();
	void RefreshSelObjOffset();

	int GetIndex(const PropID& nID);
	void SendToPropsGrid();

	int  IncCheckinCount(const PropID& nID, bool bIncludeChiefItem);
	void SetChiefInfo(const PropID& nID, CBCGPProp* pI);
	void SetItemInfo(const PropID& nID, CBCGPProp& I, CBCGPProp* pL, bool isGroup, const _variant_t* pMultiValueMask = 0);

	unsigned int DeleteProps(unsigned int idx, bool SaveUpdateInfo);
	unsigned int ClearItemInfo(unsigned int idx, bool SaveUpdateInf);
	void CommitModified();
	void OnUpdateCCTypeContent(const CDynProperty& objProp);
	CBCGPProp* CreateCCFlagProp(const PropID& nID, const _variant_t& objVal);

private:
	typedef		std::vector<SelObj>		SelObjHolder;

	IObjStateReport* m_pStateReport;
	RayHitOwner* m_pRHO;
	CFlagMgr<StateFlags> m_Status;

	CBCGPPropList*			m_pPropList;
	SelObjHolder			m_aObjects;
	CCoordinateDirection	m_focusRefFrame;

	ItemInfo m_Props[PID_END_BOUND - PID_START_BOUND];
};

#endif