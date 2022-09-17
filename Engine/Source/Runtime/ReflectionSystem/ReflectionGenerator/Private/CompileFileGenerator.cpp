// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\Private\CompileFileGenerator.h>
#include <ReflectionGenerator\Private\MetaFunction.h>
#include <ReflectionGenerator\Private\MetaProperty.h>
#include <Reflection\Private\RuntimeImplementation.h>
#include <FileUtility\Path.h>

namespace Engine
{
	using namespace FileUtility;

	namespace ReflectionGenerator
	{
#ifdef DEBUG_MODE
#define ADD_NEW_LINE() Content += "\n";
#else
#define ADD_NEW_LINE()
#endif

		using namespace Private;

		void CompileFileGenerator::Generate(const TypeList& Types, const WString& OutputFilePath)
		{
			String content = FILE_HEADER;

			GenerateHeaderCode(Types, Path::ChangeExtension<char8>(OutputFilePath.ChangeType<char8>(), ".h"), content);
			GenerateForwardDeclarationsCode(Types, content);
			GenerateTypesCode(Types, content);

			WriteToFile(OutputFilePath, content);
		}

		void CompileFileGenerator::GenerateHeaderCode(const TypeList& Types, const String& HeaderFilePath, String& Content)
		{
			ADD_NEW_LINE();

			Content += "#include <";
			Content += HeaderFilePath;
			Content += ">";
			ADD_NEW_LINE();

			Content += R"(#include <Reflection\Private\RuntimeImplementation.h>)";
			ADD_NEW_LINE();

			ADD_NEW_LINE();
		}

		void CompileFileGenerator::GenerateTypesCode(const TypeList& Types, String& Content)
		{
			for (auto& type : Types)
				GenerateTypeCode(type, Content);
		}

		void CompileFileGenerator::GenerateForwardDeclarationsCode(const TypeList& Types, String& Content)
		{
			for (auto& type : Types)
			{
				const String IMPLEMENT_NAME = GetImplementType(type);
				const String IMPLEMENT_POINTER_NAME = GetImplementTypePointerName(type);

				Content += "class " + IMPLEMENT_NAME + ";";
				ADD_NEW_LINE();
				Content += IMPLEMENT_NAME + "* " + IMPLEMENT_POINTER_NAME + " = nullptr;";
				ADD_NEW_LINE();

				ADD_NEW_LINE();

				if (IsTypeOf(type, MetaObject))
				{
					MetaObject* metaObject = ReinterpretCast(MetaObject*, type);

					TypeList nestedTypes;
					metaObject->GetNestedTypes(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, nestedTypes);
					GenerateForwardDeclarationsCode(nestedTypes, Content);

					FunctionTypeList functions;
					metaObject->GetFunctions(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, functions);
					GenerateForwardDeclarationsCode(functions.Cast<Type*>(), Content);

					PropertyTypeList properties;
					metaObject->GetProperties(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, properties);
					GenerateForwardDeclarationsCode(properties.Cast<Type*>(), Content);
				}
			}
		}

		void CompileFileGenerator::GenerateTypeCode(Type* Type, String& Content)
		{
			if (IsTypeOf(Type, MetaObject))
				GenerateObjectCode(ReinterpretCast(MetaObject*, Type), Content);
			else if (IsTypeOf(Type, MetaEnum))
				GenerateEnumCode(ReinterpretCast(MetaEnum*, Type), Content);
			else if (IsTypeOf(Type, MetaFunction))
				GenerateFunctionCode(ReinterpretCast(MetaFunction*, Type), Content);
			else if (IsTypeOf(Type, MetaProperty))
				GeneratePropertyCode(ReinterpretCast(MetaProperty*, Type), Content);
		}

