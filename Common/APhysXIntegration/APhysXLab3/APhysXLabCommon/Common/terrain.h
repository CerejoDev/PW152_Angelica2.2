/*
 * FILE: Terrain.h
 *
 * DESCRIPTION: header for terrain class on server side
 *
 * CREATED BY: Hedi, 2004/11/22 
 *
 * MODIFIED BY: HuangXin, 2009/3/3
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef __GS_TERRAIN_H__
#define __GS_TERRAIN_H__

#include <amemory.h>
#include <A3DVector.h>

typedef struct _TERRAINCONFIG
{
	int			nNumRows;		// 分几行呀
	int			nNumCols;		// 分几列呀
	int			nAreaWidth;		// 每块的宽度（以格计算）
	int			nAreaHeight;	// 每块的高度（以格计算）
	float		vGridSize;		// 每小格的尺寸

	float		vHeightMin;		// 0.0 对应的高度
	float		vHeightMax;		// 1.0 对应的高度

	char		szMapPath[256];	// 地图在那里呀，不要最后一个斜杠呀

} TERRAINCONFIG;

class CTerrain
{
private:
	// height map buffer and width height of it
	float *				m_pHeights;			// height map points of this terrain object
	int					m_nNumVertX;		// how many points in one row of this terrain object
	int					m_nNumVertZ;		// how many points in one column of this terrain object

	// range of this terrain object
	float				m_ox;				// origin (left-top point) of this terrain object
	float				m_oz;

	float				m_vGridSize;
	float				m_vGridSizeInv;		// value to be multiplied to get grid coords.

	// configuration data
	TERRAINCONFIG		m_config;			// terrain configuration data

public:
	inline float* GetHeights()			{ return m_pHeights; }
	inline int GetNumVertX() const			{ return m_nNumVertX; }
	inline int GetNumVertZ() const			{ return m_nNumVertZ; }

protected:

public:
	CTerrain();
	CTerrain(const CTerrain& rhs);
	CTerrain& operator = (const CTerrain& rhs);
	~CTerrain();

	bool Init(const TERRAINCONFIG& config, float xmin, float zmin, float xmax, float zmax);
	bool Release();

	bool GetHeightAt(float x, float z, float& y, A3DVECTOR3* pNormal = NULL) const;
};

#endif//__GS_TERRAIN_H__

