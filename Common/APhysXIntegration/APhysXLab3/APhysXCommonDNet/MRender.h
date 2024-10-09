#pragma once

//wrapper for CRender

class CRender;
namespace APhysXCommonDNet
{
	value class Vector3;
	value class MCapsule;

	public ref class MRender
	{
		typedef System::Collections::Generic::List<Vector3> POINTLIST;
		typedef System::Collections::Generic::List<WORD> INDLIST;
	private:
		CRender* _pRender;

	internal:
		CRender* GetRender() { return _pRender; }
		MRender(CRender* pRender);
	
	public:
		bool ResizeDevice(int iWid, int iHei);
		
		//render text
		void RenderText(int x, int y, System::String^ szText, DWORD dwcolor);

		void EnableDrawCameraDir(bool bEnable);

		//render primitive geometric
		void AddFlatRenderData(POINTLIST^ points, INDLIST^ indexs, DWORD dwcolor);

		void AddFlat3DBox(Vector3 vPos, Vector3 vDir, Vector3 vUp, Vector3 vRight, Vector3 vExts, DWORD dwColor);

		void AddFlatAABB(Vector3 vCenter, Vector3 vExts, DWORD dwCol);

		void AddWireRenderData(POINTLIST^ points, INDLIST^ indexs, DWORD dwcolor);

		void AddWire3DBox(Vector3 vPos, Vector3 vDir, Vector3 vUp, Vector3 vRight, Vector3 vExts, DWORD dwColor);

		void AddWireAABB(Vector3 vCenter, Vector3 vExts, DWORD dwCol);

		void AddWire3DLine(Vector3 v1, Vector3 v2, DWORD dwcolor);

		void AddWireSphere(Vector3 vPos, float fRadius, DWORD dwCol);

		void AddWireCapsule(MCapsule cc, DWORD dwCol);

		void AddBloodBar(Vector3 v3DCenter, int halfLen, float perRemain, DWORD dwCol);
	};

}