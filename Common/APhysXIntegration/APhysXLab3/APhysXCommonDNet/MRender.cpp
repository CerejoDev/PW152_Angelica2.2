#include "Stdafx.h"
#include "MCapsule.h"

using namespace System;

namespace APhysXCommonDNet
{
	MRender::MRender(CRender* pRender)
	{
		_pRender = pRender;
	}
	
	bool MRender::ResizeDevice(int iWid, int iHei)
	{
		return _pRender->ResizeDevice(iWid, iHei);
	}

	//render text
	void MRender::RenderText(int x, int y, System::String^ szText, DWORD dwcolor)
	{
		pin_ptr<const wchar_t> wch = PtrToStringChars(szText);
		TString strText = _TWC2TSting(wch);
		_pRender->TextOut(x, y, strText, strText.GetLength(), dwcolor);
	}

	void MRender::EnableDrawCameraDir(bool bEnable)
	{
		_pRender->EnableDrawCameraDir(bEnable);
	}

	//render primitive geometric
	void MRender::AddFlatRenderData(POINTLIST^ points, INDLIST^ indexs, DWORD dwcolor)
	{
		A3DVECTOR3 *pVert = new A3DVECTOR3[points->Count];
		int i;
		for (i = 0; i < points->Count; i++)
		{
			pVert[i] = Vector3::ToA3DVECTOR3(points[i]);
		}
		WORD* pIndex = new WORD[indexs->Count];
		for (i = 0; i < indexs->Count; i++)
		{
			pIndex[i] = indexs[i];
		}
		
		_pRender->GetFlatCollector()->AddRenderData_3D(pVert, points->Count, pIndex, indexs->Count, dwcolor);

		delete[] pVert;
		delete[] pIndex;
	}

	void MRender::AddFlat3DBox(Vector3 vPos, Vector3 vDir, Vector3 vUp, Vector3 vRight, Vector3 vExts, DWORD dwColor)
	{
		_pRender->GetFlatCollector()->AddBox_3D(Vector3::ToA3DVECTOR3(vPos), Vector3::ToA3DVECTOR3(vDir), 
			Vector3::ToA3DVECTOR3(vUp), Vector3::ToA3DVECTOR3(vRight), Vector3::ToA3DVECTOR3(vExts), dwColor);
	}

	void MRender::AddFlatAABB(Vector3 vCenter, Vector3 vExts, DWORD dwCol)
	{
		A3DAABB aabb;
		aabb.Center = Vector3::ToA3DVECTOR3(vCenter);
		aabb.Extents = Vector3::ToA3DVECTOR3(vExts);
		aabb.CompleteMinsMaxs();

		_pRender->GetFlatCollector()->AddAABB_3D(aabb, dwCol);
	}

	void MRender::AddWireRenderData(POINTLIST^ points, INDLIST^ indexs, DWORD dwcolor)
	{
		A3DVECTOR3 *pVert = new A3DVECTOR3[points->Count];
		int i;
		for (i = 0; i < points->Count; i++)
		{
			pVert[i] = Vector3::ToA3DVECTOR3(points[i]);
		}
		WORD* pIndex = new WORD[indexs->Count];
		for (i = 0; i < indexs->Count; i++)
		{
			pIndex[i] = indexs[i];
		}

		_pRender->GetWireCollector()->AddRenderData_3D(pVert, points->Count, pIndex, indexs->Count, dwcolor);

		delete[] pVert;
		delete[] pIndex;
	}

	void MRender::AddWire3DBox(Vector3 vPos, Vector3 vDir, Vector3 vUp, Vector3 vRight, Vector3 vExts, DWORD dwColor)
	{
		_pRender->GetWireCollector()->Add3DBox(Vector3::ToA3DVECTOR3(vPos), Vector3::ToA3DVECTOR3(vDir), Vector3::ToA3DVECTOR3(vUp), 
			Vector3::ToA3DVECTOR3(vRight), Vector3::ToA3DVECTOR3(vExts), dwColor);
	}

	void MRender::AddWireAABB(Vector3 vCenter, Vector3 vExts, DWORD dwCol)
	{
		A3DAABB aabb;
		aabb.Center = Vector3::ToA3DVECTOR3(vCenter);
		aabb.Extents = Vector3::ToA3DVECTOR3(vExts);
		aabb.CompleteMinsMaxs();

		_pRender->GetWireCollector()->AddAABB(aabb, dwCol);
	}

	void MRender::AddWire3DLine(Vector3 v1, Vector3 v2, DWORD dwcolor)
	{
		_pRender->GetWireCollector()->Add3DLine(Vector3::ToA3DVECTOR3(v1), Vector3::ToA3DVECTOR3(v2), dwcolor);
	}

	void MRender::AddWireSphere(Vector3 vPos, float fRadius, DWORD dwCol)
	{
		_pRender->GetWireCollector()->AddSphere(Vector3::ToA3DVECTOR3(vPos), fRadius, dwCol);
	}

	void MRender::AddWireCapsule(MCapsule cc, DWORD dwCol)
	{
		A3DCAPSULE capsule;
		capsule.vCenter = Vector3::ToA3DVECTOR3(cc.GetPos());
		capsule.fHalfLen = (cc.GetHeight() / 2);
		capsule.fRadius = cc.GetRadius();
		capsule.vAxisX = Vector3::ToA3DVECTOR3(cc.GetFront());
		capsule.vAxisY = Vector3::ToA3DVECTOR3(cc.GetUp());
		capsule.vAxisZ = Vector3::ToA3DVECTOR3(cc.GetRight());
		_pRender->GetWireCollector()->AddCapsule(capsule, dwCol);
	}

	void MRender::AddBloodBar(Vector3 v3DCenter, int halfLen, float perRemain, DWORD dwCol)
	{
		A3DVECTOR3 center = Vector3::ToA3DVECTOR3(v3DCenter);
		A3DVECTOR3 dist = _pRender->GetCamera()->GetPos() - center;
		float d = dist.Normalize();
		d /= 30.0f;
		d = 1 - d; 
		int nHL = int(halfLen * d);
		if (6.0 > nHL)
			return;

		float f = DotProduct(dist, _pRender->GetCamera()->GetDir());
		if (0 < f)
			return;

		_pRender->GetViewport()->Transform(center, center);
		int cx = (int)center.x;
		int cy = (int)center.y;
		int l = cx - nHL;
		int r = cx + nHL;
		int b = (int)center.y;
		int t = b - 3;

		A3DWireCollector* pWC = _pRender->GetWireCollector();
		pWC->AddRect_2D(l, t, r, b, dwCol);
		if (1 < perRemain)
			perRemain = 1.0f;
		if (0 > perRemain)
			perRemain = 0.0f;
		int curLen = int(perRemain * 2 * nHL + 0.5f); 
		if (0 < curLen)
			pWC->AddRect_2D(l, t + 1, l + curLen, t + 2, dwCol);
	}
}