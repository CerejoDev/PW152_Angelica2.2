#pragma once

#include <A3DVector.h>
#include <set>
#include <ABaseDef.h>
#include <ARect.h>
#include <AString.h>
#include <hashmap.h>

typedef std::set<DWORD> LoadedIDSet;
typedef abase::hash_map<DWORD, ARectF> ID2RectMap;

class A3DGrassLand;

class CECGrassLoader
{
public:
	CECGrassLoader(void);
	~CECGrassLoader(void);
public:
	bool			Init(A3DGrassLand* pA3DGrassLand, const char* szDataFolder);	
	bool			Load(const char* szPath);
	void			SetLoadRadius(float fLoadRadius) { m_fLoadRadius = fLoadRadius; }
	bool			SetLoadCenter(const A3DVECTOR3& vCenter);
	bool			Update(DWORD dwTickCnt, const A3DVECTOR3& vUpdatePos);
	void			Release();

protected:
	
	float			calDistance2H(const A3DVECTOR3& vPos, const ARectF& rc);
	bool			loadGrassMap(DWORD dwGrassMapID);
	bool			unloadGrassMap(DWORD dwGrassMapID);
	bool			updateLoadList(const A3DVECTOR3& vPos);
protected:
	A3DVECTOR3		m_vLastUpdatePos;
	A3DGrassLand*	m_pA3DGrassLand;
	float			m_fLoadRadius;
	float			m_fWorldWidth;
	float			m_fWorldHeight;
	float			m_fGrassMapWidth;
	int				m_iGrassMapWidth;
	int				m_iGrassMapRow;
	int				m_iGrassMapCol;
	LoadedIDSet		m_setLoadedID;
	ID2RectMap		m_mapID2Rect;
	AString			m_strDataFolder;
};