		void CompileFileGenerator::GenerateObjectCode(MetaObject* Type, String& Content)
		{
			const String RUNTIME_IMPLEMENTATION_REGISTER_META(STRINGIZE(Engine::Reflection::Private::RuntimeImplementation::RegisterMeta));
			const String I_META_OBJECT(STRINGIZE(Engine::Reflection::Private::RuntimeImplementation::IMetaObject));
			const String OBJECT_TYPE(STRINGIZE(Engine::Reflection::ObjectType));
			const String IMPLEMENT_OBJECT_TYPE(STRINGIZE(Engine::Reflection::Private::ImplementObjectType));
			const String ARGUMENT_LIST(STRINGIZE(Engine::Reflection::ArgumentsList));
			const String ANY_DATA_TYPE(STRINGIZE(Engine::Containers::AnyDataType));

			const String IMPLEMENT_NAME = GetImplementType(Type);
			const String IMPLEMENT_POINTER_NAME = GetImplementTypePointerName(Type);
			const String REGISTRY_NAME = GetRegistryTypeName(Type);
			const String REGISTRY_POINTER_NAME = GetRegistryTypePointerName(Type);

			{
				Content += "class " + REGISTRY_NAME + " : private " + I_META_OBJECT;
				ADD_NEW_LINE();
				Content += "{";
				ADD_NEW_LINE();

				Content += "private:";
				ADD_NEW_LINE();
				Content += "	" + REGISTRY_NAME + "(void)";
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "		" + RUNTIME_IMPLEMENTATION_REGISTER_META + "(this);";
				ADD_NEW_LINE();

				Content += "	}";
				ADD_NEW_LINE();

				Content += "	virtual ~" + REGISTRY_NAME + "(void)";
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "	}";
				ADD_NEW_LINE();

				Content += "	virtual Initialize(void) override";
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "	}";
				ADD_NEW_LINE();

				Content += "};";
				ADD_NEW_LINE();
				Content += REGISTRY_NAME + " " + REGISTRY_POINTER_NAME + ";";
				ADD_NEW_LINE();
			}

			ADD_NEW_LINE();

			{
				Content += "class " + IMPLEMENT_NAME + " : private " + IMPLEMENT_OBJECT_TYPE;
				ADD_NEW_LINE();
				Content += "{";
				ADD_NEW_LINE();

				Content += "private:";
				ADD_NEW_LINE();
				Content += "	" + IMPLEMENT_NAME + "(void) :";
				ADD_NEW_LINE();
				Content += "		" + IMPLEMENT_OBJECT_TYPE + "(" + GetImplementTypePointerName(Type->GetTopNest()) + ")";
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "		SetName(\"" + Type->GetName() + "\");";
				ADD_NEW_LINE();
				Content += "		SetNamespace(\"" + Type->GetNamespace() + "\");";
				ADD_NEW_LINE();
				Content += "		SetIsStruct(" + StringUtility::ToString<char8>(Type->GetIsStruct()).ToLower() + ");";
				ADD_NEW_LINE();

				auto generateAddParents = [&Content](MetaObject* Type, AccessSpecifiers AccessLevel)
				{
					StringList items;
					Type->GetParentNames(AccessLevel, items);

					for (auto& item : items)
					{
						Content += "		AddParentName(\"" + item + "\", " + GetAccessSpecifier(AccessLevel) + ");";
						ADD_NEW_LINE();
					}
				};

				auto generateAddNesteds = [&Content](MetaObject* Type, AccessSpecifiers AccessLevel)
				{
					TypeList items;
					Type->GetNestedTypes(AccessLevel, items);

					for (auto& item : items)
					{
						Content += "		AddNestedType(" + GetImplementTypePointerName(item) + ", " + GetAccessSpecifier(AccessLevel) + ");";
						ADD_NEW_LINE();
					}
				};

				auto generateAddNFunctions = [&Content](MetaObject* Type, AccessSpecifiers AccessLevel)
				{
					FunctionTypeList items;
					Type->GetFunctions(AccessLevel, items);

					for (auto& item : items)
					{
						Content += "		AddFunction(" + GetImplementTypePointerName(item) + ", " + GetAccessSpecifier(AccessLevel) + ");";
						ADD_NEW_LINE();
					}
				};

				auto generateAddNProperties = [&Content](MetaObject* Type, AccessSpecifiers AccessLevel)
				{
					PropertyTypeList items;
					Type->GetProperties(AccessLevel, items);

					for (auto& item : items)
					{
						Content += "		AddProperty(" + GetImplementTypePointerName(item) + ", " + GetAccessSpecifier(AccessLevel) + ");";
						ADD_NEW_LINE();
					}
				};

				generateAddParents(Type, AccessSpecifiers::Private);
				generateAddParents(Type, AccessSpecifiers::Protected);
				generateAddParents(Type, AccessSpecifiers::Public);

				generateAddNesteds(Type, AccessSpecifiers::Private);
				generateAddNesteds(Type, AccessSpecifiers::Protected);
				generateAddNesteds(Type, AccessSpecifiers::Public);

				generateAddNFunctions(Type, AccessSpecifiers::Private);
				generateAddNFunctions(Type, AccessSpecifiers::Protected);
				generateAddNFunctions(Type, AccessSpecifiers::Public);

				generateAddNProperties(Type, AccessSpecifiers::Private);
				generateAddNProperties(Type, AccessSpecifiers::Protected);
				generateAddNProperties(Type, AccessSpecifiers::Public);

				Content += "	}";
				ADD_NEW_LINE();

				Content += "	void CreateInstanceInternal(const " + ARGUMENT_LIST + "* Arguments, " + ANY_DATA_TYPE + "& ReturnValue) override";
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				//	Content += "\nvoid CreateInstanceInternal(AnyDataType &ReturnValue, const ArgumentsList *Arguments) const";
//	Content += "\n{\n";

//	for (auto& t : Types)
//	{
//		MetaConstructor* type = (MetaConstructor*)t;

//		Content += "\nReturnValue=new " + topNestName + "(" + GetArgumentsDataTypeText(type->GetParameters()) + ");";
//		//Content += "\n" + topNestName + "* value = ReflectionAllocators::AutoGeneratedAllocator_Allocate<" + topNestName + ">();";
//		//Content += "\nConstruct(value";
//		//String arguments = GetArgumentsDataTypeText(type->GetParameters());
//		//if (arguments != String::Empty)
//		//	Content += "," + arguments;
//		//Content += ");";

//		//Content += "\nReturnValue = value;";
//	}

//	Content += "\n}";

				Content += "	}";
				ADD_NEW_LINE();

				Content += "};";
				ADD_NEW_LINE();
			}

			ADD_NEW_LINE();

			{
				Content += "const ";
				Content += OBJECT_TYPE;
				Content += "& " + Type->GetFullQualifiedName() + "::GetType(void)";
				ADD_NEW_LINE();
				Content += "{";
				ADD_NEW_LINE();
				Content += "	RuntimeImplementation::InitializeMeta();";
				ADD_NEW_LINE();
				Content += "	return *" + IMPLEMENT_POINTER_NAME + ";";
				Content += "\n}";
				ADD_NEW_LINE();
			}

			ADD_NEW_LINE();

			TypeList nestedTypes;
			Type->GetNestedTypes(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, nestedTypes);
			GenerateTypesCode(nestedTypes, Content);

			FunctionTypeList functions;
			Type->GetFunctions(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, functions);
			GenerateTypesCode(functions.Cast<Reflection::Type*>(), Content);

			PropertyTypeList properties;
			Type->GetProperties(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, properties);
			GenerateTypesCode(properties.Cast<Reflection::Type*>(), Content);
		}

