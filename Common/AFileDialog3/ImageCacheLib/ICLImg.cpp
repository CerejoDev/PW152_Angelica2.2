/*
* FILE: ICLImg.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/03/29
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "ICLStdAfx.h"
#include "ICLImg.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement ICLImg
//	
///////////////////////////////////////////////////////////////////////////

int ICLImg::ICLIMGTYPE2CXIMGTYPE(int iICLImgType)
{
	switch(iICLImgType)
	{
	case ICLIMG_BMP:
		return CXIMAGE_FORMAT_BMP;
	case ICLIMG_JPG:
		return CXIMAGE_FORMAT_JPG;
	case ICLIMG_PNG:
	    return CXIMAGE_FORMAT_PNG;
	case ICLIMG_TGA:
		return CXIMAGE_FORMAT_TGA;
	default:
	    return CXIMAGE_FORMAT_PNG;
	}
}