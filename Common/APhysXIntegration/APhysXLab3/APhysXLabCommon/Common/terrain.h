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
	int			nNumRows;		// �ּ���ѽ
	int			nNumCols;		// �ּ���ѽ
	int			nAreaWidth;		// ÿ��Ŀ�ȣ��Ը���㣩
	int			nAreaHeight;	// ÿ��ĸ߶ȣ��Ը���㣩
	float		vGridSize;		// ÿС��ĳߴ�

	float		vHeightMin;		// 0.0 ��Ӧ�ĸ߶�
	float		vHeightMax;		// 1.0 ��Ӧ�ĸ߶�

	char		szMapPath[256];	// ��ͼ������ѽ����Ҫ���һ��б��ѽ

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

