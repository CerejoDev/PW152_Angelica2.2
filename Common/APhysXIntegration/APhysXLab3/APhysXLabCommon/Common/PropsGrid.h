/*
 * FILE: PropsGrid.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/06/02
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_PROPSGRID_H_
#define _APHYSXLAB_PROPSGRID_H_

#include <comutil.h>

enum PropID
{
	// Note: keep the children items are sequence(support iteration)!
	PID_START_BOUND = 0,
	PID_Unknown = PID_START_BOUND,
	PID_GROUP_Basic,
	PID_ITEM_Name,
	PID_ITEM_LocalName,
	PID_ITEM_Type,
	PID_ITEM_State,
	PID_ITEM_File,
	PID_ITEM_Scripts_Attached,
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
	//-----region props----  
	PID_GROUP_ShapeVolume,
	PID_ITEM_Shape,
	PID_SUBGROUP_Extent_xyz,
	PID_SGITEM_Extent_x,
	PID_SGITEM_Extent_y,
	PID_SGITEM_Extent_z,
	PID_ITEM_Radius,
	PID_ITEM_Height,
	//--------------------------
	//-----Socket props----  
	PID_GROUP_Socket,
	PID_ITEM_SocketDesc,
	PID_ITEM_SocketType,
	PID_ITEM_SocketName,
	//--------------------------
	//-----Socket props----  
	PID_GROUP_SpotLight,
	PID_ITEM_Range,
	PID_ITEM_InnerTheta,
	PID_ITEM_OuterPhi,
	PID_SUBGROUP_Attenuation_xyz,
	PID_SGITEM_Attenuation_x,
	PID_SGITEM_Attenuation_y,
	PID_SGITEM_Attenuation_z,
	PID_SUBGROUP_Color_rgba,
	PID_SGITEM_Color_r,
	PID_SGITEM_Color_g,
	PID_SGITEM_Color_b,
	PID_SGITEM_Color_a,
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
	//--------------------------
	//-----common force field props----  
	PID_GROUP_FF_Common,
	PID_ITEM_FF_Type,
	PID_ITEM_Coordinate,
	PID_GROUP_FF_Scale,
	PID_SUBGROUP_RBScale,
	PID_SGITEM_RBS_Type,
	PID_SGITEM_RBS_Value,
	PID_SUBGROUP_ClothScale,
	PID_SGITEM_ClothS_Type,
	PID_SGITEM_ClothS_Value,
	PID_SUBGROUP_FluidScale,
	PID_SGITEM_FluidS_Type,
	PID_SGITEM_FluidS_Value,
	PID_SUBGROUP_SBScale,
	PID_SGITEM_SBS_Type,
	PID_SGITEM_SBS_Value,
	//-----wind force field props----  
	PID_GROUP_FF_Wind,
	PID_SUBGROUP_Force_xyz,
	PID_SGITEM_Force_x,
	PID_SGITEM_Force_y,
	PID_SGITEM_Force_z,
	PID_ITEM_FFW_WindT,
	PID_ITEM_FFW_RestT,
	PID_ITEM_FFW_CycleT,
	PID_ITEM_FFW_CycleDist,
	//-----vortex force field props----  
	PID_GROUP_FF_Vortex,
	PID_ITEM_FFV_TargetVel,
	PID_ITEM_FFV_TargetVelScale,
	PID_ITEM_FFV_TargetR,
	PID_ITEM_FFV_TargetRScale,
	PID_ITEM_FFV_ElevationForce,
	PID_ITEM_FFV_ElevationScale,
	PID_ITEM_FFV_CentripetalForce,
	PID_ITEM_FFV_Noise,
	//-----gravitation force field props----  
	PID_GROUP_FF_Gravitation,
	PID_SUBGROUP_G_xyz,
	PID_SGITEM_G_x,
	PID_SGITEM_G_y,
	PID_SGITEM_G_z,
	//-----explosion force field props----  
	PID_GROUP_FF_Explosion,
	PID_ITEM_FFE_CenterF,
	PID_ITEM_FFE_Radius,
	PID_ITEM_FFE_LifeTime,
	PID_ITEM_FFE_QuadricFallOff,
	//-----buoyancy force field props----  
	PID_GROUP_FF_Buoyancy,
	PID_ITEM_FFB_Density,
	//-----boss force field props----  
	PID_GROUP_FF_Boss,
	PID_ITEM_FFB_Force,
	PID_ITEM_FFB_Radius,
	//-----custom force field props----  
	PID_GROUP_FF_Custom,
	PID_SUBGROUP_Const_xyz,
	PID_SGITEM_Const_x,
	PID_SGITEM_Const_y,
	PID_SGITEM_Const_z,
	PID_SUBGROUP_PosMultiplier,
	PID_SUBGROUP_PMRow0_xyz,
	PID_SGITEM_PMR0_x,
	PID_SGITEM_PMR0_y,
	PID_SGITEM_PMR0_z,
	PID_SUBGROUP_PMRow1_xyz,
	PID_SGITEM_PMR1_x,
	PID_SGITEM_PMR1_y,
	PID_SGITEM_PMR1_z,
	PID_SUBGROUP_PMRow2_xyz,
	PID_SGITEM_PMR2_x,
	PID_SGITEM_PMR2_y,
	PID_SGITEM_PMR2_z,
	PID_SUBGROUP_TargetPos_xyz,
	PID_SGITEM_TP_x,
	PID_SGITEM_TP_y,
	PID_SGITEM_TP_z,
	PID_SUBGROUP_VelMultiplier,
	PID_SUBGROUP_VMRow0_xyz,
	PID_SGITEM_VMR0_x,
	PID_SGITEM_VMR0_y,
	PID_SGITEM_VMR0_z,
	PID_SUBGROUP_VMRow1_xyz,
	PID_SGITEM_VMR1_x,
	PID_SGITEM_VMR1_y,
	PID_SGITEM_VMR1_z,
	PID_SUBGROUP_VMRow2_xyz,
	PID_SGITEM_VMR2_x,
	PID_SGITEM_VMR2_y,
	PID_SGITEM_VMR2_z,
	PID_SUBGROUP_TargetVel_xyz,
	PID_SGITEM_TV_x,
	PID_SGITEM_TV_y,
	PID_SGITEM_TV_z,
	PID_ITEM_FFC_TorusR,
	PID_SUBGROUP_Noise_xyz,
	PID_SGITEM_Noise_x,
	PID_SGITEM_Noise_y,
	PID_SGITEM_Noise_z,
	PID_SUBGROUP_FalloffLN_xyz,
	PID_SGITEM_FOLN_x,
	PID_SGITEM_FOLN_y,
	PID_SGITEM_FOLN_z,
	PID_SUBGROUP_FalloffQuad_xyz,
	PID_SGITEM_FOQD_x,
	PID_SGITEM_FOQD_y,
	PID_SGITEM_FOQD_z,

	PID_END_BOUND
};

class PropItem
{
public:
	virtual ~PropItem() {}
	virtual void SetValue(const _variant_t& varValue) = 0;
	virtual const _variant_t& GetValue() const = 0;

	virtual BOOL AddSubItem(PropItem* pProp) = 0;
	virtual int  GetSubItemsCount() const = 0;
	virtual void SetSubItemValue(int nIndex, const _variant_t& varValue) = 0;
	virtual const _variant_t& GetSubItemValue(int nIndex) const = 0; 

	virtual BOOL AddOption(LPCTSTR lpszOption, BOOL bInsertUnique = TRUE) = 0;
	virtual void RemoveAllOptions() = 0;
	virtual int  GetOptionCount() const = 0;

	virtual BOOL IsEnabled() const { return FALSE; }
	virtual void Enable(BOOL bEnable = TRUE) {}
	virtual void Expand (BOOL bExpand = TRUE) {}

	virtual void AllowEdit(BOOL bAllow = TRUE) {}
	virtual DWORD_PTR GetData () const { return NULL; }

public:
	static const _variant_t varMultiFlag;

protected:
	PropItem() {}
};

class IPropList
{
public:
	virtual ~IPropList() {}
	virtual int  AddProperty(PropItem* pProp, BOOL bRedraw = TRUE, BOOL bAdjustLayout = TRUE) = 0;
	virtual int  GetPropertyCount() const = 0;

	virtual void RemoveAll() = 0;
	virtual BOOL RedrawWindow() = 0;
	virtual void AdjustLayout() = 0;
	virtual void EnableCommands(const std::vector<TString>& cmds) = 0; 
	virtual void DisableCommands() = 0;

	virtual PropItem* CreatePropItem(const std_TString& strGroupName, DWORD_PTR dwData = 0,	BOOL bIsValueList = FALSE) = 0;
	virtual PropItem* CreatePropItem(const std_TString& strName, const _variant_t& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0, BOOL bFilePicker = FALSE,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL,
		LPCTSTR lpszValidChars = NULL) = 0;
	virtual void ReleasePropItem(PropItem* pItem, bool* outInList = 0) = 0;
};

#endif