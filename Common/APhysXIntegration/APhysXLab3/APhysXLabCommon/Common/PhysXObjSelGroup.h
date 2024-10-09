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
#include <list>
#include "CoordinateDirection.h"

class IPhysXObjBase;
class IPropObjBase;
class IPropPhysXObjBase;
class CDynProperty; 
class CAPhysXCCMgr;
class IObjStateReport;
class RayHitOwner;
class IPropList;

class CPhysXObjSelGroup : public IObjStateReport, public ApxObjOperationReport
{
public:
	typedef CCoordinateDirection::EditType EditType;
	static const TCHAR*		szEmpty;
	static const _variant_t varEmpty;
	static bool ExtractVector(const PropItem& prop, A3DVECTOR3& outVec);
	static bool ExtractColor(const PropItem& prop, A3DCOLORVALUE& outVec);
	static bool ApplyColorValue(PropItem& prop, const A3DCOLORVALUE& value);
	static PropID GetForceFieldSpecialGroupPropID(const APhysXObjectType ffType);

public:
	CPhysXObjSelGroup();
	void SetRenderUtility(IRenderUtility* pRU) { m_focusRefFrame.g_Render = pRU; }
	void SetStateReport(IObjStateReport* pStateReport) { m_pStateReport = pStateReport; }
	void SetRayHitOwner(RayHitOwner& rho) { m_pRHO = &rho; }
	void SetPropList(IPropList* Proplist) { m_pPropList = Proplist; }
	void SetGroupPos(const A3DVECTOR3& newPos_idx0, bool fromRefFrame = false);

	bool EnterRuntime(APhysXScene& aPhysXScene);
	void LeaveRuntime();
	bool IsRunTime() const { return m_Status.ReadFlag(SF_IS_GAME_RUNTIME); }
	bool IsDrawItemEnable() const;
	void DrawFrame(A3DWireCollector* pWC);
	void EnablePropItem(PropID id, bool bIsEnable);
	bool ImportAndExportFFData(bool import, const AString& strFile);

	int Size() const { return int(m_aObjects.size()); }
	// return -1, if there is no the parent object.
	// return -1, if the parent object isn't in the select group.
	// or return the index of parent object.
	int GetParentIndex(int index) const;
	bool IsOnlyFocusOn(const ObjManager::ObjTypeID& objType) const;
	IObjBase* GetFocusObject() const;
	IObjBase* GetObject(int index) const;
	IObjBase* GetFocusObjectTopAncestor(int* pOutTopAncestorIdx = 0) const;
	int GetTopAncestorObjects(std::vector<IObjBase*>& outObjs) const;

	void AppendObject(IObjBase& obj);
	void RemoveObject(const IObjBase& obj);
	void ReplaceWithObject(IObjBase* pObj);
	void ReplaceWithObjects(const std::vector<IObjBase*>& objs);

	void RefreshFrame();
	void SetRefFrameEditMode(const EditType& et) { m_focusRefFrame.SetEditType(et); }
	EditType GetRefFrameEditMode() const { return m_focusRefFrame.GetEditType(); }
	bool QueryRefFrameEditMode(const EditType& et) const { return et == GetRefFrameEditMode(); }
	bool UpdateRefFrameOnLButtonDown(const int x, const int y, const unsigned int nFlags);
	bool UpdateRefFrameOnMouseMove(const int x, const int y, const unsigned int nFlags, bool OnlyRefFrame);

	void UpdatePropItems();
	bool PropertyChanged(PropItem& prop);

	bool ReadUpdateMark(const PropID& propID);
	bool RaiseUpdateMark(const PropID& propID, bool bToDel = false);
	bool RaiseUpdateMarkToCCFlags();
	bool RaiseUpdateMarkToShapeVolume(CRegion::ShapeType st, bool bToDel = false);
	void RaiseUpdateMarkToSocketName() { m_Status.RaiseFlag(SF_HAS_SOMETHING_DIRTY); m_ToUpdateSocketName = true; }
	bool AllSelectedHaveTheSameShapeType() const;

	bool IsMerge(const IObjBase* pObj) const;
	void ExecuteItemEnable(const PropID& nID, bool bEnable);
	void ExecuteUpdateItem(const PropID& nID, const _variant_t& objVal, bool bIsMerge);
	void ExecuteUpdateItemToShapeVolume(CRegion::ShapeType st, const A3DVECTOR3& vol, bool bIsMerge);
	void FillGroupBasic(const IPropObjBase& objProp);
	void FillGroupPoseScale(const IPropObjBase& objProp);
	void FillGroupShapeVolume(const CRegion& rgn);
	void FillGroupShapeVolume(const CRegion::ShapeType st, const A3DVECTOR3& vol, bool bIsMerge);
	void FillGroupSocket(const ApxObjSocketBase& objSocket);
	void FillGroupSpotLight(const ApxObjSpotLight& objSpotLight);
	void FillGroupDrivenBy(const IPropPhysXObjBase& objProp);
	void FillGroupRoleBehavior(const CDynProperty& objProp);
	void FillGroupApxRBFlags(const CDynProperty& objProp);
	void FillGroupCCFlags(const CDynProperty& objProp);
	void OnUpdateBehaviorEnable(const CDynProperty& objProp);

