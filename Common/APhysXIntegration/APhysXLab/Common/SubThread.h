/*
 * FILE: SubThread.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2009/06/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

#pragma once
#ifndef _APHYSXLAB_SUBTHREAD_H_
#define _APHYSXLAB_SUBTHREAD_H_

#include "SubThreadInterface.h"

class CRender;
class APhysXScene;
class IPhysXObjBase;
class TestConvexMesh;

// class declaration: QTLoader 
class QTLoader : public IQueueThread<IPhysXObjBase*>
{
public:
	QTLoader(CRender& render, APhysXScene& scene) : m_Render(render), m_Scene(scene) {}

private:
	virtual void DoTask(const ElementType& it);
		
private:
	CRender&	 m_Render;
	APhysXScene& m_Scene;
};

// class declaration: QTUnLoader 
class QTUnLoader : public IQueueThread<IPhysXObjBase*>
{
private:
	virtual void DoTask(const ElementType& it);
};

// class declaration: BackgroundTask 
class BackgroundTask
{
public:
	BackgroundTask(CRender& render, APhysXScene& scene) : m_qtWakeUp(render, scene) {}

	void AddTask(IPhysXObjBase& obj, const bool bIsRelease);
	
private:
	QTLoader   m_qtWakeUp;
	QTUnLoader m_qtRelease;
};

#endif