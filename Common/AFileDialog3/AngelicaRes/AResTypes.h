/*
* FILE: AResTypes.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/06/30
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _AResTypes_H_
#define _AResTypes_H_

#include <string>
#include "AResSharedPtr.h"

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

typedef std::string AResString;
const AResString STR_EMPTY;

class ARes;
class AResAfm;
class AResBsp;
class AResChf;
class AResEcm;
class AResGfx;
class AResLmd;
class AResSingle;
class AResSki;
class AResSmd;
class AResSpt;
class AResUmd;
class AResUmds;
class AResManager;
class AResFactory;

typedef SharedPtr<ARes> AResPtr;
typedef SharedPtr<AResAfm> AResAfmPtr;
typedef SharedPtr<AResBsp> AResBspPtr;
typedef SharedPtr<AResChf> AResChfPtr;
typedef SharedPtr<AResEcm> AResEcmPtr;
typedef SharedPtr<AResGfx> AResGfxPtr;
typedef SharedPtr<AResLmd> AResLmdPtr;
typedef SharedPtr<AResSingle> AResSinglePtr;
typedef SharedPtr<AResSki> AResSkiPtr;
typedef SharedPtr<AResSmd> AResSmdPtr;
typedef SharedPtr<AResSpt> AResSptPtr;
typedef SharedPtr<AResUmd> AResUmdPtr;
typedef SharedPtr<AResUmds> AResUmdsPtr;
typedef SharedPtr<AResManager> AResManagerPtr;
typedef SharedPtr<AResFactory> AResFactroyPtr;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

inline const char* str(const std::string& s)
{
	return s.c_str();
}

inline int len(const std::string& s)
{
	return static_cast<int>(s.size());
}


#endif	//	_AResTypes_H_