		void CompileFileGenerator::GenerateEnumCode(MetaEnum* Type, String& Content)
		{
			const String IMPLEMENT_ENUM_TYPE(STRINGIZE(Engine::Reflection::Private::ImplementEnumType));
			const String IMPLEMENT_NAME = GetImplementType(Type);
			const String ITEM_VALUE_VARIABLE_NAME = "itemValue";

			{
				Content += "class " + IMPLEMENT_NAME + " : private " + IMPLEMENT_ENUM_TYPE;
				ADD_NEW_LINE();
				Content += "{";
				ADD_NEW_LINE();

				Content += "private:";
				ADD_NEW_LINE();
				Content += "	" + IMPLEMENT_NAME + "(void) :";
				ADD_NEW_LINE();
				Content += "		" + IMPLEMENT_ENUM_TYPE + "(" + GetImplementTypePointerName(Type->GetTopNest()) + ")";
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "		SetName(\"" + Type->GetName() + "\");";
				ADD_NEW_LINE();
				Content += "		SetNamespace(\"" + Type->GetNamespace() + "\");";
				ADD_NEW_LINE();

				Content += "		" + String(STRINGIZE(int32)) + " " + ITEM_VALUE_VARIABLE_NAME + " = -1;";
				ADD_NEW_LINE();

				for (auto& item : Type->GetItems())
				{
					StringList parts = item.GetName().Split("=");

					if (parts.GetSize() == 1)
						Content += "		++" + ITEM_VALUE_VARIABLE_NAME + ";";
					else
						Content += "		" + ITEM_VALUE_VARIABLE_NAME + " = " + parts[1] + ";";

					ADD_NEW_LINE();

					Content += "		AddItem(\"" + parts[0] + "\", " + ITEM_VALUE_VARIABLE_NAME + ");";
					ADD_NEW_LINE();
				}

				Content += "	}";
				ADD_NEW_LINE();

				Content += "};";
				ADD_NEW_LINE();
			}

			ADD_NEW_LINE();
		}

