#pragma once



namespace APhysXCommonDNet
{
		public ref class ScriptFactory
		{
		protected:

			System::Collections::Generic::List<System::String^>^ _refAssembles;
			
			static ScriptFactory^ _instance;
			
			//
			System::String^ _dynamicAssembleName;//动态脚本编译出的文件名

			System::Reflection::Assembly^ _assembly;

			ScriptFactory(void);

			//return the files that compiles ok
			array<System::String^>^ CompileTest(array<System::String^>^ Testfiles, System::String^% errmsg);
		public:
			static property ScriptFactory^ Instance
			{
				ScriptFactory^ get() 
				{
					if (_instance == nullptr)
					{
						_instance = gcnew ScriptFactory();
					}
					return _instance;
				}
			}

			//compile all script files in the strFolder, return true 
			bool Compile(System::String^ strFolder, System::String^% errMsg);

			property System::Collections::Generic::List<System::String^>^ RefAssembles
			{
				System::Collections::Generic::List<System::String^>^ get() { return _refAssembles; }
				void set(System::Collections::Generic::List<System::String^>^ value) { _refAssembles = value; }
			}

			property System::String^ DynamicAssembleName
			{
				System::String^ get() { return _dynamicAssembleName; }
				void set(System::String^ value) { _dynamicAssembleName = value; }
			}
						
			//creat instance from class name
			System::Object^ CreateInstance(System::String^ className);

			/// <summary>
			/// create class instance from file
			/// assume that file name is the class name
			/// </summary>
			/// <param name="filename">file name</param>
			/// <param name="conconstructArgs">constructor parameter</param>
			/// <returns>object reference</returns>
			System::Object^ CreateInstanceFromFile(System::String^ filename);
		};
	
}


