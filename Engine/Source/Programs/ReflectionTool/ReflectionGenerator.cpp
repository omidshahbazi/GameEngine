// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionTool\ReflectionGenerator.h>
#include <ReflectionTool\HeaderParser.h>
#include <Containers\StringUtility.h>
#include <ReflectionTool\MetaEnum.h>
#include <ReflectionTool\MetaConstructor.h>
#include <ReflectionTool\MetaFunction.h>
#include <ReflectionTool\MetaProperty.h>
#include <ReflectionTool\ReflectionToolAllocators.h>
#include <FileUtility\Path.h>
#include <Platform\PlatformFile.h>

namespace Engine
{
	using namespace FileUtility;
	using namespace Platform;

	namespace ReflectionTool
	{
		const cstr FILE_HEADER = "// Copyright 2016-2020 ?????????????. All Rights Reserved.\n// This file generated with ?????????????? based on what\n// you wrote in the original file, do not change it manually\n";

		String ReadFromFile(const WString& FilePath)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Input);

			uint64 size = PlatformFile::Size(handle);
			StringStream str(size);
			size = PlatformFile::Read(handle, str.GetBuffer(), size);

			PlatformFile::Close(handle);

			return str.GetBuffer();
		}

		void WriteToFile(const WString& FilePath, const String& Data)
		{
			auto handle = PlatformFile::Open(FilePath.GetValue(), PlatformFile::OpenModes::Output);

			PlatformFile::Write(handle, Data.GetValue());

			PlatformFile::Close(handle);
		}

		bool ReflectionGenerator::Generate(void)
		{
			String content = ReadFromFile(m_FilePath.ChangeType<char16>());
			HeaderParser hp(content);

			TypeList types;
			hp.Parse(types);

			if (types.GetSize() == 0)
				return false;

			String outFileName = Path::GetFileNameWithoutExtension(m_OutputBaseFileName).ChangeType<char8>();
			m_OutputClassName = outFileName.Replace('.', '_') + "Object";

			String headerContent = FILE_HEADER;
			GenerateHeaderFile(headerContent, types);
			WriteToFile(m_OutputBaseFileName + L".h", headerContent);

			String compileContent = FILE_HEADER;
			GenerateCompileFile(compileContent, types);
			WriteToFile(m_OutputBaseFileName + L".cpp", compileContent);

			for (auto& type : types)
				ReflectionToolAllocators::TypesAllocator_Deallocate(type);

			return true;
		}

		void ReflectionGenerator::GenerateHeaderFile(String& HeaderContent, const TypeList& Types)
		{
			//HeaderContent += "\n#include <ReflectionTool\\RTTI.h>\n";

			for (auto& t : Types)
			{
				HeaderContent += "\n";

				if (t->GetType() == Type::Types::DataStructure)
				{
					MetaDataStructure* type = (MetaDataStructure*)t;
					const String macroName = type->GetDeclarationMacroName();

					HeaderContent += "#include <Common\\Definitions.h>\n";
					HeaderContent += "#include <Common\\TypeTraits.h>\n";
					HeaderContent += "#include <Reflection\\DataStructureType.h>\n";
					HeaderContent += "#ifdef " + macroName + "\n";
					HeaderContent += "#undef " + macroName + "\n";
					HeaderContent += "#endif\n";
					HeaderContent += "#define " + macroName + "() \\\n";
					HeaderContent += "friend class " + m_OutputClassName + "; \\\n";
					HeaderContent += "public: \\\n";
					HeaderContent += "static const Engine::Reflection::DataStructureType &GetType(void);";

					TypeList nestedTypes;
					type->GetNestedTypes(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, nestedTypes);
					GenerateHeaderFile(HeaderContent, nestedTypes);
				}
			}
		}

		void ReflectionGenerator::GenerateCompileFile(String& CompileContent, const TypeList& Types)
		{
			String rootContent;
			String constructorContent;
			String destructorContent;
			String functionsDefinition;
			GenerateDataStructuresDefinition(rootContent, constructorContent, destructorContent, functionsDefinition, Types, AccessSpecifiers::Public);

			CompileContent += "\nclass " + m_OutputClassName + ";";

			CompileContent += "\n#include <";
			CompileContent += m_FilePath.ChangeType<char8>();
			CompileContent += ">";
			CompileContent += "\n#include <Reflection\\Private\\ReflectionAllocators.h>";
			CompileContent += "\n#include <Reflection\\Private\\ImplementDataStructureType.h>";
			CompileContent += "\n#include <Reflection\\Private\\ImplementEnumType.h>";
			CompileContent += "\n#include <Reflection\\Private\\ImplementFunctionType.h>";
			CompileContent += "\n#include <Reflection\\Private\\ImplementPropertyType.h>";
			CompileContent += "\n#include <Reflection\\Private\\RuntimeImplementation.h>";
			CompileContent += "\n#include <Allocators\\RootAllocator.h>";
			CompileContent += "\n#include <Containers\\AnyDataType.h>";

			CompileContent += "\nusing namespace Engine::Common;";
			CompileContent += "\nusing namespace Engine::Containers;";
			CompileContent += "\nusing namespace Engine::Reflection;";
			CompileContent += "\nusing namespace Engine::Reflection::Private;";
			CompileContent += "\nusing namespace Engine::Allocators;";

			for (auto& t : Types)
				if (t->GetType() == Type::Types::DataStructure)
				{
					MetaDataStructure* type = (MetaDataStructure*)t;

					auto& nsStr = type->GetNamespace();
					if (nsStr != String::Empty)
						CompileContent += "\nusing namespace " + nsStr + ";";
				}

			//CompileContent += "\nDynamicSizeAllocator allocator(\"Reflection Allocator(" + m_OutputClassName + ")\",RootAllocator::GetInstance(),MegaByte);";
			//CompileContent += "\nMEMBER_DYNAMIC_ALLOCATOR_HELPERS_DEFINITION(allocator);";

			CompileContent += rootContent;

			CompileContent += "\nclass " + m_OutputClassName + ":private RuntimeImplementation::IMetaObject";
			CompileContent += "\n{";
			CompileContent += "\npublic:";
			CompileContent += "\n" + m_OutputClassName + "(void)";
			CompileContent += "\n{";
			CompileContent += "\nRuntimeImplementation::RegisterMeta(this);";
			CompileContent += "\n}";
			CompileContent += "\n~" + m_OutputClassName + "(void)";
			CompileContent += "\n{";
			//CompileContent += "\nRuntimeImplementation::UnregisterMeta(this);";
			CompileContent += destructorContent;

			CompileContent += "\n}";
			CompileContent += "\nvoid Initialize(void) override";
			CompileContent += "\n{";
			CompileContent += "\nReflectionAllocators::Create();";
			CompileContent += constructorContent;
			CompileContent += "\n}";
			CompileContent += "\n};";
			CompileContent += "\nstatic " + m_OutputClassName + " _" + m_OutputClassName + ";";

			CompileContent += functionsDefinition;
		}

		void ReflectionGenerator::GenerateDataStructuresDefinition(String& RootContent, String& ConstructorContents, String& DestructorContents, String& FunctionsDefinition, const TypeList& Types, AccessSpecifiers Access)
		{
			for (auto& t : Types)
			{
				ConstructorContents += "\n";

				String ptrName = GetPointerName(t);

				if (t->GetType() == Type::Types::DataStructure)
				{
					MetaDataStructure* type = (MetaDataStructure*)t;

					String topNestPtrName = (type->GetTopNest() == nullptr ? "nullptr" : GetPointerName(type->GetTopNest())),
						objectName = type->GetName() + "ObjectImpl";

					RootContent += "\nclass " + objectName + ":public ImplementDataStructureType";
					RootContent += "\n{";
					RootContent += "\npublic:";
					//RootContent += "\n" + objectName + "(void):ImplementDataStructureType(&allocator, " + topNestPtrName + ")";
					RootContent += "\n" + objectName + "(void):ImplementDataStructureType(ReflectionAllocators::AutoGeneratedAllocator, " + topNestPtrName + ")";
					RootContent += "\n{";

					RootContent += "\nSetName(\"" + type->GetFullQualifiedName() + "\");";
					if (type->GetTopNest() != nullptr)
						RootContent += "\n" + topNestPtrName + "->AddNestedType(this, " + GetAccessText(Access) + ");";

					RootContent += "\n}";

					TypeList ctorTypes;
					type->GetConstructors(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, ctorTypes);
					if (ctorTypes.GetSize() == 0)
					{
						TypeList types;
						MetaConstructor metaCtor(type);
						metaCtor.SetName(type->GetName());
						types.Add(&metaCtor);

						GenerateConstructorsDefinition(RootContent, types, AccessSpecifiers::Public);
					}
					else
					{
						ctorTypes.Clear();
						type->GetConstructors(AccessSpecifiers::Public, ctorTypes);
						GenerateConstructorsDefinition(RootContent, ctorTypes, AccessSpecifiers::Public);

						ctorTypes.Clear();
						type->GetConstructors(AccessSpecifiers::Private | AccessSpecifiers::Protected, ctorTypes);
						GenerateConstructorsDefinition(RootContent, ctorTypes, AccessSpecifiers::Private);
					}

					RootContent += "\n};";

					RootContent += "\n" + objectName + " *" + ptrName + "=nullptr;";
					ConstructorContents += "\n" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<" + objectName + ">();";
					ConstructorContents += "\nConstruct(" + ptrName + "); ";

					if (type->GetTopNest() == nullptr)
					{
						DestructorContents += "\nif (" + ptrName + " != nullptr)";
						DestructorContents += "\nReflectionAllocators::AutoGeneratedAllocator_Deallocate(" + ptrName + ");";
					}

					StringList parentNames;
					type->GetParentsName(AccessSpecifiers::Public, parentNames);
					GenerateParentsNameDefinition(ConstructorContents, t, parentNames, AccessSpecifiers::Public);
					parentNames.Clear();
					type->GetParentsName(AccessSpecifiers::Private | AccessSpecifiers::Protected, parentNames);
					GenerateParentsNameDefinition(ConstructorContents, t, parentNames, AccessSpecifiers::Private);

					TypeList funcTypes;
					type->GetFunctions(AccessSpecifiers::Public, funcTypes);
					GenerateFunctionsDefinition(ConstructorContents, funcTypes, AccessSpecifiers::Public);
					funcTypes.Clear();
					type->GetFunctions(AccessSpecifiers::Private | AccessSpecifiers::Protected, funcTypes);
					GenerateFunctionsDefinition(ConstructorContents, funcTypes, AccessSpecifiers::Private);

					TypeList varTypes;
					type->GetProperties(AccessSpecifiers::Public, varTypes);
					GenerateVariablesDefinition(ConstructorContents, varTypes, AccessSpecifiers::Public);
					varTypes.Clear();
					type->GetProperties(AccessSpecifiers::Private | AccessSpecifiers::Protected, varTypes);
					GenerateVariablesDefinition(ConstructorContents, varTypes, AccessSpecifiers::Private);

					FunctionsDefinition += "\nconst DataStructureType &" + type->GetFullQualifiedName() + "::GetType(void)";
					FunctionsDefinition += "\n{";
					FunctionsDefinition += "\nRuntimeImplementation::InitializeMeta();";
					FunctionsDefinition += "\nreturn *" + ptrName + ";";
					FunctionsDefinition += "\n}";

					TypeList nestedTypes;
					type->GetNestedTypes(AccessSpecifiers::Public, nestedTypes);
					GenerateDataStructuresDefinition(RootContent, ConstructorContents, DestructorContents, FunctionsDefinition, nestedTypes, AccessSpecifiers::Public);
					nestedTypes.Clear();
					type->GetNestedTypes(AccessSpecifiers::Private | AccessSpecifiers::Protected, nestedTypes);
					GenerateDataStructuresDefinition(RootContent, ConstructorContents, DestructorContents, FunctionsDefinition, nestedTypes, AccessSpecifiers::Private);
				}
				else if (t->GetType() == Type::Types::Enum)
				{
					MetaEnum* type = (MetaEnum*)t;

					RootContent += "\nImplementEnumType *" + ptrName + "=nullptr;";

					ConstructorContents += "\n" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<ImplementEnumType>();";
					ConstructorContents += "\nConstruct(" + ptrName + "); ";

					DestructorContents += "\nif (" + ptrName + " != nullptr)";
					DestructorContents += "\nReflectionAllocators::AutoGeneratedAllocator_Deallocate(" + ptrName + ");";

					ConstructorContents += "\n" + ptrName + "->SetName(\"" + type->GetFullQualifiedName() + "\");";

					const EnumType::ItemsList& items = type->GetItems();

					String valueName = "value" + type->GetName();
					ConstructorContents += "\nint32 " + valueName + "=-1;";
					for (uint32 i = 0; i < items.GetSize(); i++)
					{
						StringList parts = items[i].GetName().Split("=");

						if (parts.GetSize() == 1)
							ConstructorContents += "\n" + valueName + "++; ";
						else
							ConstructorContents += "\n" + valueName + "=" + parts[1] + ";";

						ConstructorContents += "\n" + ptrName + "->AddItem(" + valueName + ", \"" + parts[0] + "\");";
					}
				}

				ConstructorContents += "\nRuntimeImplementation::RegisterTypeInfo(" + ptrName + ");";
			}
		}

		void ReflectionGenerator::GenerateConstructorsDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access)
		{
			if (Types.GetSize() == 0)
				return;

			String topNestName = Types[0]->GetTopNest()->GetFullQualifiedName();

			Content += "\nvoid CreateInstanceInternal(AnyDataType &ReturnValue, const ArgumentsList *Arguments) const";
			Content += "\n{\n";

			for (auto& t : Types)
			{
				MetaConstructor* type = (MetaConstructor*)t;

				Content += "\nReturnValue=new " + topNestName + "(" + GetArgumentsDataTypeText(type->GetParameters()) + ");";
				//Content += "\n" + topNestName + "* value = ReflectionAllocators::AutoGeneratedAllocator_Allocate<" + topNestName + ">();";
				//Content += "\nConstruct(value";
				//String arguments = GetArgumentsDataTypeText(type->GetParameters());
				//if (arguments != String::Empty)
				//	Content += "," + arguments;
				//Content += ");";

				//Content += "\nReturnValue = value;";
			}

			Content += "\n}";
		}

		void ReflectionGenerator::GenerateParentsNameDefinition(String& Content, Type* Type, const StringList& ParentsName, AccessSpecifiers Access)
		{
			for (auto& t : ParentsName)
			{
				Content += "\n" + GetPointerName(Type) + "->AddParentName(\"" + t + "\", " + GetAccessText(Access) + ");";
			}
		}

		void ReflectionGenerator::GenerateFunctionsDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access)
		{
			for (auto& t : Types)
			{
				MetaFunction* type = (MetaFunction*)t;

				String className = type->GetUniqueName() + "Class",
					ptrName = GetPointerName(type),
					topNestPtrName = GetPointerName(type->GetTopNest()),
					returnDataTypeName = ptrName + "DataType";

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
				Content += "\nm_Function(&" + type->GetFullQualifiedName() + ")";
				Content += "\n{";
				Content += "\nSetName(\"" + type->GetName() + "\");";
				Content += "\n}";
				Content += "\nvoid InvokeInternal(void *TargetObject, AnyDataType &ReturnValue, const ArgumentsList *Arguments) const";
				Content += "\n{\n";

				if (!(type->GetReturnType().GetValueType() == ValueTypes::VoidPointer && type->GetReturnType().GetPassType() != DataType::PassesTypes::Pointer))
					Content += "ReturnValue=";

				Content += "(((" + type->GetTopNest()->GetFullQualifiedName() + "*)TargetObject)->*m_Function)(";
				Content += GetArgumentsDataTypeText(type->GetParameters());
				Content += "); ";
				Content += "\n}";
				Content += "\n};";

				Content += "\n" + className + " *" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<" + className + ">(); ";
				Content += "\nConstruct(" + ptrName + ", " + topNestPtrName + "); ";

				Content += GetVariableDataTypeText(returnDataTypeName, type->GetReturnType());

				Content += "\n" + ptrName + "->SetReturnType(" + returnDataTypeName + ");";

				Content += "\n" + topNestPtrName + "->AddFunction(" + ptrName + ", " + GetAccessText(Access) + ");";
			}
		}

		void ReflectionGenerator::GenerateVariablesDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access)
		{
			for (auto& t : Types)
			{
				MetaProperty* type = (MetaProperty*)t;

				String ptrName = GetPointerName(type),
					topNestPtrName = GetPointerName(type->GetTopNest()),
					dataTypeName = type->GetName() + "DataType";

				Content += "\n\nImplementPropertyType *" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<ImplementPropertyType>(); ";
				Content += "\nConstruct(" + ptrName + ", " + topNestPtrName + "); ";

				Content += "\n" + ptrName + "->SetName(\"" + type->GetName() + "\");";

				Content += GetVariableDataTypeText(dataTypeName, type->GetDataType());

				Content += "\n" + ptrName + "->SetDataType(" + dataTypeName + ");";
				Content += "\n" + ptrName + "->SetOffset(OffsetOf(&" + type->GetTopNest()->GetFullQualifiedName() + "::" + type->GetName() + "));";
				Content += "\n" + topNestPtrName + "->AddProperty(" + ptrName + ", " + GetAccessText(Access) + ");";
			}
		}
	}
}