		void CompileFileGenerator::GenerateFunctionCode(MetaFunction* Type, String& Content)
		{
			const String IMPLEMENT_FUNCTION_TYPE(STRINGIZE(Engine::Reflection::Private::ImplementFunctionType));
			const String IMPLEMENT_PARAMETER_TYPE(STRINGIZE(Engine::Reflection::ImplementParameterType));
			const String IMPLEMENT_NAME = GetImplementType(Type);
			const String IMPLEMENT_POINTER_NAME = GetImplementTypePointerName(Type);
			const String RETURN_DATA_TYPE_VARIALBE_NAME = "returnDataType";
			const String ARGUMENT_LIST(STRINGIZE(Engine::Reflection::ArgumentsList));
			const String ANY_DATA_TYPE(STRINGIZE(Engine::Containers::AnyDataType));

			{
				Content += "class " + IMPLEMENT_NAME + " : private " + IMPLEMENT_FUNCTION_TYPE;
				ADD_NEW_LINE();
				Content += "{";
				ADD_NEW_LINE();

				Content += "private:";
				ADD_NEW_LINE();
				Content += "	" + IMPLEMENT_NAME + "(void) :";
				ADD_NEW_LINE();
				Content += "		" + IMPLEMENT_FUNCTION_TYPE + "(" + GetImplementTypePointerName(Type->GetTopNest()) + ")";
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "		SetName(\"" + Type->GetName() + "\");";
				ADD_NEW_LINE();

				GenerateDataType(Type->GetReturnType(), RETURN_DATA_TYPE_VARIALBE_NAME, Content);
				Content += "		SetReturnType(" + RETURN_DATA_TYPE_VARIALBE_NAME + ");";
				ADD_NEW_LINE();

				uint32 index = 0;
				for (auto& parameter : Type->GetParameters())
				{
					const String PARAMETER_DATA_TYPE_VARIALBE_NAME = "paramDataType" + StringUtility::ToString<char8>(index++);
					const String PARAMETER_VARIALBE_NAME = "param" + StringUtility::ToString<char8>(index++);

					GenerateDataType(parameter.GetDataType(), PARAMETER_DATA_TYPE_VARIALBE_NAME, Content);

					Content += "		" + IMPLEMENT_PARAMETER_TYPE + " " + PARAMETER_VARIALBE_NAME + ";";
					ADD_NEW_LINE();

					Content += "		" + PARAMETER_VARIALBE_NAME + ".SetName(\"" + parameter.GetName() + "\");";
					ADD_NEW_LINE();

					Content += "		" + PARAMETER_VARIALBE_NAME + ".SetDataType(" + PARAMETER_DATA_TYPE_VARIALBE_NAME + ");";
					ADD_NEW_LINE();

					Content += "		AddParameter(" + PARAMETER_VARIALBE_NAME + ");";
					ADD_NEW_LINE();
				}

				Content += "		SetIsConst(\"" + StringUtility::ToString<char8>(Type->GetIsConst()).ToLower() + "\");";
				ADD_NEW_LINE();

				Content += "	}";
				ADD_NEW_LINE();

				Content += "	void InvokeInternal(void* TargetObject, const " + ARGUMENT_LIST + "* Arguments, " + ANY_DATA_TYPE + "& ReturnValue) const override";
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "		THROW_IF_EXCEPTION(Categories::Reflection, TargetObject == nullptr, \"TargetObject cannot be null\");";
				ADD_NEW_LINE();
				Content += "		THROW_IF_EXCEPTION(Categories::Reflection, Arguments == nullptr && m_Parameters.GetSize() != 0, \"Arguments are not compatible with the parameters\");";
				ADD_NEW_LINE();
				Content += "		THROW_IF_EXCEPTION(Categories::Reflection, Arguments->GetSize() != m_Parameters.GetSize(), \"Arguments are not compatible with the parameters\");";
				ADD_NEW_LINE();

				Content += "		";

				if (!(Type->GetReturnType().GetValueType() == ValueTypes::Void && Type->GetReturnType().GetPassType() != DataType::PassesTypes::Pointer))
					Content += "ReturnValue = ";

				Content += "ReinterpretCast(" + Type->GetTopNest()->GetFullQualifiedName() + "*, TargetObject)->" + Type->GetName() + "(";

				GenerateArgumentListCode(Type->GetParameters(), Content);

				Content += ");";
				ADD_NEW_LINE();

				Content += "	}";
				ADD_NEW_LINE();

				Content += "};";
				ADD_NEW_LINE();
			}

			ADD_NEW_LINE();
		}

