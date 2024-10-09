// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include "APhysXLabCoreHeaders.h"

#include <afi.h>

#include <A3DObject.h>
#include <A3DDevObject.h>
#include <A3DEngine.h>
#include <A3DDevice.h>
#include <A3DMacros.h>
#include <A3DViewport.h>
#include <A3DCamera.h>
#include <A3DTerrain2.h>

#include <vcclr.h>

#include "MA3DMatrix4.h"
#include "MA3DQuaternion.h"
#include "MA3DVector3.h"
#include "MA3DVector4.h"
#include "MIObjBase.h"
#include "MPhysXObjBase.h"
#include "MScriptBehavior.h"
#include "MScene.h"
#include "MAABB.h"
#include "MRender.h"
#include "MPhysXObjDynamic.h"
#include "MPhysXObjParticle.h"
#include "MRegion.h"


#pragma make_public(CEditor)
#pragma make_public(IObjBase)

#define TRYCALL(exp)  try { (exp); }     \
		catch(System::Exception^ e) {      \
		a_LogOutput(1, "error occured in funciton: %s", __FUNCTION__);\
		System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(e->Message);\
		char* str = (char*)(void*)ptr;\
		a_LogOutput(1, str);\
		System::Runtime::InteropServices::Marshal::FreeHGlobal(ptr);}