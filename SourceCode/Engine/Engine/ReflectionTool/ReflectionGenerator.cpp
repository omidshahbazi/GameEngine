// Copyright 2012-2015 ?????????????. All Rights Reserved.
#include <ReflectionTool\ReflectionGenerator.h>
#include <ReflectionTool\HeaderParser.h>
#include <Containers\StringUtility.h>
#include <ReflectionTool\MetaEnum.h>
#include <ReflectionTool\MetaConstructor.h>
#include <ReflectionTool\MetaFunction.h>
#include <ReflectionTool\MetaProperty.h>
#include <Utility\Path.h>
#include <Platform\PlatformFile.h>

namespace Engine
{
	using namespace Utility;
	using namespace Platform;

	namespace ReflectionTool
	{
		const String FILE_HEADER = "// Copyright 2012-2015 ?????????????. All Rights Reserved.\n// This file generated with ?????????????? based on what\n// you wrote in the original file, do not change it manually\n";

		String ReadFromFile(const WString &FilePath)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Input);

			uint64 size = PlatformFile::Size(handle);
			StringStream str(size);
			size = PlatformFile::Read(handle, str.GetBuffer(), size);

			PlatformFile::Close(handle);

			return str.GetBuffer();
		}

		void WriteToFile(const WString &FilePath, const String &Data)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Output);

			PlatformFile::Write(handle, Data.GetValue());

			PlatformFile::Close(handle);
		}

		bool ReflectionGenerator::Generate(void)
		{
			String content = ReadFromFile(m_FilePath.ChangeType<char16>());
			HeaderParser hp(content);

			Type::TypesList types;
			hp.Parse(types);

			if (types.GetSize() == 0)
				return false;

			String outFileName = Path::GetFileNameWithoutExtension(m_OutputBaseFileName);
			m_OutputClassName = outFileName.Replace('.', '_') + "Object";

			String headerContent = FILE_HEADER;
			GenerateHeaderFile(headerContent, types);
			WriteToFile((m_OutputBaseFileName + ".h").ChangeType<char16>(), headerContent);

			String compileContent = FILE_HEADER;
			GenerateCompileFile(compileContent, types);
			WriteToFile((m_OutputBaseFileName + ".cpp").ChangeType<char16>(), compileContent);

			return true;
		}

		void ReflectionGenerator::GenerateHeaderFile(String &HeaderContent, const Type::TypesList &Types)
		{
			//HeaderContent += "\n#include <ReflectionTool\\RTTI.h>\n";

			for each(auto &t in Types)
			{
				HeaderContent += "\n";

				if (t->GetType() == Type::Types::DataStructure)
				{
					MetaDataStructure *type = (MetaDataStructure*)t;
					const String macroName = type->GetDeclarationMacroName();
					const String typeName = type->GetName();

					HeaderContent += "#include <Reflection\\Definitions.h>\n";
					HeaderContent += "#include <Reflection\\DataStructureType.h>\n";
					HeaderContent += "#ifdef " + macroName + "\n";
					HeaderContent += "#undef " + macroName + "\n";
					HeaderContent += "#endif\n";
					HeaderContent += "#define " + macroName + "() \\\n";
					HeaderContent += "friend class " + m_OutputClassName + "; \\\n";
					HeaderContent += "public: \\\n";
					HeaderContent += "static const Engine::Reflection::DataStructureType &GetType(void);";

					GenerateHeaderFile(HeaderContent, type->GetPublicSubTypes());
					GenerateHeaderFile(HeaderContent, type->GetNonPublicSubTypes());
				}
			}
		}

		void ReflectionGenerator::GenerateCompileFile(String &CompileContent, const Type::TypesList &Types)
		{
			String rootContent;
			String content;
			String functionsDefinition;
			GenerateDataStructuresDefinition(rootContent, content, functionsDefinition, Types, AccessSpecifiers::Public);

			CompileContent += "\nclass " + m_OutputClassName + ";";

			CompileContent += "\n#include <";
			CompileContent += m_FilePath;
			CompileContent += ">";
			CompileContent += "\n#include <Reflection\\Private\\ImplementDataStructureType.h>";
			CompileContent += "\n#include <Reflection\\Private\\ImplementEnumType.h>";
			CompileContent += "\n#include <Reflection\\Private\\ImplementFunctionType.h>";
			CompileContent += "\n#include <Reflection\\Private\\ImplementPropertyType.h>";
			CompileContent += "\n#include <Reflection\\Private\\RuntimeImplementation.h>";
			CompileContent += "\n#include <Containers\\AnyDataType.h>";
			CompileContent += "\nusing namespace Engine::Containers;";
			CompileContent += "\nusing namespace Engine::Reflection;";
			CompileContent += "\nusing namespace Engine::Reflection::Private;";

			for each(auto &t in Types)
				if (t->GetType() == Type::Types::DataStructure)
				{
					MetaDataStructure *type = (MetaDataStructure*)t;

					auto &nsStr = type->GetNamespace();
					if (nsStr.GetLength() != 0)
						CompileContent += "\nusing namespace " + nsStr + ";";
				}

			//CompileContent += "\n#include <" + m_FilePath + ">";
			//CompileContent += "\n#include \"" + outFileName + ".h\"";

			CompileContent += rootContent;

			CompileContent += "\nclass " + m_OutputClassName;
			CompileContent += "\n{";
			CompileContent += "\npublic:";
			CompileContent += "\n" + m_OutputClassName + "(void)";
			CompileContent += "\n{";

			CompileContent += content;

			CompileContent += "\n}";
			CompileContent += "\n};";
			CompileContent += "\nstatic " + m_OutputClassName + " _" + m_OutputClassName + ";";

			CompileContent += functionsDefinition;
		}

		void ReflectionGenerator::GenerateDataStructuresDefinition(String &RootContent, String &Content, String &FunctionsDefinition, const Type::TypesList &Types, AccessSpecifiers Access)
		{
			for each(auto &t in Types)
			{
				Content += "\n";

				String ptrName = GetPointerName(t);

				if (t->GetType() == Type::Types::DataStructure)
				{
					MetaDataStructure *type = (MetaDataStructure*)t;

					String topNestPtrName = (type->GetTopNest() == nullptr ? "nullptr" : GetPointerName(type->GetTopNest())),
						objectName = type->GetName() + "ObjectImpl";


					RootContent += "\nclass " + objectName + ":public ImplementDataStructureType";
					RootContent += "\n{";
					RootContent += "\npublic:";
					RootContent += "\n" + objectName + "(void):ImplementDataStructureType(" + topNestPtrName + ")";
					RootContent += "\n{";

					RootContent += "\nSetName(\"" + type->GetName() + "\");";
					if (type->GetTopNest() != nullptr)
						RootContent += "\n" + topNestPtrName + "->AddSubType(this, AccessSpecifiers::" + GetAccessText(Access) + ");";

					RootContent += "\n}";

					GenerateConstructorsDefinition(RootContent, type->GetPublicConstructors(), AccessSpecifiers::Public);
					GenerateConstructorsDefinition(RootContent, type->GetNonPublicConstructors(), AccessSpecifiers::Private);
					RootContent += "\n};";

					RootContent += "\n" + objectName + " *" + ptrName + "=nullptr;";
					Content += "\n" + ptrName + "=new " + objectName + ";"; //"(" + topNestPtrName + ");";

					GenerateFunctionsDefinition(Content, type->GetPublicFunction(), AccessSpecifiers::Public);
					GenerateFunctionsDefinition(Content, type->GetNonPublicFunction(), AccessSpecifiers::Private);

					GenerateVariablesDefinition(Content, type->GetPublicProperties(), AccessSpecifiers::Public);
					GenerateVariablesDefinition(Content, type->GetNonPublicProperties(), AccessSpecifiers::Private);

					FunctionsDefinition += "\nconst DataStructureType &" + type->GetScopedName() + "::GetType(void)";
					FunctionsDefinition += "\n{";
					FunctionsDefinition += "\nreturn *" + ptrName + ";";
					FunctionsDefinition += "\n}";

					GenerateDataStructuresDefinition(RootContent, Content, FunctionsDefinition, type->GetPublicSubTypes(), AccessSpecifiers::Public);
					GenerateDataStructuresDefinition(RootContent, Content, FunctionsDefinition, type->GetNonPublicSubTypes(), AccessSpecifiers::Private);
				}
				else if (t->GetType() == Type::Types::Enum)
				{
					MetaEnum *type = (MetaEnum*)t;

					Content += "\nImplementEnumType *" + ptrName + "=new ImplementEnumType();";
					Content += "\n" + ptrName + "->SetName(\"" + type->GetName() + "\");";

					const EnumType::ItemsList &items = type->GetItems();

					String valueName = "value" + type->GetName();
					Content += "\nint32 " + valueName + "=-1;";
					for (uint32 i = 0; i < items.GetSize(); i++)
					{
						StringList parts = items[i].GetName().Split("=");

						if (parts.GetSize() == 1)
							Content += "\n" + valueName + "++; ";
						else
							Content += "\n" + valueName + "=" + parts[1] + ";";

						Content += "\n" + ptrName + "->AddItem(" + valueName + ", \"" + parts[0] + "\");";
					}
				}

				Content += "\nRuntimeImplementation::RegisterTypeInfo(" + ptrName + ");";
			}
		}

		void ReflectionGenerator::GenerateConstructorsDefinition(String &Content, const Type::TypesList &Types, AccessSpecifiers Access)
		{
			if (Types.GetSize() == 0)
				return;

			String topNestName = Types[0]->GetTopNest()->GetScopedName();

			Content += "\nvoid CreateInstanceInternal(AnyDataType &ReturnValue, const ArgumentsList *Arguments) const";
			Content += "\n{\n";

			for each(auto &t in Types)
			{
				MetaConstructor *type = (MetaConstructor*)t;

				Content += "\nReturnValue=new " + topNestName + "(" + GetArgumentsText(type->GetParameters()) + ");";
			}

			Content += "\n}";
		}

		void ReflectionGenerator::GenerateFunctionsDefinition(String &Content, const Type::TypesList &Types, AccessSpecifiers Access)
		{
			for each(auto &t in Types)
			{
				MetaFunction *type = (MetaFunction*)t;

				String className = type->GetUniqueName() + "Class",
					ptrName = GetPointerName(type),
					topNestPtrName = GetPointerName(type->GetTopNest());

				String signature;
				String signatureName;
				type->GetSignature(signature, signatureName);

				Content += "\n\nclass " + className + " : public ImplementFunctionType";
				Content += "\n{";
				Content += "\nprivate:";
				Content += "\ntypedef " + signature + ";";
				Content += "\n" + signatureName + " m_Function;";
				Content += "\npublic:";
				Content += "\n" + className + "(Type *TopNest) :";
				Content += "\nImplementFunctionType(TopNest),";
				Content += "\nm_Function(&" + type->GetScopedName() + ")";
				Content += "\n{";
				Content += "\nSetName(\"" + type->GetName() + "\");";
				Content += "\n}";
				Content += "\nvoid InvokeInternal(void *TargetObject, AnyDataType &ReturnValue, const ArgumentsList *Arguments) const";
				Content += "\n{\n";

				if (!(type->GetReturnType().GetValueType() == ValueTypes::VoidPointer && type->GetReturnType().GetPassType() != DataType::PassesTypes::Pointer))
					Content += "ReturnValue=";

				Content += "(((" + type->GetTopNest()->GetScopedName() + "*)TargetObject)->*m_Function)(";
				Content += GetArgumentsText(type->GetParameters());
				Content += "); ";
				Content += "\n}";
				Content += "\n};";

				Content += "\n" + className + " *" + ptrName + " = new " + className + "(" + topNestPtrName + "); ";
				//Content += "\n" + ptrName + "->SetReturnType(\"" + type->SetReturnType() + "\");";
				Content += "\n" + topNestPtrName + "->AddFunction(" + ptrName + ", AccessSpecifiers::" + GetAccessText(Access) + ");";
			}
		}

		void ReflectionGenerator::GenerateVariablesDefinition(String &Content, const Type::TypesList &Types, AccessSpecifiers Access)
		{
			for each(auto &t in Types)
			{
				MetaProperty *type = (MetaProperty*)t;

				String ptrName = GetPointerName(type),
					topNestPtrName = GetPointerName(type->GetTopNest());

				Content += "\n\nImplementPropertyType *" + ptrName + " = new ImplementPropertyType(" + topNestPtrName + ");";
				Content += "\n" + ptrName + "->SetName(\"" + type->GetName() + "\");";
				//Content += "\n" + ptrName + "->SetDataType(" + GetValueTypeText(type->GetDataType()) + ");";
				Content += "\n" + ptrName + "->SetOffset((uint32)&((" + type->GetTopNest()->GetScopedName() + "*)nullptr)->" + type->GetName() + ");";
				Content += "\n" + topNestPtrName + "->AddProperty(" + ptrName + ", AccessSpecifiers::" + GetAccessText(Access) + ");";
			}
		}
	}
}