		void CompileFileGenerator::GenerateArgumentListCode(const ParameterTypeList& Parameters, String& Content)
		{
			uint32 index = 0;
			for (auto& parameter : Parameters)
			{
				if (index != 0)
					Content += ", ";

				const DataType& dataType = parameter.GetDataType();

				if (dataType.GetValueType() == ValueTypes::None)
				{
					if (dataType.GetPassType() == DataType::PassesTypes::Reference)
						Content += "*";

					Content += "ReinterpretCast(" + dataType.GetExtraValueType() + "*, ";
				}

				Content += "(*Arguments)[" + StringUtility::ToString<char8>(index++) + "].GetAs";

				if (dataType.GetValueType() == ValueTypes::None)
					Content += GetValueTypeName(ValueTypes::Void);
				else
					Content += GetValueTypeName(dataType.GetValueType());

				Content += "()";

				if (dataType.GetValueType() == ValueTypes::None)
					Content += ")";
			}
		}

		//String GetArgumentsDataTypeText(const ParameterTypeList& Parameters)
		//{
		//	String ret;

		//	if (Parameters.GetSize() != 0)
		//	{
		//		for (uint8 i = 0; i < Parameters.GetSize(); i++)
		//		{
		//			const Parameter& param = Parameters[i];

		//			ret += GetArgumentDataTypeText(i, param.GetDataType());

		//			if (i < Parameters.GetSize() - 1)
		//				ret += ",";
		//		}
		//	}

		//	return ret;
		//}

		//String GetArgumentDataTypeText(int32 Index, const DataType& Type)
		//{
		//	String ret;

		//	if (Type.GetValueType() == ValueTypes::None)
		//		if (Type.GetPassType() == DataType::PassesTypes::Pointer)
		//			ret += "(" + Type.GetExtraValueType() + "*)";

		//	ret += "(*Arguments)[" + StringUtility::ToString<char8>(Index) + "].GetAs";

		//	if (Type.GetPassType() == DataType::PassesTypes::Pointer)
		//		ret += GetValueTypeText(ValueTypes::VoidPointer);
		//	else
		//		ret += GetValueTypeText(Type.GetValueType(), false);

		//	ret += "()";

		//	return ret;
		//}

		void CompileFileGenerator::GeneratePropertyCode(MetaProperty* Type, String& Content)
		{
			const String IMPLEMENT_PROPERTY_TYPE(STRINGIZE(Engine::Reflection::Private::ImplementPropertyType));
			const String IMPLEMENT_NAME = GetImplementType(Type);
			const String DATA_TYPE_VARIALBE_NAME = "dataType";

			{
				Content += "class " + IMPLEMENT_NAME + " : private " + IMPLEMENT_PROPERTY_TYPE;
				ADD_NEW_LINE();
				Content += "{";
				ADD_NEW_LINE();

				Content += "private:";
				ADD_NEW_LINE();
				Content += "	" + IMPLEMENT_NAME + "(void) :";
				ADD_NEW_LINE();
				Content += "		" + IMPLEMENT_PROPERTY_TYPE + "(" + GetImplementTypePointerName(Type->GetTopNest()) + ")";
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "		SetName(\"" + Type->GetName() + "\");";
				ADD_NEW_LINE();

				GenerateDataType(Type->GetDataType(), DATA_TYPE_VARIALBE_NAME, Content);
				Content += "		SetDataType(" + DATA_TYPE_VARIALBE_NAME + ");";
				ADD_NEW_LINE();

				Content += "		SetOffset(OffsetOf(&" + Type->GetFullQualifiedName() + "));";
				ADD_NEW_LINE();

				Content += "	}";
				ADD_NEW_LINE();

				Content += "};";
				ADD_NEW_LINE();
			}

			ADD_NEW_LINE();
		}

#undef ADD_NEW_LINE

		//void CompileFileGenerator::GenerateCompileCode(const TypeList& Types, String& Content)
		//{
		//	String rootContent;
		//	String constructorContent;
		//	String destructorContent;
		//	String functionsDefinition;
		//	GenerateDataStructuresDefinition(rootContent, constructorContent, destructorContent, functionsDefinition, Types, AccessSpecifiers::Public);

		//	//Content += "class " + m_OutputClassName + ";";
		//	//ADD_NEW_LINE();

		//	//Content += "\nDynamicSizeAllocator allocator(\"Reflection Allocator(" + m_OutputClassName + ")\",RootAllocator::GetInstance(),MegaByte);";
		//	//Content += "\nMEMBER_DYNAMIC_ALLOCATOR_HELPERS_DEFINITION(allocator);";

		//	Content += rootContent;

