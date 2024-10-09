#ifndef _ECDYNGFXLIGHTMGR_H_
#define _ECDYNGFXLIGHTMGR_H_

#include <A3DTypes.h >
#include <A3DFuncs.h>
#include <vector.h>

class ECDynGfxLightMgr
{
	struct SCENELIGHTBLOCK
	{
		abase::vector<A3DLIGHTPARAM *>	dynamicLights;				// dynamic lights in this block
	};

public:

	ECDynGfxLightMgr();

	~ECDynGfxLightMgr();

	bool Init(float vBlockSize=12.0f, int nBlockDim=10);

	//bool Load(const char * szLightFile);
	bool Release();

	bool Update(DWORD dwDeltaTime, float fDNFactor, const A3DVECTOR3& vecCenter);
	A3DLIGHTPARAM* GetGfxLight(const A3DVECTOR3& vecPos);

protected:
private:
	A3DAABB				m_BlocksAABB;				// bounding box of all blocks.
	float				m_vBlockSize;				// size of each block
	int					m_nBlockDim;				// dimension of blocks.
	A3DVECTOR3			m_vecBlocksCenter;			// center position of all light blocks.
	SCENELIGHTBLOCK *	m_pLightBlocks;				// all light blocks currently in scene

	float				m_fDNFactor;				// day night factor, 0.0 means day, 1.0 mean night
};

#endif