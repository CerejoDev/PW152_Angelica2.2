/*
* FILE: AResFuncs.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/7/2
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResFuncs_H_
#define _AResFuncs_H_

#include "AFileExpDecl.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

struct AResHandle;
class IResInterface;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

//	Initialize the module explicitly by calling this function
//	If you do not call this, the module will not work correctly

//	Parameter: struct IResInterface* pResEngineProxy
//	You must create A3DEngine, A3DGfxEngine, and some other objectes related
//	Before you use the ARes libaray
//	The libarary will use the engine by the interface you passed in here
extern "C" _AFILE_EXP_ bool
AResInitializeModule(IResInterface* pResEngineProxy);

//	Finalize work for the module
//	if you do not use outside managed A3DEngine, then this function may be ignored
//	but it is suggested that you call this explicitly before you release your
//	A3DEngine and A3DGfxEngine
extern "C" _AFILE_EXP_ void
AResFinalizeModule();

//	When you want to get one file's all related resource
//	Create a handle here and will be used later
extern "C" _AFILE_EXP_ bool
AResCreateHandle(const char* szPath, AResHandle** pResHandle);

//	Get all related file number
extern "C" _AFILE_EXP_ int
AResGetFileNum(AResHandle* pResHandle);

//	Get file path by index
extern "C" _AFILE_EXP_ bool
AResGetFilePath(AResHandle* pResHandle, int nIdx, char* szPath, int iBufLen);

//	Close handle
extern "C" _AFILE_EXP_ void
AResCloseHandle(AResHandle* pResHandle);


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_AResFuncs_H_


