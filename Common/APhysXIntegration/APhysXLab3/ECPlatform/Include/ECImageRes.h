/*
 * FILE: ECImageRes.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Niuyadong, 2009/9/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <A3DTypes.h>
#include <AArray.h>
#include <hashtab.h>
#include <A3DVertex.h>

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

class A2DSprite;
class A2DSpriteBuffer;
class ECGraphicsModule;
class A3DTexture;
class A3DStream;
///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECImageRes
//	
///////////////////////////////////////////////////////////////////////////

class ECImageRes
{
public:		//	Types

	//	Images
	struct IMAGE
	{
		A2DSprite*	pImage;		//	Image
		int			iNumItem;	//	Number of image item
		int			iCurItem;	//	Current item
		int			iWidth;		//	Image width
		int			iHeight;	//	Image height

		A2DSpriteBuffer*	pImageBuf;	//	Image buffer
	};
	
	//	Registered draw
	struct REGDRAW
	{
		DWORD	dwSlot;
		int		x;
		int		y;
		DWORD	dwCol;
		int		iItem;
		float	z;
		int		iDstWid;
		int		iDstHei;
	};

	//	Registered draw
	struct REG_TEXT
	{
		float	x;
		float	y;
		float	z;
		int		iWid;
		int		iHei;
		A3DTexture* pTex;
		A3DCOLOR dwCol;
	};

	typedef abase::hashtab<IMAGE*, int, abase::_hash_function>	ImageTable;

public:		//	Constructor and Destructor

	ECImageRes(ECGraphicsModule* pGraphics);
	virtual ~ECImageRes();

public:		//	Attributes

public:		//	Operations

	//	Load all images
	bool LoadAllImages(const char* szImage);
	//	Release all images
	void Release();

	//	Get image item size
	void GetImageItemSize(DWORD dwSlot, int iItem, int* piWid, int* piHei);
	//	Draw image item to backbuffer
	void DrawImage(DWORD dwSlot, int x, int y, A3DCOLOR col, int iItem=0, float z=0.0f, int iDstWid=0, int iDstHei=0);
	//	Draw image item to backbuffer without scale
	void DrawImageNoScale(DWORD dwSlot, int x, int y, A3DCOLOR col, int iItem=0, float z=0.0f);
	//	Flush all rendered images
	void Flush();

	//	Register draw
	void RegisterDraw(DWORD dwSlot, int x, int y, A3DCOLOR col, int iItem=0, float z=0.0f, int iDstWid=0, int iDstHei=0);
	//	Present all registered draws
	void PresentDraws();

	//	Register draw
	void RegisterText(A3DTexture* pTex,float x, float y, float z, int iWid, int iHei,A3DCOLOR dwCol);

protected:	//	Attributes

	ImageTable			m_ImageTab;	//	Image table
	DWORD				m_iNum;
	ECGraphicsModule*	m_pGraphics;

	APtrArray<REGDRAW*>		m_aRegDraws;	//	Registered draw
	APtrArray<REG_TEXT*>	m_aRegText;		//	Registered draw
	bool					m_bSortDraws;	//	true, needs to sort	m_aRegDraws
	bool					m_bSortText;	//	true, needs to sort	m_aRegText

	A3DStream*			m_pStream;				
	A3DTLVERTEX			m_verts[4];				
	WORD				m_indices[6];
	
protected:	//	Operations

	//	Release speicfied image
	void ReleaseImage(DWORD dwSlot);

	//	Load image from file
	bool LoadImage(DWORD dwSlot, char* szFile, int iWidth, int iHeight, int iNumItem, A3DRECT* aRects, int iNumElem=1);
	//	Load image which has only one item
	bool LoadImage(DWORD dwSlot, char* szFile, int iWidth, int iHeight, int iNumElem=1);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



