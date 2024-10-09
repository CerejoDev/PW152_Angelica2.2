#pragma once



//wrapper for region
namespace APhysXCommonDNet
{
	
	public enum class MShapeType
	{
		//according to ShapeType
		ST_BOX		= 1,
		ST_SPHERE	= 2,		
		ST_CAPSULE	= 3,	
	};
	public ref class MRegion : public MIObjBase
	{
	public:
		static unsigned int GetVersion() { return CRegion::GetVersion(); }

		static float GetDefRadius();
		static bool SetDefRadius(float newR);

		static System::String^ GetShapeTypeText(MShapeType st);
		static bool GetShapeType(System::String^ strType, MShapeType% outRT);

		MShapeType GetShapeType();
		void SetShapeType(MShapeType st);

		Vector3 GetVolume(bool withScaled);
		bool SetVolume(Vector3 newVolume, bool withScaled);

	internal:
		MRegion(CRegion* pRegion);
		MRegion(CRegion* pRegion, MIObjBase^ pParent);

		virtual void EnterRegion(unsigned int nbEntities, IObjBase** entities);
		virtual void LeaveRegion(unsigned int nbEntities, IObjBase** entities);

	private:
		CRegion* m_pRegion;
	};

}