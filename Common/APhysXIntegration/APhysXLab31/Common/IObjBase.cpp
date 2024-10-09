/*
 * FILE: IObjBase.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2011/05/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

IObjBase::IObjBase() : m_pOMD(0), m_pProperties(0)
{

}

IPropObjBase* IObjBase::CreatePropSnap() const
{
	assert(0 != m_pProperties);
	return m_pProperties->CreateMyselfInstance(true);
}

bool IObjBase::SaveSerialize(NxStream& stream) const
{
	return m_pProperties->SaveToFile(stream);
}

bool IObjBase::LoadDeserialize(NxStream& stream, bool& outIsLowVersion)
{
	return m_pProperties->LoadFromFile(stream, outIsLowVersion);
}
