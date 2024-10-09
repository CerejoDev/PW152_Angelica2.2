/*
 * FILE: SubThread.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#include "stdafx.h"

void QTLoader::DoTask(const ElementType& it)
{
	it->WakeUp(m_Render, &m_Scene);
}

void QTUnLoader::DoTask(const ElementType& it)
{
	it->Sleep(true);
}

void BackgroundTask::AddTask(IPhysXObjBase& obj, const bool bIsRelease)
{
	if (bIsRelease)
		m_qtRelease.AddTask(&obj);
	else
		m_qtWakeUp.AddTask(&obj);
}
