#include "Stdafx.h"
#include "MPropList.h"
using namespace System;


namespace APhysXCommonDNet
{
	bool ListConverter::CanConvertTo(ITypeDescriptorContext^ context, Type^ destinationType)
	{
		if (destinationType->Equals(MPropItem::typeid))
			return true;

		return __super::CanConvertTo(context, destinationType);
	}

	System::Object^ ListConverter::ConvertTo(ITypeDescriptorContext^ context, System::Globalization::CultureInfo^ culture, System::Object^ value, Type^ destinationType)
	{
		if (destinationType->Equals( String::typeid) && Type::Equals(value->GetType(), MPropItem::typeid))				
		{
			MPropItem^ item = (MPropItem^)value;
			return item->Name;
		}
		return __super::ConvertTo(context, culture, value, destinationType);
	}

	MPropItem::MPropItem()
	{
		_name = "";
		_value = nullptr;
		_description = "";
		_category = "";
		_converter = nullptr;
		_editor = nullptr;
		_bReadOnly = false;
	}

	TypeConverter::StandardValuesCollection^ ListConverter::GetStandardValues(ITypeDescriptorContext^ context)
	{
		return gcnew StandardValuesCollection(_listobj);
	}

	CusPropDescriptor::CusPropDescriptor(MPropItem^ property, array<Attribute^>^ attrs)
		: PropertyDescriptor(property->Name, attrs)
	{
		_property = property;
	}
	bool CusPropDescriptor::CanResetValue(Object^ component)
	{
		return false;
	}
	Object^ CusPropDescriptor::GetValue(Object^ component)
	{
		return _property->Value;
	}

	void CusPropDescriptor::SetValue(Object^ component, Object^ value)
	{
		_property->Value = value;
	}
}