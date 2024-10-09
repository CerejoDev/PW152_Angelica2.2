/*
* FILE: TreeCtrlItemData.h
*
* DESCRIPTION: 
*
* CREATED BY: Yang Liu, 2011/03/22
*
* HISTORY: 
*
* Copyright (c) 2011 Perfect World, All Rights Reserved.
*/

#pragma once
#ifndef _TCITEMDATA_H_
#define _TCITEMDATA_H_

enum ItemType
{
   IT_ROOT,
   IT_NODE,
   IT_LEAF
};

enum ItemOperation
{
	IO_NONE,
	IO_APPEND,
	IO_REMOVE,
	IO_DELETE,
	IO_REPLACE,
};

enum ItemFlags
{
	IF_MULTI_SELECTED		= (1<<0),
	IF_MULTI_SELFOCUS		= (1<<1),
	IF_TEXT_COLOR			= (1<<2),
	IF_OPERATION_ENABLE		= (1<<3),
	IF_FIXED_DISABLESEL		= (1<<4),
};

struct CItemData
{
   CItemData(ItemType itemType)
   {
      m_itemType = itemType;
	  m_itemOpt = IO_NONE;
      m_itemTextColor = 0;
	  m_itemUserData = 0;
   }

   ItemType			m_itemType;
   ItemOperation	m_itemOpt;
   COLORREF			m_itemTextColor;
   CFlagMgr<ItemFlags>	m_itemFlags;
   DWORD_PTR m_itemUserData;
};

#endif

