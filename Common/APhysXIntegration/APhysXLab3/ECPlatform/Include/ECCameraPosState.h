#ifndef _ECCAMERAPOSSTATE_H_
#define _ECCAMERAPOSSTATE_H_


///////////////////////////////////////////////////////////////////////////
//	
//	ECCameraPosState indicate where the camera lie.
//	
///////////////////////////////////////////////////////////////////////////
#include <AExpDecl.h >

class ECDungeon;
class A3DVECTOR3;

class _AEXP_DECL ECCameraPosState
{
public:
	enum
	{
		CAMERAPOS_OUTDOORS, // Camera lie in outdoors, or camera is in dungeon but can see the entrance.
		CAMERAPOS_DUNGEONENTRANCE, // in dungeon , but can see the entrance.
		CAMERAPOS_DUNGEON,  // Camera lie in dungeon, and cannot see the entrance of the dungeon.

		CAMERAPOS_NUM
	};
public:
	ECCameraPosState(int iType):m_emState(iType),m_pOwnerDungeon(0){};
	virtual~ECCameraPosState(){}

	int GetStateFlag() { return m_emState;}
	void SetStateFlag(int state){ m_emState = state;}

	void SetCameraRegion(int idx) {m_iOwnerLeafIdx = idx;}
	void SetDungeon(ECDungeon* pDungeon); // for checking the visibility
	bool CheckVisibleByDungeon(const A3DVECTOR3& pos);


protected:
	int m_emState;

	ECDungeon* m_pOwnerDungeon; // camera lie in this dungeon;
	int m_iOwnerLeafIdx; // leaf index containing the camera.
};

#endif