		//	Content += "class " + m_OutputClassName + " : private RuntimeImplementation::IMetaObject";
		//	ADD_NEW_LINE();
		//	Content += "{";
		//	ADD_NEW_LINE();
		//	Content += "public:";
		//	ADD_NEW_LINE();
		//	Content += "" + m_OutputClassName + "(void)";
		//	ADD_NEW_LINE();
		//	Content += "{";
		//	ADD_NEW_LINE();
		//	Content += "RuntimeImplementation::RegisterMeta(this);";
		//	ADD_NEW_LINE();
		//	Content += "}";
		//	ADD_NEW_LINE();
		//	Content += "~" + m_OutputClassName + "(void)";
		//	ADD_NEW_LINE();
		//	Content += "{";
		//	ADD_NEW_LINE();
		//	//Content += "RuntimeImplementation::UnregisterMeta(this);";
		//	ADD_NEW_LINE();
		//	Content += destructorContent;
		//	ADD_NEW_LINE();

		//	Content += "}";
		//	ADD_NEW_LINE();
		//	Content += "void Initialize(void) override";
		//	ADD_NEW_LINE();
		//	Content += "{";
		//	ADD_NEW_LINE();
		//	Content += "ReflectionAllocators::Create();";
		//	ADD_NEW_LINE();
		//	Content += constructorContent;
		//	Content += "}";
		//	ADD_NEW_LINE();
		//	Content += "};";
		//	ADD_NEW_LINE();
		//	Content += "static " + m_OutputClassName + " _" + m_OutputClassName + ";";
		//	ADD_NEW_LINE();

		//	Content += functionsDefinition;
		//}

		//void CompileFileGenerator::GenerateDataStructuresDefinition(String& RootContent, String& ConstructorContents, String& DestructorContents, String& FunctionsDefinition, const TypeList& Types, AccessSpecifiers Access)
		//{
		//	for (auto& t : Types)
		//	{
		//		ConstructorContents += "\n";

		//		String ptrName = GetPointerName(t);

		//		if (IsTypeOf(t, ObjectType))
		//		{
		//			MetaObject* type = ReinterpretCast(MetaObject*, t);

		//			String topNestPtrName = (type->GetTopNest() == nullptr ? "nullptr" : GetPointerName(type->GetTopNest())),
		//				objectName = type->GetName() + "ObjectImpl";

		//			RootContent += "\nclass " + objectName + ":public ImplementDataStructureType";
		//			RootContent += "\n{";
		//			RootContent += "\npublic:";
		//			//RootContent += "\n" + objectName + "(void):ImplementDataStructureType(&allocator, " + topNestPtrName + ")";
		//			RootContent += "\n" + objectName + "(void):ImplementDataStructureType(ReflectionAllocators::AutoGeneratedAllocator, " + topNestPtrName + ")";
		//			RootContent += "\n{";

		//			RootContent += "\nSetName(\"" + type->GetFullQualifiedName() + "\");";
		//			if (type->GetTopNest() != nullptr)
		//				RootContent += "\n" + topNestPtrName + "->AddNestedType(this, " + GetAccessText(Access) + ");";

		//			RootContent += "\n}";

		//			TypeList ctorTypes;
		//			type->GetConstructors(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, ctorTypes);
		//			if (ctorTypes.GetSize() == 0)
		//			{
		//				TypeList types;
		//				MetaConstructor metaCtor(type);
		//				metaCtor.SetName(type->GetName());
		//				types.Add(&metaCtor);

		//				GenerateConstructorsDefinition(RootContent, types, AccessSpecifiers::Public);
		//			}
		//			else
		//			{
		//				ctorTypes.Clear();
		//				type->GetConstructors(AccessSpecifiers::Public, ctorTypes);
		//				GenerateConstructorsDefinition(RootContent, ctorTypes, AccessSpecifiers::Public);

		//				ctorTypes.Clear();
		//				type->GetConstructors(AccessSpecifiers::Private | AccessSpecifiers::Protected, ctorTypes);
		//				GenerateConstructorsDefinition(RootContent, ctorTypes, AccessSpecifiers::Private);
		//			}

		//			RootContent += "\n};";

		//			RootContent += "\n" + objectName + " *" + ptrName + "=nullptr;";
		//			ConstructorContents += "\n" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<" + objectName + ">();";
		//			ConstructorContents += "\nConstruct(" + ptrName + "); ";

		//			if (type->GetTopNest() == nullptr)
		//			{
		//				DestructorContents += "\nif (" + ptrName + " != nullptr)";
		//				DestructorContents += "\nReflectionAllocators::AutoGeneratedAllocator_Deallocate(" + ptrName + ");";
		//			}

