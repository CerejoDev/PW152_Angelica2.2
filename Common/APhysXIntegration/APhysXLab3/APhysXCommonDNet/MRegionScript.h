#pragma once

#using <System.dll>
#using <mscorlib.dll>


//base class for region script
namespace APhysXCommonDNet
{
	public ref class MRegionScript : public MIScriptBehavior
	{
	public:
		//enter & leave events
		virtual void EnterRegion(array<MIObjBase^>^ objs){ }
		virtual void LeaveRegion(array<MIObjBase^>^ objs){ }

		property MRegion^ Region
		{
			MRegion^ get() { return (MRegion^)ObjBase; }
		}
	};

}