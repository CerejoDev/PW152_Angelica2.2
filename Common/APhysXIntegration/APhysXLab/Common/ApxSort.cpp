
#include "ApxSort.h"
#include "PhysXObjBase.h"

/*

template<class T>
void ApxSorter<T>::Init(const T* pTArray, int iNum)
{
	m_TArray.clear();

	for(int i=0; i<iNum; i++)
	{
		m_TArray.push_back(pTArray[i]);
	}
}

template<class T>
bool ApxSorter<T>::Sort()
{

	std::sort(m_TArray.begin(), m_TArray.end(), Apx_Less<T>);

	return true;
}



bool ApxModelByDistanceSorter::Less(const IPhysXObjBasePtr& l, const IPhysXObjBasePtr& r)
{

	float dl = (l->GetPos() - m_vCameraPos).Magnitude();
	float dr = (r->GetPos() - m_vCameraPos).Magnitude();


	return dl < dr;

}

*/



bool operator<(const IDist2Camera& l, const IDist2Camera& r)
{
	return l.GetDist2Camera() < r.GetDist2Camera();
}


void ApxModelByDistanceSorter::UpdateCameraPos(const A3DVECTOR3& vCameraPos)
{
	if(vCameraPos == m_vCameraPos)
	{
		m_bCameraPosUpdated = false;
		return;
	}

	m_vCameraPos = vCameraPos;
	m_bCameraPosUpdated = true;

/*	for(int i=0; i<m_TArray.size(); i++)
	{
		m_TArray[i]->UpdateCameraPos(vCameraPos);
	}
*/
	// recover above codes
	assert(true == false);
}

bool ApxPtrLess<IPhysXObjBasePtr>(const IPhysXObjBasePtr& l, const IPhysXObjBasePtr& r)
{
	// the default STL sort is in ascending order, here, we want to render the object
	// from far to near, so we change it to descending order

//	return !((*l) < (*r));
	// recover above codes
	assert(true == false);
	return false;
}


bool ApxModelByDistanceSorter::Sort()
{
	if(m_bCameraPosUpdated)
	{
		return ApxSorter<IPhysXObjBase>::Sort();
	}
	else
		return false;
}
