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
#include <AString.h>

class IEngineUtility;
class APhysXScene;
class IPhysXObjBase;

class IObjCollection
{
public:
	virtual void RegisterObj(IPhysXObjBase& obj) = 0;
};

// class declaration: QTLoader 
class QTLoader : public IQueueThread<A3DVECTOR3>
{
public:
	QTLoader(IEngineUtility& eu, Scene& scene);
	void SetCollection(IObjCollection& col) { m_pCollection = &col;}
	
private:
	virtual void DoTask(const ElementType& it);

private:
	IObjCollection* m_pCollection;
	IEngineUtility& m_eu;
	Scene& m_Scene;
};

// class declaration: QTUnLoader 
class QTUnLoader : public IQueueThread<IPhysXObjBase*>
{
public:
	QTUnLoader(Scene& scene) : m_Scene(scene) {}

private:
	virtual void DoTask(const ElementType& it);

private:
	Scene& m_Scene;
};

// class declaration: BackgroundTask 
class BackgroundTask : public IObjCollection
{
public:
	BackgroundTask(IEngineUtility& eu, Scene& scene);
	~BackgroundTask();

	void Tick(float dtSec);
	void AddTask(const A3DVECTOR3& pos);

	virtual void RegisterObj(IPhysXObjBase& obj);

private:
	struct ConvexOBJInfo 
	{
		ConvexOBJInfo()
		{
			pObj = 0;
			fAge = 0.0f;
		}

		IPhysXObjBase* pObj;
		float fAge;
	};

private:
	static const int nLoaderNum = 2;
	typedef std::vector<ConvexOBJInfo> vecArray;
	typedef std::vector<QTLoader*> vecLoader;

	APhysXMutex m_Locker;
	vecLoader  m_qtLoader;
	QTUnLoader m_qtUnLoader;
	vecArray   m_Objs;
};

#endif