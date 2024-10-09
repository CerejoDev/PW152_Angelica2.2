/*
 * FILE: FlagMgr.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/10/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_FLAGMGR_H_
#define _APHYSXLAB_FLAGMGR_H_

template<class TBitEnum>
class CFlagMgr
{
public:
	CFlagMgr() : m_value(0) {}
	virtual ~CFlagMgr() {};

	bool ReadFlag(const TBitEnum& flag) const { return (m_value & flag)? true : false; }
	void RaiseFlag(const TBitEnum& flag) { m_value |= flag; }
	void ClearFlag(const TBitEnum& flag) { m_value &= ~flag; }
	void ClearAllFlags() { m_value = 0; }

	int  GetValue() const { return m_value; }
	void SetValue(DWORD combine) { m_value = combine; } 

private:
	DWORD m_value;
};

#endif