/*
 * FILE: APhysXLabCoreHeaders.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Yang Liu, 2010/02/01
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Perfect World, All Rights Reserved.
 */

// Note: Do not include this file in head files!

#pragma once
#ifndef _APHYSXLAB_CORE_HEADERS_H_
#define _APHYSXLAB_CORE_HEADERS_H_

#include <limits>

#ifdef UNICODE
	#define _TAS2WC(x)   AS2WC(x)
	#define _TWC2AS(x)   WC2AS(x)
#else
	#define _TAS2WC(x)   x
	#define _TWC2AS(x)   x
#endif

#include <APhysX.h>

#include <A3DMatrix.h>
class A3DFrame; // Must be declared before "a A3DLitModel.h". 
#include <A3DLitModel.h>
#include <AC.h>
#include <EC_Model.h>
#include <A3DModelPhysics.h>
#include <A3DModelPhysSync.h>
#include <A3DSkinPhysSync.h>
#include <A3DModelPhysSyncData.h>
#include <ClothSkinConfig.h>

#include "Render.h"
#include "Game.h"
#include "GraphicsSyncRB.h"
#include "CameraController.h"

#include "PhysXObjBase.h"
#include "PhysXObjStatic.h"
#include "PhysXObjParticle.h"
#include "PhysXObjForceField.h"
#include "PhysXObjMgr.h"
#include "PhysXObjDynamic.h"
#include "PhysXObjSkinModel.h"
#include "PhysXObjECModel.h"
#include "ActionPlayerBase.h"

#include "MainActor.h"
#include "SubThread.h"
#include "TConvexMesh.h"
#include "OverlapBounceDamping.h"
#include "CommonMovement.h"
#include "TScenario.h"

#include "BrushcharCtrler.h"
#include "PhysXObjChb.h"

#endif