	void FillAllFFProps(const CPhysXObjForceField& objFF, bool bIncludeCommon = true);
	void FillFFCommon(const APhysXForceFieldObjectDesc& ffDesc, bool bIsMerge);
	void FillWindFF(const APhysXWindForceFieldObjectDesc& ffDesc, bool bIsMerge);
	void FillVortexFF(const APhysXVortexForceFieldObjectDesc& ffDesc, bool bIsMerge);
	void FillGravitationFF(const APhysXGravitationForceFieldObjectDesc& ffDesc, bool bIsMerge);
	void FillExplosionFF(const APhysXExplosionForceFieldObjectDesc& ffDesc, bool bIsMerge);
	void FillBuoyancyFF(const APhysXBuoyancyForceFieldObjectDesc& ffDesc, bool bIsMerge);
	void FillBossFF(const APhysXBossForceFieldObjectDesc& ffDesc, bool bIsMerge);
	void FillCustomFF(const APhysXForceFieldObjectDesc& ffDesc, bool bIsMerge);

	virtual void OnNotifySleep(const IObjBase& obj);
	virtual void OnAddChildNotify(ApxObjBase* pParent, ApxObjBase* pChild);
	virtual void OnRemoveChildNotify(ApxObjBase* pParent, ApxObjBase* pChild);

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
			pPrevious = 0;
			newValue.Clear();
			ClearAllFlags();
			pChildren.clear();
		}

		void SetNewValue(const _variant_t& nv, bool bIsMerge);

		int nCount;
		PropItem* pItem;
		PropItem* pLeader;
		ItemInfo* pPrevious;
		_variant_t newValue;
		const _variant_t* pMultiValueMask;
		std::vector<int> pChildren;	
	};

	struct SelObj
	{
		SelObj()
		{
			pObj = 0; originScale = 0.0f; distOffset.Set(0.0f, 0.0f, 0.0f);
			bIsIndependent = true;
			bIsTopAncestor = false;
		}
		IObjBase* operator->() { return pObj; }

		IObjBase* pObj;
		float	   originScale;
		A3DVECTOR3 distOffset;
		bool	bIsIndependent;
		bool	bIsTopAncestor;
	};

	enum StateFlags
	{
		SF_REFRESH_FRAME_DISABLE = (1<<0),
		SF_IS_GAME_RUNTIME		 = (1<<1),
		SF_HAS_SOMETHING_DIRTY	 = (1<<2),
		SF_PROPLIST_LAYOUT_DIRTY = (1<<3),
	};

	class FamilyCondition
	{
	public:
		virtual ~FamilyCondition() = 0 {};
		virtual bool operator()(const IObjBase& objSample, const SelObj& objX, int* poutCodeSampleToX) const = 0;
	};

	class FCRemove : public FamilyCondition
	{
	public:
		virtual bool operator()(const IObjBase& objSample, const SelObj& objX, int* poutCodeSampleToX) const;
	};

	class FCChange : public FamilyCondition
	{
	public:
		virtual bool operator()(const IObjBase& objSample, const SelObj& objX, int* poutCodeSampleToX) const;
	};

private:
	// forbidden behavior
	CPhysXObjSelGroup(const CPhysXObjSelGroup& rhs);
	CPhysXObjSelGroup& operator= (const CPhysXObjSelGroup& rhs);

	bool IsEnableGroupOperation(int idx) const;
	bool IsEnableClone(int idx) const;
	void SetGroupRot(const float angleRadians, const A3DVECTOR3& localAxis, const A3DVECTOR3& globalAxis);
	void SetGroupScale();
	void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
	bool SetScale(const float& scale);
	bool MakeCmdsTitle(std::vector<TString>& outTitles) const;

	void OnAddObject(IObjBase& obj);
	void RefreshFramePose(bool posOnly = false);
	void RefreshFrameScale();
	void RefreshSelObjOffset();

	void UpdateFamilyInfoOnAdd(SelObj& objAdded);
	void UpdateFamilyInfoOnRemove(SelObj& objRemoved);
	void UpdateFamilyInfo(const IObjBase& objSample, const FamilyCondition& pred);

	int GetIndex(const PropID& nID);
	void SendToPropsGrid();

	int  IncCheckinCount(const PropID& nID, bool bIncludeChiefItem);
	void SetChiefInfo(const PropID& nID, PropItem* pI);
	void SetItemInfo(const PropID& nID, PropItem& I, const PropID& nParentID, bool isGroup, const _variant_t* pMultiValueMask = 0);

	unsigned int DeleteProps(unsigned int idx, bool SaveUpdateInfo);
	unsigned int ClearItemInfo(unsigned int idx, bool SaveUpdateInf);
	void AddGroupItem(PropItem* pItem, ItemInfo* pPrev);
	void CommitModified();
	void OnUpdateCCTypeContent(const CDynProperty& objProp);
	PropItem* CreateRegionVolumeProp(const PropID& nID, const _variant_t& objVal);
	PropItem* CreateCCFlagProp(const PropID& nID, const _variant_t& objVal);
	PropItem* CreateFFScaleProp(const PropID& nID, const _variant_t& objVal);
	void UpdateSocketNames();
	void OnUpdateBoneHookName(PropItem& prop, A3DSkinModel* pSkinModel, ApxSkinModelSocketType smST);

private:
	typedef		std::vector<SelObj>		SelObjHolder;
	typedef		std::list<PropItem*>	lstPropItems;

	IObjStateReport* m_pStateReport;
	RayHitOwner* m_pRHO;
	CFlagMgr<StateFlags> m_Status;

	IPropList*				m_pPropList;
	SelObjHolder			m_aObjects;
	lstPropItems			m_pGroupItems;
	CCoordinateDirection	m_focusRefFrame;
	std::vector<IObjBase*>  m_objsBackup;

	bool m_ToUpdateSocketName;
	ItemInfo m_Props[PID_END_BOUND - PID_START_BOUND];
};

#endif