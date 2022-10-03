// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\Private\CompileFileGenerator.h>
#include <ReflectionGenerator\Private\MetaFunction.h>
#include <ReflectionGenerator\Private\MetaProperty.h>
#include <ReflectionGenerator\Private\MetaParameter.h>
#include <Reflection\Private\RuntimeImplementation.h>
#include <Reflection\Private\ReflectionAllocators.h>
#include <FileUtility\Path.h>

namespace Engine
{
	using namespace Common;
	using namespace FileUtility;

	namespace ReflectionGenerator
	{
#ifdef DEBUG_MODE
#define ADD_NEW_LINE() Content += "\n";
#else
#define ADD_NEW_LINE()
#endif

		using namespace Private;

		void CompileFileGenerator::Generate(const TypeList& Types, const WString& FilePath, const WString& OutputFilePath)
		{
			String content = FILE_HEADER;

			GenerateHeaderCode(Types, FilePath.ChangeType<char8>(), Path::ChangeExtension<char8>(OutputFilePath.ChangeType<char8>(), ".h"), content);
			GenerateForwardDeclarationsCode(Types, content);
			GenerateTypesCode(Types, content);

			WriteToFile(OutputFilePath, content);
		}

		void CompileFileGenerator::GenerateHeaderCode(const TypeList& Types, const String& FilePath, const String& HeaderFilePath, String& Content)
		{
			const String ENGINE_COMMON(STRINGIZE(Engine::Common));
			const String ENGINE_CONTAINERS(STRINGIZE(Engine::Containers));
			const String ENGINE_ALLOCATORS(STRINGIZE(Engine::Allocators));
			const String ENGINE_REFLECTION(STRINGIZE(Engine::Reflection));
			const String ENGINE_REFLECTION_PRIVATE(STRINGIZE(Engine::Reflection::Private));

			ADD_NEW_LINE();

			Content += "#include <" + FilePath + ">";
			ADD_NEW_LINE();
			Content += "#include <" + HeaderFilePath + ">";
			ADD_NEW_LINE();
			Content += R"(#include <Containers\Exception.h>)";
			ADD_NEW_LINE();
			Content += R"(#include <Reflection\Private\RuntimeImplementation.h>)";
			ADD_NEW_LINE();
			Content += R"(#include <Reflection\Private\ImplementDataType.h>)";
			ADD_NEW_LINE();
			Content += R"(#include <Reflection\Private\ImplementObjectType.h>)";
			ADD_NEW_LINE();
			Content += R"(#include <Reflection\Private\ImplementEnumType.h>)";
			ADD_NEW_LINE();
			Content += R"(#include <Reflection\Private\ImplementFunctionType.h>)";
			ADD_NEW_LINE();
			Content += R"(#include <Reflection\Private\ImplementParameterType.h>)";
			ADD_NEW_LINE();
			Content += R"(#include <Reflection\Private\ImplementPropertyType.h>)";
			ADD_NEW_LINE();
			Content += R"(#include <Reflection\Private\ReflectionAllocators.h>)";
			ADD_NEW_LINE();

			ADD_NEW_LINE();

			Content += "using namespace " + ENGINE_COMMON + ";";
			ADD_NEW_LINE();
			Content += "using namespace " + ENGINE_CONTAINERS + ";";
			ADD_NEW_LINE();
			Content += "using namespace " + ENGINE_ALLOCATORS + ";";
			ADD_NEW_LINE();
			Content += "using namespace " + ENGINE_REFLECTION + ";";
			ADD_NEW_LINE();
			Content += "using namespace " + ENGINE_REFLECTION_PRIVATE + ";";
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
				GenerateNamespaceHeader(type, Content);

				const String IMPLEMENT_NAME = GetImplementType(type);
				const String IMPLEMENT_POINTER_NAME = GetImplementTypePointerName(type);

				Content += "	class " + IMPLEMENT_NAME + ";";
				ADD_NEW_LINE();
				Content += "	" + IMPLEMENT_NAME + "* " + IMPLEMENT_POINTER_NAME + " = nullptr;";
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

				GenerateNamespaceFooter(type, Content);
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

		void CompileFileGenerator::GenerateRegistryCode(Type* Type, String& Content)
		{
			const String I_META_TYPE(STRINGIZE(RuntimeImplementation::IMetaType));
			const String RUNTIME_IMPLEMENTATION_REGISTER_META(STRINGIZE(RuntimeImplementation::RegisterMeta));
			const String RUNTIME_IMPLEMENTATION_REGISTER_TYPE(STRINGIZE(RuntimeImplementation::RegisterType));
			const String RUNTIME_IMPLEMENTATION_UNREGISTER_TYPE(STRINGIZE(RuntimeImplementation::UnregisterType));
			const String REFLECTION_ALLOCATORS_CREATE(STRINGIZE(ReflectionAllocators::Create));
			const String REFLECTION_ALLOCATORS_ALLOCATE(STRINGIZE(ReflectionAllocators::AutoGeneratedAllocator_Allocate));
			const String REFLECTION_ALLOCATORS_DEALLOCATE(STRINGIZE(ReflectionAllocators::AutoGeneratedAllocator_Deallocate));
			const String REGISTRY_NAME = GetRegistryTypeName(Type);
			const String REGISTRY_POINTER_NAME = GetRegistryTypePointerName(Type);
			const String IMPLEMENT_TOP_NEST_POINTER_NAME = GetImplementTypePointerName(Type);

			TypeList types;
			if (IsTypeOf(Type, MetaObject))
			{
				MetaObject* metaObject = ReinterpretCast(MetaObject*, Type);

				FunctionTypeList functions;
				metaObject->GetFunctions(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, functions);
				types.AddRange(functions.Cast<Reflection::Type*>());

				PropertyTypeList properties;
				metaObject->GetProperties(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, properties);
				types.AddRange(properties.Cast<Reflection::Type*>());
			}
			types.Add(Type);

			Content += "	class " + REGISTRY_NAME + " : public " + I_META_TYPE;
			ADD_NEW_LINE();
			Content += "	{";
			ADD_NEW_LINE();

			Content += "	public:";
			ADD_NEW_LINE();
			Content += "		" + REGISTRY_NAME + "(void)";
			ADD_NEW_LINE();
			Content += "		{";
			ADD_NEW_LINE();

			Content += "			" + RUNTIME_IMPLEMENTATION_REGISTER_META + "(this);";
			ADD_NEW_LINE();

			Content += "		}";
			ADD_NEW_LINE();

			Content += "		virtual ~" + REGISTRY_NAME + "(void)";
			ADD_NEW_LINE();
			Content += "		{";
			ADD_NEW_LINE();

			for (auto& type : types)
			{
				const String IMPLEMENT_POINTER_NAME = GetImplementTypePointerName(type);

				Content += "			" + REFLECTION_ALLOCATORS_DEALLOCATE + "(" + IMPLEMENT_POINTER_NAME + ");";
				ADD_NEW_LINE();
			}

			Content += "			" + RUNTIME_IMPLEMENTATION_UNREGISTER_TYPE + "(" + IMPLEMENT_TOP_NEST_POINTER_NAME + ");";
			ADD_NEW_LINE();

			Content += "		}";
			ADD_NEW_LINE();

			Content += "		virtual void Initialize(void) override";
			ADD_NEW_LINE();
			Content += "		{";
			ADD_NEW_LINE();

			Content += "			" + REFLECTION_ALLOCATORS_CREATE + "();";
			ADD_NEW_LINE();

			for (auto& type : types)
			{
				const String IMPLEMENT_NAME = GetImplementType(type);
				const String IMPLEMENT_POINTER_NAME = GetImplementTypePointerName(type);

				Content += "			" + IMPLEMENT_POINTER_NAME + " = " + REFLECTION_ALLOCATORS_ALLOCATE + "<" + IMPLEMENT_NAME + ">();";
				ADD_NEW_LINE();
			}

			for (auto& type : types)
			{
				const String IMPLEMENT_POINTER_NAME = GetImplementTypePointerName(type);

				Content += "			Construct(" + IMPLEMENT_POINTER_NAME + ");";
				ADD_NEW_LINE();
			}

			Content += "			" + RUNTIME_IMPLEMENTATION_REGISTER_TYPE + "(" + IMPLEMENT_TOP_NEST_POINTER_NAME + ");";
			ADD_NEW_LINE();

			Content += "		}";
			ADD_NEW_LINE();

			Content += "	};";
			ADD_NEW_LINE();
			Content += "	" + REGISTRY_NAME + " " + REGISTRY_POINTER_NAME + ";";
			ADD_NEW_LINE();

			ADD_NEW_LINE();
		}

		void CompileFileGenerator::GenerateObjectCode(MetaObject* Type, String& Content)
		{
			const String RUNTIME_IMPLEMENTATION_INITIALIZE_META(STRINGIZE(RuntimeImplementation::InitializeMeta));
			const String OBJECT_TYPE(STRINGIZE(ObjectType));
			const String IMPLEMENT_OBJECT_TYPE(STRINGIZE(ImplementObjectType));
			const String AUTO_GENERATED_ALLOCATOR(STRINGIZE(ReflectionAllocators::AutoGeneratedAllocator));
			const String REFLECTION_ALLOCATORS_ALLOCATE(STRINGIZE(ReflectionAllocators::AutoGeneratedAllocator_Allocate));
			const String ARGUMENT_LIST(STRINGIZE(ArgumentsList));
			const String ANY_DATA_TYPE(STRINGIZE(AnyDataType));
			const String IMPLEMENT_NAME = GetImplementType(Type);
			const String IMPLEMENT_POINTER_NAME = GetImplementTypePointerName(Type);
			const String INSTANCE_NAME = "ptr";

			GenerateNamespaceHeader(Type, Content);
			{
				{
					Content += "	class " + IMPLEMENT_NAME + " : public " + IMPLEMENT_OBJECT_TYPE;
					ADD_NEW_LINE();
					Content += "	{";
					ADD_NEW_LINE();

					Content += "	public:";
					ADD_NEW_LINE();
					Content += "		" + IMPLEMENT_NAME + "(void) :";
					ADD_NEW_LINE();
					Content += "			" + IMPLEMENT_OBJECT_TYPE + "(" + AUTO_GENERATED_ALLOCATOR + ", " + GetImplementTypePointerName(Type->GetTopNest()) + ")";
					ADD_NEW_LINE();
					Content += "		{";
					ADD_NEW_LINE();

					Content += "			SetName(\"" + Type->GetName() + "\");";
					ADD_NEW_LINE();
					Content += "			SetNamespace(\"" + Type->GetNamespace() + "\");";
					ADD_NEW_LINE();
					Content += "			SetIsStruct(" + StringUtility::ToString<char8>(Type->GetIsStruct()).ToLower() + ");";
					ADD_NEW_LINE();
					Content += "			SetIsAbstract(" + StringUtility::ToString<char8>(Type->GetIsAbstract()).ToLower() + ");";
					ADD_NEW_LINE();
					Content += "			SetIsStatic(" + StringUtility::ToString<char8>(Type->GetIsStatic()).ToLower() + ");";
					ADD_NEW_LINE();

					auto generateAddParents = [&Content](MetaObject* Type, AccessSpecifiers AccessLevel)
					{
						StringList items;
						Type->GetParentNames(AccessLevel, items);

						for (auto& item : items)
						{
							Content += "			AddParentName(\"" + item + "\", " + GetAccessSpecifier(AccessLevel) + ");";
							ADD_NEW_LINE();
						}
					};

					auto generateAddNesteds = [&Content, &IMPLEMENT_OBJECT_TYPE](MetaObject* Type, AccessSpecifiers AccessLevel)
					{
						TypeList items;
						Type->GetNestedTypes(AccessLevel, items);

						for (auto& item : items)
						{
							Content += "			AddNestedType(ReinterpretCast(" + IMPLEMENT_OBJECT_TYPE + "*, " + GetImplementTypePointerName(item) + "), " + GetAccessSpecifier(AccessLevel) + ");";
							ADD_NEW_LINE();
						}
					};

					auto generateAddNFunctions = [&Content](MetaObject* Type, AccessSpecifiers AccessLevel)
					{
						const String IMPLEMENT_FUNCTION_TYPE(STRINGIZE(ImplementFunctionType));

						FunctionTypeList items;
						Type->GetFunctions(AccessLevel, items);

						for (auto& item : items)
						{
							Content += "			AddFunction(ReinterpretCast(" + IMPLEMENT_FUNCTION_TYPE + "*, " + GetImplementTypePointerName(item) + "), " + GetAccessSpecifier(AccessLevel) + ");";
							ADD_NEW_LINE();
						}
					};

					auto generateAddNProperties = [&Content](MetaObject* Type, AccessSpecifiers AccessLevel)
					{
						const String IMPLEMENT_PROPERTY_TYPE(STRINGIZE(ImplementPropertyType));

						PropertyTypeList items;
						Type->GetProperties(AccessLevel, items);

						for (auto& item : items)
						{
							Content += "			AddProperty(ReinterpretCast(" + IMPLEMENT_PROPERTY_TYPE + "*, " + GetImplementTypePointerName(item) + "), " + GetAccessSpecifier(AccessLevel) + ");";
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

					Content += "		}";
					ADD_NEW_LINE();

					Content += "		void CreateInstanceInternal(const " + ARGUMENT_LIST + "* Arguments, " + ANY_DATA_TYPE + "& ReturnValue) const override";
					ADD_NEW_LINE();
					Content += "		{";
					ADD_NEW_LINE();

					if (Type->GetIsStatic() || Type->GetIsAbstract())
					{
						Content += "			THROW_EXCEPTION(Categories::Reflection, \"Creating an instance of an static/abstract class is not applicable\");";
						ADD_NEW_LINE();
					}
					else
					{
						FunctionTypeList ctors;
						Type->GetConstructors(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, ctors);

						Content += "			" + Type->GetFullQualifiedName() + "* " + INSTANCE_NAME + " = " + REFLECTION_ALLOCATORS_ALLOCATE + "<" + Type->GetFullQualifiedName() + ">();";
						ADD_NEW_LINE();
						Content += "			ReturnValue = " + INSTANCE_NAME + ";";
						ADD_NEW_LINE();

						if (ctors.GetSize() == 0)
						{
							Content += "			ConstructMacro(" + Type->GetFullQualifiedName() + ", " + INSTANCE_NAME + ");";
							ADD_NEW_LINE();
						}
						else
						{
							Content += "			switch (Arguments->GetSize())";
							ADD_NEW_LINE();
							Content += "			{";
							ADD_NEW_LINE();

							for (auto& ctor : ctors)
							{
								Content += "			case " + StringUtility::ToString<char8>(ctor->GetParameters().GetSize()) + ":";
								ADD_NEW_LINE();
								{
									Content += "				ConstructMacro(" + Type->GetFullQualifiedName() + ", " + INSTANCE_NAME;

									if (ctor->GetParameters().GetSize() != 0)
									{
										Content += ", ";

										GenerateArgumentListCode(ctor->GetParameters(), Content);
									}

									Content += ");";

									ADD_NEW_LINE();
								}
								Content += "				break;";
								ADD_NEW_LINE();
							}

							Content += "			default:";
							ADD_NEW_LINE();
							{
								Content += "				THROW_EXCEPTION(Categories::Reflection, \"No matched constructor has found\");";
								ADD_NEW_LINE();
							}
							Content += "				break;";
							ADD_NEW_LINE();

							Content += "			}";
							ADD_NEW_LINE();
						}
					}

					Content += "		}";
					ADD_NEW_LINE();

					Content += "	};";
					ADD_NEW_LINE();
				}

				ADD_NEW_LINE();

				{
					Content += "	const ";
					Content += OBJECT_TYPE;
					Content += "& " + Type->GetFullQualifiedName() + "::GetType(void)";
					ADD_NEW_LINE();
					Content += "	{";
					ADD_NEW_LINE();
					Content += "		" + RUNTIME_IMPLEMENTATION_INITIALIZE_META + "();";
					ADD_NEW_LINE();
					Content += "		return *" + IMPLEMENT_POINTER_NAME + ";";
					ADD_NEW_LINE();
					Content += "	}";
					ADD_NEW_LINE();

					ADD_NEW_LINE();
				}

				TypeList nestedTypes;
				Type->GetNestedTypes(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, nestedTypes);
				GenerateTypesCode(nestedTypes, Content);

				FunctionTypeList functions;
				Type->GetFunctions(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, functions);
				GenerateTypesCode(functions.Cast<Reflection::Type*>(), Content);

				PropertyTypeList properties;
				Type->GetProperties(AccessSpecifiers::Private | AccessSpecifiers::Protected | AccessSpecifiers::Public, properties);
				GenerateTypesCode(properties.Cast<Reflection::Type*>(), Content);

				GenerateRegistryCode(Type, Content);
			}
			GenerateNamespaceFooter(Type, Content);
		}

		void CompileFileGenerator::GenerateEnumCode(MetaEnum* Type, String& Content)
		{
			const String IMPLEMENT_ENUM_TYPE(STRINGIZE(ImplementEnumType));
			const String INT32 = STRINGIZE(int32);
			const String IMPLEMENT_NAME = GetImplementType(Type);
			const String ITEM_VALUE_VARIABLE_NAME = "itemValue";

			GenerateNamespaceHeader(Type, Content);
			{
				Content += "	class " + IMPLEMENT_NAME + " : public " + IMPLEMENT_ENUM_TYPE;
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "	public:";
				ADD_NEW_LINE();
				Content += "		" + IMPLEMENT_NAME + "(void) :";
				ADD_NEW_LINE();
				Content += "			" + IMPLEMENT_ENUM_TYPE + "(" + GetImplementTypePointerName(Type->GetTopNest()) + ")";
				ADD_NEW_LINE();
				Content += "		{";
				ADD_NEW_LINE();

				Content += "			SetName(\"" + Type->GetName() + "\");";
				ADD_NEW_LINE();
				Content += "			SetNamespace(\"" + Type->GetNamespace() + "\");";
				ADD_NEW_LINE();

				Content += "			" + INT32 + " " + ITEM_VALUE_VARIABLE_NAME + " = -1;";
				ADD_NEW_LINE();

				for (auto& item : Type->GetItems())
				{
					StringList parts = item.GetName().Split("=");

					if (parts.GetSize() == 1)
						Content += "			++" + ITEM_VALUE_VARIABLE_NAME + ";";
					else
						Content += "			" + ITEM_VALUE_VARIABLE_NAME + " = " + parts[1] + ";";

					ADD_NEW_LINE();

					Content += "			AddItem(\"" + parts[0] + "\", " + ITEM_VALUE_VARIABLE_NAME + ");";
					ADD_NEW_LINE();
				}

				Content += "		}";
				ADD_NEW_LINE();

				Content += "	};";
				ADD_NEW_LINE();

				ADD_NEW_LINE();

				GenerateRegistryCode(Type, Content);
			}
			GenerateNamespaceFooter(Type, Content);
		}

		void CompileFileGenerator::GenerateFunctionCode(MetaFunction* Type, String& Content)
		{
			const String IMPLEMENT_FUNCTION_TYPE(STRINGIZE(ImplementFunctionType));
			const String IMPLEMENT_PARAMETER_TYPE(STRINGIZE(ImplementParameterType));
			const String IMPLEMENT_NAME = GetImplementType(Type);
			const String IMPLEMENT_POINTER_NAME = GetImplementTypePointerName(Type);
			const String RETURN_DATA_TYPE_VARIALBE_NAME = "returnDataType";
			const String ARGUMENT_LIST(STRINGIZE(ArgumentsList));
			const String ANY_DATA_TYPE(STRINGIZE(AnyDataType));

			{
				Content += "	class " + IMPLEMENT_NAME + " : public " + IMPLEMENT_FUNCTION_TYPE;
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "	public:";
				ADD_NEW_LINE();
				Content += "		" + IMPLEMENT_NAME + "(void) :";
				ADD_NEW_LINE();
				Content += "			" + IMPLEMENT_FUNCTION_TYPE + "(" + GetImplementTypePointerName(Type->GetTopNest()) + ")";
				ADD_NEW_LINE();
				Content += "		{";
				ADD_NEW_LINE();

				Content += "			SetName(\"" + Type->GetName() + "\");";
				ADD_NEW_LINE();

				GenerateDataType(Type->GetReturnType(), RETURN_DATA_TYPE_VARIALBE_NAME, Content);
				Content += "			SetReturnType(" + RETURN_DATA_TYPE_VARIALBE_NAME + ");";
				ADD_NEW_LINE();

				uint32 index = 0;
				for (auto& parameter : Type->GetParameters())
				{
					const String PARAMETER_DATA_TYPE_VARIALBE_NAME = "paramDataType" + StringUtility::ToString<char8>(index++);
					const String PARAMETER_VARIALBE_NAME = "param" + StringUtility::ToString<char8>(index++);

					GenerateDataType(parameter.GetDataType(), PARAMETER_DATA_TYPE_VARIALBE_NAME, Content);

					Content += "			" + IMPLEMENT_PARAMETER_TYPE + " " + PARAMETER_VARIALBE_NAME + ";";
					ADD_NEW_LINE();

					Content += "			" + PARAMETER_VARIALBE_NAME + ".SetName(\"" + parameter.GetName() + "\");";
					ADD_NEW_LINE();

					Content += "			" + PARAMETER_VARIALBE_NAME + ".SetDataType(" + PARAMETER_DATA_TYPE_VARIALBE_NAME + ");";
					ADD_NEW_LINE();

					Content += "			AddParameter(" + PARAMETER_VARIALBE_NAME + ");";
					ADD_NEW_LINE();
				}

				Content += "			SetIsConst(" + StringUtility::ToString<char8>(Type->GetIsConst()).ToLower() + ");";
				ADD_NEW_LINE();

				Content += "			SetIsStatic(" + StringUtility::ToString<char8>(Type->GetIsStatic()).ToLower() + ");";
				ADD_NEW_LINE();

				Content += "		}";
				ADD_NEW_LINE();

				Content += "		void InvokeInternal(void* TargetObject, const " + ARGUMENT_LIST + "* Arguments, " + ANY_DATA_TYPE + "& ReturnValue) const override";
				ADD_NEW_LINE();
				Content += "		{";
				ADD_NEW_LINE();

				if (Type->GetIsStatic())
					Content += "			THROW_IF_EXCEPTION(Categories::Reflection, TargetObject != nullptr, \"TargetObject must be null\");";
				else
					Content += "			THROW_IF_EXCEPTION(Categories::Reflection, TargetObject == nullptr, \"TargetObject cannot be null\");";
				ADD_NEW_LINE();

				Content += "			THROW_IF_EXCEPTION(Categories::Reflection, Arguments == nullptr && m_Parameters.GetSize() != 0, \"Arguments are not compatible with the parameters\");";
				ADD_NEW_LINE();
				Content += "			THROW_IF_EXCEPTION(Categories::Reflection, Arguments->GetSize() != m_Parameters.GetSize(), \"Arguments are not compatible with the parameters\");";
				ADD_NEW_LINE();

				Content += "			";

				if (!(Type->GetReturnType().GetValueType() == ValueTypes::Void && Type->GetReturnType().GetPassType() != DataType::PassesTypes::Pointer))
					Content += "ReturnValue = ";

				if (Type->GetIsStatic())
					Content += Type->GetTopNest()->GetFullQualifiedName() + "::";
				else
					Content += "ReinterpretCast(" + Type->GetTopNest()->GetFullQualifiedName() + "*, TargetObject)->";

				Content += Type->GetName() + "(";

				GenerateArgumentListCode(Type->GetParameters(), Content);

				Content += ");";
				ADD_NEW_LINE();

				Content += "		}";
				ADD_NEW_LINE();

				Content += "	};";
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

				Content += "(*Arguments)[" + StringUtility::ToString<char8>(index++) + "].Get<" + GetValueTypeType(dataType.GetValueType());

				if (dataType.GetPassType() == DataType::PassesTypes::Pointer)
					Content += "*";

				Content += ">()";

				if (dataType.GetValueType() == ValueTypes::None)
					Content += ")";
			}
		}

		void CompileFileGenerator::GeneratePropertyCode(MetaProperty* Type, String& Content)
		{
			const String IMPLEMENT_PROPERTY_TYPE(STRINGIZE(ImplementPropertyType));
			const String OFFSET_OF(STRINGIZE(OffsetOf));
			const String IMPLEMENT_NAME = GetImplementType(Type);
			const String DATA_TYPE_VARIALBE_NAME = "dataType";

			{
				Content += "	class " + IMPLEMENT_NAME + " : public " + IMPLEMENT_PROPERTY_TYPE;
				ADD_NEW_LINE();
				Content += "	{";
				ADD_NEW_LINE();

				Content += "	public:";
				ADD_NEW_LINE();
				Content += "		" + IMPLEMENT_NAME + "(void) :";
				ADD_NEW_LINE();
				Content += "			" + IMPLEMENT_PROPERTY_TYPE + "(" + GetImplementTypePointerName(Type->GetTopNest()) + ")";
				ADD_NEW_LINE();
				Content += "		{";
				ADD_NEW_LINE();

				Content += "			SetName(\"" + Type->GetName() + "\");";
				ADD_NEW_LINE();

				GenerateDataType(Type->GetDataType(), DATA_TYPE_VARIALBE_NAME, Content);
				Content += "			SetDataType(" + DATA_TYPE_VARIALBE_NAME + ");";
				ADD_NEW_LINE();

				Content += "			SetIsReadOnly(" + StringUtility::ToString<char8>(Type->GetIsReadOnly()).ToLower() + ");";
				ADD_NEW_LINE();

				Content += "		}";
				ADD_NEW_LINE();

				Content += "		AnyDataType GetValue(const void* TargetObject) const override";
				ADD_NEW_LINE();
				Content += "		{";
				ADD_NEW_LINE();

				if (Type->GetIsStatic())
					Content += "			THROW_IF_EXCEPTION(Categories::Reflection, TargetObject != nullptr, \"TargetObject must be null\");";
				else
					Content += "			THROW_IF_EXCEPTION(Categories::Reflection, TargetObject == nullptr, \"TargetObject cannot be null\");";
				ADD_NEW_LINE();

				Content += "			";

				Content += "return ";

				if (Type->GetDataType().GetValueType() == ValueTypes::None)
					Content += "StaticCast(" + String(STRINGIZE(int32)) + ", ";

				if (Type->GetIsStatic())
					Content += Type->GetTopNest()->GetFullQualifiedName() + "::";
				else
					Content += "ReinterpretCast(const " + Type->GetTopNest()->GetFullQualifiedName() + "*, TargetObject)->";

				Content += Type->GetName();

				if (Type->GetDataType().GetValueType() == ValueTypes::None)
					Content += ")";

				Content += ";";
				ADD_NEW_LINE();

				Content += "		}";
				ADD_NEW_LINE();

				Content += "		void SetValue(void* TargetObject, const AnyDataType& Value) override";
				ADD_NEW_LINE();
				Content += "		{";
				ADD_NEW_LINE();

				if (Type->GetIsReadOnly() || Type->GetDataType().GetIsConstValue())
				{
					Content += "			THROW_IF_EXCEPTION(Categories::Reflection, false, \"Property is readonly or const value\");";
				}
				else
				{
					if (Type->GetIsStatic())
						Content += "			THROW_IF_EXCEPTION(Categories::Reflection, TargetObject != nullptr, \"TargetObject must be null\");";
					else
						Content += "			THROW_IF_EXCEPTION(Categories::Reflection, TargetObject == nullptr, \"TargetObject cannot be null\");";
					ADD_NEW_LINE();

					Content += "			typedef decltype(";
					if (Type->GetIsStatic())
						Content += Type->GetTopNest()->GetFullQualifiedName() + "::";
					else
						Content += "ReinterpretCast(" + Type->GetTopNest()->GetFullQualifiedName() + "*, TargetObject)->";
					Content += Type->GetName() + ") ValueType;";
					ADD_NEW_LINE();

					Content += "			";

					if (Type->GetIsStatic())
						Content += Type->GetTopNest()->GetFullQualifiedName() + "::";
					else
						Content += "ReinterpretCast(" + Type->GetTopNest()->GetFullQualifiedName() + "*, TargetObject)->";

					Content += Type->GetName() + " = ";

					if (Type->GetDataType().GetValueType() == ValueTypes::None)
						Content += "StaticCast(";
					else
						Content += "Value.Get<";

					Content += "ValueType";

					if (Type->GetDataType().GetValueType() == ValueTypes::None)
						Content += ", Value.Get<" + String(STRINGIZE(int32)) + ">())";
					else
						Content += ">()";

					Content += ";";
				}
				ADD_NEW_LINE();

				Content += "		}";
				ADD_NEW_LINE();

				Content += "	};";
				ADD_NEW_LINE();
			}

			ADD_NEW_LINE();
		}

		void CompileFileGenerator::GenerateNamespaceHeader(Type* Type, String& Content)
		{
			String ns = String::Empty;
			if (IsTypeOf(Type, MetaObject))
			{
				if (Type->GetTopNest() != nullptr)
					return;

				ns = ReinterpretCast(MetaObject*, Type)->GetNamespace();
			}
			else if (IsTypeOf(Type, EnumType))
			{
				if (Type->GetTopNest() != nullptr)
					return;

				ns = ReinterpretCast(EnumType*, Type)->GetNamespace();
			}

			if (ns == String::Empty)
				return;

			Content += "namespace " + ns;

			ADD_NEW_LINE();

			Content += "{";
			ADD_NEW_LINE();
		}

		void CompileFileGenerator::GenerateNamespaceFooter(Type* Type, String& Content)
		{
			bool shouldProceed = false;

			if (IsTypeOf(Type, MetaObject))
			{
				if (Type->GetTopNest() != nullptr)
					return;

				shouldProceed = true;
			}
			else if (IsTypeOf(Type, EnumType))
			{
				if (Type->GetTopNest() != nullptr)
					return;

				shouldProceed = true;
			}

			if (!shouldProceed)
				return;

			Content += "}";
			ADD_NEW_LINE();

			ADD_NEW_LINE();
		}

#undef ADD_NEW_LINE
	}
}
