#ifndef _ECMODELWITHBRUSH_H_
#define _ECMODELWITHBRUSH_H_

#include <abasedef.h>
#include <A3DVector.h>
#include <AString.h>
#include <AArray.h>
#include "aabbcd.h"
#include "ConvexBrush.h"

class CECModel;
class A3DViewport;
class ECBaseBrush;

using namespace CHBasedCD;

class ECModelWithBrush
{
public:
	ECModelWithBrush();
	~ECModelWithBrush();

	bool Tick(DWORD dt);
	bool Render(A3DViewport* pViewport);

	void GenerateBrushInfo();
	
	bool TraceWithBrush(BrushTraceInfo * pInfo);
	
//	bool TraceWithBrush(RayBrushTraceInfo * pInfo);
//	bool TraceWithBrush(CapusuleBrushTraceInfo* pInfo);

	void Release();
	int GetBurshCount() { return m_pBrushes.GetSize();}

	CECModel* GetECModel() { return m_pECModel;}
	void SetECModel(CECModel* pModel) { m_pECModel = pModel;}
	void SetPos(const A3DVECTOR3& vPos) { m_vInitPos = vPos;}
	void SetDir(const A3DVECTOR3& vDir) { m_vInitDir = vDir;}
	void SetUp(const A3DVECTOR3& vUp) { m_vInitUp = vUp;}
	void SetActionName(const AString& str);

private:
	CECModel* m_pECModel;//	Model object
	
//	APtrArray<CCDBrush*>	m_pBrushes;	//	Brush object used in collision
	APtrArray<ECBaseBrush*>	m_pBrushes;	//	Brush object used in collision

	A3DVECTOR3	m_vInitPos;
	A3DVECTOR3	m_vInitDir;
	A3DVECTOR3	m_vInitUp;

	AString		m_strActName;

};
#endif