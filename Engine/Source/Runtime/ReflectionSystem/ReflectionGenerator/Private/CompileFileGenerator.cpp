//// Copyright 2016-2020 ?????????????. All Rights Reserved.
//#include <ReflectionGenerator\Private\CompileFileGenerator.h>
//#include <FileUtility\Path.h>
//
////#include <ReflectionGenerator\Private\MetaEnum.h>
////#include <ReflectionGenerator\Private\MetaObject.h>
////#include <ReflectionGenerator\Private\MetaConstructor.h>
////#include <ReflectionGenerator\Private\MetaFunction.h>
////#include <ReflectionGenerator\Private\MetaProperty.h>
////#include <ReflectionGenerator\ReflectionGeneratorException.h>
////#include <Containers\StringUtility.h>
////#include <Platform\PlatformFile.h>
//
//namespace Engine
//{
//	using namespace FileUtility;
//
//	namespace ReflectionGenerator
//	{
//#ifdef DEBUG_MODE
//#define ADD_NEW_LINE() Content += "\n";
//#else
//#define ADD_NEW_LINE()
//#endif
//
//		using namespace Private;
//
//		void CompileFileGenerator::Generate(const TypeList& Types, const WString& OutputFilePath)
//		{
//			String content = FILE_HEADER;
//
//			content += "#include <";
//			content += Path::ChangeExtension<char8>(OutputFilePath.ChangeType<char8>(), ".h");
//			content += ">\n";
//
//			GenerateCompileCode(Types, content);
//
//			WriteToFile(OutputFilePath, content);
//		}
//
//		void CompileFileGenerator::GenerateCompileCode(const TypeList& Types, String& Content)
//		{
//			String rootContent;
//			String constructorContent;
//			String destructorContent;
//			String functionsDefinition;
//			GenerateDataStructuresDefinition(rootContent, constructorContent, destructorContent, functionsDefinition, Types, AccessSpecifiers::Public);
//
//			//Content += "class " + m_OutputClassName + ";";
//			//ADD_NEW_LINE();
//
//			Content += R"(#include <Reflection\Private\ReflectionAllocators.h>)";
//			ADD_NEW_LINE();
//			Content += R"(#include <Reflection\Private\ImplementDataStructureType.h>)";
//			ADD_NEW_LINE();
//			Content += R"(#include <Reflection\Private\ImplementEnumType.h>)";
//			ADD_NEW_LINE();
//			Content += R"(#include <Reflection\Private\ImplementFunctionType.h>)";
//			ADD_NEW_LINE();
//			Content += R"(#include <Reflection\Private\ImplementPropertyType.h>)";
//			ADD_NEW_LINE();
//			Content += R"(#include <Reflection\Private\RuntimeImplementation.h>)";
//			ADD_NEW_LINE();
//			Content += R"(#include <Allocators\RootAllocator.h>)";
//			ADD_NEW_LINE();
//			Content += R"(#include <Containers\AnyDataType.h>)";
//			ADD_NEW_LINE();
//
//			Content += "using namespace Engine::Common;";
//			ADD_NEW_LINE();
//			Content += "using namespace Engine::Containers;";
//			ADD_NEW_LINE();
//			Content += "using namespace Engine::Reflection;";
//			ADD_NEW_LINE();
//			Content += "using namespace Engine::Reflection::Private;";
//			ADD_NEW_LINE();
//			Content += "using namespace Engine::Allocators;";
//			ADD_NEW_LINE();
//
//			for (auto& t : Types)
//				if (IsTypeOf(t, ObjectType))
//				{
//					MetaObject* type = ReinterpretCast(MetaObject*, t);
//
//					auto& nsStr = type->GetNamespace();
//					if (nsStr != String::Empty)
//					{
//						Content += "using namespace " + nsStr + ";";
//						ADD_NEW_LINE();
//					}
//				}
//
//			//Content += "\nDynamicSizeAllocator allocator(\"Reflection Allocator(" + m_OutputClassName + ")\",RootAllocator::GetInstance(),MegaByte);";
//			//Content += "\nMEMBER_DYNAMIC_ALLOCATOR_HELPERS_DEFINITION(allocator);";
//
//			Content += rootContent;
//
//			Content += "class " + m_OutputClassName + " : private RuntimeImplementation::IMetaObject";
//			ADD_NEW_LINE();
//			Content += "{";
//			ADD_NEW_LINE();
//			Content += "public:";
//			ADD_NEW_LINE();
//			Content += "" + m_OutputClassName + "(void)";
//			ADD_NEW_LINE();
//			Content += "{";
//			ADD_NEW_LINE();
//			Content += "RuntimeImplementation::RegisterMeta(this);";
//			ADD_NEW_LINE();
//			Content += "}";
//			ADD_NEW_LINE();
//			Content += "~" + m_OutputClassName + "(void)";
//			ADD_NEW_LINE();
//			Content += "{";
//			ADD_NEW_LINE();
//			//Content += "RuntimeImplementation::UnregisterMeta(this);";
//			ADD_NEW_LINE();
//			Content += destructorContent;
//			ADD_NEW_LINE();
//
//			Content += "}";
//			ADD_NEW_LINE();
//			Content += "void Initialize(void) override";
//			ADD_NEW_LINE();
//			Content += "{";
//			ADD_NEW_LINE();
//			Content += "ReflectionAllocators::Create();";
//			ADD_NEW_LINE();
//			Content += constructorContent;
//			Content += "}";
//			ADD_NEW_LINE();
//			Content += "};";
//			ADD_NEW_LINE();
//			Content += "static " + m_OutputClassName + " _" + m_OutputClassName + ";";
//			ADD_NEW_LINE();
//
//			Content += functionsDefinition;
//		}
//
//		void CompileFileGenerator::GenerateDataStructuresDefinition(String& RootContent, String& ConstructorContents, String& DestructorContents, String& FunctionsDefinition, const TypeList& Types, AccessSpecifiers Access)
//		{
//			for (auto& t : Types)
//			{
//				ConstructorContents += "\n";
//
//				String ptrName = GetPointerName(t);
//
//				if (IsTypeOf(t, ObjectType))
//				{
//					MetaObject* type = ReinterpretCast(MetaObject*, t);
//
//					String topNestPtrName = (type->GetTopNest() == nullptr ? "nullptr" : GetPointerName(type->GetTopNest())),
//						objectName = type->GetName() + "ObjectImpl";
//
//					RootContent += "\nclass " + objectName + ":public ImplementDataStructureType";
//					RootContent += "\n{";
//					RootContent += "\npublic:";
//					//RootContent += "\n" + objectName + "(void):ImplementDataStructureType(&allocator, " + topNestPtrName + ")";
//					RootContent += "\n" + objectName + "(void):ImplementDataStructureType(ReflectionAllocators::AutoGeneratedAllocator, " + topNestPtrName + ")";
//					RootContent += "\n{";
//
//					RootContent += "\nSetName(\"" + type->GetFullQualifiedName() + "\");";
//					if (type->GetTopNest() != nullptr)
//						RootContent += "\n" + topNestPtrName + "->AddNestedType(this, " + GetAccessText(Access) + ");";
//
//					RootContent += "\n}";
//
//					TypeList ctorTypes;
//					type->GetConstructors(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, ctorTypes);
//					if (ctorTypes.GetSize() == 0)
//					{
//						TypeList types;
//						MetaConstructor metaCtor(type);
//						metaCtor.SetName(type->GetName());
//						types.Add(&metaCtor);
//
//						GenerateConstructorsDefinition(RootContent, types, AccessSpecifiers::Public);
//					}
//					else
//					{
//						ctorTypes.Clear();
//						type->GetConstructors(AccessSpecifiers::Public, ctorTypes);
//						GenerateConstructorsDefinition(RootContent, ctorTypes, AccessSpecifiers::Public);
//
//						ctorTypes.Clear();
//						type->GetConstructors(AccessSpecifiers::Private | AccessSpecifiers::Protected, ctorTypes);
//						GenerateConstructorsDefinition(RootContent, ctorTypes, AccessSpecifiers::Private);
//					}
//
//					RootContent += "\n};";
//
//					RootContent += "\n" + objectName + " *" + ptrName + "=nullptr;";
//					ConstructorContents += "\n" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<" + objectName + ">();";
//					ConstructorContents += "\nConstruct(" + ptrName + "); ";
//
//					if (type->GetTopNest() == nullptr)
//					{
//						DestructorContents += "\nif (" + ptrName + " != nullptr)";
//						DestructorContents += "\nReflectionAllocators::AutoGeneratedAllocator_Deallocate(" + ptrName + ");";
//					}
//
//					StringList parentNames;
//					type->GetParentsName(AccessSpecifiers::Public, parentNames);
//					GenerateParentsNameDefinition(ConstructorContents, t, parentNames, AccessSpecifiers::Public);
//					parentNames.Clear();
//					type->GetParentsName(AccessSpecifiers::Private | AccessSpecifiers::Protected, parentNames);
//					GenerateParentsNameDefinition(ConstructorContents, t, parentNames, AccessSpecifiers::Private);
//
//					TypeList funcTypes;
//					type->GetFunctions(AccessSpecifiers::Public, funcTypes);
//					GenerateFunctionsDefinition(ConstructorContents, funcTypes, AccessSpecifiers::Public);
//					funcTypes.Clear();
//					type->GetFunctions(AccessSpecifiers::Private | AccessSpecifiers::Protected, funcTypes);
//					GenerateFunctionsDefinition(ConstructorContents, funcTypes, AccessSpecifiers::Private);
//
//					TypeList varTypes;
//					type->GetProperties(AccessSpecifiers::Public, varTypes);
//					GenerateVariablesDefinition(ConstructorContents, varTypes, AccessSpecifiers::Public);
//					varTypes.Clear();
//					type->GetProperties(AccessSpecifiers::Private | AccessSpecifiers::Protected, varTypes);
//					GenerateVariablesDefinition(ConstructorContents, varTypes, AccessSpecifiers::Private);
//
//					FunctionsDefinition += "\nconst DataStructureType &" + type->GetFullQualifiedName() + "::GetType(void)";
//					FunctionsDefinition += "\n{";
//					FunctionsDefinition += "\nRuntimeImplementation::InitializeMeta();";
//					FunctionsDefinition += "\nreturn *" + ptrName + ";";
//					FunctionsDefinition += "\n}";
//
//					TypeList nestedTypes;
//					type->GetNestedTypes(AccessSpecifiers::Public, nestedTypes);
//					GenerateDataStructuresDefinition(RootContent, ConstructorContents, DestructorContents, FunctionsDefinition, nestedTypes, AccessSpecifiers::Public);
//					nestedTypes.Clear();
//					type->GetNestedTypes(AccessSpecifiers::Private | AccessSpecifiers::Protected, nestedTypes);
//					GenerateDataStructuresDefinition(RootContent, ConstructorContents, DestructorContents, FunctionsDefinition, nestedTypes, AccessSpecifiers::Private);
//				}
//				if (IsTypeOf(t, MetaEnum))
//				{
//					MetaEnum* type = ReinterpretCast(MetaEnum*, t);
//
//					RootContent += "\nImplementEnumType *" + ptrName + "=nullptr;";
//
//					ConstructorContents += "\n" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<ImplementEnumType>();";
//					ConstructorContents += "\nConstruct(" + ptrName + "); ";
//
//					DestructorContents += "\nif (" + ptrName + " != nullptr)";
//					DestructorContents += "\nReflectionAllocators::AutoGeneratedAllocator_Deallocate(" + ptrName + ");";
//
//					ConstructorContents += "\n" + ptrName + "->SetName(\"" + type->GetFullQualifiedName() + "\");";
//
//					const EnumType::ItemsList& items = type->GetItems();
//
//					String valueName = "value" + type->GetName();
//					ConstructorContents += "\nint32 " + valueName + "=-1;";
//					for (uint32 i = 0; i < items.GetSize(); i++)
//					{
//						StringList parts = items[i].GetName().Split("=");
//
//						if (parts.GetSize() == 1)
//							ConstructorContents += "\n" + valueName + "++; ";
//						else
//							ConstructorContents += "\n" + valueName + "=" + parts[1] + ";";
//
//						ConstructorContents += "\n" + ptrName + "->AddItem(" + valueName + ", \"" + parts[0] + "\");";
//					}
//				}
//
//				ConstructorContents += "\nRuntimeImplementation::RegisterTypeInfo(" + ptrName + ");";
//			}
//		}
//
//		void CompileFileGenerator::GenerateConstructorsDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access)
//		{
//			if (Types.GetSize() == 0)
//				return;
//
//			String topNestName = Types[0]->GetTopNest()->GetFullQualifiedName();
//
//			Content += "\nvoid CreateInstanceInternal(AnyDataType &ReturnValue, const ArgumentsList *Arguments) const";
//			Content += "\n{\n";
//
//			for (auto& t : Types)
//			{
//				MetaConstructor* type = (MetaConstructor*)t;
//
//				Content += "\nReturnValue=new " + topNestName + "(" + GetArgumentsDataTypeText(type->GetParameters()) + ");";
//				//Content += "\n" + topNestName + "* value = ReflectionAllocators::AutoGeneratedAllocator_Allocate<" + topNestName + ">();";
//				//Content += "\nConstruct(value";
//				//String arguments = GetArgumentsDataTypeText(type->GetParameters());
//				//if (arguments != String::Empty)
//				//	Content += "," + arguments;
//				//Content += ");";
//
//				//Content += "\nReturnValue = value;";
//			}
//
//			Content += "\n}";
//		}
//
//		void CompileFileGenerator::GenerateParentsNameDefinition(String& Content, Type* Type, const StringList& ParentsName, AccessSpecifiers Access)
//		{
//			for (auto& t : ParentsName)
//			{
//				Content += "\n" + GetPointerName(Type) + "->AddParentName(\"" + t + "\", " + GetAccessText(Access) + ");";
//			}
//		}
//
//		void CompileFileGenerator::GenerateFunctionsDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access)
//		{
//			for (auto& t : Types)
//			{
//				MetaFunction* type = (MetaFunction*)t;
//
//				String className;
//				type->GetUniqueName(className);
//				className += "Class";
//
//				String ptrName = GetPointerName(type);
//				String topNestPtrName = GetPointerName(type->GetTopNest());
//				String returnDataTypeName = ptrName + "DataType";
//
//				String signature;
//				String signatureName;
//				GetSignature(type, signature, signatureName);
//
//				Content += "\n\nclass " + className + " : public ImplementFunctionType";
//				Content += "\n{";
//				Content += "\nprivate:";
//				Content += "\ntypedef " + signature + ";";
//				Content += "\n" + signatureName + " m_Function;";
//				Content += "\npublic:";
//				Content += "\n" + className + "(Type *TopNest) :";
//				Content += "\nImplementFunctionType(TopNest),";
//				Content += "\nm_Function(&" + type->GetFullQualifiedName() + ")";
//				Content += "\n{";
//				Content += "\nSetName(\"" + type->GetName() + "\");";
//				Content += "\n}";
//				Content += "\nvoid InvokeInternal(void *TargetObject, AnyDataType &ReturnValue, const ArgumentsList *Arguments) const override";
//				Content += "\n{\n";
//
//				if (!(type->GetReturnType().GetValueType() == ValueTypes::VoidPointer && type->GetReturnType().GetPassType() != DataType::PassesTypes::Pointer))
//					Content += "ReturnValue=";
//
//				Content += "(((" + type->GetTopNest()->GetFullQualifiedName() + "*)TargetObject)->*m_Function)(";
//				Content += GetArgumentsDataTypeText(type->GetParameters());
//				Content += "); ";
//				Content += "\n}";
//				Content += "\n};";
//
//				Content += "\n" + className + " *" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<" + className + ">(); ";
//				Content += "\nConstruct(" + ptrName + ", " + topNestPtrName + "); ";
//
//				Content += GetVariableDataTypeText(returnDataTypeName, type->GetReturnType());
//
//				Content += "\n" + ptrName + "->SetReturnType(" + returnDataTypeName + ");";
//
//				Content += "\n" + topNestPtrName + "->AddFunction(" + ptrName + ", " + GetAccessText(Access) + ");";
//			}
//		}
//
//		void CompileFileGenerator::GenerateVariablesDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access)
//		{
//			for (auto& t : Types)
//			{
//				MetaProperty* type = (MetaProperty*)t;
//
//				String ptrName = GetPointerName(type),
//					topNestPtrName = GetPointerName(type->GetTopNest()),
//					dataTypeName = type->GetName() + "DataType";
//
//				Content += "\n\nImplementPropertyType *" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<ImplementPropertyType>(); ";
//				Content += "\nConstruct(" + ptrName + ", " + topNestPtrName + "); ";
//
//				Content += "\n" + ptrName + "->SetName(\"" + type->GetName() + "\");";
//
//				Content += GetVariableDataTypeText(dataTypeName, type->GetDataType());
//
//				Content += "\n" + ptrName + "->SetDataType(" + dataTypeName + ");";
//				Content += "\n" + ptrName + "->SetOffset(OffsetOf(&" + type->GetTopNest()->GetFullQualifiedName() + "::" + type->GetName() + "));";
//				Content += "\n" + topNestPtrName + "->AddProperty(" + ptrName + ", " + GetAccessText(Access) + ");";
//			}
//		}
//
//#undef ADD_NEW_LINE
//	}
//}
