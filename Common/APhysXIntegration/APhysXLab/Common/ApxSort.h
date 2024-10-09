
/************************************************************************

 ApxSort.h

	this file tries to sort objects by some standard

	Created by:		He Wenfeng 
	Date:			Dec. 8, 2009

************************************************************************/

#ifndef		_APX_SORT_
#define		_APX_SORT_
	


#include <vector>
#include <algorithm>
#include <A3DVector.h>

class IDist2Camera
{
public: 

	virtual void UpdateCameraPos(const A3DVECTOR3& vCameraPos) { m_fDist2Camera = (GetMyPos() - vCameraPos).Magnitude(); }
	virtual A3DVECTOR3 GetMyPos() const = 0;
	virtual float GetDist2Camera() const
	{
		return m_fDist2Camera; 
	}

protected:
	
	float m_fDist2Camera;

};

class IPhysXObjBase;
typedef IPhysXObjBase *IPhysXObjBasePtr;


// T is a pointer type...
template<class PtrType>
bool ApxPtrLess(const PtrType& l, const PtrType& r)
{
	return (*l) < (*r);
}


template<class T>
class ApxSorter
{

public:
	
	ApxSorter(){}
	virtual ~ApxSorter() {}


	ApxSorter(T** pTArray, int iNum)
	{
		Init(pTArray, iNum);
	}

	virtual void Init(T** pTArray, int iNum)
	{
		m_TArray.clear();
		
		for(int i=0; i<iNum; i++)
		{
			m_TArray.push_back(pTArray[i]);
		}

	}

	virtual bool Sort()
	{
		std::sort(m_TArray.begin(), m_TArray.end() , ApxPtrLess<T*>);
		
		return true;
	}


	int GetElementNum() const { return m_TArray.size(); }
	T* GetElement(int i) { return m_TArray[i]; }
	const T* GetElement(int i) const { return m_TArray[i]; }

protected:

	std::vector<T*> m_TArray;

};


class ApxModelByDistanceSorter : public ApxSorter<IPhysXObjBase>
{

public:

	ApxModelByDistanceSorter():m_vCameraPos(0.0f)
	{
		m_bCameraPosUpdated = false;
	}


	void UpdateCameraPos(const A3DVECTOR3& vCameraPos);

	virtual bool Sort();


protected:

	A3DVECTOR3 m_vCameraPos;
	bool m_bCameraPosUpdated;

};



#endif
