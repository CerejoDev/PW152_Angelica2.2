#include "StdAfx.h"
#include "ScriptFactory.h"

using namespace System;
using namespace Microsoft::CSharp;
using namespace System::CodeDom::Compiler;
using namespace System::Collections::Generic;
using namespace System::Reflection;
using namespace System::IO;


namespace APhysXCommonDNet
{
		ScriptFactory::ScriptFactory(void)
		{
			_refAssembles = gcnew List<String^>();
			_refAssembles->Add("System.dll");
			String^ str = Assembly::GetAssembly(GetType())->Location;
			_refAssembles->Add(str);
			_refAssembles->Add("System.Drawing.dll");
			_refAssembles->Add("System.Windows.Forms.dll");
			_instance = nullptr;
			_assembly = nullptr;

#ifdef	APXUSERSCRIPTS_DYNAMIC_BUILD

			_dynamicAssembleName = "ApxUserScripts_DynamicBuild.dll";
#else

			_dynamicAssembleName = "ApxUserScripts_VSBuild.dll";

#endif
		}

		array<String^>^ ScriptFactory::CompileTest(array<String^>^ Testfiles, String^% errmsg)
		{
			errmsg = "";

			List<String^>^ successFiles = gcnew List<String^>(Testfiles->Length);

			CSharpCodeProvider^ objCSharpCodePrivoder = gcnew CSharpCodeProvider();
			CompilerParameters^ objCompilerParameters = gcnew CompilerParameters();
			int i;

			for (i = 0; i < _refAssembles->Count; i++ )
			{
				objCompilerParameters->ReferencedAssemblies->Add(_refAssembles[i]);
			}
			objCompilerParameters->GenerateInMemory = false;			
			objCompilerParameters->OutputAssembly = _dynamicAssembleName + a_Random();

			for (i = 0; i < Testfiles->Length; i++)
			{
				successFiles->Add(Testfiles[i]);
			}
			CompilerResults^ cr = objCSharpCodePrivoder->CompileAssemblyFromFile(objCompilerParameters, Testfiles);
			if (cr->Errors->HasErrors)
			{
				for each ( CompilerError^ ce in cr->Errors )
				{
					errmsg = errmsg + "\r\nFileName: " + ce->FileName + "  Line: " +
						Convert::ToString(ce->Line, 10) + " - " + ce->ErrorText;
					for ( i = 0; i < successFiles->Count; i++)
					{
						if (String::Compare(ce->FileName, successFiles[i], true) == 0)
						{
							successFiles->RemoveAt(i);
							break;
						}
					}
					
				}
				
			}			

			File::Delete(objCompilerParameters->OutputAssembly);
			return successFiles->ToArray();
		}

		bool ScriptFactory::Compile(System::String^ strFolder, System::String^% errMsg)
		{

#ifdef APXUSERSCRIPTS_DYNAMIC_BUILD


			errMsg = "";
			//get all files
			array<String^>^ strfiles = Directory::GetFiles(strFolder, "*.cs", SearchOption::AllDirectories);
			bool bExistErr = false;
			if (strfiles->Length == 0)
			{
				File::Delete(_dynamicAssembleName);
				_assembly = nullptr;
				return true;
			}
			else
			{
				//need to rebuild .dll?
				bool bRebuild = false;
				if (!File::Exists(_dynamicAssembleName))
					bRebuild = true;

				DateTime dlltime = File::GetLastWriteTime(_dynamicAssembleName);
				for (int i = 0; !bRebuild && i < strfiles->Length; i++)
				{
					DateTime t = File::GetLastWriteTime(strfiles[i]);
					if (dlltime.CompareTo(t) < 0)
					{
						bRebuild = true;
					}
				}
				
				if (bRebuild)
				{
					if (File::Exists(_dynamicAssembleName))
						File::Delete(_dynamicAssembleName);
					CSharpCodeProvider^ objCSharpCodePrivoder = gcnew CSharpCodeProvider();
					CompilerParameters^ objCompilerParameters = gcnew CompilerParameters();
					for (int i = 0; i < _refAssembles->Count; i++ )
					{
						objCompilerParameters->ReferencedAssemblies->Add(_refAssembles[i]);
					}
					String^ baseDir = gcnew String(af_GetBaseDir());
					objCompilerParameters->GenerateInMemory = true;
					objCompilerParameters->OutputAssembly = baseDir + gcnew String("\\") + _dynamicAssembleName;

					//compile test remove the error files
					strfiles = CompileTest(strfiles, errMsg);
					if (strfiles->Length != 0)
					{
						CompilerResults^ cr = objCSharpCodePrivoder->CompileAssemblyFromFile(objCompilerParameters, strfiles);
						if (cr->Errors->HasErrors)
						{
							return false;
						}
						_assembly = cr->CompiledAssembly;
						if (errMsg != "")
						{
							File::SetLastWriteTime(_dynamicAssembleName, dlltime);
						}
					}
					else
						_assembly = nullptr;
					
				}
				else if (_assembly == nullptr)
				{

					//load from dll
					FileStream^ fs = File::Open(_dynamicAssembleName, FileMode::Open, FileAccess::Read);
					array<unsigned char>^ buf = gcnew array<unsigned char>(int(fs->Length)); 
					fs->Read(buf, 0, (int)fs->Length);
					fs->Close();

					_assembly = AppDomain::CurrentDomain->Load(buf);
				
				}
			}
			return true;

#else
			// in debug mode, load user scripts directly from .dll, so that we can 
			// debug in the user defined scripts...

			_assembly = Assembly::LoadFrom(_dynamicAssembleName);
			return true;
#endif
		}

		Object^ ScriptFactory::CreateInstance(String^ className)
		{
			if (_assembly == nullptr)
				return nullptr;

			Object^ obj = nullptr;
			try
			{
				obj = _assembly->CreateInstance(className);
			}
			catch (Exception^ )
			{
				return nullptr;
			}
			return obj;			
		}

		Object^ ScriptFactory::CreateInstanceFromFile(String^ filename)
		{
			String^ classname = System::IO::Path::GetFileNameWithoutExtension(filename);
			return CreateInstance(classname);
		}

}