		//			StringList parentNames;
		//			type->GetParentsName(AccessSpecifiers::Public, parentNames);
		//			GenerateParentsNameDefinition(ConstructorContents, t, parentNames, AccessSpecifiers::Public);
		//			parentNames.Clear();
		//			type->GetParentsName(AccessSpecifiers::Private | AccessSpecifiers::Protected, parentNames);
		//			GenerateParentsNameDefinition(ConstructorContents, t, parentNames, AccessSpecifiers::Private);

		//			TypeList funcTypes;
		//			type->GetFunctions(AccessSpecifiers::Public, funcTypes);
		//			GenerateFunctionsDefinition(ConstructorContents, funcTypes, AccessSpecifiers::Public);
		//			funcTypes.Clear();
		//			type->GetFunctions(AccessSpecifiers::Private | AccessSpecifiers::Protected, funcTypes);
		//			GenerateFunctionsDefinition(ConstructorContents, funcTypes, AccessSpecifiers::Private);

		//			TypeList varTypes;
		//			type->GetProperties(AccessSpecifiers::Public, varTypes);
		//			GenerateVariablesDefinition(ConstructorContents, varTypes, AccessSpecifiers::Public);
		//			varTypes.Clear();
		//			type->GetProperties(AccessSpecifiers::Private | AccessSpecifiers::Protected, varTypes);
		//			GenerateVariablesDefinition(ConstructorContents, varTypes, AccessSpecifiers::Private);

		//			FunctionsDefinition += "\nconst DataStructureType &" + type->GetFullQualifiedName() + "::GetType(void)";
		//			FunctionsDefinition += "\n{";
		//			FunctionsDefinition += "\nRuntimeImplementation::InitializeMeta();";
		//			FunctionsDefinition += "\nreturn *" + ptrName + ";";
		//			FunctionsDefinition += "\n}";

		//			TypeList nestedTypes;
		//			type->GetNestedTypes(AccessSpecifiers::Public, nestedTypes);
		//			GenerateDataStructuresDefinition(RootContent, ConstructorContents, DestructorContents, FunctionsDefinition, nestedTypes, AccessSpecifiers::Public);
		//			nestedTypes.Clear();
		//			type->GetNestedTypes(AccessSpecifiers::Private | AccessSpecifiers::Protected, nestedTypes);
		//			GenerateDataStructuresDefinition(RootContent, ConstructorContents, DestructorContents, FunctionsDefinition, nestedTypes, AccessSpecifiers::Private);
		//		}
		//		if (IsTypeOf(t, MetaEnum))
		//		{
		//			MetaEnum* type = ReinterpretCast(MetaEnum*, t);

		//			RootContent += "\nImplementEnumType *" + ptrName + "=nullptr;";

		//			ConstructorContents += "\n" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<ImplementEnumType>();";
		//			ConstructorContents += "\nConstruct(" + ptrName + "); ";

		//			DestructorContents += "\nif (" + ptrName + " != nullptr)";
		//			DestructorContents += "\nReflectionAllocators::AutoGeneratedAllocator_Deallocate(" + ptrName + ");";

		//			ConstructorContents += "\n" + ptrName + "->SetName(\"" + type->GetFullQualifiedName() + "\");";

		//			const EnumType::ItemsList& items = type->GetItems();

		//			String valueName = "value" + type->GetName();
		//			ConstructorContents += "\nint32 " + valueName + "=-1;";
		//			for (uint32 i = 0; i < items.GetSize(); i++)
		//			{
		//				StringList parts = items[i].GetName().Split("=");

		//				if (parts.GetSize() == 1)
		//					ConstructorContents += "\n" + valueName + "++; ";
		//				else
		//					ConstructorContents += "\n" + valueName + "=" + parts[1] + ";";

		//				ConstructorContents += "\n" + ptrName + "->AddItem(" + valueName + ", \"" + parts[0] + "\");";
		//			}
		//		}

		//		ConstructorContents += "\nRuntimeImplementation::RegisterTypeInfo(" + ptrName + ");";
		//	}
		//}

		//void CompileFileGenerator::GenerateConstructorsDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access)
		//{
		//	if (Types.GetSize() == 0)
		//		return;

		//	String topNestName = Types[0]->GetTopNest()->GetFullQualifiedName();

		//	Content += "\nvoid CreateInstanceInternal(AnyDataType &ReturnValue, const ArgumentsList *Arguments) const";
		//	Content += "\n{\n";

		//	for (auto& t : Types)
		//	{
		//		MetaConstructor* type = (MetaConstructor*)t;

