/*
 * FILE: ECDecal.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Niuyadong, 2009/9/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <A3DTypes.h>
#include <AAssist.h>
#include <AArray.h>
#include <AList2.h>
#include "ECCounter.h"

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
class A3DViewport;
class A2DSprite;
class ECImageRes;
class AUIManager;
class A3DDevice;
class A3DFTFont;
class A3DTexture;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ECDecal
//	
///////////////////////////////////////////////////////////////////////////

class ECDecal
{
public:		//	Types


public:		//	Constructor and Destructor

	ECDecal();
	virtual ~ECDecal();

public:		//	Attributes

public:		//	Operations

	//	Render routine
	virtual bool Render(A3DViewport* pViewport) { return true; }

	//	Set / Get position
	void SetPos(const A3DVECTOR3& vPos) { m_vPos = vPos; }
	const A3DVECTOR3& GetPos() { return m_vPos; }

	void SetBaseScale(float scale) { m_fBaseScale = scale; }
	float GetBaseScale() { return m_fBaseScale; }

	//	Set color
	void SetColor(A3DCOLOR col)
	{
		//	Set color component and keep alpha component
		m_Color &= 0xff000000;
		m_Color |= (col & 0x00ffffff);
	}

	//	Get color
	A3DCOLOR GetColor() { return m_Color; }

	//	Set alpha
	void SetAlpha(float fAlpha)
	{
		DWORD dwAlpha = (DWORD)(fAlpha * 255.0f);
		if (dwAlpha > 255) dwAlpha = 255;
		m_Color &= 0x00ffffff;
		m_Color |= (dwAlpha << 24);
	}

protected:	//	Attributes

	A3DCOLOR	m_Color;		//	Decal color
	A3DVECTOR3	m_vPos;			//	Position
	float		m_fBaseScale;	//	The scale for this decal
	

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECIconDecal
//	
///////////////////////////////////////////////////////////////////////////

class ECIconDecal : public ECDecal
{
public:		//	Types

	struct ICON
	{
		int		iImageSlot;
		int		iIcon;
		int		iExtX;
		DWORD	dwCol;
	};

public:		//	Constructor and Destructor

	ECIconDecal(ECImageRes* pImageRes);
	virtual ~ECIconDecal();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool AddIcons(int iImageSlot, int iNumIcon, int* aIconIdx, A3DCOLOR col);
	bool AddIcon(int iImageSlot, int iIconIdx, A3DCOLOR col);
	bool AddNumIcons(int iImageSlot, int iNumber, A3DCOLOR col);
	bool AddTextIcons(A3DDevice* pDevice, A3DViewport* pViewport,const ACHAR* sText,A3DFTFont* pFont,bool bShadow, A3DCOLOR col);

	//	Render routine
	virtual bool Render(A3DViewport* pViewport);
	bool Render(int x, int y, float z);

	//	Set / Get scale on x and y axis
	void SetScaleX(float fScale) { m_fScaleX = fScale; }
	float GetScaleX() { return m_fScaleX; }
	void SetScaleY(float fScale) { m_fScaleY = fScale; }
	float GetScaleY() { return m_fScaleY; }
	//	Enable / Disable screen position
	void EnableScreenPos(bool bEnable) { m_bScreenPos = bEnable; }
	//	Set screen posiiton
	void SetScreenPos(int x, int y) { m_sx = x; m_sy = y; }

protected:	//	Attributes

	AArray<ICON, ICON&>	m_aIcons;	//	Icon indices

	float		m_fScaleX;
	float		m_fScaleY;

	bool		m_bScreenPos;	//	true, use screen position
	int			m_sx;
	int			m_sy;
	int			m_iExtX;
	int			m_iExtY;
	ECImageRes* m_pImageRes;

	A3DTexture* m_pTex;
	A3DDevice*	m_pA3DDevice;
					


protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECSpriteDecal
//	
///////////////////////////////////////////////////////////////////////////

class ECSpriteDecal : public ECDecal
{
public:		//	Types

public:		//	Constructor and Destructor

	ECSpriteDecal();
	virtual ~ECSpriteDecal();

public:		//	Attributes

public:		//	Operations

	//	Change sprite object and icon index
	bool ChangeSpriteIcon(A2DSprite* pA2DSprite, int iIconIdx);

	//	Render routine
	virtual bool Render(A3DViewport* pViewport);

	//	Set / Get scale on x and y axis
	void SetScaleX(float fScale) { m_fScaleX = fScale; }
	float GetScaleX() { return m_fScaleX; }
	void SetScaleY(float fScale) { m_fScaleY = fScale; }
	float GetScaleY() { return m_fScaleY; }
	//	Set / Get z position
	void SetZValue(float z) { m_fz = z; }
	float GetZValue() { return m_fz; }
	//	Set / Get position on screen
	void SetScreenPos(int x, int y) { m_ix = x; m_iy = y; }
	void GetScreenPos(int* px, int* py) { *px = m_ix; *py = m_iy; }
	//	Get extent
	void GetExtent(int* pix, int* piy) { *pix = m_iExtX, *piy = m_iExtY; }

protected:	//	Attributes

	A2DSprite*	m_pA2DSprite;
	int			m_iIconIdx;

	float		m_fScaleX;
	float		m_fScaleY;
	int			m_ix;
	int			m_iy;
	float		m_fz;
	int			m_iExtX;		//	Decal extent
	int			m_iExtY;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECFaceDecal
//	
///////////////////////////////////////////////////////////////////////////

class ECFaceDecal : public ECSpriteDecal
{
public:		//	Types

public:		//	Constructor and Destructor

	ECFaceDecal();
	virtual ~ECFaceDecal();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(AUIManager* pUIMan, int nEmotionSet, int iFace);

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(A3DViewport* pViewport);

	//	Set / Get scale on x and y axis
	void SetScaleX(float fScale) { m_fScaleX = fScale; }
	float GetScaleX() { return m_fScaleX; }
	void SetScaleY(float fScale) { m_fScaleY = fScale; }
	float GetScaleY() { return m_fScaleY; }
	//	Set / Get z position
	void SetZValue(float z) { m_fz = z; }
	float GetZValue() { return m_fz; }
	//	Set / Get position on screen
	void SetScreenPos(int x, int y) { m_ix = x; m_iy = y; }
	void GetScreenPos(int* px, int* py) { *px = m_ix; *py = m_iy; }
	//	Get extent
	void GetExtent(int* pix, int* piy) { *pix = m_iExtX, *piy = m_iExtY; }

protected:	//	Attributes

	int			m_iStartFrame;
	int			m_iNumFrame;
	int			m_iFrameCnt;
	int			m_pFrameTick[40];

	int			m_iFace;
	ECCounter	m_AnimCnt;		//	Animation time counter

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECBubbleDecal
//	
///////////////////////////////////////////////////////////////////////////

class ECBubbleDecal
{
public:		//	Types
	enum
	{
		BUBBLE_NORMAL = 0,
		BUBBLE_JUMPOUT,
		BUBBLE_WAVE,
		BUBBLE_TICK3,// 50%由虚变实-> 25%停留->25%任意方向消失
		BUBBLE_TICK4,// 40%由大变小->10%震动-> 25%停留->25%消失
		BUBBLE_TICK5,// 50%由虚变实-> 25%停留->25%消失(受重力影响)
	};

	
public:		//	Constructor and Destructor

	ECBubbleDecal(ECImageRes* pImageRes);
	virtual ~ECBubbleDecal();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	bool TickType0(DWORD dwDeltaTime);
	bool TickType1(DWORD dwDeltaTime);
	bool TickType2(DWORD dwDeltaTime);
	bool TickType3(DWORD dwDeltaTime);
	bool TickType4(DWORD dwDeltaTime);
	bool TickType5(DWORD dwDeltaTime);
	//	Render routine
	bool Render(A3DViewport* pViewport);

	//	Set / Get position
	void SetPos(const A3DVECTOR3& vPos) { m_pDecal->SetPos(vPos); }
	const A3DVECTOR3& GetPos() { return m_pDecal->GetPos(); }
	//	Get decal object
	ECDecal* GetDecal() { return m_pDecal; }

	//	Move decal from one place to another
	void SetMoveInfo(const A3DVECTOR3& vVel,int iMoveType, DWORD dwMoveTime,bool bSinMove);
	//	Set life time
	void SetLifeTime(DWORD dwLife) { m_LifeCnt.SetPeriod(dwLife); }
	//	Get cur life count
	DWORD GetCurLife(){return m_LifeCnt.GetCounter();}
	//	Get life period
	DWORD GetLifePeriod(){return m_LifeCnt.GetPeriod();}
	//	Life time end ?
	bool IsDead() { return m_LifeCnt.IsFull() ? true : false; }

protected:	//	Attributes

	ECDecal*	m_pDecal;		//	Decal object
	A3DVECTOR3	m_vMoveVel;		//	Move velocity
	ECCounter	m_MoveCnt;		//	Move time counter
	ECCounter	m_LifeCnt;		//	Life time counter
	bool		m_bSinMove;		//	Different move effect like sin value
	int			m_iBubbleType;	//	Move type this bubble

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class ECBubbleDecalList
//	
///////////////////////////////////////////////////////////////////////////

class ECBubbleDecalList
{
public:		//	Types

public:		//	Constructor and Destructor

	ECBubbleDecalList();
	virtual ~ECBubbleDecalList();

public:		//	Attributes
	
public:		//	Operations

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(A3DViewport* pViewport);

	//	Add a bubble decal
	ECBubbleDecal* AddDecal(ECImageRes* pImageRes,const A3DVECTOR3& vPos, int iBubbleType, const A3DVECTOR3& vVel,DWORD dwLife = 2000,A3DViewport* pView = NULL);

protected:	//	Attributes

	APtrList<ECBubbleDecal*>	m_DecalList;	//	Latent decals
	int m_iDensity;// 低空Bubble的密度，用于解决重合问题
	

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



