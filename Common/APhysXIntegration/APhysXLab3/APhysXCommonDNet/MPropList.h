#pragma once

using namespace System;
using namespace System::ComponentModel;

//implementation of proplist using managed c++, haven't completed yet
namespace APhysXCommonDNet
{
	ref class MPropItem;

	ref class ListConverter : public TypeConverter
	{
		array<System::Object^>^ _listobj;
	public:
		ListConverter(array<System::Object^>^ objs) { _listobj = objs; }

		virtual bool GetStandardValuesSupported(ITypeDescriptorContext^ context) override { return true; }
		virtual bool GetStandardValuesExclusive(ITypeDescriptorContext^ context) override { return true; }
		virtual bool CanConvertTo(ITypeDescriptorContext^ context, Type^ destinationType) override;
		
		virtual System::Object^ ConvertTo(ITypeDescriptorContext^ context, System::Globalization::CultureInfo^ culture, System::Object^ value, Type^ destinationType) override;

		virtual TypeConverter::StandardValuesCollection^ GetStandardValues(ITypeDescriptorContext^ context) override;
		
	};
	public ref class MPropItem
	{		
	private:
		System::String^ _name;
		System::Object^ _value;
		System::String^ _description;
		System::String^ _category;

		TypeConverter^ _converter;
		System::Object^ _editor;
		bool _bReadOnly;

	public:
		MPropItem();
		property bool IsReadOnly
		{
			bool get() { return _bReadOnly; }
			void set(bool value) { _bReadOnly = value; } 
		}
		property System::String^ Name
		{
			System::String^ get() { return _name; }
			void set(System::String^ value) { _name = value; }
		}
		property System::Object^ Value
		{
			System::Object^ get() { return _value; }
			void set(System::Object^ value) { _value = value; }
		}
		property System::String^ Description
		{
			System::String^ get() { return _description; }
			void set(System::String^ value) { _description = value; }
		}
		property System::String^ Category
		{
			System::String^ get() { return _category; }
			void set(System::String^ value) { _category = value; }
		}
		property TypeConverter^ Converter
		{
			TypeConverter^ get() { return _converter; }
			void set(TypeConverter^ value) { _converter = value; }
		}
		property System::Object^ Editor
		{
			System::Object^ get() { return _editor; }
			void set(System::Object^ value) { _editor = value; }
		}
		virtual System::String^ ToString() override
		{
			return gcnew System::String("name: ") + _name + gcnew System::String("; value: ") + _value;
		}

		void SetListString(System::Collections::Generic::List<System::String^>^ str)
		{
			_converter = gcnew ListConverter(str->ToArray());
		}
		void SetListString(array<System::String^>^ str)
		{
			_converter = gcnew ListConverter(str);
		}

		
	};


	//////////////////////////////////////////////////////////////////////////custom property descriptor
	public ref class CusPropDescriptor : public PropertyDescriptor
	{
		MPropItem^ _property; 
	public:
		CusPropDescriptor(MPropItem^ property, array<Attribute^>^ attrs);
		virtual bool CanResetValue(Object^ component) override;
		virtual Object^ GetValue(Object^ component) override;
		property Type^ PropertyType
		{
			virtual Type^ get() override { return _property->Value == nullptr ? String::typeid : _property->Value->GetType(); }
		}
		property bool IsReadOnly
		{
			virtual bool get() override { return _property == nullptr ? false : _property->IsReadOnly; }
		}
		virtual void ResetValue(Object^ component) override {  }

		virtual void SetValue(Object^ component, Object^ value) override;

		virtual bool ShouldSerializeValue(Object^ component) override { return false; }

		property Type^ ComponentType
		{
			virtual Type^ get() override { return GetType(); }
		}
		property String^ Description
		{
			virtual String^ get() override { return _property->Description == "" ? __super::Description : _property->Description; }
		}
		property String^ Category
		{
			virtual String^ get() override { return _property->Category == "" ? __super::Category : _property->Category; }
		}
		property TypeConverter^ Converter
		{
			virtual TypeConverter^ get() override { return _property->Converter; }
		}
		virtual Object^ GetEditor(Type^ editorBaseType) override
		{
			return _property->Editor == nullptr ? __super::GetEditor(editorBaseType) : _property->Editor;
		}
	};
}