		//		Content += "\nReturnValue=new " + topNestName + "(" + GetArgumentsDataTypeText(type->GetParameters()) + ");";
		//		//Content += "\n" + topNestName + "* value = ReflectionAllocators::AutoGeneratedAllocator_Allocate<" + topNestName + ">();";
		//		//Content += "\nConstruct(value";
		//		//String arguments = GetArgumentsDataTypeText(type->GetParameters());
		//		//if (arguments != String::Empty)
		//		//	Content += "," + arguments;
		//		//Content += ");";

		//		//Content += "\nReturnValue = value;";
		//	}

		//	Content += "\n}";
		//}

		//void CompileFileGenerator::GenerateParentsNameDefinition(String& Content, Type* Type, const StringList& ParentsName, AccessSpecifiers Access)
		//{
		//	for (auto& t : ParentsName)
		//	{
		//		Content += "\n" + GetPointerName(Type) + "->AddParentName(\"" + t + "\", " + GetAccessText(Access) + ");";
		//	}
		//}

		//void CompileFileGenerator::GenerateFunctionsDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access)
		//{
		//	for (auto& t : Types)
		//	{
		//		MetaFunction* type = (MetaFunction*)t;

		//		String className;
		//		type->GetUniqueName(className);
		//		className += "Class";

		//		String ptrName = GetPointerName(type);
		//		String topNestPtrName = GetPointerName(type->GetTopNest());
		//		String returnDataTypeName = ptrName + "DataType";

		//		String signature;
		//		String signatureName;
		//		GetSignature(type, signature, signatureName);

		//		Content += "\n\nclass " + className + " : public ImplementFunctionType";
		//		Content += "\n{";
		//		Content += "\nprivate:";
		//		Content += "\ntypedef " + signature + ";";
		//		Content += "\n" + signatureName + " m_Function;";
		//		Content += "\npublic:";
		//		Content += "\n" + className + "(Type *TopNest) :";
		//		Content += "\nImplementFunctionType(TopNest),";
		//		Content += "\nm_Function(&" + type->GetFullQualifiedName() + ")";
		//		Content += "\n{";
		//		Content += "\nSetName(\"" + type->GetName() + "\");";
		//		Content += "\n}";
		//		Content += "\nvoid InvokeInternal(void *TargetObject, AnyDataType &ReturnValue, const ArgumentsList *Arguments) const override";
		//		Content += "\n{\n";

		//		if (!(type->GetReturnType().GetValueType() == ValueTypes::VoidPointer && type->GetReturnType().GetPassType() != DataType::PassesTypes::Pointer))
		//			Content += "ReturnValue=";

		//		Content += "(((" + type->GetTopNest()->GetFullQualifiedName() + "*)TargetObject)->*m_Function)(";
		//		Content += GetArgumentsDataTypeText(type->GetParameters());
		//		Content += "); ";
		//		Content += "\n}";
		//		Content += "\n};";

		//		Content += "\n" + className + " *" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<" + className + ">(); ";
		//		Content += "\nConstruct(" + ptrName + ", " + topNestPtrName + "); ";

		//		Content += GetVariableDataTypeText(returnDataTypeName, type->GetReturnType());

		//		Content += "\n" + ptrName + "->SetReturnType(" + returnDataTypeName + ");";

		//		Content += "\n" + topNestPtrName + "->AddFunction(" + ptrName + ", " + GetAccessText(Access) + ");";
		//	}
		//}

		//void CompileFileGenerator::GenerateVariablesDefinition(String& Content, const TypeList& Types, AccessSpecifiers Access)
		//{
		//	for (auto& t : Types)
		//	{
		//		MetaProperty* type = (MetaProperty*)t;

		//		String ptrName = GetPointerName(type),
		//			topNestPtrName = GetPointerName(type->GetTopNest()),
		//			dataTypeName = type->GetName() + "DataType";

		//		Content += "\n\nImplementPropertyType *" + ptrName + " = ReflectionAllocators::AutoGeneratedAllocator_Allocate<ImplementPropertyType>(); ";
		//		Content += "\nConstruct(" + ptrName + ", " + topNestPtrName + "); ";

		//		Content += "\n" + ptrName + "->SetName(\"" + type->GetName() + "\");";

		//		Content += GetVariableDataTypeText(dataTypeName, type->GetDataType());

		//		Content += "\n" + ptrName + "->SetDataType(" + dataTypeName + ");";
		//		Content += "\n" + ptrName + "->SetOffset(OffsetOf(&" + type->GetTopNest()->GetFullQualifiedName() + "::" + type->GetName() + "));";
		//		Content += "\n" + topNestPtrName + "->AddProperty(" + ptrName + ", " + GetAccessText(Access) + ");";
		//	}
		//}
	}
}
